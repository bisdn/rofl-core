/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "crofbase.h"

/* static */ std::set<crofbase*> crofbase::rofbases;



crofbase::crofbase(
		caddress const& rpc_ctl_addr,
		caddress const& rpc_dpt_addr) throw (eRofBaseExists)
{
	cvastring vas;

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::crofbase()", this);

	// create rpc TCP endpoints
	rpc[RPC_CTL] = new cofrpc(cofrpc::OF_RPC_TCP_SOUTH_ENDPNT, this);
	rpc[RPC_CTL]->clisten(rpc_ctl_addr);

	rpc[RPC_DPT] = new cofrpc(cofrpc::OF_RPC_TCP_NORTH_ENDPNT, this);
	rpc[RPC_DPT]->clisten(rpc_dpt_addr);

	//register_timer(TIMER_FE_DUMP_OFPACKETS, 15);

	crofbase::rofbases.insert(this);
}


crofbase::~crofbase()
{
	crofbase::rofbases.erase(this);
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::~crofbase()", this);


	// detach from higher layer entities
	while (not ofctrl_list.empty())
	{
		delete ofctrl_list.begin()->second; // delete cofctrl instance (removes itself from ofctrl_list)
	}

	// detach from lower layer entities
	while (not ofdpath_list.empty())
	{
		delete ofdpath_list.begin()->second; // delete cofswitch instance (removes itself from ofswitch_list)
	}


	std::map<int, std::list<cofpacket*> >::iterator it;

	// remove all pending packets from all down-queues
	for (it = fe_down_queue.begin(); it != fe_down_queue.end(); ++it)
	{
		std::list<cofpacket*> plist = it->second;
		while (not plist.empty())
		{
			delete plist.front();
			plist.pop_front();
		}
	}

	// remove all pending packets from all up-queues
	for (it = fe_up_queue.begin(); it != fe_up_queue.end(); ++it)
	{
		std::list<cofpacket*> plist = it->second;
		while (not plist.empty())
		{
			delete plist.front();
			plist.pop_front();
		}
	}


	// remove rpc TCP endpoints
	delete rpc[RPC_CTL];
	delete rpc[RPC_DPT];
}




const char*
crofbase::c_str()
{
	cvastring vas(1024);

	info.assign(vas("crofbase(%p): ", this));

	// cofctrl instances
	info.append(vas("\nlist of registered cofctrl instances: =>"));
	std::map<cofbase*, cofctl*>::iterator it_ctl;
	for (it_ctl = ofctrl_list.begin(); it_ctl != ofctrl_list.end(); ++it_ctl)
	{
		info.append(vas("\n  %s", it_ctl->second->c_str()));
	}

	// cofswitch instances
	info.append(vas("\nlist of registered cofswitch instances: =>"));
	std::map<cofbase*, cofdpt*>::iterator it_dp;
	for (it_dp = ofdpath_list.begin(); it_dp != ofdpath_list.end(); ++it_dp)
	{
		info.append(vas("\n  %s", it_dp->second->c_str()));
	}

	return info.c_str();
}


void
crofbase::ofctrl_exists(const cofctl *ofctrl) throw (eRofBaseNotFound)
{
	std::map<cofbase*, cofctl*>::iterator it;
	for (it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
	{
		if (it->second == ofctrl)
			return;
	}
	throw eRofBaseNotFound();
}


void
crofbase::ofswitch_exists(const cofdpt *ofswitch) throw (eRofBaseNotFound)
{
	std::map<cofbase*, cofdpt*>::iterator it;
	for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		if (it->second == ofswitch)
			return;
	}
	throw eRofBaseNotFound();
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
crofbase::ctl_recv_message(
		cofctl *ctl,
		cofpacket *pack)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::ctl_recv_message() %s", this, pack->c_str());

	try {
		check_down_packet(ctl, pack);

		switch (pack->ofh_header->type) {
		case OFPT_HELLO:
			recv_hello_message(ctl, pack);
			break;
		}


#if 0
		fe_down_queue[pack->ofh_header->type].push_back(pack); // store pack for xid
		switch (pack->ofh_header->type) {
		case OFPT_HELLO:
			register_timer(TIMER_FE_HANDLE_HELLO, 0);
			break;
		default:
			{
				delete pack;
			}
			break;
		}
#endif

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_hello_message() packet from non-controlling entity dropped", this);

		delete pack;

	} catch (eOFbaseInval& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_hello_message() malformed packet received", this);
		delete pack;

	} catch (eRofBaseFspSupportDisabled& e) {

		WRITELOG(CROFBASE, DBG, "\n\ncrofbase(%p)::fe_down_hello_message() "
				"flowspace support for multiple controllers DISABLED!!!\n\n", this);
				delete pack;
	}
}



void
crofbase::dpt_recv_message(
		cofdpt *dpt,
		cofpacket *pack)
{

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
		ofctl_set.insert(new cofctl(this, newsd, ra, socket->domain, socket->type, socket->protocol));
	}
	else if (rpc[RPC_DPT].find(socket) != rpc[RPC_DPT].end())
	{
		WRITELOG(CROFBASE, INFO, "crofbase(%p): new dpt TCP connection", this);
		ofdpt_set.insert(new cofdpt(this, newsd, ra, socket->domain, socket->type, socket->protocol));
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
	ofctl_set.insert(new cofctl(this, ra, domain, type, protocol));
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
	ofdpt_set.insert(new cofdpt(this, ra, domain, type, protocol));
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
		case TIMER_FE_HANDLE_HELLO:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
								"TIMER_FE_HANDLE_HELLO (%d) expired", this,  TIMER_FE_HANDLE_HELLO);
			recv_hello_message();
			break;
		case TIMER_FE_HANDLE_FEATURES_REQUEST:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_FEATURES_REQUEST (%d) expired", this,  TIMER_FE_HANDLE_FEATURES_REQUEST);
			recv_features_request();
			break;
		case TIMER_FE_HANDLE_GET_CONFIG_REQUEST:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_GET_CONFIG_REQUEST (%d) expired", this,  TIMER_FE_HANDLE_GET_CONFIG_REQUEST);
			recv_get_config_request();
			break;
		case TIMER_FE_HANDLE_STATS_REQUEST:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_STATS_REQUEST (%d) expired", this,  TIMER_FE_HANDLE_STATS_REQUEST);
			recv_stats_request();
			break;
		case TIMER_FE_HANDLE_PACKET_OUT:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_PACKET_OUT (%d) expired", this,  TIMER_FE_HANDLE_PACKET_OUT);
			recv_packet_out();
			break;
		case TIMER_FE_HANDLE_PACKET_IN:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_PACKET_IN (%d) expired", this,  TIMER_FE_HANDLE_PACKET_IN);
			recv_packet_in();
			break;
		case TIMER_FE_HANDLE_ERROR:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_ERROR (%d) expired", this,  TIMER_FE_HANDLE_ERROR);
			recv_error();
			break;
		case TIMER_FE_HANDLE_FLOW_MOD:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_FLOW_MOD (%d) expired", this,  TIMER_FE_HANDLE_FLOW_MOD);
			recv_flow_mod();
			break;
		case TIMER_FE_HANDLE_GROUP_MOD:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_GROUP_MOD (%d) expired", this,  TIMER_FE_HANDLE_GROUP_MOD);
			recv_group_mod();
			break;
		case TIMER_FE_HANDLE_TABLE_MOD:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_TABLE_MOD (%d) expired", this,  TIMER_FE_HANDLE_TABLE_MOD);
			recv_table_mod();
			break;
		case TIMER_FE_HANDLE_PORT_MOD:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_PORT_MOD (%d) expired", this,  TIMER_FE_HANDLE_PORT_MOD);
			recv_port_mod();
			break;
		case TIMER_FE_HANDLE_FLOW_REMOVED:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_FLOW_REMOVED (%d) expired", this,  TIMER_FE_HANDLE_FLOW_REMOVED);
			recv_flow_removed();
			break;
		case TIMER_FE_HANDLE_SET_CONFIG:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_SET_CONFIG (%d) expired", this,  TIMER_FE_HANDLE_SET_CONFIG);
			recv_set_config();
			break;
		case TIMER_FE_HANDLE_EXPERIMENTER:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_VENDOR (%d) expired", this,  TIMER_FE_HANDLE_EXPERIMENTER);
			recv_experimenter_message();
			break;
		case TIMER_FE_HANDLE_QUEUE_GET_CONFIG_REQUEST:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_SEND_QUEUE_GET_CONFIG_REQUEST (%d) expired", this,  TIMER_FE_HANDLE_QUEUE_GET_CONFIG_REQUEST);
			recv_queue_get_config_request();
			break;
		case TIMER_FE_HANDLE_QUEUE_GET_CONFIG_REPLY:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_SEND_QUEUE_GET_CONFIG_REPLY (%d) expired", this,  TIMER_FE_HANDLE_QUEUE_GET_CONFIG_REPLY);
			recv_queue_get_config_reply();
			break;
		case TIMER_FE_HANDLE_BARRIER_REQUEST:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_BARRIER_REQUEST (%d) expired", this,  TIMER_FE_HANDLE_BARRIER_REQUEST);
			recv_barrier_request();
			break;
		case TIMER_FE_HANDLE_PORT_STATUS:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_PORT_STATUS (%d) expired", this,  TIMER_FE_HANDLE_PORT_STATUS);
			recv_port_status();
			break;
		case TIMER_FE_HANDLE_ROLE_REQUEST:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_ROLE_REQUEST (%d) expired", this,  TIMER_FE_HANDLE_ROLE_REQUEST);
			recv_role_request();
			break;
		case TIMER_FE_HANDLE_ROLE_REPLY:
			WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_ROLE_REPLY (%d) expired", this,  TIMER_FE_HANDLE_ROLE_REPLY);
			recv_role_reply();
			break;
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



	} catch (eOFbaseNoCtrl& e) {
		WRITELOG(CROFBASE, DBG, "controlling entity lost");
		// handle NoCtrl condition: simply do nothing for now,
		// TODO: reconnect to new controlling entity

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
crofbase::dpath_find(uint64_t dpid) throw (eOFbaseNotAttached)
{
	std::map<cofbase*, cofdpt*>::iterator it;
	for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		if (it->second->dpid == dpid)
			return *(it->second);
	}
	throw eOFbaseNotAttached();
}


cofdpt&
crofbase::dpath_find(std::string s_dpid) throw (eOFbaseNotAttached)
{
	std::map<cofbase*, cofdpt*>::iterator it;
	for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		if (it->second->s_dpid == s_dpid)
			return *(it->second);
	}
	throw eOFbaseNotAttached();
}


cofdpt&
crofbase::dpath_find(cmacaddr dl_dpid) throw (eOFbaseNotAttached)
{
	std::map<cofbase*, cofdpt*>::iterator it;
	for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		if (it->second->dpmac == dl_dpid)
			return *(it->second);
	}
	throw eOFbaseNotAttached();
}


cofdpt*
crofbase::ofswitch_find(cofbase *entity) throw (eOFbaseNotAttached)
{
	if (ofdpath_list.find(entity) == ofdpath_list.end())
		throw eOFbaseNotAttached();
	return ofdpath_list[entity];
}


cofdpt*
crofbase::ofswitch_find(cofdpt *entity) throw (eOFbaseNotAttached)
{
	for (std::map<cofbase*, cofdpt*>::iterator
			it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		if (it->second == entity)
		{
			return entity;
		}
	}
	throw eOFbaseNotAttached();
}


cofctl*
crofbase::ofctrl_find(cofbase *entity) throw (eOFbaseNotAttached)
{
	if (ofctrl_list.find(entity) == ofctrl_list.end())
		throw eOFbaseNotAttached();
	return ofctrl_list[entity];
}


cofctl*
crofbase::ofctrl_find(cofctl *entity) throw (eOFbaseNotAttached)
{
	for (std::map<cofbase*, cofctl*>::iterator
			it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
	{
		if (it->second == entity)
		{
			return entity;
		}
	}
	throw eOFbaseNotAttached();
}



inline
void
crofbase::check_up_packet(
		cofpacket *ofpacket,
		enum ofp_type oftype,
		cofbase *ofbase) throw (eRofBaseInval)
{
	try {
		ofswitch_find(ofbase);
	} catch (eOFbaseNotAttached& e) {
		dpath_attach(ofbase);
	}

	try {
		ofpacket->entity = ofswitch_find(ofbase)->entity;
	} catch (eOFbaseNotAttached& e) {}

	if ((not ofpacket->is_valid()) || (ofpacket->ofh_header->type != oftype))
	{
		throw eRofBaseInval();
	}
}


inline
void
crofbase::check_down_packet(
		cofpacket *ofpacket,
		enum ofp_type oftype,
		cofbase *ofbase) throw (eRofBaseInval)
{
	try {
		//fprintf(stderr, "BLUB[1.1]: %s\n\n", ofpacket->c_str());

		ofctrl_find(ofbase);

		//fprintf(stderr, "BLUB[1.2]: %s\n\n", ofpacket->c_str());
	} catch (eOFbaseNotAttached& e) {
		//fprintf(stderr, "BLUB[2.1]: %s\n\n", ofpacket->c_str());
		ctrl_attach(ofbase);
		//fprintf(stderr, "BLUB[2.2]: %s\n\n", ofpacket->c_str());
	}

	try {
		//fprintf(stderr, "BLUB[3.1]: %s\n\n", ofpacket->c_str());
		ofpacket->entity = ofctrl_find(ofbase)->ctrl;
		//fprintf(stderr, "BLUB[3.2]: %s\n\n", ofpacket->c_str());
	} catch (eOFbaseNotAttached& e) {}

	//fprintf(stderr, "BLUB[4.1]: %s\n\n", ofpacket->c_str());
	if ((not ofpacket->is_valid()) || (ofpacket->ofh_header->type != oftype))
	{
		//fprintf(stderr, "BLUB[EXCEPTION]: %s\n", ofpacket->c_str());
		throw eRofBaseInval();
	}
	//fprintf(stderr, "BLUB[4.2]: %s\n\n", ofpacket->c_str());
}




/*
* HELLO messages
*/

void
crofbase::send_up_hello_message(
	cofctl *ofctrl, bool bye)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_up_hello_message()", this);

	uint32_t hello = (bye) ? htobe32(FE_HELLO_BYE) : htobe32(FE_HELLO_ACTIVE);

	cofpacket_hello *pack = new cofpacket_hello(
										ta_new_async_xid(),
										(uint8_t*)&hello, sizeof(hello));

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_up_hello_message() new %s", this,
			pack->c_str());

	ofctrl_find(ofctrl)->ctrl->fe_up_hello_message(this, pack);
}


void
crofbase::send_down_hello_message(
	cofdpt *ofdpath, bool bye)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_down_hello_message()", this);

	uint32_t hello = (bye) ? htobe32(FE_HELLO_BYE) : htobe32(FE_HELLO_ACTIVE);

	cofpacket_hello *pack = new cofpacket_hello(
										ta_new_async_xid(),
										(uint8_t*)&hello, sizeof(hello));

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_down_hello_message() new %s", this,
			pack->c_str());

	ofdpath->entity->fe_down_hello_message(this, pack);
}


void
crofbase::fe_down_hello_message(
		cofbase *entity,
		cofpacket *pack)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_hello_message() HELLO received: %s", this, pack->c_str());

	try {
		check_down_packet(pack, OFPT_HELLO, entity);

		fe_down_queue[OFPT_HELLO].push_back(pack); // store pack for xid
		register_timer(TIMER_FE_HANDLE_HELLO, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_hello_message() packet from non-controlling entity dropped", this);

		////entity->ctrl_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_hello_message() malformed packet received", this);
		delete pack;
	} catch (eRofBaseFspSupportDisabled& e) {

		WRITELOG(CROFBASE, DBG, "\n\ncrofbase(%p)::fe_down_hello_message() "
				"flowspace support for multiple controllers DISABLED!!!\n\n", this);
				delete pack;
	}
}


void
crofbase::fe_up_hello_message(
		cofbase *entity,
		cofpacket *pack)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_hello_message() HELLO received: %s", this, pack->c_str());

	try {
		check_up_packet(pack, OFPT_HELLO, entity);

		fe_up_queue[OFPT_HELLO].push_back(pack); // store pack for xid
		register_timer(TIMER_FE_HANDLE_HELLO, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_hello_message() packet from non-controlling entity dropped", this);

		////entity->dpath_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_hello_message() malformed packet received", this);
		delete pack;
	}
}


void
crofbase::recv_hello_message(
		cofctl *ctl,
		cofpacket *pack)
{
	cofpacket *pack = NULL;
	try {

		if (not fe_down_queue[OFPT_HELLO].empty())
		{
			pack = fe_down_queue[OFPT_HELLO].front();
			fe_down_queue[OFPT_HELLO].pop_front();

			uint32_t cookie = 0;

			if (pack->body.memlen() >= sizeof(uint32_t))
			{
				memcpy(&cookie, pack->body.somem(), sizeof(uint32_t));
				cookie = be32toh(cookie);
			}

			WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_hello_message() down pack:%s cookie:%s",
					this, pack->c_str(), cookie == FE_HELLO_ACTIVE ? "FE_HELLO_ACTIVE" : "FE_HELLO_BYE");

			switch (cookie) {
			case FE_HELLO_BYE:
				// deregister controller
				try {
					delete ofctrl_find(pack->entity);
				} catch (eOFbaseNotAttached& e) {}
				break;

			case FE_HELLO_ACTIVE:
			default:

				break;
			}

			//ofctrl_find(pack->entity)->experimenter_message_rcvd(pack);
		}

		if (not fe_up_queue[OFPT_HELLO].empty())
		{
			pack = fe_up_queue[OFPT_HELLO].front();
			fe_up_queue[OFPT_HELLO].pop_front();

			uint32_t cookie = 0;

			if (pack->body.memlen() >= sizeof(uint32_t))
			{
				memcpy(&cookie, pack->body.somem(), sizeof(uint32_t));
				cookie = be32toh(cookie);
			}

			WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_hello_message() up pack:%s cookie:%s",
					this, pack->c_str(), cookie == FE_HELLO_ACTIVE ? "FE_HELLO_ACTIVE" : "FE_HELLO_BYE");

			switch (cookie) {
			case FE_HELLO_BYE:
				// deregister datapath
				delete ofswitch_find(pack->entity);
				break;

			case FE_HELLO_ACTIVE:
			default:

				break;
			}

			//ofswitch_find(pack->entity)->experimenter_message_rcvd(pack);
		}

	} catch (...) {

	}

	delete pack;

	if (not fe_down_queue[OFPT_HELLO].empty()
			or
		not fe_up_queue[OFPT_HELLO].empty())
	{
		register_timer(TIMER_FE_HANDLE_HELLO, 0); // reschedule ourselves
	}
}

/*
 * FEATURES request/reply
 */

void
crofbase::send_features_request(cofdpt *dpath)
{

	cofpacket_features_request *pack = new cofpacket_features_request(ta_add_request(OFPT_FEATURES_REQUEST));

	// straight call to layer-(n-1) entity's fe_down_features_request() method
	dpath->entity->fe_down_features_request(this, pack);

	dpath->features_request_sent();
}


void
crofbase::fe_down_features_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_features_request() FEATURES-REQUEST received: %s", this, pack->c_str());

	try {
		check_down_packet(pack, OFPT_FEATURES_REQUEST, entity);

		fe_down_queue[OFPT_FEATURES_REQUEST].push_back(pack); // store pack for xid
		register_timer(TIMER_FE_HANDLE_FEATURES_REQUEST, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_features_request() packet from non-controlling entity dropped", this);

		////entity->dpath_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_features_request() malformed packet received", this);
		delete pack;
	}
}


void
crofbase::recv_features_request()
{
	if (fe_down_queue[OFPT_FEATURES_REQUEST].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_FEATURES_REQUEST].front();
		fe_down_queue[OFPT_FEATURES_REQUEST].pop_front();

		ofctrl_find(pack->entity)->features_request_rcvd(pack);



	} catch (...) {

		delete pack;
	}

	if (not fe_down_queue[OFPT_FEATURES_REQUEST].empty())
	{
		register_timer(TIMER_FE_HANDLE_FEATURES_REQUEST, 0); // reschedule ourselves
	}
}




void
crofbase::send_features_reply(
		cofctl *ofctrl,
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

	ofctrl->features_reply_sent(reply);

	ofctrl_find(ofctrl)->ctrl->fe_up_features_reply(this, reply);
}


void
crofbase::fe_up_features_reply(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_features_reply() FEATURES-REPLY received: %s", this, pack->c_str());

	try {
		//CHECK_PACKET(pack, OFPT_FEATURES_REPLY);
		check_up_packet(pack, OFPT_FEATURES_REPLY, entity);

		pack->entity = entity;
		ta_validate(be32toh(pack->ofh_header->xid), OFPT_FEATURES_REQUEST);

		cofdpt *sw = ofswitch_find(entity);
		sw->features_reply_rcvd(pack);
		handle_features_reply(sw, pack);

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_features_reply() malformed packet received", this);

		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_features_reply() invalid session exchange xid "
					"(%u) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached &e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_features_reply() packet received from non-attached entity", this);

		delete pack;
	}
}



/*
 * GET-CONFIG request/reply
 */

void
crofbase::send_get_config_request(
		cofdpt *sw)
{
	cofpacket_get_config_request *pack = new cofpacket_get_config_request(
								ta_add_request(OFPT_GET_CONFIG_REQUEST));

	// straight call to layer-(n-1) entity's fe_down_get_config_request() method
	ofswitch_find(sw)->entity->fe_down_get_config_request(this, pack);

	sw->get_config_request_sent();

}


void
crofbase::fe_down_get_config_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_GET_CONFIG_REQUEST, entity);

		fe_down_queue[OFPT_GET_CONFIG_REQUEST].push_back(pack);	// store pack for xid
		register_timer(TIMER_FE_HANDLE_GET_CONFIG_REQUEST, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_get_config_request() packet from non-controlling entity dropped", this);

		////entity->dpath_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_get_config_request() malformed packet received", this);
		delete pack;
	}
}



void
crofbase::recv_get_config_request()
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_get_config_request()", this);

	if (fe_down_queue[OFPT_GET_CONFIG_REQUEST].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_GET_CONFIG_REQUEST].front();
		fe_down_queue[OFPT_GET_CONFIG_REQUEST].pop_front();

		ofctrl_find(pack->entity)->get_config_request_rcvd(pack);



	} catch (...) {

		delete pack;
	}

	if (not fe_down_queue[OFPT_GET_CONFIG_REQUEST].empty())
	{
		register_timer(TIMER_FE_HANDLE_GET_CONFIG_REQUEST, 0); // reschedule ourselves
	}
}



void
crofbase::send_get_config_reply(cofctl *ofctrl, uint32_t xid, uint16_t flags, uint16_t miss_send_len)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_get_config_reply()", this);

	cofpacket_get_config_reply *pack = new cofpacket_get_config_reply(xid, flags, miss_send_len);

	ofctrl->ctrl->fe_up_get_config_reply(this, pack);
}


void
crofbase::fe_up_get_config_reply(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);


	WRITELOG(CROFBASE, DBG, "GET-CONFIG-REPLY received: %s", pack->c_str());

	try {
		//CHECK_PACKET(pack, OFPT_GET_CONFIG_REPLY);
		check_up_packet(pack, OFPT_GET_CONFIG_REPLY, entity);

		ta_validate(be32toh(pack->ofh_header->xid), OFPT_GET_CONFIG_REQUEST);

		ofswitch_find(entity)->get_config_reply_rcvd(pack);


	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_get_config_reply() malformed packet received", this);

		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_get_config_reply() invalid session exchange xid "
					"(%u) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_get_config_reply() packet received from non-attached entity", this);

		delete pack;
	}
}



/*
 * STATS request/reply
 */

uint32_t
crofbase::send_stats_request(
	cofdpt *sw,
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

	// straight call to layer-(n-1) entity's fe_down_get_config_request() method
	ofswitch_find(sw)->entity->fe_down_stats_request(this, pack);

	sw->stats_request_sent(xid);

	return xid;
}


void
crofbase::fe_down_stats_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

#ifndef NDEBUG
	// WRITELOG(CROFBASE, DBG, "dpid:%u STATS-REQUEST received: %s", dpid, pack->c_str());
#endif
	try {
		check_down_packet(pack, OFPT_STATS_REQUEST, entity);

		fe_down_queue[OFPT_STATS_REQUEST].push_back(pack);
		register_timer(TIMER_FE_HANDLE_STATS_REQUEST, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_stats_request() packet from non-controlling entity dropped", this);

		////entity->dpath_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_stats_request() malformed packet received", this);
		delete pack;
	}
}



void
crofbase::recv_stats_request()
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_stats_request()", this);

	if (fe_down_queue[OFPT_STATS_REQUEST].empty())
		return;

	cofpacket *request = fe_down_queue[OFPT_STATS_REQUEST].front();
	fe_down_queue[OFPT_STATS_REQUEST].pop_front();

	ofctrl_find(request->entity)->stats_request_rcvd(request);

	// further stats request available? re-schedule ourselves
	if (not fe_down_queue[OFPT_STATS_REQUEST].empty())
	{
		register_timer(TIMER_FE_HANDLE_STATS_REQUEST, 0);
	}
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
		cofctl *ofctrl,
		uint32_t xid, /* network byte order */
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

	ofctrl_find(ofctrl)->stats_reply_sent(pack);

	ofctrl_find(ofctrl)->ctrl->fe_up_stats_reply(this, pack);
}


void
crofbase::fe_up_stats_reply(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

#ifndef NDEBUG
	WRITELOG(CROFBASE, DBG, "STATS-REPLY received: %s", pack->c_str());
#endif
	try {
		//CHECK_PACKET(pack, OFPT_STATS_REPLY);
		check_up_packet(pack, OFPT_STATS_REPLY, entity);

		pack->entity = entity;
		ta_validate(be32toh(pack->ofh_header->xid), OFPT_STATS_REQUEST);
		cofdpt *sw = ofswitch_find(entity);

		sw->stats_reply_rcvd(pack);
		handle_stats_reply(sw, pack);

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_stats_reply() malformed packet received", this);

		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_stats_reply() invalid session exchange xid "
					"(0x%x) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_stats_reply() packet received from non-attached entity", this);

		delete pack;
	}
}




/*
 * SET-CONFIG message
 */


void
crofbase::send_set_config_message(
	cofdpt *sw,
	uint16_t flags,
	uint16_t miss_send_len)
{
	cofpacket *pack = NULL;
	try {

		size_t packlen = sizeof(struct ofp_switch_config);

		pack = new cofpacket(packlen, packlen);

		pack->ofh_header->version = OFP_VERSION;
		pack->ofh_header->type = OFPT_SET_CONFIG;
		pack->ofh_header->length = htobe16(packlen);
		pack->ofh_header->xid = htobe32(ta_new_async_xid());

		pack->ofh_switch_config->flags = htobe16(flags);
		pack->ofh_switch_config->miss_send_len = htobe16(miss_send_len);

		// straight call to layer-(n-1) entity's fe_down_set_config_request() method
		ofswitch_find(sw)->entity->fe_down_set_config_request(this, pack);

	} catch (eOFbaseIsBusy& e) {
		WRITELOG(CROFBASE, DBG, "datapath entity (%llu) busy", sw->dpid);
		delete pack;
		throw;
	}
}


void
crofbase::fe_down_set_config_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_SET_CONFIG, entity);

		fe_down_queue[OFPT_SET_CONFIG].push_back(pack);
		register_timer(TIMER_FE_HANDLE_SET_CONFIG, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_set_config_request() packet from non-controlling entity dropped", this);

		//entity->dpath_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_set_config_request() malformed packet received", this);
		delete pack;
	}
}


void
crofbase::recv_set_config()
{
	if (fe_down_queue[OFPT_SET_CONFIG].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_SET_CONFIG].front();
		fe_down_queue[OFPT_SET_CONFIG].pop_front();

		ofctrl_find(pack->entity)->set_config_rcvd(pack);

	} catch (...) {
		delete pack;
	}

	if (not fe_down_queue[OFPT_SET_CONFIG].empty())
	{
		register_timer(TIMER_FE_HANDLE_SET_CONFIG, 0); // reschedule ourselves
	}
}




/*
 * PACKET-OUT message
 */


void
crofbase::send_packet_out_message(
	cofdpt *sw,
	uint32_t buffer_id,
	uint32_t in_port,
	cofaclist& aclist,
	uint8_t *data,
	size_t datalen)
{


	cofpacket_packet_out *pack = new cofpacket_packet_out(
											ta_new_async_xid(),
											buffer_id,
											in_port,
											data,
											datalen);

	pack->actions = aclist;

	//pack->pack();

	// straight call to layer-(n-1) entity's fe_down_set_config_request() method
	ofswitch_find(sw)->entity->fe_down_packet_out(this, pack);
}


void
crofbase::fe_down_packet_out(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_packet_out() PACKET-OUT received: %s", this, pack->c_str());

	try {
		check_down_packet(pack, OFPT_PACKET_OUT, entity);

		fe_down_queue[OFPT_PACKET_OUT].push_back(pack);
		register_timer(TIMER_FE_HANDLE_PACKET_OUT, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_packet_out() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_packet_out() malformed packet received");
		delete pack;

	} catch (eActionBadLen& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::handle_packet_out() "
				 "bad action len", this);
		send_error_message(ofctrl_find(entity), pack->get_xid(), OFPET_BAD_ACTION, OFPBAC_BAD_LEN,
				(uint8_t*)pack->soframe(), pack->framelen());
		delete pack;
	}

}


void
crofbase::recv_packet_out()
{
	if (fe_down_queue[OFPT_PACKET_OUT].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_PACKET_OUT].front();
		fe_down_queue[OFPT_PACKET_OUT].pop_front();

		ofctrl_find(pack->entity)->packet_out_rcvd(pack);

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_packet_out() "
				"exception on handle_packet_out() caught", this);
		delete pack;
	}

	if (not fe_down_queue[OFPT_PACKET_OUT].empty())
	{
		register_timer(TIMER_FE_HANDLE_PACKET_OUT, 0); // reschedule ourselves
	}
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
				"ofctrl_list.size()=%d", this, ofctrl_list.size());

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
					ofctrl_find(ofctrl)->ctrl->fe_up_packet_in(this, pack);
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
			if (ofctrl_list.empty())
			{
				throw eRofBaseNoCtrl();
			}

			cofctl *ofctrl = (ofctrl_list.begin()->second);




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
			ofctrl_find(ofctrl)->ctrl->fe_up_packet_in(this, pack);
		}

	} catch (eOFbaseNotFound& e) {

	}
}



void
crofbase::fe_up_packet_in(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_packet_in() "
			"PACKET-IN received: %s", this, pack->c_str());

	try {
		check_up_packet(pack, OFPT_PACKET_IN, entity);

		pack->entity = entity;
		fe_up_queue[OFPT_PACKET_IN].push_back(pack);
		register_timer(TIMER_FE_HANDLE_PACKET_IN, 0);

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_packet_in() malformed packet received", this);
		delete pack;
	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_packet_in() PACKET-IN from non registered entity %s", this, pack->c_str());
		delete pack;
	}
}


void
crofbase::recv_packet_in()
{
	if (fe_up_queue[OFPT_PACKET_IN].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_up_queue[OFPT_PACKET_IN].front();
		fe_up_queue[OFPT_PACKET_IN].pop_front();

		// update cofswitch internal state (e.g. fwdtable, ...)
		ofswitch_find(pack->entity)->packet_in_rcvd(pack);

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "packet received from non-attached entity");

		delete pack;

	} catch (...) {
		delete pack;

	}

	if (not fe_up_queue[OFPT_PACKET_IN].empty())
	{
		register_timer(TIMER_FE_HANDLE_PACKET_IN, 0); // reschedule ourselves
	}
}




/*
 * BARRIER request/reply
 */

uint32_t
crofbase::send_barrier_request(cofdpt *sw)
{
	cofpacket_barrier_request *pack = new cofpacket_barrier_request(ta_add_request(OFPT_BARRIER_REQUEST));

	uint32_t xid = be32toh(pack->ofh_header->xid);

	// straight call to layer-(n-1) entity's fe_down_barrier_request() method
	ofswitch_find(sw)->entity->fe_down_barrier_request(this, pack);

	sw->barrier_request_sent(xid);

	return xid;
}



void
crofbase::fe_down_barrier_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_BARRIER_REQUEST, entity);

		fe_down_queue[OFPT_BARRIER_REQUEST].push_back(pack);	// store pack for xid
		register_timer(TIMER_FE_HANDLE_BARRIER_REQUEST, 0);


	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_barrier_request() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_barrier_request() malformed packet received", this);
		delete pack;
	}
}



void
crofbase::recv_barrier_request()
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_barrier_request() ", this);

	if (fe_down_queue[OFPT_BARRIER_REQUEST].empty())
		return;

	cofpacket *request = fe_down_queue[OFPT_BARRIER_REQUEST].front();
	fe_down_queue[OFPT_BARRIER_REQUEST].pop_front();

	ofctrl_find(request->entity)->barrier_request_rcvd(request);




	if (not fe_down_queue[OFPT_BARRIER_REQUEST].empty())
	{
		register_timer(TIMER_FE_HANDLE_BARRIER_REQUEST, 0); // reschedule ourselves
	}
}



void
crofbase::send_barrier_reply(
		cofctl* ofctrl,
		uint32_t xid)
{
	WRITELOG(CROFBASE, DBG, "crofbase::send_barrier_reply() "
			"fe_down_queue[OFPT_BARRIER_REQUEST].size()=%d", fe_down_queue[OFPT_BARRIER_REQUEST].size());

	cofpacket_barrier_reply *pack = new cofpacket_barrier_reply(xid);

	ofctrl_find(ofctrl)->barrier_reply_sent(pack);

	// request is deleted by derived class
	ofctrl_find(ofctrl)->ctrl->fe_up_barrier_reply(this, pack);
}



void
crofbase::fe_up_barrier_reply(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_barrier_reply() ", this);

	try {
		//CHECK_PACKET(pack, OFPT_BARRIER_REPLY);
		check_up_packet(pack, OFPT_BARRIER_REPLY, entity);

		pack->entity = entity;
		ta_validate(be32toh(pack->ofh_header->xid), OFPT_BARRIER_REQUEST);

		ofswitch_find(pack->entity)->barrier_reply_rcvd(pack);

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_barrier_reply() malformed packet received", this);

		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_barrier_reply() invalid session exchange xid "
					"(%u) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_barrier_reply() packet received from non-attached entity", this);

		delete pack;
	}
}


/*
 * ROLE.request/reply
 */

void
crofbase::send_role_request(
	cofdpt *dpath,
	uint32_t role,
	uint64_t generation_id)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_role_request()", this);

	cofpacket_role_request *pack = new cofpacket_role_request(
										ta_add_request(OFPT_ROLE_REQUEST),
										role,
										generation_id);

	dpath->entity->fe_down_role_request(this, pack);
}


void
crofbase::fe_down_role_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_ROLE_REQUEST, entity);

		fe_down_queue[OFPT_ROLE_REQUEST].push_back(pack);	// store pack for xid
		register_timer(TIMER_FE_HANDLE_ROLE_REQUEST, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_role_request() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_role_request() malformed packet received", this);
		delete pack;
	}
}


void
crofbase::recv_role_request()
{
	if (fe_down_queue[OFPT_ROLE_REQUEST].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_ROLE_REQUEST].front();
		fe_down_queue[OFPT_ROLE_REQUEST].pop_front();

		ofctrl_find(pack->entity)->role_request_rcvd(pack);

	} catch (...) {
		delete pack;
	}

	if (not fe_down_queue[OFPT_ROLE_REQUEST].empty())
	{
		register_timer(TIMER_FE_HANDLE_ROLE_REQUEST, 0); // reschedule ourselves
	}
}


void
crofbase::send_role_reply(
		cofctl *ofctrl,
		uint32_t xid,
		uint32_t role,
		uint64_t generation_id)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_role_reply()", this);

	cofpacket_role_reply *pack = new cofpacket_role_reply(
										xid,
										role,
										generation_id);

	ofctrl_find(ofctrl)->role_reply_sent(pack);

	// request is deleted by derived class
	ofctrl_find(ofctrl)->ctrl->fe_up_role_reply(this, pack);
}


void
crofbase::fe_up_role_reply(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_up_packet(pack, OFPT_ROLE_REPLY, entity);

		pack->entity = entity;
		ta_validate(be32toh(pack->ofh_header->xid), OFPT_ROLE_REQUEST);
		register_timer(TIMER_FE_HANDLE_ROLE_REPLY, 0);

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_role_reply() malformed packet received", this);

		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_role_reply() invalid session exchange xid "
					"(%u) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_role_reply() packet received from non-attached entity", this);

		delete pack;
	}
}


void
crofbase::recv_role_reply()
{
	if (fe_up_queue[OFPT_ROLE_REPLY].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_up_queue[OFPT_ROLE_REPLY].front();
		fe_up_queue[OFPT_ROLE_REPLY].pop_front();

		ofswitch_find(pack->entity)->role_reply_rcvd(pack);

	} catch (...) {
		delete pack;
	}

	if (not fe_up_queue[OFPT_ROLE_REPLY].empty())
	{
		register_timer(TIMER_FE_HANDLE_ROLE_REPLY, 0); // reschedule ourselves
	}
}




/*
 * ERROR message
 */


void
crofbase::send_error_message(
	cofctl *ofctrl,
	uint32_t xid,
	uint16_t type,
	uint16_t code,
	uint8_t* data,
	size_t datalen)
{
	WRITELOG(CROFBASE, DBG, "crofbase::send_error_message()");

	xid = (xid == 0) ? ta_new_async_xid() : xid;

	if (0 != ofctrl)
	{
		cofpacket_error *pack = new cofpacket_error(xid, type, code, data, datalen);

		// straight call to layer-(n+1) entity's fe_up_packet_in() method
		ofctrl_find(ofctrl)->ctrl->fe_up_error(this, pack);
	}
	else
	{
		std::map<cofbase*, cofctl*>::iterator it;
		for (it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
		{
			cofpacket_error *pack = new cofpacket_error(xid, type, code, data, datalen);

			// straight call to layer-(n+1) entity's fe_up_packet_in() method
			it->first->fe_up_error(this, pack);
		}
	}
}


void
crofbase::fe_up_error(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		//CHECK_PACKET(pack, OFPT_ERROR);
		check_up_packet(pack, OFPT_ERROR, entity);

		pack->entity = entity;
		fe_up_queue[OFPT_ERROR].push_back(pack);
		register_timer(TIMER_FE_HANDLE_ERROR, 0);

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_error() malformed packet received", this);
		delete pack;
	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_error() packet received from non-attached entity", this);
		delete pack;
	}
}


void
crofbase::recv_error()
{
	if (fe_up_queue[OFPT_ERROR].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_up_queue[OFPT_ERROR].front();
		fe_up_queue[OFPT_ERROR].pop_front();

		handle_error(ofswitch_find(pack->entity), pack);

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "packet received from non-attached entity");

		delete pack;

	} catch (...) {
		delete pack;

	}

	if (not fe_up_queue[OFPT_ERROR].empty())
	{
		register_timer(TIMER_FE_HANDLE_ERROR, 0); // reschedule ourselves
	}
}




/*
 * FLOW-MOD message
 */


void
crofbase::send_flow_mod_message(
	cofdpt *sw,
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
	cofpacket_flow_mod *flow_mod = new cofpacket_flow_mod(
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

	flow_mod->match = ofmatch;
	flow_mod->instructions = inlist;

	flow_mod->pack();

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_mod_message() pack: %s", this, flow_mod->c_str());

	// straight call to layer-(n-1) entity's fe_down_flow_mod() method
	ofswitch_find(sw)->entity->fe_down_flow_mod(this, flow_mod);

	sw->flow_mod_sent(flow_mod);


}


void
crofbase::send_flow_mod_message(
		cofdpt *sw,
		cflowentry& fe)
{
	cofpacket_flow_mod *flow_mod = new cofpacket_flow_mod(
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

	flow_mod->match = fe.match;
	flow_mod->instructions = fe.instructions;

	flow_mod->pack();

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_mod_message() pack: %s",
			this, flow_mod->c_str());

	// straight call to layer-(n-1) entity's fe_down_flow_mod() method
	ofswitch_find(sw)->entity->fe_down_flow_mod(this, flow_mod);

	sw->flow_mod_sent(flow_mod);


}


void
crofbase::fe_down_flow_mod(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_FLOW_MOD, entity);

		fe_down_queue[OFPT_FLOW_MOD].push_back(pack);
		register_timer(TIMER_FE_HANDLE_FLOW_MOD, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "packet from non-controlling entity dropped");
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_flow_mod() malformed packet received", this);
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_flow_mod() flow-mod from non-attached entity received", this);
		delete pack;

	} catch (eActionBadLen& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_flow_mod() "
				 "bad action len", this);
		send_error_message(ofctrl_find(entity), pack->get_xid(), OFPET_BAD_ACTION, OFPBAC_BAD_LEN,
				(uint8_t*)pack->soframe(), pack->framelen());
		delete pack;
	}
}



void
crofbase::recv_flow_mod()
{
	if (fe_down_queue[OFPT_FLOW_MOD].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_FLOW_MOD].front();
		fe_down_queue[OFPT_FLOW_MOD].pop_front();

		ofctrl_find(pack->entity)->flow_mod_rcvd(pack);




	} catch (eActionBadLen& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"invalid flow-mod packet received: action with bad length", this);

		send_error_message(
					ofctrl_find(pack->entity),
					pack->get_xid(),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_LEN,
					pack->soframe(), pack->framelen());

		delete pack;


	} catch (eFspNotAllowed& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"-FLOW-MOD- blocked due to mismatch in nsp "
				"registration", this);

		send_error_message(
				ofctrl_find(pack->entity),
				pack->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_EPERM,
				pack->soframe(), pack->framelen());

		delete pack;

	} catch (eRofBaseTableNotFound& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"invalid flow-table %d specified",
				this, pack->ofh_flow_mod->table_id);

		send_error_message(
				ofctrl_find(pack->entity),
				pack->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_BAD_TABLE_ID,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eInstructionInvalType& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"unknown instruction found", this);

		send_error_message(
				ofctrl_find(pack->entity),
				pack->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNKNOWN_INST,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eRofBaseGotoTableNotFound& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"GOTO-TABLE instruction with invalid table-id", this);

		send_error_message(
				ofctrl_find(pack->entity),
				pack->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_BAD_TABLE_ID,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eInstructionBadExperimenter& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"unknown OFPIT_EXPERIMENTER extension received", this);

		send_error_message(
				ofctrl_find(pack->entity),
				pack->get_xid(),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNSUP_EXP_INST,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eOFmatchInvalBadValue& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"bad value in match structure", this);

		send_error_message(
				ofctrl_find(pack->entity),
				pack->get_xid(),
				OFPET_BAD_MATCH,
				OFPBMC_BAD_VALUE,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eOFbaseNotAttached& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"eOFbaseNotAttached thrown", this);

		delete pack;

	} catch (cerror &e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_flow_mod() "
				"default catch for cerror exception", this);

		send_error_message(
				ofctrl_find(pack->entity),
				pack->get_xid(),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_UNKNOWN,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	}



	if (not fe_down_queue[OFPT_FLOW_MOD].empty())
	{
		register_timer(TIMER_FE_HANDLE_FLOW_MOD, 0); // reschedule ourselves
	}
}




/*
 * GROUP-MOD message
 */


void
crofbase::send_group_mod_message(
		cofdpt *sw,
		cgroupentry& ge)
{
	cofpacket_group_mod *pack = new cofpacket_group_mod(
										ta_new_async_xid(),
										be16toh(ge.group_mod->command),
										ge.group_mod->type,
										be32toh(ge.group_mod->group_id));

	pack->buckets = ge.buckets;

	pack->pack();

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_group_mod_message() %s", this, pack->c_str());

	// straight call to layer-(n-1) entity's fe_down_flow_mod() method
	ofswitch_find(sw)->entity->fe_down_group_mod(this, pack);

	sw->group_mod_sent(pack);

}


void
crofbase::fe_down_group_mod(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_GROUP_MOD, entity);

		fe_down_queue[OFPT_GROUP_MOD].push_back(pack);
		register_timer(TIMER_FE_HANDLE_GROUP_MOD, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "packet from non-controlling entity dropped");
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_flow_mod() malformed packet received", this);
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_flow_mod() flow-mod from non-attached entity received", this);
		delete pack;

	} catch (eActionBadLen& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_flow_mod() "
				 "bad action len", this);
		send_error_message(ofctrl_find(entity), pack->get_xid(), OFPET_BAD_ACTION, OFPBAC_BAD_LEN,
				(uint8_t*)pack->soframe(), pack->framelen());
		delete pack;
	}
}


void
crofbase::recv_group_mod()
{
	if (fe_down_queue[OFPT_GROUP_MOD].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_GROUP_MOD].front();
		fe_down_queue[OFPT_GROUP_MOD].pop_front();

		ofctrl_find(pack->entity)->group_mod_rcvd(pack);



	} catch (eActionBadLen& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_group_mod() "
				"invalid group-mod packet received: action with "
				"bad length", this);

		send_error_message(
				ofctrl_find(pack->entity),
				pack->get_xid(),
				OFPET_BAD_ACTION,
				OFPBAC_BAD_LEN,
				pack->soframe(),
				pack->framelen());

		delete pack;

	} catch (...) {

		delete pack;
	}

	if (not fe_down_queue[OFPT_GROUP_MOD].empty())
	{
		register_timer(TIMER_FE_HANDLE_GROUP_MOD, 0); // reschedule ourselves
	}
}




/*
 * PORT-MOD message
 */


void
crofbase::send_port_mod_message(
	cofdpt *sw,
	uint32_t port_no,
	cmacaddr const& hwaddr,
	uint32_t config,
	uint32_t mask,
	uint32_t advertise)
{
	cofpacket_port_mod *pack = new cofpacket_port_mod(
										ta_new_async_xid(),
										port_no,
										hwaddr,
										config,
										mask,
										advertise);

	// straight call to layer-(n-1) entity's fe_down_flow_mod() method
	ofswitch_find(sw)->entity->fe_down_port_mod(this, pack);

	sw->port_mod_sent(pack);
}


void
crofbase::fe_down_port_mod(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "crofbase(%p)::fe_down_port_mod()", this);

	try {
		check_down_packet(pack, OFPT_PORT_MOD, entity);

		fe_down_queue[OFPT_PORT_MOD].push_back(pack);
		register_timer(TIMER_FE_HANDLE_PORT_MOD, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_port_mod() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_port_mod() malformed packet received", this);
		delete pack;
	}
}


void
crofbase::recv_port_mod()
{
	if (fe_down_queue[OFPT_PORT_MOD].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_PORT_MOD].front();
		fe_down_queue[OFPT_PORT_MOD].pop_front();

		ofctrl_find(pack->entity)->port_mod_rcvd(pack);



	} catch (eOFctrlPortNotFound& e) {

		send_error_message(
				ofctrl_find(pack->entity),
				pack->get_xid(),
				OFPET_BAD_REQUEST,
				OFPBRC_BAD_PORT,
				pack->soframe(), pack->framelen());

		delete pack;

	} catch (...) {

		delete pack;
	}

	if (not fe_down_queue[OFPT_PORT_MOD].empty())
	{
		register_timer(TIMER_FE_HANDLE_PORT_MOD, 0); // reschedule ourselves
	}
}




/*
 * TABLE-MOD message
 */


void
crofbase::send_table_mod_message(
		cofdpt *sw,
		uint8_t table_id,
		uint32_t config)
{
	cofpacket_table_mod *pack = new cofpacket_table_mod(
										ta_new_async_xid(),
										table_id,
										config);

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_table_mod_message() %s", this, pack->c_str());

	// straight call to layer-(n-1) entity's fe_down_table_mod() method
	ofswitch_find(sw)->entity->fe_down_table_mod(this, pack);

	sw->table_mod_sent(pack);

}


void
crofbase::fe_down_table_mod(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_TABLE_MOD, entity);

		fe_down_queue[OFPT_TABLE_MOD].push_back(pack);
		register_timer(TIMER_FE_HANDLE_TABLE_MOD, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_table_mod() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_table_mod() malformed packet received", this);
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_table_mod() flow-mod from non-attached entity received", this);
		delete pack;

	}
}


void
crofbase::recv_table_mod()
{
	if (fe_down_queue[OFPT_TABLE_MOD].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_TABLE_MOD].front();
		fe_down_queue[OFPT_TABLE_MOD].pop_front();

		ofctrl_find(pack->entity)->table_mod_rcvd(pack);


#if 0
	} catch (eFlowTableInval& e) {

		WRITELOG(CROFBASE, DBG, "crofbase(%s)::recv_table_mod() "
				"invalid table-mod packet received", dpname.c_str());

		send_error_message(
				ofctrl_find(pack->entity),
				OFPET_TABLE_MOD_FAILED,
				OFPTMFC_BAD_TABLE,
				pack->soframe(),
				pack->framelen());

		delete pack;
#endif

	} catch (...) {

		delete pack;
	}

	if (not fe_down_queue[OFPT_TABLE_MOD].empty())
	{
		register_timer(TIMER_FE_HANDLE_TABLE_MOD, 0); // reschedule ourselves
	}
}




/*
 * FLOW-REMOVED message
 */


void
crofbase::send_flow_removed_message(
	cofctl *ofctrl,
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

		for (std::map<cofbase*, cofctl*>::iterator
				it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
		{
			cofctl *ofctrl = it->second;

			if (OFPCR_ROLE_SLAVE == ofctrl->role)
			{
				WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_removed_message() ofctrl:%p is SLAVE", this, ofctrl);
				continue;
			}


			cofpacket_flow_removed *pack = new cofpacket_flow_removed(
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
			ofctrl_find(ofctrl)->ctrl->fe_up_flow_removed(this, pack);
		}

	} catch (eRofBaseNotFound& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_flow_removed_message() cofctrl instance not found", this);
	}

}


void
crofbase::fe_up_flow_removed(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		//CHECK_PACKET(pack, OFPT_FLOW_REMOVED);
		check_up_packet(pack, OFPT_FLOW_REMOVED, entity);

		pack->entity = entity;
		fe_up_queue[OFPT_FLOW_REMOVED].push_back(pack);
		register_timer(TIMER_FE_HANDLE_FLOW_REMOVED, 0);

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_flow_removed() malformed packet received", this);
		delete pack;
	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_flow_removed() packet received from non-attached entity", this);
		delete pack;
	}
}


void
crofbase::recv_flow_removed()
{
	if (fe_up_queue[OFPT_FLOW_REMOVED].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_up_queue[OFPT_FLOW_REMOVED].front();
		fe_up_queue[OFPT_FLOW_REMOVED].pop_front();

		cofdpt *sw = ofswitch_find(pack->entity);
		sw->flow_rmvd_rcvd(pack);
		handle_flow_removed(sw, pack);

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "packet received from non-attached entity");

		delete pack;

	} catch (...) {
		delete pack;

	}

	if (not fe_up_queue[OFPT_FLOW_REMOVED].empty())
	{
		register_timer(TIMER_FE_HANDLE_FLOW_REMOVED, 0); // reschedule ourselves
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
	for (it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
	{
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_port_status_message() to ctrl %s", this, it->second->c_str());

		cofpacket_port_status *pack = new cofpacket_port_status(
												ta_new_async_xid(),
												reason,
												phy_port,
												sizeof(struct ofp_port));

		// straight call to layer-(n+1) entity's fe_up_packet_in() method
		it->first->fe_up_port_status(this, pack);
	}
}


void
crofbase::fe_up_port_status(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_port_status() pack: %s", this, pack->c_str());

	try {
		//CHECK_PACKET(pack, OFPT_PORT_STATUS);
		check_up_packet(pack, OFPT_PORT_STATUS, entity);

		pack->entity = entity;
		fe_up_queue[OFPT_PORT_STATUS].push_back(pack);
		register_timer(TIMER_FE_HANDLE_PORT_STATUS, 0);

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_port_status() malformed packet received", this);
		delete pack;
	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_port_status() packet received from non-attached entity", this);
		delete pack;
	}
}


void
crofbase::recv_port_status()
{
	if (fe_up_queue[OFPT_PORT_STATUS].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_up_queue[OFPT_PORT_STATUS].front();
		fe_up_queue[OFPT_PORT_STATUS].pop_front();

		cofdpt *sw = ofswitch_find(pack->entity);
		sw->port_status_rcvd(pack);

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_port_status() "
				"packet received from non-attached entity", this);

		delete pack;

	} catch (...) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::recv_port_status() "
				"GENERIC ERROR", this);

		delete pack;

	}

	if (not fe_up_queue[OFPT_PORT_STATUS].empty())
	{
		register_timer(TIMER_FE_HANDLE_PORT_STATUS, 0); // reschedule ourselves
	}
}




/*
 * QUEUE-GET-CONFIG request/reply
 */

void
crofbase::send_queue_get_config_request(
	cofdpt *sw,
	uint32_t port)
{
	cofpacket_queue_get_config_request *pack = new cofpacket_queue_get_config_request(
									ta_add_request(OFPT_QUEUE_GET_CONFIG_REQUEST),
									port);

	// straight call to layer-(n-1) entity's fe_down_get_config_request() method
	ofswitch_find(sw)->entity->fe_down_queue_get_config_request(this, pack);

}


void
crofbase::fe_down_queue_get_config_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_QUEUE_GET_CONFIG_REQUEST, entity);

		fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].push_back(pack);	// store pack for xid
		register_timer(TIMER_FE_HANDLE_QUEUE_GET_CONFIG_REPLY, 0);

	} catch (eRofBaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_queue_get_config_request() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_queue_get_config_request() malformed packet received", this);
		delete pack;
	}
}




/*
 * QUEUE-GET-CONFIG request
 */
void
crofbase::recv_queue_get_config_request()
{
	if (fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].front();
		fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].pop_front();

		ofctrl_find(pack->entity)->queue_get_config_request_rcvd(pack);

	} catch (...) {
		delete pack;
	}

	if (not fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].empty())
	{
		register_timer(TIMER_FE_HANDLE_QUEUE_GET_CONFIG_REQUEST, 0); // reschedule ourselves
	}
}



void
crofbase::send_queue_get_config_reply()
{
	WRITELOG(CROFBASE, DBG, "crofbase::send_queue_get_config_reply()");

	if (fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].empty())
		return;

	cofpacket *request = fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].front();
	fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].pop_front();

	cofpacket_queue_get_config_reply *pack = new cofpacket_queue_get_config_reply(
										request->get_xid(),
										be32toh(request->ofh_queue_get_config_request->port));

	cofctl *ofctrl = ofctrl_find(request->entity);

	ofctrl_find(ofctrl)->queue_get_config_reply_sent(pack);

	ofctrl_find(ofctrl)->ctrl->fe_up_queue_get_config_reply(this, pack);

	delete request;

	if (not fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].empty())
	{
		register_timer(TIMER_FE_HANDLE_QUEUE_GET_CONFIG_REPLY, 0); // reschedule ourselves
	}

}


void
crofbase::fe_up_queue_get_config_reply(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		//CHECK_PACKET(pack, OFPT_QUEUE_GET_CONFIG_REPLY);
		check_up_packet(pack, OFPT_QUEUE_GET_CONFIG_REPLY, entity);

		pack->entity = entity;
		ta_validate(be32toh(pack->ofh_header->xid), OFPT_QUEUE_GET_CONFIG_REQUEST);
		handle_queue_get_config_reply(ofswitch_find(pack->entity), pack);

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_queue_get_config_reply() malformed packet received", this);
		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_queue_get_config_reply() invalid session exchange xid "
					"(%u) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_queue_get_config_reply() packet received from non-attached entity", this);
		delete pack;
	}
}




/*
 *  QUEUE-GET-CONFIG reply
 */

void
crofbase::recv_queue_get_config_reply()
{
	if (fe_up_queue[OFPT_QUEUE_GET_CONFIG_REPLY].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_up_queue[OFPT_QUEUE_GET_CONFIG_REPLY].front();
		fe_up_queue[OFPT_QUEUE_GET_CONFIG_REPLY].pop_front();

		ofswitch_find(pack->entity)->role_reply_rcvd(pack);

	} catch (...) {
		delete pack;
	}

	if (not fe_up_queue[OFPT_QUEUE_GET_CONFIG_REPLY].empty())
	{
		register_timer(TIMER_FE_HANDLE_QUEUE_GET_CONFIG_REPLY, 0); // reschedule ourselves
	}
}



/*
 * VENDOR messages
 */

void
crofbase::fe_down_experimenter_message(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_EXPERIMENTER, entity);

		fe_down_queue[OFPT_EXPERIMENTER].push_back(pack);
		register_timer(TIMER_FE_HANDLE_EXPERIMENTER, 0);

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_vendor_message() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_down_vendor_message() malformed packet received", this);
		delete pack;
	}
}


void
crofbase::fe_up_experimenter_message(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_up_packet(pack, OFPT_EXPERIMENTER, entity);

		pack->entity = entity;
		fe_up_queue[OFPT_EXPERIMENTER].push_back(pack);
		register_timer(TIMER_FE_HANDLE_EXPERIMENTER, 0);

	} catch (eOFbaseInval& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_experimenter_message() malformed packet received", this);
		delete pack;
	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CROFBASE, DBG, "crofbase(%p)::fe_up_experimenter_message() packet received from non-attached entity", this);
		delete pack;
	}
}


void
crofbase::recv_experimenter_message()
{
	cofpacket *pack = NULL;
	try {

		if (not fe_down_queue[OFPT_EXPERIMENTER].empty())
		{
			pack = fe_down_queue[OFPT_EXPERIMENTER].front();
			fe_down_queue[OFPT_EXPERIMENTER].pop_front();

			ofctrl_find(pack->entity)->experimenter_message_rcvd(pack);
		}
		else if (not fe_up_queue[OFPT_EXPERIMENTER].empty())
		{
			pack = fe_up_queue[OFPT_EXPERIMENTER].front();
			fe_up_queue[OFPT_EXPERIMENTER].pop_front();

			ofswitch_find(pack->entity)->experimenter_message_rcvd(pack);
		}

	} catch (...) {
		delete pack;
	}

	if (not fe_down_queue[OFPT_EXPERIMENTER].empty()
			or
		not fe_up_queue[OFPT_EXPERIMENTER].empty())
	{
		register_timer(TIMER_FE_HANDLE_EXPERIMENTER, 0); // reschedule ourselves
	}
}



void
crofbase::send_experimenter_message(
		cofdpt *sw,
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen)
{
	cofpacket_experimenter *pack = new cofpacket_experimenter(
										ta_new_async_xid(),
										experimenter_id,
										exp_type,
										body,
										bodylen);

	pack->pack();

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_experimenter_message() -down- %s", this, pack->c_str());

	if (NULL == sw) // send to all attached data path entities
	{
		std::map<cofbase*, cofdpt*>::iterator it;
		for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
		{
			it->second->entity->fe_down_experimenter_message(this, new cofpacket(*pack));
		}
		delete pack;
	}
	else
	{
		// straight call to layer-(n-1) entity's fe_down_experimenter_message() method
		ofswitch_find(sw)->entity->fe_down_experimenter_message(this, pack);
	}

}


void
crofbase::send_experimenter_message(
		cofctl *ctrl,
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t* body,
		size_t bodylen)
{
	cofpacket_experimenter *pack = new cofpacket_experimenter(
										ta_new_async_xid(),
										experimenter_id,
										exp_type,
										body,
										bodylen);

	pack->pack();

	WRITELOG(CROFBASE, DBG, "crofbase(%p)::send_experimenter_message() -up- %s", this, pack->c_str());

	if (NULL == ctrl) // send to all attached controller entities
	{
		std::map<cofbase*, cofctl*>::iterator it;
		for (it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
		{
			it->second->ctrl->fe_up_experimenter_message(this, new cofpacket(*pack));
			delete pack;
		}
	}
	else
	{
		// straight call to layer-(n+1) entity's fe_up_experimenter_message() method
		ctrl->ctrl->fe_up_experimenter_message(this, pack);
	}
}







