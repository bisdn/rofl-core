/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cclock.cc
 *
 *  Created on: 07.07.2012
 *      Author: andreas
 */

#include "cclock.h"

using namespace rofl;

#define CC_CLOCK_ONE_SECOND_S 1
#define CC_CLOCK_ONE_SECOND_NS 1000000000



cclock
cclock::now()
{
	cclock clk; clk.current_time();
	return clk;
}



cclock::cclock(
		time_t delta_sec,
		time_t delta_nsec)
{
	current_time();
	ts.tv_sec += delta_sec;
	ts.tv_nsec += delta_nsec;
}



cclock::cclock(const cclock& clk)
{
	*this = clk;
}



cclock::~cclock()
{

}



cclock&
cclock::operator =(const cclock& cc)
{
	if (this == &cc)
		return *this;

	ts.tv_sec 	= cc.ts.tv_sec;
	ts.tv_nsec	= cc.ts.tv_nsec;

	return *this;
}



cclock
cclock::operator+ (cclock const& cc)
{
	cclock clock;

	clock.ts.tv_sec		= ts.tv_sec  + cc.ts.tv_sec;
	clock.ts.tv_nsec 	= ts.tv_nsec + cc.ts.tv_nsec;

	if (clock.ts.tv_nsec > CC_CLOCK_ONE_SECOND_NS)
	{
		clock.ts.tv_sec +=  CC_CLOCK_ONE_SECOND_S;
		clock.ts.tv_nsec -=  CC_CLOCK_ONE_SECOND_NS;
	}

	return clock;
}



cclock
cclock::operator- (cclock const& cc)
{
	cclock clock;

	if (cc.ts.tv_nsec > ts.tv_nsec)
	{
		clock.ts.tv_nsec 	= ts.tv_nsec - cc.ts.tv_nsec + CC_CLOCK_ONE_SECOND_NS;
		clock.ts.tv_sec		= ts.tv_sec  - cc.ts.tv_sec  -  CC_CLOCK_ONE_SECOND_S;
	}
	else
	{
		clock.ts.tv_nsec	= ts.tv_nsec - cc.ts.tv_nsec;
		clock.ts.tv_sec		= ts.tv_sec  - cc.ts.tv_sec;
	}

	return clock;
}



cclock&
cclock::operator+= (cclock const& cc)
{
	ts.tv_sec	+= cc.ts.tv_sec;
	ts.tv_nsec 	+= cc.ts.tv_nsec;

	if (ts.tv_nsec >  CC_CLOCK_ONE_SECOND_NS)
	{
		ts.tv_sec += CC_CLOCK_ONE_SECOND_S;
		ts.tv_nsec -= CC_CLOCK_ONE_SECOND_NS;
	}

	return *this;
}



cclock&
cclock::operator-= (cclock const& cc)
{
	if (cc.ts.tv_nsec > ts.tv_nsec)
	{
		ts.tv_nsec 	= ts.tv_nsec - cc.ts.tv_nsec + CC_CLOCK_ONE_SECOND_NS;
		ts.tv_sec	= ts.tv_sec  - cc.ts.tv_sec  - CC_CLOCK_ONE_SECOND_S;
	}
	else
	{
		ts.tv_nsec	= ts.tv_nsec - cc.ts.tv_nsec;
		ts.tv_sec	= ts.tv_sec  - cc.ts.tv_sec;
	}

	return *this;
}



bool
cclock::operator!= (cclock const& clk) const
{
	return not (*this == clk);
}



bool
cclock::operator< (cclock const& cc) const
{
	if (ts.tv_sec < cc.ts.tv_sec)
	{
		return true;
	}
	else if (ts.tv_sec > cc.ts.tv_sec)
	{
		return false;
	}

	// here: ts.tv_sec == cc.ts.tv_sec

	if (ts.tv_nsec < cc.ts.tv_nsec)
	{
		return true;
	}
	else if (ts.tv_nsec > cc.ts.tv_nsec)
	{
		return false;
	}

	// here: ts.tv_nsec == cc.ts.tv_nsec

	return false;
}



bool
cclock::operator<= (cclock const& cc) const
{
	return ((*this < cc) || (*this == cc));
}


bool
cclock::operator> (cclock const& cc) const
{
	return not (*this <= cc);
}


bool
cclock::operator>= (cclock const& cc) const
{
	return ((*this > cc) || (*this == cc));
}



bool
cclock::operator== (cclock const& cc) const
{
	return not ((*this < cc) && (cc < *this));
}



void
cclock::current_time()
{
	if (clock_gettime(CLOCK_REALTIME, &ts) < 0) {
		LOGGING_ERROR << "cclock: failed to read time: "
				<< eSysCall("clock_gettime") << std::endl;
	}
}


