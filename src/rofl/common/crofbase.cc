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
				cached_generation_id((uint64_t)((int64_t)-1)),
				async_config_role_default_template(rofl::openflow13::OFP_VERSION)
{
	set_async_config_role_default_template();

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
		for (std::set<csocket*>::iterator it = listening_sockets[RPC_CTL].begin();
				it != listening_sockets[RPC_CTL].end(); ++it) {
			delete (*it);
		}
		listening_sockets[RPC_CTL].clear();

		for (std::set<csocket*>::iterator it = listening_sockets[RPC_DPT].begin();
				it != listening_sockets[RPC_DPT].end(); ++it) {
			delete (*it);
		}
		listening_sockets[RPC_DPT].clear();

		// detach from higher layer entities
		while (not rofctls.empty()) {
			drop_ctl(rofctls.begin()->first);
		}

		while (not rofdpts.empty()) {
			drop_dpt(rofdpts.begin()->first);
		}

	} catch (RoflException& e) {
		rofl::logging::error << "[rofl][crofbase][rpc_close_all] exception:" << e << std::endl;
	}
}



void
crofbase::nsp_enable(bool enable)
{
	if (enable) {
		fe_flags.set(NSP_ENABLED);
		rofl::logging::info << "[rofl][base] enabling namespace support" << std::endl;
	} else {
		fe_flags.reset(NSP_ENABLED);
		rofl::logging::info << "[rofl][base] disabling namespace support" << std::endl;
	}
}



void
crofbase::handle_connect_refused(
		crofconn *conn)
{
	rofl::logging::info << "[rofl][base] connection refused:" << std::endl << *conn;
}



void
crofbase::handle_connect_failed(
		crofconn *conn)
{
	rofl::logging::info << "[rofl][base] connection failed:" << std::endl << *conn;
}



void
crofbase::handle_connected(
		crofconn *conn,
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
	try {

		crofdpt::get_dpt(conn->get_dpid()).set_channel().add_conn(conn->get_aux_id(), conn);

	} catch (eRofDptNotFound& e) {

		// TODO: THINK: test for aux_id == 0 here?
		crofdpt *dpt = rofdpt_factory(this, versionbitmap);
		rofdpts[dpt->get_dptid()] = dpt;
		rofl::logging::info << "[rofl][base] new dpt representing handle created for dpid:"
				<< conn->get_dpid() << std::endl;

		dpt->set_channel().add_conn(conn->get_aux_id(), conn);
	}
}




uint8_t
crofbase::get_highest_supported_ofp_version()
{
	return versionbitmap.get_highest_ofp_version();
}



bool
crofbase::is_ofp_version_supported(uint8_t ofp_version)
{
	return versionbitmap.has_ofp_version(ofp_version);
}




void
crofbase::rpc_listen_for_dpts(
		enum rofl::csocket::socket_type_t socket_type,
		const cparams& params)
{
	(*(listening_sockets[RPC_DPT].insert(csocket::csocket_factory(socket_type, this)).first))->listen(params);
}




void
crofbase::rpc_listen_for_ctls(
		enum rofl::csocket::socket_type_t socket_type,
		const cparams& params)
{
	(*(listening_sockets[RPC_CTL].insert(csocket::csocket_factory(socket_type, this)).first))->listen(params);
}



void
crofbase::handle_listen(
		csocket& socket, int newsd)
{
	(new rofl::crofconn(this, versionbitmap))->accept(socket.get_socket_type(), socket.get_socket_params(), newsd);
}



void
crofbase::handle_closed(
		csocket& socket)
{
	if (listening_sockets[RPC_CTL].find(&socket) != listening_sockets[RPC_CTL].end()) {
		listening_sockets[RPC_CTL].erase(&socket);
	} else
	if (listening_sockets[RPC_DPT].find(&socket) != listening_sockets[RPC_DPT].end()) {
		listening_sockets[RPC_DPT].erase(&socket);
	}
}



rofl::crofctl&
crofbase::rpc_connect_to_ctl(
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		enum rofl::csocket::socket_type_t socket_type,
		cparams const& socket_params)
{
	const cctlid& ctlid = add_ctl(versionbitmap);
	set_ctl(ctlid).connect(socket_type, socket_params);
	return set_ctl(ctlid);
}



rofl::crofdpt&
crofbase::rpc_connect_to_dpt(
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		enum rofl::csocket::socket_type_t socket_type,
		cparams const& socket_params)
{
	const cdptid& dptid = add_dpt(versionbitmap);
	set_dpt(dptid).connect(socket_type, socket_params);
	return set_dpt(dptid);
}





cdptid const&
crofbase::add_dpt(
	const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap)
{
	crofdpt *rofdpt = rofdpt_factory(this, versionbitmap);
	rofdpts[rofdpt->get_dptid()] = rofdpt;
	return rofdpt->get_dptid();
}



void
crofbase::drop_dpt(
	const cdptid& dptid)
{
	if (rofdpts.find(dptid) == rofdpts.end()) {
		return;
	}
	delete rofdpts[dptid];
	rofdpts.erase(dptid);
}



crofdpt&
crofbase::set_dpt(
	const cdptid& dptid)
{
	if (rofdpts.find(dptid) == rofdpts.end()) {
		throw eRofBaseNotFound();
	}
	return *(rofdpts[dptid]);
}



bool
crofbase::has_dpt(
		const cdptid& dptid) const
{
	return (not (rofdpts.find(dptid) == rofdpts.end()));
}



cctlid const&
crofbase::add_ctl(
	const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap)
{
	crofctl *rofctl = rofctl_factory(this, versionbitmap);
	rofctls[rofctl->get_ctlid()] = rofctl;
	return rofctl->get_ctlid();
}



void
crofbase::drop_ctl(
	const cctlid& ctlid)
{
	if (rofctls.find(ctlid) == rofctls.end()) {
		return;
	}
	delete rofctls[ctlid];
	rofctls.erase(ctlid);
}



crofctl&
crofbase::set_ctl(
	const cctlid& ctlid)
{
	if (rofctls.find(ctlid) == rofctls.end()) {
		throw eRofBaseNotFound();
	}
	return *(rofctls[ctlid]);
}



bool
crofbase::has_ctl(
		const cctlid& ctlid) const
{
	return (not (rofctls.find(ctlid) == rofctls.end()));
}



crofdpt*
crofbase::rofdpt_factory(
		crofbase* owner,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap)
{
	return new crofdpt_impl(owner, versionbitmap);
}



crofctl*
crofbase::rofctl_factory(
		crofbase* owner,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap)
{
	return new crofctl_impl(owner, versionbitmap);
}




void
crofbase::role_request_rcvd(
		crofctl *ctl,
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
	ctl->set_role().set_generation_id(cached_generation_id);

	switch (role) {
	case rofl::openflow13::OFPCR_ROLE_MASTER: {

		// iterate over all attached controllers and check for an existing master
		for (std::map<cctlid, crofctl*>::iterator
				it = rofctls.begin(); it != rofctls.end(); ++it) {

			// ignore ctl who called this method
			if (it->second == ctl)
				continue;

			// find any other controller and set them back to role SLAVE
			if (rofl::openflow13::OFPCR_ROLE_MASTER == it->second->get_role().get_role()) {
				it->second->set_role().set_role(rofl::openflow13::OFPCR_ROLE_SLAVE);
			}
		}

		// set new master async-config to template retrieved from of-config (or default one)
		ctl->set_async_config() = async_config_role_default_template;

		ctl->set_role().set_role(rofl::openflow13::OFPCR_ROLE_MASTER);

	} break;
	case rofl::openflow13::OFPCR_ROLE_SLAVE: {

		ctl->set_async_config() = async_config_role_default_template;
		ctl->set_role().set_role(rofl::openflow13::OFPCR_ROLE_SLAVE);

	} break;
	case rofl::openflow13::OFPCR_ROLE_EQUAL: {

		ctl->set_async_config() = async_config_role_default_template;
		ctl->set_role().set_role(rofl::openflow13::OFPCR_ROLE_EQUAL);

	} break;
	case rofl::openflow13::OFPCR_ROLE_NOCHANGE:
	default: {
		// let crofctl_impl send a role-reply with the controller's unaltered current role
	}
	}
}



void
crofbase::handle_timeout(int opaque, void *data)
{
	try {
		switch (opaque) {
		case CROFBASE_TIMER_WAKEUP: {
			// do nothing, just re-schedule via ciosrv::run()::pselect()
		} break;
		default: {
		} break;
		}

	} catch (eIoSvcUnhandledTimer& e) {
		// ignore
	}
}



void
crofbase::handle_event(cevent const& ev)
{
	cevent event(ev);
	switch (event.cmd) {
	case CROFBASE_EVENT_WAKEUP: {
		// do nothing, just re-schedule via ciosrv::run()::pselect()
	} break;
	}
}


void
crofbase::wakeup()
{
	if (get_thread_id() != pthread_self())
	{
		notify(CROFBASE_EVENT_WAKEUP);
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

		switch (ctl.get_version()) {
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

		switch (ctl.get_version()) {
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
		rofl::openflow::cofport const& port)
{
	bool sent_out = false;

	for (std::map<cctlid, crofctl*>::iterator
			it = rofctls.begin(); it != rofctls.end(); ++it) {

		crofctl& ctl = *(it->second);

		if (not ctl.is_established()) {
			continue;
		}

		switch (ctl.get_version()) {
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



void
crofbase::set_async_config_role_default_template()
{
	async_config_role_default_template.set_packet_in_mask_master() =
			(1 << rofl::openflow13::OFPR_NO_MATCH) |
			(1 << rofl::openflow13::OFPR_ACTION);

	async_config_role_default_template.set_packet_in_mask_slave(0);

	async_config_role_default_template.set_port_status_mask_master() =
			(1 << rofl::openflow13::OFPPR_ADD) |
			(1 << rofl::openflow13::OFPPR_DELETE) |
			(1 << rofl::openflow13::OFPPR_MODIFY);

	async_config_role_default_template.set_port_status_mask_slave() =
			(1 << rofl::openflow13::OFPPR_ADD) |
			(1 << rofl::openflow13::OFPPR_DELETE) |
			(1 << rofl::openflow13::OFPPR_MODIFY);

	async_config_role_default_template.set_flow_removed_mask_master() =
			(1 << rofl::openflow13::OFPRR_IDLE_TIMEOUT) |
			(1 << rofl::openflow13::OFPRR_HARD_TIMEOUT) |
			(1 << rofl::openflow13::OFPRR_DELETE) |
			(1 << rofl::openflow13::OFPRR_GROUP_DELETE);

	async_config_role_default_template.set_flow_removed_mask_slave(0);
}


