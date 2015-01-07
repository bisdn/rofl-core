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

namespace rofl {

class ciosrv;

/**
 * @brief	Defines an IO service loop for a single thread.
 * @ingroup common_devel_ioservice
 *
 *
 */
class cioloop {
public:

	/**
	 * @brief	Returns reference to cioloop instance identified by thread id
	 * 		    or if none is specified, cioloop of local thread.
	 */
	static cioloop&
	get_loop(pthread_t tid = 0) {
		if (0 == tid) {
			tid = pthread_self();
		}
		if (cioloop::threads.find(tid) == cioloop::threads.end()) {
			cioloop::threads[tid] = new cioloop(tid);
		}
		return *(cioloop::threads[tid]);
	};

	/**
	 * @brief	Drop
	 */
	static void
	drop_loop(pthread_t tid) {
		if (cioloop::threads.find(tid) == cioloop::threads.end()) {
			return;
		}
		delete cioloop::threads[tid];
		cioloop::threads.erase(tid);
	};

public:

	/**
	 *
	 */
	void
	run() {
		cioloop::get_loop().run_loop();
	};

	/**
	 * @brief	Terminates cioloop instance running in thread identified by this->tid.
	 */
	void
	stop() {
		if (cioloop::threads.find(tid) == cioloop::threads.end()) {
			return;
		}
		cioloop::threads[tid]->keep_on_running = false;
		{
			RwLock lock(rfds_rwlock, RwLock::RWLOCK_WRITE);
			rfds.clear();
		}
		{
			RwLock lock(wfds_rwlock, RwLock::RWLOCK_WRITE);
			wfds.clear();
		}
		{
			RwLock lock(events_rwlock, RwLock::RWLOCK_WRITE);
			events.clear();
		}
		{
			RwLock lock(timers_rwlock, RwLock::RWLOCK_WRITE);
			timers.clear();
		}
		if (tid != pthread_self()) {
			//pipe.writemsg('1'); // wakeup main loop, just in case
		}
	};

	/**
	 * @brief	Terminates all running cioloop instances.
	 */
	void
	shutdown() {
		for (std::map<pthread_t, cioloop*>::iterator
				it = cioloop::threads.begin(); it != cioloop::threads.end(); ++it) {
			delete it->second;
		}
		cioloop::threads.clear();
	};

	/**
	 *
	 */
	pthread_t
	get_tid() const { return tid; };

protected:

	friend class ciosrv;

	/**
	 *
	 */
	void
	add_readfd(ciosrv* iosrv, int fd) {
		RwLock lock(rfds_rwlock, RwLock::RWLOCK_WRITE);
		rfds[fd] = iosrv;

		minrfd = (minrfd > (unsigned int)(fd+0)) ? (unsigned int)(fd+0) : minrfd;
		maxrfd = (maxrfd < (unsigned int)(fd+1)) ? (unsigned int)(fd+1) : maxrfd;

		if (tid != pthread_self()) {
			pipe.writemsg('1'); // wakeup main loop, just in case
		}
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

		if (tid != pthread_self()) {
			pipe.writemsg('1'); // wakeup main loop, just in case
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

		if (tid != pthread_self()) {
			pipe.writemsg('1'); // wakeup main loop, just in case
		}
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

		if (tid != pthread_self()) {
			pipe.writemsg('1'); // wakeup main loop, just in case
		}
	};

	/**
	 *
	 */
	void
	has_timer(ciosrv* iosrv) {
		RwLock lock(timers_rwlock, RwLock::RWLOCK_WRITE);
		timers[iosrv] = true;
		if (tid != pthread_self()) {
			pipe.writemsg('1'); // wakeup main loop, just in case
		}
	};

	/**
	 *
	 */
	void
	has_no_timer(ciosrv *iosrv) {
		RwLock lock(timers_rwlock, RwLock::RWLOCK_WRITE);
		timers[iosrv] = false;
	};

	/**
	 *
	 */
	void
	has_event(ciosrv* iosrv) {
		RwLock lock(events_rwlock, RwLock::RWLOCK_WRITE);
		events[iosrv] = true;
		if (tid != pthread_self()) {
			pipe.writemsg('1'); // wakeup main loop, just in case
		}
	};

	/**
	 *
	 */
	void
	has_no_event(ciosrv* iosrv) {
		RwLock lock(events_rwlock, RwLock::RWLOCK_WRITE);
		events[iosrv] = false;
	};

private:

	/**
	 *
	 */
	cioloop(pthread_t tid = 0) :
		tid(tid),
		keep_on_running(false) {

		if (0 == tid) {
			this->tid = pthread_self();
		}

		struct rlimit rlim;
		if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
			throw eSysCall("getrlimit()");
		}
		RwLock rlock(rfds_rwlock, RwLock::RWLOCK_WRITE);
		RwLock wlock(wfds_rwlock, RwLock::RWLOCK_WRITE);
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
		os << indent(0) << "<cioloop tid:0x"
				<< std::hex << ioloop.get_tid() << std::dec << ">" << std::endl;

		{
			RwLock lock(ioloop.rfds_rwlock, RwLock::RWLOCK_READ);
			os << indent(2) << "<read-fds: ";
			for (unsigned int i = 0; i < ioloop.rfds.size(); ++i) {
				if (NULL != ioloop.rfds.at(i)) {
					os << i << " ";
				}
			}
			os << ">" << std::endl;
		}

		{
			RwLock lock(ioloop.wfds_rwlock, RwLock::RWLOCK_READ);
			os << indent(2) << "<write-fds: ";
			for (unsigned int i = 0; i < ioloop.wfds.size(); ++i) {
				if (NULL != ioloop.wfds.at(i)) {
					os << i << " ";
				}
			}
			os << ">" << std::endl;
		}

		{
			RwLock lock(ioloop.timers_rwlock, RwLock::RWLOCK_READ);
			os << indent(2) << "<instances with timer needs: ";
			// locking?
			for (std::map<ciosrv*, bool>::const_iterator
					it = ioloop.timers.begin(); it != ioloop.timers.end(); ++it) {
				os << it->first << ":" << it->second << " ";
			}
			os << ">" << std::endl;
		}

		{
			RwLock lock(ioloop.events_rwlock, RwLock::RWLOCK_READ);
			os << indent(2) << "<instances with event needs: ";
			// locking?
			for (std::map<ciosrv*, bool>::const_iterator
					it = ioloop.events.begin(); it != ioloop.events.end(); ++it) {
				os << it->first << ":" << it->second << " ";
			}
			os << ">" << std::endl;
		}

		return os;
	};

private:

	static PthreadRwLock 					threads_rwlock;
	static std::map<pthread_t, cioloop*> 	threads;

	std::vector<ciosrv*>					rfds;
	mutable PthreadRwLock					rfds_rwlock;
	std::vector<ciosrv*>					wfds;
	mutable PthreadRwLock					wfds_rwlock;
	std::map<ciosrv*, bool>					timers;
	mutable PthreadRwLock					timers_rwlock;
	std::map<ciosrv*, bool>					events;
	mutable PthreadRwLock					events_rwlock;

	cpipe									pipe;
	pthread_t        			       		tid;
	bool									keep_on_running;

	unsigned int							minrfd; // lowest set readfd
	unsigned int							maxrfd; // highest set readfd
	unsigned int							minwfd; // lowest set writefd
	unsigned int							maxwfd; // highest set writefd
};




/**
 * @brief 	Base class for IO services.
 *
 * @ingroup common_devel_ioservice
 *
 * Derive from this class in order to get support for file/socket descriptor
 * and timer management. rofl::ciosrv binds a higher layer class with the
 * low layer IO loop defined inside class rofl::cioloop. This class provides
 * two groups of methods:
 *
 * 1. Management methods for typical CRUD operations on timers and descriptors.
 * 2. Event events for sending notifications towards deriving classes about
 * read or write events or expiration of timers.
 *
 * Methods for file descriptor management:
 *
 * 1.a) register_filedesc_r() register a descriptor for read IO
 *
 * 1.b) deregister_filedesc_r() deregister a read descriptor
 *
 * 1.c) register_filedesc_w() register a descriptor for write IO
 *
 * 1.d) deregister_filedesc_w() deregister a write descriptor
 *
 * Methods for timer management:
 *
 * 1.e) register_timer() register a timer
 *
 * 1.f) reset_timer() reset a timer
 *
 * 1.g) restart_timer() restart or create new timer
 *
 * 1.h) pending_timer() check for existence of timer
 *
 * 1.i) cancel_timer() cancel a timer
 *
 * 1.j) cancel_all_timer() cancel all timers
 *
 * Methods for event management:
 *
 * 1.k) notify() send an event to this rofl::ciosrv instance
 *
 * The following event handlers exist and may be overwritten by a class deriving
 * from rofl::ciosrv:
 *
 * 2.a) handle_revent() read events on file descriptor(s)
 *
 * 2.b) handle_wevent() write events on file descriptor(s)
 *
 * 2.c) handle_xevent() exceptions on file descriptor(s)
 *
 * 2.d) handle_timeout() timeout event(s)
 *
 * 2.e) handle_event() events sent via notify() method
 *
 * This class utilizes timer handles based on class rofl::ctimerid
 * for managing pending timers, e.g., cancel or restarting them.
 *
 * @see rofl::cevent
 * @see rofl::ctimerid
 */
class ciosrv : public ctimer_env {
public:

	/**
	 * @brief	Initializes all structures for this ciosrv object.
	 */
	ciosrv(pthread_t tid = 0);

	/**
	 * @brief	Deallocates resources for this ciosrv object.
	 */
	virtual
	~ciosrv();

	/**
	 * @brief	Initializes all structures for this ciosrv object.
	 */
	ciosrv(const ciosrv& iosrv);

	/**
	 *
	 */
	ciosrv&
	operator= (const ciosrv& iosrv);

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
	notify(const cevent& event) {
		events.add_event(event);
		cioloop::get_loop(get_thread_id()).has_event(this);
	};

	/**
	 * @brief	Returns thread-id of local thread.
	 *
	 * @return thread ID
	 */
	pthread_t
	get_thread_id() const
	{ return tid; };

protected:

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
	 * @param event rofl::cevent instance received
	 */
	virtual void
	handle_event(
			const rofl::cevent& event)
	{};

	/**
	 * @brief	Handler for read events on file descriptors.
	 *
	 * To be overwritten by derived class. Default behaviour: event is ignored.
	 *
	 * @param fd read event occured on file descriptor fd
	 */
	virtual void
	handle_revent(
			int fd)
	{};

	/**
	 * @brief	Handler for write events on file descriptors.
	 *
	 * To be overwritten by derived class. Default behaviour: event is ignored.
	 *
	 * @param fd write event occured on file descriptor fd
	 */
	virtual void
	handle_wevent(int fd)
	{};

	/**
	 * @brief	Handler for exceptions on file descriptors.
	 *
	 * To be overwritten by derived class. Default behaviour: event is ignored.
	 *
	 * @param fd exception occured on file descriptor fd
	 */
	virtual void
	handle_xevent(int fd)
	{};

	/**
	 * @brief	Handler for timer events.
	 *
	 * To be overwritten by derived class. Default behaviour: event is ignored.
	 *
	 * @param opaque expired timer type
	 * @param data pointer to opaque data
	 */
	virtual void
	handle_timeout(int opaque, void *data = (void*)0)
	{};

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
	register_filedesc_r(int fd) {
		rfds.insert(fd);
		cioloop::get_loop(get_thread_id()).add_readfd(this, fd);
	};

	/**
	 * @brief	Deregisters a file descriptor from read events.
	 *
	 * @param fd the file descriptor removed from the set of read events
	 */
	void
	deregister_filedesc_r(int fd) {
		rfds.erase(fd);
		cioloop::get_loop(get_thread_id()).drop_readfd(this, fd);
	};

	/**
	 * @brief	Registers a file descriptor for write events.
	 *
	 * A write event will be indicated via calling handle_wevent(fd).
	 *
	 * @param fd the file descriptor waiting for write events
	 */
	void
	register_filedesc_w(int fd) {
		wfds.insert(fd);
		cioloop::get_loop(get_thread_id()).add_writefd(this, fd);
	};

	/**
	 * @brief	Deregisters a file descriptor from write events.
	 *
	 * @param fd the file descriptor removed from the set of write events
	 */
	void
	deregister_filedesc_w(int fd) {
		wfds.erase(fd);
		cioloop::get_loop(get_thread_id()).drop_writefd(this, fd);
	};

	/**@}*/

protected:

	/**
	 *
	 */
	ctimer
	get_next_timer() {
		return timers.get_next_timer();
	};

	/**
	 * @name Management methods for timers and events
	 */

	/**@{*/

	/**
	 * @brief	Installs a new timer to fire in t seconds.
	 *
	 * @param opaque this timer type can be arbitrarily chosen
	 * @param ctimer object
	 * @return timer handle
	 */
	const rofl::ctimerid&
	register_timer(int opaque, const rofl::ctimespec& timespec) {
		if (timers.empty() || (get_thread_id() != pthread_self()))
			cioloop::get_loop().has_timer(this);
		return timers.add_timer(ctimer(this, opaque, timespec));
	};

	/**
	 * @brief	Resets a running timer of type opaque.
	 *
	 * If no timer of type opaque exists, a new timer will be started.
	 *
	 * @param opaque this timer type can be arbitrarily chosen
	 * @param t timeout in seconds of this timer
	 * @return timer handle
	 */
	const rofl::ctimerid&
	reset_timer(const rofl::ctimerid& timer_id, const rofl::ctimespec& timespec) {
		if (timers.empty() || (get_thread_id() != pthread_self()))
			cioloop::get_loop().has_timer(this);
		return timers.reset(timer_id, timespec);
	};

	/**
	 * @brief	Resets an existing or creates a new timer.
	 *
	 * If timer specified by timer_id exists, the timer is reset to the value defined by timespec.
	 * If no timer of the specified timer_id exists, a new timer is created based on values
	 * opaque and timespec.
	 *
	 * @param timer_id An existing timer_id. This value is going to be overwritten.
	 * @param opaque this timer type value can be chosen by the caller
	 * @param timespec timeout defined for this timer
	 * @return reference to parameter timer_id
	 */
	ctimerid&
	restart_timer(rofl::ctimerid& timer_id, int opaque, const rofl::ctimespec& timespec) {
		if (pending_timer(timer_id)) {
			timer_id = reset_timer(timer_id, timespec);
		} else {
			timer_id = register_timer(opaque, timespec);
		}
		return timer_id;
	};

	/**
	 * @brief	Checks for a pending timer of type opaque.
	 *
	 * @param opaque timer type the caller is seeking for
	 * @return true: timer of type opaque exists, false: no pending timer
	 */
	bool
	pending_timer(const rofl::ctimerid& timer_id) {
		return timers.pending(timer_id);
	};

	/**
	 * @brief	Cancels a pending timer.
	 *
	 * @param opaque timer type the caller is seeking for
	 */
	void
	cancel_timer(const rofl::ctimerid& timer_id) {
		timers.cancel(timer_id);
		if (timers.empty())
			cioloop::get_loop().has_no_timer(this);
	};

	/**
	 * @brief	Cancels all pending timers of this instance.
	 *
	 */
	void
	cancel_all_timers() {
		timers.clear();
		cioloop::get_loop().has_no_timer(this);
	};

	/**
	 * @brief	Cancels all pending events of this instance.
	 *
	 */
	void
	cancel_all_events() {
		events.clear();
		cioloop::get_loop().has_no_event(this);
	};

	/**@}*/

private:

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
	__handle_revent(int fd) {
		try {
			handle_revent(fd);
		} catch (eEventsNotFound& e) {/*do nothing*/}
	};

	/**
	 * @brief	Called by cioloop
	 */
	void
	__handle_timeout() {
		try {
			ctimer timer = timers.get_expired_timer();
			logging::trace << "[rofl-common][ciosrv][handle-timeout] timer: " << std::endl << timer;
			handle_timeout(timer.get_opaque());
		} catch (eTimersNotFound& e) {/*do nothing*/}
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const ciosrv& iosvc) {
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

private:

	static PthreadRwLock 			ciolist_rwlock;
	static std::set<ciosrv*> 		ciolist;

	pthread_t						tid;
	std::set<int>					rfds;
	std::set<int>					wfds;
	ctimers							timers;
	cevents							events;
};





}; // end of namespace

#endif
