/*
 * crofchan.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "rofl/common/crofchan.h"

using namespace rofl;

/*static*/std::set<crofchan_env*> crofchan_env::rofchan_envs;



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
	/*
	 * for connecting to peer entity: creates new crofconn instance and calls its connect() method
	 */

	if (conns.empty() && (0 != aux_id.get_id())) {
		rofl::logging::error << "[rofl-common][crofchan][add-conn] first connection must have aux-id:0, found " << aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl-common][crofchan][add-conn] no auxiliary connections allowed in OFP version: " << (int)ofp_version << std::endl << *this;
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

	rofl::logging::debug << "[rofl-common][crofchan] "
			<< "added connection, auxid: " << aux_id.str() << std::endl;

	return *(conns[aux_id]);
}



crofconn&
crofchan::add_conn(
		const cauxid& aux_id,
		crofconn* conn)
{
	/*
	 * for listening sockets with existing crofconn instance
	 */

	if (conns.empty() && (0 != aux_id.get_id())) {
		rofl::logging::error << "[rofl-common][crofchan][add-conn] first connection must have aux-id:0, found " << aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl-common][crofchan][add-conn] no auxiliary connections allowed in OFP version: " << ofp_version << std::endl << *this;
		throw eRofChanInval();
	}

	if (conns.find(aux_id) != conns.end()) {
		drop_conn(aux_id); // drop old connection first
	}

	if (aux_id == rofl::cauxid(0)) {
		this->ofp_version = conn->get_version();
	}

	conns[aux_id] = conn;
	conns[aux_id]->set_env(this);

	rofl::logging::debug << "[rofl-common][crofchan] "
			<< "added connection, aux-id: " << aux_id.str() << " " << str() << std::endl;

	handle_connected(*conns[aux_id], conns[aux_id]->get_version());

	return *(conns[aux_id]);
}



crofconn&
crofchan::set_conn(
		const cauxid& aux_id)
{
	if (conns.empty() && (0 != aux_id.get_id())) {
		rofl::logging::error << "[rofl-common][crofchan][set_conn] first connection must have aux-id:0, found " << aux_id << " instead." << std::endl << *this;
		throw eRofChanInval();
	}

	if ((aux_id.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl-common][crofchan][set_conn] no auxiliary connections allowed in OFP version: " << ofp_version << std::endl << *this;
		throw eRofChanInval();
	}

	if (conns.find(aux_id) == conns.end()) {
		rofl::logging::debug << "[rofl-common][crofchan][set_conn] adding connection, aux-id: " << aux_id << std::endl << *this;

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
	if (conns.find(aux_id) == conns.end()) {
		throw eRofChanNotFound();
	}
	return *const_cast<crofconn const*>(conns.at(aux_id));
}



void
crofchan::drop_conn(
		const cauxid& aux_id)
{
	rofl::logging::debug << "[rofl-common][crofchan][drop_conn] "
			<< "dropping connection, aux-id: " << aux_id.str() << " " << str() << std::endl;

	if (conns.find(aux_id) == conns.end()) {
		return;
	}

	// main connection: close main and all auxiliary connections
	if (0 == aux_id.get_id()) {
		rofl::logging::debug << "[rofl-common][crofchan][drop-conn] dropping main connection and all auxiliary connections." << std::endl << *this;
		delete conns[aux_id];
		conns.erase(aux_id);

		while (not conns.empty()) {
			drop_conn(conns.rbegin()->first);
		}
	} else {
		rofl::logging::debug << "[rofl-common][crofchan][drop-conn] dropping connection: " << aux_id << std::endl << *this;
		delete conns[aux_id];
		conns.erase(aux_id);
	}
}



bool
crofchan::has_conn(
		const cauxid& aux_id) const
{
	return (not (conns.find(aux_id) == conns.end()));
}




unsigned int
crofchan::send_message(
		const cauxid& aux_id,
		rofl::openflow::cofmsg *msg)
{
	if (conns.find(aux_id) == conns.end()) {
		rofl::logging::error << "[rofl-common][crofchan] sending message failed for aux-id:" << aux_id << " not found." << std::endl << *this;
		throw eRofChanNotFound(); // throw exception, when this connection does not exist
	}

	if (not conns[aux_id]->is_established()) {
		rofl::logging::error << "[rofl-common][crofchan] connection for aux-id:" << aux_id << " not established." << std::endl << *this;
		throw eRofChanNotConnected();
	}

	return conns[aux_id]->send_message(msg);
}


void
crofchan::handle_timeout(
		int opaque, void* data)
{
	switch (opaque) {
	case TIMER_RUN_ENGINE: {
		work_on_eventqueue();
	} break;
	default: {
		// ignore unknown timer types
	};
	}
}



void
crofchan::push_on_eventqueue(
		enum crofchan_event_t event)
{
	if (EVENT_NONE != event) {
		events.push_back(event);
	}
	register_timer(TIMER_RUN_ENGINE, rofl::ctimespec(/*second(s)=*/0));
}



void
crofchan::work_on_eventqueue()
{
	while (not events.empty()) {
		enum crofchan_event_t event = events.front();
		events.pop_front();

		switch (event) {
		case EVENT_CONN_ESTABLISHED: {
			event_conn_established();
		} break;
		case EVENT_CONN_TERMINATED: {
			event_conn_terminated();
		} return; // might call this object's destructor
		case EVENT_CONN_REFUSED: {
			event_conn_refused();
		} break;
		case EVENT_CONN_FAILED: {
			event_conn_failed();
		} break;
		default: {
			// ignore yet unknown events
		};
		}
	}
}



void
crofchan::event_conn_established()
{
	rofl::RwLock rwlock(conns_established_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_established.begin(); it != conns_established.end(); ++it) {

		rofl::logging::info << "[rofl-common][crofchan] "
				<< "auxid: " << it->str() << " -conn-established- " << str() << std::endl;

		const cauxid& aux_id = *it;

		if (0 == aux_id.get_id()) {
			this->ofp_version = conns[aux_id]->get_version();

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
				rofl::logging::warn << "[rofl-common][crofchan] auxiliary connection with invalid OFP version "
						<< "negotiated, closing connection. " << conns[aux_id]->str() << std::endl;

				drop_conn(aux_id);
				return;

			} else {
				rofl::logging::debug << "[rofl-common][crofchan] auxiliary connection with auxid:" << aux_id
						<< " established. " << conns[aux_id]->str() << std::endl;
			}
		}

		call_env().handle_conn_established(*this, aux_id);
	}

	conns_established.clear();
}



void
crofchan::event_conn_terminated()
{
	rofl::RwLock rwlock(conns_terminated_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_terminated.begin(); it != conns_terminated.end(); ++it) {

		rofl::logging::info << "[rofl-common][crofchan] "
				<< "auxid: " << it->str() << " -conn-terminated- " << str() << std::endl;

		// do nothing upon reception of this closing notification, when there is no entry in map conns
		if (conns.find(*it) == conns.end()) {
			rofl::logging::warn << "[rofl-common][crofchan] internal error: unknown connection closed." << std::endl;
			continue;
		}

		crofconn& conn = *(conns[*it]);

		/*
		 * main connection
		 */
		if (0 == conn.get_aux_id().get_id()) {

			ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN;

			/*
			 * passive connection (=controller) => drop all connections
			 */
			if (not conn.is_actively_established()) {
				rofl::logging::info << "[rofl-common][crofchan] passive main connection closed. " << str() << std::endl;

				// close all connections
				while (not conns.empty()) {
					const cauxid& aux_id = conns.begin()->first;
					delete conns[aux_id];
					conns.erase(aux_id);
				}

			/*
			 * active connection (=datapath) => close all connections and reconnect them
			 */
			} else {
				rofl::logging::info << "[rofl-common][crofchan] active main connection closed." << std::endl;

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

				call_env().handle_conn_terminated(*this, conn.get_aux_id());

				// try reconnecting main connection
				conn.reconnect(false);
			}

		/*
		 * auxiliary connection
		 */
		} else {

			/*
			 * passive connection
			 */
			if (not conn.is_actively_established()) {

				rofl::logging::info << "[rofl-common][crofchan] passive auxiliary connection closed:"
						<< conn.get_aux_id() << std::endl;

				drop_conn(conn.get_aux_id());
				return;

			/*
			 * active connection (=datapath) => reconnect
			 */
			} else {
				rofl::logging::info << "[rofl-common][crofchan] active auxiliary connection closed:"
						<< conn.get_aux_id() << std::endl;

				conn.reconnect();
				return;
			}
		}

		call_env().handle_conn_terminated(*this, conn.get_aux_id());
	}

	conns_terminated.clear();
}



void
crofchan::event_conn_refused()
{
	rofl::RwLock rwlock(conns_refused_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_refused.begin(); it != conns_refused.end(); ++it) {

		const cauxid& aux_id = *it;

		if (conns.find(aux_id) == conns.end()) {
			continue;
		}

		rofl::logging::info << "[rofl-common][crofchan] "
				<< "auxid: " << it->str() << " -conn-refused- " << str() << std::endl;

		call_env().handle_conn_refused(*this, aux_id);

		conns[aux_id]->reconnect();
	}

	conns_refused.clear();
}



void
crofchan::event_conn_failed()
{
	rofl::RwLock rwlock(conns_failed_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_failed.begin(); it != conns_failed.end(); ++it) {

		const cauxid& aux_id = *it;

		if (conns.find(aux_id) == conns.end()) {
			continue;
		}

		rofl::logging::info << "[rofl-common][crofchan] "
				<< "auxid: " << it->str() << " -conn-failed- " << str() << std::endl;

		call_env().handle_conn_failed(*this, aux_id);

		conns[aux_id]->reconnect();
	}

	conns_failed.clear();
}


