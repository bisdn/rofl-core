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
				work_interval(1),
				ta_queue_timer_id(0)
{

}



ctransactions::~ctransactions()
{

}



void
ctransactions::handle_timeout(
		int opaque, void *data)
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
		std::cerr << "===============" << std::endl << *this;
		env->ta_expired(*this, *it);
		erase(it);
		it = begin();
	}

	if (not (*this).empty()) {
		ta_queue_timer_id = register_timer(TIMER_WORK_ON_TA_QUEUE, work_interval);
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

	if (not pending_timer(ta_queue_timer_id)) {
		ta_queue_timer_id = register_timer(TIMER_WORK_ON_TA_QUEUE, work_interval);
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

	if ((*this).empty() && pending_timer(ta_queue_timer_id)) {
		cancel_timer(ta_queue_timer_id);
	}
}



