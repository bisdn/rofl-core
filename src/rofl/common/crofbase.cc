/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "crofbase.h"

using namespace rofl;

/* static */ std::set<crofbase*> crofbase::rofbases;

crofbase::crofbase() :
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

	rpc_close_all();

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

	for (std::set<csocket*>::iterator it = rpc[RPC_DPT].begin();
			it != rpc[RPC_DPT].end(); ++it)
	{
		delete (*it);
	}

	// detach from higher layer entities
	for (std::set<cofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
	{
		delete (*it);
	}

	for (std::set<cofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		delete (*it);
	}
}



const char*
crofbase::c_str()
{
	cvastring vas(1024);

	info.assign(vas("crofbase(%p): ", this));

	// cofctrl instances
	info.append(vas("\nlist of registered cofctl instances: =>"));
	for (std::set<cofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
	{
		info.append(vas("\n  %s", (*it)->c_str()));
	}

	// cofswitch instances
	info.append(vas("\nlist of registered cofdpt instances: =>"));
	for (std::set<cofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		info.append(vas("\n  %s", (*it)->c_str()));
	}

	return info.c_str();
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
		cofdpt *dpt)
{
	handle_dpath_open(dpt);
}



void
crofbase::handle_dpt_close(
		cofdpt *dpt)
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
		cofctl *ctl)
{
	handle_ctrl_open(ctl);
}



void
crofbase::handle_ctl_close(
		cofctl *ctl)
{
	if (ofctl_set.find(ctl) != ofctl_set.end())
	{
		ofctl_set.erase(ctl);
	}
	handle_ctrl_close(ctl);
	delete ctl;
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
	socket->cpopen(addr, domain, type, protocol, backlog);
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
	socket->cpopen(addr, domain, type, protocol, backlog);
	rpc[RPC_CTL].insert(socket);
}



void
crofbase::rpc_connect_to_ctl(
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	ofctl_set.insert(cofctl_factory(this, ra, domain, type, protocol));
}


void
crofbase::rpc_disconnect_from_ctl(
		cofctl *ctl)
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
crofbase::rpc_connect_to_dpt(
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	ofdpt_set.insert(cofdpt_factory(this, ra, domain, type, protocol));
}


void
crofbase::rpc_disconnect_from_dpt(
		cofdpt *dpt)
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



cofctl*
crofbase::cofctl_factory(
		crofbase* owner,
		int newsd,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	cofctl *ctl = new cofctl(owner, newsd, ra, domain, type, protocol);

	ofctl_set.insert(ctl);

	return ctl;
}



cofctl*
crofbase::cofctl_factory(
		crofbase* owner,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	cofctl *ctl = new cofctl(owner, ra, domain, type, protocol);

	ofctl_set.insert(ctl);

	return ctl;
}



cofdpt*
crofbase::cofdpt_factory(
		crofbase* owner,
		int newsd,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	cofdpt *dpt = new cofdpt(owner, newsd, ra, domain, type, protocol);

	ofdpt_set.insert(dpt);

	return dpt;
}



cofdpt*
crofbase::cofdpt_factory(
		crofbase* owner,
		caddress const& ra,
		int domain,
		int type,
		int protocol)
{
	cofdpt *dpt = new cofdpt(owner, ra, domain, type, protocol);

	ofdpt_set.insert(dpt);

	return dpt;
}




void
crofbase::handle_timeout(int opaque)
{
	try {
		switch (opaque) {
		case TIMER_FE_DUMP_OFPACKETS: {
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"cofmsg statistics => %s", this, cofmsg::packet_info());
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"cpacket statistics => %s", this, cpacket::cpacket_info());
			register_timer(TIMER_FE_DUMP_OFPACKETS, 15);
		} break;
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
	if (tid != pthread_self())
	{
		notify(CROFBASE_EVENT_WAKEUP);
	}
}


void
crofbase::handle_experimenter_message(cofctl *ofctrl, cofmsg_experimenter *pack)
{
	// base class does not support any vendor extensions, so: send error indication
	size_t datalen = (pack->framelen() > 64) ? 64 : pack->framelen();
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER,
									(unsigned char*)pack->soframe(), datalen);

	delete pack;
}


cofdpt*
crofbase::dpt_find(uint64_t dpid) throw (eRofBaseNotFound)
{
	for (std::set<cofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		if ((*it)->dpid == dpid)
			return (*it);
	}
	throw eRofBaseNotFound();
}


cofdpt*
crofbase::dpt_find(std::string s_dpid) throw (eRofBaseNotFound)
{
	for (std::set<cofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		if ((*it)->s_dpid == s_dpid)
			return (*it);
	}
	throw eRofBaseNotFound();
}


cofdpt*
crofbase::dpt_find(cmacaddr dl_dpid) throw (eRofBaseNotFound)
{
	for (std::set<cofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		if ((*it)->dpmac == dl_dpid)
			return (*it);
	}
	throw eRofBaseNotFound();
}




cofdpt*
crofbase::dpt_find(cofdpt *dpt) throw (eRofBaseNotFound)
{
	if (ofdpt_set.find(dpt) == ofdpt_set.end())
	{
		throw eRofBaseNotFound();
	}
	return dpt;
}



cofctl*
crofbase::ctl_find(cofctl *ctl) throw (eRofBaseNotFound)
{
	if (ofctl_set.find(ctl) == ofctl_set.end())
	{
		throw eRofBaseNotFound();
	}
	return ctl;
}







/*
* HELLO messages
*/

void
crofbase::send_hello_message(
		cofctl *ctl,
		uint8_t *body, size_t bodylen)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_hello_message()", this);

	cofmsg_hello *pack =
			new cofmsg_hello(
					ctl->get_version(),
					ta_new_async_xid(),
					(uint8_t*)body, bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_hello_message() new %s", this,
			pack->c_str());

	ctl_find(ctl)->send_message(pack);
}



void
crofbase::send_hello_message(
		cofdpt *dpt,
		uint8_t *body, size_t bodylen)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_hello_message()", this);

	cofmsg_hello *pack =
			new cofmsg_hello(
					dpt->get_version(),
					ta_new_async_xid(),
					body,
					bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_hello_message() new %s", this,
			pack->c_str());

	dpt_find(dpt)->send_message(pack);
}




void
crofbase::send_echo_request(
		cofdpt *dpt,
		uint8_t *body, size_t bodylen)
{
	cofmsg_echo_request *msg =
			new cofmsg_echo_request(
					dpt->get_version(),
					OFPT_ECHO_REQUEST,
					ta_add_request(OFPT_ECHO_REQUEST),
					body,
					bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_echo_request() %s",
			this, msg->c_str());

	dpt_find(dpt)->send_message(msg);
}



void
crofbase::send_echo_reply(
		cofdpt *dpt,
		uint32_t xid,
		uint8_t *body, size_t bodylen)
{
	cofmsg_echo_request *msg =
			new cofmsg_echo_request(
					dpt->get_version(),
					OFPT_ECHO_REPLY,
					xid,
					body,
					bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_echo_reply() %s",
				this, msg->c_str());

	dpt_find(dpt)->send_message(msg);
}



void
crofbase::send_echo_request(
		cofctl *ctl,
		uint8_t *body, size_t bodylen)
{
	cofmsg_echo_request *msg =
			new cofmsg_echo_request(
					ctl->get_version(),
					OFPT_ECHO_REQUEST,
					ta_add_request(OFPT_ECHO_REQUEST),
					body,
					bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_echo_request() %s",
				this, msg->c_str());

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::send_echo_reply(
		cofctl *ctl,
		uint32_t xid,
		uint8_t *body, size_t bodylen)
{
	cofmsg_echo_request *msg =
			new cofmsg_echo_request(
					ctl->get_version(),
					OFPT_ECHO_REPLY,
					xid,
					body,
					bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_echo_reply() %s",
				this, msg->c_str());

	ctl_find(ctl)->send_message(msg);
}



/*
 * FEATURES request/reply
 */

void
crofbase::send_features_request(cofdpt *dpt)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_features_request()", this);

	cofmsg_features_request *msg =
			new cofmsg_features_request(
					dpt->get_version(),
					ta_add_request(OFPT_FEATURES_REQUEST));

	dpt_find(dpt)->send_message(msg);
}



void
crofbase::send_features_reply(
		cofctl *ctl,
		uint32_t xid,
		uint64_t dpid,
		uint32_t n_buffers,
		uint8_t n_tables,
		uint32_t capabilities,
		uint8_t of13_auxiliary_id,
		uint32_t of10_actions_bitmap,
		cofportlist const& portlist)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_features_reply()", this);

	cofmsg_features_reply *reply =
			new cofmsg_features_reply(
					ctl->get_version(),
					xid,
					dpid,
					n_buffers,
					n_tables,
					capabilities,
					of13_auxiliary_id,
					of10_actions_bitmap,
					portlist);

	reply->pack(); // adjust fields, e.g. length in ofp_header

	ctl_find(ctl)->send_message(reply);
}




void
crofbase::handle_features_reply_timeout(cofdpt *dpt)
{
    if (ofdpt_set.find(dpt) != ofdpt_set.end())
    {
            delete dpt;
            ofdpt_set.erase(dpt);
    }
}




/*
 * GET-CONFIG request/reply
 */
void
crofbase::send_get_config_request(
		cofdpt *dpt)
{
	cofmsg_get_config_request *pack =
			new cofmsg_get_config_request(
					dpt->get_version(),
					ta_add_request(OFPT_GET_CONFIG_REQUEST));

	dpt_find(dpt)->send_message(pack);
}



void
crofbase::send_get_config_reply(cofctl *ctl, uint32_t xid, uint16_t flags, uint16_t miss_send_len)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_get_config_reply()", this);

	cofmsg_config *msg =
			new cofmsg_config(
					ctl->get_version(),
					OFPT_GET_CONFIG_REPLY,
					xid,
					flags,
					miss_send_len);

	ctl_find(ctl)->send_message(msg);
}



void
crofbase::handle_get_config_reply_timeout(cofdpt *dpt)
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
	cofdpt *dpt,
	uint16_t stats_type,
	uint16_t stats_flags,
	uint8_t* body,
	size_t bodylen)
{
	cofmsg_stats *msg =
			new cofmsg_stats(
					dpt->get_version(),
					OFPT_STATS_REQUEST,
					ta_add_request(OFPT_STATS_REQUEST),
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
crofbase::send_flow_stats_request(
		cofdpt *dpt,
		uint16_t flags,
		cofflow_stats_request const& flow_stats_request)
{
	cofmsg_flow_stats_request *msg =
			new cofmsg_flow_stats_request(
					dpt->get_version(),
					ta_add_request(OFPT_STATS_REQUEST),
					flags,
					flow_stats_request);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



uint32_t
crofbase::send_aggr_stats_request(
		cofdpt *dpt,
		uint16_t flags,
		cofaggr_stats_request const& aggr_stats_request)
{
	cofmsg_aggr_stats_request *msg =
			new cofmsg_aggr_stats_request(
					dpt->get_version(),
					ta_add_request(OFPT_STATS_REQUEST),
					flags,
					aggr_stats_request);

	msg->pack();

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}


void
crofbase::send_stats_reply(
		cofctl *ctl,
		uint32_t xid,
		uint16_t stats_type, /* network byte order */
		uint8_t *body, size_t bodylen,
		bool more)
{
	uint16_t flags = 0;

	flags |= (more) ? OFPSF_REPLY_MORE : 0;

	cofmsg_stats *msg =
			new cofmsg_stats(
					ctl->get_version(),
					OFPT_STATS_REPLY,
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
	cofctl *ctl,
	uint32_t xid,
	cofdesc_stats_reply const& desc_stats,
	bool more)
{
	uint16_t flags = 0;

	flags |= (more) ? OFPSF_REPLY_MORE : 0;

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
	cofctl *ctl,
	uint32_t xid,
	std::vector<coftable_stats_reply> const& table_stats,
	bool more)
{
	uint16_t flags = 0;

	flags |= (more) ? OFPSF_REPLY_MORE : 0;

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
	cofctl *ctl,
	uint32_t xid,
	std::vector<cofport_stats_reply> const& port_stats,
	bool more)
{
	uint16_t flags = 0;

	flags |= (more) ? OFPSF_REPLY_MORE : 0;

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
crofbase::send_flow_stats_reply(
	cofctl *ctl,
	uint32_t xid,
	std::vector<cofflow_stats_reply> const& flow_stats,
	bool more)
{
	uint16_t flags = 0;

	flags |= (more) ? OFPSF_REPLY_MORE : 0;

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
	cofctl *ctl,
	uint32_t xid,
	cofaggr_stats_reply const& aggr_stats,
	bool more)
{
	uint16_t flags = 0;

	flags |= (more) ? OFPSF_REPLY_MORE : 0;

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
	cofctl *ctl,
	uint32_t xid,
	std::vector<cofgroup_stats_reply> const& group_stats,
	bool more)
{
	uint16_t flags = 0;

	flags |= (more) ? OFPSF_REPLY_MORE : 0;

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
	cofctl *ctl,
	uint32_t xid,
	std::vector<cofgroup_desc_stats_reply> const& group_desc_stats,
	bool more)
{
	uint16_t flags = 0;

	flags |= (more) ? OFPSF_REPLY_MORE : 0;

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
	cofctl *ctl,
	uint32_t xid,
	cofgroup_features_stats_reply const& group_features_stats,
	bool more)
{
	uint16_t flags = 0;

	flags |= (more) ? OFPSF_REPLY_MORE : 0;

	cofmsg_group_features_stats_reply *msg =
			new cofmsg_group_features_stats_reply(
					ctl->get_version(),
					xid,
					flags,
					group_features_stats);

	msg->pack();

	ctl_find(ctl)->send_message(msg);
}



/*
 * SET-CONFIG message
 */


void
crofbase::send_set_config_message(
	cofdpt *dpt,
	uint16_t flags,
	uint16_t miss_send_len)
{
	cofmsg_config *msg =
			new cofmsg_config(
					dpt->get_version(),
					OFPT_SET_CONFIG,
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
	cofdpt *dpt,
	uint32_t buffer_id,
	uint32_t in_port,
	cofaclist& aclist,
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
	uint32_t buffer_id,
	uint16_t total_len,
	uint8_t reason,
	uint8_t table_id,
	uint64_t cookie,
	uint16_t in_port, // for OF 1.0
	cofmatch& match,
	uint8_t* data,
	size_t datalen) throw(eRofBaseNoCtrl)
{
	try {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_packet_in_message() "
				"ofctrl_list.size()=%d", this, ofctl_set.size());

		cpacket n_pack(data, datalen, match.get_in_port());

		if (fe_flags.test(NSP_ENABLED))
		{
			std::set<cfspentry*> nse_list;

			nse_list = fsptable.find_matching_entries(match.get_in_port(), total_len, n_pack);

			WRITELOG(CROFBASE, DBG, "crofbase(%p) nse_list.size()=%d", this, nse_list.size());

			if (nse_list.empty())
			{
				throw eRofBaseNoCtrl();
			}

			for (std::set<cfspentry*>::iterator
					it = nse_list.begin(); it != nse_list.end(); ++it)
			{
				cofctl *ctl = dynamic_cast<cofctl*>( (*nse_list.begin())->fspowner );
				if (OFPCR_ROLE_SLAVE == ctl->role)
				{
					WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_packet_in_message() "
							"ofctrl:%p is SLAVE, ignoring", this, ctl);
					continue;
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

				pack->pack();

				WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_packet_in_message() "
								"sending PACKET-IN for buffer_id:0x%x to controller %s, pack: %s",
								this, buffer_id, ctl->c_str(), pack->c_str());

				// straight call to layer-(n+1) entity's fe_up_packet_in() method
				ctl_find(ctl)->send_message(pack);
			}

			return;
		}
		else
		{
			if (ofctl_set.empty())
			{
				throw eRofBaseNoCtrl();
			}
			cofctl *ofctrl = *(ofctl_set.begin());

			WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_packet_in_message() "
							"sending PACKET-IN for buffer_id:0x%x to controller %s",
							this, buffer_id, ctl_find(ofctrl)->c_str());

			cofmsg_packet_in *pack =
					new cofmsg_packet_in(
							ofctrl->get_version(),
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

			pack->pack();

			WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_packet_in_message() "
							"sending PACKET-IN for buffer_id:0x%x pack: %s",
							this, buffer_id, pack->c_str());

			// straight call to layer-(n+1) entity's fe_up_packet_in() method
			ctl_find(ofctrl)->send_message(pack);
		}

	} catch (eFspNoMatch& e) {

		cpacket pack(data, datalen);
		WRITELOG(CROFBASE, ERROR, "crofbase(%p)::send_packet_in_message() no ctrl found for packet: %s", this, pack.c_str());

	} catch (eRofBaseNotFound& e) {

		cpacket pack(data, datalen);
		WRITELOG(CROFBASE, ERROR, "crofbase(%p)::send_packet_in_message() no ctrl found for packet: %s", this, pack.c_str());

	}
}




/*
 * BARRIER request/reply
 */

uint32_t
crofbase::send_barrier_request(cofdpt *dpt)
{
	cofmsg_barrier_request *msg =
			new cofmsg_barrier_request(
					dpt->get_version(),
					OFPT_BARRIER_REQUEST,
					ta_add_request(OFPT_BARRIER_REQUEST));

	uint32_t xid = msg->get_xid();

	dpt_find(dpt)->send_message(msg);

	return xid;
}



void
crofbase::send_barrier_reply(
		cofctl* ctl,
		uint32_t xid)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_barrier_reply()", this);

	cofmsg_barrier_reply *msg =
			new cofmsg_barrier_reply(
					ctl->get_version(),
					OFPT_BARRIER_REPLY,
					xid);

	ctl_find(ctl)->send_message(msg);
}





/*
 * ROLE.request/reply
 */

void
crofbase::send_role_request(
	cofdpt *dpt,
	uint32_t role,
	uint64_t generation_id)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_role_request()", this);

	cofmsg_role_request *msg =
			new cofmsg_role_request(
					dpt->get_version(),
					OFPT_ROLE_REQUEST,
					ta_add_request(OFPT_ROLE_REQUEST),
					role,
					generation_id);

	dpt_find(dpt)->send_message(msg);
}



void
crofbase::send_role_reply(
		cofctl *ctl,
		uint32_t xid,
		uint32_t role,
		uint64_t generation_id)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_role_reply()", this);

	cofmsg_role_reply *msg =
			new cofmsg_role_reply(
					ctl->get_version(),
					OFPT_ROLE_REPLY,
					xid,
					role,
					generation_id);

	ctl_find(ctl)->send_message(msg);
}







/*
 * ERROR message
 */


void
crofbase::send_error_message(
	cofctl *ctl,
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
		for (std::set<cofctl*>::iterator
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
	cofdpt *dpt,
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
		for (std::set<cofdpt*>::iterator
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



/*
 * FLOW-MOD message
 */


void
crofbase::send_flow_mod_message(
	cofdpt *dpt,
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
	cofinlist& inlist)
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

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_mod_message() "
			"pack: %s", this, pack->c_str());

	dpt_find(dpt)->send_message(pack);
}



void
crofbase::send_flow_mod_message(
		cofdpt *dpt,
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

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_mod_message() pack: %s",
			this, pack->c_str());

	dpt_find(dpt)->send_message(pack);
}



/*
 * GROUP-MOD message
 */


void
crofbase::send_group_mod_message(
		cofdpt *dpt,
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

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_group_mod_message() %s", this, pack->c_str());

	dpt_find(dpt)->send_message(pack);
}



/*
 * PORT-MOD message
 */


void
crofbase::send_port_mod_message(
	cofdpt *dpt,
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

	dpt_find(dpt)->send_message(pack);
}




/*
 * TABLE-MOD message
 */


void
crofbase::send_table_mod_message(
		cofdpt *dpt,
		uint8_t table_id,
		uint32_t config)
{
	cofmsg_table_mod *pack =
			new cofmsg_table_mod(
						dpt->get_version(),
						ta_new_async_xid(),
						table_id,
						config);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_table_mod_message() %s", this, pack->c_str());

	dpt_find(dpt)->send_message(pack);
}




/*
 * FLOW-REMOVED message
 */


void
crofbase::send_flow_removed_message(
	cofctl *ctl,
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

		for (std::set<cofctl*>::iterator
				it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
		{
			if (not (*it)->is_established()) {
				continue;
			}
			cofctl *ofctrl = (*it);

			if (OFPCR_ROLE_SLAVE == ofctrl->role)
			{
				WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_removed_message() ofctrl:%p is SLAVE", this, ofctrl);
				continue;
			}


			cofmsg_flow_removed *pack =
					new cofmsg_flow_removed(
							ctl->get_version(),
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

			WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_removed_message() to controller %s", this, ofctrl->c_str());

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
	uint8_t reason,
	cofport const& port)
{
	cofport c_port(port); // FIXME: c_str should be const
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_port_status_message() %s", this, c_port.c_str());
	//if (!ctrl)
	//	throw eRofBaseNoCtrl();

	std::map<cofbase*, cofctl*>::iterator it;
	for (std::set<cofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
	{
		if (not (*it)->is_established()) {
			continue;
		}

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_port_status_message() "
				"to ctrl %s", this, (*it)->c_str());

		cofmsg_port_status *pack =
				new cofmsg_port_status(
							(*it)->get_version(),
							ta_new_async_xid(),
							reason,
							port);

		(*it)->send_message(pack);
	}
}







/*
 * QUEUE-GET-CONFIG request/reply
 */

void
crofbase::send_queue_get_config_request(
	cofdpt *dpt,
	uint32_t port)
{
	cofmsg_queue_get_config_request *pack =
			new cofmsg_queue_get_config_request(
					dpt->get_version(),
					ta_add_request(OFPT_QUEUE_GET_CONFIG_REQUEST),
					port);

	dpt_find(dpt)->send_message(pack);
}



void
crofbase::send_queue_get_config_reply(
		cofctl *ctl,
		uint32_t xid,
		uint32_t portno)
{
	WRITELOG(CROFBASE, DBG, "crofbase::send_queue_get_config_reply()");

	cofmsg_queue_get_config_reply *pack =
			new cofmsg_queue_get_config_reply(
					ctl->get_version(),
					xid,
					portno);

	ctl_find(ctl)->send_message(pack);
}





/*
 * VENDOR messages
 */



void
crofbase::send_experimenter_message(
		cofdpt *dpt,
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen)
{
	cofmsg_experimenter *pack =
			new cofmsg_experimenter(
						dpt->get_version(),
						ta_new_async_xid(),
						experimenter_id,
						exp_type,
						body,
						bodylen);

	pack->pack();

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_experimenter_message() -down- %s", this, pack->c_str());

	if (NULL == dpt) // send to all attached data path entities
	{
		for (std::set<cofdpt*>::iterator
				it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
		{
			(*it)->send_message(new cofmsg(*pack));
		}
		delete pack;
	}
	else
	{
		dpt_find(dpt)->send_message(pack);
	}
}



void
crofbase::send_experimenter_message(
		cofctl *ctl,
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen)
{
	cofmsg_experimenter *pack =
			new cofmsg_experimenter(
						ctl->get_version(),
						ta_new_async_xid(),
						experimenter_id,
						exp_type,
						body,
						bodylen);

	pack->pack();

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_experimenter_message() -up- %s", this, pack->c_str());

	if ((cofctl*)0 == ctl) // send to all attached controller entities
	{
		for (std::set<cofctl*>::iterator
				it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
		{
			if (not (*it)->is_established()) {
				continue;
			}
			(*it)->send_message(new cofmsg(*pack));
		}

		delete pack;
	}
	else
	{
		// straight call to layer-(n+1) entity's fe_up_experimenter_message() method
		ctl->send_message(pack);
	}
}






uint32_t
crofbase::ta_add_request(
		uint8_t type)
{
	RwLock lock(&xidlock, RwLock::RWLOCK_WRITE);

	uint32_t xid = ta_new_async_xid();

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




