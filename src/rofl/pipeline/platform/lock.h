#ifndef __PLATFORM_LOCK_H__
#define __PLATFORM_LOCK_H__

#include "../util/rofl_pipeline_utils.h"

/*
*
* Header file containing a platform agnostic locking interface 
*      Author: msune
*
*/

typedef void platform_mutex_t;
typedef void platform_rwlock_t;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/* MUTEX operations */
//Init&destroy
platform_mutex_t* platform_mutex_init(void* params);
void platform_mutex_destroy(platform_mutex_t* mutex);

//Operations
void platform_mutex_lock(platform_mutex_t* mutex);
void platform_mutex_unlock(platform_mutex_t* mutex);

/* RWLOCK */
//Init&destroy
platform_rwlock_t* platform_rwlock_init(void* params);
void platform_rwlock_destroy(platform_rwlock_t* rwlock);

//Read
void platform_rwlock_rdlock(platform_rwlock_t* rwlock);
void platform_rwlock_rdunlock(platform_rwlock_t* rwlock);

//Write
void platform_rwlock_wrlock(platform_rwlock_t* rwlock);
void platform_rwlock_wrunlock(platform_rwlock_t* rwlock);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //PLATFORM_LOCK
