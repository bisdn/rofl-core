/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cclock.h
 *
 *  Created on: 07.07.2012
 *      Author: andreas
 */
#ifndef CCLOCK_H_
#define CCLOCK_H_ 

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <string.h>
#include <time.h>

#ifdef __cplusplus
}
#endif

#include "cclock.h"
#include "cvastring.h"
#include "../platform/unix/csyslog.h"


class cclock :
	public csyslog
{
private: // data structures

	std::string info;

public: // data structures

	struct timespec ts;

public:

	/** constructor
	 *
	 */
	cclock(
			time_t delta_sec = 0,
			time_t delta_nsec = 0);

	/** copy constructor
	 *
	 */
	cclock(
			cclock const& cc);

	/** destructor
	 *
	 */
	~cclock();
	/** assignment operator
	 *
	 */
	cclock&
	operator= (cclock const& cc);
	/** operator+
	 *
	 */
	cclock
	operator+ (cclock const& cc);
	/** operator-
	 *
	 */
	cclock
	operator- (cclock const& cc);
	/** operator+=
	 *
	 */
	cclock&
	operator+= (cclock const& cc);
	/** operator-=
	 *
	 */
	cclock&
	operator-= (cclock const& cc);
	/** operator<
	 *
	 */
	bool operator< (cclock const& cc);
	/**
	 *
	 */
	void
	now();
	/**
	 *
	 */
	const char*
	c_str();
};


#endif /* CCLOCK_H_ */
