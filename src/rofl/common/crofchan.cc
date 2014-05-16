/*
 * crofchan.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "rofl/common/crofchan.h"

using namespace rofl;


crofchan::crofchan() :
				env(NULL),
				ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN),
				state(STATE_DISCONNECTED),
				reconnect_start_timeout(CROFCHAN_RECONNECT_START_TIMEOUT),
				reconnect_in_seconds(CROFCHAN_RECONNECT_START_TIMEOUT),
				reconnect_variance(CROFCHAN_RECONNECT_VARIANCE_IN_SECS),
				reconnect_counter(0),
				reconnect_timer_id()
{

}



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
				reconnect_timer_id()
{

}



crofchan::~crofchan()
{
	logging::debug << "[rofl][chan] destructor:" << std::endl << *this;
	close();
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
crofchan::close()
{
	while (not conns.empty()) {
		std::map<uint8_t, crofconn*>::reverse_iterator it = conns.rbegin();
		delete it->second;
		conns.erase(it->first);
	}
}



crofconn&
crofchan::add_conn(
		uint8_t aux_id,
		enum rofl::csocket::socket_type_t socket_type,
		cparams const& socket_params)
{
	rofl::logging::debug << "[rofl][chan][add-conn] adding connection, aux-id: " << (int)aux_id << std::endl << *this;

	/*
	 * for connecting to peer entity: creates new crofconn instance and calls its connect() method
	 */

	if (conns.empty() && (0 != aux_id)) {
		logging::error << "[rofl][chan][add-conn] first connection must have aux-id:0, found " << (int)aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		logging::error << "[rofl][chan][add-conn] no auxiliary connections allowed in OFP version: " << (int)ofp_version << std::endl << *this;
		throw eRofChanInval();
	}

	if (conns.find(aux_id) != conns.end()) {
		drop_conn(aux_id); // drop old connection first
	}

	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	if (0 == aux_id) {
		vbitmap = versionbitmap;				// main connection: propose all OFP versions defined for our side
	} else {
		vbitmap.add_ofp_version(ofp_version);	// auxiliary connections: use OFP version negotiated for main connection
	}

	(conns[aux_id] = new crofconn(this, vbitmap))->connect(aux_id, socket_type, socket_params);
	return *(conns[aux_id]);
}



crofconn&
crofchan::add_conn(
		uint8_t aux_id,
		crofconn* conn)
{
	rofl::logging::debug << "[rofl][chan][add-conn] adding connection, aux-id: " << (int)aux_id << std::endl << *this;

	/*
	 * for listening sockets with existing crofconn instance
	 */

	if (conns.empty() && (0 != aux_id)) {
		logging::error << "[rofl][chan][add-conn] first connection must have aux-id:0, found " << (int)aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		logging::error << "[rofl][chan][add-conn] no auxiliary connections allowed in OFP version: " << ofp_version << std::endl << *this;
		throw eRofChanInval();
	}

	if (conns.find(aux_id) != conns.end()) {
		drop_conn(aux_id); // drop old connection first
	}

	conns[aux_id] = conn;
	conns[aux_id]->set_env(this);

	handle_connected(conns[aux_id], conns[aux_id]->get_version());

	return *(conns[aux_id]);
}



crofconn&
crofchan::set_conn(
		uint8_t aux_id)
{
	rofl::logging::debug << "[rofl][chan][set-conn] retrieving connection, aux-id: " << (int)aux_id << std::endl << *this;

	if (conns.empty() && (0 != aux_id)) {
		logging::error << "[rofl][chan][set-conn] first connection must have aux-id:0, found " << (int)aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		logging::error << "[rofl][chan][set-conn] no auxiliary connections allowed in OFP version: " << ofp_version << std::endl << *this;
		throw eRofChanInval();
	}

	if (conns.find(aux_id) == conns.end()) {
		rofl::logging::debug << "[rofl][chan][set-conn] adding connection, aux-id: " << (int)aux_id << std::endl << *this;

		rofl::openflow::cofhello_elem_versionbitmap vbitmap;
		if (0 == aux_id) {
			vbitmap = versionbitmap;				// main connection: propose all OFP versions defined for our side
		} else {
			vbitmap.add_ofp_version(ofp_version);	// auxiliary connections: use OFP version negotiated for main connection
		}
		conns[aux_id] = new crofconn(this, vbitmap);
	}
	return *(conns[aux_id]);
}



crofconn const&
crofchan::get_conn(
		uint8_t aux_id) const
{
	rofl::logging::debug << "[rofl][chan][get-conn] retrieving connection, aux-id: " << (int)aux_id << std::endl << *this;

	if (conns.find(aux_id) == conns.end()) {
		throw eRofChanNotFound();
	}
	return *const_cast<crofconn const*>(conns.at(aux_id));
}



void
crofchan::drop_conn(
		uint8_t aux_id)
{
	rofl::logging::debug << "[rofl][chan][drop-conn] closing connection, aux-id: " << (int)aux_id << std::endl << *this;

	if (conns.find(aux_id) == conns.end()) {
		return;
	}

	// main connection: close main and all auxiliary connections
	if (0 == aux_id) {
		rofl::logging::debug << "[rofl][chan][drop-conn] dropping main connection and all auxiliary connections." << std::endl << *this;
		while (not conns.empty()) {
			drop_conn(conns.rbegin()->first);
		}
	} else {
		rofl::logging::debug << "[rofl][chan][drop-conn] dropping connection: " << (int)aux_id << std::endl << *this;
		delete conns[aux_id];
		conns.erase(aux_id);
	}

	if (conns.empty()) {
		run_engine(EVENT_DISCONNECTED);
	}
}



bool
crofchan::has_conn(
		uint8_t aux_id) const
{
	return (not (conns.find(aux_id) == conns.end()));
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
crofchan::handle_connect_failed(
		crofconn *conn)
{
	logging::warn << "[rofl][chan] connection failed." << std::endl << *conn;

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

	flags.reset(FLAG_RECONNECTING); // FIXME: take aux_id into account

	if (0 == aux_id) {
		this->ofp_version = ofp_version;
		logging::info << "[rofl][chan] main connection established. Negotiated OFP version:"
				<< (int) ofp_version << std::endl << *conn;
		run_engine(EVENT_ESTABLISHED);
		cancel_timer(reconnect_timer_id);

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

			run_engine(EVENT_DISCONNECTED);

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
	 * auxiliary connection
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
		throw eRofChanNotFound(); // throw exception, when this connection does not exist
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
	int max_backoff = 16 * reconnect_start_timeout;

	if ((not flags.test(FLAG_RECONNECTING)) || (reset_timeout)) {

		try {
			cancel_timer(reconnect_timer_id);
		} catch (eTimersNotFound& e) {}

		reconnect_in_seconds = reconnect_start_timeout + reconnect_variance * crandom::draw_random_number();
		reconnect_counter = 0;

	} else {
		reconnect_in_seconds *= 2;

		if (reconnect_in_seconds > max_backoff) {
			reconnect_in_seconds = max_backoff;
		}
	}

	logging::info << "[rofl][chan] " << " scheduled reconnect in "
			<< (int)reconnect_in_seconds << " seconds." << std::endl << *this;


	reconnect_timer_id = register_timer(TIMER_RECONNECT, reconnect_in_seconds);

	++reconnect_counter;

	flags.set(FLAG_RECONNECTING); // FIXME: take aux_id into account
}

