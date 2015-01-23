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
	rofl::cioloop::get_loop(get_thread_id()).register_ciosrv(this);
}


ciosrv::~ciosrv()
{
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
			rofl::logging::trace << "[rofl-common][ciosrv][event] ciosrv instance deleted, returning from event loop" << std::endl;
			return;
		}
		rofl::logging::trace << "[rofl-common][ciosrv][event] entering event loop:" << std::endl << *this;

		cevents clone = events; events.clear();

		while (not clone.empty()) {
			rofl::logging::trace << "[rofl-common][ciosrv][event] inside event loop:" << std::endl << clone;
			if (not rofl::cioloop::get_loop(get_thread_id()).has_ciosrv(this)) {
				rofl::logging::trace << "[rofl-common][ciosrv][event] ciosrv instance deleted, returning from event loop" << std::endl;
				return;
			}
			handle_event(clone.get_event());
		}
		rofl::logging::trace << "[rofl-common][ciosrv][event] leaving event loop:" << std::endl << clone;

	} catch (eEventsNotFound& e) {
		// do nothing
	}
}




/* static */
void
cioloop::child_sig_handler (int x) {
	logging::debug <<  "[rofl-common][cioloop][child-sig-handler] got signal: " << x << std::endl;
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
	if (tid != pthread_self()) {
		return;
	}

	if (keep_on_running) {
		return;
	}

	keep_on_running = true;

	/*
	 * signal masks, etc.
	 */
	sigset_t sigmask, empty_mask;
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
	sigemptyset(&empty_mask);


	/*
	 * the infinite loop ...
	 */

	while (keep_on_running) {
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
					FD_SET(i, &writefds);
					FD_SET(i, &exceptfds);
					maxfd = (i > maxfd) ? i : maxfd;
				}
			}
		}

		rofl::logging::trace << "[rofl-common][cioloop][run] urgent timers and events:" << std::endl << *this;

		std::pair<ciosrv*, ctimespec> next_timeout(NULL, ctimespec(60));
		while (true) {
			std::map<ciosrv*, int> urgent;
			{
				RwLock lock(timers_rwlock, RwLock::RWLOCK_READ);
				for (std::map<ciosrv*, bool>::iterator
						it = timers.begin(); it != timers.end(); ++it) {
					if (not has_ciosrv(it->first))
						continue;
#ifndef NDEBUG
					try {
						rofl::logging::trace << "[rofl-common][ciosrv][loop] registered timer:" << std::endl << (*it).first->get_next_timer();
					} catch (eTimersNotFound& e) {}
#endif
					try {
						ctimer timer((*it).first->get_next_timer());

						if ((*it).first->get_next_timer().get_timespec() < ctimespec::now()) {
#ifndef NDEBUG
							rofl::logging::trace << "[rofl-common][ciosrv][loop] timer:" << std::endl << (*it).first->get_next_timer();
							rofl::logging::trace << "[rofl-common][ciosrv][loop]   now:" << std::endl << ctimer::now();
							rofl::logging::trace << "[rofl-common][ciosrv][loop] delta:" << std::endl << timer;
#endif
							//(*it).first->handle_timeout(timer.get_opaque());
							urgent[(*it).first] = timer.get_opaque();
							continue;
						}

						timer.set_timespec() -= ctimespec::now();

						if (timer.get_timespec() < next_timeout.second) {
							next_timeout = std::pair<ciosrv*, ctimespec>( (*it).first, timer.get_timespec() );
						}
					} catch (eTimersNotFound& e) {}
				}
			}
			// conduct urgent timeouts (those with a timer expired before ctimer::now())
			for (std::map<ciosrv*, int>::iterator
					it = urgent.begin(); it != urgent.end(); ++it) {
				if (not keep_on_running || not has_ciosrv(it->first))
					continue;
				(*it).first->__handle_timeout();
			}

			if (urgent.empty()) {
				break;
			}
		}

		if (not keep_on_running)
			return;

		rofl::logging::trace << "[rofl-common][cioloop][run] before select:" << std::endl << *this;

		rofl::logging::trace << "[rofl-common][cioloop][run] next-timeout for select:" << std::endl << next_timeout.second;

		rofl::indent::null();

		// blocking
		wait_on_kernel = true;
		rc = pselect(maxfd + 1, &readfds, &writefds, &exceptfds, &(next_timeout.second.get_timespec()), &empty_mask);
		wait_on_kernel = false;

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
						rofl::logging::trace << "[rofl-common][cioloop][run] checking for wfds: "
								<< std::hex << wfds[i] << std::dec << std::endl;
						wfds[i]->handle_wevent(i);
					}
				}

				// read events
				for (unsigned int i = minrfd; i < maxrfd; i++) {
					if (FD_ISSET(i, &readfds) && (NULL != rfds[i]) && has_ciosrv(rfds[i])) {
						rofl::logging::trace << "[rofl-common][cioloop][run] checking for rfds: "
								<< std::hex << rfds[i] << std::dec << std::endl;
						rfds[i]->__handle_revent(i);
					}
				}

				// pipe
				if (FD_ISSET(pipe.pipefd[0], &readfds)) {
					rofl::logging::trace << "[rofl-common][cioloop][run] entering event loop:"
							<< std::endl << *this;
					pipe.recvmsg();

					std::map<ciosrv*, bool> clone;
					{
						RwLock lock(events_rwlock, RwLock::RWLOCK_READ);
						for (std::map<ciosrv*, bool>::iterator
								it = events.begin(); it != events.end(); ++it) {
							clone[it->first] = it->second;
						}
					}
					for (std::map<ciosrv*, bool>::iterator
							it = clone.begin(); it != clone.end(); ++it) {
						cioloop::get_loop().has_no_event(it->first);
						if (not has_ciosrv(it->first))
							continue;
						it->first->__handle_event();
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
				keep_on_running = false;
				throw;
			}
		}

		// clean-up timers map
		if (true) {
			RwLock lock(timers_rwlock, RwLock::RWLOCK_WRITE);
restartT:
			for (std::map<ciosrv*, bool>::iterator
					it = timers.begin(); it != timers.end(); ++it) {
				if ((*it).second == false) {
					timers.erase(it);
					goto restartT;
				}
			}
		}

		// clean-up events map
		if (true) {
			RwLock lock(events_rwlock, RwLock::RWLOCK_WRITE);
restartE:
			for (std::map<ciosrv*, bool>::iterator
					it = events.begin(); it != events.end(); ++it) {
				if ((*it).second == false) {
					events.erase(it);
					goto restartE;
				}
			}
		}

		logging::trace << "[rofl-common][cioloop][run] after select:" << std::endl << *this;
	}

	keep_on_running = false;

	logging::debug << "[rofl-common][cioloop][run] terminating" << std::endl << *this;
}





