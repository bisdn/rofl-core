/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cofbase.h"


std::set<cofbase*> cofbase::cofbase_list;


/* static */
cofbase*
cofbase::cofbase_exists(cofbase* ofbase) throw (eOFbaseNotFound)
{
	if (cofbase::cofbase_list.find(ofbase) != cofbase::cofbase_list.end())
	{
		return ofbase;
	}
	throw eOFbaseNotFound();
}



cofbase::cofbase() :
	xid_used_max(CPCP_DEFAULT_XID_USED_MAX), // max number of lately used xids
	xid_start(crandom(sizeof(uint32_t)).uint32())

{
	WRITELOG(TERMINUS, ROFL_DBG, "new cofbase element");
	cofbase::cofbase_list.insert(this);
}

cofbase::~cofbase()
{
	cofbase::cofbase_list.erase(this);
	WRITELOG(TERMINUS, ROFL_DBG, "destroy cofbase element");

	std::map<int, std::list<cofpacket*> >::iterator it;

	// remove all pending packets from all down-queues
	for (it = fe_down_queue.begin(); it != fe_down_queue.end(); ++it)
	{		std::list<cofpacket*> plist = it->second;
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
}


const char*
cofbase::c_str()
{
	bzero(info, sizeof(info));

	snprintf(info, sizeof(info)-1, "cofbase[%p]", this);

	return info;
}


void
cofbase::fe_down_command(
		cofbase *entity,
		cofpacket *pack)
throw (eOFbaseInval)
{
	switch (pack->ofh_header->type) {
	case OFPT_EXPERIMENTER:
		fe_down_experimenter_message(entity, pack);
		break;
	case OFPT_FEATURES_REQUEST:
		fe_down_features_request(entity, pack);
		break;
	case OFPT_GET_CONFIG_REQUEST:
		fe_down_get_config_request(entity, pack);
		break;
	case OFPT_SET_CONFIG:
		fe_down_set_config_request(entity, pack);
		break;
	case OFPT_PACKET_OUT:
		fe_down_packet_out(entity, pack);
		break;
	case OFPT_FLOW_MOD:
		fe_down_flow_mod(entity, pack);
		break;
	case OFPT_GROUP_MOD:
		fe_down_group_mod(entity, pack);
		break;
	case OFPT_PORT_MOD:
		fe_down_port_mod(entity, pack);
		break;
	case OFPT_TABLE_MOD:
		fe_down_table_mod(entity, pack);
		break;
	case OFPT_STATS_REQUEST:
		fe_down_stats_request(entity, pack);
		break;
	case OFPT_BARRIER_REQUEST:
		fe_down_barrier_request(entity, pack);
		break;
	case OFPT_QUEUE_GET_CONFIG_REQUEST:
		fe_down_queue_get_config_request(entity, pack);
		break;
	default:
		throw eOFbaseInval();
	}
}


void
cofbase::fe_up_command(
		cofbase *entity,
		cofpacket *pack)
throw (eOFbaseInval)
{
	switch (pack->ofh_header->type) {
	case OFPT_ERROR:
		fe_up_error(entity, pack);
		break;
	case OFPT_EXPERIMENTER:
		fe_up_experimenter_message(entity, pack);
		break;
	case OFPT_FEATURES_REPLY:
		fe_up_features_reply(entity, pack);
		break;
	case OFPT_GET_CONFIG_REPLY:
		fe_up_get_config_reply(entity, pack);
		break;
	case OFPT_PACKET_IN:
		fe_up_packet_in(entity, pack);
		break;
	case OFPT_FLOW_REMOVED:
		fe_up_flow_removed(entity, pack);
		break;
	case OFPT_PORT_STATUS:
		fe_up_port_status(entity, pack);
		break;
	case OFPT_STATS_REPLY:
		fe_up_stats_reply(entity, pack);
		break;
	case OFPT_BARRIER_REPLY:
		fe_up_barrier_reply(entity, pack);
		break;
	case OFPT_QUEUE_GET_CONFIG_REPLY:
		fe_up_queue_get_config_reply(entity, pack);
		break;
	default:
		throw eOFbaseInval();
	}
}


// OpenFlow session ids (xid) management
//



uint32_t
cofbase::ta_add_request(uint8_t type)
{
	uint32_t xid = ta_new_async_xid();

	// add pair(type, xid) to transaction list
	//ta_pending_reqs.insert(std::make_pair<uint32_t, uint8_t>(xid, type));
	ta_pending_reqs[xid] = type;

	WRITELOG(XID, ROFL_DBG, "cofbase::ta_add_request() rand number=0x%x", xid);

#ifndef NDEBUG
	std::map<uint32_t, uint8_t>::iterator it;
	for (it = ta_pending_reqs.begin(); it != ta_pending_reqs.end(); ++it) {
		WRITELOG(XID, ROFL_DBG, "cofbase::ta_pending_request: xid=0x%x type=%d",
				 (*it).first, (*it).second);
	}
#endif

	return xid;
}

void
cofbase::ta_rem_request(uint32_t xid)
{
	ta_pending_reqs.erase(xid);
	// this yields an exception if type wasn't stored in ta_pending_reqs
}

bool
cofbase::ta_pending(uint32_t xid, uint8_t type)
{
#ifndef NDEBUG
	std::map<uint32_t, uint8_t>::iterator it;
	for (it = ta_pending_reqs.begin(); it != ta_pending_reqs.end(); ++it) {
		WRITELOG(XID, ROFL_DBG, "cofbase::ta_pending_request: xid=0x%x type=%d",
				 (*it).first, (*it).second);
	}

	WRITELOG(XID, ROFL_DBG, "%s 0x%x %d %d",
			(ta_pending_reqs.find(xid) != ta_pending_reqs.end()) ? "true" : "false",
			xid, ta_pending_reqs[xid], (int)type);
#endif

	return((ta_pending_reqs[xid] == type) &&
		   (ta_pending_reqs.find(xid) != ta_pending_reqs.end()));
}

bool
cofbase::ta_active_xid(uint32_t xid)
{
	return(ta_pending_reqs.find(xid) != ta_pending_reqs.end());
}

uint32_t
cofbase::ta_new_async_xid()
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
cofbase::ta_validate(cofpacket *pack) throw ()
{
		return ta_validate(be32toh(pack->ofh_header->xid), pack->ofh_header->type);
}


bool
cofbase::ta_validate(uint32_t xid, uint8_t type) throw (eOFbaseXidInval)
{
	// check for pending transaction of type 'type'
	if (!ta_pending(xid, type)) {
		WRITELOG(XID, ROFL_DBG, "no pending transaction");
		throw eOFbaseXidInval();
		//return false;
	}

	// delete transaction
	ta_rem_request(xid);

	return true;
}



