#include "etherswitch.h"
#include <inttypes.h>

using namespace etherswitch;

//Uncomment this to enable periodic request of stats for all datapaths
//#define REQUEST_STATS 1

//Config defines
#define IDLE_TIMEOUT 60
#define HARD_TIMEOUT 0
#define DEFAULT_PRIORITY 8000
	
ethswitch::ethswitch(rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap) :
		crofbase(versionbitmap)
{

}



ethswitch::~ethswitch()
{
}

void
ethswitch::request_flow_stats()
{
#ifdef REQUEST_STATS
	std::map<crofdpt*, std::map<uint16_t, std::map<cmacaddr, struct fibentry_t> > >::iterator it;

	for (it = fib.begin(); it != fib.end(); ++it) {
		crofdpt *dpt = it->first;

		cofflow_stats_request req;
		
		//Set version	
		req.set_version(dpt->get_version());
		req.set_table_id(OFPTT_ALL);
		req.set_match(cofmatch(dpt->get_version()));
		
		send_flow_stats_request(dpt, /*flags=*/0, req);
	}

	register_timer(ETHSWITCH_TIMER_FLOW_STATS, flow_stats_timeout);
#endif
}

void
ethswitch::handle_flow_stats_reply(crofdpt& dpt, const cauxid& auxid, rofl::openflow::cofmsg_flow_stats_reply& msg)
{
	logging::info << "Received flow-stats request:" << msg << std::endl;
}

/*
* Methods inherited from crofbase
*/ 

void
ethswitch::handle_dpt_open(crofdpt& dpt)
{

	//New connection => cleanup the RIB
	cfib::get_fib(dpt.get_dptid()).clear();

	//Remove all flows in the table
	dpt.flow_mod_reset();
	
	//Remove all groupmods
	if(dpt.get_version())
		dpt.group_mod_reset();

#if REQUEST_STATS
	//TODO: fix
#endif


	//For 1.3 set the table 0 catch-all flow-mod 
	if(dpt.get_version() == openflow13::OFP_VERSION) {
		cindex index;

		//Catch all flowmod
		rofl::openflow::cofflowmod fe_all(dpt.get_version());
		fe_all.set_priority(0); //lowest priority+1
	
		//Command
		fe_all.set_command(openflow::OFPFC_ADD);

		//Action
		fe_all.set_instructions().set_inst_apply_actions().set_actions().
			add_action_output(index++).set_port_no(rofl::openflow::OFPP_CONTROLLER);
	
		//Send the flowmod
		dpt.send_flow_mod_message(cauxid(0), fe_all);
	}

	//Construct the ETH_TYPE ARP capture flowmod
	cindex index;
	rofl::openflow::cofflowmod fe(dpt.get_version());
	
	//This is not necessary, it is done automatically by the constructor
	//fe.set_buffer_id(rofl::openflow::OFP_NO_BUFFER);
	//fe.set_table_id(0);
	fe.set_priority(1); //lowest priority+1

	//Set command	
	fe.set_command(openflow::OFPFC_ADD);
	fe.set_match().set_matches().add_match(rofl::openflow::coxmatch_ofb_eth_type(farpv4frame::ARPV4_ETHER));

	//Now add action
	//OF1.0 has no instructions, so the code here differs
	switch (dpt.get_version()) {
		case openflow10::OFP_VERSION:
			fe.set_actions().add_action_output(index++).set_port_no(rofl::openflow::OFPP_CONTROLLER);
			break;
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: 
			fe.set_instructions().set_inst_apply_actions().set_actions().
			add_action_output(index++).set_port_no(rofl::openflow::OFPP_CONTROLLER);
			break;
		default:
			assert(0);
			throw eBadVersion();
	}

	//Send the flowmod
	dpt.send_flow_mod_message(cauxid(0), fe);

	//Add the flowmod to the RIB
	cfib::get_fib(dpt.get_dptid()).dpt_bind(this, &dpt);
}



void
ethswitch::handle_dpt_close(
		crofdpt& dpt)
{
	cfib::get_fib(dpt.get_dptid()).dpt_release(this, &dpt);

	logging::info << "[ethsw][dpath-close]" << std::endl << cfib::get_fib(dpt.get_dptid());
}

/*
* End of methods inherited from crofbase
*/ 



void
ethswitch::dump_packet_in(
		crofdpt& dpt,
		openflow::cofmsg_packet_in& msg,
		cmacaddr& eth_src,
		cmacaddr& eth_dst)
{
	//Dump some information
	logging::info << "[ethsw][packet-in] PACKET-IN => frame seen, "
						<< "buffer-id:0x" << std::hex << msg.get_buffer_id() << std::dec << " "
						<< "eth-src:" << msg.set_packet().ether()->get_dl_src() << " "
						<< "eth-dst:" << msg.set_packet().ether()->get_dl_dst() << " "
						<< "eth-type:0x" << std::hex << msg.set_packet().ether()->get_dl_type() << std::dec << " "
						<< std::endl;
	logging::info << dpt.get_dptid();

}

void
ethswitch::install_drop_flowmod(
		crofdpt& dpt,
		const cauxid& auxid,
		rofl::openflow::cofmsg_packet_in& msg)
{
	
	/**
	* We want to drop all packets from this
	*/

	//Create flowmod and assign command
	rofl::openflow::cofflowmod fe(dpt.get_version());
	fe.set_command(openflow::OFPFC_ADD);

	//Set the basic parameters
	fe.set_buffer_id(msg.get_buffer_id());
	fe.set_idle_timeout(IDLE_TIMEOUT);
	fe.set_hard_timeout(HARD_TIMEOUT);
	fe.set_table_id(msg.get_table_id());
	fe.set_flags(rofl::openflow::OFPFF_SEND_FLOW_REM /*| rofl::openflow::OFPFF_CHECK_OVERLAP*/);

	//Matches
	fe.set_match().set_matches().add_match(msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT));
	fe.set_match().set_eth_dst(msg.set_packet().ether()->get_dl_dst());
	fe.set_match().set_eth_type(msg.set_match().get_eth_type());

	//Instruction with empty actions => DROP
	fe.set_instructions().add_inst_apply_actions();

	logging::info << "[ethsw][packet-in] installing new DROP Flow-Mod entry:" << std::endl << fe;

	//Install
	dpt.send_flow_mod_message(auxid, fe);
}

void
ethswitch::install_flood_flowmod(
		crofdpt& dpt,
		const cauxid& auxid,
		rofl::openflow::cofmsg_packet_in& msg)
{
	//Create flowmod and assign command
	rofl::openflow::cofflowmod fe(dpt.get_version());
	rofl::cindex index;
	fe.set_command(openflow::OFPFC_ADD);
	fe.set_flags(rofl::openflow::OFPFF_SEND_FLOW_REM /*| rofl::openflow::OFPFF_CHECK_OVERLAP*/);

	//Set the basic parameters
	fe.set_buffer_id(msg.get_buffer_id());
	fe.set_idle_timeout(IDLE_TIMEOUT);
	fe.set_hard_timeout(HARD_TIMEOUT);
	fe.set_priority(2);
	fe.set_table_id(msg.get_table_id());

	//Set matches
	fe.set_match().set_in_port(msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT).get_u32value());
	fe.set_match().set_eth_dst(msg.set_packet().ether()->get_dl_dst());
	fe.set_match().set_eth_type(msg.set_match().get_eth_type());

	//Action flood
	fe.set_instructions().add_inst_apply_actions();
	fe.set_instructions().set_inst_apply_actions().set_actions().add_action_output(index++).set_port_no(crofbase::get_ofp_flood_port(dpt.get_version()));

	logging::info << "[ethsw][packet-in] installing new FLOOD Flow-Mod entry:" << std::endl << fe;

	//Install flowmod
	dpt.send_flow_mod_message(auxid, fe);

}

rofl_result_t
ethswitch::install_fwd_flowmod(
		crofdpt& dpt,
		const cauxid& auxid,
		rofl::openflow::cofmsg_packet_in& msg,
		rofl::cmacaddr& eth_src,
		rofl::cmacaddr& eth_dst)
{

	//Check the RIB and insert if found
	try {
		cfibentry& entry = cfib::get_fib(dpt.get_dptid()).fib_lookup(
					this,
					dpt,
					msg.set_packet().ether()->get_dl_dst(),
					msg.set_packet().ether()->get_dl_src(),
					msg.set_match().get_in_port());


		if (msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT).get_u32value() == entry.get_out_port_no()) {
			indent i(2);
			rofl::logging::debug << "[ethsw][packet-in] found entry for eth-dst:" << eth_dst
					<< ", but in-port == out-port, ignoring" << std::endl << entry;

			return ROFL_FAILURE;
		}

		//Create flowmod
		rofl::openflow::cofflowmod fe(dpt.get_version());
		rofl::cindex index;
		fe.set_command(rofl::openflow12::OFPFC_ADD);
		
		//Set default properties
		fe.set_table_id(msg.get_table_id());
		fe.set_idle_timeout(IDLE_TIMEOUT);
		fe.set_hard_timeout(HARD_TIMEOUT);
		fe.set_priority(DEFAULT_PRIORITY);
		fe.set_buffer_id(msg.get_buffer_id());
		fe.set_flags(rofl::openflow::OFPFF_SEND_FLOW_REM /*| rofl::openflow::OFPFF_CHECK_OVERLAP*/);

		//Set match
		fe.set_match().set_eth_dst(eth_dst);
		fe.set_match().set_eth_src(eth_src);

		//Set actions
		fe.set_instructions().add_inst_apply_actions().set_actions().
				add_action_output(index++).set_port_no(entry.get_out_port_no());

		indent i(2); rofl::logging::debug << "[ethsw][packet-in] installing flow mod" << std::endl << fe;

		//Install flowmod
		dpt.send_flow_mod_message(auxid, fe);

		return ROFL_SUCCESS;

	} catch (eFibInval& e) {
		rofl::logging::debug << "[ethsw][packet-in] eFibInval, ignoring." << std::endl << cfib::get_fib(dpt.get_dptid());
	} catch (eFibNotFound& e) {
		rofl::logging::debug << "[ethsw][packet-in] dst:" << eth_dst << " NOT FOUND in FIB, PACKET-OUT flooding." << std::endl;
	}
	
	return ROFL_FAILURE;

}

void
ethswitch::handle_packet_in(
		crofdpt& dpt,
		const cauxid& auxid,
		rofl::openflow::cofmsg_packet_in& msg)
{

	try {

		//Use ROFL-common classifier (do not use PKT_IN matches)
		msg.set_packet().classify(msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT).get_u32value());
		cmacaddr eth_src = msg.set_packet().ether()->get_dl_src();
		cmacaddr eth_dst = msg.set_packet().ether()->get_dl_dst();

		//Dump the pkt info
		dump_packet_in(dpt, msg, eth_src, eth_dst);

		//Ignore multi-cast frames (SRC)
		if (eth_src.is_multicast()) {
			logging::warn << "[ethsw][packet-in] eth-src:" << eth_src << " is multicast, ignoring." << std::endl;
			return;
		}

//
//This block is really optional, just to show how the framework works
//
		/*
		 * block mac address 01:80:c2:00:00:00
		 */
		if (msg.set_packet().ether()->get_dl_dst() == cmacaddr("01:80:c2:00:00:00") ||
			msg.set_packet().ether()->get_dl_dst() == cmacaddr("01:00:5e:00:00:fb")) {

			//Install dropping rule and return
			install_drop_flowmod(dpt, auxid, msg);
			return;
		}


		//Update RIB (learn)
		cfib::get_fib(dpt.get_dptid()).fib_update(this,
								dpt,
								msg.set_packet().ether()->get_dl_src(),
								msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT).get_u32value());


		/*
		 * Treat flood mac addresses specially 33:33:00:00:00:02 and ff:ff:ff:ff:ff:ff
		 */
		if ((msg.set_packet().ether()->get_dl_dst() == cmacaddr("33:33:00:00:00:02")) ||
			msg.set_packet().ether()->get_dl_dst() == cmacaddr("ff:ff:ff:ff:ff:ff")) {
		
			//Install flowmod and return 
			install_flood_flowmod(dpt, auxid, msg);
			return;
		}
//
// End of optional block
//

		//Check if is multicast (dst) otherwise try to install flowmod
		if (eth_dst.is_multicast()) {
			rofl::logging::debug << "[ethsw][packet-in] eth-dst:" << eth_dst << " is multicast, PACKET-OUT flooding" << std::endl;
		} else {
			//Install flowmod and return if successful
			if( install_fwd_flowmod(dpt, auxid, msg, eth_src, eth_dst) == ROFL_SUCCESS)
				return;
		}

		/*
		* No RIB entry => PKT_OUT
		*/
		rofl::openflow::cofactions actions(dpt.get_version());
		rofl::cindex index;

		//Port number
		uint32_t ofp_no_buffer = crofbase::get_ofp_no_buffer(dpt.get_version());

		//Add actions
		actions.add_action_output(index).set_port_no(crofbase::get_ofp_flood_port(dpt.get_version()));

		if (ofp_no_buffer != msg.get_buffer_id()) {
			dpt.send_packet_out_message(auxid, msg.get_buffer_id(), msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT).get_u32value(), actions);
		} else {
			dpt.send_packet_out_message(auxid, msg.get_buffer_id(), msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT).get_u32value(), actions,
					msg.set_packet().soframe(), msg.set_packet().framelen());
		}

	} catch (...) {
		logging::error << "[ethsw][packet-in] caught some exception, use debugger for getting more info" << std::endl << msg;
		assert(0);
	}
}



