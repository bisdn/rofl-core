/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ctlbase.h"

ctlbase::ctlbase(
		std::string dpname,
		uint64_t dpid,
		uint8_t n_tables,
		uint32_t n_buffers,
		caddress const& rpc_ctl_addr,
		caddress const& rpc_dpt_addr) :
	cfwdelem(dpname, dpid, n_tables, n_buffers, rpc_ctl_addr, rpc_dpt_addr)
{

}


ctlbase::~ctlbase()
{
	while (not adapters.empty())
	{
		delete (*(adapters.begin()));
	}

	std::map<cofbase*, cofdpath*>::iterator it;
	for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		cofmatch match; // all wildcard
		it->second->fsp_close(match);
	}
}


const char*
ctlbase::c_str()
{
	cvastring vas(2048);

	info.assign(vas("\nctlbase(%s): =>", dpname.c_str()));

#if 0
	std::set<cadapter*>::iterator it;
	info.append(vas("\nlist of registered cctlmod instances: =>"));
	for (it = adapters.begin(); it != adapters.end(); ++it)
	{
		//info.append(vas("\n  %s", (*it)->c_str()));
	}
#endif

	//info.append(vas("\n%s", cfwdelem::c_str()));

	return info.c_str();
}


uint32_t
ctlbase::n_port_get_free_portno()
throw (eFwdElemNotFound)
{
	uint32_t portno = 1;
	while (n_ports.find(portno) != n_ports.end())
	{
		portno++;
		if (portno == std::numeric_limits<uint32_t>::max())
		{
			throw eFwdElemNotFound();
		}
	}
	return portno;
}


void
ctlbase::up_port_attach(
		uint32_t portno,
		cadapter *adaptor,
		cofport *ofport) throw (eCtlBaseExists)
{
	WRITELOG(CFWD, DBG, "ctlbase(%p)::up_port_attach() %s", this, c_str());

	n_ports[portno] = adaptor;

	send_port_status_message(OFPPR_ADD, ofport);
}


void
ctlbase::up_port_detach(
		uint32_t portno,
		cadapter *adapter,
		cofport *ofport) throw (eCtlBaseNotFound)
{
	WRITELOG(CFWD, DBG, "ctlbase(%p)::up_port_detach() %s", this, c_str());

	n_ports.erase(portno);

	send_port_status_message(OFPPR_DELETE, ofport);
}


void
ctlbase::up_port_modify(
			uint32_t portno,
			cadapter *adapter,
			cofport *ofport) throw (eCtlBaseNotFound)
{
	WRITELOG(CFWD, DBG, "ctlbase(%p)::up_port_modify() %s", this, c_str());

	if (n_ports.find(portno) == n_ports.end())
	{
		up_port_attach(portno, adapter, ofport);
	}
	else
	{
		send_port_status_message(OFPPR_MODIFY, ofport);
	}
}


void
ctlbase::up_packet_in(
		cadapter *ctlmod,
		uint32_t buffer_id,
		uint16_t total_len,
		uint8_t table_id,
		uint8_t reason,
		cofmatch& match,
		fframe& frame)
{
	send_packet_in_message(
			buffer_id,
			total_len,
			table_id,
			reason,
			match,
			(uint8_t*)frame.soframe(), frame.framelen());
}


void
ctlbase::up_experimenter_message(
		cadapter *ctlmod,
		uint32_t experimenter_id,
		uint32_t exp_type,
		uint8_t *body,
		size_t bodylen)
{
	std::map<cofbase*, cofctrl*>::iterator it;
	for (it = ofctrl_list.begin(); it != ofctrl_list.end(); ++it)
	{
		send_experimenter_message(it->second, experimenter_id, exp_type, body, bodylen);
	}
}


void
ctlbase::down_fsp_open(
		cadapter *ctlmod,
		cofdpath *dpath,
		const cofmatch & m)
{
	try {
		dpath->fsptable.insert_fsp_entry(ctlmod, m, false /*non-strict*/);

		cofmatch match(m);
		WRITELOG(CFWD, DBG, "ctlbase(%s)::down_fsp_open() rcvd cofmatch from cctlmod(%p):\n%s",
				dpname.c_str(), ctlmod, match.c_str());

		dpath->fsp_open(m);

	} catch (eOFbaseNotAttached& e) {

	}
}


void
ctlbase::down_fsp_close(
		cadapter *ctlmod,
		cofdpath *dpath,
		const cofmatch & m)
{
	try {

		dpath->fsptable.delete_fsp_entry(ctlmod, m, false /*non-strict*/);

		cofmatch match(m);
		WRITELOG(CFWD, DBG, "ctlbase(%s)::down_fsp_close() rcvd cofmatch from cctlmod(%p):\n%s",
				dpname.c_str(), ctlmod, match.c_str());

		dpath->fsp_close(m);

	} catch (eOFbaseNotAttached& e) {

	}
}


void
ctlbase::down_fsp_close(
		cadapter *ctlmod,
		cofdpath *dpath)
{
	try {

		dpath->fsptable.delete_fsp_entries(ctlmod);

	} catch (eOFbaseNotAttached& e) {

	}
}



void
ctlbase::handle_packet_out(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_out() "
						  "pack: %s\n"
						  "buffer-id: 0x%x in_port: 0x%x\nold-actions =>\n%s",
						  dpname.c_str(),
						  pack->c_str(),
						  be32toh(pack->ofh_packet_out->buffer_id),
						  be32toh(pack->ofh_packet_out->in_port),
						  pack->actions.c_str());
    try {
    	if (not pack->has_data())
    	{
    		cofaclist new_actions;
    		cofmatch ignore(pack->match); // not used, except inport

    		uint64_t dpid = filter_actions(pack->actions, new_actions, ignore);

    		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_out() "
    							  "buffer-id: 0x%x in_port: 0x%x\nnew-actions =>\n%s",
    							  dpname.c_str(),
    							  be32toh(pack->ofh_packet_out->buffer_id),
    							  be32toh(pack->ofh_packet_out->in_port),
    							  new_actions.c_str());

    		send_packet_out_message(
    				  &dpath_find(dpid),
    				  be32toh(pack->ofh_packet_out->buffer_id),
    				  be32toh(pack->ofh_packet_out->in_port),
    				  new_actions);
    	}
    	else
    	{
			cpacket adapted_pack(pack->get_data(), pack->get_datalen(), false);
			cofaclist new_actions; // new empty action list created and adapted from pack->actions
			cofmatch ignore(pack->match);

			uint64_t dpid = filter_actions(
					pack->actions,
					new_actions,
					ignore,
					&adapted_pack);

			WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_out() "
								  "buffer-id: 0x%x dpid:0x%x in_port: 0x%x\nnew-actions: %s\nfframe =>\n%s",
								  dpname.c_str(),
								  be32toh(pack->ofh_packet_out->buffer_id),
								  dpid,
								  be32toh(pack->ofh_packet_out->in_port),
								  new_actions.c_str(),
								  adapted_pack.c_str());

			cmemory mem(adapted_pack.length());

			adapted_pack.pack(mem.somem(), mem.memlen());

			// send to layer (N-1)
			send_packet_out_message(
					  &dpath_find(dpid),
					  be32toh(pack->ofh_packet_out->buffer_id),
					  be32toh(pack->ofh_packet_out->in_port),
					  new_actions,
					  (uint8_t*)mem.somem(), mem.memlen());
    	}

	} catch (eCtlBaseInval& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_out() "
					   "outgoing port not found, ignoring PACKET-OUT",
					   dpname.c_str());

		send_error_message(ofctrl,
			OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT,
			(uint8_t*)pack->soframe(), pack->framelen());

	} catch (eOFbaseNotAttached& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_out() "
					   "datapath element with dpid:%lu for PACKET-OUT not found",
					   dpname.c_str(),
					   dpid);

		send_error_message(ofctrl,
			OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT,
			(uint8_t*)pack->soframe(), pack->framelen());

	} catch (eNotImplemented& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_out() "
					   "internal error occured: multiple dpids issue",
					   dpname.c_str());

		send_error_message(ofctrl,
			OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT,
			(uint8_t*)pack->soframe(), pack->framelen());

	}

	delete pack;
}


void
ctlbase::handle_packet_in(
		cofdpath *sw,
		cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() pack:%s",
			dpname.c_str(), pack->c_str());

	try {
		std::set<cfspentry*> fsp_list =
				sw->fsptable.find_matching_entries(
						pack->match.get_in_port(),
						be16toh(pack->ofh_packet_in->total_len),
						(uint8_t*)pack->get_data(), pack->get_datalen());

		if (fsp_list.size() > 1)
		{
			throw eCtlBaseInval();
		}

		cfspentry& fspentry = *(*(fsp_list.begin()));

		cadapter *ctlmod = dynamic_cast<cadapter*>( fspentry.fspowner );
		if (NULL == ctlmod)
		{
			throw eInternalError();
		}

#if 1
		cpacket tmppack(
					pack->get_data(),
					pack->get_datalen(),
					pack->match.get_in_port(), false /* = do not classify */);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"flowspace subscription for packet found, data: %s\nctlmod: %s",
				dpname.c_str(), tmppack.c_str(), ctlmod->c_str());
#endif

		ctlmod->ofp_packet_in(this, sw, pack);



	} catch (eFspNoMatch& e) {

		cpacket tmppack(
					pack->get_data(),
					pack->get_datalen(),
					pack->match.get_in_port());

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"no flowspace subscription for packet found, data: %s",
				dpname.c_str(), tmppack.c_str());
		delete pack;

	} catch (eCtlBaseInval& e) {
		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"too many subscriptions found for packet, unspecified behaviour, pack: %s",
				dpname.c_str(), pack->c_str());
		delete pack;

	} catch (eInternalError& e) {
		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"internal error, found fspentry owner which is not of type cctlmod. WTF???, pack: %s",
				dpname.c_str(), pack->c_str());
		delete pack;

	} catch (eFrameInvalidSyntax& e) {
		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"frame with invalid syntax received, dropping. pack: %s",
				dpname.c_str(), pack->c_str());
		delete pack;

	} catch (eOFpacketNoData& e) {
		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"PACKET-IN without attached payload, dropping. pack: %s",
				dpname.c_str(), pack->c_str());
				delete pack;
	}
}




void
ctlbase::handle_dpath_open(
		cofdpath *sw)
{

#if 0
	cofmatch match; // all wildcard
	sw->fsp_open(match);
#endif

	// notify all ctlmodules about connecting data path
	std::set<cadapter*>::iterator it;
	for (it = adapters.begin(); it != adapters.end(); ++it)
	{
		cadapter& ctlmod = (*(*it));
		ctlmod.dpath_open(this, sw);
	}

	// TODO: RETHINK: make multiple datapath elements visible in layer (N+1)
}


void
ctlbase::handle_dpath_close(cofdpath *sw)
{
	// notify all ctlmodules about leaving data path
	std::set<cadapter*>::iterator it;
	for (it = adapters.begin(); it != adapters.end(); ++it)
	{
		cadapter& ctlmod = (*(*it));
		ctlmod.dpath_close(this, sw);
	}

	// TODO: RETHINK: make multiple datapath elements visible in layer (N+1)
}


void
ctlbase::handle_ctrl_open(cofctrl *ctrl)
{
	// notify all ctlmodules about new controller
	std::set<cadapter*>::iterator it;
	for (it = adapters.begin(); it != adapters.end(); ++it)
	{
		cadapter& ctlmod = (*(*it));
		ctlmod.ctrl_open(this, ctrl);
	}
}


void
ctlbase::handle_ctrl_close(cofctrl *ctrl)
{
	// notify all ctlmodules about leaving controller
	std::set<cadapter*>::iterator it;
	for (it = adapters.begin(); it != adapters.end(); ++it)
	{
		cadapter& ctlmod = (*(*it));
		ctlmod.ctrl_close(this, ctrl);
	}
}


void
ctlbase::handle_port_status(
		cofdpath *sw,
		cofpacket *pack,
		cofport *port)
{
	WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_port_status() %s", s_dpid.c_str(), cfwdelem::c_str());

	std::set<cadapter*>::iterator it;
	for (it = adapters.begin(); it != adapters.end(); ++it)
	{
		(*it)->ofp_port_status(this, sw,
				pack->ofh_port_status->reason, port); // notify all cctlmods about new available port
	}

	delete pack;
}


void
ctlbase::handle_flow_removed(
		cofdpath *sw,
		cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_flow_removed() %s", s_dpid.c_str(), cfwdelem::c_str());

	std::set<cadapter*>::iterator it;
	for (it = adapters.begin(); it != adapters.end(); ++it)
	{
		(*it)->ofp_flow_rmvd(this, sw, pack); // notify all cctlmods about new available port
	}

	delete pack;
}


void
ctlbase::handle_experimenter_message(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	std::map<uint32_t, cadapter*>::iterator it;
	for (it = n_ports.begin(); it != n_ports.end(); ++it)
	{
		cadapter& adapter = *(it->second);
		adapter.ofp_down_experimenter(this, ofctrl, pack);
	}
}


void
ctlbase::handle_experimenter_message(
		cofdpath *sw,
		cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_experimenter_message() pack: %s",
			dpname.c_str(),
			pack->c_str());

	std::set<cadapter*>::iterator it;
	for (it = adapters.begin(); it != adapters.end(); ++it)
	{
		cadapter& adapter = *(*it);
		adapter.ofp_up_experimenter(this, sw, pack);
	}
}


void
ctlbase::handle_flow_mod(
		cofctrl *ofctrl,
		cofpacket *pack,
		cftentry *fte)
{
	uint64_t dpid = 0;

	try {

		cofinlist insts; // new instructions

		cofmatch match(pack->match); // new match structure

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_flow_mod()\n"
				"old_match: %s\n"
				"new_match: %s", dpname.c_str(), pack->match.c_str(), match.c_str());


		/* currently, we are unable to handle out-ports on various
		 * datapath elements. All out-ports of a FLOW-MOD must be
		 * bound to physical ports of a single datapath element.
		 * The return value dpid specifies this datapath element.
		 * Throws exception eNotImplemented otherwise.
		 */
		dpid = filter_instructions(pack->instructions, insts, match);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_flow_mod() "
					"\nUUU\n\nnew_match: %s\n\nbuffer-id: 0x%x\nnew instructions list =>\n%s",
					dpname.c_str(), match.c_str(), be32toh(pack->ofh_flow_mod->buffer_id), insts.c_str());


		/* Ah!!!
		 * Hier ist die Stelle, bei der Ethernet virtual paths Tunnel zwischen
		 * zwei data path elements geschaltet werden muessen. Naemlich immer dann,
		 * wenn ein packet-out sich auf eine buffer-id bezieht, deren Speicherplatz
		 * auf einem anderen data path liegt als der ActionOutput port!
		 */

		send_flow_mod_message(
				&dpath_find(dpid),
				match,
				be64toh(pack->ofh_flow_mod->cookie),
				be64toh(pack->ofh_flow_mod->cookie_mask),
					    pack->ofh_flow_mod->table_id,
				be16toh(pack->ofh_flow_mod->command),
				be16toh(pack->ofh_flow_mod->idle_timeout),
				be16toh(pack->ofh_flow_mod->hard_timeout),
				be16toh(pack->ofh_flow_mod->priority),
				be32toh(pack->ofh_flow_mod->buffer_id),
				be32toh(pack->ofh_flow_mod->out_port),
				be32toh(pack->ofh_flow_mod->out_group),
				be16toh(pack->ofh_flow_mod->flags),
				insts);

	} catch (eCtlBaseInval& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_flow_mod() "
					   "outgoing port not found, ignoring FLOW-MOD",
					   dpname.c_str());

		send_error_message(ofctrl,
			OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT,
			(uint8_t*)pack->soframe(), pack->framelen());

	} catch (eNotImplemented &e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_flow_mod() "
					   "internal error occured: multiple dpids issue",
					   dpname.c_str());

		send_error_message(ofctrl,
			OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT,
			(uint8_t*)pack->soframe(), pack->framelen());

	} catch (eOFbaseNotAttached& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_flow_mod() "
					   "datapath element with dpid:%lu for FLOW-MOD not found",
					   dpname.c_str(), dpid);

		send_error_message(ofctrl,
			OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT,
			(uint8_t*)pack->soframe(), pack->framelen());

	}

	delete pack;
}


void
ctlbase::handle_group_mod(
		cofctrl *ofctrl,
		cofpacket *pack,
		cgtentry *gte)
{
	uint64_t dpid = 0;

	try {
		cgroupentry ge;
		ge.set_command(be16toh(pack->ofh_group_mod->command));
		ge.set_group_id(be32toh(pack->ofh_group_mod->group_id));
		ge.set_type(pack->ofh_group_mod->type);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_group_mod()", dpname.c_str());

		dpid = filter_buckets(pack->buckets, ge.buckets);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_group_mod() "
				"old-buckets: %s\n"
				"new-buckets: %s\n",
				dpname.c_str(),
				pack->buckets.c_str(),
				ge.buckets.c_str());

		send_group_mod_message(
				&dpath_find(dpid),
				ge);

	} catch (eCtlBaseInval& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_group_mod() "
					   "outgoing port not found, ignoring GROUP-MOD",
					   dpname.c_str());

		send_error_message(ofctrl,
			OFPET_BAD_ACTION, OFPBAC_BAD_ARGUMENT,
			(uint8_t*)pack->soframe(), pack->framelen());

	} catch (eNotImplemented &e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_group_mod() "
					   "internal error occured: multiple dpids issue",
					   dpname.c_str());

		send_error_message(ofctrl,
			OFPET_FLOW_MOD_FAILED, OFPFMFC_UNKNOWN,
			(uint8_t*)pack->soframe(), pack->framelen());

	} catch (eOFbaseNotAttached& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_group_mod() "
					   "datapath element with dpid:%lu for GROUP-MOD not found",
					   dpname.c_str(), dpid);

		send_error_message(ofctrl,
			OFPET_FLOW_MOD_FAILED, OFPFMFC_UNKNOWN,
			(uint8_t*)pack->soframe(), pack->framelen());

	}

	delete pack;
}


void
ctlbase::handle_table_mod(
		cofctrl *ofctrl,
		cofpacket *pack)
{
	throw eNotImplemented();
}



cadapter*
ctlbase::find_adaptor_by_portno(
		uint32_t portno) throw (eCtlBaseNotFound)
{
	if (n_ports.find(portno) == n_ports.end())
	{
		throw eCtlBaseNotFound();
	}

	return n_ports[portno];
}


uint64_t
ctlbase::filter_instructions(
		cofinlist &old_insts,
		cofinlist &new_insts,
		cofmatch &match)
{
	uint64_t dpid = 0;

	cofinlist::iterator it;
	for (it = old_insts.begin(); it != old_insts.end(); ++it)
	{
		cofinst& inst = (*it);

		new_insts.next() = inst;

		switch (be16toh(inst.oin_header->type)) {
		case OFPIT_APPLY_ACTIONS:
		case OFPIT_WRITE_ACTIONS:
			{
				cpacket ignore((size_t)0); // no data packet within FlowMods
				cofaclist actions;

				// if in-port is wildcard, we have to create individual FlowMods for each in_port
				uint32_t in_port = match.get_in_port();
				if ((0 == in_port) || (OFPP_CONTROLLER == in_port))
				{
					throw eNotImplemented(); // TODO: we have to create individual Flow-Mods in this situation
				}
				else
				{
					dpid = filter_actions(inst.actions, actions, match);
				}
				new_insts.back().actions = actions;
			}
			break;
		}
	}

	return dpid;
}


uint64_t
ctlbase::filter_buckets(
		cofbclist &old_buckets,
		cofbclist &new_buckets)
{
	uint64_t dpid = 0;

	cofbclist::iterator it;
	for (it = old_buckets.begin(); it != old_buckets.end(); ++it)
	{
		cofbucket& bucket = (*it);

		cofbucket new_bucket(bucket);

		cofmatch ignore;
		dpid = filter_actions(bucket.actions, new_bucket.actions, ignore);

		new_buckets.next() = new_bucket;
	}

	return dpid;
}


uint64_t
ctlbase::filter_actions(
		cofaclist &old_actions,
		cofaclist &new_actions,
		cofmatch &match,
		cpacket *pack) throw (eCtlBaseInval, eNotImplemented)
{
	uint64_t dpid = 0;

	WRITELOG(CFWD, DBG, "ctlbase(%s)::filter_actions()\n"
					"old-match: %s\nold-actions: %s",
					dpname.c_str(),
					match.c_str(),
					old_actions.c_str());

	if ((0 != match.get_in_port()) && (OFPP_CONTROLLER != match.get_in_port()))
	{
		if (n_ports.find(match.get_in_port()) == n_ports.end())
		{
				WRITELOG(CFWD, DBG, "ctlbase(%s)::filter_actions() "
								"portno:0x%x not found, ignoring command",
								dpname.c_str(), match.get_in_port());

				throw eCtlBaseInval();
		}

		cadapter *adaptor = dynamic_cast<cadapter*>( n_ports[match.get_in_port()] );
		if (0 == adaptor)
		{
				WRITELOG(CFWD, DBG, "ctlbase(%s)::filter_actions() "
								"oops, internal error: non ctlmod instance in phy_ports, ignoring PACKET-OUT/FLOW-MOD",
								dpname.c_str());

				throw eCtlBaseInval();
		}

		dpid = adaptor->filter_inport_match(
				this,
				match.get_in_port(),
				match,
				new_actions,
				pack); // returns immediately with updated actions and frame
	}


	// search all ActionOutput instances
	cofaclist::iterator it;
	for (it = old_actions.begin(); it != old_actions.end(); ++it)
	{
	   cofaction& action = (*it);
	   switch (action.get_type()) {

	   // adapt ActionOutput actions
	   case OFPAT_OUTPUT:
		   {
			   uint32_t portno = be32toh(action.oac_output->port);
			   if (n_ports.find(portno) == n_ports.end())
			   {
					   WRITELOG(CFWD, DBG, "ctlbase(%s)::filter_actions() "
									   "portno:%d not found, ignoring PACKET-OUT",
									   dpname.c_str(), portno);

					   throw eCtlBaseInval();
			   }

			   cadapter *adaptor = dynamic_cast<cadapter*>( n_ports[portno] );
			   if (0 == adaptor)
			   {
					   WRITELOG(CFWD, DBG, "ctlbase(%s)::filter_actions() "
									   "oops, internal error: non ctlmod instance in phy_ports, ignoring PACKET-OUT",
									   dpname.c_str());

					   throw eCtlBaseInval();
			   }

			  uint64_t tmp_dpid = adaptor->filter_outport_actions(
					   this,
					   portno,
					   new_actions,
					   pack); // returns immediately with updated actions and frame

			  if (0 == dpid)
			  {
				  dpid = tmp_dpid;
			  }
			  else if (dpid != tmp_dpid)
			  {
				  throw eCtlBaseInval();
			  }
		   }
		   break;

	   // copy all remaining actions directly into new actions list
	   default:
		  {
			  new_actions.next() = action; // copy non ActionOutput/ActionGroup action unaltered to new actions list
		  }
		  break;
	   }
   }

	WRITELOG(CFWD, DBG, "ctlbase(%s)::filter_actions()\n"
					"new-match: %s\nnew-actions: %s",
					dpname.c_str(),
					match.c_str(),
					new_actions.c_str());

	return dpid;
}


