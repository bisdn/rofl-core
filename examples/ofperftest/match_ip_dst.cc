#include "match_ip_dst.h"

#include <inttypes.h>

match_ip_dst::match_ip_dst(unsigned int n_entries) :
	n_entries(n_entries),
	fib_check_timeout(5), // check for expired FIB entries every 5 seconds
	fm_delete_all_timeout(30)
{
	// ...
	register_timer(MATCH_IP_DST_TIMER_FIB, fib_check_timeout);
	register_timer(MATCH_IP_DST_TIMER_FLOW_MOD_DELETE_ALL, fm_delete_all_timeout);
}



match_ip_dst::~match_ip_dst()
{
	flow_mod_delete_all();
}



void
match_ip_dst::handle_timeout(int opaque)
{
	switch (opaque) {
	case MATCH_IP_DST_TIMER_FIB: {
		drop_expired_fib_entries();
	} break;
	case MATCH_IP_DST_TIMER_FLOW_MOD_DELETE_ALL: {
		//flow_mod_delete_all();
	} break;
	default:
		crofbase::handle_timeout(opaque);
	}
}



void
match_ip_dst::drop_expired_fib_entries()
{
	// iterate over all FIB entries and delete expired ones ...

	register_timer(MATCH_IP_DST_TIMER_FIB, fib_check_timeout);
}



void
match_ip_dst::install_flow_mods(cofdpt *dpt, unsigned int n)
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
		fe.instructions.next() = cofinst_write_actions();
		fe.instructions.back().actions.next() = cofaction_output(portnums[(i%2)]);

		fprintf(stderr, "match_ip_dst: installing fake FLowMod entry #%d: %s\n",
				i, fe.c_str());

		send_flow_mod_message(dpt, fe);
	}
}



void
match_ip_dst::flow_mod_delete_all()
{
	std::map<cofdpt*, std::map<caddress, struct fibentry_t> >::iterator it;

	for (it = fib.begin(); it != fib.end(); ++it) {
		cofdpt *dpt = it->first;

		cflowentry fe(dpt->get_version());
		fe.set_command(OFPFC_DELETE);
		fe.set_table_id(OFPTT_ALL);
		fe.set_out_port(OFPP_ANY);
		fe.set_out_group(OFPG_ANY);

		fprintf(stderr, "FLOW-MOD: delete all: %s\n", fe.c_str());

		send_flow_mod_message(dpt, fe);
	}

	register_timer(MATCH_IP_DST_TIMER_FLOW_MOD_DELETE_ALL, fm_delete_all_timeout);
}



void
match_ip_dst::handle_dpath_open(
		cofdpt *dpt)
{
	fib[dpt] = std::map<caddress, struct fibentry_t>();
	// do nothing here

	install_flow_mods(dpt, n_entries);
}



void
match_ip_dst::handle_dpath_close(
		cofdpt *dpt)
{
	fib.erase(dpt);
}



void
match_ip_dst::handle_packet_in(
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
		fe.instructions.next() = cofinst_apply_actions();

		fprintf(stderr, "match_ip_dst: installing FLOW-MOD with entry: %s\n",
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
	default: {
		delete msg;
	}
	}
}



void
match_ip_dst::handle_packet_in_arpv4(
		cofdpt *dpt,
		cofmsg_packet_in *msg)
{
	if (msg->get_packet().ether()->get_dl_dst().is_multicast() ||
			(fib[dpt].find(msg->get_packet().arpv4()->get_nw_dst()) == fib[dpt].end())) {

		cofaclist actions;
		actions.next() = cofaction_output(OFPP_FLOOD);
		if (OFP_NO_BUFFER == msg->get_buffer_id()) {
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

		fprintf(stderr, "match_ip_dst: forwarding ARP packet (FLOOD) with ActionList: %s\n",
				actions.c_str());

	} else {

		cofaclist actions;
		actions.next() = cofaction_output(fib[dpt][msg->get_packet().arpv4()->get_nw_dst()].port_no);
		if (OFP_NO_BUFFER == msg->get_buffer_id()) {
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

		fprintf(stderr, "match_ip_dst: forwarding ARP packet (portno: %d) with ActionList: %s\n",
				fib[dpt][msg->get_packet().arpv4()->get_nw_dst()].port_no, actions.c_str());


	}


	/*
	 * update FIB
	 */
	update_fib_table(dpt, msg, msg->get_packet().arpv4()->get_nw_src());

	delete msg;
}



void
match_ip_dst::handle_packet_in_ipv4(
		cofdpt *dpt,
		cofmsg_packet_in *msg)
{

	/*
	 * update FIB
	 */
	update_fib_table(dpt, msg, msg->get_packet().ipv4()->get_ipv4_src());

	// if no entry for dst exists in our FIB, just flood the packet
	if (fib[dpt].find(msg->get_packet().ipv4()->get_ipv4_dst()) == fib[dpt].end()) {

		cofaclist actions;
		actions.next() = cofaction_output(OFPP_FLOOD);
		if (OFP_NO_BUFFER == msg->get_buffer_id()) {
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

		fprintf(stderr, "match_ip_dst: forwarding IPv4 packet (FLOOD) with ActionList: %s\n",
				actions.c_str());
	}

	delete msg;
#if 0
	caddress ip_src = msg->get_packet().ipv4()->get_ipv4_src();
	caddress ip_dst = msg->get_packet().ipv4()->get_ipv4_dst();
	cmacaddr eth_src = msg->get_packet().ether()->get_dl_src();
	cmacaddr eth_dst = msg->get_packet().ether()->get_dl_dst();

	/*
	 * sanity check: if source mac is multicast => invalid frame
	 */
	if (ip_src.is_multicast()) {
		delete msg; return;
	}



	fprintf(stderr, "match_ip_dst: PACKET-IN from dpid:0x%"PRIu64" buffer-id:0x%x => from %s to %s type: 0x%x\n",
			dpt->get_dpid(),
			msg->get_buffer_id(),
			msg->get_packet().ether()->get_dl_src().c_str(),
			msg->get_packet().ether()->get_dl_dst().c_str(),
			msg->get_packet().ether()->get_dl_type());



	/*
	 * if multicast or outgoing port unknown => FLOOD packet
	 */
	if (ip_dst.is_multicast() ||
			(fib[dpt].find(ip_dst) == fib[dpt].end()))
	{
		cofaclist actions;
		actions.next() = cofaction_output(OFPP_FLOOD);

		if (OFP_NO_BUFFER == msg->get_buffer_id()) {
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

		fprintf(stderr, "match_ip_dst: calling PACKET-OUT with ActionList: %s\n",
				actions.c_str());

	}
	/*
	 * unicast destination mac is known in FIB
	 */
	else
	{
		uint32_t out_port = fib[dpt][ip_dst].port_no;

		if (msg->get_match().get_in_port() == out_port) {
			delete msg; return;
		}

		cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_ADD);
		fe.set_buffer_id(msg->get_buffer_id());
		fe.set_idle_timeout(0);
		fe.set_table_id(msg->get_table_id());

		fe.match.set_ipv4_dst(ip_dst);
		fe.instructions.next() = cofinst_write_actions();
		fe.instructions[0].actions.next() = cofaction_output(out_port);

		fprintf(stderr, "match_ip_dst: calling FLOW-MOD with entry: %s\n",
				fe.c_str());

		send_flow_mod_message(
				dpt,
				fe);
	}

	/*
	 * update FIB
	 */
	fib[dpt][ip_src].port_no = msg->get_match().get_in_port(); // may throw eOFmatchNotFound
	fib[dpt][ip_src].timeout = time(NULL) + fib_check_timeout;
	fib[dpt][ip_src].addr = ip_src;

	fprintf(stderr, "NEW IPV4 FIB-ENTRY: dpid=%s addr=%s portno=%d\n",
			dpt->get_dpid_s().c_str(), ip_src.addr_c_str(), msg->get_match().get_in_port());

	delete msg;
#endif
}



void
match_ip_dst::update_fib_table(cofdpt *dpt, cofmsg_packet_in *msg, caddress ip_src)
{
	if (fib[dpt].find(ip_src) == fib[dpt].end()) {

		fib[dpt][ip_src].port_no = msg->get_match().get_in_port(); // may throw eOFmatchNotFound
		fib[dpt][ip_src].timeout = time(NULL) + fib_check_timeout;
		fib[dpt][ip_src].addr = ip_src;

		fprintf(stderr, "NEW IPV4 FIB-ENTRY: dpid=%s addr=%s portno=%d\n",
				dpt->get_dpid_s().c_str(), ip_src.addr_c_str(), msg->get_match().get_in_port());


		cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_ADD);
		fe.set_buffer_id(OFP_NO_BUFFER);
		fe.set_idle_timeout(0);
		fe.set_hard_timeout(0);
		fe.set_table_id(msg->get_table_id());

		fe.match.set_ipv4_dst(ip_src);
		fe.instructions.next() = cofinst_write_actions();
		fe.instructions[0].actions.next() = cofaction_output(fib[dpt][ip_src].port_no);

		fprintf(stderr, "match_ip_dst: calling FLOW-MOD with entry: %s\n",
				fe.c_str());

		send_flow_mod_message(dpt, fe);

		fprintf(stderr, "== dpid: 0x%s =========================\n", dpt->get_dpid_s().c_str());
		for (std::map<caddress, fibentry_t>::iterator
				it = fib[dpt].begin(); it != fib[dpt].end(); ++it) {
			fprintf(stderr, "dpid: 0x%s FIB table: addr=%s portno=%d\n",
					dpt->get_dpid_s().c_str(), it->second.addr.addr_c_str(), it->second.port_no);
		}
	}
}
