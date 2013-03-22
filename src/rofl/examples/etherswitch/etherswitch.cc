#include "etherswitch.h"


etherswitch::etherswitch()
{
	// ...
}



etherswitch::~etherswitch()
{
	// ...
}



void
etherswitch::handle_dpath_open(
		cofdpt *dpt)
{
	// do nothing here
}



void
etherswitch::handle_dpath_close(
		cofdpt *dpt)
{
	fib.erase(dpt);
}



void
etherswitch::handle_packet_in(
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
		actions.next() = cofaction_output(OFPP_FLOOD);

		send_packet_out_message(
				dpt,
				msg->get_buffer_id(),
				msg->get_match().get_in_port(),
				actions);
	}
	/*
	 * unicast destination mac is known in FIB
	 */
	else
	{
		uint32_t out_port = fib[dpt][vlan_id][eth_dst];

		cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_ADD);
		fe.set_buffer_id(msg->get_buffer_id());
		fe.set_idle_timeout(15);
		fe.set_table_id(0);

		fe.match.set_in_port(msg->get_match().get_in_port());
		fe.match.set_eth_dst(msg->get_packet().ether()->get_dl_dst());
		fe.instructions[0] = cofinst_write_actions();
		fe.instructions[0].actions.next() = cofaction_output(out_port);

		send_flow_mod_message(
				dpt,
				fe);
	}

	/*
	 * update FIB
	 */
	fib[dpt][vlan_id][eth_src] = msg->get_match().get_in_port(); // may throw eOFmatchNotFound

	delete msg;
}



