/*
 * cfibentry.cc
 *
 *  Created on: 15.07.2013
 *      Author: andreas
 */

#include <cfibentry.h>

using namespace etherswitch;

cfibentry::cfibentry(
		cfibtable *fib,
		rofl::crofbase *rofbase,
		rofl::cofdpt *dpt,
		rofl::cmacaddr dst,
		uint32_t out_port_no) :
		fib(fib),
		out_port_no(out_port_no),
		dst(dst),
		rofbase(rofbase),
		dpt(dpt),
		entry_timeout(CFIBENTRY_DEFAULT_TIMEOUT)
{
	register_timer(CFIBENTRY_ENTRY_EXPIRED, entry_timeout);
}


cfibentry::~cfibentry()
{

}



void
cfibentry::handle_timeout(int opaque)
{
	switch (opaque) {
	case CFIBENTRY_ENTRY_EXPIRED: {
		fib->fib_timer_expired(this);
	} break;
	}
}



void
cfibentry::set_out_port_no(uint32_t out_port_no)
{
	flow_mod_delete();

	this->out_port_no = out_port_no;

	reset_timer(CFIBENTRY_ENTRY_EXPIRED, entry_timeout);

	flow_mod_add();
}



void
cfibentry::flow_mod_add()
{
	uint32_t flood_port = 0;
	switch (dpt->get_version()) {
	case rofl::openflow10::OFP_VERSION: flood_port = rofl::openflow10::OFPP_FLOOD; break;
	case rofl::openflow12::OFP_VERSION: flood_port = rofl::openflow12::OFPP_FLOOD; break;
	case rofl::openflow13::OFP_VERSION: flood_port = rofl::openflow13::OFPP_FLOOD; break;
	default:
		throw rofl::eBadVersion();
	}

	uint8_t command = 0;
	switch (dpt->get_version()) {
	case rofl::openflow10::OFP_VERSION: command = rofl::openflow10::OFPFC_ADD; break;
	case rofl::openflow12::OFP_VERSION: command = rofl::openflow12::OFPFC_ADD; break;
	case rofl::openflow13::OFP_VERSION: command = rofl::openflow13::OFPFC_ADD; break;
	default:
		throw rofl::eBadVersion();
	}

	uint32_t out_port = 0;
	switch (dpt->get_version()) {
	case rofl::openflow10::OFP_VERSION: out_port = rofl::openflow10::OFPP_CONTROLLER; break;
	case rofl::openflow12::OFP_VERSION: out_port = rofl::openflow12::OFPP_CONTROLLER; break;
	case rofl::openflow13::OFP_VERSION: out_port = rofl::openflow13::OFPP_CONTROLLER; break;
	default:
		throw rofl::eBadVersion();
	}

	if (flood_port != out_port_no) {
		rofl::cflowentry fe(dpt->get_version());

		fe.set_command(command);
		fe.set_table_id(0);
		fe.set_hard_timeout(entry_timeout);
		fe.match.set_eth_dst(dst);

		fe.instructions.add_inst_apply_actions();
		fe.instructions.get_inst_apply_actions().get_actions().next() = rofl::cofaction_output(dpt->get_version(), out_port_no);

		rofbase->send_flow_mod_message(dpt, fe);

	} else {

		rofl::cflowentry fe(dpt->get_version());

		fe.set_command(command);
		fe.set_table_id(0);
		fe.set_hard_timeout(entry_timeout);
		fe.match.set_eth_src(dst);

		fe.instructions.add_inst_apply_actions().get_actions().next() = rofl::cofaction_output(dpt->get_version(), out_port);

		rofbase->send_flow_mod_message(dpt, fe);

	}
}



void
cfibentry::flow_mod_modify()
{
	rofl::cflowentry fe(dpt->get_version());

	uint8_t command = 0;
	switch (dpt->get_version()) {
	case rofl::openflow10::OFP_VERSION: command = rofl::openflow10::OFPFC_MODIFY_STRICT; break;
	case rofl::openflow12::OFP_VERSION: command = rofl::openflow12::OFPFC_MODIFY_STRICT; break;
	case rofl::openflow13::OFP_VERSION: command = rofl::openflow13::OFPFC_MODIFY_STRICT; break;
	default:
		throw rofl::eBadVersion();
	}

	fe.set_command(command);
	fe.set_table_id(0);
	fe.set_hard_timeout(entry_timeout);
	fe.match.set_eth_dst(dst);

	fe.instructions.add_inst_apply_actions().get_actions().next() = rofl::cofaction_output(dpt->get_version(), out_port_no);

	rofbase->send_flow_mod_message(dpt, fe);
}



void
cfibentry::flow_mod_delete()
{
	uint8_t command = 0;
	switch (dpt->get_version()) {
	case rofl::openflow10::OFP_VERSION: command = rofl::openflow10::OFPFC_DELETE_STRICT; break;
	case rofl::openflow12::OFP_VERSION: command = rofl::openflow12::OFPFC_DELETE_STRICT; break;
	case rofl::openflow13::OFP_VERSION: command = rofl::openflow13::OFPFC_DELETE_STRICT; break;
	default:
		throw rofl::eBadVersion();
	}

	uint32_t flood_port = 0;
	switch (dpt->get_version()) {
	case rofl::openflow10::OFP_VERSION: flood_port = rofl::openflow10::OFPP_FLOOD; break;
	case rofl::openflow12::OFP_VERSION: flood_port = rofl::openflow12::OFPP_FLOOD; break;
	case rofl::openflow13::OFP_VERSION: flood_port = rofl::openflow13::OFPP_FLOOD; break;
	default:
		throw rofl::eBadVersion();
	}

	if (flood_port != out_port_no) {

		rofl::cflowentry fe(dpt->get_version());

		fe.set_command(command);
		fe.set_table_id(0);
		fe.match.set_eth_dst(dst);

		rofbase->send_flow_mod_message(dpt, fe);

	} else {

		rofl::cflowentry fe(dpt->get_version());

		fe.set_command(command);
		fe.set_table_id(0);
		fe.match.set_eth_src(dst);

		rofbase->send_flow_mod_message(dpt, fe);

	}
}



