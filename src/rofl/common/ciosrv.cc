/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/ciosrv.h"

using namespace rofl;

/*static*/PthreadRwLock 				cioloop::threads_lock;
/*static*/std::map<pthread_t, int> 		cioloop::threads;
/*static*/PthreadRwLock 				cioloop::loops_rwlock;
/*static*/std::map<pthread_t, cioloop*> cioloop::loops;


ciosrv::ciosrv(
		pthread_t tid) :
				tid(tid)
{
	if (0 == tid) {
		this->tid = pthread_self();
	}
	rofl::logging::debug2 << "[rofl-common][ciosrv] "
			<< "constructor " << std::hex << this << std::dec
			<< ", parameter tid: " << std::hex << tid << std::dec
			<< ", target tid: " << std::hex << get_thread_id() << std::dec
			<< ", running tid: " << std::hex << pthread_self() << std::dec
			<< std::endl;
	rofl::cioloop::get_loop(get_thread_id()).register_ciosrv(this);
}


ciosrv::~ciosrv()
{
	rofl::logging::debug2 << "[rofl-common][ciosrv] "
			<< "destructor " << std::hex << this << std::dec
			<< ", target tid: " << std::hex << get_thread_id() << std::dec
			<< ", running tid: " << std::hex << pthread_self() << std::dec
			<< std::endl;
	timers.clear();
	events.clear();
	rofl::cioloop::get_loop(get_thread_id()).has_no_timer(this);
	rofl::cioloop::get_loop(get_thread_id()).has_no_event(this);
	for (std::set<int>::iterator
			it = rfds.begin(); it != rfds.end(); ++it) {
		rofl::cioloop::get_loop(get_thread_id()).drop_readfd(this, (*it));
	}
	for (std::set<int>::iterator
			it = wfds.begin(); it != wfds.end(); ++it) {
		rofl::cioloop::get_loop(get_thread_id()).drop_writefd(this, (*it));
	}
	rofl::cioloop::get_loop(get_thread_id()).deregister_ciosrv(this);
}



void
ciosrv::__handle_event()
{
	try {

		if (not rofl::cioloop::get_loop(get_thread_id()).has_ciosrv(this)) {
			rofl::logging::trace << "[rofl-common][ciosrv][event] ciosrv instance deleted, "
					<< "returning from event loop" << std::endl;
			return;
		}

		cevents clone = events; events.clear();

		while (not clone.empty()) {
			cevent event = clone.get_event();
			rofl::logging::trace << "[rofl-common][ciosrv][handle_event] event: " << event.get_cmd()
					<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
			if (not rofl::cioloop::get_loop(get_thread_id()).has_ciosrv(this)) {
				rofl::logging::trace << "[rofl-common][ciosrv][event] ciosrv instance deleted, "
						<< "returning from event loop" << std::endl;
				return;
			}
			handle_event(event);
		}

	} catch (eEventsNotFound& e) {
		// do nothing
	}
}




/* static */
void
cioloop::child_sig_handler (int x) {
	rofl::logging::debug <<  "[rofl-common][cioloop][child-sig-handler] got signal: " << x << std::endl;
    // signal(SIGCHLD, child_sig_handler);
	switch (x) {
	case SIGINT:
	case SIGTERM: {
		rofl::cioloop::get_loop().shutdown();
	} break;
	}
}



void
cioloop::run_loop()
{
	// must be called in assigned thread only
	if (tid != pthread_self()) {
		return;
	}

	// do not start a second loop
	if (flag_keep_on_running) {
		return;
	}
	flag_keep_on_running = true;

	/*
	 * signal masks, etc.
	 */
	sigset_t sigmask;
	struct sigaction sa;

	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGCHLD);


	if (sigprocmask(SIG_BLOCK, &sigmask, NULL) == -1) {
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	sa.sa_flags = 0;
	sa.sa_handler = child_sig_handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	std::pair<ciosrv*, ctimespec> next_timeout(NULL, ctimespec::now() + ctimespec(3600));

	/*
	 * the infinite loop ...
	 */
	while (flag_keep_on_running) {

		do {
			run_on_events();
			run_on_timers(next_timeout);
		} while (flag_has_timer || flag_has_event);

		run_on_kernel(next_timeout);
	}

	flag_keep_on_running = false;

	rofl::logging::debug << "[rofl-common][cioloop][run] terminating,"
			<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
}



bool
cioloop::run_on_timers(
		std::pair<ciosrv*, ctimespec>& next_timeout)
{
	rofl::logging::trace << "[rofl-common][cioloop][run_on_timers] looking for timers,"
			<< " tid: 0x" << std::hex << tid << std::dec << std::endl;

	while (flag_has_timer) {

		std::list<ciosrv*> clone;

		// make a copy of timers map
		{
			flag_has_timer = false;
			RwLock lock(timers_rwlock, RwLock::RWLOCK_READ);
			for (std::map<ciosrv*, bool>::iterator
					it = timers.begin(); it != timers.end(); ++it) {
				if (true == it->second)
					clone.push_back(it->first);
			}
		}

		rofl::logging::trace << "[rofl-common][cioloop][run_on_timers] there are " << clone.size()
							<< " active timer(s), tid: 0x" << std::hex << tid << std::dec << std::endl;

		// iterate over all elements with active timer needs
		for (std::list<ciosrv*>::iterator
				it = clone.begin(); it != clone.end(); ++it) {
			ciosrv* svc = *it;

			if (not has_ciosrv(svc) || not svc->has_next_timer())
				continue;

			while (svc->has_expired_timer()) {
				try {
					rofl::logging::trace << "[rofl-common][cioloop][run_on_timers] urgent timer found,"
							<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
					// this may set again FLAG_HAS_TIMER
					svc->__handle_timeout();
				} catch (RoflException& e) {/* do nothing */};
			}

			// no more urgent timers, extract next potential timeout for select/poll/...
			if ((svc->has_next_timer()) &&
					(svc->get_next_timer().get_timespec() < next_timeout.second)) {
				next_timeout = std::pair<ciosrv*, ctimespec>( svc, svc->get_next_timer().get_timespec() );
				rofl::logging::trace << "[rofl-common][cioloop][run_on_timers] normal timer found, "
						<< "tid: 0x" << std::hex << tid << std::dec << ", timeout: "
						<< (svc->get_next_timer().get_timespec() - ctimespec::now()).str() << std::endl;
			}
		}
	};

	rofl::logging::trace << "[rofl-common][cioloop][run_on_timers] done with urgent timers,"
			<< " next timeout: " << (next_timeout.second - ctimespec::now()).str()
			<< " tid: 0x" << std::hex << tid << std::dec
			<< std::endl;
	return false;
}



bool
cioloop::run_on_events()
{
	while (flag_has_event) {
		rofl::logging::trace << "[rofl-common][cioloop][run_on_events] looking for events,"
				<< " tid: 0x" << std::hex << tid << std::dec << std::endl;

		std::list<ciosrv*> clone;

		// make a copy of events map
		{
			flag_has_event = false;
			RwLock lock(events_rwlock, RwLock::RWLOCK_READ);
			for (std::map<ciosrv*, bool>::iterator
					it = events.begin(); it != events.end(); ++it) {
				if (true == it->second)
					clone.push_back(it->first);
			}
		}

		rofl::logging::trace << "[rofl-common][cioloop][run_on_events] there are " << clone.size()
							<< " active event(s), tid: 0x" << std::hex << tid << std::dec << std::endl;

		for (std::list<ciosrv*>::iterator
				it = clone.begin(); it != clone.end(); ++it) {
			if (not has_ciosrv(*it))
				continue;
			(*(*it)).__handle_event();
		}
	}

	return false;
}



void
cioloop::run_on_kernel(std::pair<ciosrv*, ctimespec>& next_timeout)
{
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;
	unsigned int maxfd = 0;
	int rc = 0;

	FD_ZERO(&exceptfds);

	FD_ZERO(&readfds);
	{
		RwLock lock(rfds_rwlock, RwLock::RWLOCK_READ);
		for (unsigned int i = minrfd; i < maxrfd; i++) {
			if (NULL != rfds[i]) {
				rofl::logging::trace << "[rofl-common][cioloop][run_on_kernel] adding rfds: " << i << std::endl;
				FD_SET(i, &readfds);
				FD_SET(i, &exceptfds);
				maxfd = (i > maxfd) ? i : maxfd;
			}
		}
	}
	FD_SET(pipe.pipefd[0], &readfds);
	FD_SET(pipe.pipefd[0], &exceptfds);

	maxfd = ((unsigned int)pipe.pipefd[0] > maxfd) ? pipe.pipefd[0] : maxfd;

	FD_ZERO(&writefds);
	{
		RwLock lock(wfds_rwlock, RwLock::RWLOCK_READ);
		for (unsigned int i = minwfd; i < maxwfd; i++) {
			if (NULL != wfds[i]) {
				rofl::logging::trace << "[rofl-common][cioloop][run_on_kernel] adding wfds: " << i << std::endl;
				FD_SET(i, &writefds);
				FD_SET(i, &exceptfds);
				maxfd = (i > maxfd) ? i : maxfd;
			}
		}
	}

	if (not flag_keep_on_running)
		return;

	rofl::indent::null();

	struct timespec ts;
	ctimespec now(ctimespec::now());
	if ( next_timeout.second < now ) {
		ts.tv_nsec = 0;
		ts.tv_sec = 0;
	} else {
		ts = (next_timeout.second - now).get_timespec();
	}

	rofl::logging::trace << "[rofl-common][cioloop][run] before select,"
			<< " next timeout: " << ctimespec(ts).str()
			<< " tid: 0x" << std::hex << tid << std::dec << std::endl << *this;

	// blocking
	flag_wait_on_kernel = true;
	rc = pselect(maxfd + 1, &readfds, &writefds, &exceptfds, &(ts), NULL);
	flag_wait_on_kernel = false;

	rofl::logging::trace << "[rofl-common][cioloop][run] after select,"
			<< " tid: 0x" << std::hex << tid << std::dec << std::endl << *this;

	if (rc < 0) {

		switch (errno) {
		case EINTR:
			break;
		default:
			rofl::logging::error << "[rofl-common][cioloop][run] " << eSysCall("pselect()") << std::endl;
			break;
		}

	} else if ((0 == rc)/* || (EINTR == errno)*/) {

		if ((NULL != next_timeout.first) && has_ciosrv(next_timeout.first)) {
			rofl::logging::trace << "[rofl-common][cioloop][run] timeout event: "
					<< next_timeout.first << std::endl;
			next_timeout.first->__handle_timeout();
		}

		next_timeout = std::pair<ciosrv*, ctimespec>(NULL, ctimespec::now() + ctimespec(3600));

	} else { // rc > 0
		try {

			// exceptions
			for (unsigned int i = std::min(minrfd, minwfd); i < maxfd; i++) {
				if (FD_ISSET(i, &exceptfds)) {
					if ((NULL != rfds[i]) && has_ciosrv(rfds[i])) {
						rofl::logging::trace << "[rofl-common][cioloop][run] checking for exceptfds: "
								<< std::hex << rfds[i] << std::dec << std::endl;
						rfds[i]->handle_xevent(i);
					} else
					if ((NULL != wfds[i]) && has_ciosrv(wfds[i])) {
						rofl::logging::trace << "[rofl-common][cioloop][run] checking for exceptfds: "
								<< std::hex << rfds[i] << std::dec << std::endl;
						wfds[i]->handle_xevent(i);
					}
				}
			}

			// write events
			for (unsigned int i = minwfd; i < maxwfd; i++) {
				if (FD_ISSET(i, &writefds) && (NULL != wfds[i]) && has_ciosrv(wfds[i])) {
					rofl::logging::trace << "[rofl-common][cioloop][run]"
							<< " checking for wfds: " << i << " on " << std::hex << wfds[i] << std::dec
							<< " tid: 0x" << std::hex << tid << std::dec << std::endl;;
					wfds[i]->handle_wevent(i);
				}
			}

			// read events
			for (unsigned int i = minrfd; i < maxrfd; i++) {
				if (FD_ISSET(i, &readfds) && (NULL != rfds[i]) && has_ciosrv(rfds[i])) {
					rofl::logging::trace << "[rofl-common][cioloop][run]"
							<< " checking for rfds: " << i << " on " << std::hex << rfds[i] << std::dec
							<< " tid: 0x" << std::hex << tid << std::dec << std::endl;;
					rfds[i]->__handle_revent(i);
				}
			}

			// pipe
			if (FD_ISSET(pipe.pipefd[0], &readfds)) {
				rofl::logging::trace << "[rofl-common][cioloop][run] wakeup signal received via pipe"
						<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
				flag_waking_up = false;
				pipe.recvmsg();
			}

		} catch (rofl::RoflException& e) {
			rofl::logging::error << "[rofl-common][cioloop][run] caught "
					<< "RoflException in main loop: " << e.what() << std::endl;
			rofl::indent::null();

		} catch (std::exception& e) {
			rofl::logging::error << "[rofl-common][cioloop][run] caught "
					<< "std::exception in main loop: " << e.what() << std::endl;
			rofl::indent::null();
			flag_keep_on_running = false;
			throw;
		}
	}
}



