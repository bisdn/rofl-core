/*
 * crofctl.cc
 *
 *  Created on: 07.01.2014
 *      Author: andreas
 */

#include "rofl/common/crofctl.h"

using namespace rofl;

uint64_t crofctl::next_ctlid = 0;

/*static*/std::set<crofctl_env*> crofctl_env::rofctl_envs;
/*static*/std::map<cctlid, crofctl*> crofctl::rofctls;

/*static*/crofctl&
crofctl::get_ctl(
		const cctlid& ctlid)
{
	if (crofctl::rofctls.find(ctlid) == crofctl::rofctls.end()) {
		throw eRofCtlNotFound();
	}
	return *(crofctl::rofctls[ctlid]);
}



void
crofctl::init_async_config_role_default_template()
{
	async_config_role_default_template.set_packet_in_mask_master() =
			(1 << rofl::openflow13::OFPR_NO_MATCH) |
			(1 << rofl::openflow13::OFPR_ACTION);

	async_config_role_default_template.set_packet_in_mask_slave(0);

	async_config_role_default_template.set_port_status_mask_master() =
			(1 << rofl::openflow13::OFPPR_ADD) |
			(1 << rofl::openflow13::OFPPR_DELETE) |
			(1 << rofl::openflow13::OFPPR_MODIFY);

	async_config_role_default_template.set_port_status_mask_slave() =
			(1 << rofl::openflow13::OFPPR_ADD) |
			(1 << rofl::openflow13::OFPPR_DELETE) |
			(1 << rofl::openflow13::OFPPR_MODIFY);

	async_config_role_default_template.set_flow_removed_mask_master() =
			(1 << rofl::openflow13::OFPRR_IDLE_TIMEOUT) |
			(1 << rofl::openflow13::OFPRR_HARD_TIMEOUT) |
			(1 << rofl::openflow13::OFPRR_DELETE) |
			(1 << rofl::openflow13::OFPRR_GROUP_DELETE);

	async_config_role_default_template.set_flow_removed_mask_slave(0);
}


