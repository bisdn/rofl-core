/*
 * ctimers.cc
 *
 *  Created on: 26.01.2014
 *      Author: andreas
 */

#include "ctimers.h"

using namespace rofl;

ctimers::ctimers()
{

}


ctimers::ctimers(ctimers const& timers)
{
	*this = timers;
}


ctimers&
ctimers::operator= (ctimers const& t)
{
	if (this == &t)
		return *this;

	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	timers.clear();
	timers.insert(t.timers.begin(), t.timers.end());

	return *this;
}


ctimers::~ctimers()
{

}


bool
ctimers::empty()
{
	RwLock lock(rwlock, RwLock::RWLOCK_READ);
	return (timers.empty());
}


ctimer
ctimers::get_next_timer()
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	if (timers.empty())
		throw eTimersNotFound();
	std::multiset<ctimer>::iterator first = timers.begin();
	ctimer timer(*first);
	timers.erase(first);
	return timer;
}


uint32_t
ctimers::add_timer(ctimer const& t)
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	timers.insert(t);
	return t.get_timer_id();
}


uint32_t
ctimers::reset(uint32_t timer_id, time_t t)
{
	ctimer timer;
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	std::set<ctimer>::iterator it;
	if ((it = find_if(timers.begin(), timers.end(), ctimer::ctimer_find_by_timer_id(timer_id))) == timers.end()) {
		throw eTimersNotFound();
	}
	timer = (*it);
	timers.erase(it);
	timer.get_ts().tv_sec = t;
	timers.insert(timer);
	return timer_id;
}


ctimer
ctimers::get_expired_timer()
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	if (timers.empty()) {
		throw eTimersNotFound();
	}
	ctimer now = ctimer::now();

	std::set<ctimer>::iterator it = timers.begin();

	ctimer timer = *(it);

	if (timer > now) {
		throw eTimersNotFound();
	}

	timers.erase(it);

	return timer;
}


bool
ctimers::pending(uint32_t timer_id)
{
	RwLock lock(rwlock, RwLock::RWLOCK_READ);
	std::set<ctimer>::iterator it;
	if ((it = find_if(timers.begin(), timers.end(), ctimer::ctimer_find_by_timer_id(timer_id))) == timers.end()) {
		return false;
	}
	return true;
}


void
ctimers::cancel(uint32_t timer_id)
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	std::set<ctimer>::iterator it;
	if ((it = find_if(timers.begin(), timers.end(), ctimer::ctimer_find_by_timer_id(timer_id))) == timers.end()) {
		return;
	}
	timers.erase(it);
}


void
ctimers::cancel_all()
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	timers.clear();
}


