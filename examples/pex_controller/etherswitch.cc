#include "etherswitch.h"

#include <inttypes.h>

using namespace etherswitch;

ethswitch::ethswitch(rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap) :
		crofbase(versionbitmap),
		dpt(NULL)
{

}



ethswitch::~ethswitch()
{
	// ...
}



void
ethswitch::handle_timeout(int opaque, void *data)
{
	switch (opaque) {
	case ETHSWITCH_TIMER_FLOW_MOD_DELETE_ALL: {
		logging::info << "REMOVING ALL FLOW-MODs" << std::endl;
		dpt->flow_mod_reset();
	} break;
	}

}




void
ethswitch::request_flow_stats()
{
}



void
ethswitch::handle_flow_stats_reply(crofdpt& dpt, rofl::openflow::cofmsg_flow_stats_reply& msg, uint8_t aux_id)
{
}

void ethswitch::handle_table_stats_reply(rofl::crofdpt& dpt, rofl::openflow::cofmsg_table_stats_reply& msg, uint8_t aux_id)
{
	std::cerr << "Received table_stats_reply" << std::endl;
}
void ethswitch::insertRule(crofdpt& dpt, uint32_t port_match, uint32_t port_action)
{

	rofl::openflow::cofflowmod fe(dpt.get_version());

	cfib::get_fib(dpt.get_dpid()).clear();

	this->dpt = &dpt;
	
	switch (dpt.get_version()) {
	case openflow10::OFP_VERSION: {
		fe.set_command(openflow10::OFPFC_ADD);
		fe.set_table_id(0);
				
		fe.match.set_matches().add_match(rofl::openflow::coxmatch_ofb_in_port(port_match));
		fe.instructions.set_inst_apply_actions().get_actions().append_action_output(port_match);

	} break;
	case openflow12::OFP_VERSION: {
		fe.set_command(openflow12::OFPFC_ADD);
		fe.set_table_id(0);
		fe.match.set_matches().add_match(rofl::openflow::coxmatch_ofb_in_port(port_match));
		fe.instructions.set_inst_apply_actions().get_actions().append_action_output(port_action);

	} break;
	case openflow13::OFP_VERSION: {
		fe.set_command(openflow13::OFPFC_ADD);
		fe.set_table_id(0);
		fe.match.set_matches().add_match(rofl::openflow::coxmatch_ofb_in_port(port_match));
		fe.instructions.set_inst_apply_actions().get_actions().append_action_output(port_action);

	} break;
	default:
		throw eBadVersion();
	}

	dpt.send_flow_mod_message(fe);



}

void ethswitch::handle_features_reply(rofl::crofdpt& dpt, rofl::openflow::cofmsg_features_reply& msg, uint8_t aux_id) 
{
	std::cerr << "features_reply" << std::endl;
}

void ethswitch::handle_dpath_open(
		crofdpt& dpt)
{
	std::cerr << "Sending table stats request message" << std::endl;
	dpt.send_table_stats_request(0);

/*	std::cerr << "Sending table features request message" << std::endl;
	dpt.send_table_features_stats_request(0);
*/
	std::cerr << "Inserting rules in the switch" << std::endl;

	insertRule(dpt,1,2);
	insertRule(dpt,2,1);
//	insertRule(dpt,3,1);	

	cfib::get_fib(dpt.get_dpid()).dpt_bind(this, &dpt);
}


void
ethswitch::handle_port_status(rofl::crofdpt& dpt, rofl::openflow::cofmsg_port_status& msg, uint8_t aux_id)
{

}



void
ethswitch::handle_dpath_close(
		crofdpt& dpt)
{
/*	cfib::get_fib(dpt.get_dpid()).dpt_release(this, &dpt);

	logging::info << "[ethsw][dpath-close]" << std::endl << cfib::get_fib(dpt.get_dpid());*/
}



void
ethswitch::handle_packet_in(
		crofdpt& dpt,
		rofl::openflow::cofmsg_packet_in& msg,
		uint8_t aux_id)
{
	std::cerr << "Received pkt IN - not implemented" << std::endl;

}



