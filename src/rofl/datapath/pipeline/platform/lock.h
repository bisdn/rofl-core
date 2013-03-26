/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __PLATFORM_LOCK_H__
#define __PLATFORM_LOCK_H__

#include "../util/rofl_pipeline_utils.h"

/**
* @file lock.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Defines the locking interface used by the library. 
* The user of the library MUST provide an implementation for his/her platform.
*
* The locking API is the set of calls used by library
* to perform mutual exclusion operations. It is pretty much a standard
* POSIX-like interface, except for minor details. 
*
* The user MUST provide an implementation for his/her particular
* platform. 
*
* In a typical POSIX/libc system this would probably be mapped straight
* forward to pthread_mutex and pthread_rwlock calls.
* 
*/

typedef void platform_mutex_t;
typedef void platform_rwlock_t;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/* MUTEX operations */

/**
* @brief Allocates and initializes a mutex
* @ingroup platform_lock
*
* platform_mutex_init() has a slightly different semantic as POSIXs
* mutex_init. platform_mutex_init() must allocate the memory for 
* the mutex AND then perform the mutex initialization (POSIX's mutex_init).
*/
platform_mutex_t* platform_mutex_init(void* params);

/**
* @brief Destroys and deallocates a mutex previously inited by 
* platform_mutex_init() 
* @ingroup platform_lock
*
* platform_mutex_destroy() has a slightly different semantic as POSIXs
* mutex_destroy. platform_mutex_desttroy() must first destroy (POSIX's
* mutex_destroy) the lock AND then release the memory previously allocated.
*/
void platform_mutex_destroy(platform_mutex_t* mutex);

//Operations
/**
* @brief Locks the platform_mutex_t mutex
* platform_mutex_init() 
* @ingroup platform_lock
*
* This has the same semantic as POSIX call.
*/
void platform_mutex_lock(platform_mutex_t* mutex);

/**
* @brief Unlocks the platform_mutex_t mutex
* platform_mutex_init() 
* @ingroup platform_lock
*
* This has the same semantic as POSIX call.
*/
void platform_mutex_unlock(platform_mutex_t* mutex);

/* RWLOCK */
//Init&destroy

/**
* @brief Allocates and initializes a rwlock
* @ingroup platform_lock
*
* platform_rwlock_init() has a slightly different semantic as POSIXs
* rwlock_init. platform_mutex_init() must allocate the memory for the
* rwlock AND then perform the mutex initialization (POSIX's rwlock_init).
*/
platform_rwlock_t* platform_rwlock_init(void* params);

/**
* @brief Destroys and deallocates a rwlock previously inited by 
* platform_rwlock_init()
* @ingroup platform_lock
*
* platform_rwlock_destroy() has a slightly different semantic as POSIXs
* rwlock_destroy. platform_rwlcok_destroy() must first destroy the lock (POSIX's
* rwlock_destroy) AND then release the memory previously allocated.
*/
void platform_rwlock_destroy(platform_rwlock_t* rwlock);

//Read
/**
* @brief Performs a read-lock over the platform_rwlock_t mutex
* platform_mutex_init() 
* @ingroup platform_lock
*
* This has the same semantic as POSIX call.
*/
void platform_rwlock_rdlock(platform_rwlock_t* rwlock);

/**
* @brief Performs a read-unlock over the platform_rwlock_t mutex
* platform_mutex_init() 
* @ingroup platform_lock
*
* This has the same semantic as POSIX call.
*/
void platform_rwlock_rdunlock(platform_rwlock_t* rwlock);

//Write
/**
* @brief Performs a write-lock over the platform_rwlock_t mutex
* platform_mutex_init() 
* @ingroup platform_lock
*
* This has the same semantic as POSIX call.
*/
void platform_rwlock_wrlock(platform_rwlock_t* rwlock);

/**
* @brief Performs a write-unlock over the platform_rwlock_t mutex
* platform_mutex_init() 
* @ingroup platform_lock
*
* This has the same semantic as POSIX call.
*/
void platform_rwlock_wrunlock(platform_rwlock_t* rwlock);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //PLATFORM_LOCK
