/*
 * ccontrol.cpp
 *
 *  Created on: 01.08.2014
 *      Author: andreas
 */

#include "ccontrol.hpp"

using namespace testomat;

void
ccontrol::handle_dpt_open(
		rofl::crofdpt& dpt)
{
	std::cout << "[ccontrol] dpt open, dpid: " << dpt.get_dpid_s() << std::endl;

	std::cout << "[ccontrol] sending Desc-Stats-Request " << std::endl;
	dpt.send_desc_stats_request(rofl::cauxid(0), 0);
}



void
ccontrol::handle_dpt_close(
		rofl::crofdpt& dpt)
{
	std::cout << "[ccontrol] dpt close, dpid: " << dpt.get_dpid_s() << std::endl;
}



void
ccontrol::handle_desc_stats_reply(
		rofl::crofdpt& dpt, const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_desc_stats_reply& msg)
{
	std::cout << "[ccontrol] rcvd Desc-Stats-Reply: " << std::endl << msg;

	rofl::openflow::cofflowmod fm(dpt.get_version());
	fm.set_command(rofl::openflow::OFPFC_ADD);
	fm.set_table_id(0);
	fm.set_idle_timeout(0);
	fm.set_hard_timeout(0);
	fm.set_buffer_id(rofl::openflow::OFP_NO_BUFFER);
	fm.set_match().set_in_port(1);
	rofl::cindex index(0);
	fm.set_instructions().set_inst_apply_actions().set_actions().
			add_action_push_vlan(index++).set_eth_type(rofl::fvlanframe::VLAN_CTAG_ETHER);
	fm.set_instructions().set_inst_apply_actions().set_actions().
			add_action_set_field(index++).set_oxm(rofl::openflow::coxmatch_ofb_vlan_vid(16 | rofl::openflow::OFPVID_PRESENT));
	fm.set_instructions().set_inst_apply_actions().set_actions().
			add_action_output(index++).set_port_no(2);

	dpt.send_flow_mod_message(rofl::cauxid(0), fm);

	std::cout << "[ccontrol] sending push-vlan flow-mod: " << std::endl << msg;

	fm.clear();
	fm.set_command(rofl::openflow::OFPFC_ADD);
	fm.set_table_id(0);
	fm.set_idle_timeout(0);
	fm.set_hard_timeout(0);
	fm.set_buffer_id(rofl::openflow::OFP_NO_BUFFER);
	fm.set_match().set_in_port(2);
	rofl::cindex jndex(0);
	fm.set_instructions().set_inst_apply_actions().set_actions().
			add_action_pop_vlan(jndex++);
	fm.set_instructions().set_inst_apply_actions().set_actions().
			add_action_output(jndex++).set_port_no(1);

	dpt.send_flow_mod_message(rofl::cauxid(0), fm);

	std::cout << "[ccontrol] sending pop-vlan flow-mod: " << std::endl << msg;
}



