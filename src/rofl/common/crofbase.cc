/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

extern "C" {
// autoconf AC_CHECK_LIB helper function as C-declaration
void librofl_is_present(void) {};
}

#include "crofbase.h"

using namespace rofl;

/* static */ std::set<crofbase*> crofbase::rofbases;

crofbase::crofbase(uint32_t supported_ofp_versions) :
		supported_ofp_versions(supported_ofp_versions),
		xid_used_max(CPCP_DEFAULT_XID_USED_MAX),
		xid_start(crandom(sizeof(uint32_t)).uint32())
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::crofbase()", this);

	//register_timer(TIMER_FE_DUMP_OFPACKETS, 15);

	pthread_rwlock_init(&xidlock, 0);

	crofbase::rofbases.insert(this);
}


crofbase::~crofbase()
{
	rpc_close_all();
	
	crofbase::rofbases.erase(this);
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::~crofbase()", this);

	pthread_rwlock_destroy(&xidlock);
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
	if (enable)
	{
		fe_flags.set(NSP_ENABLED);
		WRITELOG(CROFBASE, INFO, "crofbase(%p)::nsp_enable() enabling -NAMESPACE- support", this);
	}
	else
	{
		fe_flags.reset(NSP_ENABLED);
		WRITELOG(CROFBASE, INFO, "crofbase(%p)::nsp_enable() disabling -NAMESPACE- support", this);
	}
}



void
crofbase::handle_dpt_open(
		crofdpt *dpt)
{
	handle_dpath_open(dpt);
}



void
crofbase::handle_dpt_close(
		crofdpt *dpt)
{
	handle_dpath_close(dpt);
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
	if (supported_ofp_versions & (1 << OFP13_VERSION)) {
		return OFP13_VERSION;
	}
	else if (supported_ofp_versions & (1 << OFP12_VERSION)) {
		return OFP12_VERSION;
	}
	else if (supported_ofp_versions & (1 << OFP10_VERSION)) {
		return OFP10_VERSION;
	}
	throw eRofBaseNotFound();
}



bool
crofbase::is_ofp_version_supported(uint8_t ofp_version)
{
	return ((1 << ofp_version) & supported_ofp_versions) ? true : false;
}



void
crofbase::handle_accepted(
		csocket *socket,
		int newsd,
		caddress const& ra)
{
	if (rpc[RPC_CTL].find(socket) != rpc[RPC_CTL].end())
	{
#ifndef NDEBUG
		caddress raddr(ra);
		fprintf(stderr, "A:ctl[%s] ", raddr.c_str());
#endif
		WRITELOG(CROFBASE, INFO, "crofbase(%p): new ctl TCP connection", this);
		ofctl_set.insert(cofctl_factory(this, newsd, ra, socket->domain, socket->type, socket->protocol));
	}
	else if (rpc[RPC_DPT].find(socket) != rpc[RPC_DPT].end())
	{
#ifndef NDEBUG
		caddress raddr(ra);
		fprintf(stderr, "A:dpt[%s] ", raddr.c_str());
#endif
		WRITELOG(CROFBASE, INFO, "crofbase(%p): new dpt TCP connection", this);
		ofdpt_set.insert(cofdpt_factory(this, newsd, ra, socket->domain, socket->type, socket->protocol));
	}
	else
	{
		WRITELOG(CROFBASE, INFO, "crofbase(%p): new unknown TCP connection, closing", this);
		close(newsd);
	}
}



void
crofbase::handle_connected(
		csocket *socket,
		int sd)
{
	// do nothing here, as our TCP sockets are used as listening sockets only
}



void
crofbase::handle_connect_refused(
		csocket *socket,
		int sd)
{
	// do nothing here, as our TCP sockets are used as listening sockets only
}



void
crofbase::handle_read(
		csocket *socket,
		int sd)
{
	// do nothing here, as our TCP sockets are used as listening sockets only
}



void
crofbase::handle_closed(
		csocket *socket,
		int sd)
{
	if (rpc[RPC_CTL].find(socket) != rpc[RPC_CTL].end())
	{
		rpc[RPC_CTL].erase(socket);
	}
	else if (rpc[RPC_DPT].find(socket) != rpc[RPC_DPT].end())
	{
		rpc[RPC_DPT].erase(socket);
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
		int backlog)
{
	csocket *socket = new csocket(this, domain, type, protocol, backlog);
	socket->clisten(addr, domain, type, protocol, backlog);
	rpc[RPC_DPT].insert(socket);
}



void
crofbase::rpc_listen_for_ctls(
		caddress const& addr,
		int domain,
		int type,
		int protocol,
		int backlog)
{
	csocket *socket = new csocket(this, domain, type, protocol, backlog);
	socket->clisten(addr, domain, type, protocol, backlog);
	rpc[RPC_CTL].insert(socket);
}



void
crofbase::rpc_connect_to_ctl(
		uint8_t ofp_version,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	ofctl_set.insert(cofctl_factory(this, ofp_version, reconnect_start_timeout, ra, domain, type, protocol));
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
crofbase::rpc_connect_to_dpt(
		uint8_t ofp_version,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	ofdpt_set.insert(cofdpt_factory(this, ofp_version, reconnect_start_timeout, ra, domain, type, protocol));
}


void
crofbase::rpc_disconnect_from_dpt(
		crofdpt *dpt)
{
	if (0 == dpt)
	{
		return;
	}

	if (ofdpt_set.find(dpt) == ofdpt_set.end())
	{
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
		int newsd,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	return new crofctlImpl(owner, newsd, ra, domain, type, protocol);
}



crofctl*
crofbase::cofctl_factory(
		crofbase* owner,
		uint8_t ofp_version,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	return new crofctlImpl(owner, ofp_version, reconnect_start_timeout, ra, domain, type, protocol);
}



crofdpt*
crofbase::cofdpt_factory(
		crofbase* owner,
		int newsd,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	return new crofdptImpl(owner, newsd, ra, domain, type, protocol);
}



crofdpt*
crofbase::cofdpt_factory(
		crofbase* owner,
		uint8_t ofp_version,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	return new crofdptImpl(owner, ofp_version, reconnect_start_timeout, ra, domain, type, protocol);
}



void
crofbase::role_request_rcvd(
		crofctl *ctl,
		uint32_t role)
{
	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		switch (role) {
		case openflow12::OFPCR_ROLE_NOCHANGE: {
		} break;
		case openflow12::OFPCR_ROLE_EQUAL: {
		} break;
		case openflow12::OFPCR_ROLE_MASTER: {
			for (std::set<crofctl*>::iterator
					it = ofctl_set.begin(); it != ofctl_set.end(); ++it) {
				crofctl *tctl = (*it);

				if (tctl == ctl)
					continue;

				if (openflow12::OFPCR_ROLE_MASTER == tctl->get_role())
					tctl->set_role(openflow12::OFPCR_ROLE_SLAVE);
			}
		} break;
		case openflow12::OFPCR_ROLE_SLAVE: {
		} break;
		default: {
		} break;
		}
	} break;
	case openflow13::OFP_VERSION: {
		switch (role) {
		case openflow13::OFPCR_ROLE_NOCHANGE: {
		} break;
		case openflow13::OFPCR_ROLE_EQUAL: {
		} break;
		case openflow13::OFPCR_ROLE_MASTER: {
			for (std::set<crofctl*>::iterator
					it = ofctl_set.begin(); it != ofctl_set.end(); ++it) {
				crofctl *tctl = (*it);

				if (tctl == ctl)
					continue;

				if (openflow13::OFPCR_ROLE_MASTER == tctl->get_role())
					tctl->set_role(openflow13::OFPCR_ROLE_SLAVE);
			}
		} break;
		case openflow13::OFPCR_ROLE_SLAVE: {
		} break;
		default: {
		} break;
		}
	} break;
	default:
		throw eBadVersion();
	}
}



void
crofbase::handle_timeout(int opaque)
{
	try {
		switch (opaque) {
		case CROFBASE_TIMER_WAKEUP: {
			// do nothing, just re-schedule via ciosrv::run()::pselect()
		} break;
		default: {
			//WRITELOG(CROFBASE, DBG, "crofbase::handle_timeout() "
			//		"received unknown timer event %d", opaque);
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


void
crofbase::handle_experimenter_message(crofctl *ofctrl, cofmsg_experimenter *pack)
{
	// base class does not support any vendor extensions, so: send error indication
	size_t datalen = (pack->framelen() > 64) ? 64 : pack->framelen();
	send_error_bad_request_bad_experimenter(ofctrl, pack->get_xid(),
									(unsigned char*)pack->soframe(), datalen);

	delete pack;
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



/*
* HELLO messages
*/

void
crofbase::send_hello_message(
		crofctl *ctl,
		uint8_t *body, size_t bodylen)
{
	cofmsg_hello *pack =
			new cofmsg_hello(
					ctl->get_version(),
					ta_new_async_xid(),
					(uint8_t*)body, bodylen);

	ctl_find(ctl)->send_message(pack);
}



void
crofbase::send_hello_message(
		crofdpt *dpt,
		uint8_t *body, size_t bodylen)
{
	cofmsg_hello *pack =
			new cofmsg_hello(
					dpt->get_version(),
					ta_new_async_xid(),
					body,
					bodylen);

	logging::debug << "sending HELLO message " << std::endl << *pack << std::endl;

	dpt_find(dpt)->send_message(pack);
}




void
crofbase::send_echo_request(
		crofdpt *dpt,
		uint8_t *body, size_t bodylen)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_ECHO_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_ECHO_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_ECHO_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_echo_request *msg =
			new cofmsg_echo_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					body,
					bodylen);

	dpt_find(dpt)->send_message(msg);
}



void
crofbase::send_echo_reply(
		crofdpt *dpt,
		uint32_t xid,
		uint8_t *body, size_t bodylen)
{
	cofmsg_echo_reply *msg =
			new cofmsg_echo_reply(
					dpt->get_version(),
					xid,
					body,
					bodylen);

	dpt_find(dpt)->send_message(msg);
}



void
crofbase::send_echo_request(
		crofctl *ctl,
		uint8_t *body, size_t bodylen)
{
	uint8_t msg_type = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_ECHO_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_ECHO_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_ECHO_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_echo_request *msg =
			new cofmsg_echo_request(
					ctl->get_version(),
					ta_add_request(msg_type),
					body,
					bodylen);

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_echo_reply(
		crofctl *ctl,
		uint32_t xid,
		uint8_t *body, size_t bodylen)
{
	cofmsg_echo_reply *msg =
			new cofmsg_echo_reply(
					ctl->get_version(),
					xid,
					body,
					bodylen);

	ctl_find(ctl)->send_message(msg);
}



/*
 * FEATURES request/reply
 */

uint32_t
crofbase::send_features_request(crofdpt *dpt)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_FEATURES_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_FEATURES_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_FEATURES_REQUEST; break;
	default:
		throw eBadVersion();
	}

	uint32_t xid = 0;

	cofmsg_features_request *msg =
			new cofmsg_features_request(
					dpt->get_version(),
					ta_add_request(msg_type));

	xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



void
crofbase::send_features_reply(
		crofctl *ctl,
		uint32_t xid,
		uint64_t dpid,
		uint32_t n_buffers,
		uint8_t n_tables,
		uint32_t capabilities,
		uint8_t of13_auxiliary_id,
		uint32_t of10_actions_bitmap,
		cofportlist const& portlist)
{
	cofmsg_features_reply *reply =
			new cofmsg_features_reply(
					ctl->get_version(),
					xid,
					dpid,
					n_buffers,
					n_tables,
					capabilities,
					of10_actions_bitmap,
					of13_auxiliary_id,
					portlist);

	reply->pack(); // adjust fields, e.g. length in ofp_header

	ctl_find(ctl)->send_message(reply);
}




void
crofbase::handle_features_reply_timeout(crofdpt *dpt)
{
    if (ofdpt_set.find(dpt) != ofdpt_set.end()) {
    	delete dpt;
    	ofdpt_set.erase(dpt);
    }
}




/*
 * GET-CONFIG request/reply
 */
uint32_t
crofbase::send_get_config_request(
		crofdpt *dpt)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_GET_CONFIG_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_GET_CONFIG_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_GET_CONFIG_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_get_config_request *msg =
			new cofmsg_get_config_request(
					dpt->get_version(),
					ta_add_request(msg_type));

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}





void
crofbase::send_get_config_reply(crofctl *ctl, uint32_t xid, uint16_t flags, uint16_t miss_send_len)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_get_config_reply()", this);

	cofmsg_get_config_reply *msg =
			new cofmsg_get_config_reply(
					ctl->get_version(),
					xid,
					flags,
					miss_send_len);

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::handle_get_config_reply_timeout(crofdpt *dpt)
{
    if (ofdpt_set.find(dpt) != ofdpt_set.end())
    {
           delete dpt;
           ofdpt_set.erase(dpt);
    }
}



/*
 * STATS request/reply
 */

uint32_t
crofbase::send_stats_request(
	crofdpt *dpt,
	uint16_t stats_type,
	uint16_t stats_flags,
	uint8_t* body,
	size_t bodylen)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_STATS_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_STATS_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_stats *msg =
			new cofmsg_stats(
					dpt->get_version(),
					ta_add_request(msg_type),
					stats_type,
					stats_flags,
					body,
					bodylen);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



uint32_t
crofbase::send_desc_stats_request(
		crofdpt *dpt,
		uint16_t flags)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case OFP10_VERSION: msg_type = openflow10::OFPT_STATS_REQUEST; break;
	case OFP12_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case OFP13_VERSION: msg_type = openflow13::OFPT_STATS_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_desc_stats_request *msg =
			new cofmsg_desc_stats_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					flags);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



uint32_t
crofbase::send_flow_stats_request(
		crofdpt *dpt,
		uint16_t flags,
		cofflow_stats_request const& flow_stats_request)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_STATS_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_STATS_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_flow_stats_request *msg =
			new cofmsg_flow_stats_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					flags,
					flow_stats_request);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



uint32_t
crofbase::send_aggr_stats_request(
		crofdpt *dpt,
		uint16_t flags,
		cofaggr_stats_request const& aggr_stats_request)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_STATS_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_STATS_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_aggr_stats_request *msg =
			new cofmsg_aggr_stats_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					flags,
					aggr_stats_request);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



uint32_t
crofbase::send_table_stats_request(
		crofdpt *dpt,
		uint16_t flags)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_STATS_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_STATS_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_table_stats_request *msg =
			new cofmsg_table_stats_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					flags);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



uint32_t
crofbase::send_port_stats_request(
		crofdpt *dpt,
		uint16_t flags,
		cofport_stats_request const& port_stats_request)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_STATS_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_STATS_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_port_stats_request *msg =
			new cofmsg_port_stats_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					flags,
					port_stats_request);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



uint32_t
crofbase::send_queue_stats_request(
	crofdpt *dpt,
	uint16_t flags,
	cofqueue_stats_request const& queue_stats_request)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_STATS_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_STATS_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_queue_stats_request *msg =
			new cofmsg_queue_stats_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					flags,
					queue_stats_request);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



uint32_t
crofbase::send_group_stats_request(
	crofdpt *dpt,
	uint16_t flags,
	cofgroup_stats_request const& group_stats_request)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_STATS_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_group_stats_request *msg =
			new cofmsg_group_stats_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					flags,
					group_stats_request);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



uint32_t
crofbase::send_group_desc_stats_request(
		crofdpt *dpt,
		uint16_t flags)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_STATS_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_group_desc_stats_request *msg =
			new cofmsg_group_desc_stats_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					flags);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



uint32_t
crofbase::send_group_features_stats_request(
		crofdpt *dpt,
		uint16_t flags)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_STATS_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_group_features_stats_request *msg =
			new cofmsg_group_features_stats_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					flags);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



uint32_t
crofbase::send_experimenter_stats_request(
	crofdpt *dpt,
	uint16_t flags,
	uint32_t exp_id,
	uint32_t exp_type,
	cmemory const& body)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_STATS_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_STATS_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_STATS_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_experimenter_stats_request *msg =
			new cofmsg_experimenter_stats_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					flags,
					exp_id,
					exp_type,
					body);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



void
crofbase::send_stats_reply(
		crofctl *ctl,
		uint32_t xid,
		uint16_t stats_type, /* network byte order */
		uint8_t *body, size_t bodylen,
		bool more)
{
	uint16_t flags = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: flags |= (more) ? openflow10::OFPSF_REPLY_MORE : 0; break;
	case openflow12::OFP_VERSION: flags |= (more) ? openflow12::OFPSF_REPLY_MORE : 0; break;
	case openflow13::OFP_VERSION: flags |= (more) ? openflow13::OFPSF_REPLY_MORE : 0; break;
	default:
		throw eBadVersion();
	}

	cofmsg_stats *msg =
			new cofmsg_stats(
					ctl->get_version(),
					xid,
					stats_type,
					flags,
					body,
					bodylen);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_desc_stats_reply(
	crofctl *ctl,
	uint32_t xid,
	cofdesc_stats_reply const& desc_stats,
	bool more)
{
	uint16_t flags = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: flags |= (more) ? openflow10::OFPSF_REPLY_MORE : 0; break;
	case openflow12::OFP_VERSION: flags |= (more) ? openflow12::OFPSF_REPLY_MORE : 0; break;
	case openflow13::OFP_VERSION: flags |= (more) ? openflow13::OFPSF_REPLY_MORE : 0; break;
	default:
		throw eBadVersion();
	}

	cofmsg_desc_stats_reply *msg =
			new cofmsg_desc_stats_reply(
					ctl->get_version(),
					xid,
					flags,
					desc_stats);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_table_stats_reply(
	crofctl *ctl,
	uint32_t xid,
	std::vector<coftable_stats_reply> const& table_stats,
	bool more)
{
	uint16_t flags = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: flags |= (more) ? openflow10::OFPSF_REPLY_MORE : 0; break;
	case openflow12::OFP_VERSION: flags |= (more) ? openflow12::OFPSF_REPLY_MORE : 0; break;
	case openflow13::OFP_VERSION: flags |= (more) ? openflow13::OFPSF_REPLY_MORE : 0; break;
	default:
		throw eBadVersion();
	}

	cofmsg_table_stats_reply *msg =
			new cofmsg_table_stats_reply(
					ctl->get_version(),
					xid,
					flags,
					table_stats);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_port_stats_reply(
	crofctl *ctl,
	uint32_t xid,
	std::vector<cofport_stats_reply> const& port_stats,
	bool more)
{
	uint16_t flags = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: flags |= (more) ? openflow10::OFPSF_REPLY_MORE : 0; break;
	case openflow12::OFP_VERSION: flags |= (more) ? openflow12::OFPSF_REPLY_MORE : 0; break;
	case openflow13::OFP_VERSION: flags |= (more) ? openflow13::OFPSF_REPLY_MORE : 0; break;
	default:
		throw eBadVersion();
	}

	cofmsg_port_stats_reply *msg =
			new cofmsg_port_stats_reply(
					ctl->get_version(),
					xid,
					flags,
					port_stats);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_queue_stats_reply(
		crofctl *ctl,
		uint32_t xid,
		std::vector<cofqueue_stats_reply> const& queue_stats,
		bool more)
{
	uint16_t flags = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: flags |= (more) ? openflow10::OFPSF_REPLY_MORE : 0; break;
	case openflow12::OFP_VERSION: flags |= (more) ? openflow12::OFPSF_REPLY_MORE : 0; break;
	case openflow13::OFP_VERSION: flags |= (more) ? openflow13::OFPSF_REPLY_MORE : 0; break;
	default:
		throw eBadVersion();
	}

	cofmsg_queue_stats_reply *msg =
			new cofmsg_queue_stats_reply(
					ctl->get_version(),
					xid,
					flags,
					queue_stats);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_flow_stats_reply(
	crofctl *ctl,
	uint32_t xid,
	std::vector<cofflow_stats_reply> const& flow_stats,
	bool more)
{
	uint16_t flags = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: flags |= (more) ? openflow10::OFPSF_REPLY_MORE : 0; break;
	case openflow12::OFP_VERSION: flags |= (more) ? openflow12::OFPSF_REPLY_MORE : 0; break;
	case openflow13::OFP_VERSION: flags |= (more) ? openflow13::OFPSF_REPLY_MORE : 0; break;
	default:
		throw eBadVersion();
	}

	cofmsg_flow_stats_reply *msg =
			new cofmsg_flow_stats_reply(
					ctl->get_version(),
					xid,
					flags,
					flow_stats);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_aggr_stats_reply(
	crofctl *ctl,
	uint32_t xid,
	cofaggr_stats_reply const& aggr_stats,
	bool more)
{
	uint16_t flags = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: flags |= (more) ? openflow10::OFPSF_REPLY_MORE : 0; break;
	case openflow12::OFP_VERSION: flags |= (more) ? openflow12::OFPSF_REPLY_MORE : 0; break;
	case openflow13::OFP_VERSION: flags |= (more) ? openflow13::OFPSF_REPLY_MORE : 0; break;
	default:
		throw eBadVersion();
	}

	cofmsg_aggr_stats_reply *msg =
			new cofmsg_aggr_stats_reply(
					ctl->get_version(),
					xid,
					flags,
					aggr_stats);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_group_stats_reply(
	crofctl *ctl,
	uint32_t xid,
	std::vector<cofgroup_stats_reply> const& group_stats,
	bool more)
{
	uint16_t flags = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: flags |= (more) ? openflow10::OFPSF_REPLY_MORE : 0; break;
	case openflow12::OFP_VERSION: flags |= (more) ? openflow12::OFPSF_REPLY_MORE : 0; break;
	case openflow13::OFP_VERSION: flags |= (more) ? openflow13::OFPSF_REPLY_MORE : 0; break;
	default:
		throw eBadVersion();
	}

	cofmsg_group_stats_reply *msg =
			new cofmsg_group_stats_reply(
					ctl->get_version(),
					xid,
					flags,
					group_stats);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_group_desc_stats_reply(
	crofctl *ctl,
	uint32_t xid,
	std::vector<cofgroup_desc_stats_reply> const& group_desc_stats,
	bool more)
{
	uint16_t flags = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: flags |= (more) ? openflow10::OFPSF_REPLY_MORE : 0; break;
	case openflow12::OFP_VERSION: flags |= (more) ? openflow12::OFPSF_REPLY_MORE : 0; break;
	case openflow13::OFP_VERSION: flags |= (more) ? openflow13::OFPSF_REPLY_MORE : 0; break;
	default:
		throw eBadVersion();
	}

	cofmsg_group_desc_stats_reply *msg =
			new cofmsg_group_desc_stats_reply(
					ctl->get_version(),
					xid,
					flags,
					group_desc_stats);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_group_features_stats_reply(
	crofctl *ctl,
	uint32_t xid,
	cofgroup_features_stats_reply const& group_features_stats,
	bool more)
{
	uint16_t flags = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: flags |= (more) ? openflow10::OFPSF_REPLY_MORE : 0; break;
	case openflow12::OFP_VERSION: flags |= (more) ? openflow12::OFPSF_REPLY_MORE : 0; break;
	case openflow13::OFP_VERSION: flags |= (more) ? openflow13::OFPSF_REPLY_MORE : 0; break;
	default:
		throw eBadVersion();
	}

	cofmsg_group_features_stats_reply *msg =
			new cofmsg_group_features_stats_reply(
					ctl->get_version(),
					xid,
					flags,
					group_features_stats);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_experimenter_stats_reply(
		crofctl *ctl,
		uint32_t xid,
		uint32_t exp_id,
		uint32_t exp_type,
		cmemory const& body,
		bool more)
{
	uint16_t flags = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: flags |= (more) ? openflow10::OFPSF_REPLY_MORE : 0; break;
	case openflow12::OFP_VERSION: flags |= (more) ? openflow12::OFPSF_REPLY_MORE : 0; break;
	case openflow13::OFP_VERSION: flags |= (more) ? openflow13::OFPSF_REPLY_MORE : 0; break;
	default:
		throw eBadVersion();
	}

	cofmsg_experimenter_stats_reply *msg =
			new cofmsg_experimenter_stats_reply(
					ctl->get_version(),
					xid,
					flags,
					exp_id,
					exp_type,
					body);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}




/*
 * SET-CONFIG message
 */


void
crofbase::send_set_config_message(
	crofdpt *dpt,
	uint16_t flags,
	uint16_t miss_send_len)
{
	cofmsg_set_config *msg =
			new cofmsg_set_config(
					dpt->get_version(),
					ta_new_async_xid(),
					flags,
					miss_send_len);

	dpt_find(dpt)->send_message(msg);
}





/*
 * PACKET-OUT message
 */


void
crofbase::send_packet_out_message(
	crofdpt *dpt,
	uint32_t buffer_id,
	uint32_t in_port,
	cofactions& aclist,
	uint8_t *data,
	size_t datalen)
{
	cofmsg_packet_out *msg =
			new cofmsg_packet_out(
					dpt->get_version(),
					ta_new_async_xid(),
					buffer_id,
					in_port,
					aclist,
					data,
					datalen);

	msg->pack();

	dpt_find(dpt)->send_message(msg);
}



/*
 * PACKET-IN message
 */


void
crofbase::send_packet_in_message(
	crofctl *ctl,
	uint32_t buffer_id,
	uint16_t total_len,
	uint8_t reason,
	uint8_t table_id,
	uint64_t cookie,
	uint16_t in_port, // for OF 1.0
	cofmatch& match,
	uint8_t* data,
	size_t datalen)
{
	try {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_packet_in_message() "
				"ofctrl_list.size()=%d", this, ofctl_set.size());

		cpacket n_pack(data, datalen, match.get_in_port());

		if (0 != ctl) { // cofctl instance was specified

			if (ofctl_set.find(ctl) == ofctl_set.end()) {
				throw eRofBaseNotConnected();
			}

			cofmsg_packet_in *pack =
					new cofmsg_packet_in(
							ctl->get_version(),
							ta_new_async_xid(),
							buffer_id,
							total_len,
							reason,
							table_id,
							cookie,
							in_port, /* in_port for OF1.0 */
							match,
							data,
							datalen);

			logging::debug << "sending Packet-In message " << std::endl << *pack << std::endl;

			pack->pack();

			ctl_find(ctl)->send_message(pack);

		} else if (fe_flags.test(NSP_ENABLED)) { //cofctl was not specified and flowspace registration is enabled

			std::set<cfspentry*> nse_list;

			nse_list = fsptable.find_matching_entries(match.get_in_port(), total_len, n_pack);

			WRITELOG(CROFBASE, DBG, "crofbase(%p) nse_list.size()=%d", this, nse_list.size());

			if (nse_list.empty()) {
				throw eRofBaseNotConnected();
			}

			bool no_active_ctl = true;

			uint32_t role_is_slave = 0;
			switch (ctl->get_version()) {
			case openflow12::OFP_VERSION: role_is_slave = openflow12::OFPCR_ROLE_SLAVE; break;
			case openflow13::OFP_VERSION: role_is_slave = openflow13::OFPCR_ROLE_SLAVE; break;
			default:
				throw eBadVersion();
			}

			for (std::set<cfspentry*>::iterator
					it = nse_list.begin(); it != nse_list.end(); ++it)
			{
				crofctl *ctl = dynamic_cast<crofctl*>( (*nse_list.begin())->fspowner );
				if (role_is_slave == ctl->get_role()) {
					// entity has role slave
					continue;
				}

				if (not ctl->is_established()) {
					continue;
				}

				no_active_ctl = false;

				cofmsg_packet_in *pack =
						new cofmsg_packet_in(
								ctl->get_version(),
								ta_new_async_xid(),
								buffer_id,
								total_len,
								reason,
								table_id,
								cookie,
								in_port, /* in_port for OF1.0 */
								match,
								data,
								datalen);

				logging::debug << "sending Packet-In message " << std::endl << *pack << std::endl;

				pack->pack();

				// straight call to layer-(n+1) entity's fe_up_packet_in() method
				ctl_find(ctl)->send_message(pack);
			}

			if (no_active_ctl) {
				throw eRofBaseNotConnected();
			}

			return;

		} else { // cofctl was not specified and there is no flowspace registration active

			if (ofctl_set.empty()) {
				throw eRofBaseNotConnected();
			}

			bool no_active_ctl = true;

			for (std::set<crofctl*>::iterator it = ofctl_set.begin(); it != ofctl_set.end(); ++it) {

				if (not (*it)->is_established()) {
					continue;
				}

				if ((*it)->is_slave()) {
					continue;
				}

				no_active_ctl = false;

				cofmsg_packet_in *pack =
						new cofmsg_packet_in(
								(*it)->get_version(),
								ta_new_async_xid(),
								buffer_id,
								total_len,
								reason,
								table_id,
								cookie,
								in_port, /* in_port for OF1.0 */
								match,
								data,
								datalen);

				logging::debug << "sending Packet-In message " << std::endl << *pack << std::endl;

				pack->pack();

				// straight call to layer-(n+1) entity's fe_up_packet_in() method
				ctl_find(*it)->send_message(pack);
			}

			if (no_active_ctl) {
				throw eRofBaseNotConnected();
			}
		}

	} catch (eFspNoMatch& e) {

		cpacket pack(data, datalen);

		logging::warn << "no ctl found for Packet-In" << std::endl << pack << std::endl;

	} catch (eRofBaseNotFound& e) {

		cpacket pack(data, datalen);

		logging::warn << "no ctl found for Packet-In" << std::endl << pack << std::endl;

	}
}




/*
 * BARRIER request/reply
 */

uint32_t
crofbase::send_barrier_request(crofdpt *dpt)
{
	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_BARRIER_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_BARRIER_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_BARRIER_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_barrier_request *msg =
			new cofmsg_barrier_request(
					dpt->get_version(),
					ta_add_request(msg_type));

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



void
crofbase::send_barrier_reply(
		crofctl* ctl,
		uint32_t xid)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_barrier_reply()", this);

	cofmsg_barrier_reply *msg = (cofmsg_barrier_reply*)0;

	switch (ctl->get_version()) {
	case OFP10_VERSION: {
		msg = new cofmsg_barrier_reply(
				ctl->get_version(),
				xid);
	} break;
	case OFP12_VERSION: {
		msg = new cofmsg_barrier_reply(
				ctl->get_version(),
				xid);
	} break;
	case OFP13_VERSION: {
		msg = new cofmsg_barrier_reply(
				ctl->get_version(),
				xid);
	} break;
	default:
		throw eBadVersion();
	}

	ctl_find(ctl)->send_message(msg);
}





/*
 * ROLE.request/reply
 */

uint32_t
crofbase::send_role_request(
	crofdpt *dpt,
	uint32_t role,
	uint64_t generation_id)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_role_request()", this);

	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_ROLE_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_ROLE_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_role_request *msg =
			new cofmsg_role_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					role,
					generation_id);

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



void
crofbase::send_role_reply(
		crofctl *ctl,
		uint32_t xid,
		uint32_t role,
		uint64_t generation_id)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_role_reply()", this);

	cofmsg_role_reply *msg =
			new cofmsg_role_reply(
					ctl->get_version(),
					xid,
					role,
					generation_id);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}







/*
 * ERROR message
 */


void
crofbase::send_error_message(
	crofctl *ctl,
	uint32_t xid,
	uint16_t type,
	uint16_t code,
	uint8_t* data,
	size_t datalen)
{
	WRITELOG(CROFBASE, DBG, "crofbase::send_error_message()");

	xid = (xid == 0) ? ta_new_async_xid() : xid;

	if (0 != ctl)
	{
		cofmsg_error *pack =
				new cofmsg_error(
						ctl->get_version(),
						xid,
						type,
						code,
						data, datalen);

		// straight call to layer-(n+1) entity's fe_up_packet_in() method
		ctl_find(ctl)->send_message(pack);
	}
	else
	{
		for (std::set<crofctl*>::iterator
				it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
		{
			if (not (*it)->is_established()) {
				continue;
			}
			cofmsg_error *pack =
					new cofmsg_error(
							(*it)->get_version(),
							xid,
							type,
							code,
							data, datalen);

			(*it)->send_message(pack);
		}
	}
}



void
crofbase::send_error_message(
	crofdpt *dpt,
	uint32_t xid,
	uint16_t type,
	uint16_t code,
	uint8_t* data,
	size_t datalen)
{
	WRITELOG(CROFBASE, DBG, "crofbase::send_error_message()");

	xid = (xid == 0) ? ta_new_async_xid() : xid;

	if (0 != dpt)
	{
		cofmsg_error *pack =
				new cofmsg_error(
						dpt->get_version(),
						xid,
						type,
						code,
						data, datalen);

		// straight call to layer-(n+1) entity's fe_up_packet_in() method
		dpt_find(dpt)->send_message(pack);
	}
	else
	{
		for (std::set<crofdpt*>::iterator
				it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
		{
			cofmsg_error *pack =
					new cofmsg_error(
							(*it)->get_version(),
							xid,
							type,
							code,
							data, datalen);

			(*it)->send_message(pack);
		}
	}
}



void
crofbase::send_error_bad_request_bad_len(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_REQUEST; code = openflow10::OFPBRC_BAD_LEN;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_BAD_LEN;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_BAD_LEN;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/BadLen for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_bad_version(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_REQUEST; code = openflow10::OFPBRC_BAD_VERSION;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_BAD_VERSION;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_BAD_VERSION;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/BadVersion for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_bad_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_REQUEST; code = openflow10::OFPBRC_BAD_TYPE;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_BAD_TYPE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_BAD_TYPE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/BadType for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_bad_stat(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_REQUEST; code = openflow10::OFPBRC_BAD_STAT;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_BAD_STAT;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_BAD_STAT;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/BadStat for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_bad_experimenter(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_REQUEST; code = openflow10::OFPBRC_BAD_VENDOR;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_BAD_EXPERIMENTER;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_BAD_EXPERIMENTER;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/BadExperimenter for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_bad_exp_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_REQUEST; code = openflow10::OFPBRC_BAD_SUBTYPE;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_BAD_EXP_TYPE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_BAD_EXP_TYPE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/BadExpType for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_REQUEST; code = openflow10::OFPBRC_EPERM;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_EPERM;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_EPERM;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/EPerm for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_buffer_empty(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_REQUEST; code = openflow10::OFPBRC_BUFFER_EMPTY;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_BUFFER_EMPTY;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_BUFFER_EMPTY;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/BufferEmpty for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_buffer_unknown(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_REQUEST; code = openflow10::OFPBRC_BUFFER_UNKNOWN;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_BUFFER_UNKNOWN;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_BUFFER_UNKNOWN;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/BufferUnknown for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_bad_table_id(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_BAD_TABLE_ID;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_BAD_TABLE_ID;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/BadTableId for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_is_slave(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_IS_SLAVE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_IS_SLAVE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/IsSlave for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_bad_port(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_BAD_PORT;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_BAD_PORT;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/BadPort for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_request_bad_packet(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_REQUEST; code = openflow12::OFPBRC_BAD_PACKET;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_REQUEST; code = openflow13::OFPBRC_BAD_PACKET;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadRequest/BadPacket for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_bad_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_ACTION; code = openflow10::OFPBAC_BAD_TYPE;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_BAD_TYPE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_BAD_TYPE;
	} break;
	default:
		logging::warn << "[rofl][crofbase] cannot send BadAction/BadType for ofp-version:" << (int)ctl->get_version() << std::endl;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_bad_len(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_ACTION; code = openflow10::OFPBAC_BAD_LEN;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_BAD_LEN;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_BAD_LEN;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/BadLen for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_bad_experimenter(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_ACTION; code = openflow10::OFPBAC_BAD_VENDOR;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_BAD_EXPERIMENTER;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_BAD_EXPERIMENTER;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/BadExperimenter for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_bad_experimenter_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_ACTION; code = openflow10::OFPBAC_BAD_VENDOR_TYPE;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_BAD_EXPERIMENTER_TYPE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_BAD_EXPERIMENTER_TYPE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/BadExperimenter for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_bad_out_port(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_ACTION; code = openflow10::OFPBAC_BAD_OUT_PORT;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_BAD_OUT_PORT;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_BAD_OUT_PORT;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/BadOutPort for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_bad_argument(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_ACTION; code = openflow10::OFPBAC_BAD_ARGUMENT;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_BAD_ARGUMENT;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_BAD_ARGUMENT;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/BadArgument for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_ACTION; code = openflow10::OFPBAC_EPERM;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_EPERM;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_EPERM;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/EPerm for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_too_many(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_ACTION; code = openflow10::OFPBAC_TOO_MANY;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_TOO_MANY;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_TOO_MANY;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/TooMany for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_bad_queue(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_BAD_ACTION; code = openflow10::OFPBAC_BAD_QUEUE;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_BAD_QUEUE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_BAD_QUEUE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/BadQueue for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_bad_out_group(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_BAD_OUT_GROUP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_BAD_OUT_GROUP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/BadOutGroup for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_match_inconsistent(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_MATCH_INCONSISTENT;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_MATCH_INCONSISTENT;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/MatchIncońsistent for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_unsupported_order(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_UNSUPPORTED_ORDER;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_UNSUPPORTED_ORDER;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/UnsupportedOrder for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_action_bad_tag(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_ACTION; code = openflow12::OFPBAC_BAD_TAG;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_ACTION; code = openflow13::OFPBAC_BAD_TAG;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadAction/BadTag for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_inst_unknown_inst(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_INSTRUCTION; code = openflow12::OFPBIC_UNKNOWN_INST;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_INSTRUCTION; code = openflow13::OFPBIC_UNKNOWN_INST;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadInst/UnknownInst for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_inst_unsup_inst(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_INSTRUCTION; code = openflow12::OFPBIC_UNSUP_INST;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_INSTRUCTION; code = openflow13::OFPBIC_UNSUP_INST;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadInst/UnsupInst for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_inst_bad_table_id(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_INSTRUCTION; code = openflow12::OFPBIC_BAD_TABLE_ID;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_INSTRUCTION; code = openflow13::OFPBIC_BAD_TABLE_ID;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadInst/BadTableId for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_inst_unsup_metadata(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_INSTRUCTION; code = openflow12::OFPBIC_UNSUP_METADATA;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_INSTRUCTION; code = openflow13::OFPBIC_UNSUP_METADATA;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadInst/UnsupportedMetadata for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_inst_unsup_metadata_mask(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_INSTRUCTION; code = openflow12::OFPBIC_UNSUP_METADATA_MASK;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_INSTRUCTION; code = openflow13::OFPBIC_UNSUP_METADATA_MASK;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadInst/UnsupportedMetadataMask for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_inst_unsup_exp_inst(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_INSTRUCTION; code = openflow12::OFPBIC_UNSUP_EXP_INST;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_INSTRUCTION; code = openflow13::OFPBIC_UNSUP_EXP_INST;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadInst/UnsupExpInst for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_match_bad_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_MATCH; code = openflow12::OFPBMC_BAD_TYPE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_MATCH; code = openflow13::OFPBMC_BAD_TYPE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadMatch/BadType for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_match_bad_len(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_MATCH; code = openflow12::OFPBMC_BAD_LEN;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_MATCH; code = openflow13::OFPBMC_BAD_LEN;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadMatch/BadLen for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_match_bad_tag(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_MATCH; code = openflow12::OFPBMC_BAD_TAG;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_MATCH; code = openflow13::OFPBMC_BAD_TAG;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadMatch/BadTag for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_match_bad_dladdr_mask(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_MATCH; code = openflow12::OFPBMC_BAD_DL_ADDR_MASK;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_MATCH; code = openflow13::OFPBMC_BAD_DL_ADDR_MASK;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadMatch/BadDlAddrMask for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_match_bad_nwaddr_mask(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_MATCH; code = openflow12::OFPBMC_BAD_NW_ADDR_MASK;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_MATCH; code = openflow13::OFPBMC_BAD_NW_ADDR_MASK;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadMatch/BadNwAddrMask for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_match_bad_wildcards(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_MATCH; code = openflow12::OFPBMC_BAD_WILDCARDS;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_MATCH; code = openflow13::OFPBMC_BAD_WILDCARDS;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadMatch/BadWildcards for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_match_bad_field(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_MATCH; code = openflow12::OFPBMC_BAD_FIELD;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_MATCH; code = openflow13::OFPBMC_BAD_FIELD;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadMatch/BadField for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_bad_match_bad_value(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_BAD_MATCH; code = openflow12::OFPBMC_BAD_VALUE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_BAD_MATCH; code = openflow13::OFPBMC_BAD_VALUE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send BadMatch/BadValue for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_hello_failed_incompatible(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_HELLO_FAILED; code = openflow12::OFPHFC_INCOMPATIBLE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_HELLO_FAILED; code = openflow13::OFPHFC_INCOMPATIBLE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send HelloFailed/Incompatible for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_hello_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_HELLO_FAILED; code = openflow12::OFPHFC_EPERM;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_HELLO_FAILED; code = openflow13::OFPHFC_EPERM;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send HelloFailed/EPerm for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_switch_config_failed_bad_flags(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_SWITCH_CONFIG_FAILED; code = openflow12::OFPSCFC_BAD_FLAGS;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_SWITCH_CONFIG_FAILED; code = openflow13::OFPSCFC_BAD_FLAGS;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send SwitchConfigFailed/BadFlags for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_switch_config_failed_bad_len(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_SWITCH_CONFIG_FAILED; code = openflow12::OFPSCFC_BAD_LEN;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_SWITCH_CONFIG_FAILED; code = openflow13::OFPSCFC_BAD_LEN;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send SwitchConfigFailed/BadLen for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_unknown(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_UNKNOWN;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_UNKNOWN;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/Unknown for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_table_full(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_FLOW_MOD_FAILED; code = openflow10::OFPFMFC_ALL_TABLES_FULL;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_TABLE_FULL;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_TABLE_FULL;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/TableFull for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_bad_table_id(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_BAD_TABLE_ID;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_BAD_TABLE_ID;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/BadTableId for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_overlap(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_FLOW_MOD_FAILED; code = openflow10::OFPFMFC_OVERLAP;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_OVERLAP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_OVERLAP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/Overlap for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_FLOW_MOD_FAILED; code = openflow10::OFPFMFC_EPERM;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_EPERM;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_EPERM;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/EPerm for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_bad_timeout(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_FLOW_MOD_FAILED; code = openflow10::OFPFMFC_BAD_EMERG_TIMEOUT;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_BAD_TIMEOUT;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_BAD_TIMEOUT;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/BadTimeout for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_flow_mod_failed_bad_command(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow10::OFP_VERSION: {
		type = openflow10::OFPET_FLOW_MOD_FAILED; code = openflow10::OFPFMFC_BAD_COMMAND;
	} break;
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_FLOW_MOD_FAILED; code = openflow12::OFPFMFC_BAD_COMMAND;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_FLOW_MOD_FAILED; code = openflow13::OFPFMFC_BAD_COMMAND;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send FlowModFailed/BadCommand for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_group_exists(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_GROUP_EXISTS;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_GROUP_EXISTS;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/GroupExists for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_inval_group(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_INVALID_GROUP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_INVALID_GROUP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/InvalGroup for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_weight_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_WEIGHT_UNSUPPORTED;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_WEIGHT_UNSUPPORTED;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/WeightUnsupported for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_out_of_groups(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_OUT_OF_GROUPS;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_OUT_OF_GROUPS;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/OutOfGroups for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_out_of_buckets(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_OUT_OF_BUCKETS;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_OUT_OF_BUCKETS;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/OutOfBuckets for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_chaining_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_CHAINING_UNSUPPORTED;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_CHAINING_UNSUPPORTED;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/ChainingUnsupported for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_watch_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_WATCH_UNSUPPORTED;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_WATCH_UNSUPPORTED;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/WatchUnsupported for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_loop(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_LOOP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_LOOP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/Loop for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_unknown_group(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_UNKNOWN_GROUP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_UNKNOWN_GROUP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/UnknownGroup for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_chained_group(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_CHAINED_GROUP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_CHAINED_GROUP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/ChainedGroup for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_bad_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_BAD_TYPE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_BAD_TYPE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/BadType for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_bad_command(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_BAD_COMMAND;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_BAD_COMMAND;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/BadCommand for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_bad_bucket(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_BAD_BUCKET;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_BAD_BUCKET;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/BadBucket for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_bad_watch(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_BAD_WATCH;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_BAD_WATCH;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/BadWatch for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_group_mod_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_GROUP_MOD_FAILED; code = openflow12::OFPGMFC_EPERM;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_GROUP_MOD_FAILED; code = openflow13::OFPGMFC_EPERM;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send GroupModFailed/EPerm for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_port_mod_failed_bad_port(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_PORT_MOD_FAILED; code = openflow12::OFPPMFC_BAD_PORT;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_PORT_MOD_FAILED; code = openflow13::OFPPMFC_BAD_PORT;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send PortModFailed/BadPort for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_port_mod_failed_bad_hw_addr(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_PORT_MOD_FAILED; code = openflow12::OFPPMFC_BAD_HW_ADDR;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_PORT_MOD_FAILED; code = openflow13::OFPPMFC_BAD_HW_ADDR;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send PortModFailed/BadHwAddr for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_port_mod_failed_bad_config(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_PORT_MOD_FAILED; code = openflow12::OFPPMFC_BAD_CONFIG;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_PORT_MOD_FAILED; code = openflow13::OFPPMFC_BAD_CONFIG;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send PortModFailed/BadConfig for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_port_mod_failed_bad_advertise(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_PORT_MOD_FAILED; code = openflow12::OFPPMFC_BAD_ADVERTISE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_PORT_MOD_FAILED; code = openflow13::OFPPMFC_BAD_ADVERTISE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send PortModFailed/BadAdvertise for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_table_mod_failed_bad_table(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_TABLE_MOD_FAILED; code = openflow12::OFPTMFC_BAD_TABLE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_TABLE_MOD_FAILED; code = openflow13::OFPTMFC_BAD_TABLE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send TableModFailed/BadTable for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_table_mod_failed_bad_config(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_TABLE_MOD_FAILED; code = openflow12::OFPTMFC_BAD_CONFIG;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_TABLE_MOD_FAILED; code = openflow13::OFPTMFC_BAD_CONFIG;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send TableModFailed/BadConfig for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_role_request_failed_stale(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_ROLE_REQUEST_FAILED; code = openflow12::OFPRRFC_STALE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_ROLE_REQUEST_FAILED; code = openflow13::OFPRRFC_STALE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send RoleRequestFailed/Stale for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_role_request_failed_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_ROLE_REQUEST_FAILED; code = openflow12::OFPRRFC_UNSUP;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_ROLE_REQUEST_FAILED; code = openflow13::OFPRRFC_UNSUP;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send RoleRequestFailed/Unsupported for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_role_request_failed_bad_role(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (ctl->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_ROLE_REQUEST_FAILED; code = openflow12::OFPRRFC_BAD_ROLE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_ROLE_REQUEST_FAILED; code = openflow13::OFPRRFC_BAD_ROLE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send RoleRequestFailed/BadRole for ofp-version:" << (int)ctl->get_version() << std::endl;
	} return;
	}

	send_error_message(ctl, xid, type, code, data, datalen);
}



void
crofbase::send_error_hello_failed_incompatible(
		crofdpt *dpt,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (dpt->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_HELLO_FAILED; code = openflow12::OFPHFC_INCOMPATIBLE;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_HELLO_FAILED; code = openflow13::OFPHFC_INCOMPATIBLE;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send HelloFailed/Incompatible for ofp-version:" << (int)dpt->get_version() << std::endl;
	} return;
	}

	send_error_message(dpt, xid, type, code, data, datalen);
}



void
crofbase::send_error_hello_failed_eperm(
		crofdpt *dpt,
		uint32_t xid,
		uint8_t* data,
		size_t datalen)
{
	uint16_t type = 0, code = 0;

	switch (dpt->get_version()) {
	case openflow12::OFP_VERSION: {
		type = openflow12::OFPET_HELLO_FAILED; code = openflow12::OFPHFC_EPERM;
	} break;
	case openflow13::OFP_VERSION: {
		type = openflow13::OFPET_HELLO_FAILED; code = openflow13::OFPHFC_EPERM;
	} break;
	default: {
		logging::warn << "[rofl][crofbase] cannot send HelloFailed/EPerm for ofp-version:" << (int)dpt->get_version() << std::endl;
	} return;
	}

	send_error_message(dpt, xid, type, code, data, datalen);
}










/*
 * FLOW-MOD message
 */


void
crofbase::send_flow_mod_message(
	crofdpt *dpt,
	cofmatch& ofmatch,
	uint64_t cookie,
	uint64_t cookie_mask,
	uint8_t table_id,
	uint8_t command,
	uint16_t idle_timeout,
	uint16_t hard_timeout,
	uint16_t priority,
	uint32_t buffer_id,
	uint32_t out_port,
	uint32_t out_group,
	uint16_t flags,
	cofinstructions& inlist)
{
	cofmsg_flow_mod *pack =
			new cofmsg_flow_mod(
					dpt->get_version(),
					ta_new_async_xid(),
					cookie,
					cookie_mask,
					table_id,
					command,
					idle_timeout,
					hard_timeout,
					priority,
					buffer_id,
					out_port,
					out_group,
					flags,
					inlist,
					ofmatch);

	pack->pack();

	logging::debug << "send Flow-Mod message" << std::endl << pack << std::endl;

	dpt_find(dpt)->send_message(pack);
}



void
crofbase::send_flow_mod_message(
		crofdpt *dpt,
		cflowentry& fe)
{
	cofmsg_flow_mod *pack =
			new cofmsg_flow_mod(
					dpt->get_version(),
					ta_new_async_xid(),
					fe.get_cookie(),
					fe.get_cookie_mask(),
					fe.get_table_id(),
					fe.get_command(),
					fe.get_idle_timeout(),
					fe.get_hard_timeout(),
					fe.get_priority(),
					fe.get_buffer_id(),
					fe.get_out_port(),
					fe.get_out_group(),
					fe.get_flags(),
					fe.instructions,
					fe.match);

	pack->pack();

	logging::debug << "sending Flow-Mod message" << std::endl << *pack << std::endl;

	dpt_find(dpt)->send_message(pack);
}



/*
 * GROUP-MOD message
 */


void
crofbase::send_group_mod_message(
		crofdpt *dpt,
		cgroupentry& ge)
{
	cofmsg_group_mod *pack =
			new cofmsg_group_mod(
					dpt->get_version(),
					ta_new_async_xid(),
					be16toh(ge.group_mod->command),
					ge.group_mod->type,
					be32toh(ge.group_mod->group_id),
					ge.buckets);

	pack->pack();

	logging::debug << "sending Group-Mod message" << std::endl << *pack << std::endl;

	dpt_find(dpt)->send_message(pack);
}



/*
 * PORT-MOD message
 */


void
crofbase::send_port_mod_message(
	crofdpt *dpt,
	uint32_t port_no,
	cmacaddr const& hwaddr,
	uint32_t config,
	uint32_t mask,
	uint32_t advertise)
{
	cofmsg_port_mod *pack =
			new cofmsg_port_mod(
					dpt->get_version(),
					ta_new_async_xid(),
					port_no,
					hwaddr,
					config,
					mask,
					advertise);

	pack->pack();

	logging::debug << "sending Port-Mod message" << std::endl << pack << std::endl;

	dpt_find(dpt)->send_message(pack);
}




/*
 * TABLE-MOD message
 */


void
crofbase::send_table_mod_message(
		crofdpt *dpt,
		uint8_t table_id,
		uint32_t config)
{
	cofmsg_table_mod *pack =
			new cofmsg_table_mod(
						dpt->get_version(),
						ta_new_async_xid(),
						table_id,
						config);

	pack->pack();

	logging::debug << "sending Table-Mod message" << std::endl << pack << std::endl;

	dpt_find(dpt)->send_message(pack);
}




/*
 * FLOW-REMOVED message
 */


void
crofbase::send_flow_removed_message(
	crofctl *ctl,
	cofmatch& ofmatch,
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
	try {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_removed_message()", this);

		//ofctrl_exists(ofctrl);

		for (std::set<crofctl*>::iterator
				it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
		{
			if (not (*it)->is_established()) {
				continue;
			}
			crofctl *ofctrl = (*it);

			if (ofctrl->is_slave())
			{
				WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_removed_message() ofctrl:%p is SLAVE", this, ofctrl);
				continue;
			}


			cofmsg_flow_removed *pack =
					new cofmsg_flow_removed(
							ofctrl->get_version(),
							ta_new_async_xid(),
							cookie,
							priority,
							reason,
							table_id,
							duration_sec,
							duration_nsec,
							idle_timeout,
							hard_timeout,
							packet_count,
							byte_count,
							ofmatch);

			pack->pack();

			logging::debug << "send Flow-Removed message" << std::endl << pack << std::endl;

			// straight call to layer-(n+1) entity's fe_up_packet_in() method
			ctl_find(ofctrl)->send_message(pack);
		}

	} catch (eRofBaseNotFound& e) {

		WRITELOG(CROFBASE, ERROR, "crofbase(%p)::send_flow_removed_message() cofctrl instance not found", this);

	}
}






/*
 * PORT-STATUS message
 */

void
crofbase::send_port_status_message(
	crofctl *ctl,
	uint8_t reason,
	cofport const& port)
{
	if (0 != ctl) {

		if (ofctl_set.find(ctl) == ofctl_set.end()) {
			throw eRofBaseNotConnected();
		}

		cofmsg_port_status *pack =
				new cofmsg_port_status(
							ctl->get_version(),
							ta_new_async_xid(),
							reason,
							port);

		pack->pack();

		logging::debug << "sending PortStatus message" << std::endl << pack << std::endl;

		(ctl)->send_message(pack);

	} else {

		std::map<cofbase*, crofctl*>::iterator it;
		for (std::set<crofctl*>::iterator
				it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
		{
			if (not (*it)->is_established()) {
				continue;
			}

			cofmsg_port_status *pack =
					new cofmsg_port_status(
								(*it)->get_version(),
								ta_new_async_xid(),
								reason,
								port);

			pack->pack();

			logging::debug << "sending PortStatus message" << std::endl << pack << std::endl;

			(*it)->send_message(pack);
		}
	}
}







/*
 * QUEUE-GET-CONFIG request/reply
 */

uint32_t
crofbase::send_queue_get_config_request(
	crofdpt *dpt,
	uint32_t port)
{
	uint32_t xid = 0;

	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow10::OFP_VERSION: msg_type = openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST; break;
	case openflow12::OFP_VERSION: msg_type = openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST; break;
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_queue_get_config_request *msg =
			new cofmsg_queue_get_config_request(
					dpt->get_version(),
					ta_add_request(msg_type),
					port);

	xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



void
crofbase::send_queue_get_config_reply(
		crofctl *ctl,
		uint32_t xid,
		uint32_t portno,
		cofpacket_queue_list const& pql)
{
	WRITELOG(CROFBASE, DBG, "crofbase::send_queue_get_config_reply()");

	cofmsg_queue_get_config_reply *pack =
			new cofmsg_queue_get_config_reply(
					ctl->get_version(),
					xid,
					portno,
					pql);

	//std::cerr << *pack << std::endl;

	ctl_find(ctl)->send_message(pack);
}





/*
 * VENDOR messages
 */



uint32_t
crofbase::send_experimenter_message(
		crofdpt *dpt,
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen)
{
	uint32_t xid = 0;

	cofmsg_experimenter *msg =
			new cofmsg_experimenter(
						dpt->get_version(),
						ta_new_async_xid(),
						experimenter_id,
						exp_type,
						body,
						bodylen);

	msg->pack();

	logging::debug << "sending Experimenter message" << std::endl << *msg << std::endl;

	xid = msg->get_xid();

	if (NULL == dpt) // send to all attached data path entities
	{
		for (std::set<crofdpt*>::iterator
				it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
		{
			(*it)->send_message(new cofmsg(*msg));
		}
		delete msg;
	}
	else
	{
		dpt_find(dpt)->send_message(msg);
	}

	return xid;
}



uint32_t
crofbase::send_experimenter_message(
		crofctl *ctl,
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen)
{
	uint32_t xid = 0;

	cofmsg_experimenter *msg =
			new cofmsg_experimenter(
						ctl->get_version(),
						ta_new_async_xid(),
						experimenter_id,
						exp_type,
						body,
						bodylen);

	msg->pack();

	xid = msg->get_xid();

	logging::debug << "sending Experimenter message" << std::endl << *msg << std::endl;

	if ((crofctl*)0 == ctl) // send to all attached controller entities
	{
		for (std::set<crofctl*>::iterator
				it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
		{
			if (not (*it)->is_established()) {
				continue;
			}
			(*it)->send_message(new cofmsg(*msg));
		}

		delete msg;
	}
	else
	{
		// straight call to layer-(n+1) entity's fe_up_experimenter_message() method
		ctl->send_message(msg);
	}

	return xid;
}




/*
 * GET-ASYNC-CONFIG request/reply
 */
uint32_t
crofbase::send_get_async_config_request(
		crofdpt *dpt)
{
	uint32_t xid = 0;

	uint8_t msg_type = 0;

	switch (dpt->get_version()) {
	case openflow13::OFP_VERSION: msg_type = openflow13::OFPT_GET_ASYNC_REQUEST; break;
	default:
		throw eBadVersion();
	}

	cofmsg_get_async_config_request *msg =
			new cofmsg_get_async_config_request(
					dpt->get_version(),
					ta_add_request(msg_type));

	xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



void
crofbase::send_get_async_config_reply(
		crofctl *ctl,
		uint32_t xid,
		uint32_t packet_in_mask0,
		uint32_t packet_in_mask1,
		uint32_t port_status_mask0,
		uint32_t port_status_mask1,
		uint32_t flow_removed_mask0,
		uint32_t flow_removed_mask1)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_get_async_config_reply()", this);

	cofmsg_get_async_config_reply *msg =
			new cofmsg_get_async_config_reply(
					ctl->get_version(),
					xid,
					packet_in_mask0,
					packet_in_mask1,
					port_status_mask0,
					port_status_mask1,
					flow_removed_mask0,
					flow_removed_mask1);

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::handle_get_async_config_reply_timeout(crofdpt *dpt)
{

}



/*
 * SET-ASYNC-CONFIG message
 */


void
crofbase::send_set_async_config_message(
	crofdpt *dpt,
	uint32_t packet_in_mask0,
	uint32_t packet_in_mask1,
	uint32_t port_status_mask0,
	uint32_t port_status_mask1,
	uint32_t flow_removed_mask0,
	uint32_t flow_removed_mask1)
{
	cofmsg_set_async_config *msg =
			new cofmsg_set_async_config(
					dpt->get_version(),
					ta_new_async_xid(),
					packet_in_mask0,
					packet_in_mask1,
					port_status_mask0,
					port_status_mask1,
					flow_removed_mask0,
					flow_removed_mask1);

	dpt_find(dpt)->send_message(msg);
}











uint32_t
crofbase::ta_add_request(
		uint8_t type)
{
	uint32_t xid = ta_new_async_xid();

	RwLock lock(&xidlock, RwLock::RWLOCK_WRITE);

	// add pair(type, xid) to transaction list
	//ta_pending_reqs.insert(std::make_pair<uint32_t, uint8_t>(xid, type));
	ta_pending_reqs[xid] = type;

	WRITELOG(XID, DBG, "cofbase::ta_add_request() rand number=0x%x", xid);

#ifndef NDEBUG
	std::map<uint32_t, uint8_t>::iterator it;
	for (it = ta_pending_reqs.begin(); it != ta_pending_reqs.end(); ++it) {
		WRITELOG(XID, DBG, "cofbase::ta_pending_request: xid=0x%x type=%d",
				 (*it).first, (*it).second);
	}
#endif

	return xid;
}



void
crofbase::ta_rem_request(
		uint32_t xid)
{
	RwLock lock(&xidlock, RwLock::RWLOCK_WRITE);

	ta_pending_reqs.erase(xid);
	// this yields an exception if type wasn't stored in ta_pending_reqs
}



bool
crofbase::ta_pending(
		uint32_t xid, uint8_t type)
{
	RwLock lock(&xidlock, RwLock::RWLOCK_WRITE);

#ifndef NDEBUG
	std::map<uint32_t, uint8_t>::iterator it;
	for (it = ta_pending_reqs.begin(); it != ta_pending_reqs.end(); ++it) {
		WRITELOG(XID, DBG, "cofbase::ta_pending_request: xid=0x%x type=%d",
				 (*it).first, (*it).second);
	}

	WRITELOG(XID, DBG, "%s 0x%x %d %d",
			(ta_pending_reqs.find(xid) != ta_pending_reqs.end()) ? "true" : "false",
			xid, ta_pending_reqs[xid], (int)type);
#endif

	return((ta_pending_reqs[xid] == type) &&
		   (ta_pending_reqs.find(xid) != ta_pending_reqs.end()));
}



bool
crofbase::ta_active_xid(
		uint32_t xid)
{
	RwLock lock(&xidlock, RwLock::RWLOCK_READ);

	return(ta_pending_reqs.find(xid) != ta_pending_reqs.end());
}



uint32_t
crofbase::ta_new_async_xid()
{
#if 0
	int count = xid_used_max;
	// if xid_used is larger than xid_used_max, remove oldest entries
	while ((xids_used.size() > xid_used_max) && (--count)) {
		// do not remove xids from active transactions
		if (!ta_active_xid(xids_used.front()))
			xids_used.pop_front();
	}

	// allocate new xid not used before
	crandom r(sizeof(uint32_t));
	while (find(xids_used.begin(), xids_used.end(), r.uint32()) != xids_used.end())
		r.rand(sizeof(uint32_t));

	// store new allocated xid
	xids_used.push_back(r.uint32());

	return r.uint32();
#endif

	RwLock lock(&xidlock, RwLock::RWLOCK_WRITE);

	if (xids_used.size() > xid_used_max) {
		xids_used.clear();
	}

	do {
		xid_start++;
	} while (xids_used.find(xid_start) != xids_used.end());

	xids_used.insert(xid_start);

	return xid_start;
}



bool
crofbase::ta_validate(
		cofmsg *msg)
{
		return ta_validate(msg->get_xid(), msg->get_type());
}



bool
crofbase::ta_validate(
		uint32_t xid,
		uint8_t type) throw (eRofBaseXidInval)
{
	// check for pending transaction of type 'type'
	if (!ta_pending(xid, type))
	{
		WRITELOG(XID, DBG, "crofbase(%p)::ta_validate() no pending transaction for xid: 0x%x", this, xid);
		throw eRofBaseXidInval();
	}

	// delete transaction
	ta_rem_request(xid);

	return true;
}




