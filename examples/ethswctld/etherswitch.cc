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
		crofbase(versionbitmap),
		dump_fib_interval(DUMP_FIB_DEFAULT_INTERNAL)
{
	//register timer for dumping ethswitch's internal state
	timer_id_dump_fib = register_timer(TIMER_DUMP_FIB, rofl::ctimespec(dump_fib_interval));
}



ethswitch::~ethswitch()
{
}



void
ethswitch::handle_timeout(int opaque, void* data)
{
	switch (opaque) {
	case TIMER_DUMP_FIB: {
		rofl::logging::notice << "****************************************" << std::endl;
		rofl::logging::notice << *this;
		rofl::logging::notice << "****************************************" << std::endl;
		//re-register timer for next round
		timer_id_dump_fib = register_timer(TIMER_DUMP_FIB, rofl::ctimespec(dump_fib_interval));
	} break;
	default: {
		assert(0);
	};
	}
}


void
ethswitch::request_flow_stats()
{
#ifdef REQUEST_STATS
	std::map<crofdpt*, std::map<uint16_t, std::map<rofl::caddress_ll, struct fibentry_t> > >::iterator it;

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
ethswitch::handle_flow_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_flow_stats_reply& msg)
{
	rofl::logging::info << "Received flow-stats request:" << msg << std::endl;
}

/*
* Methods inherited from crofbase
*/ 

void
ethswitch::handle_dpt_open(rofl::crofdpt& dpt)
{
	dptid = dpt.get_dptid();

	//New connection => cleanup the RIB
	cfibtable::get_fib(dpt.get_dptid()).clear();

	//Remove all flows in the table
	dpt.flow_mod_reset();
	
	//Remove all groupmods
	if(dpt.get_version() > rofl::openflow10::OFP_VERSION) {
		dpt.group_mod_reset();
	}

#if REQUEST_STATS
	//TODO: fix
#endif

	//Construct the ETH_TYPE ARP capture flowmod
	rofl::openflow::cofflowmod fe(dpt.get_version());
	
	//This is not necessary, it is done automatically by the constructor
	//fe.set_buffer_id(rofl::openflow::OFP_NO_BUFFER);
	//fe.set_table_id(0);
	fe.set_priority(0);

	//Set command	
	fe.set_command(rofl::openflow::OFPFC_ADD);

	//Now add action
	//OF1.0 has no instructions, so the code here differs
	switch (dpt.get_version()) {
		case rofl::openflow10::OFP_VERSION:
			fe.set_actions().
					add_action_output(rofl::cindex(0)).set_port_no(rofl::openflow::OFPP_CONTROLLER);
			break;
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION:
			fe.set_instructions().set_inst_apply_actions().set_actions().
					add_action_output(rofl::cindex(0)).set_port_no(rofl::openflow::OFPP_CONTROLLER);
			break;
		default:
			assert(0);
			throw rofl::eBadVersion();
	}

	//Send the flowmod
	dpt.send_flow_mod_message(rofl::cauxid(0), fe);
}



void
ethswitch::handle_dpt_close(
		rofl::crofdpt& dpt)
{
	rofl::logging::info << "[ethsw][dpath-close]" << std::endl << cfibtable::get_fib(dpt.get_dptid());
}

/*
* End of methods inherited from crofbase
*/ 



void
ethswitch::dump_packet_in(
		rofl::crofdpt& dpt,
		rofl::openflow::cofmsg_packet_in& msg)
{
	//Dump some information
	rofl::logging::info << "[ethsw][packet-in] PACKET-IN => frame seen, "
						<< "buffer-id:0x" << std::hex << msg.get_buffer_id() << std::dec << " "
						<< "eth-src:" << msg.set_packet().ether()->get_dl_src() << " "
						<< "eth-dst:" << msg.set_packet().ether()->get_dl_dst() << " "
						<< "eth-type:0x" << std::hex << msg.set_packet().ether()->get_dl_type() << std::dec << " "
						<< std::endl;
	rofl::logging::info << dpt.get_dpid_s();

}

void
ethswitch::handle_packet_in(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_packet_in& msg)
{
	try {
		cfibtable& fib = cfibtable::get_fib(dpt.get_dptid());
		cflowtable& ftb = cflowtable::get_flowtable(dpt.get_dptid());

		const rofl::caddress_ll& eth_src = msg.get_match().get_eth_src();
		const rofl::caddress_ll& eth_dst = msg.set_match().get_eth_dst();
		uint32_t in_port = msg.get_match().get_in_port();

		//Ignore multi-cast frames (SRC)
		if (eth_src.is_multicast()) {
			rofl::logging::warn << "[ethsw][packet-in] eth-src:" << eth_src << " is multicast, ignoring." << std::endl;
			return;
		}

		//SRC and DST are unicast addresses => Update RIB: learn the source address and its associated portno
		fib.set_fib_entry(eth_src, in_port).set_port_no(in_port);

		//Drop frames destined to 01:80:c2:00:00:00
		if (eth_dst == rofl::caddress_ll("01:80:c2:00:00:00")) {
			dpt.drop_buffer(auxid, msg.get_buffer_id());
			return;
		}

		//Dump the pkt info
		dump_packet_in(dpt, msg);

		//Flood multicast and yet unknown frames (DST)
		if (eth_dst.is_multicast() || (not fib.has_fib_entry(eth_dst))) {
			rofl::openflow::cofactions actions(dpt.get_version());
			actions.add_action_output(rofl::cindex(0)).set_port_no(rofl::crofbase::get_ofp_flood_port(dpt.get_version()));
			dpt.send_packet_out_message(auxid, msg.get_buffer_id(), msg.get_match().get_in_port(), actions);
			return;
		}

		//SRC and DST are unicast addresses => Create flow entry on data path
		if (fib.has_fib_entry(eth_dst)) {
			ftb.set_flow_entry(eth_src, eth_dst, fib.get_fib_entry(eth_dst).get_port_no());

			if (rofl::openflow::OFP_NO_BUFFER != msg.get_buffer_id()) {
				rofl::openflow::cofactions actions(dpt.get_version());
				actions.add_action_output(rofl::cindex(0)).set_port_no(fib.get_fib_entry(eth_dst).get_port_no());
				dpt.send_packet_out_message(auxid, msg.get_buffer_id(), msg.get_match().get_in_port(), actions);
			}
		}


	} catch (...) {
		rofl::logging::error << "[ethsw][packet-in] caught some exception, use debugger for getting more info" << std::endl << msg;
		assert(0);
		throw;
	}
}



