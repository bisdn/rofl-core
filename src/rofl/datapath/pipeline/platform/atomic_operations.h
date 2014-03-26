/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __PLATFORM_ATOMIC_OPERATIONS_H__
#define __PLATFORM_ATOMIC_OPERATIONS_H__

#include <inttypes.h>
#include "rofl.h"
#include "lock.h"

#if defined(__COMPILING_ROFL_PIPELINE__) || !defined(ROFL_PIPELINE_INLINE_PP_PLATFORM_FUNCS)

/**
* @file atomic_operations.h
* @author Victor Alvarez<victor.alvarez (at) bisdn.de>
*
* @brief Defines the atomic operations API, mostly atomic increments and 
* addition operations. 
*
* The atomic operations API is the set of calls used by library
* to perform atomic increments 

* The user MUST provide an implementation for his/her particular
* platform. 
*
*/

//C++ extern C
ROFL_BEGIN_DECLS

/**
* @brief Performs an atomic increment to the counter (64 bit type). 
* @ingroup platform_atomic
*
* The implementation may or may not use the mutex that covers the
* scope of the counter.
*
* @param mutex mutex instance over the scope of counter.
* 
*/
void platform_atomic_inc64(uint64_t* counter, platform_mutex_t* mutex);

/**
* @brief Performs an atomic increment to the counter (32 bit type). 
* @ingroup platform_atomic
*
* The implementation may or may not use the mutex that covers the
* scope of the counter.
*
* @param mutex mutex instance over the scope of counter.
* 
*/
void platform_atomic_inc32(uint32_t* counter, platform_mutex_t* mutex);

/**
* @brief Performs an atomic decrement of the counter (32 bit type). 
* @ingroup platform_atomic
*
* The implementation may or may not use the mutex that covers the
* scope of the counter.
*
* @param mutex mutex instance over the scope of counter.
* 
*/
void platform_atomic_dec32(uint32_t* counter, platform_mutex_t* mutex);
	
/**
* @brief Performs an atomic addition to the counter (64 bit type). 
* @ingroup platform_atomic
*
* The implementation may or may not use the mutex that covers the
* scope of the counter.
*
* @param mutex mutex instance over the scope of counter.
* 
*/
void platform_atomic_add64(uint64_t* counter, uint64_t value, platform_mutex_t* mutex);

/**
* @brief Performs an atomic addition to the counter (32 bit type). 
* @ingroup platform_atomic
*
* The implementation may or may not use the mutex that covers the
* scope of the counter.
*
* @param mutex mutex instance over the scope of counter.
* 
*/
void platform_atomic_add32(uint32_t* counter, uint32_t value, platform_mutex_t* mutex);

//C++ extern C
ROFL_END_DECLS

#endif //if
#endif //__PLATFORM_ATOMIC_OPERATIONS_H__
