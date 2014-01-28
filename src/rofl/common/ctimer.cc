/*
 * ctimer.cc
 *
 *  Created on: 26.01.2014
 *      Author: andreas
 */

#include "ctimer.h"

using namespace rofl;

uint32_t ctimer::next_timer_id = 0;

uint32_t
ctimer::get_next_timer_id() {
	if (++ctimer::next_timer_id == 0) {
		++ctimer::next_timer_id;
	}
	return ctimer::next_timer_id;
}


ctimer::ctimer() :
		timer_id(0),
		ptr(0),
		opaque(0),
		data(0)
{
	ts.tv_sec 	= 0;
	ts.tv_nsec 	= 0;
}


ctimer::ctimer(ptrciosrv* ptr, int opaque, long tv_sec) :
		timer_id(++next_timer_id),
		ptr(ptr),
		opaque(opaque),
		data(0)
{
	ts.tv_sec 	= tv_sec;
	ts.tv_nsec 	= 0;
	*this += ctimer::now();
}


ctimer::ctimer(ptrciosrv* ptr, int opaque, long tv_sec, long tv_nsec) :
		timer_id(++next_timer_id),
		ptr(ptr),
		opaque(opaque),
		data(0)
{
	ts.tv_sec 	= tv_sec;
	ts.tv_nsec 	= tv_nsec;
	*this += ctimer::now();
}


ctimer::ctimer(ptrciosrv* ptr, int opaque, long tv_sec, long tv_nsec, void *data) :
		timer_id(++next_timer_id),
		ptr(ptr),
		opaque(opaque),
		data(data)
{
	ts.tv_sec 	= tv_sec;
	ts.tv_nsec 	= tv_nsec;
	*this += ctimer::now();
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
	ts.tv_sec	= timer.ts.tv_sec;
	ts.tv_nsec	= timer.ts.tv_nsec;
	opaque		= timer.opaque;
	data		= timer.data;

	return *this;
}


ctimer::~ctimer()
{

}



ctimer
ctimer::now()
{
	ctimer timer;
	if (clock_gettime(CLOCK_MONOTONIC, &(timer.ts)) < 0) {
		throw eSysCall("clock_gettime()");
	}
	return timer;
}


ctimer
ctimer::operator+ (ctimer const& t)
{
	ctimer timer;

	timer.ts.tv_sec		= ts.tv_sec  + t.ts.tv_sec;
	timer.ts.tv_nsec 	= ts.tv_nsec + t.ts.tv_nsec;

	if (timer.ts.tv_nsec > CC_TIMER_ONE_SECOND_NS) {
		timer.ts.tv_sec 	+=  CC_TIMER_ONE_SECOND_S;
		timer.ts.tv_nsec 	-=  CC_TIMER_ONE_SECOND_NS;
	}

	return timer;
}



ctimer
ctimer::operator- (ctimer const& t)
{
	ctimer timer;

	if (t.ts.tv_nsec > ts.tv_nsec) {
		timer.ts.tv_nsec 	= ts.tv_nsec - t.ts.tv_nsec + CC_TIMER_ONE_SECOND_NS;
		timer.ts.tv_sec		= ts.tv_sec  - t.ts.tv_sec  - CC_TIMER_ONE_SECOND_S;
	} else {
		timer.ts.tv_nsec	= ts.tv_nsec - t.ts.tv_nsec;
		timer.ts.tv_sec		= ts.tv_sec  - t.ts.tv_sec;
	}

	return timer;
}



ctimer&
ctimer::operator+= (ctimer const& t)
{
	ts.tv_sec	+= t.ts.tv_sec;
	ts.tv_nsec 	+= t.ts.tv_nsec;

	if (ts.tv_nsec >  CC_TIMER_ONE_SECOND_NS) {
		ts.tv_sec 	+= CC_TIMER_ONE_SECOND_S;
		ts.tv_nsec 	-= CC_TIMER_ONE_SECOND_NS;
	}

	return *this;
}



ctimer&
ctimer::operator-= (ctimer const& t)
{
	if (t.ts.tv_nsec > ts.tv_nsec) {
		ts.tv_nsec 	= ts.tv_nsec - t.ts.tv_nsec + CC_TIMER_ONE_SECOND_NS;
		ts.tv_sec	= ts.tv_sec  - t.ts.tv_sec  - CC_TIMER_ONE_SECOND_S;
	} else {
		ts.tv_nsec	= ts.tv_nsec - t.ts.tv_nsec;
		ts.tv_sec	= ts.tv_sec  - t.ts.tv_sec;
	}

	return *this;
}



bool
ctimer::operator!= (ctimer const& t) const
{
	return not (*this == t);
}



bool
ctimer::operator< (ctimer const& t) const
{
	if (ts.tv_sec < t.ts.tv_sec) {
		return true;
	} else
	if (ts.tv_sec > t.ts.tv_sec) {
		return false;
	}

	// here: ts.tv_sec == t.ts.tv_sec

	if (ts.tv_nsec < t.ts.tv_nsec) {
		return true;
	} else
	if (ts.tv_nsec > t.ts.tv_nsec) {
		return false;
	}

	// here: ts.tv_nsec == t.ts.tv_nsec

	return false;
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


