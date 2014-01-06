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



ctransactions::~ctransactions()
{

}



void
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
	RwLock lock(queuelock, RwLock::RWLOCK_WRITE);

	std::list<ctransaction>::iterator it = (*this).begin();
	while ((it != (*this).end()) && ((*it).get_expires() <= cclock::now())) {
		env->ta_expired(*this, (*it));
		(*this).erase(it);
	}

	if (not (*this).empty()) {
		register_timer(TIMER_WORK_ON_TA_QUEUE, work_interval);
	}
}



uint32_t
ctransactions::add_ta(
		cclock const& delta,
		uint8_t msg_type,
		uint16_t msg_sub_type)
{
	RwLock lock(queuelock, RwLock::RWLOCK_WRITE);

	++nxid;

	cclock expires(delta);

	std::list<ctransaction>::iterator it = begin();
	while ((it != end()) && ((*it).get_expires() < expires)) {
		++it;
	}
	(*this).insert(it, ctransaction(nxid, delta, msg_type, msg_sub_type));

	if (not pending_timer(TIMER_WORK_ON_TA_QUEUE)) {
		register_timer(TIMER_WORK_ON_TA_QUEUE, work_interval);
	}

	return nxid;
}



void
ctransactions::drop_ta(
		uint32_t xid)
{
	RwLock lock(queuelock, RwLock::RWLOCK_WRITE);

	for (std::list<ctransaction>::iterator
			it = begin(); it != end(); ++it) {
		if ((*it).get_xid() == xid) {
			(*this).erase(it);
			break;
		}
	}

	if ((*this).empty() && pending_timer(TIMER_WORK_ON_TA_QUEUE)) {
		cancel_timer(TIMER_WORK_ON_TA_QUEUE);
	}
}



