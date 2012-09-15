/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ctlbase.h"

ctlbase::ctlbase(
		uint64_t lldpid,
		std::string dpname,
		uint64_t dpid,
		uint8_t n_tables,
		uint32_t n_buffers,
		caddress const& rpc_ctl_addr,
		caddress const& rpc_dpt_addr) :
	cfwdelem(dpname, dpid, n_tables, n_buffers, rpc_ctl_addr, rpc_dpt_addr),
	lldpid(lldpid),
	dpath(0)
{

}


ctlbase::~ctlbase()
{
	std::map<cofbase*, cofdpath*>::iterator it;
	for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		cofmatch match; // all wildcard
		it->second->fsp_close(match);
	}

	adstacks.clear();
}


const char*
ctlbase::c_str()
{
	cvastring vas(2048);

	info.assign(vas("\nctlbase(%s): =>", dpname.c_str()));

#if 0
	std::set<cadapt*>::iterator it;
	info.append(vas("\nlist of registered cctlmod instances: =>"));
	for (it = stack.begin(); it != stack.end(); ++it)
	{
		//info.append(vas("\n  %s", (*it)->c_str()));
	}
#endif

	//info.append(vas("\n%s", cfwdelem::c_str()));

	return info.c_str();
}



void
ctlbase::stack_load(
		unsigned int stack_index,
		cadapt *adapt) throw (eCtlBaseInval)
{
	if (0 == adapt)
	{
		throw eCtlBaseInval();
	}

	std::list<cadapt*> stack;

	stack.push_back(adapt);

	stack_load(stack_index, stack);
}



void
ctlbase::stack_load(
		unsigned int stack_index,
		std::list<cadapt*>& stack) throw (eCtlBaseInval)
{
	if (stack.empty())
	{
		throw eCtlBaseInval();
	}

	if (adstacks.find(stack_index) != adstacks.end())
	{
		throw eCtlBaseInval();
	}

	adstacks[stack_index] = stack;

	dynamic_cast<cadapt_dpt*>(adstacks[stack_index].front())->bind(this);
	dynamic_cast<cadapt_ctl*>(adstacks[stack_index].back ())->bind(this);

	// make sure, that all adapters are linked to each other within stack
	for (std::list<cadapt*>::iterator
			it = stack.begin(); it != stack.end(); ++it)
	{
		cadapt_ctl* ctl = dynamic_cast<cadapt_ctl*>( *it );

		if (0 == ctl)
		{
			continue;
		}

		std::list<cadapt*>::iterator next = it;
		std::advance(next, 1);

		if (next == stack.end())
		{
			break;
		}

		cadapt_dpt* dpt = dynamic_cast<cadapt_dpt*>( *next );

		if (0 == dpt)
		{
			continue;
		}

		ctl->bind(dpt);
	}
}


void
ctlbase::stack_unload(
		unsigned int stack_index) throw (eCtlBaseInval)
{
	if (adstacks.find(stack_index) == adstacks.end())
	{
		return;
	}

	dynamic_cast<cadapt_dpt*>(adstacks[stack_index].front())->unbind(this);
	dynamic_cast<cadapt_ctl*>(adstacks[stack_index].back ())->unbind(this);

	adstacks.erase(stack_index);
}













/***********************************************************
 *
 * overloaded handler methods from cfwdelem
 *
 */

void
ctlbase::handle_dpath_open(
		cofdpath *dpath)
{
	if ((0 == dpath) || (dpath->dpid != lldpid))
	{
		return;
	}

	/* TODO: We still lack the ability to control an entire OpenFlow
	 * domain, i.e. we can only use a single data path for now.
	 * This is going to change.
	 */
	this->dpath = dpath;

	WRITELOG(CCTLMOD, DBG, "ctlbase(%s)::handle_dpath_open() "
			"dpid: %llu adapters: %d", dpname.c_str(), dpath->dpid, adstacks.size());

	/*
	 * inform adapters about existence of our layer (n-1) datapath
	 */
	for (std::map<unsigned int, std::list<cadapt*> >::iterator
			it = adstacks.begin(); it != adstacks.end(); ++it)
	{
		std::list<cadapt*>& stack = it->second;

		for (std::map<uint32_t, cofport*>::iterator
				jt = dpath->ports.begin(); jt != dpath->ports.end(); ++jt)
		{
			stack.back()->ctl_handle_port_status(this, OFPPR_ADD, jt->second);
		}
	}
}


void
ctlbase::handle_dpath_close(
		cofdpath *dpath)
{
	if ((0 == dpath) || (dpath->dpid != lldpid))
	{
		return;
	}

	WRITELOG(CCTLMOD, DBG, "ctlbase(%s)::handle_dpath_close() "
			"dpid: %llu", dpname.c_str(), dpath->dpid);

	/*
	 * inform adapters about detachment of our layer (n-1) datapath
	 */
	for (std::map<unsigned int, std::list<cadapt*> >::iterator
			it = adstacks.begin(); it != adstacks.end(); ++it)
	{
		std::list<cadapt*>& stack = it->second;

		for (std::map<uint32_t, cofport*>::iterator
				jt = dpath->ports.begin(); jt != dpath->ports.end(); ++jt)
		{
			stack.back()->ctl_handle_port_status(this, OFPPR_DELETE, jt->second);
		}
	}

	this->dpath = (cofdpath*)0;
}


/*
 * STATS-reply
 */
void
ctlbase::handle_stats_reply(
		cofdpath *sw,
		cofpacket *pack)
{

}


/*
 * ERROR
 */
void
ctlbase::handle_error(
		cofdpath *sw,
		cofpacket *pack)
{

}


/*
 * PACKET-IN
 *
 * received by layer (n-1) datapath, sending to adapters for doing the adaptation
 */
void
ctlbase::handle_packet_in(
		cofdpath *sw,
		cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "\n\n\n\nUUU\n\nctlbase(%s)::handle_packet_in() pack:%s",
			dpname.c_str(), pack->c_str());

	WRITELOG(CFWD, DBG, "\n\n\n\nVVV\n\nctlbase(%s)::handle_packet_in() fsptable:%s",
			dpname.c_str(), sw->fsptable.c_str());


	if (sw != dpath)
	{
		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() rcvd packet from non-registered dpath",
				dpname.c_str());

		delete pack; return;
	}

	try {
		// extract the payload as a new data packet
		cpacket n_pack(
					pack->get_data(),
					pack->get_datalen(),
					pack->match.get_in_port(), true /* = do classify */);


		// find all adapters whose flowspace registrations match the packet
		std::set<cfspentry*> fsp_list =
				sw->fsptable.find_matching_entries(
						pack->match.get_in_port(),
						be16toh(pack->ofh_packet_in->total_len),
						(uint8_t*)pack->get_data(), pack->get_datalen());

		// more than one subscription matches? should not happen here => error
		if (fsp_list.size() > 1)
		{
			throw eCtlBaseInval();
		}

		cfspentry* fspentry = (*(fsp_list.begin()));

		cadapt *adapt = dynamic_cast<cadapt*>( fspentry->fspowner );
		if (0 == adapt)
		{
			throw eInternalError();
		}


		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"flowspace subscription for packet found, data: %s\nctlmod: %s",
				dpname.c_str(), n_pack.c_str(), adapt->c_str());


		adapt->ctl_handle_packet_in(
				this,
				be32toh(pack->ofh_packet_in->buffer_id),
				be16toh(pack->ofh_packet_in->total_len),
				pack->ofh_packet_in->table_id,
				pack->ofh_packet_in->reason,
				pack->match,
				n_pack);


	} catch (eFspNoMatch& e) {

		cpacket tmppack(
					pack->get_data(),
					pack->get_datalen(),
					pack->match.get_in_port());

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"no flowspace subscription for packet found, data: %s",
				dpname.c_str(), tmppack.c_str());

	} catch (eCtlBaseInval& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"too many subscriptions found for packet, unspecified behaviour, pack: %s",
				dpname.c_str(), pack->c_str());

	} catch (eInternalError& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"internal error, found fspentry owner which is not of type cctlmod. WTF???, pack: %s",
				dpname.c_str(), pack->c_str());

	} catch (eFrameInvalidSyntax& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"frame with invalid syntax received, dropping. pack: %s",
				dpname.c_str(), pack->c_str());

	} catch (eOFpacketNoData& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"PACKET-IN without attached payload, dropping. pack: %s",
				dpname.c_str(), pack->c_str());
	}

	delete pack;
}



/*
 * PORT-STATUS
 */

/*
 * received by layer (n-1) datapath, sending to adapters for handling changes in port status
 */
void
ctlbase::handle_port_status(
		cofdpath *sw,
		cofpacket *pack,
		cofport *port)
{
	WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_port_status() %s", dpname.c_str(), port->c_str());

	if (sw != dpath)
	{
		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_port_status() rcvd packet from non-registered dpath",
				dpname.c_str());

		delete pack; return;
	}

	WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_port_status() "
			"%s", dpname.c_str(), port->c_str());

	for (std::map<unsigned int, std::list<cadapt*> >::iterator
			it = adstacks.begin(); it != adstacks.end(); ++it)
	{
		std::list<cadapt*>& stack = it->second;

		stack.back()->ctl_handle_port_status(
				this,
				pack->ofh_port_status->reason,
				port);
	}

	delete pack;
}









/**********************************************************
 *
 * cadapt_ctl
 *
 */



uint32_t
ctlbase::ctl_get_free_portno(
		cadapt_dpt *dpt)
			throw (eAdaptNotFound)
{
	uint32_t portno = 1;
	while (n_ports.find(portno) != n_ports.end())
	{
		portno++;
		if (portno == std::numeric_limits<uint32_t>::max())
		{
			throw eAdaptNotFound();
		}
	}

	WRITELOG(CFWD, DBG, "ctlbase(%s)::ctl_get_free_portno() "
			"assigning port-no: %d", dpname.c_str(), portno);

	return portno;
}


void
ctlbase::ctl_handle_error(
		cadapt_dpt *dpt,
		uint16_t type,
		uint16_t code,
		uint8_t *data,
		size_t datalen)
{
	// TODO: inform derived transport controller class
}


void
ctlbase::ctl_handle_port_status(
		cadapt_dpt *dpt,
		uint8_t reason,
		cofport *ofport)
{
	cadapt* adapt = dynamic_cast<cadapt*>( dpt );

	if ((0 == adapt) || (0 == ofport))
	{
		return;
	}

	WRITELOG(CFWD, DBG, "ctlbase(%s)::ctl_handle_port_status() "
			"reason: %d port: %s\n"
			"ctlbase: %s", dpname.c_str(), reason, ofport->c_str(), this->c_str());

	switch (reason) {
	case OFPPR_ADD:
		{
			/* sanity check: the port_no must not be in use currently */
			if (n_ports.find(ofport->port_no) != n_ports.end())
			{
				throw eCtlBaseExists();
			}

			n_ports[ofport->port_no] = adapt;
			adports[ofport->port_no] = ofport;
		}
		break;
	case OFPPR_DELETE:
		{
			/* sanity check: the port_no must exist */
			if (n_ports.find(ofport->port_no) == n_ports.end())
			{
				return;
			}

			if (n_ports[ofport->port_no] != adapt)
			{
				return;
			}

			n_ports.erase(ofport->port_no);
			adports.erase(ofport->port_no);
		}
		break;
	case OFPPR_MODIFY:
		{
			/* sanity check: the port_no must exist */
			if (n_ports.find(ofport->port_no) == n_ports.end())
			{
				throw eCtlBaseNotFound();
			}
		}
		break;
	default:

		break;
	}

	handle_port_status(adapt, reason, ofport); // call method from derived transport controller
}



void
ctlbase::ctl_handle_packet_in(
		cadapt_dpt *dpt,
		uint32_t buffer_id,
		uint16_t total_len,
		uint8_t table_id,
		uint8_t reason,
		cofmatch& match,
		cpacket& pack)
{
	cadapt* adapt = dynamic_cast<cadapt*>( dpt );

	if (0 == adapt)
	{
		return;
	}

	WRITELOG(CFWD, DBG, "ctlbase(%s)::send_packet_in() "
			"match: %s\npack:%s", dpname.c_str(), match.c_str(), pack.c_str());

	handle_packet_in(
			adapt,
			buffer_id,
			total_len,
			table_id,
			reason,
			match,
			pack); // let derived transport controller handle incoming packet
}


void
ctlbase::bound(
		cadapt_dpt *dpt)
{

}



void
ctlbase::unbound(
		cadapt_dpt *dpt)
{

}









/**********************************************************
 *
 * cadapt_dpt
 *
 */


/*
 * cadapt_dpt
 */
void
ctlbase::dpt_handle_packet_out(
		cadapt_ctl *ctl,
		uint32_t buffer_id,
		uint32_t in_port,
		cofaclist& actions,
		cpacket& pack)
{
	if (0 == dpath)
	{
		return;
	}

	cmemory mem(pack.length());

	pack.pack(mem.somem(), mem.memlen()); // TODO: can we get rid of this additional copying ... ?

	cfwdelem::send_packet_out_message(
						dpath,
						buffer_id,
						in_port,
						actions,
						mem.somem(), mem.memlen());
}


/*
 * cadapt_dpt
 */
void
ctlbase::dpt_handle_flow_mod(
		cadapt_ctl *ctl,
		cflowentry& fe)
{
	if (0 == dpath)
	{
		return;
	}

	cfwdelem::send_flow_mod_message(dpath, fe);
}



/*
 * cadapt_dpt
 */
void
ctlbase::dpt_handle_port_mod(
		cadapt_ctl *ctl,
		uint32_t port_no,
		uint32_t config,
		uint32_t mask,
		uint32_t advertise)
{
	// ignore this command
}



/*
 * cadapt_dpt
 */
void
ctlbase::dpt_flowspace_open(
		cadapt_ctl* ctl,
		cofmatch& match) throw (eAdaptNotConnected)
{
	if (0 == dpath)
	{
		throw eAdaptNotConnected();
	}

	try {
		cadapt *adapt = dynamic_cast<cadapt*>( ctl );

		if (0 == adapt)
		{
			return;
		}

		WRITELOG(CFWD, DBG, "ctlbase(%s)::flowspace_open() rcvd cofmatch [1] from adapter: %s:\n%s",
				dpname.c_str(), adapt->c_str(), match.c_str());

		dpath->fsptable.insert_fsp_entry(adapt, match, false /*non-strict*/);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::flowspace_open() rcvd cofmatch [2] from adapter: %s:\n%s",
				dpname.c_str(), adapt->c_str(), match.c_str());

		dpath->fsp_open(match);

	} catch (eOFbaseNotAttached& e) {

	} catch (eFspEntryOverlap& e) {

	}
}




/*
 * cadapt_dpt
 */
void
ctlbase::dpt_flowspace_close(
		cadapt_ctl* ctl,
		cofmatch& match) throw (eAdaptNotConnected)
{
	try {
		cadapt *adapt = dynamic_cast<cadapt*>( ctl );

		if (0 == adapt)
		{
			return;
		}

		dpath->fsptable.delete_fsp_entry(adapt, match, false /*non-strict*/);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::down_fsp_close() rcvd cofmatch from adapter: %s:\n%s",
				dpname.c_str(), adapt->c_str(), match.c_str());

		dpath->fsp_close(match);

	} catch (eOFbaseNotAttached& e) {

	} catch (eFspEntryNotFound& e) {

	}
}


cofaclist
ctlbase::dpt_filter_match(
		cadapt_ctl* ctl,
		uint32_t port_no,
		cofmatch& match) throw (eAdaptNotFound)
{
	cofaclist actions;
	return actions;
}


cofaclist
ctlbase::dpt_filter_action(
		cadapt_ctl* ctl,
		uint32_t port_no,
		cofaction& action) throw (eAdaptNotFound)
{
	cofaclist actions;

	WRITELOG(CFWD, DBG, "ctlbase(%s)::dpt_filter_action() "
			"action: %s", dpname.c_str(), action.c_str());

	switch (action.get_type()) {
	case OFPAT_OUTPUT:
		{
			actions.next() = cofaction_output(port_no);
		}
		break;
	}

	return actions;
}


void
ctlbase::dpt_filter_packet(
		cadapt_ctl* ctl,
		uint32_t port_no,
		cpacket& pack) throw (eAdaptNotFound)
{
	// do nothing
}


cofport*
ctlbase::dpt_find_port(
		cadapt_ctl* ctl,
		uint32_t port_no)
						throw (eAdaptNotFound)
{
	return (cofport*)0;
}


void
ctlbase::bound(
		cadapt_ctl *ctl)
{

}



void
ctlbase::unbound(
		cadapt_ctl *ctl)
{

}













/***************************************************************
 *
 * methods overloaded from cfwdelem for use by derived transport controller
 *
 */


/*
 * FLOW-MOD
 */


/*
 * received from derived transport controller
 */
void
ctlbase::send_flow_mod_message(
			cflowentry& flowentry)
{
	send_flow_mod_message(
			flowentry.match,
			flowentry.get_cookie(),
			flowentry.get_cookie_mask(),
			flowentry.get_table_id(),
			flowentry.get_command(),
			flowentry.get_idle_timeout(),
			flowentry.get_hard_timeout(),
			flowentry.get_priority(),
			flowentry.get_buffer_id(),
			flowentry.get_out_port(),
			flowentry.get_out_group(),
			flowentry.get_flags(),
			flowentry.instructions);
}


/*
 * received from derived transport controller
 */
void
ctlbase::send_flow_mod_message(
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
		cofinlist const& inlist)
{
	try {
		if (0 == dpath)
		{
			return;
		}

		/*
		 * adjust match on incoming port only
		 */
		cofmatch match(ofmatch); // new matches

		uint32_t in_port = match.get_in_port();

		if ((OFPP_CONTROLLER != in_port) && (n_ports.find(in_port) == n_ports.end()))
		{
			throw eCtlBaseInval();
		}

		WRITELOG(CFWD, DBG, "ctlbase(%s)::send_flow_mod_message() =>\n"
				"  match [original] => %s\n",
				dpname.c_str(), match.c_str());


		cofaclist match_add_this = n_ports[in_port]->dpt_filter_match(this, in_port, match);


		cofinlist instructions(inlist);
		WRITELOG(CFWD, DBG, "ctlbase(%s)::send_flow_mod_message() =>\n"
				"  match [adapted by in-port] => %s\n"
				"  actions [in-port: to be added] => %s\n"
				"  instructions [original] => %s",
				dpname.c_str(), match.c_str(), match_add_this.c_str(), instructions.c_str());



		/*
		 * iterate over all instructions and find affected adapters by their port-no
		 */
		cofinlist insts; // new instructions

		for (cofinlist::const_iterator
				it = inlist.begin(); it != inlist.end(); ++it)
		{
			cofinst inst(*it);

			switch (inst.get_type()) {
			case OFPIT_APPLY_ACTIONS:
			case OFPIT_WRITE_ACTIONS:
				{
					/*
					 * iterate over all actions in aclist and call adapt methods
					 */
					cofaclist actions(match_add_this); // list of adapted actions


					for (cofaclist::const_iterator
							it = inst.actions.begin(); it != inst.actions.end(); ++it)
					{
						cofaction action(*it);

						switch (action.get_type()) {
						case OFPAT_OUTPUT:
							{
								uint32_t out_port = be32toh(action.oac_output->port);

								if (n_ports.find(out_port) == n_ports.end())
								{
									throw eCtlBaseInval(); // outgoing port is invalid
								}

								cofaclist add_this = n_ports[out_port]->dpt_filter_action(this, out_port, action);

								// copy all adapted actions to the actions list
								for (cofaclist::const_iterator
										it = add_this.begin(); it != add_this.end(); ++it)
								{
									actions.next() = (*it);
								}
							}
							break;
						default:
							{
								actions.next() = action; // push other actions directly on the list of adapted actions
							}
							break;
						}
					}

					inst.actions = actions;
				}
				break;
			}

			insts.next() = inst;
		}


		WRITELOG(CFWD, DBG, "ctlbase(%s)::send_flow_mod_message() =>\n"
				"instructions [adapted] => %s\n",
				dpname.c_str(), insts.c_str());



		cfwdelem::send_flow_mod_message(
							dpath,
							match,
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
							insts);


	} catch (eOFmatchNotFound& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_flow_mod() "
					   "no in_port found in Flow-Mod message, ignoring",
					   dpname.c_str());

		throw eCtlBaseInval();

	}
}






/*
 * PACKET-OUT
 */


/*
 * received from derived transport controller
 * replacing cfwdelem's version of send_packet_out_message()
 */
void
ctlbase::send_packet_out_message(
		uint32_t buffer_id,
		uint32_t in_port,
		cofaclist const& aclist,
		cpacket *pack) throw (eCtlBaseInval)
{
	if (0 == dpath)
	{
		return;
	}

	/*
	 * iterate over all actions in aclist and call adapt methods
	 */
	cofaclist actions; // list of adapted actions

	cofaclist accopy(aclist);
	WRITELOG(CFWD, DBG, "ctlbase(%s)::send_packet_out_message() "
			"\nactions [original] => %s", dpname.c_str(), accopy.c_str());






	for (cofaclist::const_iterator
			it = aclist.begin(); it != aclist.end(); ++it)
	{
		cofaction action(*it);

		switch (action.get_type()) {
		case OFPAT_OUTPUT:
			{
				uint32_t out_port = be32toh(action.oac_output->port);

				if (n_ports.find(out_port) == n_ports.end())
				{
					throw eCtlBaseInval(); // outgoing port is invalid
				}

				// call the stack and filter all actions
				cofaclist add_this = n_ports[out_port]->dpt_filter_action(this, out_port, action);

				// copy all adapted actions to the actions list
				for (cofaclist::const_iterator
						it = add_this.begin(); it != add_this.end(); ++it)
				{
					actions.next() = (*it);
				}

				WRITELOG(CFWD, DBG, "ctlbase(%s)::send_packet_out_message() "
						"\nactions [add_this] => %s", dpname.c_str(), add_this.c_str());



				if (0 != pack)
				{
					// call the stack and filter the cpacket
					n_ports[out_port]->dpt_filter_packet(this, out_port, *pack);
				}

				/*
				 * TROET! fundamental problem arises here!
				 * if we have multiple ActionOutput definitions in a PACKET-OUT,
				 * adjusting the packet cannot work, think of two ActionOutputs
				 * bound to pppoe => this cannot work!
				 */

				/*
				 * suppress PACKET-OUT messages with multiple ActionOutputs???
				 */


			}
			break;
		default:
			{
				actions.next() = action; // push other actions directly on the list of adapted actions
			}
			break;
		}
	}

	WRITELOG(CFWD, DBG, "ctlbase(%s)::send_packet_out_message() "
			"\nactions [adapted] => %s", dpname.c_str(), actions.c_str());

	if (0 != pack)
	{
		WRITELOG(CFWD, DBG, "ctlbase(%s)::send_packet_out_message() "
				"\npack [adapted] => %s", dpname.c_str(), pack->c_str());
	}


	if (0 == pack)
	{
		cfwdelem::send_packet_out_message(
							dpath,
							buffer_id,
							in_port,
							actions);
	}
	else
	{
		cmemory mem(pack->length());

		pack->pack(mem.somem(), mem.memlen());

		cfwdelem::send_packet_out_message(
							dpath,
							buffer_id,
							in_port,
							actions,
							mem.somem(), mem.memlen());
	}
}











void
ctlbase::send_stats_request(
		uint16_t type,
		uint16_t flags,
		uint8_t *body,
		size_t bodylen)

{
	if (0 == dpath)
	{
		return;
	}

	switch (type) {
	case OFPST_DESC:
	case OFPST_TABLE:
	case OFPST_GROUP:
	case OFPST_GROUP_DESC:
	case OFPST_GROUP_FEATURES:
	case OFPST_EXPERIMENTER:
		{
			cfwdelem::send_stats_request(dpath, type, flags, body, bodylen);
		}
		break;
	case OFPST_FLOW:
		{
			throw eNotImplemented();
		}
		break;
	case OFPST_AGGREGATE:
		{
			throw eNotImplemented();
		}
		break;
	case OFPST_PORT:
		{
			throw eNotImplemented();
		}
		break;
	case OFPST_QUEUE:
		{
			throw eNotImplemented();
		}
		break;
	default:
		{

		}
		break;
	}
}


void
ctlbase::send_group_mod_message(
			cgroupentry& groupentry)
{
	if (0 == dpath)
	{
		return;
	}

	cofbclist buckets; // new list of adapted buckets

	for (cofbclist::const_iterator
			it = groupentry.buckets.begin(); it != groupentry.buckets.end(); ++it)
	{
		cofbucket bucket = (*it);

		/*
		 * iterate over all actions in aclist and call adapt methods
		 */
		cofaclist actions; // list of adapted actions

		for (cofaclist::const_iterator
				it = bucket.actions.begin(); it != bucket.actions.end(); ++it)
		{
			cofaction action(*it);

			switch (action.get_type()) {
			case OFPAT_OUTPUT:
				{
					uint32_t out_port = be32toh(action.oac_output->port);

					if (n_ports.find(out_port) == n_ports.end())
					{
						throw eCtlBaseInval(); // outgoing port is invalid
					}

					cofaclist add_this = n_ports[out_port]->dpt_filter_action(this, out_port, action);

					// copy all adapted actions to the actions list
					for (cofaclist::const_iterator
							it = add_this.begin(); it != add_this.end(); ++it)
					{
						actions.next() = (*it);
					}
				}
				break;
			default:
				{
					actions.next() = action; // push other actions directly on the list of adapted actions
				}
				break;
			}
		}

		bucket.actions = actions;

		buckets.next() = bucket;
	}

	groupentry.buckets = buckets; // store new list of adapted buckets in groupentry

	WRITELOG(CFWD, DBG, "ctlbase(%s)::send_group_mod_message() %s",
				   dpname.c_str(), groupentry.c_str());

	cfwdelem::send_group_mod_message(
								dpath,
								groupentry);
}


void
ctlbase::send_table_mod_message(
		uint8_t table_id,
		uint32_t config)
{
	if (0 == dpath)
	{
		return;
	}

	cfwdelem::send_table_mod_message(
								dpath,
								table_id,
								config);
}


void
ctlbase::send_port_mod_message(
		uint32_t port_no,
		cmacaddr const& hwaddr,
		uint32_t config,
		uint32_t mask,
		uint32_t advertise) throw (eCtlBaseNotFound)
{
	if (0 == dpath)
	{
		return;
	}

	if (n_ports.find(port_no) == n_ports.end())
	{
		throw eCtlBaseNotFound();
	}

	n_ports[port_no]->dpt_handle_port_mod(
			this,
			port_no,
			config,
			mask,
			advertise);

	/*
	 * TODO: thinking :) =>
	 * What is happening, if the transport controller tears down
	 * a port? This may affect adapted ports only or also physical ports
	 * from layer (n-1). When is this happening? Who is sending the
	 * Port-Mod message to the layer (n-1) datapath?
	 */
}







cadapt*
ctlbase::find_adaptor_by_portno(
		uint32_t portno) throw (eCtlBaseNotFound)
{
	if (n_ports.find(portno) == n_ports.end())
	{
		throw eCtlBaseNotFound();
	}

	return n_ports[portno];
}


