/*
 * lock.h
 *
 *  Created on: Dec 3, 2012
 *      Author: tobi
 */

#ifndef LOCK_H_
#define LOCK_H_

/**
 * PTHREAD compatible platform
 */
#include <pthread.h>

/* MUTEX operations */
typedef pthread_mutex_t platform_mutex_t;

/* RWLOCK */
typedef pthread_rwlock_t platform_rwlock_t;

#endif /* LOCK_H_ */
