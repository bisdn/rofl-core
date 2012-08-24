/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofdpath.h"

cofdpath::cofdpath(
		cfwdelem *fwdelem,
		cofbase *entity,
		std::map<cofbase*, cofdpath*>* ofswitch_list,
		uint64_t dpid,
		uint32_t n_buffers,
		uint8_t n_tables,
		uint32_t capabilities,
		uint32_t actions,
		struct ofp_port *__ports, int num_ports) :
	fwdelem(fwdelem),
	entity(entity),
	ofswitch_list(ofswitch_list),
	dpid(dpid),
	dpmac(OFP_ETH_ALEN),
	n_buffers(n_buffers),
	n_tables(n_tables),
	capabilities(capabilities),
	actions(actions),
	flags(0),
	miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
	features_reply_timeout(DEFAULT_DP_FEATURES_REPLY_TIMEOUT),
	get_config_reply_timeout(DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT),
	stats_reply_timeout(DEFAULT_DP_STATS_REPLY_TIMEOUT),
	get_fsp_reply_timeout(DEFAULT_DP_GET_FSP_REPLY_TIMEOUT),
	barrier_reply_timeout(DEFAULT_DB_BARRIER_REPLY_TIMEOUT)
{
	if ((struct ofp_port*)0 != __ports)
	{
		for (int i = 0; i < num_ports; i++)
		{
			new cofport(&ports, &__ports[i]);
		}
	}

	// set initial state
	init_state(DP_STATE_INIT);

	// trigger sending of FEATURES request
	register_timer(COFDPATH_TIMER_FEATURES_REQUEST, 1);

	(*ofswitch_list)[entity] = this;

	//lldp_init();

	WRITELOG(COFDPATH, DBG, "cofdpath(%p)::cofdpath() "
			"dpid:%"UINT64DBGFMT" child:%p",
			this, dpid, entity);
}


cofdpath::~cofdpath()
{
	WRITELOG(COFDPATH, DBG, "cofdpath(%p)::~cofdpath() "
			"dpid:%"UINT64DBGFMT" child:%p",
			this, dpid, entity);

	fwdelem->handle_dpath_close(this);

	//lldp_terminate();

	ofswitch_list->erase(entity);

	// remove all cofport instances
	while (not ports.empty())
	{
		delete (ports.begin()->second);
	}
}



void
cofdpath::handle_timeout(int opaque)
{
	switch (opaque) {
	case COFDPATH_TIMER_FEATURES_REQUEST:
		{
			WRITELOG(COFDPATH, DBG, "cofdpath(%p): sending -FEATURES-REQUEST-", this);
			fwdelem->send_features_request(this);
		}
		break;
	case COFDPATH_TIMER_FEATURES_REPLY:
		{
			handle_features_reply_timeout();
		}
		break;
	case COFDPATH_TIMER_GET_CONFIG_REQUEST:
		{
			WRITELOG(COFDPATH, DBG, "cofdpath(%p): sending -GET-CONFIG-REQUEST-", this);
			fwdelem->send_get_config_request(this);
		}
		break;
	case COFDPATH_TIMER_GET_CONFIG_REPLY:
		{
			handle_get_config_reply_timeout();
		}
		break;
	case COFDPATH_TIMER_STATS_REQUEST:
		{
			WRITELOG(COFDPATH, DBG, "cofdpath(%p): sending -STATS-REQUEST-", this);
			fwdelem->send_stats_request(this, OFPST_TABLE, 0);
		}
		break;
	case COFDPATH_TIMER_STATS_REPLY:
		{
			handle_stats_reply_timeout();
		}
		break;
	case COFDPATH_TIMER_GET_FSP_REQUEST:
		{
			WRITELOG(COFDPATH, DBG, "cofdpath(%p): sending -GET-FSP-REQUEST-", this);
			fwdelem->send_experimenter_ext_rofl_nsp_get_fsp_request(this);
		}
		break;
	case COFDPATH_TIMER_GET_FSP_REPLY:
		{
			handle_get_fsp_reply_timeout();
		}
		break;
	case COFDPATH_TIMER_BARRIER_REQUEST:
		{
			WRITELOG(COFDPATH, DBG, "cofdpath(%p): sending -BARRIER-REQUEST-", this);
			fwdelem->send_barrier_request(this);
		}
		break;
	case COFDPATH_TIMER_BARRIER_REPLY:
		{
			handle_barrier_reply_timeout();
		}
		break;
	default:
		WRITELOG(COFDPATH, DBG, "unknown timer event %d", opaque);
		ciosrv::handle_timeout(opaque);
		break;
	}


}


void
cofdpath::features_request_sent()
{
	register_timer(COFDPATH_TIMER_FEATURES_REPLY, features_reply_timeout /* seconds */);
}


void
cofdpath::features_reply_rcvd(
		cofpacket *pack)
{
	try {
		cancel_timer(COFDPATH_TIMER_FEATURES_REPLY);

		/* check for existing cofdpath controlling this associated dpid
		 * we assume that a duplicated dpid is caused by loss of and a
		 * reconnected TCP connection, as we cannot determine whether a
		 * second connection attempt is some kind of DoS attack
		 */
		try {
			uint64_t dpid = be64toh(pack->ofh_switch_features->datapath_id);

			cofdpath *dpath = &(fwdelem->dpath_find(dpid));

			delete dpath; // clean-up and calls dpath's destructor

		} catch (eOFbaseNotAttached& e) {}

		WRITELOG(COFDPATH, DBG, "cofdpath(%p)::features_reply_rcvd() "
				"dpid:%"UINT64DBGFMT" ",
				this, dpid);


		features_save(pack); // save struct ofp_switch_features


		if (DP_STATE_INIT == cur_state())
		{
			// next step: send GET-CONFIG request to datapath
			register_timer(COFDPATH_TIMER_GET_CONFIG_REQUEST, 0);

			//lldp_init();
		}


	} catch (eOFportMalformed& e) {

		WRITELOG(COFDPATH, DBG, "exception: malformed FEATURES reply received");

		fwdelem->send_down_hello_message(this, true /*bye*/);

		delete this;

	}
}


void
cofdpath::handle_features_reply_timeout()
{
	WRITELOG(COFDPATH, DBG, "cofdpath(%p)::handle_features_reply_timeout() ", this);

	fwdelem->handle_features_reply_timeout(this);

	delete this;
}


void
cofdpath::get_config_request_sent()
{
	register_timer(COFDPATH_TIMER_GET_CONFIG_REPLY, get_config_reply_timeout);
}


void
cofdpath::get_config_reply_rcvd(
		cofpacket *pack)
{
	cancel_timer(COFDPATH_TIMER_GET_CONFIG_REPLY);

	config_save(pack); // save struct ofp_switch_config

	fwdelem->handle_get_config_reply(this, pack);

	if (cur_state() == DP_STATE_INIT)
	{
		// send stats request during initialization
		register_timer(COFDPATH_TIMER_STATS_REQUEST, 0);
	}
}


void
cofdpath::handle_get_config_reply_timeout()
{
	WRITELOG(COFDPATH, DBG, "cofdpath(%p)::handle_get_config_reply_timeout() "
			"dpid:%"UINT64DBGFMT" ",
			this, dpid);

	fwdelem->handle_get_config_reply_timeout(this);

	delete this;
}


void
cofdpath::stats_request_sent()
{
	register_timer(COFDPATH_TIMER_STATS_REPLY, stats_reply_timeout);
}


void
cofdpath::stats_reply_rcvd(
		cofpacket *pack)
{
	cancel_timer(COFDPATH_TIMER_STATS_REPLY);

	// extract all ofp_table_stats structures from
	if (OFPST_TABLE == be16toh(pack->ofh_stats_reply->type))
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

	if (cur_state() == DP_STATE_INIT) // enter state running during initialization
	{
		flow_mod_reset();

		group_mod_reset();

		new_state(DP_STATE_RUNNING);

		//lldp_emulated_ports(); // skip this for now, we move that to a derived controller at some point in the future

		fwdelem->handle_dpath_open(this);
	}
}


void
cofdpath::handle_stats_reply_timeout()
{
	WRITELOG(COFDPATH, DBG, "cofdpath(%p)::handle_stats_reply_timeout() "
			"dpid:%"UINT64DBGFMT" ",
			this, dpid);

	fwdelem->handle_stats_reply_timeout(this);

	delete this;
}


void
cofdpath::get_fsp_request_sent()
{
	// do nothing for now
}


void
cofdpath::get_fsp_reply_rcvd(
		cofpacket *pack)
{
	// do nothing for now
}


void
cofdpath::handle_get_fsp_reply_timeout()
{
	fwdelem->handle_get_fsp_reply_timeout(this);
}


void
cofdpath::barrier_request_sent()
{
	register_timer(COFDPATH_TIMER_BARRIER_REPLY, barrier_reply_timeout);
}


void
cofdpath::barrier_reply_rcvd()
{
	cancel_timer(COFDPATH_TIMER_BARRIER_REPLY);
}


void
cofdpath::handle_barrier_reply_timeout()
{
	fwdelem->handle_barrier_reply_timeout(this);
}


void
cofdpath::flow_mod_sent(
		cofpacket *pack) throw (eOFdpathNotFound)
{
	try {
		WRITELOG(COFDPATH, DBG, "cofdpath(%p)::flow_mod_sent() table_id: %d", this, pack->ofh_flow_mod->table_id);

		if (0xff == pack->ofh_flow_mod->table_id)
		{
			std::map<uint8_t, cfttable*>::iterator it;
			for (it = flow_tables.begin(); it != flow_tables.end(); ++it)
			{
				it->second->update_ft_entry(this, pack);
			}
		}
		else
		{
			if (flow_tables.find(pack->ofh_flow_mod->table_id) == flow_tables.end())
			{
				throw eOFdpathNotFound();
			}

			// check for existence of flow_table with id pack->ofh_flow_mod->table_id first?
			flow_tables[pack->ofh_flow_mod->table_id]->update_ft_entry(this, pack);

			WRITELOG(COFDPATH, DBG, "cofdpath(%p)::flow_mod_sent() table-id:%d flow-table: %s",
					this, pack->ofh_flow_mod->table_id, flow_tables[pack->ofh_flow_mod->table_id]->c_str());
		}

	} catch (cerror& e) {
		WRITELOG(CFTTABLE, DBG, "unable to add ftentry to local flow_table instance");
	}
}


void
cofdpath::flow_removed_rcvd(
		cofpacket *pack)
{
	try {
		// check for existence of flow_table with id pack->ofh_flow_mod->table_id first?
		flow_tables[pack->ofh_flow_mod->table_id]->update_ft_entry(this, pack);

	} catch (cerror& e) {
		WRITELOG(CFTTABLE, DBG, "unable to remove ftentry from local flow_table instance");

	}
}


void
cofdpath::flow_mod_reset()
{
	cflowentry fe;
	fe.set_command(OFPFC_DELETE);
	fe.set_table_id(OFPTT_ALL /*all tables*/);

	fwdelem->send_flow_mod_message(this, fe); // calls this->flow_mod_sent() implicitly
}


void
cofdpath::group_mod_sent(
		cofpacket *pack)
{
	try {

		grp_table.update_gt_entry(this, pack->ofh_group_mod);

	} catch (cerror& e) {
		WRITELOG(CFTTABLE, DBG, "unable to handle gtentry within local grp_table instance");

	}
}


void
cofdpath::group_mod_reset()
{
	cgroupentry ge;
	ge.set_command(OFPGC_DELETE);
	ge.set_group_id(OFPG_ALL);

	fwdelem->send_group_mod_message(this, ge); // calls this->group_mod_sent() implicitly
}


void
cofdpath::table_mod_sent(cofpacket *pack)
{


}


void
cofdpath::packet_in_rcvd(cofpacket *pack)
{
	WRITELOG(COFDPATH, DBG, "cofdpath(0x%llx)::packet_in_rcvd() %s", dpid, pack->c_str());

	// update forwarding table
	uint32_t in_port = pack->match.get_in_port();

	fetherframe ether(pack->get_data(), pack->get_datalen(),
			be16toh(pack->ofh_packet_in->total_len));

	// datalen must be at least one Ethernet header in size
	if (pack->get_datalen() >= (2 * OFP_ETH_ALEN + sizeof(uint16_t)))
	{

		// update local forwarding table
		fwdtable.mac_learning(ether, dpid, in_port);

		WRITELOG(COFDPATH, DBG, "cofdpath(0x%llx)::packet_in_rcvd() local fwdtable: %s",
				dpid, fwdtable.c_str());

#if 0
		fwdelem->fwdtable.mac_learning(ether, dpid, in_port);
#endif

		WRITELOG(COFDPATH, DBG, "cofdpath(0x%llx)::packet_in_rcvd() global fwdtable: %s",
				dpid, fwdelem->fwdtable.c_str());

		// let derived class handle PACKET-IN event
		fwdelem->handle_packet_in(this, pack);
	}
}


void
cofdpath::port_status_rcvd(cofpacket *pack)
{
	WRITELOG(COFDPATH, DBG, "cfwdelem(%s)::cofdpath(0x%016llx)::port_status_rcvd() %s",
			fwdelem->get_s_dpid(), dpid, pack->c_str());

	std::map<uint32_t, cofport*>::iterator it;
	switch (pack->ofh_port_status->reason) {
	case OFPPR_ADD:
		if (ports.find(be32toh(pack->ofh_port_status->desc.port_no)) == ports.end())
		{
			cofport *lport = new cofport(&ports, &(pack->ofh_port_status->desc));

			// let derived class handle PORT-STATUS message
			fwdelem->handle_port_status(this, pack, lport);
		}
		break;
	case OFPPR_DELETE:
		if (ports.find(be32toh(pack->ofh_port_status->desc.port_no)) != ports.end())
		{
			// let derived class handle PORT-STATUS message
			fwdelem->handle_port_status(this, pack, ports[be32toh(pack->ofh_port_status->desc.port_no)]);

			delete ports[be32toh(pack->ofh_port_status->desc.port_no)];

			ports.erase(be32toh(pack->ofh_port_status->desc.port_no));
		}
		break;
	case OFPPR_MODIFY:
		if (ports.find(be32toh(pack->ofh_port_status->desc.port_no)) != ports.end())
		{
			ports[be32toh(pack->ofh_port_status->desc.port_no)]->unpack(
																&(pack->ofh_port_status->desc),
																sizeof(struct ofp_port));

			// let derived class handle PORT-STATUS message
			fwdelem->handle_port_status(this, pack, ports[be32toh(pack->ofh_port_status->desc.port_no)]);
		}
		break;
	}
}


void
cofdpath::fsp_open(cofmatch const& ofmatch)
{
	// future work: store ofmatch in std::set<cofmatch*> ???
	fwdelem->send_experimenter_ext_rofl_nsp_open_request(this, ofmatch);
}


void
cofdpath::fsp_close(cofmatch const& ofmatch)
{
	// future work: remove ofmatch from std::set<cofmatch*> ???
	fwdelem->send_experimenter_ext_rofl_nsp_close_request(this, ofmatch);
}


void
cofdpath::experimenter_message_rcvd(cofpacket *pack)
{
	switch (be32toh(pack->ofh_experimenter->experimenter)) {
	case VENDOR_EXT_ROFL:
	{
		switch (be32toh(pack->ofh_experimenter->exp_type)) {
		case OFPRET_PORT_EXTD_CONFIG_REQUEST:
		case OFPRET_PORT_EXTD_CONFIG_REPLY:
		case OFPRET_NSP_OPEN_REQUEST:
		case OFPRET_NSP_OPEN_REPLY:
		case OFPRET_NSP_CLOSE_REQUEST:
		case OFPRET_NSP_CLOSE_REPLY:
		case OFPRET_NSP_IOCTL_REQUEST:
		case OFPRET_NSP_IOCTL_REPLY:
		default:
			break;
		}

		// maybe we will do something here in the future

		break;
	}
	}

	// for now: send vendor extensions directly to class derived from cfwdelem
	fwdelem->handle_experimenter_message(this, pack);
}


void
cofdpath::role_reply_rcvd(cofpacket *pack)
{
	fwdelem->handle_role_reply(this, pack);
}


/** notification for ftentry hard timeout
 */
void
cofdpath::ftentry_timeout(cftentry *entry, uint16_t timeout)
{
	// notify derived class from cfwdelem here?
}


/** notification for gtentry removal
 */
void
cofdpath::gtentry_timeout(cgtentry *entry, uint16_t timeout)
{
	// notify derived class from cfwdelem here?
}


const char*
cofdpath::c_str()
{
	cvastring vas;
	info.assign(vas("cofdpath(%p) dpid:0x%llx =>", this, dpid));

	std::map<uint32_t, cofport*>::iterator it;
	for (it = ports.begin(); it != ports.end(); ++it)
	{
		info.append(vas("\n  %s", it->second->c_str()));
	}

	return info.c_str();
}



cofport*
cofdpath::find_cofport(
	uint32_t port_no) throw (eOFdpathNotFound)
{
	std::map<uint32_t, cofport*>::iterator it;
	if (ports.find(port_no) == ports.end())
	{
		throw eOFdpathNotFound();
	}
	return ports[port_no];

#if 0
	for (it = ports.begin(); it != ports.end(); ++it) {
		if ((*it)->port_no == port_no)
			return (*it);
	}
	throw eOFdpathNotFound();
#endif
}


cofport*
cofdpath::find_cofport(
	std::string port_name) throw (eOFdpathNotFound)
{

	std::map<uint32_t, cofport*>::iterator it;
	if ((it = find_if(ports.begin(), ports.end(),
			cofport_find_by_port_name(port_name))) == ports.end())
	{
		throw eOFdpathNotFound();
	}
	return it->second;

#if 0
	std::set<cofport*>::iterator it;
	for (it = ports.begin(); it != ports.end(); ++it) {
		std::string itport_name((*it)->name);
		if (itport_name == port_name)
			return (*it);
	}
	throw eOFdpathNotFound();
#endif
}


void
cofdpath::features_save(
	cofpacket* pack)
{

	dpid = be64toh(pack->ofh_switch_features->datapath_id);
	n_buffers = be32toh(pack->ofh_switch_features->n_buffers);
	n_tables = pack->ofh_switch_features->n_tables;
	capabilities = be32toh(pack->ofh_switch_features->capabilities);

	int portslen = be16toh(pack->ofh_switch_features->header.length) -
											sizeof(struct ofp_switch_features);

	WRITELOG(COFDPATH, DBG, "cofdpath(%p)::features_save() %s", this, this->c_str());

	ports = cofport::ports_parse(pack->ofh_switch_features->ports, portslen);

#ifndef NDEBUG
	std::map<uint32_t, cofport*>::iterator it;
	for (it = ports.begin(); it != ports.end(); ++it)
	{
		WRITELOG(COFDPATH, DBG, "cofdpath(%p)::features_save() XXX => %s", this, it->second->c_str());
	}
#endif

	// dpid as std::string
	cvastring vas;
	s_dpid = std::string(vas("0x%llx", dpid));

	// lower 48bits from dpid as datapath mac address
	dpmac[0] = (dpid & 0x0000ff0000000000ULL) >> 40;
	dpmac[1] = (dpid & 0x000000ff00000000ULL) >> 32;
	dpmac[2] = (dpid & 0x00000000ff000000ULL) >> 24;
	dpmac[3] = (dpid & 0x0000000000ff0000ULL) >> 16;
	dpmac[4] = (dpid & 0x000000000000ff00ULL) >>  8;
	dpmac[5] = (dpid & 0x00000000000000ffULL) >>  0;
	dpmac[0] &= 0xfc;

	WRITELOG(COFDPATH, DBG, "cofdpath(%p)::features_save() dpid=%llu s_dpid=%s dl_dpid=%s",
			this, dpid, s_dpid.c_str(), dpmac.c_str());
}


void
cofdpath::config_save(
	cofpacket* pack)
{
	flags = be16toh(pack->ofh_switch_config->flags);
	miss_send_len = be16toh(pack->ofh_switch_config->miss_send_len);
}


