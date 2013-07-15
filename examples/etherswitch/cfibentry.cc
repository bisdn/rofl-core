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
	if (OFPP_FLOOD != out_port_no) {
		rofl::cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_ADD);
		fe.set_table_id(0);
		fe.set_hard_timeout(entry_timeout);
		fe.match.set_eth_dst(dst);

		fe.instructions.next() = rofl::cofinst_apply_actions();
		fe.instructions.back().actions.next() = rofl::cofaction_output(out_port_no);

		rofbase->send_flow_mod_message(dpt, fe);

	} else {

		rofl::cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_ADD);
		fe.set_table_id(0);
		fe.set_hard_timeout(entry_timeout);
		fe.match.set_eth_src(dst);

		fe.instructions.next() = rofl::cofinst_apply_actions();
		fe.instructions.back().actions.next() = rofl::cofaction_output(OFPP_CONTROLLER);

		rofbase->send_flow_mod_message(dpt, fe);

	}
}



void
cfibentry::flow_mod_modify()
{
	rofl::cflowentry fe(dpt->get_version());

	fe.set_command(OFPFC_MODIFY_STRICT);
	fe.set_table_id(0);
	fe.set_hard_timeout(entry_timeout);
	fe.match.set_eth_dst(dst);

	fe.instructions.next() = rofl::cofinst_apply_actions();
	fe.instructions.back().actions.next() = rofl::cofaction_output(out_port_no);

	rofbase->send_flow_mod_message(dpt, fe);
}



void
cfibentry::flow_mod_delete()
{
	if (OFPP_FLOOD != out_port_no) {

		rofl::cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_DELETE_STRICT);
		fe.set_table_id(0);
		fe.match.set_eth_dst(dst);

		rofbase->send_flow_mod_message(dpt, fe);

	} else {

		rofl::cflowentry fe(dpt->get_version());

		fe.set_command(OFPFC_DELETE_STRICT);
		fe.set_table_id(0);
		fe.match.set_eth_src(dst);

		rofbase->send_flow_mod_message(dpt, fe);

	}
}



