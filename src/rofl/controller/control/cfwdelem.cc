/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cfwdelem.h"

using namespace rofl;

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
		caddress const& rpc_dpt_addr) :
				dpname(dpname),
				dpid(dpid),
				n_buffers(n_buffers),
				n_tables(n_tables),
				capabilities(OFPC_FLOW_STATS | OFPC_TABLE_STATS | OFPC_PORT_STATS | OFPC_GROUP_STATS),
				// OFPC_IP_REASM, OFPC_QUEUE_STATS, OFPC_PORT_BLOCKED
				flags(0),
				miss_send_len(DEFAULT_FE_MISS_SEND_LEN)
{
	crofbase::rpc_listen_for_ctls(rpc_ctl_addr);

	crofbase::rpc_listen_for_dpts(rpc_dpt_addr);

	cvastring vas;
	s_dpid.assign(vas("dpid[%016llx]", dpid));

	// create all flow-tables ...
	for (int i = 0; i < n_tables; ++i)
	{
		flow_tables[i] = new cfttable(this, i);
	}

	WRITELOG(CFWD, DBG, "cfwdelem(%p)::cfwdelem() "
			"dpid:%llu dpname=%s", this, dpid, dpname.c_str());

	cfwdelem::fwdelems.insert(this);
}



cfwdelem::~cfwdelem()
{
	cfwdelem::fwdelems.erase(this);

	WRITELOG(CFWD, DBG, "destroy cfwdelem(%p)::cfwdelem() ", this);

	// remove all physical ports
	while (not phy_ports.empty())
	{
		delete (phy_ports.begin()->second);
	}

	while (not fib_table.empty())
	{
		delete *(fib_table.begin());
	}

	// remove all flow-tables
	std::map<uint8_t, cfttable*>::iterator ft;
	for (ft = flow_tables.begin(); ft != flow_tables.end(); ++ft)
	{
		delete (*ft).second;
	}
	flow_tables.clear();
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

	info.assign(vas("cfwdelem(%p): =>", this));

	// cofport instances
	info.append(vas("\nlist of registered cofport instances: =>"));
	std::map<uint32_t, cofport*>::iterator it;
	for (it = phy_ports.begin(); it != phy_ports.end(); ++it)
	{
		info.append(vas("\n  %s", it->second->c_str()));
	}

	return info.c_str();
}



void
cfwdelem::port_attach(
		std::string devname,
		uint32_t port_no)
{
	if (phy_ports.find(port_no) == phy_ports.end())
	{
		new cphyport(&phy_ports, port_no);

		send_port_status_message(OFPPR_ADD, *phy_ports[port_no]);
	}
}



void
cfwdelem::port_detach(
		uint32_t port_no)
{
	if (phy_ports.find(port_no) != phy_ports.end())
	{
		send_port_status_message(OFPPR_DELETE, *phy_ports[port_no]);

		delete phy_ports[port_no];
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
cfwdelem::handle_timeout(int opaque)
{
	try {
		switch (opaque) {
		default: {
			crofbase::handle_timeout(opaque);
		} break;
		}


	} catch (eIoSvcUnhandledTimer& e) {
		// ignore
	}
}



cftentry*
cfwdelem::hw_create_cftentry(
		uint8_t of_version,
		cftentry_owner *owner,
		std::set<cftentry*> *flow_table,
		cofmsg_flow_mod *pack)
{
	return new cftentry(of_version, owner, flow_table, pack);
}



void
cfwdelem::ftentry_timeout(
		cftentry *fte,
		uint16_t timeout)
{
	if (fte->get_flags() & OFPFF_SEND_FLOW_REM)
	{
		cclock since;
		since -= fte->flow_create_time;

		send_flow_removed_message(
			fte->ctl,
			fte->ofmatch,
			fte->get_cookie(),
			fte->get_priority(),
			fte->removal_reason,
			fte->get_tableid(),
			since.ts.tv_sec,
			since.ts.tv_nsec,
			fte->get_idle_timeout(),
			fte->get_hard_timeout(),
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



void
cfwdelem::handle_features_request(cofctl *ctl, cofmsg_features_request *request)
{
 	WRITELOG(CFWD, DBG, "cfwdelem(%s)::handle_features_request()", dpname.c_str());

 	cofportlist portlist;

 	for (std::map<uint32_t, cofport*>::iterator
 			it = phy_ports.begin(); it != phy_ports.end(); ++it)
 	{
 		portlist.next() = *(it->second);
 	}

 	send_features_reply(
 			ctl,
 			request->get_xid(),
 			dpid,
 			n_buffers,
 			n_tables,
 			capabilities,
 			0, // of13_auxiliary_id
 			0, // of10_actions_bitmap
 			portlist);

 	delete request;
}



void
cfwdelem::handle_get_config_request(cofctl *ctl, cofmsg_get_config_request *pack)
{
	send_get_config_reply(
			ctl,
			pack->get_xid(),
			flags,
			miss_send_len);

	delete pack;
}



void
cfwdelem::handle_set_config(cofctl *ctl, cofmsg_set_config *pack)
{
	flags 			= pack->get_flags();
	miss_send_len 	= pack->get_miss_send_len();

	//flags = be16toh(pack->ofh_switch_config->flags);
	//miss_send_len = be16toh(pack->ofh_switch_config->miss_send_len);

	delete pack;
}



void
cfwdelem::handle_desc_stats_request(cofctl *ctl, cofmsg_stats_request *pack)
{
	std::string mfr_desc("Revised OpenFlow Library");
	std::string hw_desc("v0.2.22");
	std::string sw_desc("v0.2.22");
	std::string serial_num("0");
	std::string dp_desc("somehow, somewhere, ...");

	cofdesc_stats_reply desc_stats_reply(
			ctl->get_version(),
			mfr_desc,
			hw_desc,
			sw_desc,
			serial_num,
			dp_desc);

	send_desc_stats_reply(
					ctl,
					pack->get_xid(),
					desc_stats_reply);

	delete pack;
}



void
cfwdelem::handle_table_stats_request(cofctl *ctl, cofmsg_table_stats_request *msg)
{
	switch (ctl->get_version()) {
	case OFP10_VERSION: {

	} break;
	case OFP12_VERSION: {
		cmemory body(flow_tables.size() * sizeof(struct ofp12_table_stats)); // array of struct ofp_table_stats

		struct ofp12_table_stats *table_stats = // auxiliary pointer for iterating through array of table_stats
				(struct ofp12_table_stats*)body.somem();


		std::map<uint8_t, cfttable*>::iterator it;
		for (it = flow_tables.begin(); it != flow_tables.end(); ++it)
		{
			coftable_stats_reply& table_stats_reply = it->second->get_table_stats();

			table_stats_reply.pack((uint8_t*)table_stats, sizeof(struct ofp12_table_stats)); // FIXME: version dependency

			table_stats++; // jump to start of array + 1
		}

		WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_stats_table_request() table_stats[%p] body: %s",
				this, table_stats, body.c_str());

		send_stats_reply(
				ctl,
				msg->get_xid(),
				OFPST_TABLE,
				(uint8_t*)body.somem(), body.memlen());

	} break;
	default:
		break;
	}

	delete msg;
}



void
cfwdelem::handle_port_stats_request(cofctl *ctl, cofmsg_port_stats_request *msg)
{
	//uint32_t port_no = be32toh(pack->ofb_port_stats_request->port_no);
	uint32_t port_no = msg->get_port_stats().get_portno();

	cmemory body(0);

	try {
		if (OFPP_ANY == port_no)
		{
			for (std::map<uint32_t, cofport*>::iterator
					it = phy_ports.begin(); it != phy_ports.end(); ++it)
			{
				cofport *port = it->second;
				cofport_stats_reply& stats_reply = port->get_port_stats();
				cmemory m_stats(stats_reply.length());
				stats_reply.pack(m_stats.somem(), m_stats.memlen());

				body += m_stats;
			}
		}
		else
		{
			if (phy_ports.find(port_no) == phy_ports.end())
			{
				throw eFwdElemNotFound();
			}

			cofport_stats_reply& stats_reply = phy_ports[port_no]->get_port_stats();
			body.resize(stats_reply.length());
			stats_reply.pack(body.somem(), body.memlen());
		}

		send_stats_reply(
					ctl,
					msg->get_xid(),
					OFPST_PORT,
					body.somem(), body.memlen());



	} catch (eFwdElemNotFound& e) {

		send_error_message(ctl, msg->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_PORT,
				msg->soframe(), msg->framelen());

	}

	delete msg;
}



void
cfwdelem::handle_flow_stats_request(cofctl *ctl, cofmsg_flow_stats_request *msg)
{
	//uint8_t table_id = pack->ofb_flow_stats_request->table_id;
	uint8_t table_id = msg->get_flow_stats().get_table_id();

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
						msg->get_flow_stats().get_out_port(),
						msg->get_flow_stats().get_out_group(),
						msg->get_flow_stats().get_cookie(),
						msg->get_flow_stats().get_cookie_mask(),
						msg->get_flow_stats().get_match());
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
							msg->get_flow_stats().get_out_port(),
							msg->get_flow_stats().get_out_group(),
							msg->get_flow_stats().get_cookie(),
							msg->get_flow_stats().get_cookie_mask(),
							msg->get_flow_stats().get_match());
		}

		send_stats_reply(
						ctl,
						msg->get_xid(),
						OFPST_FLOW,
						body.somem(), body.memlen());



	} catch (eFwdElemTableNotFound& e) {

		send_error_message(ctl, msg->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_TABLE_ID,
				msg->soframe(), msg->framelen());

	}

	delete msg;
}



void
cfwdelem::handle_aggregate_stats_request(cofctl *ctl, cofmsg_aggr_stats_request *msg)
{
	//uint8_t table_id = pack->ofb_aggregate_stats_request->table_id;
	uint8_t table_id = msg->get_aggr_stats().get_table_id();

	uint64_t packet_count = 0;
	uint64_t byte_count = 0;

	uint64_t flow_count = 0; // FIXME: flow_count should be uint32_t

	cofstats_aggregate_request aggr(msg->get_body().somem(), msg->get_body().memlen());

#if 0
	fprintf(stderr, "cfwdelem::handle_aggregate_stats_request() "
			"table-id: %d match: %s\n",
			aggr.ofs_aggr_stats_request->table_id, msg->match.c_str());
#endif

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
						msg->get_aggr_stats().get_out_port(),
						msg->get_aggr_stats().get_out_group(),
						msg->get_aggr_stats().get_cookie(),
						msg->get_aggr_stats().get_cookie_mask(),
						msg->get_aggr_stats().get_match());

#if 0
				fprintf(stderr, "cfwdelem::handle_aggregate_stats_request() "
						"packet_count: 0x%lx byte_count: 0x%lx flow_count: 0x%lx",
						packet_count, byte_count, flow_count);
#endif
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
							msg->get_aggr_stats().get_out_port(),
							msg->get_aggr_stats().get_out_group(),
							msg->get_aggr_stats().get_cookie(),
							msg->get_aggr_stats().get_cookie_mask(),
							msg->get_aggr_stats().get_match());
		}

		cofaggr_stats_reply aggr_stats_reply(ctl->get_version());

		aggr_stats_reply.set_packet_count(packet_count);
		aggr_stats_reply.set_byte_count(byte_count);
		aggr_stats_reply.set_flow_count(flow_count);

		WRITELOG(CFWD, DBG, "cfwdelem(%s)::handle_aggregate_stats_request() "
				"packet_count:%llu byte_count:%llu flow_count:%llu",
				dpname.c_str(), packet_count, byte_count, flow_count);

		cmemory body(aggr_stats_reply.length());
		aggr_stats_reply.pack(body.somem(), body.memlen());

		send_stats_reply(
						ctl,
						msg->get_xid(),
						OFPST_AGGREGATE,
						body.somem(), body.memlen());



	} catch (eFwdElemTableNotFound& e) {

		send_error_message(ctl, msg->get_xid(), OFPET_BAD_REQUEST, OFPBRC_BAD_TABLE_ID,
				msg->soframe(), msg->framelen());

	}

	delete msg;
}



void
cfwdelem::handle_queue_stats_request(cofctl *ctl, cofmsg_stats_request *pack)
{
	/*
	 * we do not support queues in cfwdelem (maybe in a derived class),
	 * so we send an empty reply message back (body is empty)
	 */

	send_stats_reply(
				ctl,
				pack->get_xid(),
				OFPST_QUEUE);

	delete pack;
}



void
cfwdelem::handle_group_stats_request(cofctl *ctl, cofmsg_group_stats_request *msg)
{
	//uint32_t group_id = be32toh(pack->ofb_group_stats_request->group_id);
	uint32_t group_id = msg->get_group_stats().get_group_id();

	cmemory body(0);

	if (OFPG_ALL == group_id)
	{
		group_table.get_group_stats(body);
	}
	else
	{
		group_table[group_id]->get_group_stats(body);
	}

	send_stats_reply(
				ctl,
				msg->get_xid(),
				OFPST_GROUP,
				body.somem(), body.memlen());

	delete msg;
}



void
cfwdelem::handle_group_desc_stats_request(cofctl *ctl, cofmsg_stats_request *pack)
{
	cmemory body(0);

	group_table.get_group_desc_stats(body);

	send_stats_reply(
					ctl,
					pack->get_xid(),
					OFPST_GROUP_DESC,
					body.somem(), body.memlen());

	delete pack;
}



void
cfwdelem::handle_group_features_stats_request(cofctl *ctl, cofmsg_stats_request *pack)
{
	cmemory body(0);

	group_table.get_group_features_stats(body);

	send_stats_reply(
					ctl,
					pack->get_xid(),
					OFPST_GROUP_FEATURES,
					body.somem(), body.memlen());

	delete pack;
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



void
cfwdelem::handle_flow_mod(cofctl *ctl, cofmsg_flow_mod *pack)
{
	cftentry *fte = NULL;

	try {

		try {
			WRITELOG(CFWD, DBG, "cfwdelem(%p)::handle_flow_mod() checking Goto-Table", this);

			cofinst& inst = pack->get_instructions().find_inst(OFPIT_GOTO_TABLE);

			if (flow_tables.find(inst.oin_goto_table->table_id) == flow_tables.end()) {
				writelog(CFWD, WARN, "cfwdelem(%s)::handle_flow_mod() "
						"flow-entry error, bad table-id %d in Goto-Table instruction",
						dpname.c_str(), pack->get_table_id());
				throw eFlowModBadTableId();
			}

		} catch (eInListNotFound& e) {}


		// table_id == 255 (all tables)
		if (OFPTT_ALL == pack->get_table_id())
		{
			std::map<uint8_t, cfttable*>::iterator it;
			for (it = flow_tables.begin(); it != flow_tables.end(); ++it)
			{
				if ((fte = it->second->update_ft_entry(this, pack)) != NULL)
				{
					fte->ctl = ctl; // store controlling entity for this cftentry
					WRITELOG(CFWD, DBG, "cofctrl(%p)::flow_mod_rcvd() table_id %d new %s",
							this, pack->get_table_id(), fte->c_str());
				}
			}
		}
		// single table
		else
		{
			// check for existence of specified table
			if (flow_tables.find(pack->get_table_id()) == flow_tables.end()) {
				writelog(CFWD, WARN, "cfwdelem(%s)::handle_flow_mod() "
						"flow-entry error, bad table-id %d",
						dpname.c_str(), pack->get_table_id());
				throw eFlowModBadTableId();
			}

			// do not lock here flow_table[i]

			fte = flow_tables[pack->get_table_id()]->update_ft_entry(this, pack);

			switch (pack->get_command()) {
			case OFPFC_ADD:
			case OFPFC_MODIFY:
			case OFPFC_MODIFY_STRICT: { // check cftentry instance
				if (0 == fte) {
					writelog(CFWD, WARN, "cfwdelem(%s)::handle_flow_mod() "
							"flow-entry error when updating local flow-table",
							dpname.c_str());
					throw eFlowModUnknown();
				}
				fte->ctl = ctl; // store controlling entity for this cftentry
				WRITELOG(CFWD, DBG, "cofctrl(%p)::flow_mod_rcvd() table_id %d new %s",
						this, pack->get_table_id(), fte->c_str());
			} break;
			case OFPFC_DELETE:
			case OFPFC_DELETE_STRICT:
			default: {
				// do nothing here
			} break;
			}
		}

		switch (pack->get_command()) {
		case OFPFC_ADD: {
			flow_mod_add(ctl, pack, flow_tables[pack->get_table_id()], fte);
		} break;
		case OFPFC_MODIFY:
		case OFPFC_MODIFY_STRICT: {
			flow_mod_modify(ctl, pack, flow_tables[pack->get_table_id()], fte);
		} break;
		case OFPFC_DELETE:
		case OFPFC_DELETE_STRICT: {
			flow_mod_delete(ctl, pack);
		} break;
		default: {
			writelog(CFWD, WARN, "cfwdelem(%s)::handle_flow_mod() "
					"flow-entry error => invalid command %d",
					dpname.c_str(), pack->get_command());
			throw eFlowModUnknown();
		} break;
		}

	} catch (eFlowTableEntryOverlaps& e) {

		writelog(CFWD, WARN, "cfwdelem(%s)::handle_flow_mod() "
				"flow-entry error: entry overlaps", dpname.c_str());
		throw eFlowModOverlap();
	}
}



void
cfwdelem::handle_group_mod(cofctl *ctl, cofmsg_group_mod *msg)
{

	cgtentry *gte = group_table.update_gt_entry(this, msg);

	if (0 != gte)
	{
		switch (msg->get_command()) {
		case OFPGC_ADD: {
			group_mod_add(ctl, msg, gte);
		} break;
		case OFPGC_MODIFY: {
			group_mod_modify(ctl, msg, gte);
		} break;
		case OFPGC_DELETE: {
			group_mod_delete(ctl, msg, gte);
		} break;
		}
	}

	delete msg;
}


void
cfwdelem::handle_table_mod(cofctl *ofctrl, cofmsg_table_mod *pack)
{
	if (flow_tables.find(pack->get_table_id()) != flow_tables.end()) {
		flow_tables[pack->get_table_id()]->set_config(pack->get_config());
	}
	delete pack;
}



void
cfwdelem::handle_port_mod(cofctl *ofctrl, cofmsg_port_mod *pack)
{
	uint32_t port_no = pack->get_port_no();

	if (phy_ports.find(port_no) == phy_ports.end()) {
		throw ePortModBadPort();
	}

	phy_ports[port_no]->recv_port_mod(pack->get_config(), pack->get_mask(), pack->get_advertise());

	delete pack;
}



void
cfwdelem::handle_stats_reply(cofdpt *dpt, cofmsg_stats_reply *msg)
{
	delete msg;
}



void
cfwdelem::handle_table_stats_reply(cofdpt *dpt, cofmsg_table_stats_reply *msg)
{
	std::vector<coftable_stats_reply>& table_stats_list = msg->get_table_stats();

	for (std::vector<coftable_stats_reply>::iterator
			it = table_stats_list.begin(); it != table_stats_list.end(); ++it) {

		coftable_stats_reply& table_stats = (*it);

		if (flow_tables.find(table_stats.get_table_id()) == flow_tables.end()) {
			flow_tables[table_stats.get_table_id()] = new cfttable(0, table_stats.get_table_id());
		}

		flow_tables[table_stats.get_table_id()]->set_table_stats(table_stats);
	}

	delete msg;
}



void
cfwdelem::handle_flow_removed(cofdpt *sw, cofmsg_flow_removed *pack)
{
	if (flow_tables.find(pack->get_table_id()) != flow_tables.end()) {
		flow_tables[pack->get_table_id()]->update_ft_entry(this, pack);
	}
	delete pack;
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
			throw eRofBaseNotFound();
		}
	}
	return portno;
}



void
cfwdelem::inc_group_reference_count(
		uint32_t groupid,
		cftentry *fte)
{
	group_table[groupid]->ref_count++;
}



void
cfwdelem::dec_group_reference_count(
		uint32_t groupid,
		cftentry *fte)
{
	group_table[groupid]->ref_count--;
}



void
cfwdelem::cftentry_idle_timeout(
		cftentry *fte)
{
	if (OFPFF_SEND_FLOW_REM & fte->get_flags())
	{
		cclock since;
		since -= fte->flow_create_time;

		send_flow_removed_message(
			fte->ctl,
			fte->ofmatch,
			fte->get_cookie(),
			fte->get_priority(),
			fte->removal_reason,
			fte->get_tableid(),
			since.ts.tv_sec,
			since.ts.tv_nsec,
			fte->get_idle_timeout(),
			fte->get_hard_timeout(),
			fte->rx_packets,
			fte->rx_bytes);
	}
}




void
cfwdelem::cftentry_hard_timeout(
		cftentry *fte)
{
	if (OFPFF_SEND_FLOW_REM & fte->get_flags())
	{
		cclock since;
		since -= fte->flow_create_time;

		send_flow_removed_message(
			fte->ctl,
			fte->ofmatch,
			fte->get_cookie(),
			fte->get_priority(),
			fte->removal_reason,
			fte->get_tableid(),
			since.ts.tv_sec,
			since.ts.tv_nsec,
			fte->get_idle_timeout(),
			fte->get_hard_timeout(),
			fte->rx_packets,
			fte->rx_bytes);
	}
}


#if 0
void
cfwdelem::cftentry_delete(
		cftentry *fte)
{

}
#endif


