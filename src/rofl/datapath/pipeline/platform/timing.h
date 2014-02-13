/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __PLATFORM_TIME_H__
#define __PLATFORM_TIME_H__

/**
* @file timing.h
* @author Victor Alvarez<victor.alvarez (at) bisdn.de>
*
* @brief Defines the timing interface used by the library. 
* The user of the library MUST provide an implementation for his/her platform.
* 
*/

struct timeval;

//C++ extern C
ROFL_BEGIN_DECLS

/* MUTEX operations */

/**
* @brief Gets the system time.
* @ingroup platform_timing
*
* platform_gettimeofday() has the same semantic as POSIXs
* gettimeofday. Gets the time and stores it in the structure timeval
*/
int platform_gettimeofday(struct timeval* tval);

//C++ extern C
ROFL_END_DECLS


#endif //__PLATFORM_TIME_H__