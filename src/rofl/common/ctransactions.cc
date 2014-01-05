/*
 * ctransactions.cc
 *
 *  Created on: 05.01.2014
 *      Author: andi
 */

#include "ctransactions.h"

using namespace rofl::openflow;

ctransactions::ctransactions(
		ctransactions_env *env) :
				env(env),
				nxid(crandom(sizeof(uint32_t)).uint32()),
				work_interval(1)
{

}



virtual void
ctransactions::handle_timeout(
		int opaque)
{
	switch (opaque) {
	case TIMER_WORK_ON_TA_QUEUE: {
		work_on_ta_queue();
	} break;
	}
}



void
ctransactions::work_on_ta_queue()
{
	// TODO

	if (not empty()) {
		register_timer(TIMER_WORK_ON_TA_QUEUE, work_interval);
	}
}



uint32_t
ctransactions::add_ta(
		cclock const& delta)
{
	++nxid;

	(*this).push_back(ctransaction(nxid, delta));

	if (not pending_timer(TIMER_WORK_ON_TA_QUEUE)) {
		register_timer(TIMER_WORK_ON_TA_QUEUE, work_interval);
	}

	return nxid;
}



void
ctransactions::drop_ta(
		uint32_t xid)
{
	// TODO

	if (this->empty() && pending_timer(TIMER_WORK_ON_TA_QUEUE)) {
		cancel_timer(TIMER_WORK_ON_TA_QUEUE);
	}
}



