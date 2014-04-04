/*
 * crofchan.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "rofl/common/crofchan.h"

using namespace rofl;


crofchan::crofchan(
		crofchan_env *env,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap) :
				env(env),
				versionbitmap(versionbitmap),
				ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN),
				state(STATE_DISCONNECTED),
				reconnect_start_timeout(CROFCHAN_RECONNECT_START_TIMEOUT),
				reconnect_in_seconds(CROFCHAN_RECONNECT_START_TIMEOUT),
				reconnect_variance(CROFCHAN_RECONNECT_VARIANCE_IN_SECS),
				reconnect_counter(0),
				reconnect_timer_id(0)
{

}



crofchan::~crofchan()
{
	logging::debug << "[rofl][chan] destructor:" << std::endl << *this;
	clear();
}



void
crofchan::run_engine(enum crofchan_event_t event)
{
	if (EVENT_NONE != event) {
		events.push_back(event);
	}

	while (not events.empty()) {
		enum crofchan_event_t event = events.front();
		events.pop_front();

		switch (event) {
		case EVENT_DISCONNECTED:	event_disconnected();	return; // might call this object's destructor
		case EVENT_ESTABLISHED: 	event_established(); 	break;
		default: {
			logging::error << "[rofl][chan] unknown event seen, internal error" << std::endl << *this;
		};
		}
	}
}


void
crofchan::event_disconnected()
{
	logging::debug << "[rofl][chan] event_disconnected -entry point-" << std::endl << *this;

	switch (state) {
	case STATE_DISCONNECTED: {
		// do nothing
	} break;
	case STATE_CONNECT_PENDING:
	case STATE_ESTABLISHED: {
		logging::debug << "[rofl][chan] event_disconnected -calling env->handle_disconnected()-" << std::endl << *this;

		state = STATE_DISCONNECTED;
		env->handle_disconnected(this);

	} break;
	default: {
		logging::error << "[rofl][chan] event -DISCONNECTED- invalid state reached, internal error" << std::endl << *this;
	};
	}
}


void
crofchan::event_established()
{
	switch (state) {
	case STATE_DISCONNECTED:
	case STATE_CONNECT_PENDING:
	case STATE_ESTABLISHED: {
		state = STATE_ESTABLISHED;
		env->handle_established(this);

	} break;
	default: {
		logging::error << "[rofl][chan] event -ESTABLISHED- invalid state reached, internal error" << std::endl << *this;
	};
	}
}


bool
crofchan::is_established() const
{
	if (conns.empty())
		return false;
	// this channel is up, when its main connection is up
	return conns.at(0)->is_established();
}



void
crofchan::clear()
{
	while (not conns.empty()) {
		std::map<uint8_t, crofconn*>::reverse_iterator it = conns.rbegin();
		crofconn *conn = it->second;
		conns.erase(it->first);
		delete conn;
	}
}



void
crofchan::add_conn(
		uint8_t aux_id,
		int reconnect_start_timeout,
		enum rofl::csocket::socket_type_t socket_type,
		cparams const& socket_params)
{
	this->reconnect_start_timeout = reconnect_start_timeout;

	if (conns.find(aux_id) != conns.end()) {
		throw eRofChanExists();
	}
	if (conns.empty() && (0 != aux_id)) {
		logging::error << "[rofl][chan] first connection must have aux-id:0, found " << (int)aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		logging::error << "[rofl][chan] no auxiliary connections allowed in OFP version: " << ofp_version << std::endl << *this;
		throw eRofChanInval();
	}

	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	if (0 == aux_id) {
		vbitmap = versionbitmap;				// main connection: propose all OFP versions defined for our side
	} else {
		vbitmap.add_ofp_version(ofp_version);	// auxiliary connections: use OFP version negotiated for main connection
	}

	(conns[aux_id] = new crofconn(this, vbitmap))->connect(aux_id, socket_type, socket_params);
}



void
crofchan::add_conn(
		crofconn* conn,
		uint8_t aux_id)
{
	if (conns.empty() && (0 != aux_id)) {
		logging::error << "[rofl][chan] first connection must have aux-id:0, found " << (int)aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		logging::error << "[rofl][chan] no auxiliary connections allowed in OFP version: " << ofp_version << std::endl << *this;
		throw eRofChanInval();
	}



	if (conns.find(aux_id) != conns.end()) {
		drop_conn(aux_id); // drop old connection first
	}
	conns[aux_id] = conn;
	conns[aux_id]->set_env(this);
	handle_connected(conns[aux_id], conns[aux_id]->get_version());
}



crofconn&
crofchan::get_conn(
		uint8_t aux_id)
{
	if (conns.find(aux_id) == conns.end()) {
		throw eRofChanNotFound();
	}
	return *(conns[aux_id]);
}



void
crofchan::drop_conn(
		uint8_t aux_id)
{
	if ((0 != aux_id) && (conns.size() > 1)) {
		logging::warn << "[rofl][chan] dropping main connection, while still auxiliary connections active." << std::endl << *this;
		while (conns.size() > 1) {
			std::map<uint8_t, crofconn*>::reverse_iterator it = conns.rbegin();
			if (0 == it->first)
				continue;
			else
				drop_conn(it->first);
		}
	}

	if (conns.find(aux_id) == conns.end()) {
		return;
	}
	crofconn *conn = conns[aux_id]; // temporary pointer
	conns.erase(aux_id); // remove pointer from map conns
	delete conn; // call destructor => this will lead to a call to crofchan::handle_closed(conn);

	if (conns.empty()) {
		run_engine(EVENT_DISCONNECTED);
	}
}



void
crofchan::handle_connect_refused(
		crofconn *conn)
{
	logging::warn << "[rofl][chan] connection refused." << std::endl << *conn;

	uint8_t aux_id = conn->get_aux_id();

	if (0 == aux_id) {
		backoff_reconnect(false);
	} else {
		conn->reconnect();
	}
}



void
crofchan::handle_connected(
		crofconn *conn,
		uint8_t ofp_version)
{
	uint8_t aux_id = conn->get_aux_id();

	if (0 == aux_id) {
		this->ofp_version = ofp_version;
		logging::info << "[rofl][chan] main connection established. Negotiated OFP version:"
				<< (int) ofp_version << std::endl << *conn;
		run_engine(EVENT_ESTABLISHED);
		cancel_timer(TIMER_RECONNECT);

		for (std::map<uint8_t, crofconn*>::iterator
				it = conns.begin(); it != conns.end(); ++it) {
			if (0 == it->first)
				continue;
			if (it->second->is_actively_established()) {
				it->second->reconnect();
			}
		}

	} else {
		if (this->ofp_version != ofp_version) {
			logging::warn << "[rofl][chan] auxiliary connection with invalid OFP version "
					<< "negotiated, closing connection." << std::endl << *conn;

			delete conn;
			conns.erase(aux_id);

		} else {
			logging::debug << "[rofl][chan] auxiliary connection with aux-id:" << (int)aux_id
					<< " established." << std::endl << *conn;
		}
	}
}



void
crofchan::handle_closed(crofconn *conn)
{
	logging::debug << "[rofl][chan] handle_closed" << std::endl << *this;

	// do nothing upon reception of this closing notification, when there is no entry in map conns
	if (conns.find(conn->get_aux_id()) == conns.end()) {
		logging::warn << "[rofl][chan] internal error: unknown connection closed." << std::endl;
		return;
	}

	/*
	 * main connection
	 */
	if (0 == conn->get_aux_id()) {

		ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN;

		/*
		 * passive connection (=controller) => drop all connections
		 */
		if (not conn->is_actively_established()) {
			logging::info << "[rofl][chan] passive main connection closed." << std::endl << *this;

			// close all connections
			while (not conns.empty()) {
				uint8_t aux_id = conns.begin()->first;
				delete conns[aux_id];
				conns.erase(aux_id);
			}

		/*
		 * active connection (=datapath) => close all connections and reconnect them
		 */
		} else {
			logging::info << "[rofl][chan] active main connection closed." << std::endl;

restart:
			// remove all passive connections (there should be none, though ...)
			for (std::map<uint8_t, crofconn*>::iterator
					it = conns.begin(); it != conns.end(); ++it) {
				if (not it->second->is_actively_established()) {
					delete it->second;
					conns.erase(it->first);
					goto restart;
				}
			}

			// closing and reconnecting the active connections
			for (std::map<uint8_t, crofconn*>::iterator
					it = conns.begin(); it != conns.end(); ++it) {

				it->second->close();
			}

			// try reconnecting main connection
			backoff_reconnect(true);
		}

		run_engine(EVENT_DISCONNECTED);

		return;


	/*
	 * auxialiary connection
	 */
	} else {

		/*
		 * passive connection
		 */
		if (not conn->is_actively_established()) {

			logging::info << "[rofl][chan] passive auxiliary connection closed:"
					<< (int)conn->get_aux_id() << std::endl;

			conns.erase(conn->get_aux_id());
			delete conn;
			return;

		/*
		 * active connection (=datapath) => reconnect
		 */
		} else {
			logging::info << "[rofl][chan] active auxiliary connection closed:"
					<< (int)conn->get_aux_id() << std::endl;

			conn->reconnect();
			return;
		}

	}
}



void
crofchan::recv_message(crofconn *conn, rofl::openflow::cofmsg *msg)
{
	env->recv_message(this, conn->get_aux_id(), msg);
}



void
crofchan::send_message(
		rofl::openflow::cofmsg *msg,
		uint8_t aux_id)
{
	if (conns.find(aux_id) == conns.end()) {
		throw eRofChanNotFound();
	}
	conns[aux_id]->send_message(msg);
}



uint32_t
crofchan::get_async_xid(crofconn *conn)
{
	return env->get_async_xid(this);
}

uint32_t
crofchan::get_sync_xid(crofconn *conn, uint8_t msg_type, uint16_t msg_sub_type)
{
	return env->get_sync_xid(this, msg_type, msg_sub_type);
}

void
crofchan::release_sync_xid(crofconn *conn, uint32_t xid)
{
	env->release_sync_xid(this, xid);
}



void
crofchan::handle_timeout(int opaque, void *data)
{
	switch (opaque) {
	case TIMER_RECONNECT: {
		if (conns.find(0) != conns.end()) {
			conns[0]->reconnect();
		}
	} break;
	default:
		logging::warn << "[rofl][chan] unknown timer event" << std::endl;
	}
}



void
crofchan::backoff_reconnect(bool reset_timeout)
{
	logging::info << "[rofl][chan] " << " scheduled reconnect in "
			<< (int)reconnect_in_seconds << " seconds." << std::endl << *this;

	int max_backoff = 16 * reconnect_start_timeout;

	if ((0 == reconnect_timer_id) || (reset_timeout)) {

		reconnect_in_seconds = reconnect_start_timeout + reconnect_variance * crandom::draw_random_number();
		reconnect_counter = 0;

		if ((0 != reconnect_timer_id) && reset_timeout) {
			cancel_timer(reconnect_timer_id);
		}

	} else {
		reconnect_in_seconds *= 2;

		if (reconnect_in_seconds > max_backoff) {
			reconnect_in_seconds = max_backoff;
		}

	}

	reconnect_timer_id = register_timer(TIMER_RECONNECT, reconnect_in_seconds);

	++reconnect_counter;
}
