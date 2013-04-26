/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ROFL_PIPELINE_TIME_H_
#define ROFL_PIPELINE_TIME_H_

/**
* @author Marc Sune <marc.sune (at) bisdn.de>
* @brief Defines useful MACROS for portability
*/

#define TIMERSUB(x, y, diff)\
	do{\
		(diff)->tv_sec = (x)->tv_sec-(y)->tv_sec;\
		(diff)->tv_usec = (x)->tv_usec-(y)->tv_usec;\
		\
		if((diff)->tv_usec < 0) {\
			(diff)->tv_sec--;\
			(diff)->tv_usec += 1000000;\
		}\
	}while(0)

#endif //ROFL_PIPELINE_TIME
