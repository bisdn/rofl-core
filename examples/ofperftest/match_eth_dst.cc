#include "match_eth_dst.h"

#include <inttypes.h>

match_eth_dst::match_eth_dst(unsigned int n_entries) :
	n_entries(n_entries),
	fib_check_timeout(5), // check for expired FIB entries every 5 seconds
	fm_delete_all_timeout(30)
{
	// ...
	register_timer(ETHSWITCH_TIMER_FIB, fib_check_timeout);
	register_timer(ETHSWITCH_TIMER_FLOW_MOD_DELETE_ALL, fm_delete_all_timeout);
}



match_eth_dst::~match_eth_dst()
{
	flow_mod_delete_all();
}



void
match_eth_dst::handle_timeout(int opaque)
{
	switch (opaque) {
	case ETHSWITCH_TIMER_FIB: {
		drop_expired_fib_entries();
	} break;
	case ETHSWITCH_TIMER_FLOW_MOD_DELETE_ALL: {
		//flow_mod_delete_all();
	} break;
	default:
		crofbase::handle_timeout(opaque);
	}
}



void
match_eth_dst::drop_expired_fib_entries()
{
	// iterate over all FIB entries and delete expired ones ...

	register_timer(ETHSWITCH_TIMER_FIB, fib_check_timeout);
}



void
match_eth_dst::install_flow_mods(cofdpt *dpt, unsigned int n)
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
	uint64_t r_num = r.uint64();

	fprintf(stderr, "installing %u fake FlowMod entries\n", n);

	for (unsigned int i = 0; i < n; i++) {

		cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_ADD);
		fe.set_buffer_id(OFP_NO_BUFFER);
		fe.set_idle_timeout(0);
		fe.set_hard_timeout(0);
		fe.set_table_id(0);

		r_num += i;
		cmacaddr r_mac("00:00:00:00:00:00");
		r_mac[5] = ((uint8_t*)&r_num)[0];
		r_mac[4] = ((uint8_t*)&r_num)[1];
		r_mac[3] = ((uint8_t*)&r_num)[2];
		r_mac[2] = ((uint8_t*)&r_num)[3];
		r_mac[1] = ((uint8_t*)&r_num)[4];
		r_mac[0] = ((uint8_t*)&r_num)[5];

		r_mac[5] &= 0xf7;

		fe.match.set_eth_dst(r_mac);
		fe.instructions.next() = cofinst_write_actions();
		fe.instructions.back().actions.next() = cofaction_output(dpt->get_version(), portnums[0]);

		fprintf(stderr, "match_eth_dst: calling FLOW-MOD with entry: %s\n",
				fe.c_str());

		send_flow_mod_message(
				dpt,
				fe);
	}
}



void
match_eth_dst::flow_mod_delete_all()
{
	std::map<cofdpt*, std::map<uint16_t, std::map<cmacaddr, struct fibentry_t> > >::iterator it;

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

	register_timer(ETHSWITCH_TIMER_FLOW_MOD_DELETE_ALL, fm_delete_all_timeout);
}



void
match_eth_dst::handle_dpath_open(
		cofdpt *dpt)
{
	fib[dpt] = std::map<uint16_t, std::map<cmacaddr, struct fibentry_t> >();
	// do nothing here

	install_flow_mods(dpt, n_entries);
}



void
match_eth_dst::handle_dpath_close(
		cofdpt *dpt)
{
	fib.erase(dpt);
}



void
match_eth_dst::handle_packet_in(
		cofdpt *dpt,
		cofmsg_packet_in *msg)
{
	cmacaddr eth_src = msg->get_packet().ether()->get_dl_src();

	/*
	 * sanity check: if source mac is multicast => invalid frame
	 */
	if (eth_src.is_multicast()) {
		delete msg; return;
	}

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

		fprintf(stderr, "match_eth_dst: installing FLOW-MOD with entry: %s\n",
				fe.c_str());

		send_flow_mod_message(
				dpt,
				fe);

		delete msg; return;
	}

	fprintf(stderr, "match_eth_dst: PACKET-IN from dpid:0x%"PRIu64" buffer-id:0x%x => from %s to %s type: 0x%x\n",
			dpt->get_dpid(),
			msg->get_buffer_id(),
			msg->get_packet().ether()->get_dl_src().c_str(),
			msg->get_packet().ether()->get_dl_dst().c_str(),
			msg->get_packet().ether()->get_dl_type());

	/*
	 * get VLAN-ID and destination mac
	 */
	uint16_t vlan_id = 0xffff;
	cmacaddr eth_dst = msg->get_packet().ether()->get_dl_dst();

	try {
		// vlan(): if no VLAN tag found => throws ePacketNotFound
		vlan_id = msg->get_packet().vlan()->get_dl_vlan_id();
	} catch (ePacketNotFound& e) {}



	/*
	 * if multicast or outgoing port unknown => FLOOD packet
	 */
	if (eth_dst.is_multicast() ||
			(fib[dpt][vlan_id].find(eth_dst) == fib[dpt][vlan_id].end()))
	{
		cofaclist actions;
		actions.next() = cofaction_output(dpt->get_version(), OFPP_FLOOD);

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

		fprintf(stderr, "match_eth_dst: calling PACKET-OUT with ActionList: %s\n",
				actions.c_str());

	}
	/*
	 * unicast destination mac is known in FIB
	 */
	else
	{
		uint32_t out_port = fib[dpt][vlan_id][eth_dst].port_no;

		if (msg->get_match().get_in_port() == out_port) {
			delete msg; return;
		}

		cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_ADD);
		fe.set_buffer_id(msg->get_buffer_id());
		fe.set_idle_timeout(0);
		fe.set_table_id(msg->get_table_id());

		fe.match.set_eth_dst(eth_dst);
		fe.instructions.next() = cofinst_write_actions();
		fe.instructions[0].actions.next() = cofaction_output(dpt->get_version(), out_port);

		fprintf(stderr, "match_eth_dst: calling FLOW-MOD with entry: %s\n",
				fe.c_str());

		send_flow_mod_message(
				dpt,
				fe);
	}

	/*
	 * update FIB
	 */
	fib[dpt][vlan_id][eth_src].port_no = msg->get_match().get_in_port(); // may throw eOFmatchNotFound
	fib[dpt][vlan_id][eth_src].timeout = time(NULL) + fib_check_timeout;

	delete msg;
}



