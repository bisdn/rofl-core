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
				state(STATE_DISCONNECTED)
{

}



crofchan::crofchan(
		crofchan_env *env,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap) :
				env(env),
				versionbitmap(versionbitmap),
				ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN),
				state(STATE_DISCONNECTED)
{

}



crofchan::~crofchan()
{
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
			rofl::logging::error << "[rofl][chan] unknown event seen, internal error" << std::endl << *this;
		};
		}
	}
}


void
crofchan::event_disconnected()
{
	rofl::logging::debug << "[rofl][chan] event_disconnected -entry point-" << std::endl << *this;

	switch (state) {
	case STATE_DISCONNECTED: {
		// do nothing
	} break;
	case STATE_CONNECT_PENDING:
	case STATE_ESTABLISHED: {
		rofl::logging::debug << "[rofl][chan] event_disconnected -calling env->handle_disconnected()-" << std::endl << *this;

		state = STATE_DISCONNECTED;
		env->handle_disconnected(this);

	} break;
	default: {
		rofl::logging::error << "[rofl][chan] event -DISCONNECTED- invalid state reached, internal error" << std::endl << *this;
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
		rofl::logging::error << "[rofl][chan] event -ESTABLISHED- invalid state reached, internal error" << std::endl << *this;
	};
	}
}


bool
crofchan::is_established() const
{
	if (conns.empty())
		return false;
	// this channel is up, when its main connection is up
	return conns.at(cauxid(0))->is_established();
}



void
crofchan::close()
{
	while (not conns.empty()) {
		std::map<cauxid, crofconn*>::reverse_iterator it = conns.rbegin();
		delete it->second;
		conns.erase(it->first);
	}
}



crofconn&
crofchan::add_conn(
		const cauxid& aux_id,
		enum rofl::csocket::socket_type_t socket_type,
		cparams const& socket_params)
{
	rofl::logging::debug << "[rofl][chan][add-conn] adding connection, aux-id: " << aux_id << std::endl << *this;

	/*
	 * for connecting to peer entity: creates new crofconn instance and calls its connect() method
	 */

	if (conns.empty() && (0 != aux_id.get_id())) {
		rofl::logging::error << "[rofl][chan][add-conn] first connection must have aux-id:0, found " << aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl][chan][add-conn] no auxiliary connections allowed in OFP version: " << (int)ofp_version << std::endl << *this;
		throw eRofChanInval();
	}

	if (conns.find(aux_id) != conns.end()) {
		drop_conn(aux_id); // drop old connection first
	}

	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	if (0 == aux_id.get_id()) {
		vbitmap = versionbitmap;				// main connection: propose all OFP versions defined for our side
	} else {
		vbitmap.add_ofp_version(ofp_version);	// auxiliary connections: use OFP version negotiated for main connection
	}

	(conns[aux_id] = new crofconn(this, vbitmap))->connect(aux_id, socket_type, socket_params);
	return *(conns[aux_id]);
}



crofconn&
crofchan::add_conn(
		const cauxid& aux_id,
		crofconn* conn)
{
	rofl::logging::debug << "[rofl][chan][add-conn] adding connection, aux-id: " << aux_id << std::endl << *this;

	/*
	 * for listening sockets with existing crofconn instance
	 */

	if (conns.empty() && (0 != aux_id.get_id())) {
		rofl::logging::error << "[rofl][chan][add-conn] first connection must have aux-id:0, found " << aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl][chan][add-conn] no auxiliary connections allowed in OFP version: " << ofp_version << std::endl << *this;
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
		const cauxid& aux_id)
{
	rofl::logging::debug << "[rofl][chan][set-conn] retrieving connection, aux-id: " << aux_id << std::endl << *this;

	if (conns.empty() && (0 != aux_id.get_id())) {
		rofl::logging::error << "[rofl][chan][set-conn] first connection must have aux-id:0, found " << aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl][chan][set-conn] no auxiliary connections allowed in OFP version: " << ofp_version << std::endl << *this;
		throw eRofChanInval();
	}

	if (conns.find(aux_id) == conns.end()) {
		rofl::logging::debug << "[rofl][chan][set-conn] adding connection, aux-id: " << aux_id << std::endl << *this;

		rofl::openflow::cofhello_elem_versionbitmap vbitmap;
		if (0 == aux_id.get_id()) {
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
		const cauxid& aux_id) const
{
	rofl::logging::debug << "[rofl][chan][get-conn] retrieving connection, aux-id: " << aux_id << std::endl << *this;

	if (conns.find(aux_id) == conns.end()) {
		throw eRofChanNotFound();
	}
	return *const_cast<crofconn const*>(conns.at(aux_id));
}



void
crofchan::drop_conn(
		const cauxid& aux_id)
{
	rofl::logging::debug << "[rofl][chan][drop-conn] closing connection, aux-id: " << aux_id << std::endl << *this;

	if (conns.find(aux_id) == conns.end()) {
		return;
	}

	// main connection: close main and all auxiliary connections
	if (0 == aux_id.get_id()) {
		rofl::logging::debug << "[rofl][chan][drop-conn] dropping main connection and all auxiliary connections." << std::endl << *this;
		while (not conns.empty()) {
			drop_conn(conns.rbegin()->first);
		}
	} else {
		rofl::logging::debug << "[rofl][chan][drop-conn] dropping connection: " << aux_id << std::endl << *this;
		delete conns[aux_id];
		conns.erase(aux_id);
	}

	if (conns.empty()) {
		run_engine(EVENT_DISCONNECTED);
	}
}



bool
crofchan::has_conn(
		const cauxid& aux_id) const
{
	return (not (conns.find(aux_id) == conns.end()));
}



void
crofchan::handle_connect_refused(
		crofconn *conn)
{
	rofl::logging::warn << "[rofl][chan] OFP transport connection refused." << std::endl << *conn;

	conn->reconnect();
}



void
crofchan::handle_connect_failed(
		crofconn *conn)
{
	rofl::logging::warn << "[rofl][chan] OFP transport connection failed." << std::endl << *conn;

	conn->reconnect();
}



void
crofchan::handle_connected(
		crofconn *conn,
		uint8_t ofp_version)
{
	const cauxid& aux_id = conn->get_aux_id();

	if (0 == aux_id.get_id()) {
		this->ofp_version = ofp_version;
		rofl::logging::info << "[rofl][chan] main connection established. Negotiated OFP version:"
				<< (int) ofp_version << std::endl << *conn;
		run_engine(EVENT_ESTABLISHED);

		for (std::map<cauxid, crofconn*>::iterator
				it = conns.begin(); it != conns.end(); ++it) {
			if (0 == it->first.get_id())
				continue;
			if (it->second->is_actively_established()) {
				it->second->reconnect(true);
			}
		}

	} else {
		if (this->ofp_version != ofp_version) {
			rofl::logging::warn << "[rofl][chan] auxiliary connection with invalid OFP version "
					<< "negotiated, closing connection." << std::endl << *conn;

			drop_conn(conn->get_aux_id());
			return;

		} else {
			rofl::logging::debug << "[rofl][chan] auxiliary connection with aux-id:" << aux_id
					<< " established." << std::endl << *conn;
		}
	}
}



void
crofchan::handle_closed(crofconn *conn)
{
	rofl::logging::debug << "[rofl][chan] OFP connection indicated OFP transport connection closed." << std::endl << *this;

	// do nothing upon reception of this closing notification, when there is no entry in map conns
	if (conns.find(conn->get_aux_id()) == conns.end()) {
		rofl::logging::warn << "[rofl][chan] internal error: unknown connection closed." << std::endl;
		return;
	}

	/*
	 * main connection
	 */
	if (0 == conn->get_aux_id().get_id()) {

		ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN;

		/*
		 * passive connection (=controller) => drop all connections
		 */
		if (not conn->is_actively_established()) {
			rofl::logging::info << "[rofl][chan] passive main connection closed." << std::endl << *this;

			// close all connections
			while (not conns.empty()) {
				const cauxid& aux_id = conns.begin()->first;
				delete conns[aux_id];
				conns.erase(aux_id);
			}

			run_engine(EVENT_DISCONNECTED);

		/*
		 * active connection (=datapath) => close all connections and reconnect them
		 */
		} else {
			rofl::logging::info << "[rofl][chan] active main connection closed." << std::endl;

restart:
			// remove all passive connections (there should be none, though ...)
			for (std::map<cauxid, crofconn*>::iterator
					it = conns.begin(); it != conns.end(); ++it) {
				if (not it->second->is_actively_established()) {
					delete it->second;
					conns.erase(it->first);
					goto restart;
				}
			}

			// closing and reconnecting the active connections
			for (std::map<cauxid, crofconn*>::iterator
					it = conns.begin(); it != conns.end(); ++it) {

				it->second->close();
			}

			// try reconnecting main connection
			conn->reconnect(true);
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

			rofl::logging::info << "[rofl][chan] passive auxiliary connection closed:"
					<< conn->get_aux_id() << std::endl;

			drop_conn(conn->get_aux_id());
			return;

		/*
		 * active connection (=datapath) => reconnect
		 */
		} else {
			rofl::logging::info << "[rofl][chan] active auxiliary connection closed:"
					<< conn->get_aux_id() << std::endl;

			conn->reconnect();
			return;
		}

	}
}



void
crofchan::handle_write(crofconn *conn)
{
	env->handle_write(this, conn->get_aux_id());
}



void
crofchan::recv_message(crofconn *conn, rofl::openflow::cofmsg *msg)
{
	env->recv_message(this, conn->get_aux_id(), msg);
}



unsigned int
crofchan::send_message(
		const cauxid& aux_id,
		rofl::openflow::cofmsg *msg)
{
	if (conns.find(aux_id) == conns.end()) {
		rofl::logging::error << "[rofl][chan] sending message failed for aux-id:" << aux_id << " not found." << std::endl << *this;
		throw eRofChanNotFound(); // throw exception, when this connection does not exist
	}

	if (not conns[aux_id]->is_established()) {
		rofl::logging::error << "[rofl][chan] connection for aux-id:" << aux_id << " not established." << std::endl << *this;
		throw eRofChanNotConnected();
	}

	return conns[aux_id]->send_message(msg);
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



