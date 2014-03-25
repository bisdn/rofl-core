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

	crofbase::rofbases.insert(this);
}


crofbase::~crofbase()
{
	crofbase::rofbases.erase(this);

	rpc_close_all();
}



void
crofbase::send_packet_in_message(
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

	for (std::set<crofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it) {

		crofctl& ctl = *(*it);

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

	for (std::set<crofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it) {

		crofctl& ctl = *(*it);

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
	uint8_t reason,
	rofl::openflow::cofport const& port)
{
	bool sent_out = false;

	for (std::set<crofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it) {

		crofctl& ctl = *(*it);

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

		ctl.send_port_status_message(reason, port);

		sent_out = true;
	}

	if (not sent_out) {
		throw eRofBaseNotConnected();
	}
}



void
crofbase::rpc_close_all()
{
	// close the listening sockets
	for (std::set<csocket*>::iterator it = rpc[RPC_CTL].begin();
			it != rpc[RPC_CTL].end(); ++it)
	{
		delete (*it);
	}
	rpc[RPC_CTL].clear();

	for (std::set<csocket*>::iterator it = rpc[RPC_DPT].begin();
			it != rpc[RPC_DPT].end(); ++it)
	{
		delete (*it);
	}
	rpc[RPC_DPT].clear();

	// detach from higher layer entities
	for (std::set<crofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
	{
		delete (*it);
	}
	ofctl_set.clear();

	for (std::set<crofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		delete (*it);
	}
	ofdpt_set.clear();
}






void
crofbase::nsp_enable(bool enable)
{
	if (enable) {
		fe_flags.set(NSP_ENABLED);
		logging::info << "[rofl][base] enabling namespace support" << std::endl;
	} else {
		fe_flags.reset(NSP_ENABLED);
		logging::info << "[rofl][base] disabling namespace support" << std::endl;
	}
}



void
crofbase::handle_connect_refused(
		crofconn *conn)
{

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

		crofdpt::get_dpt(conn->get_dpid()).get_channel().add_conn(conn, conn->get_aux_id());

	} catch (eRofDptNotFound& e) {

		// TODO: THINK: test for aux_id == 0 here?
		crofdpt *dpt = cofdpt_factory(this, versionbitmap);
		ofdpt_set.insert(dpt);
		logging::info << "[rofl][base] new dpt representing handle created for dpid:"
				<< conn->get_dpid() << std::endl;

		dpt->get_channel().add_conn(conn, conn->get_aux_id());
	}
}



void
crofbase::handle_closed(
		crofconn *conn)
{

}


#if 0
void
crofbase::handle_connected(
		crofchan *chan,
		uint8_t aux_id)
{

}



void
crofbase::handle_closed(
		crofchan *chan,
		uint8_t aux_id)
{

}
#endif


void
crofbase::handle_dpt_open(
		crofdpt *dpt)
{
	handle_dpath_open(*dpt);
}



void
crofbase::handle_dpt_close(
		crofdpt *dpt)
{
	handle_dpath_close(*dpt);
	if (ofdpt_set.find(dpt) != ofdpt_set.end())
	{
		delete dpt;
		ofdpt_set.erase(dpt);
	}
}



void
crofbase::handle_ctl_open(
		crofctl *ctl)
{
	handle_ctrl_open(ctl);
}



void
crofbase::handle_ctl_close(
		crofctl *ctl)
{
	if (ofctl_set.find(ctl) != ofctl_set.end())
	{
		ofctl_set.erase(ctl);
	}
	handle_ctrl_close(ctl);
	delete ctl;
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
crofbase::handle_accepted(
		csocket& socket,
		int newsd,
		caddress const& ra)
{
#ifdef HAVE_OPENSSL
	(new rofl::crofconn(this, versionbitmap))->accept(newsd, socket.ssl_ctx);
#else
	(new rofl::crofconn(this, versionbitmap))->accept(newsd, NULL);
#endif
}



void
crofbase::handle_connected(
		csocket& socket)
{
	// do nothing here, as our TCP sockets are used as listening sockets only
}



void
crofbase::handle_connect_refused(
		csocket& socket)
{
	// do nothing here, as our TCP sockets are used as listening sockets only
}



void
crofbase::handle_read(
		csocket& socket)
{
	// do nothing here, as our TCP sockets are used as listening sockets only
}



void
crofbase::handle_closed(
		csocket& socket)
{
	if (rpc[RPC_CTL].find(&socket) != rpc[RPC_CTL].end())
	{
		rpc[RPC_CTL].erase(&socket);
	}
	else if (rpc[RPC_DPT].find(&socket) != rpc[RPC_DPT].end())
	{
		rpc[RPC_DPT].erase(&socket);
	}
	else
	{
		// do nothing
	}
}



void
crofbase::rpc_listen_for_dpts(
		caddress const& addr,
		int domain,
		int type,
		int protocol,
		ssl_context *ssl_ctx,
		int backlog)
{
	csocket *socket = new csocket(this, domain, type, protocol, backlog);
	socket->listen(addr, domain, type, protocol, ssl_ctx, backlog);
	rpc[RPC_DPT].insert(socket);
}



void
crofbase::rpc_listen_for_ctls(
		caddress const& addr,
		int domain,
		int type,
		int protocol,
		ssl_context *ssl_ctx,
		int backlog)
{
	csocket *socket = new csocket(this, domain, type, protocol, backlog);
	socket->listen(addr, domain, type, protocol, ssl_ctx, backlog);
	rpc[RPC_CTL].insert(socket);
}



void
crofbase::rpc_connect_to_ctl(
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol,
		ssl_context *ssl_ctx)
{
	ofctl_set.insert(cofctl_factory(this, versionbitmap, reconnect_start_timeout, ra, domain, type, protocol, ssl_ctx));
}


void
crofbase::rpc_disconnect_from_ctl(
		crofctl *ctl)
{
	if (0 == ctl)
	{
		return;
	}

	if (ofctl_set.find(ctl) == ofctl_set.end())
	{
		return;
	}

	delete ctl;

	ofctl_set.erase(ctl);
}


void
crofbase::rpc_disconnect_from_ctl(
		caddress const& ra)
{
	for (std::set<crofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it) {
		crofctl *ctl = (*it);
		if (ctl->get_peer_addr() == ra) {
			rpc_disconnect_from_ctl(ctl);
			return;
		}
	}
}



void
crofbase::rpc_disconnect_from_dpt(
		crofdpt *dpt)
{
	if (0 == dpt) {
		return;
	}

	if (ofdpt_set.find(dpt) == ofdpt_set.end()) {
		return;
	}

	delete dpt;

	ofdpt_set.erase(dpt);
}



void
crofbase::rpc_disconnect_from_dpt(
		caddress const& ra)
{
	for (std::set<crofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it) {
		crofdpt *dpt = (*it);
		if (dpt->get_peer_addr() == ra) {
			rpc_disconnect_from_dpt(dpt);
			return;
		}
	}
}



crofctl*
crofbase::cofctl_factory(
		crofbase* owner,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol,
		ssl_context *ssl_ctx)
{
	return new crofctl_impl(owner, versionbitmap, reconnect_start_timeout, ra, domain, type, protocol, ssl_ctx);
}



crofdpt*
crofbase::cofdpt_factory(
		crofbase* owner,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap)
{
	return new crofdpt_impl(owner, versionbitmap);
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
		for (std::set<crofctl*>::iterator
				it = ofctl_set.begin(); it != ofctl_set.end(); ++it) {

			// ignore ctl who called this method
			if (*it == ctl)
				continue;

			// find any other controller and set them back to role SLAVE
			if (rofl::openflow13::OFPCR_ROLE_MASTER == (*it)->get_role().get_role()) {
				(*it)->set_role().set_role(rofl::openflow13::OFPCR_ROLE_SLAVE);
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




crofdpt*
crofbase::dpt_find(uint64_t dpid) throw (eRofBaseNotFound)
{
	for (std::set<crofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		if ((*it)->get_dpid() == dpid)
			return (*it);
	}
	throw eRofBaseNotFound();
}


crofdpt&
crofbase::get_dpt(
	uint64_t dpid)
{
	for (std::set<crofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it) {
		if ((*it)->get_dpid() == dpid)
			return *(*it);
	}
	throw eRofBaseNotFound();
}


crofdpt*
crofbase::dpt_find(std::string s_dpid) throw (eRofBaseNotFound)
{
	for (std::set<crofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		if ((*it)->get_dpid_s() == s_dpid)
			return (*it);
	}
	throw eRofBaseNotFound();
}


crofdpt*
crofbase::dpt_find(cmacaddr dl_dpid) throw (eRofBaseNotFound)
{
	for (std::set<crofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		if ((*it)->get_hwaddr() == dl_dpid)
			return (*it);
	}
	throw eRofBaseNotFound();
}




crofdpt*
crofbase::dpt_find(crofdpt *dpt) throw (eRofBaseNotFound)
{
	if (ofdpt_set.find(dpt) == ofdpt_set.end())
	{
		throw eRofBaseNotFound();
	}
	return dpt;
}



crofctl*
crofbase::ctl_find(crofctl *ctl) throw (eRofBaseNotFound)
{
	if (ofctl_set.find(ctl) == ofctl_set.end())
	{
		throw eRofBaseNotFound();
	}
	return ctl;
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


