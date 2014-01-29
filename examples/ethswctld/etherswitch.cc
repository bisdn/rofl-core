#include "etherswitch.h"

#include <inttypes.h>

using namespace etherswitch;

ethswitch::ethswitch(cofhello_elem_versionbitmap const& versionbitmap) :
		crofbase(versionbitmap)
{

}



ethswitch::~ethswitch()
{
	// ...
}



void
ethswitch::handle_timeout(int opaque, void *data)
{

}




void
ethswitch::request_flow_stats()
{
#if 0
	std::map<crofdpt*, std::map<uint16_t, std::map<cmacaddr, struct fibentry_t> > >::iterator it;

	for (it = fib.begin(); it != fib.end(); ++it) {
		crofdpt *dpt = it->first;

		cofflow_stats_request req;

		switch (dpt->get_version()) {
		case OFP10_VERSION: {
			req.set_version(dpt->get_version());
			req.set_table_id(OFPTT_ALL);
			req.set_match(cofmatch(OFP10_VERSION));
			req.set_out_port(OFPP_ANY);
		} break;
		case OFP12_VERSION: {
			req.set_version(dpt->get_version());
			req.set_table_id(OFPTT_ALL);
			cofmatch match(OFP12_VERSION);
			//match.set_eth_dst(cmacaddr("01:80:c2:00:00:00"));
			req.set_match(match);
			req.set_out_port(OFPP_ANY);
			req.set_out_group(OFPG_ANY);
			req.set_cookie(0);
			req.set_cookie_mask(0);
		} break;
		default: {
			// do nothing
		} break;
		}

		fprintf(stderr, "etherswitch: calling FLOW-STATS-REQUEST for dpid: 0x%"PRIu64"\n",
				dpt->get_dpid());

		send_flow_stats_request(dpt, /*flags=*/0, req);
	}

	register_timer(ETHSWITCH_TIMER_FLOW_STATS, flow_stats_timeout);
#endif
}



void
ethswitch::handle_flow_stats_reply(crofdpt& dpt, cofmsg_flow_stats_reply& msg, uint8_t aux_id)
{
#if 0
	if (fib.find(dpt) == fib.end()) {
		delete msg; return;
	}

	std::vector<cofflow_stats_reply>& replies = msg->get_flow_stats();

	std::vector<cofflow_stats_reply>::iterator it;
	for (it = replies.begin(); it != replies.end(); ++it) {
		switch (it->get_version()) {
		case OFP10_VERSION: {
			fprintf(stderr, "FLOW-STATS-REPLY:\n  match: %s\n  actions: %s\n",
					it->get_match().c_str(), it->get_actions().c_str());
		} break;
		case OFP12_VERSION: {
			fprintf(stderr, "FLOW-STATS-REPLY:\n  match: %s\n  instructions: %s\n",
					it->get_match().c_str(), it->get_instructions().c_str());
		} break;
		default: {
			// do nothing
		} break;
		}
	}
#endif
}





void
ethswitch::handle_dpath_open(
		crofdpt& dpt)
{
	dpt.flow_mod_reset();

	rofl::cflowentry fe(dpt.get_version());

	switch (dpt.get_version()) {
	case openflow10::OFP_VERSION: {
		fe.set_command(openflow10::OFPFC_ADD);
		fe.set_table_id(0);
		fe.instructions.set_inst_apply_actions().get_actions().append_action_output(openflow10::OFPP_CONTROLLER);
		fe.match.set_eth_type(farpv4frame::ARPV4_ETHER);

	} break;
	case openflow12::OFP_VERSION: {
		fe.set_command(openflow12::OFPFC_ADD);
		fe.set_table_id(0);
		fe.instructions.set_inst_apply_actions().get_actions().append_action_output(openflow12::OFPP_CONTROLLER);
#if 0
		fe.instructions.set_inst_write_actions().get_actions().append_action_push_mpls(fmplsframe::MPLS_ETHER);
		fe.instructions.set_inst_write_actions().get_actions().append_action_set_field(coxmatch_ofb_mpls_label(0x12345678));
		fe.instructions.set_inst_write_actions().get_actions().append_action_set_field(coxmatch_ofb_mpls_tc(7));
		fe.instructions.set_inst_write_actions().get_actions().append_action_push_vlan(fvlanframe::VLAN_STAG_ETHER);
		fe.instructions.set_inst_write_actions().get_actions().append_action_set_field(coxmatch_ofb_vlan_vid(coxmatch_ofb_vlan_vid::VLAN_TAG_MODE_NORMAL, 4));
		fe.instructions.set_inst_write_actions().get_actions().append_action_set_field(coxmatch_ofb_vlan_pcp(3));
		fe.match.set_eth_type(farpv4frame::ARPV4_ETHER);
		fe.match.set_eth_dst(cmacaddr("00:11:22:33:44:55"));
		fe.instructions.set_inst_goto_table().set_table_id(4);

		cgroupentry ge(openflow12::OFP_VERSION);
		ge.set_command(openflow12::OFPFC_ADD);
		ge.set_group_id(1111);
		ge.set_type(openflow12::OFPGT_ALL);
		ge.buckets.append_bucket(cofbucket(openflow12::OFP_VERSION, 1, 2, 3));
		ge.buckets.back().get_actions().append_action_output(5);
		ge.buckets.back().get_actions().append_action_push_mpls(fmplsframe::MPLS_ETHER);
		ge.buckets.back().get_actions().append_action_set_field(coxmatch_ofb_mpls_label(0x55555555));
		ge.buckets.back().get_actions().append_action_set_field(coxmatch_ofb_mpls_tc(7));

		std::cerr << ge << std::endl;
#endif
	} break;
	case openflow13::OFP_VERSION: {
		fe.set_command(openflow13::OFPFC_ADD);
		fe.set_table_id(0);
		fe.instructions.set_inst_apply_actions().get_actions().append_action_output(openflow13::OFPP_CONTROLLER);
		fe.match.set_eth_type(farpv4frame::ARPV4_ETHER);

	} break;
	default:
		throw eBadVersion();
	}

	dpt.send_flow_mod_message(fe);

	cfib::get_fib(dpt.get_dpid()).dpt_bind(this, &dpt);
}



void
ethswitch::handle_dpath_close(
		crofdpt& dpt)
{
	cfib::get_fib(dpt.get_dpid()).dpt_release(this, &dpt);
}



void
ethswitch::handle_packet_in(
		crofdpt& dpt,
		cofmsg_packet_in& msg,
		uint8_t aux_id)
{
	cmacaddr eth_src = msg.get_packet().ether()->get_dl_src();
	cmacaddr eth_dst = msg.get_packet().ether()->get_dl_dst();

	/*
	 * sanity check: if source mac is multicast => invalid frame
	 */
	if (eth_src.is_multicast()) {
		return;
	}

	/*
	 * block mac address 01:80:c2:00:00:00
	 */
	if (msg.get_packet().ether()->get_dl_dst() == cmacaddr("01:80:c2:00:00:00") ||
		msg.get_packet().ether()->get_dl_dst() == cmacaddr("01:00:5e:00:00:fb")) {
		cflowentry fe(dpt.get_version());

		switch (dpt.get_version()) {
		case openflow10::OFP_VERSION: fe.set_command(openflow10::OFPFC_ADD); break;
		case openflow12::OFP_VERSION: fe.set_command(openflow12::OFPFC_ADD); break;
		case openflow13::OFP_VERSION: fe.set_command(openflow13::OFPFC_ADD); break;
		default:
			throw eBadVersion();
		}

		fe.set_buffer_id(msg.get_buffer_id());
		fe.set_idle_timeout(15);
		fe.set_table_id(msg.get_table_id());

		fe.match.set_in_port(msg.get_match().get_in_port());
		fe.match.set_eth_dst(msg.get_packet().ether()->get_dl_dst());
		fe.instructions.add_inst_apply_actions();

		fprintf(stderr, "etherswitch: installing FLOW-MOD with entry: ");
		std::cerr << fe << std::endl;

		dpt.send_flow_mod_message(fe);

		return;
	}

	fprintf(stderr, "etherswitch: PACKET-IN from dpid:0x%"PRIu64" buffer-id:0x%x => from %s to %s type: 0x%x\n",
			dpt.get_dpid(),
			msg.get_buffer_id(),
			msg.get_packet().ether()->get_dl_src().c_str(),
			msg.get_packet().ether()->get_dl_dst().c_str(),
			msg.get_packet().ether()->get_dl_type());


	cofactions actions(dpt.get_version());

	cfib::get_fib(dpt.get_dpid()).fib_update(
							this,
							dpt,
							msg.get_packet().ether()->get_dl_src(),
							msg.get_match().get_in_port());

	if (eth_dst.is_multicast()) {

		switch (dpt.get_version()) {
		case openflow10::OFP_VERSION:
			actions.append_action_output(openflow10::OFPP_FLOOD); break;
		case openflow12::OFP_VERSION:
			actions.append_action_output(openflow12::OFPP_FLOOD); break;
		case openflow13::OFP_VERSION:
			actions.append_action_output(openflow13::OFPP_FLOOD); break;
		default:
			throw eBadVersion();
		}

	} else {

		try {
			cfibentry& entry = cfib::get_fib(dpt.get_dpid()).fib_lookup(
						this,
						dpt,
						msg.get_packet().ether()->get_dl_dst(),
						msg.get_packet().ether()->get_dl_src(),
						msg.get_match().get_in_port());

			if (msg.get_match().get_in_port() == entry.get_out_port_no()) {
				return;
			}

			cflowentry fe(dpt.get_version());

			fe.set_command(rofl::openflow12::OFPFC_ADD);
			fe.set_table_id(0);
			fe.set_idle_timeout(10);
			fe.set_priority(0x8000);
			fe.set_buffer_id(msg.get_buffer_id());
			fe.set_flags(rofl::openflow12::OFPFF_SEND_FLOW_REM);

			fe.match.set_eth_dst(eth_dst);
			fe.match.set_eth_src(eth_src);

			fe.instructions.add_inst_apply_actions().get_actions().append_action_output(entry.get_out_port_no());

			dpt.send_flow_mod_message(fe);

			return;

		} catch (eFibNotFound& e) {
			switch (dpt.get_version()) {
			case openflow10::OFP_VERSION:
				actions.append_action_output(openflow10::OFPP_FLOOD); break;
			case openflow12::OFP_VERSION:
				actions.append_action_output(openflow12::OFPP_FLOOD); break;
			case openflow13::OFP_VERSION:
				actions.append_action_output(openflow13::OFPP_FLOOD); break;
			default:
				throw eBadVersion();
			}

		}
	}

	uint32_t ofp_no_buffer = 0;
	switch (dpt.get_version()) {
	case openflow10::OFP_VERSION: ofp_no_buffer = openflow10::OFP_NO_BUFFER; break;
	case openflow12::OFP_VERSION: ofp_no_buffer = openflow12::OFP_NO_BUFFER; break;
	case openflow13::OFP_VERSION: ofp_no_buffer = openflow13::OFP_NO_BUFFER; break;
	default:
		throw eBadVersion();
	}

	if (ofp_no_buffer != msg.get_buffer_id()) {
		dpt.send_packet_out_message(msg.get_buffer_id(), msg.get_match().get_in_port(), actions);
	} else {
		dpt.send_packet_out_message(msg.get_buffer_id(), msg.get_match().get_in_port(), actions,
				msg.get_packet().soframe(), msg.get_packet().framelen());
	}
}



