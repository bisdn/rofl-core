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
#ifndef NDEBUG
			rofl::logging::trace << "[rofl-common][ciosrv][event] ciosrv instance deleted, "
					<< "returning from event loop" << std::endl;
#endif
			return;
		}

		cevents clone = events; events.clear();

		while (not clone.empty()) {
			cevent event = clone.get_event();
#ifndef NDEBUG
			rofl::logging::trace << "[rofl-common][ciosrv][handle_event] event: " << event.get_cmd()
					<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
#endif
			if (not rofl::cioloop::get_loop(get_thread_id()).has_ciosrv(this)) {
#ifndef NDEBUG
				rofl::logging::trace << "[rofl-common][ciosrv][event] ciosrv instance deleted, "
						<< "returning from event loop" << std::endl;
#endif
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
	struct sigaction sa;

	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGCHLD);


	if (pthread_sigmask(SIG_BLOCK, &sigmask, NULL) == -1) {
		perror("pthread_sigmask");
		exit(EXIT_FAILURE);
	}

	sa.sa_flags = 0;
	sa.sa_handler = child_sig_handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	// register pipe read file descriptor
	{
		check_poll_set(pipe.pipefd[0]);
		RwLock lock(poll_rwlock, RwLock::RWLOCK_READ);
		poll_events[pipe.pipefd[0]].events |= (EPOLLET | EPOLLIN);
		poll_events[pipe.pipefd[0]].data.fd = pipe.pipefd[0];
		if ((epoll_ctl(epollfd, EPOLL_CTL_ADD, pipe.pipefd[0],
				&poll_events[pipe.pipefd[0]])) < 0) {
			throw eSysCall("epoll_ctl()");
		}
	}

	std::pair<ciosrv*, ctimespec> next_timeout(NULL, ctimespec::now() + ctimespec(3600));

	/*
	 * the infinite loop ...
	 */
	while (flag_keep_on_running) {

		do {
			run_on_events();
			run_on_timers(next_timeout);
		} while (flag_new_timer_installed || flag_new_event_installed);

		run_on_kernel(next_timeout);
	}

	// deregister pipe read file descriptor
	{
		RwLock lock(poll_rwlock, RwLock::RWLOCK_WRITE);
		if (poll_events.find(pipe.pipefd[0]) != poll_events.end()) {
			if ((epoll_ctl(epollfd, EPOLL_CTL_DEL, pipe.pipefd[0],
					&poll_events[pipe.pipefd[0]])) < 0) {
				// do nothing
			}
			poll_events.erase(pipe.pipefd[0]);
		}
	}

	flag_keep_on_running = false;

	rofl::logging::debug << "[rofl-common][cioloop][run] terminating,"
			<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
}



void
cioloop::run_on_timers(
		std::pair<ciosrv*, ctimespec>& next_timeout)
{
#ifndef NDEBUG
	rofl::logging::trace << "[rofl-common][cioloop][run_on_timers] looking for timers,"
			<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
#endif

	do {

		std::list<ciosrv*> clone;

		// make a copy of timers map
		{
			flag_new_timer_installed = false;
			RwLock lock(timers_rwlock, RwLock::RWLOCK_READ);
			for (std::map<ciosrv*, bool>::iterator
					it = timers.begin(); it != timers.end(); ++it) {
				if ((true == it->second) || (it->first->has_next_timer()))
					clone.push_back(it->first);
			}
		}

		if (clone.empty()) {
			next_timeout = std::pair<ciosrv*, ctimespec>(NULL, ctimespec::now() + ctimespec(3600));
			return;
		}

#ifndef NDEBUG
		rofl::logging::trace << "[rofl-common][cioloop][run_on_timers] there are " << clone.size()
							<< " active timer(s), tid: 0x" << std::hex << tid << std::dec << std::endl;
#endif

		// iterate over all elements with active timer needs
		for (std::list<ciosrv*>::iterator
				it = clone.begin(); it != clone.end(); ++it) {
			ciosrv* svc = *it;

			if (not has_ciosrv(svc) || not svc->has_next_timer())
				continue;

			while (svc->has_expired_timer()) {
				try {
#ifndef NDEBUG
					rofl::logging::trace << "[rofl-common][cioloop][run_on_timers] urgent timer found,"
							<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
#endif
					// this may set again FLAG_HAS_TIMER
					svc->__handle_timeout();
				} catch (RoflException& e) {/* do nothing */};
			}

			// no more urgent timers, extract next potential timeout for select/poll/...
			if ((svc->has_next_timer()) &&
					(svc->get_next_timer().get_timespec() < next_timeout.second)) {
				next_timeout = std::pair<ciosrv*, ctimespec>( svc, svc->get_next_timer().get_timespec() );
#ifndef NDEBUG
				rofl::logging::trace << "[rofl-common][cioloop][run_on_timers] normal timer found, "
						<< "tid: 0x" << std::hex << tid << std::dec << ", timeout: "
						<< (svc->get_next_timer().get_timespec() - ctimespec::now()).str() << std::endl;
#endif
			}
		}
	} while (flag_new_timer_installed);

#ifndef NDEBUG
	rofl::logging::trace << "[rofl-common][cioloop][run_on_timers] done with urgent timers,"
			<< " next timeout: " << (next_timeout.second - ctimespec::now()).str()
			<< " tid: 0x" << std::hex << tid << std::dec
			<< std::endl;
#endif
}



void
cioloop::run_on_events()
{
	do {
#ifndef NDEBUG
		rofl::logging::trace << "[rofl-common][cioloop][run_on_events] looking for events,"
				<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
#endif

		std::list<ciosrv*> clone;

		// make a copy of events map
		{
			flag_new_event_installed = false;
			RwLock lock(events_rwlock, RwLock::RWLOCK_READ);
			for (std::map<ciosrv*, bool>::iterator
					it = events.begin(); it != events.end(); ++it) {
				if ((true == it->second) || (it->first->has_next_event()))
					clone.push_back(it->first);
			}
		}

#ifndef NDEBUG
		rofl::logging::trace << "[rofl-common][cioloop][run_on_events] there are " << clone.size()
							<< " active event(s), tid: 0x" << std::hex << tid << std::dec << std::endl;
#endif

		for (std::list<ciosrv*>::iterator
				it = clone.begin(); it != clone.end(); ++it) {
			if (not has_ciosrv(*it))
				continue;
			(*(*it)).__handle_event();
		}
	} while (flag_new_event_installed);
}



void
cioloop::run_on_kernel(std::pair<ciosrv*, ctimespec>& next_timeout)
{
	int rc = 0;

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

#ifndef NDEBUG
	rofl::logging::trace << "[rofl-common][cioloop][run] before epoll_wait,"
			<< " next timeout: " << ctimespec(ts).str()
			<< " tid: 0x" << std::hex << tid << std::dec << std::endl << *this;
#endif

	// blocking
	flag_wait_on_kernel = true;
	std::vector<struct epoll_event> events(poll_events.size());
	int timeout = ts.tv_sec * 1000 + (ts.tv_nsec / 1e6);
	rc = epoll_pwait(epollfd, &events[0], events.size(), timeout, &sigmask);
	flag_wait_on_kernel = false;

#ifndef NDEBUG
	rofl::logging::trace << "[rofl-common][cioloop][run] after epoll_wait,"
			<< " tid: 0x" << std::hex << tid << std::dec << std::endl << *this;
#endif

	if (rc < 0) {

		switch (errno) {
		case EINTR:
			break;
		default:
			rofl::logging::error << "[rofl-common][cioloop][run] " << eSysCall("epoll_wait()") << std::endl;
			break;
		}

	} else if ((0 == rc)/* || (EINTR == errno)*/) {

		if ((NULL != next_timeout.first) && has_ciosrv(next_timeout.first)) {
#ifndef NDEBUG
			rofl::logging::trace << "[rofl-common][cioloop][run] timeout event: "
					<< next_timeout.first << std::endl;
#endif
			next_timeout.first->__handle_timeout();
		}

		next_timeout = std::pair<ciosrv*, ctimespec>(NULL, ctimespec::now() + ctimespec(3600));

	} else { // rc > 0
		try {
			ciosrv* iosrv = NULL;

			for (int i = 0; i < rc; i++) {
				{
					RwLock lock(poll_rwlock, RwLock::RWLOCK_READ);

					if (poll_iosrvs.find(events[i].data.fd) == poll_iosrvs.end()) {

						// pipe
						if ((events[i].events & EPOLLIN) && (events[i].data.fd == pipe.pipefd[0])) {
#ifndef NDEBUG
							rofl::logging::trace << "[rofl-common][cioloop][run] wakeup signal received via pipe"
									<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
#endif
							flag_waking_up = false;
							pipe.recvmsg();
						}

						continue;
					}

					iosrv = poll_iosrvs[events[i].data.fd];
				}

				if ((events[i].events & EPOLLERR) && has_ciosrv(iosrv)) {
#ifndef NDEBUG
					rofl::logging::trace << "[rofl-common][cioloop][run]"
							<< " error event: " << events[i].data.fd << " on " << std::hex << iosrv << std::dec
							<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
#endif
					iosrv->handle_xevent(events[i].data.fd);
				}
				if ((events[i].events & EPOLLOUT) && has_ciosrv(iosrv)) {
#ifndef NDEBUG
					rofl::logging::trace << "[rofl-common][cioloop][run]"
							<< " write event: " << events[i].data.fd << " on " << std::hex << iosrv << std::dec
							<< " tid: 0x" << std::hex << tid << std::dec << std::endl;
#endif
					iosrv->handle_wevent(events[i].data.fd);
				}
				if ((events[i].events & EPOLLIN ) && has_ciosrv(iosrv)) {
#ifndef NDEBUG
					rofl::logging::trace << "[rofl-common][cioloop][run]"
							<< " read event: " << events[i].data.fd << " on " << std::hex << iosrv << std::dec
							<< " tid: 0x" << std::hex << tid << std::dec << std::endl;;
#endif
					iosrv->handle_revent(events[i].data.fd);
				}

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



