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

#include "cerror.h"
#include "rofl/platform/unix/csyslog.h"

namespace rofl
{

class eLockBase : public cerror {};
class eLockInval : public eLockBase {};
class eLockWouldBlock : public eLockBase {};

class Lock :
	public csyslog
{
public:
	Lock(pthread_mutex_t *mutex, bool blocking = true) throw (eLockWouldBlock) :
		mutex(mutex), locked(false)
	{
		WRITELOG(CTHREAD, DBG, "thread %x lock mutex %p -trying-", pthread_self(), mutex);
		if (blocking)
		{
			pthread_mutex_lock(this->mutex);
		}
		else
		{
			if (pthread_mutex_trylock(this->mutex) < 0)
			{
				switch (errno) {
				case EBUSY:
					throw eLockWouldBlock();

				default:
					throw eInternalError();
				}
			}
		}
		locked = true;
		WRITELOG(CTHREAD, DBG, "thread %x lock mutex %p -locked-", pthread_self(), mutex);
	}

	virtual
	~Lock()
	{
		WRITELOG(CTHREAD, DBG, "thread %x unlock mutex %p -trying-", pthread_self(), mutex);
		if (locked)
		{
			pthread_mutex_unlock(this->mutex);
		}
		WRITELOG(CTHREAD, DBG, "thread %x unlock mutex %p -unlocked-", pthread_self(), mutex);
	}

private:
	pthread_mutex_t *mutex;
	bool locked;
};


class RwLock :
	public csyslog
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
			WRITELOG(CTHREAD, DBG, "RwLock(%p) thread %x rwlock %p -trying- %s lock",
					this, pthread_self(), rwlock, (rwtype == RWLOCK_READ) ? "READ" : "WRITE");
			if (blocking)
			{
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
			}
			else
			{
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

				if (rc < 0)
				{
					switch (errno) {
					case EBUSY:
						throw eLockWouldBlock();

					default:
						throw eInternalError();
					}
				}
			}
			locked = true;
			WRITELOG(CTHREAD, DBG, "RwLock(%p) thread %x rwlock %p -locked- %s lock",
					this, pthread_self(), rwlock, (rwtype == RWLOCK_READ) ? "READ" : "WRITE");
		};

		/** destructor unlocks rwlock (if it has been locked)
		 *
		 */
		virtual
		~RwLock()
		{
			WRITELOG(CTHREAD, DBG, "RwLock(%p) thread %x unlock rwlock %p -trying-",
					this, pthread_self(), rwlock);
			if (locked)
			{
				pthread_rwlock_unlock(rwlock);
			}
			WRITELOG(CTHREAD, DBG, "RwLock(%p) thread %x unlock rwlock %p -unlocked-",
					this, pthread_self(), rwlock);
		}
};

}; // end of namespace

#endif /* THREAD_HELPER_H_ */
