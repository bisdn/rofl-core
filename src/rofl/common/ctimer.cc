/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ctimer.cc
 *
 *  Created on: 26.01.2014
 *      Author: andreas
 */

#include "ctimer.h"

using namespace rofl;


ctimer
ctimer::now()
{
	ctimer timer;
	timer.set_timespec() = ctimespec::now();
	return timer;
}


ctimer::ctimer() :
		ptr(0),
		opaque(0),
		data(0)
{

}


ctimer::ctimer(ptrciosrv* ptr, int opaque, const ctimespec& timespec, void* data) :
		ptr(ptr),
		timespec(timespec),
		opaque(opaque),
		data(data)
{
	this->timespec += ctimespec::now();
}


ctimer::ctimer(ctimer const& timer)
{
	*this = timer;
}


ctimer&
ctimer::operator= (ctimer const& timer)
{
	if (this == &timer)
		return *this;

	timer_id	= timer.timer_id;
	ptr			= timer.ptr;
	timespec	= timer.timespec;
	opaque		= timer.opaque;
	data		= timer.data;

	return *this;
}


ctimer::~ctimer()
{

}


#if 0
ctimer
ctimer::operator+ (ctimer const& t)
{
	ctimer timer;
	timer.timespec = timespec + t.timespec;
	return timer;
}



ctimer
ctimer::operator- (ctimer const& t)
{
	ctimer timer;
	timer.timespec = timespec - t.timespec;
	return timer;
}



ctimer&
ctimer::operator+= (ctimer const& t)
{
	timespec += t.timespec;
	return *this;
}



ctimer&
ctimer::operator-= (ctimer const& t)
{
	timespec -= t.timespec;
	return *this;
}
#endif


bool
ctimer::operator!= (ctimer const& t) const
{
	return not (*this == t);
}



bool
ctimer::operator< (ctimer const& t) const
{
	return (timespec < t.timespec);
}



bool
ctimer::operator<= (ctimer const& t) const
{
	return ((*this < t) || (*this == t));
}


bool
ctimer::operator> (ctimer const& t) const
{
	return not (*this <= t);
}


bool
ctimer::operator>= (ctimer const& t) const
{
	return ((*this > t) || (*this == t));
}



bool
ctimer::operator== (ctimer const& t) const
{
	return not ((*this < t) && (t < *this));
}


