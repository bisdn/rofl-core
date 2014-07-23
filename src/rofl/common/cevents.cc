/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cevents.cc
 *
 *  Created on: 26.01.2014
 *      Author: andreas
 */

#include "cevents.h"

using namespace rofl;

cevents::cevents()
{

}


cevents::cevents(cevents const& events)
{
	*this = events;
}


cevents&
cevents::operator= (cevents const& evs)
{
	if (this == &evs)
		return *this;

	RwLock rwlock1(rwlock, RwLock::RWLOCK_WRITE);
	// TODO: RwLock rwlock2(evs.rwlock, RwLock::RWLOCK_READ);
	events.clear();
	events.insert(events.begin(), evs.events.begin(), evs.events.end());

	return *this;
}


cevents::~cevents()
{

}


void
cevents::add_event(cevent const& event)
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	events.push_back(event);
}


cevent
cevents::get_event()
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	if (events.empty()) {
		throw eEventsNotFound();
	}
	cevent event = events.front(); events.pop_front();
	return event;
}


bool
cevents::empty()
{
	RwLock lock(rwlock, RwLock::RWLOCK_READ);
	return events.empty();
}



void
cevents::clear()
{
	RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
	events.clear();
}


