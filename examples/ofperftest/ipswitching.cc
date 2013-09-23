#include "ipswitching.h"

#include <inttypes.h>

ipswitching::ipswitching(unsigned int n_entries) :
	n_entries(n_entries),
	fib_check_timeout(5), // check for expired FIB entries every 5 seconds
	fm_delete_all_timeout(30)
{
	// ...
	register_timer(IPSWITCHING_TIMER_FIB, fib_check_timeout);
	register_timer(IPSWITCHING_TIMER_FLOW_MOD_DELETE_ALL, fm_delete_all_timeout);
}



ipswitching::~ipswitching()
{
	flow_mod_delete_all();
}



void
ipswitching::handle_timeout(int opaque)
{
	switch (opaque) {
	case IPSWITCHING_TIMER_FIB: {
		drop_expired_fib_entries();
	} break;
	case IPSWITCHING_TIMER_FLOW_MOD_DELETE_ALL: {
		//flow_mod_delete_all();
	} break;
	default:
		crofbase::handle_timeout(opaque);
	}
}



void
ipswitching::drop_expired_fib_entries()
{
	// iterate over all FIB entries and delete expired ones ...

	register_timer(IPSWITCHING_TIMER_FIB, fib_check_timeout);
}



void
ipswitching::install_flow_mods(cofdpt *dpt, unsigned int n)
{
	if (0 == dpt) {
		fprintf(stderr, "error installing test FlowMod entries on data path: dpt is NULL");
		return;
	}

	// sanity check: data path should have at least two ports :)
	if (dpt->get_ports().size() < 2) {
		fprintf(stderr, "error installing test FlowMod entries on data path: less than 2 ports");
		return;
	}

	uint32_t portnums[2];

	std::map<uint32_t, cofport*>::iterator it = dpt->get_ports().begin();
	portnums[0] = it->first;
	it++;
	portnums[1] = it->first;

	crandom r;

	fprintf(stderr, "installing %u fake FlowMod entries\n", n);

	for (unsigned int i = 0; i < n; i++) {

		cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_ADD);
		fe.set_buffer_id(OFP_NO_BUFFER);
		fe.set_idle_timeout(0);
		fe.set_hard_timeout(0);
		fe.set_table_id(0);

		caddress addr(AF_INET);

		addr.ca_s4addr->sin_addr.s_addr = htobe32(r.uint32()); // draw new random number
		addr[0] = 10; // force everything to be from 10/8 => our test setup assumes client addresses from that range
		addr[3] += i;

		fe.match.set_ipv4_dst(addr);
		fe.instructions.next() = cofinst_write_actions(dpt->get_version());
		fe.instructions.back().actions.next() = cofaction_output(dpt->get_version(), portnums[(i%2)]);

		fprintf(stderr, "ipswitching: installing fake FLowMod entry #%d: %s\n",
				i, fe.c_str());

		send_flow_mod_message(dpt, fe);
	}
}



void
ipswitching::flow_mod_delete_all()
{
	std::map<cofdpt*, std::map<caddress, struct fibentry_t> >::iterator it;

	for (it = fib.begin(); it != fib.end(); ++it) {
		cofdpt *dpt = it->first;

		cflowentry fe(dpt->get_version());
		fe.set_command(OFPFC_DELETE);
		fe.set_table_id(OFPTT_ALL);
		fe.set_out_port(OFPP12_ANY);
		fe.set_out_group(OFPG12_ANY);

		fprintf(stderr, "FLOW-MOD: delete all: %s\n", fe.c_str());

		send_flow_mod_message(dpt, fe);
	}

	register_timer(IPSWITCHING_TIMER_FLOW_MOD_DELETE_ALL, fm_delete_all_timeout);
}



void
ipswitching::handle_dpath_open(
		cofdpt *dpt)
{
	fib[dpt] = std::map<caddress, struct fibentry_t>();
	// do nothing here

	install_flow_mods(dpt, n_entries);
}



void
ipswitching::handle_dpath_close(
		cofdpt *dpt)
{
	fib.erase(dpt);
}



void
ipswitching::handle_packet_in(
		cofdpt *dpt,
		cofmsg_packet_in *msg)
{
	/*
	 * block mac address 01:80:c2:00:00:00
	 */
	if (msg->get_packet().ether()->get_dl_dst() == cmacaddr("01:80:c2:00:00:00") ||
		msg->get_packet().ether()->get_dl_dst() == cmacaddr("01:00:5e:00:00:fb")) {
		cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_ADD);
		fe.set_buffer_id(msg->get_buffer_id());
		fe.set_idle_timeout(15);
		fe.set_table_id(msg->get_table_id());

		fe.match.set_in_port(msg->get_match().get_in_port());
		fe.match.set_eth_dst(msg->get_packet().ether()->get_dl_dst());
		fe.instructions.next() = cofinst_apply_actions(dpt->get_version());

		fprintf(stderr, "ipswitching: installing FLOW-MOD with entry: %s\n",
				fe.c_str());

		send_flow_mod_message(dpt, fe);

		delete msg; return;
	}



	switch (msg->get_packet().ether()->get_dl_type()) {
	case farpv4frame::ARPV4_ETHER: {
		return handle_packet_in_arpv4(dpt, msg);
	} break;
	case fipv4frame::IPV4_ETHER: {
		return handle_packet_in_ipv4(dpt, msg);
	} break;
	case fvlanframe::VLAN_CTAG_ETHER: {
		switch (msg->get_packet().vlan()->get_dl_type()) {
		case farpv4frame::ARPV4_ETHER: {
			return handle_packet_in_arpv4(dpt, msg);
		} break;
		case fipv4frame::IPV4_ETHER: {
			return handle_packet_in_ipv4(dpt, msg);
		} break;
		default: {
			delete msg;
		}
		}

	} break;
	default: {
		delete msg;
	}
	}
}



void
ipswitching::handle_packet_in_arpv4(
		cofdpt *dpt,
		cofmsg_packet_in *msg)
{
#if 0
	fprintf(stderr, "ipswitching::handle_packet_in_arpv4() packet: %s\n", msg->get_packet().c_str());
#endif

	caddress ip_src(AF_INET);
	caddress ip_dst(AF_INET);

	switch (msg->get_packet().arpv4()->get_opcode()) {
	case farpv4frame::ARPV4_OPCODE_REQUEST: {
		ip_src = msg->get_packet().arpv4()->get_nw_src();
		ip_dst = msg->get_packet().arpv4()->get_nw_dst();
#if 0
		fprintf(stderr, "XXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
		fprintf(stderr, "ipswitching::handle_packet_in_arpv4() [REQUEST] %s => %s\n", ip_src.c_str(), ip_dst.c_str());
		fprintf(stderr, "HHHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
#endif
	} break;
	case farpv4frame::ARPV4_OPCODE_REPLY: {
		ip_src = msg->get_packet().arpv4()->get_nw_src();
		ip_dst = msg->get_packet().arpv4()->get_nw_dst();
#if 0
		fprintf(stderr, "XXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
		fprintf(stderr, "ipswitching::handle_packet_in_arpv4() [REPLY] %s => %s\n", ip_src.c_str(), ip_dst.c_str());
		fprintf(stderr, "HHHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
#endif
	} break;
	default: {
		delete msg; return;
	}
	}


	/*
	 * update FIB
	 */
	update_fib_table(dpt, msg, ip_src);



	if (ip_dst.is_multicast() ||
			(fib[dpt].find(ip_dst) == fib[dpt].end())) {

		fprintf(stderr, "ipswitching::handle_packet_in_arpv4() Pt.1.1\n");

		flood_vlans(dpt, msg, ip_src);


	} else {

		fprintf(stderr, "ipswitching::handle_packet_in_arpv4() Pt.1.2\n");

		cofaclist actions;

		// both use vlan => just reset the vid
		if ((fib[dpt][ip_dst].vid != 0xffff) && (fib[dpt][ip_src].vid != 0xffff)) {
			actions.next() = cofaction_set_field(dpt->get_version(), coxmatch_ofb_vlan_vid(fib[dpt][ip_dst].vid));
		}
		// src uses vlan, dst is untagged => pop vlan tag
		else if ((fib[dpt][ip_dst].vid == 0xffff) && (fib[dpt][ip_src].vid != 0xffff)) {
			actions.next() = cofaction_pop_vlan(dpt->get_version());
		}
		// src is untagged, dst uses vlan => push vlan tag
		else if ((fib[dpt][ip_dst].vid != 0xffff) && (fib[dpt][ip_src].vid == 0xffff)) {
			actions.next() = cofaction_push_vlan(dpt->get_version(), fvlanframe::VLAN_CTAG_ETHER);
			actions.next() = cofaction_set_field(dpt->get_version(), coxmatch_ofb_vlan_vid(fib[dpt][ip_dst].vid));
		}
		// src and dst are untagged => do nothing
		else {
			// do nothing
		}

		actions.next() = cofaction_output(dpt->get_version(), fib[dpt][ip_dst].port_no);
		if (OFP_NO_BUFFER == msg->get_buffer_id()) {
			fprintf(stderr, "NOEEEETTTTT!!!!!\n");
			send_packet_out_message(
					dpt,
					msg->get_buffer_id(),
					msg->get_match().get_in_port(),
					actions,
					msg->get_packet().soframe(), msg->get_packet().framelen());
		} else {
			send_packet_out_message(
					dpt,
					msg->get_buffer_id(),
					msg->get_match().get_in_port(),
					actions);
		}

#if 0
		fprintf(stderr, "ipswitching: forwarding ARP packet (portno: %d) with ActionList: %s\n",
				fib[dpt][msg->get_packet().arpv4()->get_nw_dst()].port_no, actions.c_str());
#endif

	}

	delete msg;
}



void
ipswitching::handle_packet_in_ipv4(
		cofdpt *dpt,
		cofmsg_packet_in *msg)
{
	caddress ip_src = msg->get_packet().ipv4()->get_ipv4_src();
	caddress ip_dst = msg->get_packet().ipv4()->get_ipv4_dst();


	/*
	 * update FIB
	 */
	update_fib_table(dpt, msg, msg->get_packet().ipv4()->get_ipv4_src());

	// if no entry for dst exists in our FIB, just flood the packet
	if (fib[dpt].find(ip_dst) == fib[dpt].end()) {

		flood_vlans(dpt, msg, ip_src);

	}
	else // if an entry exists, create a new FlowMod
	{
		cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_ADD);
		fe.set_buffer_id(OFP_NO_BUFFER);
		fe.set_idle_timeout(0);
		fe.set_hard_timeout(0);
		fe.set_table_id(msg->get_table_id());

		fe.match.set_ipv4_src(ip_src);
		fe.match.set_ipv4_dst(ip_dst);

		fe.instructions.next() = cofinst_write_actions(dpt->get_version());

		// both use vlan => just reset the vid
		if ((fib[dpt][ip_dst].vid != 0xffff) && (fib[dpt][ip_src].vid != 0xffff)) {
			fe.match.set_vlan_vid(fib[dpt][ip_src].vid);
			fe.instructions.back().actions.next() = cofaction_set_field(dpt->get_version(), coxmatch_ofb_vlan_vid(fib[dpt][ip_dst].vid));
		}
		// src uses vlan, dst is untagged => pop vlan tag
		else if ((fib[dpt][ip_dst].vid == 0xffff) && (fib[dpt][ip_src].vid != 0xffff)) {
			fe.match.set_vlan_vid(fib[dpt][ip_src].vid);
			fe.instructions.back().actions.next() = cofaction_pop_vlan(dpt->get_version());
		}
		// src is untagged, dst uses vlan => push vlan tag
		else if ((fib[dpt][ip_dst].vid != 0xffff) && (fib[dpt][ip_src].vid == 0xffff)) {
			fe.instructions.back().actions.next() = cofaction_push_vlan(dpt->get_version(), fvlanframe::VLAN_CTAG_ETHER);
			fe.instructions.back().actions.next() = cofaction_set_field(dpt->get_version(), coxmatch_ofb_vlan_vid(fib[dpt][ip_dst].vid));
		}
		// src and dst are untagged => do nothing
		else {
			// do nothing
		}

		fe.instructions.back().actions.next() = cofaction_output(dpt->get_version(), fib[dpt][ip_dst].port_no);

		fprintf(stderr, "ipswitching::handle_packet_in_ipv4() setting FlowMod entry: %s\n",
				fe.c_str());

		send_flow_mod_message(dpt, fe);
	}

	delete msg;

}



void
ipswitching::update_fib_table(cofdpt *dpt, cofmsg_packet_in *msg, caddress ip_src)
{
	fprintf(stderr, "ipswitching::update_fib_table() Pt. 3.1 ip_src:%s\n", ip_src.c_str());
	if (fib[dpt].find(ip_src) == fib[dpt].end()) {
		fprintf(stderr, "ipswitching::update_fib_table() Pt. 3.2 ip_src:%s\n", ip_src.c_str());
		fib[dpt][ip_src].port_no = msg->get_match().get_in_port(); // may throw eOFmatchNotFound
		fib[dpt][ip_src].timeout = time(NULL) + fib_check_timeout;
		fib[dpt][ip_src].addr = ip_src;
		fib[dpt][ip_src].vid = 0xffff;

		try {
			fib[dpt][ip_src].vid = msg->get_packet().vlan()->get_dl_vlan_id();
		} catch (ePacketNotFound& e) {}

		fprintf(stderr, "\n\n ==> NEW IPV4 FIB-ENTRY: dpid=%s addr=%s portno=%d vid=%d\n\n",
				dpt->get_dpid_s().c_str(),
				ip_src.addr_c_str(),
				msg->get_match().get_in_port(),
				fib[dpt][ip_src].vid);


		fprintf(stderr, "== dpid: 0x%s =========================\n", dpt->get_dpid_s().c_str());
		for (std::map<caddress, fibentry_t>::iterator
				it = fib[dpt].begin(); it != fib[dpt].end(); ++it) {
			fprintf(stderr, "dpid: 0x%s FIB table: addr=%s portno=%d vid=%d\n",
					dpt->get_dpid_s().c_str(),
					it->second.addr.addr_c_str(),
					it->second.port_no,
					it->second.vid);
		}
		fprintf(stderr, "=======================================\n");
	}
}



void
ipswitching::flood_vlans(cofdpt *dpt, cofmsg_packet_in *msg, caddress ip_src)
{
	/*
	 * this is a workaround, as OFPP12_TABLE is not implemented yet in xdpd
	 */
	for (std::map<caddress, fibentry_t>::iterator
			it = fib[dpt].begin(); it != fib[dpt].end(); ++it) {

		// do not send the packet back to where it came from
		if (ip_src == it->second.addr)
			continue;

		cofaclist actions;

		// both use vlan => just reset the vid
		if ((fib[dpt][it->second.addr].vid != 0xffff) && (fib[dpt][ip_src].vid != 0xffff)) {
			actions.next() = cofaction_set_field(dpt->get_version(), coxmatch_ofb_vlan_vid(fib[dpt][it->second.addr].vid));
		}
		// src uses vlan, dst is untagged => pop vlan tag
		else if ((fib[dpt][it->second.addr].vid == 0xffff) && (fib[dpt][ip_src].vid != 0xffff)) {
			actions.next() = cofaction_pop_vlan(dpt->get_version());
		}
		// src is untagged, dst uses vlan => push vlan tag
		else if ((fib[dpt][it->second.addr].vid != 0xffff) && (fib[dpt][ip_src].vid == 0xffff)) {
			actions.next() = cofaction_push_vlan(dpt->get_version(), fvlanframe::VLAN_CTAG_ETHER);
			actions.next() = cofaction_set_field(dpt->get_version(), coxmatch_ofb_vlan_vid(fib[dpt][it->second.addr].vid));
		}
		// src and dst are untagged => do nothing
		else {
			// do nothing
		}

		actions.next() = cofaction_output(dpt->get_version(), it->second.port_no);

#if 1
		fprintf(stderr, "ipswitching::flood_vlans() %s => %s with actions: %s\n",
				ip_src.addr_c_str(), it->second.addr.addr_c_str(), actions.c_str());
#endif

		if (OFP_NO_BUFFER == msg->get_buffer_id()) {
			fprintf(stderr, "NOEEEETTTTT!!!!! (2.1)\n");
			send_packet_out_message(
					dpt,
					msg->get_buffer_id(),
					msg->get_match().get_in_port(),
					actions,
					msg->get_packet().soframe(), msg->get_packet().framelen());
		} else {
			send_packet_out_message(
					dpt,
					msg->get_buffer_id(),
					msg->get_match().get_in_port(),
					actions);
		}
	}


	/*
	 *
	 */

	cofaclist actions;
	if (fib[dpt][ip_src].vid != 0xffff) {
		actions.next() = cofaction_pop_vlan(dpt->get_version());
	}
	actions.next() = cofaction_output(dpt->get_version(), OFPP12_FLOOD);
	if (OFP_NO_BUFFER == msg->get_buffer_id()) {
		fprintf(stderr, "NOEEEETTTTT!!!!! (2.2)\n");
		send_packet_out_message(
				dpt,
				msg->get_buffer_id(),
				msg->get_match().get_in_port(),
				actions,
				msg->get_packet().soframe(), msg->get_packet().framelen());
	} else {
		send_packet_out_message(
				dpt,
				msg->get_buffer_id(),
				msg->get_match().get_in_port(),
				actions);
	}

	fprintf(stderr, "ipswitching: forwarding ARP packet (FLOOD) with ActionList: %s\n",
			actions.c_str());
}
