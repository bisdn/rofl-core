/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/ciosrv.h"

using namespace rofl;

#if 0
bool cont = true;

void sighandler(int sig)
{
	std::cerr << "signal caught " << sig << std::endl;
	switch (sig) {
	case SIGINT:
	case SIGTERM:
		cont = false;
		break;
	}
}
#endif

PthreadRwLock 					ciosrv::ciolist_rwlock;
std::set<ciosrv*> 				ciosrv::ciolist;

PthreadRwLock 					cioloop::threads_rwlock;
std::map<pthread_t, cioloop*> 	cioloop::threads;


ciosrv::ciosrv(pthread_t tid) :
		tid(tid)
{
	if (0 == tid) {
		this->tid = pthread_self();
	}
	RwLock lock(ciosrv::ciolist_rwlock, RwLock::RWLOCK_WRITE);
	ciosrv::ciolist.insert(this);
	if (tid != pthread_self()) {
		notify(cevent());
	}
	//rofl::logging::debug << "[rofl-common][ciosrv] constructor " << std::hex << this << std::dec << std::endl;
}


ciosrv::~ciosrv()
{
	//rofl::logging::debug << "[rofl-common][ciosrv] destructor " << std::hex << this << std::dec << std::endl;
	{
		RwLock lock(ciosrv::ciolist_rwlock, RwLock::RWLOCK_WRITE);
		ciosrv::ciolist.erase(this);
	}
	//logging::debug << "[rofl-common][ciosrv][destructor] -1-" << std::endl << *this;
	//logging::debug << "[rofl-common][ciosrv][destructor] -1-" << std::endl << cioloop::get_loop();

	timers.clear();
	events.clear();
	cioloop::get_loop().has_no_timer(this);
	cioloop::get_loop().has_no_event(this);
	for (std::set<int>::iterator it = rfds.begin(); it != rfds.end(); ++it) {
		cioloop::get_loop().drop_readfd(this, (*it));
	}
	for (std::set<int>::iterator it = wfds.begin(); it != wfds.end(); ++it) {
		cioloop::get_loop().drop_writefd(this, (*it));
	}

	//logging::debug << "[rofl-common][ciosrv][destructor] -2-" << std::endl << *this;
	//logging::debug << "[rofl-common][ciosrv][destructor] -2-" << std::endl << cioloop::get_loop();
}


ciosrv::ciosrv(ciosrv const& iosrv)
{
	*this = iosrv;
}


ciosrv&
ciosrv::operator= (ciosrv const& iosrv)
{
	if (this == &iosrv)
		return *this;

	throw eNotImplemented();

	return *this;
}



void
ciosrv::__handle_event()
{
	try {

		{
			RwLock lock(ciosrv::ciolist_rwlock, RwLock::RWLOCK_READ);
			if (ciosrv::ciolist.find(this) == ciosrv::ciolist.end()) {
				logging::trace << "[rofl-common][ciosrv][event] ciosrv instance deleted, returning from event loop" << std::endl;
				return;
			}
		}
		logging::trace << "[rofl-common][ciosrv][event] entering event loop:" << std::endl << *this;

		cevents clone = events; events.clear();

		while (not clone.empty()) {
			logging::trace << "[rofl-common][ciosrv][event] inside event loop:" << std::endl << clone;
			{
				RwLock lock(ciosrv::ciolist_rwlock, RwLock::RWLOCK_READ);
				if (ciosrv::ciolist.find(this) == ciosrv::ciolist.end()) {
					logging::trace << "[rofl-common][ciosrv][event] ciosrv instance deleted, returning from event loop" << std::endl;
					return;
				}
			}
			handle_event(clone.get_event());
		}
		logging::trace << "[rofl-common][ciosrv][event] leaving event loop:" << std::endl << clone;

	} catch (eEventsNotFound& e) {
		// do nothing
	}
}




/* static */
void
cioloop::child_sig_handler (int x) {
	logging::debug <<  "[rofl-common][cioloop][child-sig-handler] got signal: " << x << std::endl;
    // signal(SIGCHLD, child_sig_handler);
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

#if 0
	signal(SIGINT, &sighandler);
	signal(SIGTERM, &sighandler);
#endif
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

		//logging::error << "[rofl-common][ciosrv][loop] minrfd:" << minrfd << " maxrfd:" << maxrfd << " minwfd:" << minwfd << " maxwfd:" << maxwfd << std::endl << *this;

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
				for (std::map<ciosrv*, bool>::iterator it = timers.begin(); it != timers.end(); ++it) {
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
			for (std::map<ciosrv*, int>::iterator it = urgent.begin(); it != urgent.end(); ++it) {
				{
					RwLock lock(ciosrv::ciolist_rwlock, RwLock::RWLOCK_READ);
					if (ciosrv::ciolist.find(it->first) == ciosrv::ciolist.end()) {
						continue;
					}
				}
				(*it).first->__handle_timeout();

			}

			if (urgent.empty()) {
				break;
			}
		}


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

			if ((NULL != next_timeout.first)) {
				{
					RwLock lock(ciosrv::ciolist_rwlock, RwLock::RWLOCK_READ);
					if (ciosrv::ciolist.find(next_timeout.first) == ciosrv::ciolist.end()) {
						continue;
					}
				}
				rofl::logging::trace << "[rofl-common][cioloop][run] timeout event: " << next_timeout.first << std::endl;
				next_timeout.first->__handle_timeout();
			}

		} else { // rc > 0
			try {

				for (unsigned int i = std::min(minrfd, minwfd); i < maxfd; i++) {
					if (FD_ISSET(i, &exceptfds)) {
						if(NULL != rfds[i]) {
							rofl::logging::trace << "[rofl-common][cioloop][run] checking for exceptfds: " << std::hex << rfds[i] << std::dec << std::endl;
							{
								RwLock lock(ciosrv::ciolist_rwlock, RwLock::RWLOCK_READ);
								if (ciosrv::ciolist.find(rfds[i]) == ciosrv::ciolist.end()) {
									continue;
								}
							}
							rfds[i]->handle_xevent(i);
						} else if (NULL != wfds[i]) {
							rofl::logging::trace << "[rofl-common][cioloop][run] checking for exceptfds: " << std::hex << rfds[i] << std::dec << std::endl;
							{
								RwLock lock(ciosrv::ciolist_rwlock, RwLock::RWLOCK_READ);
								if (ciosrv::ciolist.find(wfds[i]) == ciosrv::ciolist.end()) {
									continue;
								}
							}
							wfds[i]->handle_xevent(i);
						}

					}
				}

				for (unsigned int i = minwfd; i < maxwfd; i++) {
					if (FD_ISSET(i, &writefds)  && (NULL != wfds[i])) {
						rofl::logging::trace << "[rofl-common][cioloop][run] checking for wfds: " << std::hex << wfds[i] << std::dec << std::endl;
						{
							RwLock lock(ciosrv::ciolist_rwlock, RwLock::RWLOCK_READ);
							if (ciosrv::ciolist.find(wfds[i]) == ciosrv::ciolist.end()) {
								continue;
							}
						}
						rofl::logging::trace << "[rofl-common][cioloop][run] checking for readfds before handle_wevent" << std::endl << *this;
						wfds[i]->handle_wevent(i);
						rofl::logging::trace << "[rofl-common][cioloop][run] checking for readfds after handle_wevent" << std::endl << *this;
					}
				}

				for (unsigned int i = minrfd; i < maxrfd; i++) {
					if (FD_ISSET(i, &readfds) 	 && (NULL != rfds[i])) {
						rofl::logging::trace << "[rofl-common][cioloop][run] checking for rfds: " << std::hex << rfds[i] << std::dec << std::endl;
						{
							RwLock lock(ciosrv::ciolist_rwlock, RwLock::RWLOCK_READ);
							if (ciosrv::ciolist.find(rfds[i]) == ciosrv::ciolist.end()) {
								continue;
							}
						}
						rofl::logging::trace << "[rofl-common][cioloop][run] checking for readfds before handle_revent" << std::endl << *this;
						rfds[i]->__handle_revent(i);
						rofl::logging::trace << "[rofl-common][cioloop][run] checking for readfds after handle_revent" << std::endl << *this;
					}
				}

				if (FD_ISSET(pipe.pipefd[0], &readfds)) {
					logging::trace << "[rofl-common][cioloop][run] entering event loop:" << std::endl << *this;
					pipe.recvmsg();

					std::map<ciosrv*, bool> clone;
					{
						RwLock lock(events_rwlock, RwLock::RWLOCK_READ);
						for (std::map<ciosrv*, bool>::iterator it = events.begin(); it != events.end(); ++it) {
							clone[it->first] = it->second;
						}
					}
					for (std::map<ciosrv*, bool>::iterator it = clone.begin(); it != clone.end(); ++it) {
						cioloop::get_loop().has_no_event(it->first);
						it->first->__handle_event();
					}
				}

			} catch (rofl::RoflException& e) {
				rofl::logging::error << "[rofl-common][cioloop][run] caught RoflException in main loop: " << e.what() << std::endl;
				rofl::indent::null();

			} catch (std::exception& e) {
				rofl::logging::error << "[rofl-common][cioloop][run] caught std::exception in main loop: " << e.what() << std::endl;
				rofl::indent::null();
				keep_on_running = false;
				throw;
			}
		}

		// clean-up timers map
		if (true) {
			RwLock lock(timers_rwlock, RwLock::RWLOCK_WRITE);
restartT:
			for (std::map<ciosrv*, bool>::iterator it = timers.begin(); it != timers.end(); ++it) {
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
			for (std::map<ciosrv*, bool>::iterator it = events.begin(); it != events.end(); ++it) {
				if ((*it).second == false) {
					events.erase(it);
					goto restartE;
				}
			}
		}

		logging::trace << "[rofl-common][cioloop][run] after select:" << std::endl << *this;
	}

	keep_on_running = false;
}





