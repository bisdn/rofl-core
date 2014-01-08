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

crofbase::crofbase(
		cofhello_elem_versionbitmap const& versionbitmap) :
				versionbitmap(versionbitmap),
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
crofbase::send_packet_in_message(
		uint32_t buffer_id,
		uint16_t total_len,
		uint8_t reason,
		uint8_t table_id,
		uint64_t cookie,
		uint16_t in_port, // for OF1.0
		cofmatch &match,
		uint8_t *data,
		size_t datalen)
{
	for (std::set<crofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it) {

		// TODO: roles

		(*(*it)).send_packet_in_message(
				buffer_id,
				total_len,
				reason,
				table_id,
				cookie,
				in_port, // for OF1.0
				match,
				data,
				datalen);
	}
	//throw eNotImplemented("crofbase::send_packet_in_message()");
}



void
crofbase::send_flow_removed_message(
	cofmatch& match,
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
	for (std::set<crofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it) {

		// TODO: roles

		(*(*it)).send_flow_removed_message(
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
	}
	//throw eNotImplemented("crofbase::send_flow_removed_message()");
}



void
crofbase::send_port_status_message(
	uint8_t reason,
	cofport const& port)
{
	for (std::set<crofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it) {

		// TODO: roles

		(*(*it)).send_port_status_message(reason, port);
	}
	//throw eNotImplemented("crofbase::send_port_status_message()");
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
		ofctl_set.insert(cofctl_factory(this, versionbitmap, newsd));
	}
	else if (rpc[RPC_DPT].find(socket) != rpc[RPC_DPT].end())
	{
#ifndef NDEBUG
		caddress raddr(ra);
		fprintf(stderr, "A:dpt[%s] ", raddr.c_str());
#endif
		WRITELOG(CROFBASE, INFO, "crofbase(%p): new dpt TCP connection", this);
		ofdpt_set.insert(cofdpt_factory(this, versionbitmap, newsd));
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
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	ofctl_set.insert(cofctl_factory(this, versionbitmap, reconnect_start_timeout, ra, domain, type, protocol));
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
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	ofdpt_set.insert(cofdpt_factory(this, versionbitmap, reconnect_start_timeout, ra, domain, type, protocol));
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
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int newsd)
{
	return new crofctlImpl(owner, versionbitmap, newsd);
}



crofctl*
crofbase::cofctl_factory(
		crofbase* owner,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	return new crofctlImpl(owner, versionbitmap, reconnect_start_timeout, ra, domain, type, protocol);
}



crofdpt*
crofbase::cofdpt_factory(
		crofbase* owner,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int newsd)
{
	return new crofdptImpl(owner, versionbitmap, newsd);
}



crofdpt*
crofbase::cofdpt_factory(
		crofbase* owner,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
		int reconnect_start_timeout,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	return new crofdptImpl(owner, versionbitmap, reconnect_start_timeout, ra, domain, type, protocol);
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




