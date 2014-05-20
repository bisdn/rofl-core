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
		rofl::crofdpt *dpt,
		rofl::cmacaddr dst,
		uint32_t out_port_no) :
		fib(fib),
		out_port_no(out_port_no),
		dst(dst),
		rofbase(rofbase),
		dpt(dpt),
		entry_timeout(CFIBENTRY_DEFAULT_TIMEOUT),
		expiration_timer_id()
{
	expiration_timer_id = register_timer(CFIBENTRY_ENTRY_EXPIRED, entry_timeout);
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
#if 0
	flow_mod_delete();
#endif

	this->out_port_no = out_port_no;

	try {
		reset_timer(expiration_timer_id, entry_timeout);
	} catch (rofl::eTimersBase& e) {
		rofl::logging::info << "TIMERSSSSSSSSSSSSSSS!!!!" << std::endl;
		register_timer(CFIBENTRY_ENTRY_EXPIRED, entry_timeout);
	}

#if 0
	flow_mod_add();
#endif
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
		rofl::openflow::cofflowmod fe(dpt->get_version());

		fe.set_command(command);
		fe.set_table_id(0);
		fe.set_hard_timeout(entry_timeout);
		fe.match.set_eth_dst(dst);

		fe.instructions.set_inst_apply_actions().get_actions().append_action_output(out_port_no);

		dpt->send_flow_mod_message(rofl::cauxid(0), fe);

	} else {

		rofl::openflow::cofflowmod fe(dpt->get_version());

		fe.set_command(command);
		fe.set_table_id(0);
		fe.set_hard_timeout(entry_timeout);
		fe.match.set_eth_src(dst);

		fe.instructions.set_inst_apply_actions().get_actions().append_action_output(out_port);

		dpt->send_flow_mod_message(rofl::cauxid(0), fe);

	}
}



void
cfibentry::flow_mod_modify()
{
	rofl::openflow::cofflowmod fe(dpt->get_version());

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

	fe.instructions.set_inst_apply_actions().get_actions().append_action_output(out_port_no);

	dpt->send_flow_mod_message(rofl::cauxid(0), fe);
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

		rofl::openflow::cofflowmod fe(dpt->get_version());

		fe.set_command(command);
		fe.set_table_id(0);
		fe.match.set_eth_dst(dst);

		dpt->send_flow_mod_message(rofl::cauxid(0), fe);

	} else {

		rofl::openflow::cofflowmod fe(dpt->get_version());

		fe.set_command(command);
		fe.set_table_id(0);
		fe.match.set_eth_src(dst);

		dpt->send_flow_mod_message(rofl::cauxid(0), fe);

	}
}



