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
#include <sys/epoll.h>


#include "rofl/common/cmemory.h"
#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/cpipe.h"
#include "rofl/common/cevents.h"
#include "rofl/common/ctimers.h"
#include "rofl/common/ctimer.h"

namespace rofl {

class eRofIoLoopBase : public std::runtime_error {
public:
	eRofIoLoopBase(const std::string& __arg) : std::runtime_error(__arg) {};
};
class eRofIoLoopBusy : public eRofIoLoopBase {
public:
	eRofIoLoopBusy(const std::string& __arg) : eRofIoLoopBase(__arg) {};
};

class ciosrv;

/**
 * @brief	Defines an IO service loop for a single thread.
 * @ingroup common_devel_ioservice
 *
 *
 */
class cioloop {
private:

	/**
	 * @brief	Static C function for running an io loop instance forever in a new thread.
	 */
	static void*
	run_thread(void* arg) {

		pthread_t tid = pthread_self();

		// run io loop for this thread until someone calls method rofl::cioloop::get_loop().stop()
		rofl::cioloop::get_loop(tid).run();

		logging::debug << "[rofl-common][thread] stopping, tid: 0x" << std::hex << tid << std::dec << std::endl;

		// set result code, usually just 0
		rofl::cioloop::threads[tid] = 0;

		return &(rofl::cioloop::threads[tid]);
	};

	static PthreadRwLock 			threads_lock;
	static std::map<pthread_t, int> threads; // including result code

public:

	/**
	 * @brief	Create a new POSIX thread running an instance of class rofl::cioloop.
	 *
	 * Add objects of classes deriving from rofl::ciosrv. Use the thread's identifier
	 * (pthread_t) during its construction to assign the object to this particular
	 * thread.
	 */
	static pthread_t
	add_thread() {
		int rc = 0;
		pthread_t tid = 0;
		if ((rc = pthread_create(&tid, NULL, &cioloop::run_thread, NULL)) < 0) {
			throw eSysCall("pthread_create() failed");
		}
		logging::debug << "[rofl-common][cioloop][thread] starting, tid: 0x"
				<< std::hex << tid << std::dec << std::endl;
		RwLock(cioloop::threads_lock, RwLock::RWLOCK_WRITE);
		cioloop::threads[tid] = 0;
		return tid;
	};

	/**
	 * @brief	Stops a running rofl::cioloop instance and drops the previously created POSIX thread for this loop.
	 *
	 * @param tid identifier of thread to be removed
	 * @throw throws exception eRofIoLoopBusy, when there are still active
	 * elements assigned to this thread's IO loop
	 */
	static void
	drop_thread(pthread_t tid) {
		logging::debug << "[rofl-common][cioloop][thread] done, tid: 0x"
				<< std::hex << tid << std::dec << std::endl;
		int rc = 0;
		RwLock(cioloop::threads_lock, RwLock::RWLOCK_WRITE);
		if (cioloop::threads.find(tid) == cioloop::threads.end()) {
			return;
		}
		if (cioloop::get_loop(tid).has_active_elements()) {
			throw eRofIoLoopBusy("loop has still active elements");
		}
		cioloop::get_loop(tid).stop();
		if ((rc = pthread_join(tid, NULL)) < 0) {
			pthread_cancel(tid);
		}
		cioloop::threads.erase(tid);
		cioloop::drop_loop(tid);
	};

	/**
	 * @brief	Checks for existence of a certain pthread_t thread identifier.
	 */
	static bool
	has_thread(pthread_t tid) {
		RwLock(cioloop::threads_lock, RwLock::RWLOCK_READ);
		return (not (cioloop::threads.find(tid) == cioloop::threads.end()));
	};

private:

	static std::map<pthread_t, cioloop*> 	loops;
	static PthreadRwLock 					loops_rwlock;

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
		RwLock lock(rofl::cioloop::loops_rwlock, RwLock::RWLOCK_WRITE);
		if (cioloop::loops.find(tid) == cioloop::loops.end()) {
			logging::debug << "[rofl-common][cioloop][loop] creating loop, tid: 0x"
					<< std::hex << tid << std::dec << std::endl;
			cioloop::loops[tid] = new cioloop(tid);
		}
		return *(cioloop::loops[tid]);
	};

	/**
	 * @brief	Drop
	 */
	static void
	drop_loop(pthread_t tid) {
		RwLock lock(rofl::cioloop::loops_rwlock, RwLock::RWLOCK_WRITE);
		if (cioloop::loops.find(tid) == cioloop::loops.end()) {
			return;
		}
		logging::debug << "[rofl-common][cioloop][loop] destroying loop, tid: 0x"
				<< std::hex << tid << std::dec << std::endl;
		delete cioloop::loops[tid];
		cioloop::loops.erase(tid);
	};

	/**
	 *
	 */
	static bool
	has_loop(pthread_t tid) {
		RwLock lock(rofl::cioloop::loops_rwlock, RwLock::RWLOCK_READ);
		return (not (cioloop::loops.find(tid) == cioloop::loops.end()));
	};

	/**
	 * @brief	Instructs all running cioloop instances to terminate.
	 *
	 * Safe to be used by signal handlers.
	 */
	static void
	shutdown() {
		RwLock lock(rofl::cioloop::loops_rwlock, RwLock::RWLOCK_READ);
		for (std::map<pthread_t, cioloop*>::iterator
				it = cioloop::loops.begin(); it != cioloop::loops.end(); ++it) {
			it->second->flag_keep_on_running = false;		}
	};

	/**
	 * @brief	Instructs all running cioloop instances to terminate and waits for them to stop, then deletes them.
	 */
	static void
	cleanup_on_exit() {
		{
			RwLock lock(rofl::cioloop::loops_rwlock, RwLock::RWLOCK_READ);
			for (std::map<pthread_t, cioloop*>::iterator
					it = cioloop::loops.begin(); it != cioloop::loops.end(); ++it) {
				it->second->flag_keep_on_running = false;
				if (it->first != pthread_self()) {
					pthread_join(it->first, NULL);
					delete it->second;
				}
			}
		}
		cioloop::drop_loop(pthread_self());
		cioloop::loops.clear();
	};

public:

	/**
	 *
	 */
	void
	run()
	{ cioloop::get_loop().run_loop(); };

	/**
	 * @brief	Terminates cioloop instance running in thread identified by this->tid.
	 */
	void
	stop() {
		if (not has_loop(get_tid())) {
			return;
		}
		cioloop::loops[get_tid()]->flag_keep_on_running = false;
		wakeup();
	};

	/**
	 *
	 */
	bool
	has_active_elements() const {
		RwLock(ciolist_rwlock, RwLock::RWLOCK_READ);
		return (not ciolist.empty());
	};

	/**
	 *
	 */
	pthread_t
	get_tid() const
	{ return tid; };

protected:

	friend class ciosrv;

	/**
	 *
	 */
	void
	register_ciosrv(ciosrv* elem) {
		logging::trace << "[rofl-common][cioloop][register_ciosrv] svc:"
				<< elem << ", target tid: 0x" << std::hex << tid << std::dec
				<< ", running tid: 0x" << std::hex << pthread_self() << std::dec << std::endl;
		{
			RwLock lock(ciolist_rwlock, RwLock::RWLOCK_WRITE);
			ciolist.insert(elem);
			wakeup();
		}
		{
			RwLock lock(timers_rwlock, RwLock::RWLOCK_WRITE);
			if (timers.find(elem) == timers.end())
				timers[elem] = false;
		}
		{
			RwLock lock(events_rwlock, RwLock::RWLOCK_WRITE);
			if (events.find(elem) == events.end())
				events[elem] = false;
		}
	};

	/**
	 *
	 */
	void
	deregister_ciosrv(ciosrv* elem) {
		logging::trace << "[rofl-common][cioloop][deregister_ciosrv] svc:"
				<< elem << ", target tid: 0x" << std::hex << tid << std::dec
				<< ", running tid: 0x" << std::hex << pthread_self() << std::dec << std::endl;
		{
			RwLock lock(ciolist_rwlock, RwLock::RWLOCK_WRITE);
			ciolist.erase(elem);
			wakeup();
		}

		{
			RwLock lock(timers_rwlock, RwLock::RWLOCK_WRITE);
			for (std::map<ciosrv*, bool>::iterator
					it = timers.begin(); it != timers.end(); ++it) {
				if (elem == it->first) {
					timers.erase(it); break;
				}
			}
		}

		{
			RwLock lock(events_rwlock, RwLock::RWLOCK_WRITE);
			for (std::map<ciosrv*, bool>::iterator
					it = events.begin(); it != events.end(); ++it) {
				if (elem == it->first) {
					events.erase(it); break;
				}
			}
		}
	};

	/**
	 *
	 */
	bool
	has_ciosrv(ciosrv* elem) const {
		RwLock lock(ciolist_rwlock, RwLock::RWLOCK_READ);
		return (not (ciolist.find(elem) == ciolist.end()));
	};

	/**
	 *
	 */
	void
	add_readfd(ciosrv* iosrv, int fd) {
		{
			poll_events[fd].events |= EPOLLIN;

			if (NULL == poll_iosrvs[fd]) {
				RwLock lock(poll_rwlock, RwLock::RWLOCK_WRITE);
				poll_iosrvs[fd] = iosrv;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &poll_events[fd]) < 0) {
					switch (errno) {
					case EEXIST:
					case ENOENT: {
						rofl::logging::debug << "rofl::cioloop::add_readfd() " << eSysCall("epoll_ctl()") << std::endl;
					} break;
					default: {
						throw eSysCall("epoll_ctl()");
					}
					}
				}
			} else {
				if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &poll_events[fd]) < 0) {
					switch (errno) {
					case EEXIST:
					case ENOENT: {
						rofl::logging::debug << "rofl::cioloop::add_readfd() " << eSysCall("epoll_ctl()") << std::endl;
					} break;
					default: {
						throw eSysCall("epoll_ctl()");
					}
					}
				}
			}
		}
		logging::trace << "[rofl-common][cioloop][add_readfd] fd:" << fd
				<< ", tid: 0x" << std::hex << tid << std::dec << std::endl << *this;
		wakeup(); // wakeup main loop, just in case
	};

	/**
	 *
	 */
	void
	drop_readfd(ciosrv* iosrv, int fd) {
		{
			poll_events[fd].events &= ~EPOLLIN;

			if (EPOLLET == poll_events[fd].events) {
				RwLock lock(poll_rwlock, RwLock::RWLOCK_WRITE);
				poll_iosrvs[fd] = NULL;
				if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &poll_events[fd]) < 0) {
					switch (errno) {
					case EEXIST:
					case ENOENT: {
						rofl::logging::debug << "rofl::cioloop::drop_readfd() " << eSysCall("epoll_ctl()") << std::endl;
					} break;
					default: {
						throw eSysCall("epoll_ctl()");
					}
					}
				}
			} else {
				if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &poll_events[fd]) < 0) {
					switch (errno) {
					case EEXIST:
					case ENOENT: {
						rofl::logging::debug << "rofl::cioloop::drop_readfd() " << eSysCall("epoll_ctl()") << std::endl;
					} break;
					default: {
						throw eSysCall("epoll_ctl()");
					}
					}
				}
			}
		}
		logging::trace << "[rofl-common][cioloop][drop_readfd] fd:" << fd
				<< ", tid: 0x" << std::hex << tid << std::dec << std::endl << *this;
		wakeup(); // wakeup main loop, just in case
	};

	/**
	 *
	 */
	void
	add_writefd(ciosrv* iosrv, int fd) {
		{
			RwLock lock(poll_rwlock, RwLock::RWLOCK_WRITE);

			poll_events[fd].events |= EPOLLOUT;

			if (NULL == poll_events[fd].data.ptr) {
				poll_iosrvs[fd] = iosrv;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &poll_events[fd]) < 0) {
					switch (errno) {
					case EEXIST:
					case ENOENT: {
						rofl::logging::debug << "rofl::cioloop::add_writefd() " << eSysCall("epoll_ctl()") << std::endl;
					} break;
					default: {
						throw eSysCall("epoll_ctl()");
					}
					}
				}
			} else {
				if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &poll_events[fd]) < 0) {
					switch (errno) {
					case EEXIST:
					case ENOENT: {
						rofl::logging::debug << "rofl::cioloop::add_writefd() " << eSysCall("epoll_ctl()") << std::endl;
					} break;
					default: {
						throw eSysCall("epoll_ctl()");
					}
					}
				}
			}
		}
		logging::trace << "[rofl-common][cioloop][add_writefd] fd:" << fd
				<< ", tid: 0x" << std::hex << tid << std::dec << std::endl << *this;
		wakeup(); // wakeup main loop, just in case
	};

	/**
	 *
	 */
	void
	drop_writefd(ciosrv* iosrv, int fd) {
		{
			poll_events[fd].events &= ~EPOLLOUT;

			if (EPOLLET == poll_events[fd].events) {
				RwLock lock(poll_rwlock, RwLock::RWLOCK_WRITE);
				poll_iosrvs[fd] = NULL;
				if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &poll_events[fd]) < 0) {
					switch (errno) {
					case EEXIST:
					case ENOENT: {
						rofl::logging::debug << "rofl::cioloop::drop_writefd() " << eSysCall("epoll_ctl()") << std::endl;
					} break;
					default: {
						throw eSysCall("epoll_ctl()");
					}
					}
				}
			} else {
				if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &poll_events[fd]) < 0) {
					switch (errno) {
					case EEXIST:
					case ENOENT: {
						rofl::logging::debug << "rofl::cioloop::drop_writefd() " << eSysCall("epoll_ctl()") << std::endl;
					} break;
					default: {
						throw eSysCall("epoll_ctl()");
					}
					}
				}
			}

		}
		logging::trace << "[rofl-common][cioloop][drop_writefd] fd:" << fd
				<< ", tid: 0x" << std::hex << tid << std::dec << std::endl << *this;
		wakeup(); // wakeup main loop, just in case
	};

	/**
	 *
	 */
	void
	has_timer(ciosrv* iosrv) {
		flag_new_timer_installed = true;
		RwLock lock(timers_rwlock, RwLock::RWLOCK_READ);
		timers[iosrv] = true; // already exists => previously created in register_ciosrv
		wakeup(); // wakeup main loop, just in case
	};

	/**
	 *
	 */
	void
	has_no_timer(ciosrv *iosrv) {
		RwLock lock(timers_rwlock, RwLock::RWLOCK_READ);
		timers[iosrv] = false; // already exists => previously created in register_ciosrv
	};

	/**
	 *
	 */
	void
	has_event(ciosrv* iosrv) {
		flag_new_event_installed = true;
		RwLock lock(events_rwlock, RwLock::RWLOCK_READ);
		events[iosrv] = true; // already exists => previously created in register_ciosrv
		wakeup(); // wakeup main loop, just in case
	};

	/**
	 *
	 */
	void
	has_no_event(ciosrv* iosrv) {
		RwLock lock(events_rwlock, RwLock::RWLOCK_READ);
		events[iosrv] = false; // already exists => previously created in register_ciosrv
	};

private:

	/**
	 *
	 */
	cioloop(
			pthread_t tid = 0,
			unsigned int max_num_poll_events = DEFAULT_NUM_POLL_EVENTS) :
				flag_keep_on_running(false),
				flag_wait_on_kernel(false),
				flag_waking_up(false),
				flag_new_event_installed(false),
				flag_new_timer_installed(false),
				tid(tid),
				epollfd(-1),
				max_num_poll_events(max_num_poll_events) {
		if (0 == tid) {
			this->tid = pthread_self();
		}

		struct rlimit rlim;
		if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
			throw eSysCall("getrlimit()");
		}

		for (unsigned int i = 0; i < this->max_num_poll_events; i++) {
			struct epoll_event event;
			memset(&event, 0, sizeof(event));
			event.data.fd = i;
			event.events |= EPOLLET; // edge triggered
			poll_events.push_back(event);
		}

		if ((epollfd = epoll_create(max_num_poll_events)) < 0) {
			throw eSysCall("epoll_create()");
		}

		poll_events[pipe.pipefd[0]].events |= (EPOLLET | EPOLLIN);
		poll_events[pipe.pipefd[0]].data.fd = pipe.pipefd[0];
		if ((epoll_ctl(epollfd, EPOLL_CTL_ADD, pipe.pipefd[0],
				&poll_events[pipe.pipefd[0]])) < 0) {
			throw eSysCall("epoll_ctl()");
		}
	};

	/**
	 *
	 */
	virtual
	~cioloop()
	{ close(epollfd); };

	/**
	 *
	 */
	cioloop(
			const cioloop& t)
	{ *this = t; };

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

	/**
	 * @brief	Wake up this loop from this or other thread.
	 */
	void
	wakeup() {
		logging::trace << "[rofl-common][cioloop][wakeup] waking up thread, "
				<< "tid: 0x" << std::hex << tid << std::dec << std::endl;
		if (not flag_waking_up) {
			flag_waking_up = true;
			pipe.writemsg('1');
		}
	};

private:

	void
	run_on_timers(
			std::pair<ciosrv*, ctimespec>& next_timeout);

	void
	run_on_events();

	void
	run_on_kernel(
			std::pair<ciosrv*, ctimespec>& next_timeout);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cioloop const& ioloop) {
		os << indent(0) << "<cioloop tid:0x"
				<< std::hex << ioloop.get_tid() << std::dec << ">" << std::endl;

		{
			RwLock lock(ioloop.poll_rwlock, RwLock::RWLOCK_READ);
			os << indent(2) << "<instances with rfds: ";
			for (std::vector<struct epoll_event>::const_iterator
					it = ioloop.poll_events.begin(); it != ioloop.poll_events.end(); ++it) {
				if (ioloop.poll_iosrvs.find(it->data.fd) == ioloop.poll_iosrvs.end()) {
					continue;
				}
				if (it->events & EPOLLIN) {
					os << ioloop.poll_iosrvs.at(it->data.fd) << ":" << it->data.fd << " ";
				}
			}
			os << ">" << std::endl;
		}

		{
			RwLock lock(ioloop.poll_rwlock, RwLock::RWLOCK_READ);
			os << indent(2) << "<instances with wfds: ";
			for (std::vector<struct epoll_event>::const_iterator
					it = ioloop.poll_events.begin(); it != ioloop.poll_events.end(); ++it) {
				if (ioloop.poll_iosrvs.find(it->data.fd) == ioloop.poll_iosrvs.end()) {
					continue;
				}
				if (it->events & EPOLLOUT) {
					os << ioloop.poll_iosrvs.at(it->data.fd) << ":" << it->data.fd << " ";
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

	/* use separate boolean values as flags:
	 * bitset is not thread-safe and I am too lazy to write a wrapper ... */
	bool									flag_keep_on_running;
	bool									flag_wait_on_kernel;
	bool									flag_waking_up;
	bool									flag_new_event_installed;
	bool									flag_new_timer_installed;

	std::set<ciosrv*> 						ciolist;
	mutable PthreadRwLock 					ciolist_rwlock;
	std::map<ciosrv*, bool>					timers;
	mutable PthreadRwLock					timers_rwlock;
	std::map<ciosrv*, bool>					events;
	mutable PthreadRwLock					events_rwlock;

	cpipe									pipe;
	pthread_t        			       		tid;

	int										epollfd;
	static unsigned int						DEFAULT_NUM_POLL_EVENTS;
	unsigned int							max_num_poll_events;
	mutable PthreadRwLock					poll_rwlock;
	std::vector<struct epoll_event>			poll_events;
	std::map<int, ciosrv*>					poll_iosrvs;
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
	ciosrv(
			pthread_t tid = 0);

	/**
	 * @brief	Deallocates resources for this ciosrv object.
	 */
	virtual
	~ciosrv();

	/**
	 * @brief	Initializes all structures for this ciosrv object.
	 */
	ciosrv(
			const ciosrv& svc)
	{ *this = svc; };

	/**
	 *
	 */
	ciosrv&
	operator= (
			const ciosrv& svc) {
		if (this == &svc)
			return *this;
		throw eNotImplemented();
		return *this;
	};

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
		rofl::cioloop::get_loop(get_thread_id()).has_event(this);
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
		RwLock lock(rfds_rwlock, RwLock::RWLOCK_WRITE);
		rfds.insert(fd);
		rofl::cioloop::get_loop(get_thread_id()).add_readfd(this, fd);
	};

	/**
	 * @brief	Deregisters a file descriptor from read events.
	 *
	 * @param fd the file descriptor removed from the set of read events
	 */
	void
	deregister_filedesc_r(int fd) {
		RwLock lock(rfds_rwlock, RwLock::RWLOCK_WRITE);
		rfds.erase(fd);
		rofl::cioloop::get_loop(get_thread_id()).drop_readfd(this, fd);
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
		RwLock lock(wfds_rwlock, RwLock::RWLOCK_WRITE);
		wfds.insert(fd);
		rofl::cioloop::get_loop(get_thread_id()).add_writefd(this, fd);
	};

	/**
	 * @brief	Deregisters a file descriptor from write events.
	 *
	 * @param fd the file descriptor removed from the set of write events
	 */
	void
	deregister_filedesc_w(int fd) {
		RwLock lock(wfds_rwlock, RwLock::RWLOCK_WRITE);
		wfds.erase(fd);
		rofl::cioloop::get_loop(get_thread_id()).drop_writefd(this, fd);
	};

	/**@}*/

protected:

	/**
	 *
	 */
	bool
	has_next_event() const
	{ return (not events.empty()); };

	/**
	 *
	 */
	ctimer
	get_next_timer()
	{ return timers.get_next_timer(); };

	/**
	 *
	 */
	bool
	has_next_timer() const
	{ return (not (timers.empty())); };

	/**
	 *
	 */
	ctimer
	get_expired_timer()
	{ return timers.get_expired_timer(); };

	/**
	 *
	 */
	bool
	has_expired_timer() const
	{ return timers.has_expired_timer(); };

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
		rofl::cioloop::get_loop(get_thread_id()).has_timer(this);
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
		rofl::cioloop::get_loop(get_thread_id()).has_timer(this);
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
			rofl::cioloop::get_loop(get_thread_id()).has_no_timer(this);
	};

	/**
	 * @brief	Cancels all pending timers of this instance.
	 *
	 */
	void
	cancel_all_timers() {
		timers.clear();
		rofl::cioloop::get_loop(get_thread_id()).has_no_timer(this);
	};

	/**
	 * @brief	Cancels all pending events of this instance.
	 *
	 */
	void
	cancel_all_events() {
		events.clear();
		rofl::cioloop::get_loop(get_thread_id()).has_no_event(this);
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
			rofl::logging::trace << "[rofl-common][ciosrv][handle_timeout] #timers: "
					<< timers.size() << std::endl << *this;
			ctimer timer = timers.get_expired_timer();
			rofl::logging::trace << "[rofl-common][ciosrv][handle_timeout] timer: "
					<< (ctimer::now().get_timespec() - timer.get_timespec()).str() << std::endl;
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

	pthread_t						tid;
	std::set<int>					rfds;
	mutable PthreadRwLock 			rfds_rwlock;
	std::set<int>					wfds;
	mutable PthreadRwLock 			wfds_rwlock;
	ctimers							timers; // has its own locking
	cevents							events; // has its own locking
};

}; // end of namespace

#endif
