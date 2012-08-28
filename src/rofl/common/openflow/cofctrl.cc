/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofctrl.h"

cofctrl::cofctrl(cfwdelem* fwdelem, cofbase *ctrl, std::map<cofbase*, cofctrl*> *ofctrl_list) :
		fwdelem(fwdelem),
		ofctrl_list(ofctrl_list),
		ctrl(ctrl),
		flags(0),
		miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
		//flow_table(0),
		role_initialized(false),
		role(OFPCR_ROLE_EQUAL),
		cached_generation_id(0)
{
	(*ofctrl_list)[ctrl] = this;
        flow_table = new cfttable(0);
	WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::cofctrl()", this);

	fwdelem->handle_ctrl_open(this);
}


cofctrl::~cofctrl()
{
	WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::~cofctrl()", this);

	fwdelem->handle_ctrl_close(this);

	ofctrl_list->erase(ctrl);

	//ctrl->dpath_detach(fwdelem);

	fwdelem->fsptable.delete_fsp_entries(this);
	delete flow_table;
}


void
cofctrl::features_request_rcvd(cofpacket *pack)
{
	fwdelem->handle_features_request(this, pack);
}


void
cofctrl::packet_out_rcvd(cofpacket *pack)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		size_t len = (pack->length() > 64) ? 64 : pack->length();
		fwdelem->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
										pack->soframe(), len);
		return;
	}

	fwdelem->handle_packet_out(this, pack);
}


void
cofctrl::flow_mod_rcvd(cofpacket *pack)
{
	WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_flow_mod() pack: %s", this, pack->c_str());

	try {
		if (OFPCR_ROLE_SLAVE == role)
		{
			size_t len = (pack->length() > 64) ? 64 : pack->length();
			fwdelem->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
											pack->soframe(), len);
			return;
		}

		// check, whether the controlling pack->entity is allowed to install this flow-mod
		if (fwdelem->fe_flags.test(cfwdelem::NSP_ENABLED))
		{
			fwdelem->fsptable.flow_mod_allowed(this, pack->match);
		}

		cftentry *fte = NULL;

		// table_id == 255 (all tables)
		if (OFPTT_ALL == pack->ofh_flow_mod->table_id)
		{
			std::map<uint8_t, cfttable*>::iterator it;
			for (it = fwdelem->flow_tables.begin(); it != fwdelem->flow_tables.end(); ++it)
			{
				if ((fte = it->second->update_ft_entry(fwdelem, pack)) != NULL)
				{
					fte->ofctrl = this; // store controlling entity for this cftentry
					WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_flow_mod() table_id %d new %s",
							this, pack->ofh_flow_mod->table_id, fte->c_str());
				}
			}
		}
		// single table
		else
		{
			// check for existence of specified table
			if (fwdelem->flow_tables.find(pack->ofh_flow_mod->table_id) == fwdelem->flow_tables.end())
			{
				throw eFwdElemTableNotFound();
			}

			// do not lock here flow_table[i]

			if ((fte = fwdelem->flow_tables[pack->ofh_flow_mod->table_id]->
							update_ft_entry(fwdelem, pack)) != NULL)
			{
				fte->ofctrl = this; // store controlling entity for this cftentry
				WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_flow_mod() table_id %d new %s",
						this, pack->ofh_flow_mod->table_id, fte->c_str());
			}

			try {
				WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_flow_mod() new fte created: %s", this, fte->c_str());

				cofinst& inst = fte->instructions.find_inst(OFPIT_GOTO_TABLE);

				if (fwdelem->flow_tables.find(inst.oin_goto_table->table_id)
								== fwdelem->flow_tables.end())
				{
					throw eFwdElemGotoTableNotFound();
				}

			} catch (eInListNotFound& e) {}
		}

		fwdelem->handle_flow_mod(this, pack, fte);

	} catch (eLockWouldBlock& e) {

		throw;

	} catch (eLockInval& e) {

		throw;
	}
}


void
cofctrl::group_mod_rcvd(cofpacket *pack)
{
	try {

		if (OFPCR_ROLE_SLAVE == role)
		{
			size_t len = (pack->length() > 64) ? 64 : pack->length();
			fwdelem->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
											pack->soframe(), len);
			return;
		}


		cgtentry *gte = fwdelem->group_table.update_gt_entry(fwdelem, pack->ofh_group_mod);
		fwdelem->handle_group_mod(this, pack, gte);




	} catch (eGroupTableExists& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry already exists, dropping", this);

		fwdelem->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_GROUP_EXISTS,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableNotFound& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry not found", this);

		fwdelem->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_UNKNOWN_GROUP,
				pack->soframe(), pack->framelen());

	} catch (eGroupEntryInval& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry is invalid", this);

		fwdelem->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_INVALID_GROUP,
				pack->soframe(), pack->framelen());

	} catch (eGroupEntryBadType& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry with bad type", this);

		fwdelem->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_BAD_TYPE,
				pack->soframe(), pack->framelen());

	} catch (eActionBadOutPort& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry with action with bad type", this);

		fwdelem->send_error_message(this, OFPET_BAD_ACTION, OFPBAC_BAD_OUT_PORT,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableLoopDetected& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry produces loop, dropping", this);

		fwdelem->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_LOOP,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableModNonExisting& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry for modification not found, dropping", this);

		fwdelem->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_UNKNOWN_GROUP,
				pack->soframe(), pack->framelen());

	}
}


void
cofctrl::port_mod_rcvd(cofpacket *pack) throw (eOFctrlPortNotFound)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		size_t len = (pack->length() > 64) ? 64 : pack->length();
		fwdelem->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
										pack->soframe(), len);
		return;
	}

	uint32_t port_no = be32toh(pack->ofh_port_mod->port_no);

	/*
	 * update cofport structure in fwdelem->phy_ports
	 */
	if (fwdelem->phy_ports.find(port_no) == fwdelem->phy_ports.end())
	{
		throw eOFctrlPortNotFound();
	}

	fwdelem->phy_ports[port_no]->recv_port_mod(
						be32toh(pack->ofh_port_mod->config),
						be32toh(pack->ofh_port_mod->mask),
						be32toh(pack->ofh_port_mod->advertise));

	fwdelem->handle_port_mod(this, pack);
}


void
cofctrl::table_mod_rcvd(cofpacket *pack)
{
	try {
		if (OFPCR_ROLE_SLAVE == role)
		{
			size_t len = (pack->length() > 64) ? 64 : pack->length();
			fwdelem->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
											pack->soframe(), len);
			return;
		}

		if (fwdelem->flow_tables.find(pack->ofh_table_mod->table_id) != fwdelem->flow_tables.end())
		{
			fwdelem->flow_tables[pack->ofh_table_mod->table_id]->set_config(
												be32toh(pack->ofh_table_mod->config));
		}

	} catch (eLockWouldBlock& e) {

		throw;

	} catch (eLockInval& e) {

		throw;

	}

	fwdelem->handle_table_mod(this, pack);
}


void
cofctrl::role_request_rcvd(cofpacket *pack)
{
	switch (be32toh(pack->ofh_role_request->role)) {
	case OFPCR_ROLE_MASTER:
	case OFPCR_ROLE_SLAVE:
		if (role_initialized)
		{
			uint64_t gen_id = be64toh(pack->ofh_role_request->generation_id);
			uint64_t dist = (gen_id > cached_generation_id) ?
					(gen_id - cached_generation_id) % std::numeric_limits<uint64_t>::max() :
					(gen_id + std::numeric_limits<uint64_t>::max() + cached_generation_id) % std::numeric_limits<uint64_t>::max();

			if (dist >= (std::numeric_limits<uint64_t>::max() / 2))
			{
				fwdelem->send_error_message(this, OFPET_ROLE_REQUEST_FAILED, OFPRRFC_STALE);
				return;
			}
		}
		else
		{
			role_initialized = true;
		}
		cached_generation_id = be64toh(pack->ofh_role_request->generation_id);
		break;
	default:
		break;
	}

	role = be32toh(pack->ofh_role_request->role);


	for (std::map<cofbase*, cofctrl*>::iterator
			it = fwdelem->ofctrl_list.begin(); it != fwdelem->ofctrl_list.end(); ++it)
	{
		cofctrl* ofctrl = it->second;

		if (ofctrl == this)
		{
			continue;
		}

		if (OFPCR_ROLE_MASTER == ofctrl->role)
		{
			ofctrl->role = OFPCR_ROLE_SLAVE;
		}
	}

	//pack->ofh_role_request->generation_id;

	fwdelem->handle_role_request(this, pack);
}


void
cofctrl::barrier_request_rcvd(cofpacket *pack)
{
	fwdelem->handle_barrier_request(this, pack);
}


void
cofctrl::experimenter_message_rcvd(cofpacket *pack)
{
	switch (be32toh(pack->ofh_experimenter->experimenter)) {
	case VENDOR_EXT_ROFL:
	{
		switch (be32toh(pack->ofh_experimenter->exp_type)) {
		case OFPRET_NSP_GET_FSP_REQUEST:
			handle_fsp_get_fsp_request(pack);
			break;
		case OFPRET_NSP_GET_FSP_REPLY:
			handle_fsp_get_fsp_reply(pack);
			break;
		case OFPRET_NSP_OPEN_REQUEST:
			handle_fsp_open_request(pack);
			break;
		case OFPRET_NSP_OPEN_REPLY:
			handle_fsp_open_reply(pack);
			break;
		case OFPRET_NSP_CLOSE_REQUEST:
			handle_fsp_close_request(pack);
			break;
		case OFPRET_NSP_CLOSE_REPLY:
			handle_fsp_close_reply(pack);
			break;
		case OFPRET_NSP_IOCTL_REQUEST:
			handle_fsp_ioctl_request(pack);
			break;
		case OFPRET_NSP_IOCTL_REPLY:
			handle_fsp_ioctl_reply(pack);
			break;
		default:
			fwdelem->handle_experimenter_message(this, pack);
			break;
		}

		// remove packet from heap
		delete pack;

		break;
	}

	default:
		fwdelem->handle_experimenter_message(this, pack);
		break;
	}
}


void
cofctrl::handle_fsp_get_fsp_request(
		cofpacket *pack)
{

}


void
cofctrl::handle_fsp_get_fsp_reply(
		cofpacket *pack)
{
	// intentionally empty
}


void
cofctrl::handle_fsp_open_request(
		cofpacket *pack)
{
	try {
		struct ofp_vendor_ext_rofl_nsp *rofl = (struct ofp_vendor_ext_rofl_nsp*)(pack->soframe());

		size_t matchlen = (pack->framelen() - sizeof(struct ofp_vendor_ext_rofl_nsp));

		cofmatch match(rofl->match, matchlen);

		WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::handle_fsp_open_request() %s", this, match.c_str());

		WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::handle_fsp_open_request() -VVVVVVVVV- %s", this, c_str());

		fwdelem->fsptable.insert_fsp_entry(this, match);

		fwdelem->send_experimenter_ext_rofl_nsp_open_reply(pack, this,
					OFPRET_NSP_RESULT_OK, match);

		WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::handle_fsp_open_request() -ACCEPTED- %s\n%s", this, c_str(), fwdelem->fsptable.c_str());

	} catch (eFspEntryOverlap& e) {

		// FIXME: what cofmatch object shall we return when overlapping?
		fwdelem->send_experimenter_ext_rofl_nsp_open_reply(pack, this,
							OFPRET_NSP_RESULT_OVERLAP, pack->match);

		WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::handle_fsp_open_request() -REJECTED- %s", this, c_str());

	}

}


void
cofctrl::handle_fsp_open_reply(
		cofpacket *pack)
{

}


void
cofctrl::handle_fsp_close_request(
		cofpacket *pack)
{
//	bool found = false;

	WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::handle_fsp_close_request() %s", this, pack->match.c_str());

#if 1
	cofmatch match;
	try {
		struct ofp_vendor_ext_rofl_nsp *rofl = (struct ofp_vendor_ext_rofl_nsp*)(pack->soframe());

		size_t matchlen = (pack->framelen() - sizeof(struct ofp_vendor_ext_rofl_nsp));

		match = cofmatch(rofl->match, matchlen);

		fwdelem->fsptable.delete_fsp_entry(this, match, true /*strict*/);

		fwdelem->send_experimenter_ext_rofl_nsp_close_reply(pack, this,
								OFPRET_NSP_RESULT_OK, match);

	} catch (eFspEntryNotFound& e) {

		fwdelem->send_experimenter_ext_rofl_nsp_close_reply(pack, this,
								OFPRET_NSP_RESULT_NOT_FOUND, match);

	}

#else
	std::set<cofmatch*>::iterator it;
	for (it = nspaces.begin(); it != nspaces.end(); ++it)
	{
		if ((*it)->overlaps(&(nspreq->match), true /*strict*/))
		{
			nspaces.erase(it);
			fwdelem->send_experimenter_ext_rofl_nsp_close_reply(pack, this,
							OFPRET_NSP_RESULT_OK, ofmatch);
			found = true;
		}
	}

	if (not found)
	{
		fwdelem->send_experimenter_ext_rofl_nsp_close_reply(pack, this,
								OFPRET_NSP_RESULT_NOT_FOUND, ofmatch);
	}
#endif

	WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::handle_fsp_close_request() %s", this, c_str());
}


void
cofctrl::handle_fsp_close_reply(
		cofpacket *pack)
{

}


void
cofctrl::handle_fsp_ioctl_request(
		cofpacket *pack)
{

}


void
cofctrl::handle_fsp_ioctl_reply(
		cofpacket *pack)
{

}





const char*
cofctrl::c_str()
{
#if 0
	std::string t_str;

	std::map<cofbase*, cofctrl*>::iterator it;
	for (it = ofctrl_list->begin(); it != ofctrl_list->end(); ++it)
	{
		std::set<cofmatch*>::iterator nit;
		for (nit = it->second->nspaces.begin(); nit != it->second->nspaces.end(); ++nit)
		{
			t_str.append("\n");
			t_str.append(it->second->ctrl->c_str());
			t_str.append(" => ");
			t_str.append((*nit)->c_str());
			t_str.append("\n");
		}
	}
#endif

	cvastring vas;

	info.assign(vas("cofctrl(%p) %s", this, ctrl->c_str()));

	return info.c_str();
}



/*
 * callbacks for receiving notifications from cftentry instances
 */


/** called upon hard timer expiration
 */
void
cofctrl::ftentry_timeout(
		cftentry *fte,
		uint16_t timeout)
{
	fwdelem->ftentry_timeout(fte, timeout);
}

