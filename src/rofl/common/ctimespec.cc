/*
 * ctimespec.cc
 *
 *  Created on: 04.06.2014
 *      Author: andreas
 */

#include "ctimespec.h"

using namespace rofl;

ctimespec::ctimespec()
{
	memset(&ts, 0, sizeof(ts));
}


ctimespec::ctimespec(long tv_sec)
{
	ts.tv_sec 	= tv_sec;
	ts.tv_nsec 	= 0;
}


ctimespec::ctimespec(long tv_sec, long tv_nsec)
{
	ts.tv_sec 	= tv_sec;
	ts.tv_nsec 	= tv_nsec;
}


ctimespec::ctimespec(const ctimespec& timespec)
{
	*this = timespec;
}


ctimespec&
ctimespec::operator= (const ctimespec& timespec)
{
	if (this == &timespec)
		return *this;

	ts.tv_sec	= timespec.ts.tv_sec;
	ts.tv_nsec	= timespec.ts.tv_nsec;

	return *this;
}


ctimespec::~ctimespec()
{

}



ctimespec
ctimespec::now()
{
	ctimespec timespec;
	if (clock_gettime(CLOCK_MONOTONIC, &(timespec.ts)) < 0) {
		throw eSysCall("clock_gettime()");
	}
	return timespec;
}


ctimespec
ctimespec::operator+ (const ctimespec& t)
{
	ctimespec timespec;

	timespec.ts.tv_sec		= ts.tv_sec  + t.ts.tv_sec;
	timespec.ts.tv_nsec 	= ts.tv_nsec + t.ts.tv_nsec;

	if (timespec.ts.tv_nsec > CC_TIMER_ONE_SECOND_NS) {
		timespec.ts.tv_sec 	+=  CC_TIMER_ONE_SECOND_S;
		timespec.ts.tv_nsec -=  CC_TIMER_ONE_SECOND_NS;
	}

	return timespec;
}



ctimespec
ctimespec::operator- (const ctimespec& t)
{
	ctimespec timespec;

	if (t.ts.tv_nsec > ts.tv_nsec) {
		timespec.ts.tv_nsec = ts.tv_nsec - t.ts.tv_nsec + CC_TIMER_ONE_SECOND_NS;
		timespec.ts.tv_sec	= ts.tv_sec  - t.ts.tv_sec  - CC_TIMER_ONE_SECOND_S;
	} else {
		timespec.ts.tv_nsec	= ts.tv_nsec - t.ts.tv_nsec;
		timespec.ts.tv_sec	= ts.tv_sec  - t.ts.tv_sec;
	}

	return timespec;
}



ctimespec&
ctimespec::operator+= (const ctimespec& t)
{
	ts.tv_sec	+= t.ts.tv_sec;
	ts.tv_nsec 	+= t.ts.tv_nsec;

	if (ts.tv_nsec >  CC_TIMER_ONE_SECOND_NS) {
		ts.tv_sec 	+= CC_TIMER_ONE_SECOND_S;
		ts.tv_nsec 	-= CC_TIMER_ONE_SECOND_NS;
	}

	return *this;
}



ctimespec&
ctimespec::operator-= (const ctimespec& t)
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
ctimespec::operator!= (const ctimespec& t) const
{
	return not (*this == t);
}



bool
ctimespec::operator< (const ctimespec& t) const
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
ctimespec::operator<= (const ctimespec& t) const
{
	return ((*this < t) || (*this == t));
}


bool
ctimespec::operator> (const ctimespec& t) const
{
	if (ts.tv_sec > t.ts.tv_sec) {
		return true;
	} else
	if (ts.tv_sec < t.ts.tv_sec) {
		return false;
	}

	// here: ts.tv_sec == t.ts.tv_sec

	if (ts.tv_nsec > t.ts.tv_nsec) {
		return true;
	} else
	if (ts.tv_nsec < t.ts.tv_nsec) {
		return false;
	}

	// here: ts.tv_nsec == t.ts.tv_nsec

	return false;
}


bool
ctimespec::operator>= (const ctimespec& t) const
{
	return ((*this > t) || (*this == t));
}



bool
ctimespec::operator== (const ctimespec& t) const
{
	return not ((*this < t) && (t < *this));
}




