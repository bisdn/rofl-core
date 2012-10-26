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
		caddress const& rpc_dpt_addr) :
		crofbase(dpname, dpid, n_tables, n_buffers, rpc_ctl_addr, rpc_dpt_addr)
{
	cvastring vas;

	// create all flow-tables ...
	for (int i = 0; i < n_tables; ++i)
	{
		flow_tables[i] = new cfttable(this, i);
	}

	WRITELOG(CFWD, ROFL_DBG, "cfwdelem(%p)::cfwdelem() "
			"dpid:%llu dpname=%s", this, dpid, dpname.c_str());

	cfwdelem::fwdelems.insert(this);
}


cfwdelem::~cfwdelem()
{
	cfwdelem::fwdelems.erase(this);

	WRITELOG(CFWD, ROFL_DBG, "destroy cfwdelem(%p)::cfwdelem() ", this);

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
cfwdelem::handle_dpath_open(cofdpath *dpt)
{

}



void
cfwdelem::handle_dpath_close(cofdpath *dpt)
{

}



void
cfwdelem::handle_ctrl_open(cofctrl *ctl)
{

}



void
cfwdelem::handle_ctrl_close(cofctrl *ctl)
{

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

	return info.c_str();
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
		default:
			{
				crofbase::handle_timeout(opaque);
			}
			break;
		}


	} catch (eOFbaseNoCtrl& e) {
		WRITELOG(CFWD, ROFL_DBG, "controlling entity lost");
		// handle NoCtrl condition: simply do nothing for now,
		// TODO: reconnect to new controlling entity

	} catch (eIoSvcUnhandledTimer& e) {
		// ignore
	}
}





cftentry*
cfwdelem::hw_create_cftentry(
	cftentry_owner *owner,
	std::set<cftentry*> *flow_table,
	cofpacket *pack)
{
	return new cftentry(owner, flow_table, pack, this);
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
	WRITELOG(CFWD, ROFL_DBG, "cfwdelem(%p)::fibentry_timeout() %s",
			this, fibentry->c_str());

#if 0
	rib.rem_edge(fibentry->from, fibentry->to);

	rib.dijkstra();

	WRITELOG(CFWD, ROFL_DBG, "cfwdelem(%p)::fibentry_timeout() crib: \n%s",
			this, rib.c_str());
#endif
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

	WRITELOG(CFWD, ROFL_DBG, "cfwdelem(%p)::handle_stats_table_request() table_stats[%p] body: %s",
			this, table_stats, body.c_str());

	send_stats_reply(
			ofctrl,
			pack->get_xid(),
			OFPST_TABLE,
			(uint8_t*)body.somem(), body.memlen());

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

	cofstats_aggregate_request aggr(pack->body.somem(), pack->body.memlen());

#if 0
	fprintf(stderr, "cfwdelem::handle_aggregate_stats_request() "
			"table-id: %d match: %s\n",
			aggr.ofs_aggr_stats_request->table_id, pack->match.c_str());
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
						be32toh(pack->ofb_flow_stats_request->out_port),
						be32toh(pack->ofb_flow_stats_request->out_group),
						be64toh(pack->ofb_flow_stats_request->cookie),
						be64toh(pack->ofb_flow_stats_request->cookie_mask),
						pack->match);

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

		WRITELOG(CFWD, ROFL_DBG, "cfwdelem(%s)::handle_aggregate_stats_request() "
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
cfwdelem::handle_flow_mod(cofctrl *ofctrl, cofpacket *pack)
{
	cftentry *fte = NULL;

	// table_id == 255 (all tables)
	if (OFPTT_ALL == pack->ofh_flow_mod->table_id)
	{
		std::map<uint8_t, cfttable*>::iterator it;
		for (it = flow_tables.begin(); it != flow_tables.end(); ++it)
		{
			if ((fte = it->second->update_ft_entry(this, pack)) != NULL)
			{
				fte->ofctrl = ofctrl; // store controlling entity for this cftentry
				WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::flow_mod_rcvd() table_id %d new %s",
						this, pack->ofh_flow_mod->table_id, fte->c_str());
			}
		}
	}
	// single table
	else
	{
		// check for existence of specified table
		if (flow_tables.find(pack->ofh_flow_mod->table_id) == flow_tables.end())
		{
			throw eFwdElemTableNotFound();
		}

		// do not lock here flow_table[i]

		if ((fte = flow_tables[pack->ofh_flow_mod->table_id]->
						update_ft_entry(this, pack)) == NULL)
		{
			return;
		}



		fte->ofctrl = ofctrl; // store controlling entity for this cftentry
		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::flow_mod_rcvd() table_id %d new %s",
				this, pack->ofh_flow_mod->table_id, fte->c_str());


		try {
			WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::flow_mod_rcvd() new fte created: %s", this, fte->c_str());

			cofinst& inst = fte->instructions.find_inst(OFPIT_GOTO_TABLE);

			if (flow_tables.find(inst.oin_goto_table->table_id) == flow_tables.end())
			{
				throw eFwdElemGotoTableNotFound();
			}

		} catch (eInListNotFound& e) {}
	}

	if (0 != fte)
	{
		switch (pack->ofh_group_mod->command) {
		case OFPFC_ADD:
			{
				flow_mod_add(pack, flow_tables[pack->ofh_flow_mod->table_id], fte);
			}
			break;
		case OFPFC_MODIFY:
		case OFPFC_MODIFY_STRICT:
			{
				flow_mod_modify(pack, flow_tables[pack->ofh_flow_mod->table_id], fte);
			}
			break;
		case OFPFC_DELETE:
		case OFPFC_DELETE_STRICT:
			{
				flow_mod_delete(pack, flow_tables[pack->ofh_flow_mod->table_id], fte);
			}
			break;
		}
	}

	delete pack;
}



void
cfwdelem::handle_group_mod(cofctrl *ctl, cofpacket *pack)
{
	try {

		cgtentry *gte = group_table.update_gt_entry(this, pack->ofh_group_mod);

		if (0 != gte)
		{
			switch (pack->ofh_group_mod->command) {
			case OFPGC_ADD:
				{
					group_mod_add(pack, gte);
				}
				break;
			case OFPGC_MODIFY:
				{
					group_mod_modify(pack, gte);
				}
				break;
			case OFPGC_DELETE:
				{
					group_mod_delete(pack, gte);
				}
				break;
			}
		}

	} catch (eGroupTableExists& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry already exists, dropping", this);

		send_error_message(ctl, OFPET_GROUP_MOD_FAILED, OFPGMFC_GROUP_EXISTS,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableNotFound& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry not found", this);

		send_error_message(ctl, OFPET_GROUP_MOD_FAILED, OFPGMFC_UNKNOWN_GROUP,
				pack->soframe(), pack->framelen());

	} catch (eGroupEntryInval& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry is invalid", this);

		send_error_message(ctl, OFPET_GROUP_MOD_FAILED, OFPGMFC_INVALID_GROUP,
				pack->soframe(), pack->framelen());

	} catch (eGroupEntryBadType& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry with bad type", this);

		send_error_message(ctl, OFPET_GROUP_MOD_FAILED, OFPGMFC_BAD_TYPE,
				pack->soframe(), pack->framelen());

	} catch (eActionBadOutPort& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry with action with bad type", this);

		send_error_message(ctl, OFPET_BAD_ACTION, OFPBAC_BAD_OUT_PORT,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableLoopDetected& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry produces loop, dropping", this);

		send_error_message(ctl, OFPET_GROUP_MOD_FAILED, OFPGMFC_LOOP,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableModNonExisting& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry for modification not found, dropping", this);

		send_error_message(ctl, OFPET_GROUP_MOD_FAILED, OFPGMFC_UNKNOWN_GROUP,
				pack->soframe(), pack->framelen());

	}

	delete pack;
}


void
cfwdelem::handle_table_mod(cofctrl *ofctrl, cofpacket *pack)
{
	if (flow_tables.find(pack->ofh_table_mod->table_id) != flow_tables.end())
	{
		flow_tables[pack->ofh_table_mod->table_id]->set_config(
											be32toh(pack->ofh_table_mod->config));
	}
	delete pack;
}



void
cfwdelem::handle_stats_reply(cofdpath *dpt, cofpacket *pack)
{
	// extract all ofp_table_stats structures from
	switch (be16toh(pack->ofh_stats_reply->type)) {
	case OFPST_TABLE:
		{
			try
			{
				int n_tables = pack->get_datalen() / sizeof(struct ofp_table_stats);

				for (int i = 0; i < n_tables; ++i)
				{
					struct ofp_table_stats *table_stats =
							&((struct ofp_table_stats*)pack->ofh_stats_reply->body)[i];

					if (flow_tables.find(table_stats->table_id) == flow_tables.end())
					{
						flow_tables[table_stats->table_id] =
										new cfttable(0, table_stats->table_id);
					}

					flow_tables[table_stats->table_id]->set_table_stats(
										table_stats, sizeof(struct ofp_table_stats));
				}

			} catch (eOFpacketNoData& e) {}
		}
		break;
	default:
		{

		}
		break;
	}

	delete pack;
}



void
cfwdelem::handle_flow_removed(cofdpath *sw, cofpacket *pack)
{
	if (flow_tables.find(pack->ofh_flow_mod->table_id) != flow_tables.end())
	{
		flow_tables[pack->ofh_flow_mod->table_id]->update_ft_entry(this, pack);
	}
	delete pack;
}

