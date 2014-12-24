/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */



#include "crofbase.h"

using namespace rofl;

/* static */ std::set<crofbase*> crofbase::rofbases;

crofbase::crofbase(
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap) :
				versionbitmap(versionbitmap),
				transactions(this),
				generation_is_defined(false),
				cached_generation_id((uint64_t)((int64_t)-1))
{
	crofbase::rofbases.insert(this);
}



crofbase::~crofbase()
{
	crofbase::rofbases.erase(this);

	rpc_close_all();
}



void
crofbase::rpc_close_all()
{
	try {
		// close the listening sockets
		for (std::set<csocket*>::iterator
				it = dpt_sockets.begin(); it != dpt_sockets.end(); ++it) {
			delete (*it);
		}
		dpt_sockets.clear();

		for (std::set<csocket*>::iterator
				it = ctl_sockets.begin(); it != ctl_sockets.end(); ++it) {
			delete (*it);
		}
		ctl_sockets.clear();

		// detach from higher layer entities
		while (not rofctls.empty()) {
			drop_ctl(rofctls.begin()->first);
		}

		while (not rofdpts.empty()) {
			drop_dpt(rofdpts.begin()->first);
		}

	} catch (RoflException& e) {
		rofl::logging::error << "[rofl-common][crofbase][rpc_close_all] exception:" << e << std::endl;
	}
}



void
crofbase::handle_connect_refused(
		crofconn& conn)
{
	rofl::logging::info << "[rofl-common][crofbase] connection refused: " << conn.str() << std::endl;
}



void
crofbase::handle_connect_failed(
		crofconn& conn)
{
	rofl::logging::info << "[rofl-common][crofbase] connection failed: " << conn.str() << std::endl;
}



void
crofbase::handle_connected(
		crofconn& conn,
		uint8_t ofp_version)
{
	/*
	 * situation:
	 * 1. csocket accepted new connection
	 * 2. crofconn was created and socket descriptor handed over
	 * 3. crofconn conducts HELLO exchange and FEATURES.request/reply => learn dpid and aux-id
	 * 4. this method is called
	 *
	 * next step: check for existing crofdpt instance for dpid seen by crofconn
	 * if none exists, create new one, otherwise, add connection to existing crofdpt
	 */

	switch (conn.get_flavour()) {
	case rofl::crofconn::FLAVOUR_CTL: {
		rofl::logging::info << "[rofl-common][crofbase] "
				<< "creating new crofctl instance for ctl peer" << std::endl;
		add_ctl(get_idle_ctlid(), conn.get_versionbitmap(), /*remove_upon_channel_termination=*/true).add_connection(&conn);
	} break;
	case rofl::crofconn::FLAVOUR_DPT: try {
		crofdpt::get_dpt(conn.get_dpid()).add_connection(&conn);
	} catch (eRofDptNotFound& e) {
		rofl::logging::info << "[rofl-common][crofbase] "
				<< "creating new crofdpt instance for dpt peer, dpid:" << conn.get_dpid() << std::endl;
		add_dpt(get_idle_dptid(), conn.get_versionbitmap(), /*remove_upon_channel_termination=*/true).add_connection(&conn);
	} break;
	default: {

	};
	}
}



void
crofbase::rpc_listen_for_dpts(
		enum rofl::csocket::socket_type_t socket_type,
		const cparams& params)
{
	(*(dpt_sockets.insert(csocket::csocket_factory(socket_type, this)).first))->listen(params);
}




void
crofbase::rpc_listen_for_ctls(
		enum rofl::csocket::socket_type_t socket_type,
		const cparams& params)
{
	(*(ctl_sockets.insert(csocket::csocket_factory(socket_type, this)).first))->listen(params);
}



void
crofbase::handle_listen(
		csocket& socket, int newsd)
{
	if (ctl_sockets.find(&socket) != ctl_sockets.end()) {
		rofl::logging::debug << "[rofl-common][crofbase] "
				<< "accept => creating new crofconn for ctl peer on sd: " << newsd << std::endl;
		(new rofl::crofconn(this, versionbitmap))->accept(
				socket.get_socket_type(), socket.get_socket_params(), newsd, rofl::crofconn::FLAVOUR_CTL);
	} else
	if (dpt_sockets.find(&socket) != dpt_sockets.end()) {
		rofl::logging::debug << "[rofl-common][crofbase] "
						<< "accept => creating new crofconn for dpt peer on sd: " << newsd << std::endl;
		(new rofl::crofconn(this, versionbitmap))->accept(
				socket.get_socket_type(), socket.get_socket_params(), newsd, rofl::crofconn::FLAVOUR_DPT);
	}
}



void
crofbase::handle_closed(
		csocket& socket)
{
	if (ctl_sockets.find(&socket) != ctl_sockets.end()) {
		ctl_sockets.erase(&socket); // FIXME: remove socket from heap?
	} else
	if (dpt_sockets.find(&socket) != dpt_sockets.end()) {
		dpt_sockets.erase(&socket); // FIXME: remove socket from heap?
	}
}



void
crofbase::role_request_rcvd(
		crofctl& ctl,
		uint32_t role,
		uint64_t rcvd_generation_id)
{
	if (generation_is_defined &&
			(rofl::openflow::cofrole::distance((int64_t)rcvd_generation_id, (int64_t)cached_generation_id) < 0)) {
		if ((rofl::openflow13::OFPCR_ROLE_MASTER == role) || (rofl::openflow13::OFPCR_ROLE_SLAVE == role)) {
			throw eRoleRequestStale();
		}
	} else {
		cached_generation_id = rcvd_generation_id;
		generation_is_defined = true;
	}

	// in either case: send current generation_id value back to controller
	ctl.set_role().set_generation_id(cached_generation_id);

	switch (role) {
	case rofl::openflow13::OFPCR_ROLE_MASTER: {

		// iterate over all attached controllers and check for an existing master
		for (std::map<cctlid, crofctl*>::iterator
				it = rofctls.begin(); it != rofctls.end(); ++it) {

			// ignore ctl who called this method
			if (it->second->get_ctlid() == ctl.get_ctlid())
				continue;

			// find any other controller and set them back to role SLAVE
			if (rofl::openflow13::OFPCR_ROLE_MASTER == it->second->get_role().get_role()) {
				it->second->set_role().set_role(rofl::openflow13::OFPCR_ROLE_SLAVE);
			}
		}

		// set new master async-config to template retrieved from of-config (or default one)
		ctl.set_async_config() = ctl.get_async_config_role_default_template();

		ctl.set_role().set_role(rofl::openflow13::OFPCR_ROLE_MASTER);

	} break;
	case rofl::openflow13::OFPCR_ROLE_SLAVE: {

		ctl.set_async_config() = ctl.get_async_config_role_default_template();
		ctl.set_role().set_role(rofl::openflow13::OFPCR_ROLE_SLAVE);

	} break;
	case rofl::openflow13::OFPCR_ROLE_EQUAL: {

		ctl.set_async_config() = ctl.get_async_config_role_default_template();
		ctl.set_role().set_role(rofl::openflow13::OFPCR_ROLE_EQUAL);

	} break;
	case rofl::openflow13::OFPCR_ROLE_NOCHANGE:
	default: {
		// let crofctl_impl send a role-reply with the controller's unaltered current role
	}
	}
}



uint32_t
crofbase::get_ofp_no_buffer(uint8_t ofp_version)
{
	switch (ofp_version) {
	case openflow10::OFP_VERSION: return openflow10::OFP_NO_BUFFER;
	case openflow12::OFP_VERSION: return openflow12::OFP_NO_BUFFER;
	case openflow13::OFP_VERSION: return openflow13::OFP_NO_BUFFER;
	default:
		throw eBadVersion();
	}
}



uint32_t
crofbase::get_ofp_controller_port(uint8_t ofp_version)
{
	switch (ofp_version) {
	case openflow10::OFP_VERSION: return openflow10::OFPP_CONTROLLER;
	case openflow12::OFP_VERSION: return openflow12::OFPP_CONTROLLER;
	case openflow13::OFP_VERSION: return openflow13::OFPP_CONTROLLER;
	default:
		throw eBadVersion();
	}
}



uint32_t
crofbase::get_ofp_flood_port(uint8_t ofp_version)
{
	switch (ofp_version) {
	case openflow10::OFP_VERSION: return openflow10::OFPP_FLOOD;
	case openflow12::OFP_VERSION: return openflow12::OFPP_FLOOD;
	case openflow13::OFP_VERSION: return openflow13::OFPP_FLOOD;
	default:
		throw eBadVersion();
	}
}



uint8_t
crofbase::get_ofp_command(uint8_t ofp_version, enum openflow::ofp_flow_mod_command const& cmd)
{
	switch (ofp_version) {
	case openflow10::OFP_VERSION: {
		switch (cmd) {
		case openflow::OFPFC_ADD: 			return openflow10::OFPFC_ADD;
		case openflow::OFPFC_MODIFY: 		return openflow10::OFPFC_MODIFY;
		case openflow::OFPFC_MODIFY_STRICT: return openflow10::OFPFC_MODIFY_STRICT;
		case openflow::OFPFC_DELETE: 		return openflow10::OFPFC_DELETE;
		case openflow::OFPFC_DELETE_STRICT: return openflow10::OFPFC_DELETE_STRICT;
		default:
			throw eBadVersion();
		}
	} break;
	case openflow12::OFP_VERSION: {
		switch (cmd) {
		case openflow::OFPFC_ADD: 			return openflow12::OFPFC_ADD;
		case openflow::OFPFC_MODIFY: 		return openflow12::OFPFC_MODIFY;
		case openflow::OFPFC_MODIFY_STRICT: return openflow12::OFPFC_MODIFY_STRICT;
		case openflow::OFPFC_DELETE: 		return openflow12::OFPFC_DELETE;
		case openflow::OFPFC_DELETE_STRICT: return openflow12::OFPFC_DELETE_STRICT;
		default:
			throw eBadVersion();
		}
	} break;
	case openflow13::OFP_VERSION: {
		switch (cmd) {
		case openflow::OFPFC_ADD: 			return openflow13::OFPFC_ADD;
		case openflow::OFPFC_MODIFY: 		return openflow13::OFPFC_MODIFY;
		case openflow::OFPFC_MODIFY_STRICT: return openflow13::OFPFC_MODIFY_STRICT;
		case openflow::OFPFC_DELETE: 		return openflow13::OFPFC_DELETE;
		case openflow::OFPFC_DELETE_STRICT: return openflow13::OFPFC_DELETE_STRICT;
		default:
			throw eBadVersion();
		}
	} break;
	default:
		throw eBadVersion();
	}
}



void
crofbase::send_packet_in_message(
		const cauxid& auxid,
		uint32_t buffer_id,
		uint16_t total_len,
		uint8_t reason,
		uint8_t table_id,
		uint64_t cookie,
		uint16_t in_port, // for OF1.0
		rofl::openflow::cofmatch &match,
		uint8_t *data,
		size_t datalen)
{
	bool sent_out = false;

	for (std::map<cctlid, crofctl*>::iterator
			it = rofctls.begin(); it != rofctls.end(); ++it) {

		crofctl& ctl = *(it->second);

		if (not ctl.is_established()) {
			continue;
		}

		switch (ctl.get_version_negotiated()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			// channel lost?
			continue;
		} break;
		case rofl::openflow12::OFP_VERSION: {

			switch (ctl.get_role().get_role()) {
			case rofl::openflow13::OFPCR_ROLE_SLAVE: {
					continue;
			} break;
			default: {
				// master/equal/unknown role: send packet-in to controller
			};
			}

		} break;
		case rofl::openflow13::OFP_VERSION: {

			switch (ctl.get_role().get_role()) {
			case rofl::openflow13::OFPCR_ROLE_EQUAL:
			case rofl::openflow13::OFPCR_ROLE_MASTER: {
				if (not (ctl.get_async_config().get_packet_in_mask_master() & (1 << reason))) {
					continue;
				}
			} break;
			case rofl::openflow13::OFPCR_ROLE_SLAVE: {
				if (not (ctl.get_async_config().get_packet_in_mask_slave() & (1 << reason))) {
					continue;
				}
			} break;
			default: {
				// unknown role: send packet-in to controller
			};
			}

		} break;
		default: {
			// unknown version: send packet-in to controller
		};
		}

		ctl.send_packet_in_message(
				auxid,
				buffer_id,
				total_len,
				reason,
				table_id,
				cookie,
				in_port, // for OF1.0
				match,
				data,
				datalen);

		sent_out = true;
	}

	if (not sent_out) {
		throw eRofBaseNotConnected();
	}
}



void
crofbase::send_flow_removed_message(
		const cauxid& auxid,
		rofl::openflow::cofmatch& match,
		uint64_t cookie,
		uint16_t priority,
		uint8_t reason,
		uint8_t table_id,
		uint32_t duration_sec,
		uint32_t duration_nsec,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint64_t packet_count,
		uint64_t byte_count)
{
	bool sent_out = false;

	for (std::map<cctlid, crofctl*>::iterator
			it = rofctls.begin(); it != rofctls.end(); ++it) {

		crofctl& ctl = *(it->second);

		if (not ctl.is_established()) {
			continue;
		}

		switch (ctl.get_version_negotiated()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			// channel lost?
			continue;
		} break;
		case rofl::openflow12::OFP_VERSION: {

			switch (ctl.get_role().get_role()) {
			case rofl::openflow13::OFPCR_ROLE_SLAVE: {
					continue;
			} break;
			default: {
				// master/equal/unknown role: send packet-in to controller
			};
			}

		} break;
		case rofl::openflow13::OFP_VERSION: {

			switch (ctl.get_role().get_role()) {
			case rofl::openflow13::OFPCR_ROLE_EQUAL:
			case rofl::openflow13::OFPCR_ROLE_MASTER: {
				if (not (ctl.get_async_config().get_flow_removed_mask_master() & (1 << reason))) {
					continue;
				}
			} break;
			case rofl::openflow13::OFPCR_ROLE_SLAVE: {
				if (not (ctl.get_async_config().get_flow_removed_mask_slave() & (1 << reason))) {
					continue;
				}
			} break;
			default: {
				// unknown role: send packet-in to controller
			};
			}

		} break;
		default: {
			// unknown version: send packet-in to controller
		};
		}

		ctl.send_flow_removed_message(
				auxid,
				match,
				cookie,
				priority,
				reason,
				table_id,
				duration_sec,
				duration_nsec,
				idle_timeout,
				hard_timeout,
				packet_count,
				byte_count);

		sent_out = true;
	}

	if (not sent_out) {
		throw eRofBaseNotConnected();
	}
}



void
crofbase::send_port_status_message(
		const cauxid& auxid,
		uint8_t reason,
		const rofl::openflow::cofport& port)
{
	bool sent_out = false;

	for (std::map<cctlid, crofctl*>::iterator
			it = rofctls.begin(); it != rofctls.end(); ++it) {

		crofctl& ctl = *(it->second);

		if (not ctl.is_established()) {
			continue;
		}

		switch (ctl.get_version_negotiated()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			// channel lost?
			continue;
		} break;
		case rofl::openflow12::OFP_VERSION: {

			switch (ctl.get_role().get_role()) {
			case rofl::openflow13::OFPCR_ROLE_SLAVE: {
					continue;
			} break;
			default: {
				// master/equal/unknown role: send packet-in to controller
			};
			}

		} break;
		case rofl::openflow13::OFP_VERSION: {

			switch (ctl.get_role().get_role()) {
			case rofl::openflow13::OFPCR_ROLE_EQUAL:
			case rofl::openflow13::OFPCR_ROLE_MASTER: {
				if (not (ctl.get_async_config().get_port_status_mask_master() & (1 << reason))) {
					continue;
				}
			} break;
			case rofl::openflow13::OFPCR_ROLE_SLAVE: {
				if (not (ctl.get_async_config().get_port_status_mask_slave() & (1 << reason))) {
					continue;
				}
			} break;
			default: {
				// unknown role: send packet-in to controller
			};
			}

		} break;
		default: {
			// unknown version: send packet-in to controller
		};
		}

		ctl.send_port_status_message(auxid, reason, port);

		sent_out = true;
	}

	if (not sent_out) {
		throw eRofBaseNotConnected();
	}
}




