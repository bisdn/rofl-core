/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cfwdelem.h"

/* static */ std::set<cfwdelem*> cfwdelem::fwdelems;

/* static */
cfwdelem*
cfwdelem::find_by_name(const std::string &dpname) throw (eFwdElemNotFound)
{
	std::set<cfwdelem*>::iterator it;
	if ((it = find_if(cfwdelem::fwdelems.begin(), cfwdelem::fwdelems.end(),
			cfwdelem::cfwdelem_find_by_name(dpname))) == cfwdelem::fwdelems.end())
	{
		throw eFwdElemNotFound();
	}
	return (*it);
}


/* static */
cfwdelem*
cfwdelem::find_by_dpid(uint64_t dpid) throw (eFwdElemNotFound)
{
	std::set<cfwdelem*>::iterator it;
	if ((it = find_if(cfwdelem::fwdelems.begin(), cfwdelem::fwdelems.end(),
			cfwdelem::cfwdelem_find_by_dpid(dpid))) == cfwdelem::fwdelems.end())
	{
		throw eFwdElemNotFound();
	}
	return (*it);
}


cfwdelem::cfwdelem(
		std::string dpname,
		uint64_t dpid,
		uint8_t n_tables,
		uint32_t n_buffers,
		caddress const& rpc_ctl_addr,
		caddress const& rpc_dpt_addr) throw (eFwdElemExists) :
	ctrl(NULL),
	dpname(dpname),
	dpid(dpid),
	flags(0),
	miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
	n_buffers(n_buffers),
	n_tables(n_tables),
	capabilities(OFPC_FLOW_STATS | OFPC_TABLE_STATS | OFPC_PORT_STATS /* | OFPC_STP */ | OFPC_QUEUE_STATS | OFPC_ARP_MATCH_IP),
	group_table()
{
	cvastring vas;
	s_dpid.assign(vas("dpid[%016llx]", dpid));

	// create all flow-tables ...
	for (int i = 0; i < n_tables; ++i)
	{
		flow_tables[i] = new cfttable(this, i);
	}

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::cfwdelem() dpid:%llu dpname=%s", this, dpid, dpname.c_str());
	std::set<cfwdelem*>::iterator it;

	for (it = fwdelems.begin(); it != fwdelems.end(); ++it)
	{
		if (((*it)->dpid == dpid) || ((*it)->dpname == dpname))
		{
			throw eFwdElemExists();
		}
	}

	// create rpc TCP endpoints
	rpc[RPC_CTL] = new cofrpc(cofrpc::OF_RPC_TCP_SOUTH_ENDPNT, this);
	rpc[RPC_CTL]->clisten(rpc_ctl_addr);

	rpc[RPC_DPT] = new cofrpc(cofrpc::OF_RPC_TCP_NORTH_ENDPNT, this);
	rpc[RPC_DPT]->clisten(rpc_dpt_addr);

	cfwdelem::fwdelems.insert(this);
}


cfwdelem::~cfwdelem()
{
	cfwdelem::fwdelems.erase(this);
	WRITELOG(CFWD, DBG, "destroy cfwdelem(%p)::cfwdelem() dpid:%llu", this, dpid);


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


	while (not fib_table.empty())
	{
		delete *(fib_table.begin());
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

	// remove all flow-tables
	std::map<uint8_t, cfttable*>::iterator ft;
	for (ft = flow_tables.begin(); ft != flow_tables.end(); ++ft)
	{
		delete (*ft).second;
	}
	flow_tables.clear();

	// remove rpc TCP endpoints
	delete rpc[RPC_CTL];
	delete rpc[RPC_DPT];
}



void
cfwdelem::tables_reset()
{
	group_table.reset();

	std::map<uint8_t, cfttable*>::iterator it;
	for (it = flow_tables.begin(); it != flow_tables.end(); ++it)
	{
		delete (it->second);
	}
	flow_tables.clear();
}


const char*
cfwdelem::c_str()
{
	cvastring vas(1024);

	info.assign(vas("cfwdelem(%p): dpname[%s] dpid[%llx] =>", this, dpname.c_str(), dpid));

	// cofport instances
	info.append(vas("\nlist of registered cofport instances: =>"));
	std::map<uint32_t, cofport*>::iterator it;
	for (it = phy_ports.begin(); it != phy_ports.end(); ++it)
	{
		info.append(vas("\n  %s", it->second->c_str()));
	}

	// cofctrl instances
	info.append(vas("\nlist of registered cofctrl instances: =>"));
	std::map<cofbase*, cofctrl*>::iterator it_ctl;
	for (it_ctl = ofctrl_list.begin(); it_ctl != ofctrl_list.end(); ++it_ctl)
	{
		info.append(vas("\n  %s", it_ctl->second->c_str()));
	}

	// cofswitch instances
	info.append(vas("\nlist of registered cofswitch instances: =>"));
	std::map<cofbase*, cofdpath*>::iterator it_dp;
	for (it_dp = ofdpath_list.begin(); it_dp != ofdpath_list.end(); ++it_dp)
	{
		info.append(vas("\n  %s", it_dp->second->c_str()));
	}

	return info.c_str();
}


void
cfwdelem::ofctrl_exists(const cofctrl *ofctrl) throw (eFwdElemNotFound)
{
	std::map<cofbase*, cofctrl*>::iterator it;
	for (it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
	{
		if (it->second == ofctrl)
			return;
	}
	throw eFwdElemNotFound();
}


void
cfwdelem::ofswitch_exists(const cofdpath *ofswitch) throw (eFwdElemNotFound)
{
	std::map<cofbase*, cofdpath*>::iterator it;
	for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		if (it->second == ofswitch)
			return;
	}
	throw eFwdElemNotFound();
}


void
cfwdelem::nsp_enable(bool enable)
{
	if (enable)
	{
		fe_flags.set(NSP_ENABLED);
		WRITELOG(CFWD, INFO, "cfwdelem(%p)::nsp_enable() enabling -NAMESPACE- support", this);
	}
	else
	{
		fe_flags.reset(NSP_ENABLED);
		WRITELOG(CFWD, INFO, "cfwdelem(%p)::nsp_enable() disabling -NAMESPACE- support", this);
	}
}


cfttable&
cfwdelem::get_fttable(uint8_t tableid) throw (eFwdElemNotFound)
{
	if (flow_tables.find(tableid) == flow_tables.end())
	{
		throw eFwdElemNotFound();
	}
	return (*flow_tables[tableid]);
}


cfttable&
cfwdelem::get_succ_fttable(uint8_t tableid) throw (eFwdElemNotFound)
{
	std::map<uint8_t, cfttable*>::iterator it = flow_tables.find(tableid);

	if (it == flow_tables.end())
	{
		throw eFwdElemNotFound();
	}

	std::advance(it, 1);

	if (it == flow_tables.end())
	{
		throw eFwdElemNotFound();
	}

	return *(it->second);
}


void
cfwdelem::ctl_connect(caddress const& ra)
{
	rpc[RPC_CTL]->cconnect(ra);
}


void
cfwdelem::dpt_connect(caddress const& ra)
{
	rpc[RPC_DPT]->cconnect(ra);
}


void
cfwdelem::dpath_attach(cofbase* dp)
{
	if (NULL == dp) return;

	cofdpath *sw = NULL;

	try {

		sw = ofswitch_find(dp);

	} catch (eOFbaseNotAttached& e) {
		sw = new cofdpath(this, dp, &ofdpath_list);
		WRITELOG(CFWD, INFO, "cfwdelem(%p)::dpath_attach() cofbase: %p cofswitch: %s", this, dp, sw->c_str());
	}

	send_down_hello_message(sw);
}


void
cfwdelem::dpath_detach(cofbase* dp)
{
	if (NULL == dp) return;

	cofdpath *sw = NULL;
	try {
		sw = ofswitch_find(dp);

		// sends a HELLO with BYE flag to controller and deletes our ofctrl instance
		send_down_hello_message(sw, true /*BYE*/);

		handle_dpath_close(sw);

		WRITELOG(CFWD, INFO, "cfwdelem(%p)::dpath_detach() cofbase: %p cofswitch: %s", this, dp, sw->c_str());

		delete sw;


	} catch (eOFbaseNotAttached& e) { }
}


void
cfwdelem::ctrl_attach(cofbase* dp) throw (eOFbaseIsBusy)
{
	// sanity check: entity must exist
	if (NULL == dp) return;

	cofctrl *ofctrl = NULL;

	// namespaces disabled? block attachment attempts
	if ((not fe_flags.test(NSP_ENABLED)) && (not ofctrl_list.empty()))
	{
		throw eOFbaseIsBusy();
	}

	// check for existence of control entity
	if (ofctrl_list.find(dp) == ofctrl_list.end())
	{
		ofctrl = new cofctrl(this, dp, &ofctrl_list);
		WRITELOG(CFWD, INFO, "cfwdelem(%p)::ctrl_attach() cofbase: %p cofctrl: %s",
				this, dp, ofctrl->c_str());
	}

	send_up_hello_message(ofctrl);
}


void
cfwdelem::ctrl_detach(cofbase* dp)
{
	if (NULL == dp) return;

	std::map<cofbase*, cofctrl*>::iterator it;
	if ((it = ofctrl_list.find(dp)) != ofctrl_list.end())
	{
		WRITELOG(CFWD, INFO, "cfwdelem(%p)::ctrl_detach() cofbase: %p cofctrl: %s",
				this, dp, it->second->c_str());

		// sends a HELLO with BYE flag to controller and deletes our ofctrl instance
		send_up_hello_message(it->second, true /*BYE*/);

		delete it->second;
	}
}



void
cfwdelem::handle_timeout(int opaque)
{
	try {
		switch (opaque) {
		case TIMER_FE_HANDLE_HELLO:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
								"TIMER_FE_HANDLE_HELLO (%d) expired", this,  TIMER_FE_HANDLE_HELLO);
			recv_hello_message();
			break;
		case TIMER_FE_HANDLE_FEATURES_REQUEST:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_FEATURES_REQUEST (%d) expired", this,  TIMER_FE_HANDLE_FEATURES_REQUEST);
			recv_features_request();
			break;
		case TIMER_FE_SEND_GET_CONFIG_REPLY:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_SEND_GET_CONFIG_REPLY (%d) expired", this,  TIMER_FE_SEND_GET_CONFIG_REPLY);
			send_get_config_reply();
			break;
		case TIMER_FE_HANDLE_STATS_REQUEST:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_STATS_REQUEST (%d) expired", this,  TIMER_FE_HANDLE_STATS_REQUEST);
			recv_stats_request();
			break;
		case TIMER_FE_HANDLE_PACKET_OUT:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_PACKET_OUT (%d) expired", this,  TIMER_FE_HANDLE_PACKET_OUT);
			recv_packet_out();
			break;
		case TIMER_FE_HANDLE_PACKET_IN:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_PACKET_IN (%d) expired", this,  TIMER_FE_HANDLE_PACKET_IN);
			recv_packet_in();
			break;
		case TIMER_FE_HANDLE_ERROR:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_ERROR (%d) expired", this,  TIMER_FE_HANDLE_ERROR);
			recv_error();
			break;
		case TIMER_FE_HANDLE_FLOW_MOD:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_FLOW_MOD (%d) expired", this,  TIMER_FE_HANDLE_FLOW_MOD);
			recv_flow_mod();
			break;
		case TIMER_FE_HANDLE_GROUP_MOD:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_GROUP_MOD (%d) expired", this,  TIMER_FE_HANDLE_GROUP_MOD);
			recv_group_mod();
			break;
		case TIMER_FE_HANDLE_TABLE_MOD:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_TABLE_MOD (%d) expired", this,  TIMER_FE_HANDLE_TABLE_MOD);
			recv_table_mod();
			break;
		case TIMER_FE_HANDLE_PORT_MOD:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_PORT_MOD (%d) expired", this,  TIMER_FE_HANDLE_PORT_MOD);
			recv_port_mod();
			break;
		case TIMER_FE_HANDLE_FLOW_REMOVED:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_FLOW_REMOVED (%d) expired", this,  TIMER_FE_HANDLE_FLOW_REMOVED);
			recv_flow_removed();
			break;
		case TIMER_FE_HANDLE_SET_CONFIG:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_SET_CONFIG (%d) expired", this,  TIMER_FE_HANDLE_SET_CONFIG);
			recv_set_config();
			break;
		case TIMER_FE_HANDLE_EXPERIMENTER:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_VENDOR (%d) expired", this,  TIMER_FE_HANDLE_EXPERIMENTER);
			recv_experimenter_message();
			break;
		case TIMER_FE_SEND_QUEUE_GET_CONFIG_REPLY:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_SEND_QUEUE_GET_CONFIG_REPLY (%d) expired", this,  TIMER_FE_SEND_QUEUE_GET_CONFIG_REPLY);
			send_queue_get_config_reply();
			break;
		case TIMER_FE_HANDLE_BARRIER_REQUEST:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_BARRIER_REQUEST (%d) expired", this,  TIMER_FE_HANDLE_BARRIER_REQUEST);
			recv_barrier_request();
			break;
		case TIMER_FE_HANDLE_PORT_STATUS:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_PORT_STATUS (%d) expired", this,  TIMER_FE_HANDLE_PORT_STATUS);
			recv_port_status();
			break;
		case TIMER_FE_HANDLE_ROLE_REQUEST:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_ROLE_REQUEST (%d) expired", this,  TIMER_FE_HANDLE_ROLE_REQUEST);
			recv_role_request();
			break;
		case TIMER_FE_HANDLE_ROLE_REPLY:
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_timeout() "
					"TIMER_FE_HANDLE_ROLE_REPLY (%d) expired", this,  TIMER_FE_HANDLE_ROLE_REPLY);
			recv_role_reply();
			break;
		default:
			//WRITELOG(CFWD, DBG, "cfwdelem::handle_timeout() "
			//		"received unknown timer event %d", opaque);
			break;
		}



	} catch (eOFbaseNoCtrl& e) {
		WRITELOG(CFWD, DBG, "controlling entity lost");
		// handle NoCtrl condition: simply do nothing for now,
		// TODO: reconnect to new controlling entity

	} catch (eIoSvcUnhandledTimer& e) {
		// ignore
	}
}



void
cfwdelem::handle_experimenter_message(cofctrl *ofctrl, cofpacket *pack)
{
	// base class does not support any vendor extensions, so: send error indication
	size_t datalen = (pack->framelen() > 64) ? 64 : pack->framelen();
	send_error_message(ofctrl, OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER,
									(unsigned char*)pack->soframe(), datalen);

	delete pack;
}


cofdpath&
cfwdelem::dpath_find(uint64_t dpid) throw (eOFbaseNotAttached)
{
	std::map<cofbase*, cofdpath*>::iterator it;
	for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		if (it->second->dpid == dpid)
			return *(it->second);
	}
	throw eOFbaseNotAttached();
}


cofdpath&
cfwdelem::dpath_find(std::string s_dpid) throw (eOFbaseNotAttached)
{
	std::map<cofbase*, cofdpath*>::iterator it;
	for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		if (it->second->s_dpid == s_dpid)
			return *(it->second);
	}
	throw eOFbaseNotAttached();
}


cofdpath&
cfwdelem::dpath_find(cmacaddr dl_dpid) throw (eOFbaseNotAttached)
{
	std::map<cofbase*, cofdpath*>::iterator it;
	for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		if (it->second->dpmac == dl_dpid)
			return *(it->second);
	}
	throw eOFbaseNotAttached();
}


cofdpath*
cfwdelem::ofswitch_find(cofbase *entity) throw (eOFbaseNotAttached)
{
	if (ofdpath_list.find(entity) == ofdpath_list.end())
		throw eOFbaseNotAttached();
	return ofdpath_list[entity];
}


cofctrl*
cfwdelem::ofctrl_find(cofbase *entity) throw (eOFbaseNotAttached)
{
	if (ofctrl_list.find(entity) == ofctrl_list.end())
		throw eOFbaseNotAttached();
	return ofctrl_list[entity];
}


cftentry*
cfwdelem::hw_create_cftentry(
	cftentry_owner *owner,
	std::set<cftentry*> *flow_table,
	cofpacket *pack)
{
	return new cftentry(owner, flow_table, pack, this);
}


inline
void
cfwdelem::check_up_packet(
		cofpacket *ofpacket,
		enum ofp_type oftype,
		cofbase *ofbase) throw (eFwdElemInval)
{
	try {
		ofswitch_find(ofbase);
	} catch (eOFbaseNotAttached& e) {
		dpath_attach(ofbase);
	}

	ofpacket->entity = ofswitch_find(ofbase)->entity;

	if ((not ofpacket->is_valid()) || (ofpacket->ofh_header->type != oftype))
	{
		throw eFwdElemInval();
	}
}


inline
void
cfwdelem::check_down_packet(
		cofpacket *ofpacket,
		enum ofp_type oftype,
		cofbase *ofbase) throw (eFwdElemInval)
{
	try {
		ofctrl_find(ofbase);
	} catch (eOFbaseNotAttached& e) {
		ctrl_attach(ofbase);
	}

	ofpacket->entity = ofctrl_find(ofbase)->ctrl;

	if ((not ofpacket->is_valid()) || (ofpacket->ofh_header->type != oftype))
	{
		throw eFwdElemInval();
	}
}


void
cfwdelem::ftentry_timeout(
		cftentry *fte,
		uint16_t timeout)
{
	if (OFPFF_SEND_FLOW_REM & be16toh(fte->flow_mod->flags))
	{
		cclock since;
		since -= fte->flow_create_time;

		send_flow_removed_message(
			fte->ofctrl,
			fte->ofmatch,
			be64toh(fte->flow_mod->cookie),
			fte->flow_mod->priority,
			fte->removal_reason,
			fte->flow_mod->table_id,
			since.ts.tv_sec,
			since.ts.tv_nsec,
			be16toh(fte->flow_mod->idle_timeout),
			be16toh(fte->flow_mod->hard_timeout),
			fte->rx_packets,
			fte->rx_bytes);
	}
}


void
cfwdelem::gtentry_timeout(
		cgtentry *gte,
		uint16_t timeout)
{


}


 void
cfwdelem::fibentry_timeout(cfibentry *fibentry)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%p)::fibentry_timeout() %s",
			this, fibentry->c_str());

#if 0
	rib.rem_edge(fibentry->from, fibentry->to);

	rib.dijkstra();

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::fibentry_timeout() crib: \n%s",
			this, rib.c_str());
#endif
}


/*
* HELLO messages
*/

void
cfwdelem::send_up_hello_message(
	cofctrl *ofctrl, bool bye)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_up_hello_message()", this);

	uint32_t hello = (bye) ? htobe32(FE_HELLO_BYE) : htobe32(FE_HELLO_ACTIVE);

	cofpacket_hello *pack = new cofpacket_hello(
										ta_new_async_xid(),
										(uint8_t*)&hello, sizeof(hello));

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_up_hello_message() new %s", this,
			pack->c_str());

	ofctrl->ctrl->fe_up_hello_message(this, pack);
}


void
cfwdelem::send_down_hello_message(
	cofdpath *ofdpath, bool bye)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_down_hello_message()", this);

	uint32_t hello = (bye) ? htobe32(FE_HELLO_BYE) : htobe32(FE_HELLO_ACTIVE);

	cofpacket_hello *pack = new cofpacket_hello(
										ta_new_async_xid(),
										(uint8_t*)&hello, sizeof(hello));

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_down_hello_message() new %s", this,
			pack->c_str());

	ofdpath->entity->fe_down_hello_message(this, pack);
}


void
cfwdelem::fe_down_hello_message(
		cofbase *entity,
		cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_hello_message() HELLO received: %s", this, pack->c_str());

	try {
		check_down_packet(pack, OFPT_HELLO, entity);

		fe_down_queue[OFPT_HELLO].push_back(pack); // store pack for xid
		register_timer(TIMER_FE_HANDLE_HELLO, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_hello_message() packet from non-controlling entity dropped", this);

		////entity->ctrl_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_hello_message() malformed packet received", this);
		delete pack;
	}
}


void
cfwdelem::fe_up_hello_message(
		cofbase *entity,
		cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_hello_message() HELLO received: %s", this, pack->c_str());

	try {
		check_up_packet(pack, OFPT_HELLO, entity);

		fe_up_queue[OFPT_HELLO].push_back(pack); // store pack for xid
		register_timer(TIMER_FE_HANDLE_HELLO, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_hello_message() packet from non-controlling entity dropped", this);

		////entity->dpath_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_hello_message() malformed packet received", this);
		delete pack;
	}
}


void
cfwdelem::recv_hello_message()
{
	cofpacket *pack = NULL;
	try {

		if (not fe_down_queue[OFPT_HELLO].empty())
		{
			pack = fe_down_queue[OFPT_HELLO].front();
			fe_down_queue[OFPT_HELLO].pop_front();

			uint32_t cookie;

			if (pack->body.memlen() >= sizeof(uint32_t))
			{
				memcpy(&cookie, pack->body.somem(), sizeof(uint32_t));
				cookie = be32toh(cookie);
			}

			WRITELOG(CFWD, DBG, "cfwdelem(%p)::recv_hello_message() down pack:%s cookie:%s",
					this, pack->c_str(), cookie == FE_HELLO_ACTIVE ? "FE_HELLO_ACTIVE" : "FE_HELLO_BYE");

			switch (cookie) {
			case FE_HELLO_BYE:
				// deregister controller
				delete ofctrl_find(pack->entity);
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

			uint32_t cookie;

			if (pack->body.memlen() >= sizeof(uint32_t))
			{
				memcpy(&cookie, pack->body.somem(), sizeof(uint32_t));
				cookie = be32toh(cookie);
			}

			WRITELOG(CFWD, DBG, "cfwdelem(%p)::recv_hello_message() up pack:%s cookie:%s",
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
cfwdelem::send_features_request(cofdpath *dpath)
{

	cofpacket_features_request *pack = new cofpacket_features_request(ta_add_request(OFPT_FEATURES_REQUEST));

	// straight call to layer-(n-1) entity's fe_down_features_request() method
	dpath->entity->fe_down_features_request(this, pack);

	dpath->features_request_sent();
}


void
cfwdelem::fe_down_features_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_features_request() FEATURES-REQUEST received: %s", this, pack->c_str());

	try {
		check_down_packet(pack, OFPT_FEATURES_REQUEST, entity);

		fe_down_queue[OFPT_FEATURES_REQUEST].push_back(pack); // store pack for xid
		register_timer(TIMER_FE_HANDLE_FEATURES_REQUEST, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_features_request() packet from non-controlling entity dropped", this);

		////entity->dpath_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_features_request() malformed packet received", this);
		delete pack;
	}
}


void
cfwdelem::recv_features_request()
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
cfwdelem::handle_features_request(cofctrl *ofctrl, cofpacket *request)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%s)::handle_features_request()", dpname.c_str());

	send_features_reply(ofctrl, request->get_xid());
}


void
cfwdelem::send_features_reply(cofctrl *ofctrl, uint32_t xid)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%s)::send_features_reply()", dpname.c_str());

	cofpacket_features_reply *reply = new cofpacket_features_reply(
													xid,
													dpid,
													n_buffers,
													n_tables,
													capabilities);

	for (std::map<uint32_t, cofport*>::iterator
			it = phy_ports.begin(); it != phy_ports.end(); ++it)
	{
		reply->ports.next() = *(it->second);
	}

	reply->pack(); // adjust fields, e.g. length in ofp_header

	ofctrl->ctrl->fe_up_features_reply(this, reply);
}


void
cfwdelem::fe_up_features_reply(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_features_reply() FEATURES-REPLY received: %s", this, pack->c_str());

	try {
		//CHECK_PACKET(pack, OFPT_FEATURES_REPLY);
		check_up_packet(pack, OFPT_FEATURES_REPLY, entity);

		pack->entity = entity;
		ta_validate(be32toh(pack->ofh_header->xid), OFPT_FEATURES_REQUEST);

		cofdpath *sw = ofswitch_find(entity);
		sw->features_reply_rcvd(pack);
		handle_features_reply(sw, pack);

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_features_reply() malformed packet received", this);

		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_features_reply() invalid session exchange xid "
					"(%u) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached &e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_features_reply() packet received from non-attached entity", this);

		delete pack;
	}
}



/*
 * GET-CONFIG request/reply
 */

void
cfwdelem::send_get_config_request(
		cofdpath *sw)
{
	cofpacket_get_config_request *pack = new cofpacket_get_config_request(
								ta_add_request(OFPT_GET_CONFIG_REQUEST));

	// straight call to layer-(n-1) entity's fe_down_get_config_request() method
	sw->entity->fe_down_get_config_request(this, pack);

	sw->get_config_request_sent();

}


void
cfwdelem::fe_down_get_config_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_GET_CONFIG_REQUEST, entity);

		fe_down_queue[OFPT_GET_CONFIG_REQUEST].push_back(pack);	// store pack for xid
		register_timer(TIMER_FE_SEND_GET_CONFIG_REPLY, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_get_config_request() packet from non-controlling entity dropped", this);

		////entity->dpath_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_get_config_request() malformed packet received", this);
		delete pack;
	}
}


void
cfwdelem::send_get_config_reply()
{
	WRITELOG(CFWD, DBG, "cfwdelem::send_get_config_reply()");

	if (fe_down_queue[OFPT_GET_CONFIG_REQUEST].empty())
		return;

	cofpacket *request = fe_down_queue[OFPT_GET_CONFIG_REQUEST].front();
	fe_down_queue[OFPT_GET_CONFIG_REQUEST].pop_front();

	cofpacket_get_config_reply *pack = new cofpacket_get_config_reply(request->get_xid(), flags, miss_send_len);

	request->entity->fe_up_get_config_reply(this, pack);

	delete request;

	if (not fe_down_queue[OFPT_GET_CONFIG_REQUEST].empty())
	{
		register_timer(TIMER_FE_SEND_GET_CONFIG_REPLY, 0); // reschedule ourselves
	}

}


void
cfwdelem::fe_up_get_config_reply(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);


	WRITELOG(CFWD, DBG, "GET-CONFIG-REPLY received: %s", pack->c_str());

	try {
		//CHECK_PACKET(pack, OFPT_GET_CONFIG_REPLY);
		check_up_packet(pack, OFPT_GET_CONFIG_REPLY, entity);

		ta_validate(be32toh(pack->ofh_header->xid), OFPT_GET_CONFIG_REQUEST);
		cofdpath *sw = ofswitch_find(entity);
		sw->get_config_reply_rcvd(pack);
		handle_get_config_reply(sw, pack);

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_get_config_reply() malformed packet received", this);

		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_get_config_reply() invalid session exchange xid "
					"(%u) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_get_config_reply() packet received from non-attached entity", this);

		delete pack;
	}
}



/*
 * STATS request/reply
 */

void
cfwdelem::send_stats_request(
	cofdpath *sw,
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

	// straight call to layer-(n-1) entity's fe_down_get_config_request() method
	sw->entity->fe_down_stats_request(this, pack);

	sw->stats_request_sent();
}


void
cfwdelem::fe_down_stats_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

#ifndef NDEBUG
	// WRITELOG(CFWD, DBG, "dpid:%u STATS-REQUEST received: %s", dpid, pack->c_str());
#endif
	try {
		check_down_packet(pack, OFPT_STATS_REQUEST, entity);

		fe_down_queue[OFPT_STATS_REQUEST].push_back(pack);
		register_timer(TIMER_FE_HANDLE_STATS_REQUEST, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_stats_request() packet from non-controlling entity dropped", this);

		////entity->dpath_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_stats_request() malformed packet received", this);
		delete pack;
	}
}


void
cfwdelem::recv_stats_request()
{
	WRITELOG(CFWD, DBG, "cfwdelem::recv_stats_request()");

	if (fe_down_queue[OFPT_STATS_REQUEST].empty())
		return;

	cofpacket *request = fe_down_queue[OFPT_STATS_REQUEST].front();
	fe_down_queue[OFPT_STATS_REQUEST].pop_front();

	switch (be16toh(request->ofh_stats_request->type)) {
	case OFPST_DESC:
		{
			handle_desc_stats_request(ofctrl_find(request->entity), request);
		}
		break;
	case OFPST_TABLE:
		{
			handle_table_stats_request(ofctrl_find(request->entity), request);
		}
		break;
	case OFPST_PORT:
		{
			handle_port_stats_request(ofctrl_find(request->entity), request);
		}
		break;
	case OFPST_FLOW:
		{
			handle_flow_stats_request(ofctrl_find(request->entity), request);
		}
		break;
	case OFPST_AGGREGATE:
		{
			handle_aggregate_stats_request(ofctrl_find(request->entity), request);
		}
		break;
	case OFPST_QUEUE:
		{
			handle_queue_stats_request(ofctrl_find(request->entity), request);
		}
		break;
	case OFPST_GROUP:
		{
			handle_group_stats_request(ofctrl_find(request->entity), request);
		}
		break;
	case OFPST_GROUP_DESC:
		{
			handle_group_desc_stats_request(ofctrl_find(request->entity), request);
		}
		break;
	case OFPST_GROUP_FEATURES:
		{
			handle_group_features_stats_request(ofctrl_find(request->entity), request);
		}
		break;
	case OFPST_EXPERIMENTER:
		{
			handle_experimenter_stats_request(ofctrl_find(request->entity), request);
		}
		break;
	default:
		WRITELOG(CFWD, DBG, "unknown stats request type (%d)",
			be16toh(request->ofh_stats_request->type));

		handle_stats_request(ofctrl_find(request->entity), request);

		break;
	}


	// further stats request available? re-schedule ourselves
	if (not fe_down_queue[OFPT_STATS_REQUEST].empty())
	{
		register_timer(TIMER_FE_HANDLE_STATS_REQUEST, 0);
	}
}


void
cfwdelem::handle_stats_request(cofctrl *ofctrl, cofpacket *pack)
{
	/*
	 * default handler for all unknown (or unimplemented :) stats requests
	 */
	send_error_message(ofctrl, OFPET_BAD_REQUEST, OFPBRC_BAD_STAT,
					pack->soframe(), pack->framelen());

	delete pack;
}


void
cfwdelem::handle_desc_stats_request(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	struct ofp_desc_stats desc_stats;
	memset(&desc_stats, 0, sizeof(desc_stats));
	snprintf(desc_stats.mfr_desc, 		DESC_STR_LEN, 	"rofl");
	snprintf(desc_stats.hw_desc, 		DESC_STR_LEN, 	"v0.1.0");
	snprintf(desc_stats.sw_desc, 		DESC_STR_LEN, 	"v0.1.0");
	snprintf(desc_stats.serial_num, 	SERIAL_NUM_LEN, "0");
	snprintf(desc_stats.dp_desc, 		DESC_STR_LEN, 	"revised OpenFlow library");

	send_stats_reply(
					ofctrl,
					pack->get_xid(),
					OFPST_DESC,
					(uint8_t*)&desc_stats, sizeof(desc_stats));

	delete pack;
}


void
cfwdelem::handle_table_stats_request(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	cmemory body(flow_tables.size() * sizeof(struct ofp_table_stats)); // array of struct ofp_table_stats

	struct ofp_table_stats *table_stats = // auxiliary pointer for iterating through array of table_stats
			(struct ofp_table_stats*)body.somem();


	std::map<uint8_t, cfttable*>::iterator it;
	for (it = flow_tables.begin(); it != flow_tables.end(); ++it)
	{
		it->second->get_table_stats(table_stats, sizeof(struct ofp_table_stats));

		table_stats++; // jump to start of array + 1
	}

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_stats_table_request() table_stats[%p] body: %s",
			this, table_stats, body.c_str());

	send_stats_reply(
			ofctrl,
			pack->get_xid(),
			OFPST_TABLE,
			(uint8_t*)body.somem(), body.memlen());

	delete pack;
}


void
cfwdelem::handle_port_stats_request(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	uint32_t port_no = be32toh(pack->ofb_port_stats_request->port_no);

	cmemory body(0);

	try {
		if (OFPP_ANY == port_no)
		{
			for (std::map<uint32_t, cofport*>::iterator
					it = phy_ports.begin(); it != phy_ports.end(); ++it)
			{
				cofport *port = it->second;
				port->get_port_stats(body);
			}
		}
		else
		{
			if (phy_ports.find(port_no) == phy_ports.end())
			{
				throw eFwdElemOFportNotFound();
			}

			phy_ports[port_no]->get_port_stats(body);
		}

		send_stats_reply(
					ofctrl,
					pack->get_xid(),
					OFPST_PORT,
					body.somem(), body.memlen());



	} catch (eFwdElemOFportNotFound& e) {

		send_error_message(ofctrl, OFPET_BAD_REQUEST, OFPBRC_BAD_PORT,
				pack->soframe(), pack->framelen());

	}

	delete pack;
}


void
cfwdelem::handle_flow_stats_request(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	uint8_t table_id = pack->ofb_flow_stats_request->table_id;

	cmemory body(0);

	try {

		if (OFPTT_ALL == table_id)
		{
			for (std::map<uint8_t, cfttable*>::iterator
					it = flow_tables.begin(); it != flow_tables.end(); ++it)
			{
				cfttable* fttable = it->second;
				fttable->get_flow_stats(
						body,
						be32toh(pack->ofb_flow_stats_request->out_port),
						be32toh(pack->ofb_flow_stats_request->out_group),
						be64toh(pack->ofb_flow_stats_request->cookie),
						be64toh(pack->ofb_flow_stats_request->cookie_mask),
						pack->match);
			}
		}
		else
		{
			if (flow_tables.find(table_id) == flow_tables.end())
			{
				throw eFwdElemTableNotFound();
			}

			flow_tables[table_id]->get_flow_stats(
							body,
							be32toh(pack->ofb_flow_stats_request->out_port),
							be32toh(pack->ofb_flow_stats_request->out_group),
							be64toh(pack->ofb_flow_stats_request->cookie),
							be64toh(pack->ofb_flow_stats_request->cookie_mask),
							pack->match);
		}

		send_stats_reply(
						ofctrl,
						pack->get_xid(),
						OFPST_FLOW,
						body.somem(), body.memlen());



	} catch (eFwdElemTableNotFound& e) {

		send_error_message(ofctrl, OFPET_BAD_REQUEST, OFPBRC_BAD_TABLE_ID,
				pack->soframe(), pack->framelen());

	}

	delete pack;
}


void
cfwdelem::handle_aggregate_stats_request(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	uint8_t table_id = pack->ofb_aggregate_stats_request->table_id;

	uint64_t packet_count = 0;
	uint64_t byte_count = 0;

	uint64_t flow_count = 0; // FIXME: flow_count should be uint32_t

	try {

		if (OFPTT_ALL == table_id)
		{
			for (std::map<uint8_t, cfttable*>::iterator
					it = flow_tables.begin(); it != flow_tables.end(); ++it)
			{
				cfttable* fttable = it->second;

				fttable->get_aggregate_flow_stats(
						packet_count,
						byte_count,
						flow_count,
						be32toh(pack->ofb_flow_stats_request->out_port),
						be32toh(pack->ofb_flow_stats_request->out_group),
						be64toh(pack->ofb_flow_stats_request->cookie),
						be64toh(pack->ofb_flow_stats_request->cookie_mask),
						pack->match);
			}
		}
		else
		{
			if (flow_tables.find(table_id) == flow_tables.end())
			{
				throw eFwdElemTableNotFound();
			}

			flow_tables[table_id]->get_aggregate_flow_stats(
							packet_count,
							byte_count,
							flow_count,
							be32toh(pack->ofb_flow_stats_request->out_port),
							be32toh(pack->ofb_flow_stats_request->out_group),
							be64toh(pack->ofb_flow_stats_request->cookie),
							be64toh(pack->ofb_flow_stats_request->cookie_mask),
							pack->match);
		}

		struct ofp_aggregate_stats_reply aggr_stats_reply;

		aggr_stats_reply.packet_count 	= htobe64(packet_count);
		aggr_stats_reply.byte_count 	= htobe64(byte_count);
		aggr_stats_reply.flow_count		= htobe32(flow_count);

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::handle_aggregate_stats_request() "
				"packet_count:%llu byte_count:%llu flow_count:%llu",
				dpname.c_str(), packet_count, byte_count, flow_count);

		send_stats_reply(
						ofctrl,
						pack->get_xid(),
						OFPST_AGGREGATE,
						(uint8_t*)&aggr_stats_reply, sizeof(aggr_stats_reply));



	} catch (eFwdElemTableNotFound& e) {

		send_error_message(ofctrl, OFPET_BAD_REQUEST, OFPBRC_BAD_TABLE_ID,
				pack->soframe(), pack->framelen());

	}

	delete pack;
}


void
cfwdelem::handle_queue_stats_request(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	/*
	 * we do not support queues in cfwdelem (maybe in a derived class),
	 * so we send an empty reply message back (body is empty)
	 */

	send_stats_reply(
				ofctrl,
				pack->get_xid(),
				OFPST_QUEUE);

	delete pack;
}


void
cfwdelem::handle_group_stats_request(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	uint32_t group_id = be32toh(pack->ofb_group_stats_request->group_id);

	cmemory body(0);

	try {

		if (OFPG_ALL == group_id)
		{
			group_table.get_group_stats(body);
		}
		else
		{
			group_table[group_id]->get_group_stats(body);
		}

		send_stats_reply(
					ofctrl,
					pack->get_xid(),
					OFPST_GROUP,
					body.somem(), body.memlen());



	} catch (eGroupTableNotFound& e) {

		// FIXME: check for correct error type: OF1.2 spec is unprecise here
		send_error_message(ofctrl, OFPET_BAD_REQUEST, OFPBRC_BAD_PORT,
				pack->soframe(), pack->framelen());

	}

	delete pack;
}


void
cfwdelem::handle_group_desc_stats_request(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	cmemory body(0);

	group_table.get_group_desc_stats(body);

	send_stats_reply(
					ofctrl,
					pack->get_xid(),
					OFPST_GROUP_DESC,
					body.somem(), body.memlen());

	delete pack;
}


void
cfwdelem::handle_group_features_stats_request(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	cmemory body(0);

	group_table.get_group_features_stats(body);

	send_stats_reply(
					ofctrl,
					pack->get_xid(),
					OFPST_GROUP_FEATURES,
					body.somem(), body.memlen());

	delete pack;
}


void
cfwdelem::handle_experimenter_stats_request(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	/*
	 * send error message back
	 */
	handle_stats_request(ofctrl, pack);
}


void
cfwdelem::send_stats_reply(
		cofctrl *ofctrl,
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

	ofctrl->ctrl->fe_up_stats_reply(this, pack);
}


void
cfwdelem::fe_up_stats_reply(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

#ifndef NDEBUG
	WRITELOG(CFWD, DBG, "STATS-REPLY received: %s", pack->c_str());
#endif
	try {
		//CHECK_PACKET(pack, OFPT_STATS_REPLY);
		check_up_packet(pack, OFPT_STATS_REPLY, entity);

		pack->entity = entity;
		ta_validate(be32toh(pack->ofh_header->xid), OFPT_STATS_REQUEST);
		cofdpath *sw = ofswitch_find(entity);

		sw->stats_reply_rcvd(pack);
		handle_stats_reply(sw, pack);

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_stats_reply() malformed packet received", this);

		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_stats_reply() invalid session exchange xid "
					"(0x%x) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_stats_reply() packet received from non-attached entity", this);

		delete pack;
	}
}




/*
 * SET-CONFIG message
 */


void
cfwdelem::send_set_config_message(
	cofdpath *sw,
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
		sw->entity->fe_down_set_config_request(this, pack);

	} catch (eOFbaseIsBusy& e) {
		WRITELOG(CFWD, DBG, "datapath entity (%llu) busy", sw->dpid);
		delete pack;
		throw;
	}
}


void
cfwdelem::fe_down_set_config_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_SET_CONFIG, entity);

		fe_down_queue[OFPT_SET_CONFIG].push_back(pack);
		register_timer(TIMER_FE_HANDLE_SET_CONFIG, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_set_config_request() packet from non-controlling entity dropped", this);

		//entity->dpath_detach(this);

		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_set_config_request() malformed packet received", this);
		delete pack;
	}
}


void
cfwdelem::recv_set_config()
{
	if (fe_down_queue[OFPT_SET_CONFIG].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_SET_CONFIG].front();
		fe_down_queue[OFPT_SET_CONFIG].pop_front();

		this->flags = be16toh(pack->ofh_switch_config->flags);
		this->miss_send_len = be16toh(pack->ofh_switch_config->miss_send_len);

		handle_set_config(ofctrl_find(pack->entity), pack);

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
cfwdelem::send_packet_out_message(
	cofdpath *sw,
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

	pack->pack();

	// straight call to layer-(n-1) entity's fe_down_set_config_request() method
	sw->entity->fe_down_packet_out(this, pack);

}


void
cfwdelem::fe_down_packet_out(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%s)::fe_down_packet_out() PACKET-OUT received: %s", s_dpid.c_str(), pack->c_str());

	try {
		check_down_packet(pack, OFPT_PACKET_OUT, entity);

		fe_down_queue[OFPT_PACKET_OUT].push_back(pack);
		register_timer(TIMER_FE_HANDLE_PACKET_OUT, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_packet_out() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_packet_out() malformed packet received");
		delete pack;

	} catch (eActionBadLen& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_packet_out() "
				 "bad action len", this);
		send_error_message(ofctrl_find(entity), OFPET_BAD_ACTION, OFPBAC_BAD_LEN,
				(uint8_t*)pack->soframe(), pack->framelen());
		delete pack;
	}

}


void
cfwdelem::recv_packet_out()
{
	if (fe_down_queue[OFPT_PACKET_OUT].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_PACKET_OUT].front();
		fe_down_queue[OFPT_PACKET_OUT].pop_front();

		ofctrl_find(pack->entity)->packet_out_rcvd(pack);

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_packet_out() exception on handle_packet_out() caught", dpname.c_str());
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
cfwdelem::send_packet_in_message(
	uint32_t buffer_id,
	uint16_t total_len,
	uint8_t reason,
	uint8_t table_id,
	cofmatch& match,
	uint8_t* data,
	size_t datalen) throw(eFwdElemNoCtrl)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_packet_in_message() ofctrl_list.size()=%d", this, ofctrl_list.size());

	try {

		for (std::map<cofbase*, cofctrl*>::iterator
				it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
		{
			cofctrl *ofctrl = it->second;

			if (OFPCR_ROLE_SLAVE == ofctrl->role)
			{
				WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_packet_in_message() ofctrl:%p is SLAVE", this, ofctrl);
				continue;
			}

		/*
		 * TODO: this is our flowspace registration support, check with ROLE model of OpenFlow 1.2
		 */
#if 0
		std::set<cfspentry*> nse_list;
		cofctrl *ofctrl = NULL;

		if (fe_flags.test(NSP_ENABLED))
		{
			nse_list = fsptable.find_matching_entries(
					match.oxmlist.oxm_find(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IN_PORT).uint32(),
					total_len,
					data,
					datalen);

			if (nse_list.size() > 1)
			{
				WRITELOG(CFWD, WARN, "cfwdelem(%p) nse_list.size()=%d", this, nse_list.size());
			}

			if (nse_list.empty())
				throw eFwdElemNoCtrl();

			ofctrl = dynamic_cast<cofctrl*>( (*nse_list.begin())->fspowner );
		}
		else
		{
			if (ofctrl_list.empty())
				throw eFwdElemNoCtrl();

			ofctrl = (ofctrl_list.begin()->second);
		}
#endif

			WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_packet_in_message() "
							"sending PACKET-IN for buffer_id:0x%x to controller %s",
							this, buffer_id, ofctrl->c_str());

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

			WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_packet_in_message() "
							"sending PACKET-IN for buffer_id:0x%x pack: %s",
							this, buffer_id, pack->c_str());

			// straight call to layer-(n+1) entity's fe_up_packet_in() method
			ofctrl->ctrl->fe_up_packet_in(this, pack);

		}

	} catch (eFspNoMatch& e) {
		cpacket pack(data, datalen);
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_packet_in_message() no ctrl found for packet: %s", this, pack.c_str());
		throw eFwdElemNoCtrl();
	}
}


void
cfwdelem::fe_up_packet_in(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%s)::fe_up_packet_in() PACKET-IN received: %s", dpname.c_str(), pack->c_str());

	try {
		check_up_packet(pack, OFPT_PACKET_IN, entity);

		pack->entity = entity;
		fe_up_queue[OFPT_PACKET_IN].push_back(pack);
		register_timer(TIMER_FE_HANDLE_PACKET_IN, 0);

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_packet_in() malformed packet received", this);
		delete pack;
	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_packet_in() PACKET-IN from non registered entity %s", this, pack->c_str());
		delete pack;
	}
}


void
cfwdelem::recv_packet_in()
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
		WRITELOG(CFWD, DBG, "packet received from non-attached entity");

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

void
cfwdelem::send_barrier_request(cofdpath *sw)
{
	cofpacket_barrier_request *pack = new cofpacket_barrier_request(ta_add_request(OFPT_BARRIER_REQUEST));

	// straight call to layer-(n-1) entity's fe_down_barrier_request() method
	sw->entity->fe_down_barrier_request(this, pack);

	sw->barrier_request_sent();

}


void
cfwdelem::fe_down_barrier_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_BARRIER_REQUEST, entity);

		fe_down_queue[OFPT_BARRIER_REQUEST].push_back(pack);	// store pack for xid
		register_timer(TIMER_FE_HANDLE_BARRIER_REQUEST, 0);


	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_barrier_request() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_barrier_request() malformed packet received", this);
		delete pack;
	}
}


void
cfwdelem::recv_barrier_request()
{
	WRITELOG(CFWD, DBG, "cfwdelem::send_barrier_reply() "
			"fe_down_queue[OFPT_BARRIER_REQUEST].size()=%d", fe_down_queue[OFPT_BARRIER_REQUEST].size());

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
cfwdelem::send_barrier_reply(
		cofctrl* ofctrl,
		uint32_t xid)
{
	WRITELOG(CFWD, DBG, "cfwdelem::send_barrier_reply() "
			"fe_down_queue[OFPT_BARRIER_REQUEST].size()=%d", fe_down_queue[OFPT_BARRIER_REQUEST].size());

	cofpacket_barrier_reply *pack = new cofpacket_barrier_reply(xid);

	// request is deleted by derived class
	ofctrl->ctrl->fe_up_barrier_reply(this, pack);
}


void
cfwdelem::fe_up_barrier_reply(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%s)::fe_up_barrier_reply() ", dpname.c_str());

	try {
		//CHECK_PACKET(pack, OFPT_BARRIER_REPLY);
		check_up_packet(pack, OFPT_BARRIER_REPLY, entity);

		pack->entity = entity;
		ta_validate(be32toh(pack->ofh_header->xid), OFPT_BARRIER_REQUEST);

		cofdpath *sw = ofswitch_find(pack->entity);
		sw->barrier_reply_rcvd();
		handle_barrier_reply(sw, pack);

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_barrier_reply() malformed packet received", this);

		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_barrier_reply() invalid session exchange xid "
					"(%u) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_barrier_reply() packet received from non-attached entity", this);

		delete pack;
	}
}


/*
 * ROLE.request/reply
 */

void
cfwdelem::send_role_request(
	cofdpath *dpath,
	uint32_t role,
	uint64_t generation_id)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_role_request()", this);

	cofpacket_role_request *pack = new cofpacket_role_request(
										ta_add_request(OFPT_ROLE_REQUEST),
										role,
										generation_id);

	dpath->entity->fe_down_role_request(this, pack);
}


void
cfwdelem::fe_down_role_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_ROLE_REQUEST, entity);

		fe_down_queue[OFPT_ROLE_REQUEST].push_back(pack);	// store pack for xid
		register_timer(TIMER_FE_HANDLE_ROLE_REQUEST, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_role_request() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_role_request() malformed packet received", this);
		delete pack;
	}
}


void
cfwdelem::recv_role_request()
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
cfwdelem::send_role_reply(
		cofctrl *ofctrl,
		uint32_t xid,
		uint32_t role,
		uint64_t generation_id)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_role_reply()", this);

	cofpacket_role_reply *pack = new cofpacket_role_reply(
										xid,
										role,
										generation_id);

	// request is deleted by derived class
	ofctrl->ctrl->fe_up_role_reply(this, pack);
}


void
cfwdelem::fe_up_role_reply(
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
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_barrier_reply() malformed packet received", this);

		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_barrier_reply() invalid session exchange xid "
					"(%u) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_barrier_reply() packet received from non-attached entity", this);

		delete pack;
	}
}


void
cfwdelem::recv_role_reply()
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
cfwdelem::send_error_message(
	cofctrl *ofctrl,
	uint16_t type,
	uint16_t code,
	uint8_t* data,
	size_t datalen)
{
	WRITELOG(CFWD, DBG, "cfwdelem::send_error_message()");

	std::map<cofbase*, cofctrl*>::iterator it;
	for (it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
	{
		cofpacket_error *pack = new cofpacket_error(ta_new_async_xid(), type, code, data, datalen);

		// straight call to layer-(n+1) entity's fe_up_packet_in() method
		it->first->fe_up_error(this, pack);
	}

}


void
cfwdelem::fe_up_error(
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
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_error() malformed packet received", this);
		delete pack;
	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_error() packet received from non-attached entity", this);
		delete pack;
	}
}


void
cfwdelem::recv_error()
{
	if (fe_up_queue[OFPT_ERROR].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_up_queue[OFPT_ERROR].front();
		fe_up_queue[OFPT_ERROR].pop_front();

		handle_error(ofswitch_find(pack->entity), pack);

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "packet received from non-attached entity");

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
cfwdelem::send_flow_mod_message(
	cofdpath *sw,
	cofmatch& ofmatch,
	uint64_t cookie,
	uint64_t cookie_mask,
	uint8_t table_id,
	uint16_t command,
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

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_flow_mod_message() pack: %s", this, flow_mod->c_str());

	// straight call to layer-(n-1) entity's fe_down_flow_mod() method
	sw->entity->fe_down_flow_mod(this, flow_mod);

	sw->flow_mod_sent(flow_mod);


}


void
cfwdelem::send_flow_mod_message(
		cofdpath *sw,
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

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_flow_mod_message() pack: %s",
			this, flow_mod->c_str());

	// straight call to layer-(n-1) entity's fe_down_flow_mod() method
	sw->entity->fe_down_flow_mod(this, flow_mod);

	sw->flow_mod_sent(flow_mod);


}


void
cfwdelem::fe_down_flow_mod(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_FLOW_MOD, entity);

		fe_down_queue[OFPT_FLOW_MOD].push_back(pack);
		register_timer(TIMER_FE_HANDLE_FLOW_MOD, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "packet from non-controlling entity dropped");
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_flow_mod() malformed packet received", this);
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_flow_mod() flow-mod from non-attached entity received", this);
		delete pack;

	} catch (eActionBadLen& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_flow_mod() "
				 "bad action len", this);
		send_error_message(ofctrl_find(entity), OFPET_BAD_ACTION, OFPBAC_BAD_LEN,
				(uint8_t*)pack->soframe(), pack->framelen());
		delete pack;
	}
}


void
cfwdelem::recv_flow_mod()
{
	if (fe_down_queue[OFPT_FLOW_MOD].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_FLOW_MOD].front();
		fe_down_queue[OFPT_FLOW_MOD].pop_front();

		ofctrl_find(pack->entity)->flow_mod_rcvd(pack);




	} catch (eActionBadLen& e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_flow_mod() "
				"invalid flow-mod packet received: action with bad length", dpname.c_str());

		send_error_message(
					ofctrl_find(pack->entity),
					OFPET_BAD_ACTION,
					OFPBAC_BAD_LEN,
					pack->soframe(), pack->framelen());

		delete pack;

	} catch (eFlowTableEntryOverlaps& e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_flow_mod() "
				"flow-entry error: entry overlaps", dpname.c_str());

		send_error_message(
				ofctrl_find(pack->entity),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_OVERLAP,
				pack->soframe(), pack->framelen());

		delete pack;

	} catch (eFspNotAllowed& e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_flow_mod() "
				"-FLOW-MOD- blocked due to mismatch in nsp "
				"registration", dpname.c_str());

		send_error_message(
				ofctrl_find(pack->entity),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_EPERM,
				pack->soframe(), pack->framelen());

		delete pack;

	} catch (eFwdElemTableNotFound& e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_flow_mod() "
				"invalid flow-table %d specified",
				dpname.c_str(), pack->ofh_flow_mod->table_id);

		send_error_message(
				ofctrl_find(pack->entity),
				OFPET_FLOW_MOD_FAILED,
				OFPFMFC_BAD_TABLE_ID,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eInstructionInvalType& e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_flow_mod() "
				"unknown instruction found", dpname.c_str());

		send_error_message(
				ofctrl_find(pack->entity),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNKNOWN_INST,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eFwdElemGotoTableNotFound& e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_flow_mod() "
				"GOTO-TABLE instruction with invalid table-id", dpname.c_str());

		send_error_message(
				ofctrl_find(pack->entity),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_BAD_TABLE_ID,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eInstructionBadExperimenter& e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_flow_mod() "
				"unknown OFPIT_EXPERIMENTER extension received", dpname.c_str());

		send_error_message(
				ofctrl_find(pack->entity),
				OFPET_BAD_INSTRUCTION,
				OFPBIC_UNSUP_EXP_INST,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eOFmatchInvalBadValue& e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_flow_mod() "
				"bad value in match structure", dpname.c_str());

		send_error_message(
				ofctrl_find(pack->entity),
				OFPET_BAD_MATCH,
				OFPBMC_BAD_VALUE,
				(uint8_t*)pack->soframe(), pack->framelen());

		delete pack;

	} catch (eOFbaseNotAttached& e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_flow_mod() "
				"eOFbaseNotAttached thrown", dpname.c_str());

		delete pack;

	} catch (cerror &e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_flow_mod() "
				"default catch for cerror exception", dpname.c_str());

		send_error_message(
				ofctrl_find(pack->entity),
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
cfwdelem::send_group_mod_message(
		cofdpath *sw,
		cgroupentry& ge)
{
	cofpacket_group_mod *pack = new cofpacket_group_mod(
										ta_new_async_xid(),
										be16toh(ge.group_mod->command),
										ge.group_mod->type,
										be32toh(ge.group_mod->group_id));

	pack->buckets = ge.buckets;

	pack->pack();

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_group_mod_message() %s", this, pack->c_str());

	// straight call to layer-(n-1) entity's fe_down_flow_mod() method
	sw->entity->fe_down_group_mod(this, pack);

	sw->group_mod_sent(pack);

}


void
cfwdelem::fe_down_group_mod(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_GROUP_MOD, entity);

		fe_down_queue[OFPT_GROUP_MOD].push_back(pack);
		register_timer(TIMER_FE_HANDLE_GROUP_MOD, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "packet from non-controlling entity dropped");
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_flow_mod() malformed packet received", this);
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_flow_mod() flow-mod from non-attached entity received", this);
		delete pack;

	} catch (eActionBadLen& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_flow_mod() "
				 "bad action len", this);
		send_error_message(ofctrl_find(entity), OFPET_BAD_ACTION, OFPBAC_BAD_LEN,
				(uint8_t*)pack->soframe(), pack->framelen());
		delete pack;
	}
}


void
cfwdelem::recv_group_mod()
{
	if (fe_down_queue[OFPT_GROUP_MOD].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_GROUP_MOD].front();
		fe_down_queue[OFPT_GROUP_MOD].pop_front();

		ofctrl_find(pack->entity)->group_mod_rcvd(pack);



	} catch (eActionBadLen& e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_group_mod() "
				"invalid group-mod packet received: action with "
				"bad length", dpname.c_str());

		send_error_message(
				ofctrl_find(pack->entity),
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
cfwdelem::send_port_mod_message(
	cofdpath *sw,
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
	sw->entity->fe_down_port_mod(this, pack);

}


void
cfwdelem::fe_down_port_mod(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "cfwdelem(%s)::fe_down_port_mod()", dpname.c_str());

	try {
		check_down_packet(pack, OFPT_PORT_MOD, entity);

		fe_down_queue[OFPT_PORT_MOD].push_back(pack);
		register_timer(TIMER_FE_HANDLE_PORT_MOD, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_port_mod() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_port_mod() malformed packet received", this);
		delete pack;
	}
}


void
cfwdelem::recv_port_mod()
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
cfwdelem::send_table_mod_message(
		cofdpath *sw,
		uint8_t table_id,
		uint32_t config)
{
	cofpacket_table_mod *pack = new cofpacket_table_mod(
										ta_new_async_xid(),
										table_id,
										config);

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_table_mod_message() %s", this, pack->c_str());

	// straight call to layer-(n-1) entity's fe_down_table_mod() method
	sw->entity->fe_down_table_mod(this, pack);

	sw->table_mod_sent(pack);

}


void
cfwdelem::fe_down_table_mod(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_TABLE_MOD, entity);

		fe_down_queue[OFPT_TABLE_MOD].push_back(pack);
		register_timer(TIMER_FE_HANDLE_TABLE_MOD, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_table_mod() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_table_mod() malformed packet received", this);
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_table_mod() flow-mod from non-attached entity received", this);
		delete pack;

	}
}


void
cfwdelem::recv_table_mod()
{
	if (fe_down_queue[OFPT_TABLE_MOD].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_down_queue[OFPT_TABLE_MOD].front();
		fe_down_queue[OFPT_TABLE_MOD].pop_front();

		ofctrl_find(pack->entity)->table_mod_rcvd(pack);



	} catch (eFlowTableInval& e) {

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_table_mod() "
				"invalid table-mod packet received", dpname.c_str());

		send_error_message(
				ofctrl_find(pack->entity),
				OFPET_TABLE_MOD_FAILED,
				OFPTMFC_BAD_TABLE,
				pack->soframe(),
				pack->framelen());

		delete pack;

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
cfwdelem::send_flow_removed_message(
	cofctrl *ofctrl,
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
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_flow_removed_message()", this);

		//ofctrl_exists(ofctrl);

		for (std::map<cofbase*, cofctrl*>::iterator
				it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
		{
			cofctrl *ofctrl = it->second;

			if (OFPCR_ROLE_SLAVE == ofctrl->role)
			{
				WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_flow_removed_message() ofctrl:%p is SLAVE", this, ofctrl);
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

			WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_flow_removed_message() to controller %s", this, ofctrl->c_str());

			// straight call to layer-(n+1) entity's fe_up_packet_in() method
			ofctrl->ctrl->fe_up_flow_removed(this, pack);
		}

	} catch (eFwdElemNotFound& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_flow_removed_message() cofctrl instance not found", this);
	}

}


void
cfwdelem::fe_up_flow_removed(
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
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_flow_removed() malformed packet received", this);
		delete pack;
	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_flow_removed() packet received from non-attached entity", this);
		delete pack;
	}
}


void
cfwdelem::recv_flow_removed()
{
	if (fe_up_queue[OFPT_FLOW_REMOVED].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_up_queue[OFPT_FLOW_REMOVED].front();
		fe_up_queue[OFPT_FLOW_REMOVED].pop_front();

		cofdpath *sw = ofswitch_find(pack->entity);
		sw->flow_removed_rcvd(pack);
		handle_flow_removed(sw, pack);

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "packet received from non-attached entity");

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
cfwdelem::send_port_status_message(
	uint8_t reason,
	cofport *port)
{
	WRITELOG(CFWD, DBG, "cfwdelem::send_port_status_message() %s", port->c_str());
	struct ofp_port phy_port;
	send_port_status_message(reason, port->copy(&phy_port));
}


void
cfwdelem::send_port_status_message(
	uint8_t reason,
	struct ofp_port *phy_port)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%s)::send_port_status_message()", get_s_dpid());
	//if (!ctrl)
	//	throw eFwdElemNoCtrl();

	std::map<cofbase*, cofctrl*>::iterator it;
	for (it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
	{
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_port_status_message() to ctrl %s", this, it->second->c_str());

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
cfwdelem::fe_up_port_status(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "cfwdelem(%s)::fe_up_port_status() pack(%p): %s", get_s_dpid(), pack, pack->c_str());

	try {
		//CHECK_PACKET(pack, OFPT_PORT_STATUS);
		check_up_packet(pack, OFPT_PORT_STATUS, entity);

		pack->entity = entity;
		fe_up_queue[OFPT_PORT_STATUS].push_back(pack);
		register_timer(TIMER_FE_HANDLE_PORT_STATUS, 0);

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_port_status() malformed packet received", this);
		delete pack;
	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_port_status() packet received from non-attached entity", this);
		delete pack;
	}
}


void
cfwdelem::recv_port_status()
{
	if (fe_up_queue[OFPT_PORT_STATUS].empty())
		return;

	cofpacket *pack = NULL;
	try {

		pack = fe_up_queue[OFPT_PORT_STATUS].front();
		fe_up_queue[OFPT_PORT_STATUS].pop_front();

		cofdpath *sw = ofswitch_find(pack->entity);
		sw->port_status_rcvd(pack);

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_port_status() packet received from non-attached entity", get_s_dpid());

		delete pack;

	} catch (...) {
		WRITELOG(CFWD, DBG, "cfwdelem(%s)::recv_port_status() GENERIC ERROR", get_s_dpid());

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
cfwdelem::send_queue_get_config_request(
	cofdpath *sw,
	uint32_t port)
{
	cofpacket_queue_get_config_request *pack = new cofpacket_queue_get_config_request(
									ta_add_request(OFPT_QUEUE_GET_CONFIG_REQUEST),
									port);

	// straight call to layer-(n-1) entity's fe_down_get_config_request() method
	sw->entity->fe_down_queue_get_config_request(this, pack);

}


void
cfwdelem::fe_down_queue_get_config_request(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_QUEUE_GET_CONFIG_REQUEST, entity);

		fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].push_back(pack);	// store pack for xid
		register_timer(TIMER_FE_SEND_QUEUE_GET_CONFIG_REPLY, 0);

	} catch (eFwdElemNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_queue_get_config_request() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_queue_get_config_request() malformed packet received", this);
		delete pack;
	}
}


void
cfwdelem::send_queue_get_config_reply()
{
	WRITELOG(CFWD, DBG, "cfwdelem::send_queue_get_config_reply()");

	if (fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].empty())
		return;

	cofpacket *request = fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].front();
	fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].pop_front();

	cofpacket_queue_get_config_reply *pack = new cofpacket_queue_get_config_reply(
										request->get_xid(),
										be32toh(request->ofh_queue_get_config_request->port));

	cofctrl *ofctrl = ofctrl_find(request->entity);

	ofctrl->ctrl->fe_up_queue_get_config_reply(this, pack);

	delete request;

	if (not fe_down_queue[OFPT_QUEUE_GET_CONFIG_REQUEST].empty())
	{
		register_timer(TIMER_FE_SEND_QUEUE_GET_CONFIG_REPLY, 0); // reschedule ourselves
	}

}


void
cfwdelem::fe_up_queue_get_config_reply(
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
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_queue_get_config_reply() malformed packet received", this);
		delete pack;

	} catch (eOFbaseXidInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_queue_get_config_reply() invalid session exchange xid "
					"(%u) received", this, be32toh(pack->ofh_header->xid));
		delete pack;

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_queue_get_config_reply() packet received from non-attached entity", this);
		delete pack;
	}
}


/*
 * VENDOR messages
 */

void
cfwdelem::fe_down_experimenter_message(
	cofbase *entity,
	cofpacket *pack)
{
	WRITELOG(UNKNOWN, DBG, "%s()", __FUNCTION__);

	try {
		check_down_packet(pack, OFPT_EXPERIMENTER, entity);

		fe_down_queue[OFPT_EXPERIMENTER].push_back(pack);
		register_timer(TIMER_FE_HANDLE_EXPERIMENTER, 0);

	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_vendor_message() packet from non-controlling entity dropped", this);
		//entity->dpath_detach(this);
		delete pack;

	} catch (eOFbaseInval& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_down_vendor_message() malformed packet received", this);
		delete pack;
	}
}


void
cfwdelem::fe_up_experimenter_message(
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
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_experimenter_message() malformed packet received", this);
		delete pack;
	} catch (eOFbaseNotAttached& e) {
		WRITELOG(CFWD, DBG, "cfwdelem(%p)::fe_up_experimenter_message() packet received from non-attached entity", this);
		delete pack;
	}
}


void
cfwdelem::recv_experimenter_message()
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
cfwdelem::send_experimenter_message(
		cofdpath *sw,
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

	WRITELOG(CFWD, DBG, "cfwdelem(%s)::send_experimenter_message() -down- %s", get_s_dpid(), pack->c_str());

	if (NULL == sw) // send to all attached data path entities
	{
		std::map<cofbase*, cofdpath*>::iterator it;
		for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
		{
			it->second->entity->fe_down_experimenter_message(this, new cofpacket(*pack));
		}
		delete pack;
	}
	else
	{
		// straight call to layer-(n-1) entity's fe_down_experimenter_message() method
		sw->entity->fe_down_experimenter_message(this, pack);
	}

}


void
cfwdelem::send_experimenter_message(
		cofctrl *ctrl,
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

	WRITELOG(CFWD, DBG, "cfwdelem(%s)::send_experimenter_message() -up- %s", get_s_dpid(), pack->c_str());

	if (NULL == ctrl) // send to all attached controller entities
	{
		std::map<cofbase*, cofctrl*>::iterator it;
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


void
cfwdelem::send_experimenter_ext_rofl_nsp_get_fsp_request(
		cofdpath *sw)
{

}


void
cfwdelem::send_experimenter_ext_rofl_nsp_get_fsp_reply(
		cofpacket *request,
		cofctrl *ofctrl,
		std::set<cofmatch*>& match_list)
{

}


void
cfwdelem::send_experimenter_ext_rofl_nsp_open_request(
		cofdpath *sw,
		cofmatch const& match)
{
	cofmatch m(match);

	cmemory mem(m.length());

	m.pack((struct ofp_match*)mem.somem(), mem.memlen());

	cofpacket_experimenter *pack = new cofpacket_experimenter(
										ta_add_request(OFPT_EXPERIMENTER),
										VENDOR_EXT_ROFL,
										OFPRET_NSP_OPEN_REQUEST,
										mem.somem(),
										mem.memlen());

	pack->pack();

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_experimenter_ext_rofl_rsp_open_request() "
			"pack: %s", this, pack->c_str());

	// straight call to layer-(n-1) entity's fe_down_flow_mod() method
	sw->entity->fe_down_experimenter_message(this, pack);

}


void
cfwdelem::send_experimenter_ext_rofl_nsp_open_reply(
		cofpacket *request,
		cofctrl *ofctrl,
		uint32_t result, // host byte order
		cofmatch const& match)
{
	cofmatch m(match);

	cmemory mem(m.length());

	m.pack((struct ofp_match*)mem.somem(), mem.memlen());

	cofpacket_experimenter *pack = new cofpacket_experimenter(
										request->get_xid(),
										VENDOR_EXT_ROFL,
										OFPRET_NSP_OPEN_REPLY,
										mem.somem(),
										mem.memlen());

	pack->pack();

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_experimenter_ext_rofl_rsp_open_reply() "
			"pack: %s", this, pack->c_str());

	ofctrl->ctrl->fe_up_experimenter_message(this, pack);

}


void
cfwdelem::send_experimenter_ext_rofl_nsp_close_request(
		cofdpath *sw,
		cofmatch const& match)
{
	cofmatch m(match);

	cmemory mem(m.length());

	m.pack((struct ofp_match*)mem.somem(), mem.memlen());

	cofpacket_experimenter *pack = new cofpacket_experimenter(
										ta_add_request(OFPT_EXPERIMENTER),
										VENDOR_EXT_ROFL,
										OFPRET_NSP_CLOSE_REQUEST,
										mem.somem(),
										mem.memlen());

	pack->pack();

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_experimenter_ext_rofl_rsp_close_request() "
			"pack: %s", this, pack->c_str());

	// straight call to layer-(n-1) entity's fe_down_flow_mod() method
	sw->entity->fe_down_experimenter_message(this, pack);

}


void
cfwdelem::send_experimenter_ext_rofl_nsp_close_reply(
		cofpacket *request,
		cofctrl *ofctrl,
		uint32_t result, // host byte order
		cofmatch const& match)
{
	cofmatch m(match);

	cmemory mem(m.length());

	m.pack((struct ofp_match*)mem.somem(), mem.memlen());

	cofpacket_experimenter *pack = new cofpacket_experimenter(
										request->get_xid(),
										VENDOR_EXT_ROFL,
										OFPRET_NSP_CLOSE_REPLY,
										mem.somem(),
										mem.memlen());

	pack->pack();

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::send_experimenter_ext_rofl_rsp_close_reply() "
			"pack: %s", this, pack->c_str());

	ofctrl->ctrl->fe_up_experimenter_message(this, pack);


}



uint32_t
cfwdelem::fib_table_find(uint64_t from, uint64_t to) throw (eFwdElemNotFound)
{
	std::set<cfibentry*>::iterator kt;
	if ((kt = find_if(fib_table.begin(), fib_table.end(),
			cfibentry::cfibentry_map_port(from, to))) == fib_table.end())
	{
		throw eFwdElemNotFound();
	}
	return (*kt)->port_no;
}


uint32_t
cfwdelem::phy_port_get_free_portno()
throw (eFwdElemNotFound)
{
	uint32_t portno = 1;
	while (phy_ports.find(portno) != phy_ports.end())
	{
		portno++;
		if (portno == std::numeric_limits<uint32_t>::max())
		{
			throw eFwdElemNotFound();
		}
	}
	return portno;
}


