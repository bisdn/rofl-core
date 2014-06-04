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
	return timer;
}


ctimerid const&
ctimers::add_timer(ctimer const& t)
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	std::multiset<ctimer>::iterator it = timers.insert(t);
	return it->get_timer_id();
}


ctimerid const&
ctimers::reset(ctimerid const& timer_id, const ctimespec& timespec)
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	std::multiset<ctimer>::iterator it;
	if ((it = find_if(timers.begin(), timers.end(), ctimer::ctimer_find_by_timer_id(timer_id))) == timers.end()) {
		throw eTimersNotFound();
	}
	return timers.insert(ctimer(it->get_ptrciosrv(), it->get_opaque(), timespec))->get_timer_id();
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
ctimers::pending(ctimerid const& timer_id)
{
	RwLock lock(rwlock, RwLock::RWLOCK_READ);
	std::set<ctimer>::iterator it;
	if ((it = find_if(timers.begin(), timers.end(), ctimer::ctimer_find_by_timer_id(timer_id))) == timers.end()) {
		return false;
	}
	return true;
}


void
ctimers::cancel(ctimerid const& timer_id)
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	//rofl::logging::debug << "[rofl][ctimers][0] cancel: " << std::endl << timer_id;
	//rofl::logging::debug << "[rofl][ctimers][1] cancel: " << std::endl << *this;
	std::set<ctimer>::iterator it;
	if ((it = find_if(timers.begin(), timers.end(), ctimer::ctimer_find_by_timer_id(timer_id))) == timers.end()) {
		return;
	}
	timers.erase(it);
	//rofl::logging::debug << "[rofl][ctimers][2] cancel: " << std::endl << *this;
}


void
ctimers::cancel_all()
{
	clear();
}



void
ctimers::clear()
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	timers.clear();
}


