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

#ifdef __cplusplus
extern "C" {
#endif
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

#ifdef __cplusplus
}
#endif

#include "cpacket.h"
#include "cmemory.h"
#include "cerror.h"
#include "cvastring.h"
#include "thread_helper.h"

#include "rofl/platform/unix/csyslog.h"
#include "rofl/platform/unix/cpipe.h"

/** class defining ioctl commands exchanged between ciosrv entities
 *
 */
class cevent :
	public csyslog
{
public:
	/**
	 */
	cevent(int __cmd = -1) : cmd(__cmd), opaque((size_t)0)
	{
		WRITELOG(CIOSRV, DBG, "cevent(%p)::cevent() cmd:0x%x", this, cmd);
	};
	/**
	 */
	virtual
	~cevent()
	{
		WRITELOG(CIOSRV, DBG, "cevent(%p)::~cevent() cmd:0x%x", this, cmd);
	};
	/**
	 */
	cevent(cevent const& ioctl) :
		cmd(-1),
		opaque((size_t)0)
	{
		WRITELOG(CIOSRV, DBG, "cevent(%p)::cevent() cmd:0x%x from 0x%x", this, cmd, &ioctl);
		*this = ioctl;
	};
	/**
	 */
	cevent& operator= (cevent const& ioctl)
	{
		if (this == &ioctl)
			return *this;
		cmd = ioctl.cmd;
		opaque = ioctl.opaque;
		return *this;
	};
	/**
	 *
	 */
	const char*
	c_str()
	{
		cvastring vas;
		info.assign(vas("cevent(%p): cmd: 0x%x opaque: %s", this, cmd, opaque.c_str()));
		//info.assign(vas("cevent(%p): cmd: 0x%x", this, cmd));
		return info.c_str();
	};

public: // data structures

	int cmd; // command
	cmemory opaque; // additional data
	std::string info; // info string :P

public: // auxiliary classes

	class cevent_find_by_cmd {
		int cmd;
	public:
		cevent_find_by_cmd(int __cmd) :
			cmd(__cmd) {};
		bool operator() (cevent const* ev) {
			return (ev->cmd == cmd);
		};
	};
};


/* error classes */
class eIoSvcBase			: public cerror {}; 	//< base error class for ciosrv
class eIoSvcInitFailed 		: public eIoSvcBase {};	//< init of ciosrv instance failed
class eIoSvcRunError 		: public eIoSvcBase {}; //< error in core loop (select)
class eIoSvcUnhandledTimer 	: public eIoSvcBase {}; //< unhandled timer
class eIoSvcNotFound        : public eIoSvcBase {}; //< element not found

/**
 * (Abstract) Base class for IO services.
 * This class is a base class that adds IO event support to a derived class.
 * ciosrv provides a static method for running an infinite loop of select()/poll()
 * to handle file/socket descriptors. Each instance adds methods for adding/removing
 * file descriptors to/from the set of monitored descriptors. A derived class
 * may overwrite handler methods for receiving events:
 * - handle_revent() read events
 * - handle_wevent() write events
 * - handle_xevent() exceptions
 * - handle_timeout() timeout events
 * Methods for descriptor management are:
 * - register_filedesc_r() register a descriptor for read IO
 * - deregister_filedesc_r() deregister a read descriptor
 * - register_filedesc_w() register a descriptor for write IO
 * - deregister_filedesc_w() deregister a write descriptoe
 * - register_timer() register a timer
 * - reset_timer() reset a timer
 * - cancel_timer() cancel a timer
 *
 */
class ciosrv : public virtual csyslog
{
	class ciodata {
	public:
		pthread_t tid;		// thread id
		cpipe *pipe;		// wakeup pipe
		std::map<int, ciosrv*> rfds; 	// read fds
		std::map<int, ciosrv*> wfds;	// write fds
		std::list<ciosrv*> ciosrv_timeouts; // set with all ciosrv instances with timeout in next round
		std::bitset<32> flags; //< flags
	public:
		/** constructor
		 *
		 */
		ciodata() :
			tid(pthread_self())
		{
			pipe = new cpipe();
		};
		~ciodata()
		{
			delete pipe;
		};
	};


protected:


	static std::map<pthread_t, ciodata*> iodata; // fds and timers for thread tid

	static std::map<pthread_t, pthread_rwlock_t> ciosrv_wakeup_rwlock; // rwlock for cevent lists

private: // static

	enum ciosrv_flag_t {
		CIOSRV_FLAG_WAKEUP_CALLED = (1 << 0), // when set, pipe was already instructed to
						// wake up called thread
	};

	static std::map<pthread_t, pthread_mutex_t> ciosrv_list_mutex; // mutex for cevent lists
	static std::map<pthread_t, std::set<class ciosrv*> > ciosrv_insertion_list; //< list of all ciosrv instances new inserted
	static std::map<pthread_t, std::set<class ciosrv*> > ciosrv_list; //< list of all ciosrv instances
	static std::map<pthread_t, std::set<class ciosrv*> > ciosrv_deletion_list; //< list of all ciosrv instances scheduled for deletion
	static std::map<pthread_t, std::set<class ciosrv*> > ciosrv_wakeup; //< list of all cioctl commands rcvd
	//static std::map<pthread_t, pthread_mutex_t*> ciosrvlock; //< mutexes for threads


	static std::map<pthread_t, int> evlockinit; // = 0 => destroy mutex

	/** dump information about all registered fdsets
	 *
	 */
	static void
	dump_fdsets();

	/** dump information about all active fdsets (FD_ISSET yields true)
	 *
	 */
	static void
	dump_active_fdsets(
			int rc,
			fd_set* readfds,
			fd_set* writefds,
			fd_set* exceptfds);



public: // static

	/** check for existence of ciosrv
	 *
	 */
	static ciosrv* ciosrv_exists(ciosrv* iosrv) throw (eIoSvcNotFound);

	/**
	 * Run the main loop for an infinite time.
	 * This static method provides the select call with an infinite loop.
	 * This method should be called by a main()-routine. Not thread-safe (yet).
	 * The select() call is blocking.
	 */
	static void run();

	/**
	 * Init static variables before running this ciosrv instance.
	 */
	static void init();


public:

	/**
	 * Constructor.
	 */
	ciosrv();

	/**
	 * Destructor.
	 */
	virtual
	~ciosrv();

	/**
	 * send notification to this ciosrv instance
	 */
	void
	notify(cevent const& ioctl = cevent());

	/**
	 *
	 */
	const char*
	c_str();

protected: // methods, to be overwritten by derived classes

	//
	// cioctl related methods
	//

	/**
	 * Handle cioctl events received
	 */
	virtual void
	handle_event(cevent const& ev)
	{
	};

	//
	// file descriptor related methods
	//

	/**
	 * Handle read events on a specific file descriptor.
	 * Called after return of select() call in the proper
	 * ciosrv instance.
	 * This method should be overwritten by a derived class
	 * to actually receive read IO events for file descriptors.
	 * @param fd The file descriptor that caused the read event
	 */
	virtual void
	handle_revent(int fd)
	{
	};

	/**
	 * Handle write events on a specific file descriptor.
	 * Called after return of select() call in the proper
	 * ciosrv instance.
	 * This method should be overwritten by a derived class
	 * to actually receive write IO events for file descriptors.
	 * @param fd The file descriptor that caused the write event
	 */
	virtual void
	handle_wevent(int fd)
	{
	};

	/**
	 * Handle exception events on a specific file descriptor.
	 * Called after return of select() call in the proper
	 * ciosrv instance.
	 * This method should be overwritten by a derived class
	 * to actually receive exception IO events for file descriptors.
	 * @param fd The file descriptor that caused the exception
	 */
	virtual void
	handle_xevent(int fd)
	{
	};

	/**
	 * Register a file descriptor for reading.
	 *
	 * This method may be called by a derived class for registering
	 * a file or socket descriptor. The descriptor will be stored
	 * in ciosrv::rfds and will be added in consecutive select() calls
	 * until it is removed by calling deregister_filedesc_r().
	 * @param fd The file descriptor to be observed for read IO events.
	 */
	virtual void register_filedesc_r(int fd);

	/**
	 * Deregister a file descriptor for reading.
	 *
	 * This method may be called by a derived class for deregistering
	 * a file or socket descriptor. The descriptor will be removed
	 * from ciosrv::rfds.
	 * @param fd The file descriptor to be removed.
	 */
	virtual void deregister_filedesc_r(int fd);

	/**
	 * Register a file descriptor for writing.
	 * This method may be called by a derived class for registering
	 * a file or socket descriptor. The descriptor will be stored
	 * in ciosrv::wfds and will be added in consecutive select() calls
	 * until it is removed by calling deregister_filedesc_w().
	 * @param fd The file descriptor to be observed for write IO events.
	 */
	virtual void register_filedesc_w(int fd);

	/**
	 * Deregister a file descriptor for writing.
	 * This method may be called by a derived class for deregistering
	 * a file or socket descriptor. The descriptor will be removed
	 * from ciosrv::wfds.
	 * @param fd The file descriptor to be removed.
	 */
	virtual void deregister_filedesc_w(int fd);

	//
	// timer related methods
	//

	/**
	 * Handle timeout events.
	 * This method is called once a timeout event happens for this
	 * ciosrv instance.
	 * @param opaque An opaque integer value freely selectable by the
	 * derived class. ciosrv will take care to prevent any name clashes.
	 * However, the user must not register the same opaque value twice,
	 * as this will lead to an unpredictable behaviour. Calling handle_timeout()
	 * may be delayed when there are other ciosrv instances requiring the processing
	 * on the CPU. Precision of timers is in seconds.
	 * @para, opaque An opaque integer handle to refer to an internal event.
	 */
	virtual void
	handle_timeout(int opaque)
	{
		throw eIoSvcUnhandledTimer();
	};

	/**
	 * Register a new timer.
	 * This method registers a new timer scheduled in 't' seconds. The opaque
	 * handle may be selected freely by this instance, but must not be in use
	 * already.
	 * @param opaque An opaque integer handle freely selectable by this ciosrv instance.
	 * @param t The timeout value this timer will expire.
	 */
	virtual void register_timer(int opaque, time_t t);

	/**
	 * Reset a timer of a specific type.
	 * This method resets an existing timer handle with a new timeout
	 * value, i.e. the old expiration date will be replaced with the new
	 * timeout value.
	 * @param opaque The timer handler to be reset.
	 * @param t The new timeout value.
	 */
	virtual void reset_timer(int opaque, time_t);

	/**
	 * Check for a pending timer handle.
	 * This method returns a boolean value indicating whether a specific
	 * opaque handle is registered active or not.
	 * @param opaque The timer handle.
	 * @return boolean value: true, timer handle is active, false otherwise
	 */
	virtual bool pending_timer(int opaque);

	/**
	 * Cancel an existing timer handle.
	 * This method cancels an opaque timer handle.
	 * @param opaque The timer handle to be canceled.
	 */
	virtual void cancel_timer(int opaque);

	/**
	 * Cancel all timers.
	 * This method is called in ciosrv's destructor to cancel
	 * all pending timers. It may also be called by a derived class
	 * any time to cancel all timers :)
	 */
	virtual void cancel_all_timer();

protected: // data structures

	std::list<cevent*> events;

private: // methods

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

public: // data structures

	pthread_t tid; // thread-id

protected: // data structures

	pthread_mutex_t event_mutex; //< mutex for event list

	pthread_mutex_t timer_mutex; //< mutex for timer_list

private: // data structures

	struct timeval* tv; //< timeval structure for timeout for select()

	std::map<time_t, std::list<int> > timers_list;

	cmemory tv_mem; //< memory for timeval structure

	std::string info;

};

#endif
