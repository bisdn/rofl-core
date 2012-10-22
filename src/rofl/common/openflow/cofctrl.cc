/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofctrl.h"

cofctrl::cofctrl(cofbase* ofbase, cofiface *ctrl, std::map<cofiface*, cofctrl*> *ofctrl_list) :
		ofbase(ofbase),
		ofctrl_list(ofctrl_list),
		ctrl(ctrl),
		flags(0),
		miss_send_len(OFP_DEFAULT_MISS_SEND_LEN),
		role_initialized(false),
		role(OFPCR_ROLE_EQUAL),
		cached_generation_id(0)
{
	(*ofctrl_list)[ctrl] = this;
	WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::cofctrl()", this);

	ofbase->handle_ctrl_open(this);
}


cofctrl::~cofctrl()
{
	WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::~cofctrl()", this);

	ofbase->handle_ctrl_close(this);

	ofctrl_list->erase(ctrl);

	//ctrl->dpath_detach(fwdelem);

	ofbase->fsptable.delete_fsp_entries(this);
}


void
cofctrl::features_request_rcvd(cofpacket *pack)
{
	ofbase->handle_features_request(this, pack);
}


void
cofctrl::packet_out_rcvd(cofpacket *pack)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		size_t len = (pack->length() > 64) ? 64 : pack->length();
		ofbase->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
										pack->soframe(), len);
		return;
	}

	ofbase->handle_packet_out(this, pack);
}


void
cofctrl::flow_mod_rcvd(cofpacket *pack)
{
	WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::flow_mod_rcvd() pack: %s", this, pack->c_str());

#if 0


	ofbase->handle_flow_mod(pack);
#endif

#if 0
	try {
		if (OFPCR_ROLE_SLAVE == role)
		{
			size_t len = (pack->length() > 64) ? 64 : pack->length();
			ofbase->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
											pack->soframe(), len);
			return;
		}

		// check, whether the controlling pack->entity is allowed to install this flow-mod
		if (ofbase->fe_flags.test(cfwdelem::NSP_ENABLED))
		{
			switch (pack->ofh_flow_mod->command) {
			case OFPFC_ADD:
			case OFPFC_MODIFY:
			case OFPFC_MODIFY_STRICT:
				ofbase->fsptable.flow_mod_allowed(this, pack->match);
				break;
				/*
				 * this allows generic DELETE commands to be applied
				 * FIXME: does this affect somehow entries from other controllers?
				 */
			}
		}

		cftentry *fte = NULL;

		// table_id == 255 (all tables)
		if (OFPTT_ALL == pack->ofh_flow_mod->table_id)
		{
			std::map<uint8_t, cfttable*>::iterator it;
			for (it = ofbase->flow_tables.begin(); it != ofbase->flow_tables.end(); ++it)
			{
				if ((fte = it->second->update_ft_entry(fwdelem, pack)) != NULL)
				{
					fte->ofctrl = this; // store controlling entity for this cftentry
					WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::flow_mod_rcvd() table_id %d new %s",
							this, pack->ofh_flow_mod->table_id, fte->c_str());
				}
			}
		}
		// single table
		else
		{
			// check for existence of specified table
			if (ofbase->flow_tables.find(pack->ofh_flow_mod->table_id) == ofbase->flow_tables.end())
			{
				throw eFwdElemTableNotFound();
			}

			// do not lock here flow_table[i]

			if ((fte = ofbase->flow_tables[pack->ofh_flow_mod->table_id]->
							update_ft_entry(fwdelem, pack)) == NULL)
			{
				return;
			}



			fte->ofctrl = this; // store controlling entity for this cftentry
			WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::flow_mod_rcvd() table_id %d new %s",
					this, pack->ofh_flow_mod->table_id, fte->c_str());


			try {
				WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::flow_mod_rcvd() new fte created: %s", this, fte->c_str());

				cofinst& inst = fte->instructions.find_inst(OFPIT_GOTO_TABLE);

				if (ofbase->flow_tables.find(inst.oin_goto_table->table_id)
								== ofbase->flow_tables.end())
				{
					throw eFwdElemGotoTableNotFound();
				}

			} catch (eInListNotFound& e) {}
		}

		if (0 != fte)
		{
			ofbase->handle_flow_mod(this, pack, fte);
		}

	} catch (eLockWouldBlock& e) {

		throw;

	} catch (eLockInval& e) {

		throw;
	}
#endif
}


void
cofctrl::group_mod_rcvd(cofpacket *pack)
{
#if 0
	try {

		if (OFPCR_ROLE_SLAVE == role)
		{
			size_t len = (pack->length() > 64) ? 64 : pack->length();
			ofbase->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
											pack->soframe(), len);
			return;
		}


		cgtentry *gte = ofbase->group_table.update_gt_entry(fwdelem, pack->ofh_group_mod);
		ofbase->handle_group_mod(this, pack, gte);




	} catch (eGroupTableExists& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry already exists, dropping", this);

		ofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_GROUP_EXISTS,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableNotFound& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry not found", this);

		ofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_UNKNOWN_GROUP,
				pack->soframe(), pack->framelen());

	} catch (eGroupEntryInval& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry is invalid", this);

		ofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_INVALID_GROUP,
				pack->soframe(), pack->framelen());

	} catch (eGroupEntryBadType& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry with bad type", this);

		ofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_BAD_TYPE,
				pack->soframe(), pack->framelen());

	} catch (eActionBadOutPort& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry with action with bad type", this);

		ofbase->send_error_message(this, OFPET_BAD_ACTION, OFPBAC_BAD_OUT_PORT,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableLoopDetected& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry produces loop, dropping", this);

		ofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_LOOP,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableModNonExisting& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry for modification not found, dropping", this);

		ofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_UNKNOWN_GROUP,
				pack->soframe(), pack->framelen());

	}
#endif
}


void
cofctrl::port_mod_rcvd(cofpacket *pack) throw (eOFctrlPortNotFound)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		size_t len = (pack->length() > 64) ? 64 : pack->length();
		ofbase->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
										pack->soframe(), len);
		return;
	}

	uint32_t port_no = be32toh(pack->ofh_port_mod->port_no);

	/*
	 * update cofport structure in ofbase->phy_ports
	 */
	if (ofbase->phy_ports.find(port_no) == ofbase->phy_ports.end())
	{
		throw eOFctrlPortNotFound();
	}

	ofbase->phy_ports[port_no]->recv_port_mod(
						be32toh(pack->ofh_port_mod->config),
						be32toh(pack->ofh_port_mod->mask),
						be32toh(pack->ofh_port_mod->advertise));

	ofbase->handle_port_mod(this, pack);
}


void
cofctrl::table_mod_rcvd(cofpacket *pack)
{
#if 0
	try {
		if (OFPCR_ROLE_SLAVE == role)
		{
			size_t len = (pack->length() > 64) ? 64 : pack->length();
			ofbase->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
											pack->soframe(), len);
			return;
		}

		if (ofbase->flow_tables.find(pack->ofh_table_mod->table_id) != ofbase->flow_tables.end())
		{
			ofbase->flow_tables[pack->ofh_table_mod->table_id]->set_config(
												be32toh(pack->ofh_table_mod->config));
		}

	} catch (eLockWouldBlock& e) {

		throw;

	} catch (eLockInval& e) {

		throw;

	}
#endif
	ofbase->handle_table_mod(this, pack);
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
				ofbase->send_error_message(this, OFPET_ROLE_REQUEST_FAILED, OFPRRFC_STALE);
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


	for (std::map<cofiface*, cofctrl*>::iterator
			it = ofbase->ofctrl_list.begin(); it != ofbase->ofctrl_list.end(); ++it)
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

	ofbase->handle_role_request(this, pack);
}


void
cofctrl::barrier_request_rcvd(cofpacket *pack)
{
	ofbase->handle_barrier_request(this, pack);
}


void
cofctrl::experimenter_message_rcvd(cofpacket *pack)
{
	switch (be32toh(pack->ofh_experimenter->experimenter)) {
	case OFPEXPID_ROFL:
	{
		switch (be32toh(pack->ofh_experimenter->exp_type)) {
		case croflexp::OFPRET_FLOWSPACE:
		{
			croflexp rexp(pack->body.somem(), pack->body.memlen());

			switch (rexp.rext_fsp->command) {
			case croflexp::OFPRET_FSP_ADD:
			{
				try {

					WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => pending for %s",
							this, rexp.match.c_str());

					ofbase->fsptable.insert_fsp_entry(this, rexp.match);

					WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => -ADDED- %s\n%s",
							this, c_str(), ofbase->fsptable.c_str());

				} catch (eFspEntryOverlap& e) {

					WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => -REJECTED- (overlap)",
							this);

				}

				break;
			}
			case croflexp::OFPRET_FSP_DELETE:
			{
				try {

					WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_DELETE => pending for %s",
							this, rexp.match.c_str());

					ofbase->fsptable.delete_fsp_entry(this, rexp.match, true /*strict*/);

					WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_DELETE => -DELETED- %s\n%s",
							this, c_str(), ofbase->fsptable.c_str());

				} catch (eFspEntryNotFound& e) {

					WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_DELETE => -NOT-FOUND-",
							this);

				}

				break;
			}
			}

			break;
		}

		}

		delete pack;
		break;
	}


	default:
		ofbase->handle_experimenter_message(this, pack);
		break;
	}
}






const char*
cofctrl::c_str()
{
#if 0
	std::string t_str;

	std::map<cofiface*, cofctrl*>::iterator it;
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




