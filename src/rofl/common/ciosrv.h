/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CIOSRV_H
#define CIOSRV_H

#include <set>
#include <list>
#include <bitset>
#include <utility>
#include <map>
#include <algorithm>
#include <vector>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>


#include "rofl/common/cmemory.h"
#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/cpipe.h"
#include "rofl/common/cevents.h"
#include "rofl/common/ctimers.h"
#include "rofl/common/ctimer.h"

namespace rofl
{


/* error classes */
class eIoSvcBase			: public RoflException {}; 	//< base error class for ciosrv
class eIoSvcInitFailed 		: public eIoSvcBase {};	//< init of ciosrv instance failed
class eIoSvcRunError 		: public eIoSvcBase {}; //< error in core loop (select)
class eIoSvcUnhandledTimer 	: public eIoSvcBase {}; //< unhandled timer
class eIoSvcNotFound        : public eIoSvcBase {}; //< element not found

class cioloop;

/**
 * (Abstract) Base class for IO services.
 * This class is a base class that adds IO event support to a derived class.
 * ciosrv provides a static method for running an infinite loop of select()/poll()
 * to handle file/socket descriptors. Each instance adds methods for adding/removing
 * file descriptors to/from the set of monitored descriptors.
 *
 * A derived class may overwrite event handlers for receiving the following event types:
 * - handle_revent() read events on file descriptors
 * - handle_wevent() write events on file descriptors
 * - handle_xevent() exceptions on file descriptors
 * - handle_timeout() timeout events
 * - handle_event() events emitted from external threads via cevent
 *
 * Methods for file descriptor management:
 * - register_filedesc_r() register a descriptor for read IO
 * - deregister_filedesc_r() deregister a read descriptor
 * - register_filedesc_w() register a descriptor for write IO
 * - deregister_filedesc_w() deregister a write descriptor
 *
 * Methods for timer management:
 * - register_timer() register a timer
 * - reset_timer() reset a timer
 * - cancel_timer() cancel a timer
 * - cancel_all_timer() cancel all timers
 *
 */
class ciosrv :
		public ptrciosrv
{
	static std::set<ciosrv*> 		ciolist;

	pthread_t						tid;
	std::set<int>					rfds;
	std::set<int>					wfds;
	ctimers							timers;
	cevents							events;

public:

	/**
	 * @brief	Initializes all structures for this ciosrv object.
	 */
	ciosrv();

	/**
	 * @brief	Deallocates resources for this ciosrv object.
	 */
	virtual
	~ciosrv();

	/**
	 * @brief	Initializes all structures for this ciosrv object.
	 */
	ciosrv(ciosrv const& iosrv);

	/**
	 *
	 */
	ciosrv&
	operator= (ciosrv const& iosrv);

public:

	/**
	 * @brief	Sends a notification to this ciosrv instance.
	 *
	 * Method notify() can be called from every thread within the
	 * running application. The event management system wakes up
	 * this thread when in sleeping condition in a poll/select loop and calls its
	 * handle_event() method.
	 *
	 * @param ev the event to be sent to this instance
	 */
	void
	notify(
			cevent const& event);


	/**
	 * @brief	Returns thread-id of local thread.
	 *
	 * @return thread ID
	 */
	pthread_t
	get_thread_id() const { return tid; };

	/**
	 *
	 */
	static void
	run();

	/**
	 *
	 */
	static void
	stop();

protected:

	/**
	 *
	 * @return
	 */
	void
	events_clear() { events.clear(); };

	friend class cioloop;

	/**
	 * @brief	Called by cioloop
	 */
	void
	__handle_event();

	/**
	 * @brief	Called by cioloop
	 */
	void
	__handle_revent(int fd);

	/**
	 * @brief	Called by cioloop
	 */
	void
	__handle_timeout();

	/**
	 * @name Event handlers
	 *
	 * These methods define handlers for file descriptor and cevent notifications.
	 * Derived classes should overwwrite all relevant handlers.
	 */

	/**@{*/

	/**
	 * @brief	Handler for event notifications using cevent instances.
	 *
	 * To be overwritten by derived class. Default behaviour: event is ignored.
	 *
	 * @see notify(cevent const& ev)
	 *
	 * @param ev cevent instance received
	 */
	virtual void
	handle_event(cevent const& ev) {};

	/**
	 * @brief	Handler for read events on file descriptors.
	 *
	 * To be overwritten by derived class. Default behaviour: event is ignored.
	 *
	 * @param fd read event occured on file descriptor fd
	 */
	virtual void
	handle_revent(int fd) {};

	/**
	 * @brief	Handler for write events on file descriptors.
	 *
	 * To be overwritten by derived class. Default behaviour: event is ignored.
	 *
	 * @param fd write event occured on file descriptor fd
	 */
	virtual void
	handle_wevent(int fd) {};

	/**
	 * @brief	Handler for exceptions on file descriptors.
	 *
	 * To be overwritten by derived class. Default behaviour: event is ignored.
	 *
	 * @param fd exception occured on file descriptor fd
	 */
	virtual void
	handle_xevent(int fd) {};

	/**
	 * @brief	Handler for timer events.
	 *
	 * To be overwritten by derived class. Default behaviour: event is ignored.
	 *
	 * @param opaque expired timer type
	 */
	virtual void
	handle_timeout(int opaque, void *data = (void*)0) {};

	/**@}*/

protected:

	/**
	 * @name Management methods for file descriptors
	 */

	/**@{*/

	/**
	 * @brief	Registers a file descriptor for read events.
	 *
	 * A read event will be indicated via calling handle_revent(fd).
	 *
	 * @param fd the file descriptor waiting for read events
	 */
	void
	register_filedesc_r(int fd);

	/**
	 * @brief	Deregisters a file descriptor from read events.
	 *
	 * @param fd the file descriptor removed from the set of read events
	 */
	void
	deregister_filedesc_r(int fd);

	/**
	 * @brief	Registers a file descriptor for write events.
	 *
	 * A write event will be indicated via calling handle_wevent(fd).
	 *
	 * @param fd the file descriptor waiting for write events
	 */
	void
	register_filedesc_w(int fd);

	/**
	 * @brief	Deregisters a file descriptor from write events.
	 *
	 * @param fd the file descriptor removed from the set of write events
	 */
	void
	deregister_filedesc_w(int fd);

	/**@}*/

protected:

	/**
	 *
	 */
	ctimer
	get_next_timer();

	/**
	 * @name Management methods for timers
	 */

	/**@{*/

	/**
	 * @brief	Installs a new timer to fire in t seconds.
	 *
	 * @param opaque this timer type can be arbitrarily chosen
	 * @param t timeout in seconds of this timer
	 * @return timer handle
	 */
	ctimerid const&
	register_timer(int opaque, time_t t);

	/**
	 * @brief	Resets a running timer of type opaque.
	 *
	 * If no timer of type opaque exists, a new timer will be started.
	 *
	 * @param opaque this timer type can be arbitrarily chosen
	 * @param t timeout in seconds of this timer
	 * @return timer handle
	 */
	ctimerid const&
	reset_timer(ctimerid const& timer_id, time_t t);

	/**
	 * @brief	Checks for a pending timer of type opaque.
	 *
	 * @param opaque timer type the caller is seeking for
	 * @return true: timer of type opaque exists, false: no pending timer
	 */
	bool
	pending_timer(ctimerid const& timer_id);

	/**
	 * @brief	Cancels a pending timer.
	 *
	 * @param opaque timer type the caller is seeking for
	 */
	void
	cancel_timer(ctimerid const& timer_id);

	/**
	 * @brief	Cancels all pending timer of this instance.
	 *
	 */
	void
	cancel_all_timer();

	/**@}*/

public:

	friend std::ostream&
	operator<< (std::ostream& os, ciosrv const& iosvc) {
		os << indent(0) << "<ciosrv >" << std::endl;
			os << indent(2) << "<rfds: ";
			for (std::set<int>::const_iterator it = iosvc.rfds.begin(); it != iosvc.rfds.end(); ++it) {
				os << (*it) << " ";
			}
			os << ">" << std::endl;
			os << indent(2) << "<wfds: ";
			for (std::set<int>::const_iterator it = iosvc.wfds.begin(); it != iosvc.wfds.end(); ++it) {
				os << (*it) << " ";
			}
			os << ">" << std::endl;

			{ indent i(2); os << iosvc.timers; }
			{ indent i(2); os << iosvc.events; }
		return os;
	};
};






class cioloop {

	static PthreadRwLock 					threads_rwlock;
	static std::map<pthread_t, cioloop*> 	threads;

	std::vector<ciosrv*>					rfds;
	PthreadRwLock							rfds_rwlock;
	std::vector<ciosrv*>					wfds;
	PthreadRwLock							wfds_rwlock;
	std::map<ciosrv*, bool>					timers;
	PthreadRwLock							timers_rwlock;
	std::map<ciosrv*, bool>					events;
	PthreadRwLock							events_rwlock;

	cpipe									pipe;
	pthread_t        			       		tid;
	struct timespec 						ts;
	bool									keep_on_running;

	unsigned int							minrfd; // lowest set readfd
	unsigned int							maxrfd; // highest set readfd
	unsigned int							minwfd; // lowest set writefd
	unsigned int							maxwfd; // highest set writefd

public:

	friend class ciosrv;


public:

	/**
	 *
	 */
	static cioloop&
	get_loop() {
		pthread_t tid = pthread_self();
		if (cioloop::threads.find(tid) == cioloop::threads.end()) {
			cioloop::threads[tid] = new cioloop();
		}
		return *(cioloop::threads[tid]);
	};

	/**
	 *
	 */
	static void
	run() {
		cioloop::get_loop().run_loop();
	};

	/**
	 *
	 */
	static void
	stop() {
		pthread_t tid = pthread_self();
		if (cioloop::threads.find(tid) == cioloop::threads.end()) {
			return;
		}
		cioloop::threads[tid]->keep_on_running = false;
	};

	/**
	 *
	 */
	static void
	shutdown() {
		for (std::map<pthread_t, cioloop*>::iterator
				it = cioloop::threads.begin(); it != cioloop::threads.end(); ++it) {
			delete it->second;
		}
		cioloop::threads.clear();
	};

public:



	/**
	 *
	 */
	void
	add_readfd(ciosrv* iosrv, int fd) {
		RwLock lock(rfds_rwlock, RwLock::RWLOCK_WRITE);
		rfds[fd] = iosrv;

		minrfd = (minrfd > (unsigned int)(fd+0)) ? (unsigned int)(fd+0) : minrfd;
		maxrfd = (maxrfd < (unsigned int)(fd+1)) ? (unsigned int)(fd+1) : maxrfd;
	};

	/**
	 *
	 */
	void
	drop_readfd(ciosrv* iosrv, int fd) {
		RwLock lock(rfds_rwlock, RwLock::RWLOCK_WRITE);
		rfds[fd] = NULL;

		if (minrfd == (unsigned int)(fd+0)) {
			minrfd = rfds.size();
			for (unsigned int i = fd + 1; i < rfds.size(); i++) {
				if (rfds[i] != NULL) {
					minrfd = i;
					break;
				}
			}
		}

		if (maxrfd == (unsigned int)(fd+1)) {
			maxrfd = 0;
			for (unsigned int i = fd; i > 0; i--) {
				if (rfds[i] != NULL) {
					maxrfd = (i+1);
					break;
				}
			}
		}
	};

	/**
	 *
	 */
	void
	add_writefd(ciosrv* iosrv, int fd) {
		RwLock lock(wfds_rwlock, RwLock::RWLOCK_WRITE);
		wfds[fd] = iosrv;

		minwfd = (minwfd > (unsigned int)(fd+0)) ? (unsigned int)(fd+0) : minwfd;
		maxwfd = (maxwfd < (unsigned int)(fd+1)) ? (unsigned int)(fd+1) : maxwfd;
	};

	/**
	 *
	 */
	void
	drop_writefd(ciosrv* iosrv, int fd) {
		RwLock lock(wfds_rwlock, RwLock::RWLOCK_WRITE);
		wfds[fd] = NULL;

		if (minwfd == (unsigned int)(fd+0)) {
			minwfd = wfds.size();
			for (unsigned int i = fd + 1; i < wfds.size(); i++) {
				if (wfds[i] != NULL) {
					minwfd = i;
					break;
				}
			}
		}

		if (maxwfd == (unsigned int)(fd+1)) {
			maxwfd = 0;
			for (unsigned int i = fd; i > 0; i--) {
				if (wfds[i] != NULL) {
					maxwfd = (i+1);
					break;
				}
			}
		}
	};

	/**
	 *
	 */
	void
	has_timer(ciosrv* iosrv) {
		RwLock lock(timers_rwlock, RwLock::RWLOCK_WRITE);
		timers[iosrv] = true;
	};

	/**
	 *
	 */
	void
	has_no_timer(ciosrv *iosrv) {
		RwLock lock(timers_rwlock, RwLock::RWLOCK_READ);
		timers[iosrv] = false;
	};

	/**
	 *
	 */
	void
	has_event(ciosrv* iosrv) {
		RwLock lock(events_rwlock, RwLock::RWLOCK_WRITE);
		events[iosrv] = true;
		pipe.writemsg('1'); // wakeup main loop, just in case
	};

	/**
	 *
	 */
	void
	has_no_event(ciosrv* iosrv) {
		RwLock lock(events_rwlock, RwLock::RWLOCK_READ);
		events[iosrv] = false;
	};

private:

	/**
	 *
	 */
	cioloop() :
		tid(pthread_self()),
		keep_on_running(false) {

		struct rlimit rlim;
		if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
			throw eSysCall("getrlimit()");
		}
		for (unsigned int i = 0; i < rlim.rlim_cur; i++) {
			rfds.push_back(NULL);
			wfds.push_back(NULL);
		}
		minrfd = rfds.size();
		maxrfd = 0;
		minwfd = wfds.size();
		maxwfd = 0;
	};

	/**
	 *
	 */
	virtual
	~cioloop() {};

	/**
	 *
	 */
	cioloop(cioloop const& t) {
		*this = t;
	};

	/**
	 *
	 */
	cioloop&
	operator= (cioloop const& t) {
		if (this == &t)
			return *this;
		return *this;
	};

	/**
	 * returns immediately, when called twice => checks keep_on_running flag
	 */
	void
	run_loop();


	/**
	 * A signal handler.
	 * for SIGUSR1
	 */
	static void
	child_sig_handler (int x);



public:

	friend std::ostream&
	operator<< (std::ostream& os, cioloop const& ioloop) {
		os << indent(0) << "<cioloop >" << std::endl;

		os << indent(2) << "<read-fds: ";
		for (unsigned int i = 0; i < ioloop.rfds.size(); ++i) {
			if (NULL != ioloop.rfds.at(i)) {
				os << i << " ";
			}
		}
		os << ">" << std::endl;

		os << indent(2) << "<write-fds: ";
		for (unsigned int i = 0; i < ioloop.wfds.size(); ++i) {
			if (NULL != ioloop.wfds.at(i)) {
				os << i << " ";
			}
		}
		os << ">" << std::endl;

		os << indent(2) << "<instances with timer needs: ";
		// locking?
		for (std::map<ciosrv*, bool>::const_iterator
				it = ioloop.timers.begin(); it != ioloop.timers.end(); ++it) {
			os << it->first << ":" << it->second << " ";
		}
		os << ">" << std::endl;

		os << indent(2) << "<instances with event needs: ";
		// locking?
		for (std::map<ciosrv*, bool>::const_iterator
				it = ioloop.events.begin(); it != ioloop.events.end(); ++it) {
			os << it->first << ":" << it->second << " ";
		}
		os << ">" << std::endl;

		return os;
	};
};



}; // end of namespace

#endif
