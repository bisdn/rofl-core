/*
 * ccontrol.cpp
 *
 *  Created on: 01.08.2014
 *      Author: andreas
 */

#include "ccontrol.hpp"

using namespace rofl::examples::testomat;

void
ccontrol::handle_dpt_open(
		rofl::crofdpt& dpt)
{
	std::cout << "[ccontrol] dpt open, dpid: " << dpt.get_dpid() << std::endl;

#if 0
	enum rofl::csocket::socket_type_t socket_type = rofl::csocket::SOCKET_TYPE_PLAIN;
	rofl::cparams socket_params = rofl::csocket::get_default_params(socket_type);

	socket_params.set_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string("0.0.0.0");
	socket_params.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string("6633");
	rofl::common::crofshim::add_listening_socket_in4(socket_type, socket_params);

	socket_params.set_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string("0.0.0.0");
	socket_params.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string("6653");
	rofl::common::crofshim::add_listening_socket_in4(socket_type, socket_params);
#endif

	std::cout << "[ccontrol] sending Desc-Stats-Request " << std::endl;
	dpt.send_desc_stats_request(rofl::cauxid(0), 0);
}



void
ccontrol::handle_dpt_close(
		const rofl::cdptid& dptid)
{
	std::cout << "[ccontrol] dpt close, dpid: " << dptid << std::endl;
}



void
ccontrol::handle_desc_stats_reply(
		rofl::crofdpt& dpt, const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_desc_stats_reply& msg)
{
	std::cout << "[ccontrol] rcvd Desc-Stats-Reply: " << std::endl << msg;



	std::cout << "[ccontrol] sending push-vlan flow-mod: " << std::endl;

	rofl::openflow::cofgroupmod gm(dpt.get_version_negotiated());
	gm.set_command(rofl::openflow::OFPGC_ADD);
	gm.set_type(rofl::openflow::OFPGT_ALL);
	gm.set_group_id(1);
#if 1
	gm.set_buckets().add_bucket(0).set_actions().
			add_action_push_vlan(rofl::cindex(0)).set_eth_type(rofl::fvlanframe::VLAN_CTAG_ETHER);
	gm.set_buckets().set_bucket(0).set_actions().
			add_action_set_field(rofl::cindex(1)).set_oxm(rofl::openflow::coxmatch_ofb_vlan_vid(16 | rofl::openflow::OFPVID_PRESENT));
#endif
	gm.set_buckets().set_bucket(0).set_actions().
			add_action_output(rofl::cindex(2)).set_port_no(2);
#if 1
	gm.set_buckets().add_bucket(1).set_actions().
			add_action_push_vlan(rofl::cindex(0)).set_eth_type(rofl::fvlanframe::VLAN_CTAG_ETHER);
	gm.set_buckets().set_bucket(1).set_actions().
			add_action_set_field(rofl::cindex(1)).set_oxm(rofl::openflow::coxmatch_ofb_vlan_vid(16 | rofl::openflow::OFPVID_PRESENT));
#endif
	gm.set_buckets().set_bucket(1).set_actions().
			add_action_output(rofl::cindex(2)).set_port_no(3);

	gm.set_buckets().clear();

	dpt.send_group_mod_message(rofl::cauxid(0), gm);

	rofl::openflow::cofflowmod fm(dpt.get_version_negotiated());
	fm.set_command(rofl::openflow::OFPFC_ADD);
	fm.set_table_id(0);
	fm.set_idle_timeout(0);
	fm.set_hard_timeout(0);
	fm.set_buffer_id(rofl::openflow::OFP_NO_BUFFER);
	fm.set_match().set_in_port(1);
	fm.set_instructions().set_inst_apply_actions().set_actions().
			add_action_group(rofl::cindex(0)).set_group_id(1);

	dpt.send_flow_mod_message(rofl::cauxid(0), fm);


	std::cout << "[ccontrol] sending pop-vlan flow-mod: " << std::endl;

	gm.set_command(rofl::openflow::OFPGC_ADD);
	gm.set_type(rofl::openflow::OFPGT_ALL);
	gm.set_group_id(2);
#if 1
	gm.set_buckets().add_bucket(0).set_actions().
			add_action_pop_vlan(rofl::cindex(0));
#endif
	gm.set_buckets().set_bucket(0).set_actions().
			add_action_output(rofl::cindex(1)).set_port_no(1);
	gm.set_buckets().add_bucket(1).set_actions().
			add_action_output(rofl::cindex(0)).set_port_no(3);

	dpt.send_group_mod_message(rofl::cauxid(0), gm);

	fm.clear();
	fm.set_command(rofl::openflow::OFPFC_ADD);
	fm.set_table_id(0);
	fm.set_idle_timeout(0);
	fm.set_hard_timeout(0);
	fm.set_buffer_id(rofl::openflow::OFP_NO_BUFFER);
	fm.set_match().set_in_port(2);
	fm.set_instructions().set_inst_apply_actions().set_actions().
			add_action_group(rofl::cindex(0)).set_group_id(2);
	fm.set_instructions().set_inst_write_actions().set_actions().
			add_action_output(rofl::cindex(1)).set_port_no(rofl::openflow::OFPP_TABLE);

	dpt.send_flow_mod_message(rofl::cauxid(0), fm);
}



