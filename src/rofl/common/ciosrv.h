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
#include "rofl/common/cfdset.h"
#include "rofl/common/cevents.h"
#include "rofl/common/ctimers.h"

namespace rofl
{


/* error classes */
class eIoSvcBase			: public RoflException {}; 	//< base error class for ciosrv
class eIoSvcInitFailed 		: public eIoSvcBase {};	//< init of ciosrv instance failed
class eIoSvcRunError 		: public eIoSvcBase {}; //< error in core loop (select)
class eIoSvcUnhandledTimer 	: public eIoSvcBase {}; //< unhandled timer
class eIoSvcNotFound        : public eIoSvcBase {}; //< element not found

class ciosrv;



class cioelem {

	ciosrv		*iosrv;	// corresponding ciosrv instance

public:

	enum cioelem_state_t {
		STATE_ADD		= 0,
		STATE_RUNNING	= 1,
		STATE_DROP		= 2,
	};

	cioelem_state_t		state;

public:

	cioelem(ciosrv *iosrv, cioelem_state_t state) :
		iosrv(iosrv),
		state(state) {};

	virtual
	~cioelem() {};

	cioelem(cioelem const& elem) {
		*this = elem;
	};

	cioelem&
	operator= (cioelem const& elem) {
		if (this == &elem)
			return *this;
		iosrv 	= elem.iosrv;
		state 	= elem.state;
		return *this;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cioelem const& elem) {
		os << "<cioelem iosrv:" << elem.iosrv << " ";
		switch (elem.state) {
		case STATE_ADD: 	os << "state:-ADD-"; 		break;
		case STATE_RUNNING: os << "state:-RUNNING-"; 	break;
		case STATE_DROP: 	os << "state:-DROP-"; 		break;
		}
		os << " >" << std::endl;
		return os;
	};
};



class cioloop {

	static PthreadRwLock 					threads_rwlock;
	static std::map<pthread_t, cioloop*> 	threads;

public:

	friend class ciosrv;

	pthread_t               tid;
	PthreadRwLock			rwlock;
	std::set<cioelem>		elems_add;
	std::set<cioelem>		elems_drop;
	std::set<cioelem> 		elems;

	cpipe					pipe;
	cfdset					rfdset;
	cfdset					wfdset;
	ctimers					timers;
	cevents					events;

	struct timespec 		ts;
	bool					keep_on_running;

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
		if (cioloop::threads.find(tid) == cioloop::threads.end()) {
			return;
		}
		cioloop::threads[tid]->keep_on_running = false;
	};

public:

	/**
	 *
	 */
	void
	add_elem(ciosrv* iosrv) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		elems_add.insert(cioelem(iosrv, cioelem::STATE_ADD));
	};

	/**
	 *
	 */
	void
	drop_elem(ciosrv* iosrv) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		elems_drop.erase(cioelem(iosrv, cioelem::STATE_DROP));
	};


	/**
	 *
	 */
	static void
	daemonize(
			std::string const& pidfile, std::string const& logfile);


private:

	/**
	 *
	 */
	cioloop() :
		tid(pthread_self()),
		keep_on_running(false) {};

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

public:

	friend std::ostream&
	operator<< (std::ostream& os, cioloop const& thread) {
		os << indent(0) << "<cthread >";
		indent i(2);
		os << "<read-fds: >" << std::endl;
		{ indent i(2); os << thread.rfdset; }
		os << "<write-fds: >" << std::endl;
		{ indent i(2); os << thread.wfdset; }
		os << "<timers: >" << std::endl;
		{ indent i(2); os << thread.timers; }
		os << "<events: >" << std::endl;
		{ indent i(2); os << thread.events; }
		return os;
	};
};



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
		public cfdowner
{
#if 0
	class ciothread {
	public:
		pthread_t               tid;		// thread id
		cpipe                   *pipe;	        // wakeup pipe
		std::set<ciosrv*>		ciosrv_elements;// all ciosrv objects within this thread
		std::map<int, ciosrv*>  rfds; 	        // read fds
		pthread_rwlock_t		rfds_rwlock;
		std::map<int, ciosrv*>  wfds;	        // write fds
		pthread_rwlock_t		wfds_rwlock;
		std::list<ciosrv*>      ciosrv_timeouts;// set with all ciosrv instances with timeout in next round
		std::bitset<32>         flags;          //< flags

		pthread_rwlock_t wakeup_rwlock;  // rwlock for cevent lists
        pthread_mutex_t  ciosrv_list_mutex;     // mutex for cevent lists
        std::set<class ciosrv*> ciosrv_insertion_list; //< list of all ciosrv instances new inserted
        std::set<class ciosrv*> ciosrv_list;           //< list of all ciosrv instances
        std::set<class ciosrv*> ciosrv_deletion_list;  //< list of all ciosrv instances scheduled for deletion
        std::set<class ciosrv*> ciosrv_wakeup;         //< list of all cioctl commands rcvd
        int evlockinit; // = 0 => destroy mutex
        bool					keep_on;

	public:
		/** constructor
		 *
		 */
		ciothread() :
			tid(pthread_self()),
			evlockinit(0),
			keep_on(true)
		{
			pipe = new cpipe();
			pthread_rwlock_init(&(wakeup_rwlock), NULL);
			pthread_mutex_init(&(ciosrv_list_mutex), NULL);
			pthread_rwlock_init(&(rfds_rwlock), NULL);
			pthread_rwlock_init(&(wfds_rwlock), NULL);
		};
		~ciothread()
		{
#if 0
                        for (std::set<ciosrv*>::iterator it = ciosrv_list.begin();
                            it != ciosrv_list.end(); ++it)
                        {
                            delete (*it);
                        }
#endif
            pthread_rwlock_destroy(&wfds_rwlock);
            pthread_rwlock_destroy(&rfds_rwlock);
            pthread_mutex_destroy(&(ciosrv_list_mutex));
	        pthread_rwlock_destroy(&(wakeup_rwlock));
			delete pipe;
		};
	};
#endif

private: // static



	enum ciosrv_flag_t {
		CIOSRV_FLAG_WAKEUP_CALLED = (1 << 0), // when set, pipe was already instructed to
						// wake up called thread
	};



private:

	enum ciosrv_thread_state_t {
		STATE_STOPPED 		= 0,
		STATE_INITIALIZED	= 1,
		STATE_STARTED		= 2,
	};

	/**
 	 *
	 */
	static int
	thread_init();

	/**
	 *
	 */
	static void
	thread_shutdown();




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
	handle_timeout(int opaque) {};


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
	 * @name Management methods for timers
	 */

	/**@{*/


	/**
	 * @brief	Installs a new timer to fire in t seconds.
	 *
	 * @param opaque this timer type can be arbitrarily chosen
	 * @param t timeout in seconds of this timer
	 */
	void
	register_timer(int opaque, time_t t);



	/**
	 * @brief	Resets a running timer of type opaque.
	 *
	 * If no timer of type opaque exists, a new timer will be started.
	 *
	 * @param opaque this timer type can be arbitrarily chosen
	 * @param t timeout in seconds of this timer
	 */
	void
	reset_timer(int opaque, time_t t);



	/**
	 * @brief	Checks for a pending timer of type opaque.
	 *
	 * @param opaque timer type the caller is seeking for
	 * @return true: timer of type opaque exists, false: no pending timer
	 */
	bool
	pending_timer(int opaque);



	/**
	 * @brief	Cancels a pending timer.
	 *
	 * @param opaque timer type the caller is seeking for
	 */
	void
	cancel_timer(int opaque);




	/**
	 * @brief	Cancels all pending timer of this instance.
	 *
	 */
	void
	cancel_all_timer();


	/**@}*/



private:


private:


	/**
	 * Prepare struct fd_set's for select().
	 * This private method fills in fd_sets for calling select().
	 * Each ciosrv instance holds two STL-sets for storing read and
	 * write file descriptors. fdset() will query all ciosrv instances
	 * (stored in ciosrv::ciosrv_list) for the read/write descriptors
	 * and fill that into the struct fd_set suitable for calling select().
	 * It sets the maxfd number for select() appropriately and selects
	 * the next timeout from the ciosrv instances.
	 * @param maxfd The reference to maxfd will be set to the highest
	 * file descriptor value incremented by one
	 * @param readfds pointer to the read_fds structure
	 * @param writefds pointer to the write_fds structure
	 * @param ntimeout this reference value is filled with the next timeout value
	 *
	 */
	static void
	fdset(
			int& maxfd,
			fd_set* readfds,
			fd_set* writefds);

	/**
	 * find next timeout from all ciosrv instances
	 */
	static void
	next_timeout(
			time_t &ntimeout);

	/**
	 * Handle file descriptors after returning from select().
	 * This static method checks which file descriptors have had an IO event
	 * and will call the appropriate handler methods of the ciosrv instances, i.e.
	 * handle_revent(), handle_wevent(), handle_xevent(), handle_timeout().
	 * @param rc result code obtained from select() call
	 * @param readfds read fd_set structure as set by select()
	 * @param writefds write fd_set structure as set by select()
	 * @param exceptfds exception fd_set structure as set by select()
	 */
	static void
	handle(
			int rc,
			fd_set* readfds,
			fd_set* writefds,
			fd_set* exceptfds);

	/**
	 *
	 */
	static void
	handle_rfds(std::list<int>& rfds,
			int rc,
			fd_set* readfds,
			fd_set* exceptfds);


	/**
	 *
	 */
	static void
	handle_wfds(std::list<int>& wfds,
			int rc,
			fd_set* writefds,
			fd_set* exceptfds);


	/**
	 *
	 */
	static void
	handle_timeouts();

	/**
	 *
	 */
	static void
	handle_events(int rc,
			fd_set* readfds,
			fd_set* exceptfds);

	/**
	 * A signal handler.
	 * for SIGUSR1
	 */
	static void
	child_sig_handler (int x);

	/**
	 *
	 */
	void
	__handle_timeout();

public:

	friend std::ostream&
	operator<< (std::ostream& os, ciosrv const& iosvc) {
		os << indent(0) << "<ciosrv >";
		return os;
	};
};

}; // end of namespace

#endif
