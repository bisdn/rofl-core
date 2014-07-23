/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
ethswitch::handle_flow_stats_reply(crofdpt& dpt, const cauxid& auxid, rofl::openflow::cofmsg_flow_stats_reply& msg)
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
					it->set_match().c_str(), it->get_actions().c_str());
		} break;
		case OFP12_VERSION: {
			fprintf(stderr, "FLOW-STATS-REPLY:\n  match: %s\n  instructions: %s\n",
					it->set_match().c_str(), it->get_instructions().c_str());
		} break;
		default: {
			// do nothing
		} break;
		}
	}
#endif
}





void
ethswitch::handle_dpt_open(
		crofdpt& dpt)
{
	dpt.flow_mod_reset();

	rofl::openflow::cofflowmod fe(dpt.get_version());
	fe.set_buffer_id(rofl::openflow::OFP_NO_BUFFER);
	cfib::get_fib(dpt.get_dptid()).clear();

	dpt.flow_mod_reset();
	dpt.group_mod_reset();

	this->dpt = &dpt;

	//register_timer(ETHSWITCH_TIMER_FLOW_MOD_DELETE_ALL, 30);

	cindex index;

	switch (dpt.get_version()) {
	case openflow10::OFP_VERSION: {
		fe.set_command(openflow10::OFPFC_ADD);
		fe.set_table_id(0);
		fe.set_match().set_matches().add_match(rofl::openflow::coxmatch_ofb_eth_type(farpv4frame::ARPV4_ETHER));
		fe.set_actions().add_action_output(index++).set_port_no(rofl::openflow10::OFPP_CONTROLLER);

	} break;
	case openflow12::OFP_VERSION: {
		fe.set_command(openflow12::OFPFC_ADD);
		fe.set_table_id(0);
		fe.set_match().set_matches().add_match(rofl::openflow::coxmatch_ofb_eth_type(farpv4frame::ARPV4_ETHER));
		fe.set_instructions().set_inst_apply_actions().set_actions().
				add_action_output(index++).set_port_no(rofl::openflow12::OFPP_CONTROLLER);

	} break;
	case openflow13::OFP_VERSION: {
		fe.set_command(openflow13::OFPFC_ADD);
		fe.set_table_id(0);
		fe.set_match().set_matches().add_match(rofl::openflow::coxmatch_ofb_eth_type(farpv4frame::ARPV4_ETHER));
		fe.set_instructions().set_inst_apply_actions().set_actions().
				add_action_output(index++).set_port_no(rofl::openflow13::OFPP_CONTROLLER);

	} break;
	default:
		throw eBadVersion();
	}

	dpt.send_flow_mod_message(cauxid(0), fe);

	cfib::get_fib(dpt.get_dptid()).dpt_bind(this, &dpt);
}



void
ethswitch::handle_dpt_close(
		crofdpt& dpt)
{
	cfib::get_fib(dpt.get_dptid()).dpt_release(this, &dpt);

	logging::info << "[ethsw][dpath-close]" << std::endl << cfib::get_fib(dpt.get_dptid());
}



void
ethswitch::handle_packet_in(
		crofdpt& dpt,
		const cauxid& auxid,
		rofl::openflow::cofmsg_packet_in& msg)
{
	try {
		msg.set_packet().classify(msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT).get_u32value());

		cmacaddr eth_src = msg.set_packet().ether()->get_dl_src();
		cmacaddr eth_dst = msg.set_packet().ether()->get_dl_dst();



		logging::info << "[ethsw][packet-in] PACKET-IN => frame seen, "
							<< "buffer-id:0x" << std::hex << msg.get_buffer_id() << std::dec << " "
							<< "eth-src:" << msg.set_packet().ether()->get_dl_src() << " "
							<< "eth-dst:" << msg.set_packet().ether()->get_dl_dst() << " "
							<< "eth-type:0x" << std::hex << msg.set_packet().ether()->get_dl_type() << std::dec << " "
							<< std::endl;
		logging::info << dpt.get_dptid();

		/*
		 * sanity check: if source mac is multicast => invalid frame
		 */
		if (eth_src.is_multicast()) {
			logging::warn << "[ethsw][packet-in] eth-src:" << eth_src << " is multicast, ignoring." << std::endl;
			return;
		}



		/*
		 * block mac address 01:80:c2:00:00:00
		 */
		if (msg.set_packet().ether()->get_dl_dst() == cmacaddr("01:80:c2:00:00:00") ||
			msg.set_packet().ether()->get_dl_dst() == cmacaddr("01:00:5e:00:00:fb")) {
			rofl::openflow::cofflowmod fe(dpt.get_version());

			switch (dpt.get_version()) {
			case openflow10::OFP_VERSION: fe.set_command(openflow10::OFPFC_ADD); break;
			case openflow12::OFP_VERSION: fe.set_command(openflow12::OFPFC_ADD); break;
			case openflow13::OFP_VERSION: fe.set_command(openflow13::OFPFC_ADD); break;
			default:
				throw eBadVersion();
			}

			fe.set_buffer_id(msg.get_buffer_id());
			fe.set_idle_timeout(60);
			fe.set_table_id(msg.get_table_id());
			fe.set_flags(rofl::openflow12::OFPFF_SEND_FLOW_REM | rofl::openflow12::OFPFMFC_OVERLAP);

			fe.set_match().set_matches().add_match(msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT));
			fe.set_match().set_eth_dst(msg.set_packet().ether()->get_dl_dst());
			fe.set_match().set_eth_type(msg.set_match().get_eth_type());
			fe.set_instructions().add_inst_apply_actions();

			logging::info << "[ethsw][packet-in] installing new Flow-Mod entry:" << std::endl << fe;

			dpt.send_flow_mod_message(auxid, fe);

			return;
		}



		cfib::get_fib(dpt.get_dptid()).fib_update(
								this,
								dpt,
								msg.set_packet().ether()->get_dl_src(),
								msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT).get_u32value());


		/*
		 * flood mac addresses 33:33:00:00:00:02 and ff:ff:ff:ff:ff:ff
		 */
		if ((msg.set_packet().ether()->get_dl_dst() == cmacaddr("33:33:00:00:00:02"))) {
			rofl::openflow::cofflowmod fe(dpt.get_version());
			rofl::cindex index;

			switch (dpt.get_version()) {
			case openflow10::OFP_VERSION: fe.set_command(openflow10::OFPFC_ADD); break;
			case openflow12::OFP_VERSION: fe.set_command(openflow12::OFPFC_ADD); break;
			case openflow13::OFP_VERSION: fe.set_command(openflow13::OFPFC_ADD); break;
			default:
				throw eBadVersion();
			}

			fe.set_buffer_id(msg.get_buffer_id());
			fe.set_idle_timeout(60);
			fe.set_table_id(msg.get_table_id());
			fe.set_flags(rofl::openflow12::OFPFMFC_OVERLAP);

			fe.set_match().set_in_port(msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT).get_u32value());
			fe.set_match().set_eth_dst(msg.set_packet().ether()->get_dl_dst());
			fe.set_match().set_eth_type(msg.set_match().get_eth_type());
			fe.set_instructions().add_inst_apply_actions();
			fe.set_instructions().set_inst_apply_actions().set_actions().add_action_output(index++).set_port_no(crofbase::get_ofp_flood_port(dpt.get_version()));

			logging::info << "[ethsw][packet-in] installing new Flow-Mod entry:" << std::endl << fe;

			dpt.send_flow_mod_message(auxid, fe);

			return;
		}


		if (eth_dst.is_multicast()) {

			rofl::logging::debug << "[ethsw][packet-in] eth-dst:" << eth_dst << " is multicast, PACKET-OUT flooding" << std::endl;

		} else {

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

					return;
				}

				rofl::openflow::cofflowmod fe(dpt.get_version());
				rofl::cindex index;

				fe.set_command(rofl::openflow12::OFPFC_ADD);
				fe.set_table_id(0);
				fe.set_idle_timeout(60);
				fe.set_priority(0x8000);
				fe.set_buffer_id(msg.get_buffer_id());
				fe.set_flags(rofl::openflow12::OFPFF_SEND_FLOW_REM | rofl::openflow12::OFPFMFC_OVERLAP);


				fe.set_match().set_eth_dst(eth_dst);
				fe.set_match().set_eth_src(eth_src);
				fe.set_match().set_eth_type(msg.set_match().get_eth_type());

				fe.set_instructions().add_inst_apply_actions().set_actions().
						add_action_output(index++).set_port_no(entry.get_out_port_no());

				indent i(2); rofl::logging::debug << "[ethsw][packet-in] installing flow mod" << std::endl << fe;

				dpt.send_flow_mod_message(auxid, fe);

				return;

			} catch (eFibInval& e) {

				rofl::logging::debug << "[ethsw][packet-in] eFibInval, ignoring." << std::endl << cfib::get_fib(dpt.get_dptid());

				return;

			} catch (eFibNotFound& e) {

				rofl::logging::debug << "[ethsw][packet-in] dst:" << eth_dst << " NOT FOUND in FIB, PACKET-OUT flooding." << std::endl;
			}
		}




		rofl::openflow::cofactions actions(dpt.get_version());
		rofl::cindex index;

		switch (dpt.get_version()) {
		case openflow10::OFP_VERSION:
			actions.add_action_output(index).set_port_no(openflow10::OFPP_FLOOD); break;
		case openflow12::OFP_VERSION:
			actions.add_action_output(index).set_port_no(openflow12::OFPP_FLOOD); break;
		case openflow13::OFP_VERSION:
			actions.add_action_output(index).set_port_no(openflow13::OFPP_FLOOD); break;
		default:
			throw eBadVersion();
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
			dpt.send_packet_out_message(auxid, msg.get_buffer_id(), msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT).get_u32value(), actions);
		} else {
			dpt.send_packet_out_message(auxid, msg.get_buffer_id(), msg.set_match().get_matches().get_match(rofl::openflow::OXM_TLV_BASIC_IN_PORT).get_u32value(), actions,
					msg.set_packet().soframe(), msg.set_packet().framelen());
		}

	} catch (...) {
		logging::error << "[ethsw][packet-in] caught some exception, use debugger for getting more info" << std::endl << msg;
	}
}



