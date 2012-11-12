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
	std::set<cofdpt*>::iterator it;
	for (it = ofdpt_set.begin(); it != ofdpt_set.end(); ++it)
	{
		cofmatch match; // all wildcard
		(*it)->fsp_close(match);
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

	if (0 != dpath)
	{
		for (std::map<uint32_t, cofport*>::iterator
				it = dpath->ports.begin(); it != dpath->ports.end(); ++it)
		{
			stack.back()->ctl_handle_port_status(this, OFPPR_ADD, it->second);
		}
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
		cofdpt *dpath)
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

	WRITELOG(CCTLMOD, DBG, "ctlbase(%s)::handle_dpath_open() => "
			"adapters: %d #ports-on-dpt: %d\ndpath: %s ",
			dpname.c_str(), adstacks.size(), dpath->ports.size(), dpath->c_str());

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
		cofdpt *dpath)
{
	if ((0 == dpath) || (dpath->dpid != lldpid))
	{
		return;
	}

	WRITELOG(CCTLMOD, DBG, "ctlbase(%s)::handle_dpath_close() "
			"dpid: %llu", dpname.c_str(), dpath->dpid);

	this->dpath = (cofdpt*)0;

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
}



/*
 * STATS-reply
 */
void
ctlbase::handle_stats_reply(
		cofdpt *sw,
		cofpacket *pack)
{
	uint32_t xid = be32toh(pack->ofh_header->xid);
	try {

		WRITELOG(CCTLMOD, DBG, "ctlbase(%s)::handle_stats_reply() ", dpname.c_str());

		if (xidstore.xid_find(xid).owner == this)
		{
			handle_stats_reply(this, xid, be16toh(pack->ofh_stats_reply->type),
									pack->body.somem(), pack->body.memlen());
		}
		else
		{
			call_adapter(dynamic_cast<cadapt*>( xidstore.xid_find(xid).owner ))->
					ctl_handle_stats_reply(this, xid, be16toh(pack->ofh_stats_reply->type),
									pack->body.somem(), pack->body.memlen());
		}

		xidstore.xid_rem(xid); // remove xid from store

	} catch (eXidStoreNotFound& e) {

		// xid was not found in xidstore
	} catch (eCtlBaseNotFound& e) {

		// adapter was not found (deleted in the mean time??)
	}
}



void
ctlbase::handle_stats_reply_timeout(
		cofdpt *sw,
		uint32_t xid)
{
	if (xidstore.xid_find(xid).owner == this)
	{
		handle_stats_reply_timeout(this, xid);
	}
	else
	{
		// call adapter here?
	}

	xidstore.xid_rem(xid); // remove xid from store
}



void
ctlbase::handle_barrier_reply(
		cofdpt *sw,
		cofpacket *pack)
{
	uint32_t xid = be32toh(pack->ofh_header->xid);
	try {
#if 0
		call_adapter(dynamic_cast<cadapt*>( xidstore.xid_find(xid).owner ))->
				ctl_handle_barrier_reply(this, xid);
#endif

		xidstore.xid_rem(xid); // remove xid from store

	} catch (eXidStoreNotFound& e) {

		// xid was not found in xidstore
	} catch (eCtlBaseNotFound& e) {

		// adapter was not found (deleted in the mean time??)
	}
}



void
ctlbase::handle_barrier_reply_timeout(
		cofdpt *sw,
		uint32_t xid)
{
	// call adapter here?

	xidstore.xid_rem(xid); // remove xid from store
}



/*
 * ERROR
 */
void
ctlbase::handle_error(
		cofdpt *sw,
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
		cofdpt *sw,
		cofpacket *pack)
{
	WRITELOG(CFWD, DBG, "\n\n\nUUU\n\n\nctlbase(%s)::handle_packet_in() pack:%s",
			dpname.c_str(), pack->c_str());



	if (sw != dpath)
	{
		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() rcvd packet from non-registered dpath"
				"sw: %p dpath: %p", dpname.c_str(), sw, dpath);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() rcvd packet from non-registered dpath",
				dpname.c_str());

		delete pack; return;
	}

	try {

		// find all adapters whose flowspace registrations match the packet
		std::set<cfspentry*> fsp_list =
				sw->fsptable.find_matching_entries(
						pack->match.get_in_port(),
						be16toh(pack->ofh_packet_in->total_len),
						pack->packet);

		// more than one subscription matches? should not happen here => error
		if (fsp_list.size() > 1)
		{
			WRITELOG(CFWD, DBG, "\n\n\n\nVVV\n\nctlbase(%s)::handle_packet_in() "
					"too many results from fsptable:%s",
					dpname.c_str(), sw->fsptable.c_str());

			throw eCtlBaseInval();
		}

		cfspentry* fspentry = (*(fsp_list.begin()));

		cadapt *adapt = dynamic_cast<cadapt*>( fspentry->fspowner );
		if (0 == adapt)
		{
			throw eInternalError();
		}

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"flowspace subscription for packet found => "
				"fspentry: %s\ndata: %s\nctlmod: %s",
				dpname.c_str(),
				fspentry->c_str(),
				pack->packet.c_str(),
				adapt->c_str());


		adapt->ctl_handle_packet_in(
				this,
				be32toh(pack->ofh_packet_in->buffer_id),
				be16toh(pack->ofh_packet_in->total_len),
				pack->ofh_packet_in->table_id,
				pack->ofh_packet_in->reason,
				pack->match,
				pack->packet);


	} catch (eFspNoMatch& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"no flowspace subscription for packet found =>"
				" fsptable: %s\ndata: %s",
				dpname.c_str(), fsptable.c_str(), pack->packet.c_str());

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

		throw;

	} catch (eOFpacketNoData& e) {

		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_packet_in() "
				"PACKET-IN without attached payload, dropping. pack: %s",
				dpname.c_str(), pack->c_str());
	}

	delete pack;
}



void
ctlbase::fsp_open(
		cofmatch& match)
{
	try {
		WRITELOG(CFWD, DBG, "ctlbase(%s)::fsp_open() [original] "
				"match: %s",
				dpname.c_str(), match.c_str());

		uint32_t in_port = match.get_in_port();

		// match is defined for a specific inport, filter through the adapter
		if (n_ports.find(in_port) == n_ports.end())
		{
			WRITELOG(CFWD, DBG, "ctlbase(%s)::fsp_open() "
					"match refers to in_port: 0x%x, which does not exist, ignoring",
					dpname.c_str(), in_port);

			return;
		}
		n_ports[in_port]->dpt_filter_match(this, in_port, match);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::fsp_open() [adapted] "
				"match: %s",
				dpname.c_str(), match.c_str());

	} catch (eOFmatchNotFound& e) {

		// no in_port, do nothing with our adapters
	}

	if (0 == dpath)
	{
		WRITELOG(CFWD, DBG, "ctlbase(%s)::fsp_open() "
				"no dpt available, igoring",
				dpname.c_str());

		return;
	}

	dpath->fsp_open(match);
}



void
ctlbase::fsp_close(
		cofmatch& match)
{
	try {
		WRITELOG(CFWD, DBG, "ctlbase(%s)::fsp_close() [original] "
				"match: %s",
				dpname.c_str(), match.c_str());

		uint32_t in_port = match.get_in_port();

		// match is defined for a specific inport, filter through the adapter
		if (n_ports.find(in_port) == n_ports.end())
		{
			WRITELOG(CFWD, DBG, "ctlbase(%s)::fsp_close() "
					"match refers to in_port: 0x%x, which does not exist, ignoring",
					dpname.c_str(), in_port);

			return;
		}
		n_ports[in_port]->dpt_filter_match(this, in_port, match);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::fsp_close() [adapted] "
				"match: %s",
				dpname.c_str(), match.c_str());

	} catch (eOFmatchNotFound& e) {

		// no in_port, do nothing with our adapters
	}

	if (0 == dpath)
	{
		WRITELOG(CFWD, DBG, "ctlbase(%s)::fsp_close() "
				"no dpt available, igoring",
				dpname.c_str());

		return;
	}

	dpath->fsp_close(match);
}



/*
 * PORT-STATUS
 */

/*
 * received by layer (n-1) datapath, sending to adapters for handling changes in port status
 */
void
ctlbase::handle_port_status(
		cofdpt *sw,
		cofpacket *pack,
		cofport *port)
{
	WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_port_status() %s", dpname.c_str(), port->c_str());

	if (sw != dpath)
	{
		WRITELOG(CFWD, DBG, "ctlbase(%s)::handle_port_status() "
				"rcvd packet from non-registered dpath"
				" sw: %p dpath: %p", dpname.c_str(), sw, dpath);

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
		cadapt_dpt *dpt,
		uint32_t requested)
			throw (eAdaptNotFound)
{
	uint32_t portno = (requested != 0) ? requested : 1;;

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

	WRITELOG(CFWD, DBG, "ctlbase(%s)::ctl_handle_packet_in() "
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
ctlbase::ctl_handle_stats_reply(
				cadapt_dpt *dpt,
				uint32_t xid,
				uint16_t type,
				uint8_t* body,
				size_t bodylen)
{
	cadapt* adapt = dynamic_cast<cadapt*>( dpt );

	if (0 == adapt)
	{
		return;
	}

	WRITELOG(CFWD, DBG, "ctlbase(%s)::ctl_handle_stats_reply() "
			"type: %d", dpname.c_str(), type);

	handle_stats_reply(
			adapt,
			xid,
			type,
			body,
			bodylen); // let derived transport controller handle incoming stats-reply
}



void
ctlbase::ctl_handle_stats_reply_timeout(
		cadapt_dpt *dpt,
		uint32_t xid)
{
	cadapt* adapt = dynamic_cast<cadapt*>( dpt );

	if (0 == adapt)
	{
		return;
	}

	WRITELOG(CFWD, DBG, "ctlbase(%s)::ctl_handle_stats_reply_timeout() ", dpname.c_str());

	handle_stats_reply_timeout(
			adapt,
			xid); // let derived transport controller handle stats-reply timeout
}



void
ctlbase::ctl_handle_barrier_reply(
				cadapt_dpt *dpt,
				uint32_t xid)
{
	cadapt* adapt = dynamic_cast<cadapt*>( dpt );

	if (0 == adapt)
	{
		return;
	}

	WRITELOG(CFWD, DBG, "ctlbase(%s)::ctl_handle_barrier_reply() ", dpname.c_str());

	handle_barrier_reply(
			adapt,
			xid); // let derived transport controller handle incoming barrier-reply
}



void
ctlbase::ctl_handle_barrier_reply_timeout(
				cadapt_dpt *dpt,
				uint32_t xid)
{
	cadapt* adapt = dynamic_cast<cadapt*>( dpt );

	if (0 == adapt)
	{
		return;
	}

	WRITELOG(CFWD, DBG, "ctlbase(%s)::ctl_handle_barrier_reply_timeout() ", dpname.c_str());

	handle_barrier_reply_timeout(
			adapt,
			xid); // let derived transport controller handle barrier-reply timeout
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

		WRITELOG(CFWD, DBG, "ctlbase(%s)::dpt_flowspace_open() "
				"rcvd cofmatch [1] from adapter: %s:\n%s",
				dpname.c_str(), adapt->c_str(), match.c_str());

		dpath->fsptable.insert_fsp_entry(adapt, match, false /*non-strict*/);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::dpt_flowspace_open() "
				"rcvd cofmatch [2] from adapter: %s:\n%s",
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

		if (0 == dpath)
		{
			return;
		}

		dpath->fsptable.delete_fsp_entry(adapt, match, false /*non-strict*/);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::dpt_flowspace_close() "
				"rcvd cofmatch from adapter: %s:\n%s",
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


uint32_t
ctlbase::dpt_get_phy_port(
		cadapt_ctl* ctl,
		uint32_t port_no)
						throw (eAdaptNotFound)
{
	return 0;
}


uint32_t
ctlbase::dpt_handle_stats_request(
				cadapt_ctl *ctl,
				uint32_t port_no,
				uint16_t type,
				uint8_t *body,
				size_t bodylen)
						throw (eAdaptNotFound)
{
	uint32_t xid = 0;
	try {
		WRITELOG(CFWD, DBG, "ctlbase(%s)::dpt_handle_stats_request()", dpname.c_str());

		xid = cfwdelem::send_stats_request(dpath, type, 0, body, bodylen);

		WRITELOG(CFWD, DBG, "ctlbase(%s)::dpt_handle_stats_request() "
				"new stats-request sent => type: 0x%x xid: 0x%x ", dpname.c_str(), type, xid);

		xidstore.xid_add(ctl, xid); // remember => "ctl" triggered this transaction "xid"

	} catch (eXidStoreXidBusy& e) {

		WRITELOG(CFWD, WARN, "ctlbase(%s)::dpt_handle_stats_request() "
				"xid: 0x%x already stored in xidstore", dpname.c_str(), xid);
	} catch (eOFbaseNotAttached& e) {

		WRITELOG(CFWD, WARN, "ctlbase(%s)::dpt_handle_stats_request() "
				"no dpath attached", dpname.c_str());
	}

	return xid;
}



uint32_t
ctlbase::dpt_handle_barrier_request(
		cadapt_ctl *ctl,
		uint32_t port_no)
				throw (eAdaptNotFound)
{
	uint32_t xid = 0;
	try {
		xid = cfwdelem::send_barrier_request(dpath);

		xidstore.xid_add(ctl, xid); // remember => "ctl" triggered this transaction "xid"

	} catch (eXidStoreXidBusy& e) {

		WRITELOG(CFWD, WARN, "ctlbase(%s)::dpt_handle_barrier_request() "
				"xid: 0x%x already stored in xidstore", dpname.c_str(), xid);

	} catch (eOFbaseNotAttached& e) {

		WRITELOG(CFWD, WARN, "ctlbase(%s)::dpt_handle_barrier_request() "
				"no dpath attached", dpname.c_str());
	}

	return xid;
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


#if 0
		uint32_t in_port = match.get_in_port();

		if ((OFPP_CONTROLLER != in_port) && (n_ports.find(in_port) == n_ports.end()))
		{
			throw eCtlBaseInval();
		}
#endif

		WRITELOG(CFWD, DBG, "ctlbase(%s)::send_flow_mod_message() =>\n"
				"  match [original] => %s\n",
				dpname.c_str(), match.c_str());

		cofaclist match_add_this;

		try {
			uint32_t in_port = match.get_in_port();

			if (n_ports.find(in_port) != n_ports.end())
			{
				match_add_this = n_ports[in_port]->dpt_filter_match(this, in_port, match);
			}

		} catch (eOFmatchNotFound& e) {}


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

								if (out_port > OFPP_MAX)
								{
									actions.next() = action;
									continue;
								}

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

		WRITELOG(CFWD, DBG, "ctlbase(%s)::send_flow_mod_message() "
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
	 * if there are specific ports > OFPP_MAX (e.g. OFPP_FLOOD, OFPP_ALL),
	 * we are generating multiple packet-outs, each per port, as adaptation
	 * might be different for each port.
	 */
	if (aclist.count_action_output(OFPP_ALL) || aclist.count_action_output(OFPP_FLOOD))
	{
		for (std::map<uint32_t, cadapt*>::iterator
				it = n_ports.begin(); it != n_ports.end(); ++it)
		{
			if (in_port == it->first)
			{
				continue; // ignore in-port
			}

			cofaclist accopy;
			WRITELOG(CFWD, DBG, "ctlbase(%s)::send_packet_out_message() "
					"special purpose outport found, calling for out-port: %lu",
					dpname.c_str(), it->first);

			for (cofaclist::const_iterator
					jt = aclist.begin(); jt != aclist.end(); ++jt)
			{
				cofaction action(*jt);

				switch (action.get_type()) {
				case OFPAT_OUTPUT:
					{
						uint32_t out_port = be32toh(action.oac_output->port);

						switch (out_port) {
						case OFPP_ALL:
						case OFPP_FLOOD:
							{
								accopy.next() = cofaction_output(it->first);
							}
							break;
						default:
							{
								accopy.next() = action;
							}
							break;
						}
					}
					break;
				default:
					{
						accopy.next() = action;
					}
					break;
				}
			}

			if (0 == pack)
			{
				send_packet_out_message(buffer_id, in_port, accopy);
			}
			else
			{
				cpacket* n_pack = new cpacket(*pack); // clone of packet

				send_packet_out_message(buffer_id, in_port, accopy, n_pack);
			}
		}

		return; // we are done
	}


	/*
	 * iterate over all actions in aclist and call adapt methods
	 */
	cofaclist actions; // list of adapted actions

	cofaclist accopy(aclist);
	WRITELOG(CFWD, DBG, "ctlbase(%s)::send_packet_out_message() "
			"\nactions [original] => %s", dpname.c_str(), accopy.c_str());


	if (0 != pack)
	{
		WRITELOG(CFWD, DBG, "ctlbase(%s)::send_packet_out_message() =>\n"
				"GGG[1] %s", dpname.c_str(), pack->c_str());
	}







	for (cofaclist::const_iterator
			it = aclist.begin(); it != aclist.end(); ++it)
	{
		cofaction action(*it);

		switch (action.get_type()) {
		case OFPAT_OUTPUT:
			{
				uint32_t out_port = be32toh(action.oac_output->port);

				if (out_port > OFPP_MAX)
				{
					actions.next() = action; // e.g. OFPP_ALL, OFPP_FLOOD, etc.
					continue;
				}


				if (n_ports.find(out_port) == n_ports.end())
				{
					throw eCtlBaseInval(); // outgoing port is invalid
				}


				/*
				 * add actions for inport
				 */
				{
					cofmatch empty;

					cofaclist add_this = n_ports[out_port]->dpt_filter_match(this, out_port, empty);

					// copy all adapted actions to the actions list
					for (cofaclist::const_iterator
							it = add_this.begin(); it != add_this.end(); ++it)
					{
						actions.next() = (*it);
					}

					WRITELOG(CFWD, DBG, "ctlbase(%s)::send_packet_out_message() HHH =>\n"
							"actions [add_this_inport] => %s", dpname.c_str(), add_this.c_str());
				}


				/*
				 * add actions for outport
				 */
				{
					// call the stack and filter all actions
					cofaclist add_this = n_ports[out_port]->dpt_filter_action(this, out_port, action);

					// copy all adapted actions to the actions list
					for (cofaclist::const_iterator
							it = add_this.begin(); it != add_this.end(); ++it)
					{
						actions.next() = (*it);
					}

					WRITELOG(CFWD, DBG, "ctlbase(%s)::send_packet_out_message() "
							"\nactions [add_this_outport] => %s", dpname.c_str(), add_this.c_str());
				}


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


	if (0 != pack)
	{
		WRITELOG(CFWD, DBG, "ctlbase(%s)::send_packet_out_message() =>\n"
				"GGG[2] %s", dpname.c_str(), pack->c_str());
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











uint32_t
ctlbase::send_stats_request(
		uint16_t type,
		uint16_t flags,
		uint8_t *body,
		size_t bodylen)
	throw (eCtlBaseInval)
{
	if (0 == dpath)
	{
		return 0;
	}

	uint32_t xid = 0;

	switch (type) {
	case OFPST_DESC:
	case OFPST_TABLE:
	case OFPST_GROUP:
	case OFPST_GROUP_DESC:
	case OFPST_GROUP_FEATURES:
	case OFPST_EXPERIMENTER:
		{
			xid = cfwdelem::send_stats_request(dpath, type, flags, body, bodylen);

			xidstore.xid_add(this, xid, 15/*seconds*/);
		}
		break;
	case OFPST_FLOW:
		{
			throw eNotImplemented();
		}
		break;
	case OFPST_AGGREGATE:
		{
			/*
			 * filter match through our adapters
			 */
			cofstats_aggregate_request aggr(body, bodylen);

			// out_port
			uint32_t out_port = be32toh(aggr.ofs_aggr_stats_request->out_port);
			if (OFPP_ANY != out_port)
			{
				if (n_ports.find(out_port) == n_ports.end())
				{
					throw eCtlBaseInval();
				}
				aggr.ofs_aggr_stats_request->out_port =
						htobe32(n_ports[out_port]->dpt_get_phy_port(this, out_port));
			}

			// in_port
			try {

				uint32_t in_port = aggr.match.get_in_port();

				if (n_ports.find(in_port) == n_ports.end())
				{
					throw eCtlBaseInval();
				}
				aggr.match.set_in_port(in_port);

				dpt_filter_match(this, in_port, aggr.match); // ignore returned action list

			} catch (eOFmatchNotFound& e) {}

			// in_phy_port
			try {

				uint32_t in_phy_port = aggr.match.get_in_phy_port();

				if (n_ports.find(in_phy_port) == n_ports.end())
				{
					throw eCtlBaseInval();
				}
				aggr.match.set_in_phy_port(in_phy_port);

			} catch (eOFmatchNotFound& e) {}



			xid = cfwdelem::send_stats_request(dpath, type, flags, body, bodylen);

			xidstore.xid_add(this, xid, 15/*seconds*/);
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

	return xid;
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

					if (out_port > OFPP_MAX)
					{
						actions.next() = action;
						continue;
					}

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



cadapt*
ctlbase::call_adapter(cadapt* adapt)
			throw (eCtlBaseNotFound)
{
	std::map<unsigned int, std::list<cadapt*> >::iterator it;
	for (it = adstacks.begin(); it != adstacks.end(); ++it)
	{
		std::list<cadapt*>& adstack = it->second;

		if (adstack.back() == adapt)
		{
			return adapt;
		}
	}
	throw eCtlBaseNotFound();
}


