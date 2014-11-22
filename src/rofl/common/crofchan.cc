/*
 * crofchan.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "rofl/common/crofchan.h"

using namespace rofl;


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
			rofl::logging::error << "[rofl-common][channel] unknown event seen, internal error" << std::endl << *this;
		};
		}
	}
}


void
crofchan::event_disconnected()
{

	switch (state) {
	case STATE_DISCONNECTED: {
		rofl::logging::debug << "[rofl-common][channel] channel is in state -disconnected-" << std::endl;
		// do nothing
	} break;
	case STATE_CONNECT_PENDING:
	case STATE_ESTABLISHED: {
		rofl::logging::info << "[rofl-common][channel] channel entering state -disconnected-" << std::endl;
		state = STATE_DISCONNECTED;
		env->handle_disconnected(this);
	} break;
	default: {
		rofl::logging::error << "[rofl-common][channel] event -DISCONNECTED- in invalid state reached, internal error" << std::endl;
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
		rofl::logging::info << "[rofl-common][channel] entering state -established-" << std::endl;
		state = STATE_ESTABLISHED;
		env->handle_established(this);
	} break;
	default: {
		rofl::logging::error << "[rofl-common][channel] event -ESTABLISHED- in invalid state reached, internal error" << std::endl << *this;
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



cauxid
crofchan::get_next_auxid()
{
	uint8_t aux_id = 0;
	while (conns.find(aux_id) != conns.end()) {
		aux_id++;
		if (aux_id == 255) {
			throw eAuxIdNotFound("crofchan::get_next_cauxid() auxid namespace exhausted");
		}
	}
	return cauxid(aux_id);
}



std::list<cauxid>
crofchan::get_conn_index() const
{
	std::list<cauxid> connections;
	for (std::map<cauxid, crofconn*>::const_iterator
			it = conns.begin(); it != conns.end(); ++it) {
		connections.push_back(it->first);
	}
	return connections;
}



crofconn&
crofchan::add_conn(
		const cauxid& aux_id,
		enum rofl::csocket::socket_type_t socket_type,
		cparams const& socket_params)
{
	rofl::logging::debug << "[rofl-common][channel][add-conn] adding connection, aux-id: " << (int)aux_id.get_id() << std::endl << *this;

	/*
	 * for connecting to peer entity: creates new crofconn instance and calls its connect() method
	 */

	if (conns.empty() && (0 != aux_id.get_id())) {
		rofl::logging::error << "[rofl-common][channel][add-conn] first connection must have aux-id:0, found " << aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl-common][channel][add-conn] no auxiliary connections allowed in OFP version: " << (int)ofp_version << std::endl << *this;
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
	rofl::logging::debug << "[rofl-common][channel][add-conn] adding connection, aux-id: " << aux_id << std::endl << *this;

	/*
	 * for listening sockets with existing crofconn instance
	 */

	if (conns.empty() && (0 != aux_id.get_id())) {
		rofl::logging::error << "[rofl-common][channel][add-conn] first connection must have aux-id:0, found " << aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl-common][channel][add-conn] no auxiliary connections allowed in OFP version: " << ofp_version << std::endl << *this;
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
	rofl::logging::debug << "[rofl-common][channel][set-conn] retrieving connection, aux-id: " << aux_id << std::endl << *this;

	if (conns.empty() && (0 != aux_id.get_id())) {
		rofl::logging::error << "[rofl-common][channel][set-conn] first connection must have aux-id:0, found " << aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl-common][channel][set-conn] no auxiliary connections allowed in OFP version: " << ofp_version << std::endl << *this;
		throw eRofChanInval();
	}

	if (conns.find(aux_id) == conns.end()) {
		rofl::logging::debug << "[rofl-common][channel][set-conn] adding connection, aux-id: " << aux_id << std::endl << *this;

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
	rofl::logging::debug << "[rofl-common][channel][get-conn] retrieving connection, aux-id: " << aux_id << std::endl << *this;

	if (conns.find(aux_id) == conns.end()) {
		throw eRofChanNotFound();
	}
	return *const_cast<crofconn const*>(conns.at(aux_id));
}



void
crofchan::drop_conn(
		const cauxid& aux_id)
{
	rofl::logging::debug << "[rofl-common][channel][drop-conn] closing connection, aux-id: " << aux_id << std::endl << *this;

	if (conns.find(aux_id) == conns.end()) {
		return;
	}

	// main connection: close main and all auxiliary connections
	if (0 == aux_id.get_id()) {
		rofl::logging::debug << "[rofl-common][channel][drop-conn] dropping main connection and all auxiliary connections." << std::endl << *this;
		while (not conns.empty()) {
			drop_conn(conns.rbegin()->first);
		}
	} else {
		rofl::logging::debug << "[rofl-common][channel][drop-conn] dropping connection: " << aux_id << std::endl << *this;
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
crofchan::handle_connected(
		crofconn *conn,
		uint8_t ofp_version)
{
	const cauxid& aux_id = conn->get_aux_id();

	if (0 == aux_id.get_id()) {
		this->ofp_version = ofp_version;
		rofl::logging::info << "[rofl-common][channel] main connection established. Negotiated OFP version:"
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
			rofl::logging::warn << "[rofl-common][channel] auxiliary connection with invalid OFP version "
					<< "negotiated, closing connection." << std::endl << *conn;

			drop_conn(conn->get_aux_id());
			return;

		} else {
			rofl::logging::debug << "[rofl-common][channel] auxiliary connection with aux-id:" << aux_id
					<< " established." << std::endl << *conn;
		}
	}
}



void
crofchan::handle_closed(crofconn *conn)
{
	rofl::logging::debug << "[rofl-common][channel] OFP connection indicated OFP transport connection closed." << std::endl << *this;

	// do nothing upon reception of this closing notification, when there is no entry in map conns
	if (conns.find(conn->get_aux_id()) == conns.end()) {
		rofl::logging::warn << "[rofl-common][channel] internal error: unknown connection closed." << std::endl;
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
			rofl::logging::info << "[rofl-common][channel] passive main connection closed." << std::endl << *this;

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
			rofl::logging::info << "[rofl-common][channel] active main connection closed." << std::endl;

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

			run_engine(EVENT_DISCONNECTED);

			// try reconnecting main connection
			conn->reconnect(true);
		}

		return;


	/*
	 * auxiliary connection
	 */
	} else {

		/*
		 * passive connection
		 */
		if (not conn->is_actively_established()) {

			rofl::logging::info << "[rofl-common][channel] passive auxiliary connection closed:"
					<< conn->get_aux_id() << std::endl;

			drop_conn(conn->get_aux_id());
			return;

		/*
		 * active connection (=datapath) => reconnect
		 */
		} else {
			rofl::logging::info << "[rofl-common][channel] active auxiliary connection closed:"
					<< conn->get_aux_id() << std::endl;

			conn->reconnect();
			return;
		}

	}
}



unsigned int
crofchan::send_message(
		const cauxid& aux_id,
		rofl::openflow::cofmsg *msg)
{
	if (conns.find(aux_id) == conns.end()) {
		rofl::logging::error << "[rofl-common][channel] sending message failed for aux-id:" << aux_id << " not found." << std::endl << *this;
		throw eRofChanNotFound(); // throw exception, when this connection does not exist
	}

	if (not conns[aux_id]->is_established()) {
		rofl::logging::error << "[rofl-common][channel] connection for aux-id:" << aux_id << " not established." << std::endl << *this;
		throw eRofChanNotConnected();
	}

	return conns[aux_id]->send_message(msg);
}




