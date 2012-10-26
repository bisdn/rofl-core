/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofctrl.h"

cofctrl::cofctrl(
		crofbase* fwdelem,
		cofbase *ctrl,
		std::map<cofbase*, cofctrl*> *ofctrl_list) :
		rofbase(fwdelem),
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

	rofbase->handle_ctrl_open(this);
}


cofctrl::~cofctrl()
{
	WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::~cofctrl()", this);

	rofbase->handle_ctrl_close(this);

	ofctrl_list->erase(ctrl);

	//ctrl->dpath_detach(fwdelem);

	rofbase->fsptable.delete_fsp_entries(this);
}


void
cofctrl::features_request_rcvd(cofpacket *pack)
{
	rofbase->handle_features_request(this, pack);
}


void
cofctrl::packet_out_rcvd(cofpacket *pack)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		size_t len = (pack->length() > 64) ? 64 : pack->length();
		rofbase->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
										pack->soframe(), len);
		return;
	}

	rofbase->handle_packet_out(this, pack);
}


void
cofctrl::flow_mod_rcvd(cofpacket *pack)
{
	WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::flow_mod_rcvd() pack: %s", this, pack->c_str());

	try {
		if (OFPCR_ROLE_SLAVE == role)
		{
			size_t len = (pack->length() > 64) ? 64 : pack->length();
			rofbase->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
											pack->soframe(), len);
			return;
		}

		// check, whether the controlling pack->entity is allowed to install this flow-mod
		if (rofbase->fe_flags.test(crofbase::NSP_ENABLED))
		{
			switch (pack->ofh_flow_mod->command) {
			case OFPFC_ADD:
			case OFPFC_MODIFY:
			case OFPFC_MODIFY_STRICT:
				rofbase->fsptable.flow_mod_allowed(this, pack->match);
				break;
				/*
				 * this allows generic DELETE commands to be applied
				 * FIXME: does this affect somehow entries from other controllers?
				 */
			}
		}


		rofbase->handle_flow_mod(this, pack);


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
			rofbase->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
											pack->soframe(), len);
			return;
		}

		rofbase->handle_group_mod(this, pack);


	} catch (eGroupTableExists& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry already exists, dropping", this);

		rofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_GROUP_EXISTS,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableNotFound& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry not found", this);

		rofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_UNKNOWN_GROUP,
				pack->soframe(), pack->framelen());

	} catch (eGroupEntryInval& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry is invalid", this);

		rofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_INVALID_GROUP,
				pack->soframe(), pack->framelen());

	} catch (eGroupEntryBadType& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry with bad type", this);

		rofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_BAD_TYPE,
				pack->soframe(), pack->framelen());

	} catch (eActionBadOutPort& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry with action with bad type", this);

		rofbase->send_error_message(this, OFPET_BAD_ACTION, OFPBAC_BAD_OUT_PORT,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableLoopDetected& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry produces loop, dropping", this);

		rofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_LOOP,
				pack->soframe(), pack->framelen());

	} catch (eGroupTableModNonExisting& e) {

		WRITELOG(CFWD, ROFL_DBG, "cofctrl(%p)::handle_group_mod() "
				"group entry for modification not found, dropping", this);

		rofbase->send_error_message(this, OFPET_GROUP_MOD_FAILED, OFPGMFC_UNKNOWN_GROUP,
				pack->soframe(), pack->framelen());

	}
}


void
cofctrl::port_mod_rcvd(cofpacket *pack) throw (eOFctrlPortNotFound)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		size_t len = (pack->length() > 64) ? 64 : pack->length();
		rofbase->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
										pack->soframe(), len);
		return;
	}

	uint32_t port_no = be32toh(pack->ofh_port_mod->port_no);

	/*
	 * update cofport structure in fwdelem->phy_ports
	 */
	if (rofbase->phy_ports.find(port_no) == rofbase->phy_ports.end())
	{
		throw eOFctrlPortNotFound();
	}

	rofbase->phy_ports[port_no]->recv_port_mod(
						be32toh(pack->ofh_port_mod->config),
						be32toh(pack->ofh_port_mod->mask),
						be32toh(pack->ofh_port_mod->advertise));

	rofbase->handle_port_mod(this, pack);
}


void
cofctrl::table_mod_rcvd(cofpacket *pack)
{
	if (OFPCR_ROLE_SLAVE == role)
	{
		size_t len = (pack->length() > 64) ? 64 : pack->length();
		rofbase->send_error_message(this, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE,
										pack->soframe(), len);
		return;
	}

	rofbase->handle_table_mod(this, pack);

#if 0
	if (rofbase->flow_tables.find(pack->ofh_table_mod->table_id) != rofbase->flow_tables.end())
	{
		rofbase->flow_tables[pack->ofh_table_mod->table_id]->set_config(
											be32toh(pack->ofh_table_mod->config));
	}
#endif
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
				rofbase->send_error_message(this, OFPET_ROLE_REQUEST_FAILED, OFPRRFC_STALE);
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
			it = rofbase->ofctrl_list.begin(); it != rofbase->ofctrl_list.end(); ++it)
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

	rofbase->handle_role_request(this, pack);
}


void
cofctrl::barrier_request_rcvd(cofpacket *pack)
{
	rofbase->handle_barrier_request(this, pack);
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

					rofbase->fsptable.insert_fsp_entry(this, rexp.match);

					WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_ADD => -ADDED- %s\n%s",
							this, c_str(), rofbase->fsptable.c_str());

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

					rofbase->fsptable.delete_fsp_entry(this, rexp.match, true /*strict*/);

					WRITELOG(COFCTRL, ROFL_DBG, "cofctrl(%p)::experimenter_message_rcvd() "
							"OFPRET_FLOWSPACE => OFPRET_FSP_DELETE => -DELETED- %s\n%s",
							this, c_str(), rofbase->fsptable.c_str());

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
		rofbase->handle_experimenter_message(this, pack);
		break;
	}
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

	info.assign(vas("cofctrl(%p) %s", this, cofbase::cofbase_exists(ctrl)->c_str()));

	return info.c_str();
}



#if 0
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
	rofbase->ftentry_timeout(fte, timeout);
}
#endif

