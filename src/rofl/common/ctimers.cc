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


void
ctimers::add_timer(ctimer const& t)
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	timers.insert(t);
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



