/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "crofbase.h"

/* static */ std::set<crofbase*> crofbase::rofbases;



crofbase::crofbase() throw (eRofBaseExists) :
		xid_used_max(CPCP_DEFAULT_XID_USED_MAX),
		xid_start(crandom(sizeof(uint32_t)).uint32())
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::crofbase()", this);

	//register_timer(TIMER_FE_DUMP_OFPACKETS, 15);

	crofbase::rofbases.insert(this);
}


crofbase::~crofbase()
{
	crofbase::rofbases.erase(this);
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::~crofbase()", this);


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



cofctl*
crofbase::ofctrl_exists(const cofctl *ofctl) throw (eRofBaseNotFound)
{
	cofctl *ctl = (cofctl*)ofctl;
	if (ofctl_set.find(ctl) == ofctl_set.end())
	{
		throw eRofBaseNotFound();
	}
	return ctl;
}



cofdpt*
crofbase::ofswitch_exists(const cofdpt *ofdpt) throw (eRofBaseNotFound)
{
	cofdpt *dpt = (cofdpt*)ofdpt;
	if (ofdpt_set.find(dpt) == ofdpt_set.end())
	{
		throw eRofBaseNotFound();
	}
	return dpt;
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
	handle_ctrl_close(ctl);
	if (ofctl_set.find(ctl) != ofctl_set.end())
	{
		delete ctl;
		ofctl_set.erase(ctl);
	}
}



void
crofbase::handle_accepted(
		csocket *socket,
		int newsd,
		caddress const& ra)
{
	if (rpc[RPC_CTL].find(socket) != rpc[RPC_CTL].end())
	{
		WRITELOG(CROFBASE, INFO, "crofbase(%p): new ctl TCP connection", this);
		ofctl_set.insert(cofctl_factory(this, newsd, ra, socket->domain, socket->type, socket->protocol));
	}
	else if (rpc[RPC_DPT].find(socket) != rpc[RPC_DPT].end())
	{
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



#if 0
void
crofbase::dpath_attach(cofbase* dp)
{
	if (NULL == dp) return;

	cofdpt *sw = NULL;

	try {

		sw = ofswitch_find(dp);

	} catch (eOFbaseNotAttached& e) {
		sw = new cofdpt(this, dp, &ofdpath_list);
		WRITELOG(CROFBASE, INFO, "crofbase(%p)::dpath_attach() cofbase: %p cofswitch: %s", this, dp, sw->c_str());
	}

	send_down_hello_message(sw);
}


void
crofbase::dpath_detach(cofbase* dp)
{
	if (NULL == dp) return;

	cofdpt *sw = NULL;
	try {
		sw = ofswitch_find(dp);

		// sends a HELLO with BYE flag to controller and deletes our ofctrl instance
		send_down_hello_message(sw, true /*BYE*/);

		handle_dpath_close(sw);

		WRITELOG(CROFBASE, INFO, "crofbase(%p)::dpath_detach() cofbase: %p cofswitch: %s", this, dp, sw->c_str());

		delete sw;


	} catch (eOFbaseNotAttached& e) { }
}


void
crofbase::ctrl_attach(cofbase* dp) throw (eRofBaseFspSupportDisabled)
{
	// sanity check: entity must exist
	if (NULL == dp) return;

	cofctl *ofctrl = NULL;

	// namespaces disabled? block attachment attempts
	if ((not fe_flags.test(NSP_ENABLED)) && (not ofctrl_list.empty()))
	{
		throw eRofBaseFspSupportDisabled();
	}

	// check for existence of control entity
	if (ofctrl_list.find(dp) == ofctrl_list.end())
	{
		ofctrl = new cofctl(this, dp, &ofctrl_list);
		WRITELOG(CROFBASE, INFO, "crofbase(%p)::ctrl_attach() cofbase: %p cofctrl: %s",
				this, dp, ofctrl->c_str());
	}

	send_up_hello_message(ofctrl);
}


void
crofbase::ctrl_detach(cofbase* dp)
{
	if (NULL == dp) return;

	std::map<cofbase*, cofctl*>::iterator it;
	if ((it = ofctrl_list.find(dp)) != ofctrl_list.end())
	{
		WRITELOG(CROFBASE, INFO, "crofbase(%p)::ctrl_detach() cofbase: %p cofctrl: %s",
				this, dp, it->second->c_str());

		// sends a HELLO with BYE flag to controller and deletes our ofctrl instance
		send_up_hello_message(it->second, true /*BYE*/);

		delete it->second;
	}
}
#endif



void
crofbase::handle_timeout(int opaque)
{
	try {
		switch (opaque) {
		case TIMER_FE_DUMP_OFPACKETS:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"cofpacket statistics => %s", this, cofpacket::packet_info());
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"cpacket statistics => %s", this, cpacket::cpacket_info());
#if 0
			fprintf(stdout, "crofbase(%p)::handle_timeout() "
					"cofpacket statistics => %s", this, cofpacket::packet_info());
			fprintf(stdout, "crofbase(%p)::handle_timeout() "
					"cpacket statistics => %s", this, cpacket::cpacket_info());
#endif
			register_timer(TIMER_FE_DUMP_OFPACKETS, 15);
			break;
		default:
			//WRITELOG(CROFBASE, DBG, "crofbase::handle_timeout() "
			//		"received unknown timer event %d", opaque);
			break;
		}

	} catch (eIoSvcUnhandledTimer& e) {
		// ignore
	}
}



void
crofbase::handle_experimenter_message(cofctl *ofctrl, cofpacket *pack)
{
	// base class does not support any vendor extensions, so: send error indication
	size_t datalen = (pack->framelen() > 64) ? 64 : pack->framelen();
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER,
									(unsigned char*)pack->soframe(), datalen);

	delete pack;
}


cofdpt&
crofbase::dpath_find(uint64_t dpid) throw (eRofBaseNotFound)
{
	for (std::set<cofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		if ((*it)->dpid == dpid)
			return *(*it);
	}
	throw eRofBaseNotFound();
}


cofdpt&
crofbase::dpath_find(std::string s_dpid) throw (eRofBaseNotFound)
{
	for (std::set<cofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		if ((*it)->s_dpid == s_dpid)
			return *(*it);
	}
	throw eRofBaseNotFound();
}


cofdpt&
crofbase::dpath_find(cmacaddr dl_dpid) throw (eRofBaseNotFound)
{
	for (std::set<cofdpt*>::iterator
			it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		if ((*it)->dpmac == dl_dpid)
			return *(*it);
	}
	throw eRofBaseNotFound();
}




cofdpt*
crofbase::ofswitch_find(cofdpt *dpt) throw (eRofBaseNotFound)
{
	if (ofdpt_set.find(dpt) == ofdpt_set.end())
	{
		throw eRofBaseNotFound();
	}
	return dpt;
}



cofctl*
crofbase::ofctrl_find(cofctl *ctl) throw (eRofBaseNotFound)
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

	cofpacket_hello *pack = new cofpacket_hello(
										ta_new_async_xid(),
										(uint8_t*)body, bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_hello_message() new %s", this,
			pack->c_str());

	ofctrl_find(ctl)->send_message(pack);
}



void
crofbase::send_hello_message(
		cofdpt *dpt,
		uint8_t *body, size_t bodylen)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_hello_message()", this);

	cofpacket_hello *pack = new cofpacket_hello(
										ta_new_async_xid(),
										(uint8_t*)body, bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_hello_message() new %s", this,
			pack->c_str());

	ofswitch_find(dpt)->send_message(pack);
}




void
crofbase::send_echo_request(
		cofdpt *dpt,
		uint8_t *body, size_t bodylen)
{
	cofpacket_echo_request *pack =
			new cofpacket_echo_request(
					ta_add_request(OFPT_ECHO_REQUEST), body, bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_echo_request() %s",
			this, pack->c_str());

	ofswitch_find(dpt)->send_message(pack);
}



void
crofbase::send_echo_reply(
		cofdpt *dpt,
		uint32_t xid,
		uint8_t *body, size_t bodylen)
{
	cofpacket_echo_reply *pack =
			new cofpacket_echo_reply(
					xid, body, bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_echo_reply() %s",
				this, pack->c_str());

	ofswitch_find(dpt)->send_message(pack);
}



void
crofbase::send_echo_request(
		cofctl *ctl,
		uint8_t *body, size_t bodylen)
{
	cofpacket_echo_request *pack =
			new cofpacket_echo_request(
					ta_add_request(OFPT_ECHO_REQUEST), body, bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_echo_request() %s",
				this, pack->c_str());

	ofctrl_find(ctl)->send_message(pack);
}



void
crofbase::send_echo_reply(
		cofctl *ctl,
		uint32_t xid,
		uint8_t *body, size_t bodylen)
{
	cofpacket_echo_reply *pack =
			new cofpacket_echo_reply(
					xid, body, bodylen);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_echo_reply() %s",
				this, pack->c_str());

	ofctrl_find(ctl)->send_message(pack);
}



/*
 * FEATURES request/reply
 */

void
crofbase::send_features_request(cofdpt *dpt)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_features_request()", this);

	cofpacket_features_request *pack = new cofpacket_features_request(
							ta_add_request(OFPT_FEATURES_REQUEST));

	ofswitch_find(dpt)->send_message(pack);
}



void
crofbase::send_features_reply(
		cofctl *ctl,
		uint32_t xid,
		uint64_t dpid,
		uint32_t n_buffers,
		uint8_t n_tables,
		uint32_t capabilities,
		uint8_t *ports,
		size_t portslen)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_features_reply()", this);

	cofpacket_features_reply *reply = new cofpacket_features_reply(
													xid,
													dpid,
													n_buffers,
													n_tables,
													capabilities);

	reply->ports.unpack((struct ofp_port*)ports, portslen);

	reply->pack(); // adjust fields, e.g. length in ofp_header

	ofctrl_find(ctl)->send_message(reply);
}



#if 0
void
crofbase::recv_message(
		cofpacket *pack)
{
	try {
		ta_validate(be32toh(pack->ofh_header->xid), pack->ofh_header->type);
	} catch (eOFbaseXidInval& e) {

	}
}
#endif



/*
 * GET-CONFIG request/reply
 */
void
crofbase::send_get_config_request(
		cofdpt *dpt)
{
	cofpacket_get_config_request *pack = new cofpacket_get_config_request(
								ta_add_request(OFPT_GET_CONFIG_REQUEST));

	ofswitch_find(dpt)->send_message(pack);
}



void
crofbase::send_get_config_reply(cofctl *ctl, uint32_t xid, uint16_t flags, uint16_t miss_send_len)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_get_config_reply()", this);

	cofpacket_get_config_reply *pack =
			new cofpacket_get_config_reply(
					xid,
					flags,
					miss_send_len);

	ofctrl_find(ctl)->send_message(pack);
}



/*
 * STATS request/reply
 */

uint32_t
crofbase::send_stats_request(
	cofdpt *dpt,
	uint16_t type,
	uint16_t flags,
	uint8_t* body,
	size_t bodylen)
{
	cofpacket_stats_request *pack = new cofpacket_stats_request(
											ta_add_request(OFPT_STATS_REQUEST),
											type,
											flags,
											body,
											bodylen);

	pack->pack();

	uint32_t xid = be32toh(pack->ofh_header->xid);

	ofswitch_find(dpt)->send_message(pack);

	return xid;
}



void
crofbase::handle_stats_request(cofctl *ofctrl, cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
}



void
crofbase::handle_desc_stats_request(
		cofctl *ofctrl,
		cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
}


void
crofbase::handle_table_stats_request(
		cofctl *ofctrl,
		cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
}


void
crofbase::handle_port_stats_request(
		cofctl *ofctrl,
		cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
}


void
crofbase::handle_flow_stats_request(
		cofctl *ofctrl,
		cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
}


void
crofbase::handle_aggregate_stats_request(
		cofctl *ofctrl,
		cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
}


void
crofbase::handle_queue_stats_request(
		cofctl *ofctrl,
		cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
}


void
crofbase::handle_group_stats_request(
		cofctl *ofctrl,
		cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
}


void
crofbase::handle_group_desc_stats_request(
		cofctl *ofctrl,
		cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
}


void
crofbase::handle_group_features_stats_request(
		cofctl *ofctrl,
		cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
}


void
crofbase::handle_experimenter_stats_request(
		cofctl *ofctrl,
		cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, pack->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
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
	if (more)
		flags = htobe16(OFPSF_REPLY_MORE);
	else
		flags = htobe16(0);

	cofpacket_stats_reply *pack = new cofpacket_stats_reply(
										xid,
										stats_type,
										flags,
										body,
										bodylen);

	ofctrl_find(ctl)->send_message(pack);
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
		cofpacket_set_config *pack =
				new cofpacket_set_config(
						ta_new_async_xid(),
						flags,
						miss_send_len);

		ofswitch_find(dpt)->send_message(pack);
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
	cofpacket_packet_out *pack =
			new cofpacket_packet_out(
					ta_new_async_xid(),
					buffer_id,
					in_port,
					data,
					datalen);

	pack->actions = aclist;

	pack->pack();

	ofswitch_find(dpt)->send_message(pack);
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
			try {
				std::set<cfspentry*> nse_list;

				nse_list = fsptable.find_matching_entries(
						match.oxmlist.oxm_find(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IN_PORT).uint32(),
						total_len,
						n_pack);

				WRITELOG(CROFBASE, WARN, "crofbase(%p) nse_list.size()=%d", this, nse_list.size());

				if (nse_list.empty())
				{
					throw eRofBaseNoCtrl();
				}

				for (std::set<cfspentry*>::iterator
						it = nse_list.begin(); it != nse_list.end(); ++it)
				{
					cofctl *ofctrl = dynamic_cast<cofctl*>( (*nse_list.begin())->fspowner );
					if (OFPCR_ROLE_SLAVE == ofctrl->role)
					{
						WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_packet_in_message() "
								"ofctrl:%p is SLAVE, ignoring", this, ofctrl);
						continue;
					}


					cofpacket_packet_in *pack = new cofpacket_packet_in(
														ta_new_async_xid(),
														buffer_id,
														total_len,
														reason,
														table_id,
														data,
														datalen);

					pack->match = match;

					pack->pack();

					WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_packet_in_message() "
									"sending PACKET-IN for buffer_id:0x%x to controller %s, pack: %s",
									this, buffer_id, ofctrl->c_str(), pack->c_str());

					// straight call to layer-(n+1) entity's fe_up_packet_in() method
					ofctrl_find(ofctrl)->send_message(pack);
				}

			} catch (eFspNoMatch& e) {
				cpacket pack(data, datalen);
				WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_packet_in_message() no ctrl found for packet: %s", this, pack.c_str());
				throw eRofBaseNoCtrl();
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
							this, buffer_id, ofctrl_find(ofctrl)->c_str());

			cofpacket_packet_in *pack = new cofpacket_packet_in(
												ta_new_async_xid(),
												buffer_id,
												total_len,
												reason,
												table_id,
												data,
												datalen);

			pack->match = match;

			pack->pack();

			WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_packet_in_message() "
							"sending PACKET-IN for buffer_id:0x%x pack: %s",
							this, buffer_id, pack->c_str());

			// straight call to layer-(n+1) entity's fe_up_packet_in() method
			ofctrl_find(ofctrl)->send_message(pack);
		}

	} catch (eRofBaseNotFound& e) {

	}
}




/*
 * BARRIER request/reply
 */

uint32_t
crofbase::send_barrier_request(cofdpt *sw)
{
	cofpacket_barrier_request *pack =
			new cofpacket_barrier_request(
					ta_add_request(OFPT_BARRIER_REQUEST));

	uint32_t xid = be32toh(pack->ofh_header->xid);

	ofswitch_find(sw)->send_message(pack);

	return xid;
}



void
crofbase::send_barrier_reply(
		cofctl* ctl,
		uint32_t xid)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_barrier_reply()", this);

	cofpacket_barrier_reply *pack =
			new cofpacket_barrier_reply(xid);

	ofctrl_find(ctl)->send_message(pack);
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

	cofpacket_role_request *pack =
			new cofpacket_role_request(
					ta_add_request(OFPT_ROLE_REQUEST),
					role,
					generation_id);

	ofswitch_find(dpt)->send_message(pack);
}



void
crofbase::send_role_reply(
		cofctl *ctl,
		uint32_t xid,
		uint32_t role,
		uint64_t generation_id)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_role_reply()", this);

	cofpacket_role_reply *pack =
			new cofpacket_role_reply(
					xid,
					role,
					generation_id);

	ofctrl_find(ctl)->send_message(pack);
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
		cofpacket_error *pack =
				new cofpacket_error(
						xid,
						type,
						code,
						data, datalen);

		// straight call to layer-(n+1) entity's fe_up_packet_in() method
		ofctrl_find(ctl)->send_message(pack);
	}
	else
	{
		for (std::set<cofctl*>::iterator
				it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
		{
			cofpacket_error *pack =
					new cofpacket_error(
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
	cofpacket_flow_mod *pack = new cofpacket_flow_mod(
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
											flags);

	pack->match = ofmatch;
	pack->instructions = inlist;

	pack->pack();

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_mod_message() "
			"pack: %s", this, pack->c_str());

	ofswitch_find(dpt)->send_message(pack);
}



void
crofbase::send_flow_mod_message(
		cofdpt *dpt,
		cflowentry& fe)
{
	cofpacket_flow_mod *pack =
			new cofpacket_flow_mod(
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
						fe.get_flags());

	pack->match = fe.match;
	pack->instructions = fe.instructions;

	pack->pack();

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_mod_message() pack: %s",
			this, pack->c_str());

	ofswitch_find(dpt)->send_message(pack);
}



/*
 * GROUP-MOD message
 */


void
crofbase::send_group_mod_message(
		cofdpt *dpt,
		cgroupentry& ge)
{
	cofpacket_group_mod *pack =
			new cofpacket_group_mod(
						ta_new_async_xid(),
						be16toh(ge.group_mod->command),
						ge.group_mod->type,
						be32toh(ge.group_mod->group_id));

	pack->buckets = ge.buckets;

	pack->pack();

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_group_mod_message() %s", this, pack->c_str());

	ofswitch_find(dpt)->send_message(pack);
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
	cofpacket_port_mod *pack =
			new cofpacket_port_mod(
						ta_new_async_xid(),
						port_no,
						hwaddr,
						config,
						mask,
						advertise);

	ofswitch_find(dpt)->send_message(pack);
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
	cofpacket_table_mod *pack =
			new cofpacket_table_mod(
						ta_new_async_xid(),
						table_id,
						config);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_table_mod_message() %s", this, pack->c_str());

	ofswitch_find(dpt)->send_message(pack);
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
			cofctl *ofctrl = (*it);

			if (OFPCR_ROLE_SLAVE == ofctrl->role)
			{
				WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_removed_message() ofctrl:%p is SLAVE", this, ofctrl);
				continue;
			}


			cofpacket_flow_removed *pack =
					new cofpacket_flow_removed(
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
								byte_count);

			pack->match = ofmatch;

			pack->pack();

			WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_removed_message() to controller %s", this, ofctrl->c_str());

			// straight call to layer-(n+1) entity's fe_up_packet_in() method
			ofctrl_find(ofctrl)->send_message(pack);
		}

	} catch (eRofBaseNotFound& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_removed_message() cofctrl instance not found", this);

	}
}






/*
 * PORT-STATUS message
 */

void
crofbase::send_port_status_message(
	uint8_t reason,
	cofport *port)
{
	WRITELOG(CROFBASE, DBG, "crofbase::send_port_status_message() %s", port->c_str());
	struct ofp_port phy_port;
	send_port_status_message(reason, port->pack(&phy_port, sizeof(phy_port)));
}



void
crofbase::send_port_status_message(
	uint8_t reason,
	struct ofp_port *phy_port)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_port_status_message()", this);
	//if (!ctrl)
	//	throw eRofBaseNoCtrl();

	std::map<cofbase*, cofctl*>::iterator it;
	for (std::set<cofctl*>::iterator
			it = ofctl_set.begin(); it != ofctl_set.end(); ++it)
	{
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_port_status_message() "
				"to ctrl %s", this, (*it)->c_str());

		cofpacket_port_status *pack =
				new cofpacket_port_status(
							ta_new_async_xid(),
							reason,
							phy_port,
							sizeof(struct ofp_port));

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
	cofpacket_queue_get_config_request *pack =
			new cofpacket_queue_get_config_request(
						ta_add_request(OFPT_QUEUE_GET_CONFIG_REQUEST),
						port);

	ofswitch_find(dpt)->send_message(pack);
}



void
crofbase::send_queue_get_config_reply(
		cofctl *ctl,
		uint32_t xid,
		uint32_t portno)
{
	WRITELOG(CROFBASE, DBG, "crofbase::send_queue_get_config_reply()");

	cofpacket_queue_get_config_reply *pack =
			new cofpacket_queue_get_config_reply(
						xid,
						portno);

	ofctrl_find(ctl)->send_message(pack);
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
	cofpacket_experimenter *pack =
			new cofpacket_experimenter(
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
			(*it)->send_message(new cofpacket(*pack));
		}
		delete pack;
	}
	else
	{
		ofswitch_find(dpt)->send_message(pack);
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
	cofpacket_experimenter *pack =
			new cofpacket_experimenter(
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
			(*it)->send_message(new cofpacket(*pack));
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
	ta_pending_reqs.erase(xid);
	// this yields an exception if type wasn't stored in ta_pending_reqs
}



bool
crofbase::ta_pending(
		uint32_t xid, uint8_t type)
{
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

	while (ta_pending_reqs.find(xid_start) != ta_pending_reqs.end())
	{
		xid_start++;
	}

	return xid_start;
}



bool
crofbase::ta_validate(
		cofpacket *pack)
{
		return ta_validate(be32toh(pack->ofh_header->xid), pack->ofh_header->type);
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




