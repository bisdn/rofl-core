/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * thread_helper.h
 *
 *  Created on: Jan 9, 2012
 *      Author: tobi
 */

#ifndef THREAD_HELPER_H_
#define THREAD_HELPER_H_

#include <pthread.h>
#include <inttypes.h>

#include "croflexception.h"

namespace rofl
{

class RwLock; // forward declaration

class PthreadRwLock {
	pthread_rwlock_t rwlock;
	friend class RwLock;
public:
	PthreadRwLock(pthread_rwlockattr_t *attr = NULL) {
		pthread_rwlock_init(&rwlock, attr);
	};
	~PthreadRwLock() {
		pthread_rwlock_destroy(&rwlock);
	}
};

class eLockBase : public RoflException {};
class eLockInval : public eLockBase {};
class eLockWouldBlock : public eLockBase {};

class Lock
{
public:
	Lock(pthread_mutex_t *mutex, bool blocking = true) throw (eLockWouldBlock) :
		mutex(mutex), locked(false)
	{
		if (blocking) {
			pthread_mutex_lock(this->mutex);
		} else {
			if (pthread_mutex_trylock(this->mutex) < 0) {
				switch (errno) {
				case EBUSY:
					throw eLockWouldBlock();
				default:
					throw eInternalError();
				}
			}
		}
		locked = true;
	}

	virtual
	~Lock()
	{
		if (locked) {
			pthread_mutex_unlock(this->mutex);
		}
	}

private:
	pthread_mutex_t *mutex;
	bool locked;
};


class RwLock
{
		pthread_rwlock_t *rwlock;
		bool locked;
public:

		enum rwlock_rwtype_t {
			RWLOCK_READ = 1,
			RWLOCK_WRITE = 2,
		};

		/** constructor locks rwlock (or checks for existing lock)
		 *
		 */
		RwLock(pthread_rwlock_t *rwlock,
				uint8_t rwtype = RWLOCK_WRITE, // safe option: lock for writing
				bool blocking = true) throw (eLockWouldBlock, eLockInval) :
			rwlock(rwlock), locked(false)
		{
			if (blocking) {
				switch (rwtype) {
				case RWLOCK_READ:
					pthread_rwlock_rdlock(rwlock);
					break;
				case RWLOCK_WRITE:
					pthread_rwlock_wrlock(rwlock);
					break;
				default:
					throw eLockInval();
				}
			} else {
				int rc = 0;

				switch (rwtype) {
				case RWLOCK_READ:
					rc = pthread_rwlock_tryrdlock(rwlock);
					break;
				case RWLOCK_WRITE:
					rc = pthread_rwlock_trywrlock(rwlock);
					break;
				default:
					throw eLockInval();
				}

				if (rc < 0) {
					switch (errno) {
					case EBUSY:
						throw eLockWouldBlock();

					default:
						throw eInternalError();
					}
				}
			}
			locked = true;
		};

		/** constructor locks rwlock (or checks for existing lock)
		 *
		 */
		RwLock(PthreadRwLock& pthreadRwLock,
				uint8_t rwtype = RWLOCK_WRITE, // safe option: lock for writing
				bool blocking = true) throw (eLockWouldBlock, eLockInval) :
			rwlock(&(pthreadRwLock.rwlock)), locked(false)
		{
			if (blocking) {
				switch (rwtype) {
				case RWLOCK_READ:
					pthread_rwlock_rdlock(rwlock);
					break;
				case RWLOCK_WRITE:
					pthread_rwlock_wrlock(rwlock);
					break;
				default:
					throw eLockInval();
				}
			} else {
				int rc = 0;

				switch (rwtype) {
				case RWLOCK_READ:
					rc = pthread_rwlock_tryrdlock(rwlock);
					break;
				case RWLOCK_WRITE:
					rc = pthread_rwlock_trywrlock(rwlock);
					break;
				default:
					throw eLockInval();
				}

				if (rc < 0) {
					switch (errno) {
					case EBUSY:
						throw eLockWouldBlock();

					default:
						throw eInternalError();
					}
				}
			}
			locked = true;
		};



		/** destructor unlocks rwlock (if it has been locked)
		 *
		 */
		virtual
		~RwLock()
		{
			if (locked) {
				pthread_rwlock_unlock(rwlock);
			}
		}
};

}; // end of namespace

#endif /* THREAD_HELPER_H_ */
