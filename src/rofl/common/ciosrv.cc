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


std::set<ciosrv*> 				ciosrv::ciolist;

PthreadRwLock 					cioloop::threads_rwlock;
std::map<pthread_t, cioloop*> 	cioloop::threads;


ciosrv::ciosrv() :
		tid(pthread_self())
{
	ciosrv::ciolist.insert(this);
	rfds.insert(pipe.pipefd[0]);
	register_filedesc_r(pipe.pipefd[0]);
}


ciosrv::~ciosrv()
{
	ciosrv::ciolist.erase(this);
	//logging::debug << "[rofl][ciosrv][destructor] -1-" << std::endl << *this;
	//logging::debug << "[rofl][ciosrv][destructor] -1-" << std::endl << cioloop::get_loop();

	timers.clear();
	events.clear();
	cioloop::get_loop().has_no_timer(this);
	deregister_filedesc_r(pipe.pipefd[0]);
	for (std::set<int>::iterator it = rfds.begin(); it != rfds.end(); ++it) {
		cioloop::get_loop().drop_readfd(this, (*it));
	}
	for (std::set<int>::iterator it = wfds.begin(); it != wfds.end(); ++it) {
		cioloop::get_loop().drop_writefd(this, (*it));
	}

	//logging::debug << "[rofl][ciosrv][destructor] -2-" << std::endl << *this;
	//logging::debug << "[rofl][ciosrv][destructor] -2-" << std::endl << cioloop::get_loop();
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
ciosrv::run()
{
	cioloop::get_loop().run();
}


void
ciosrv::stop()
{
	cioloop::get_loop().stop();
}


void
ciosrv::register_filedesc_r(int fd)
{
	rfds.insert(fd);
	cioloop::get_loop().add_readfd(this, fd);
}


void
ciosrv::deregister_filedesc_r(int fd)
{
	rfds.erase(fd);
	cioloop::get_loop().drop_readfd(this, fd);
}


void
ciosrv::register_filedesc_w(int fd)
{
	wfds.insert(fd);
	cioloop::get_loop().add_writefd(this, fd);
}


void
ciosrv::deregister_filedesc_w(int fd)
{
	wfds.erase(fd);
	cioloop::get_loop().drop_writefd(this, fd);
}


void
ciosrv::notify(cevent const& ev)
{
	events.add_event(ev);
	pipe.writemsg('1');
}


void
ciosrv::__handle_revent(int fd)
{
	try {
		if (pipe.pipefd[0] == fd) {
			//logging::debug << "[rofl][ciosrv][revent] entering event loop:" << std::endl << *this;
			if (ciosrv::ciolist.find(this) == ciosrv::ciolist.end()) {
				//logging::debug << "[rofl][ciosrv][revent] ciosrv instance deleted, returning from event loop" << std::endl;
				return;
			}
			pipe.recvmsg();

			cevents clone = events; events.clear();

			while (not clone.empty()) {
				//logging::debug << "[rofl][ciosrv][revent] inside event loop:" << std::endl << clone;
				if (ciosrv::ciolist.find(this) == ciosrv::ciolist.end()) {
					//logging::debug << "[rofl][ciosrv][revent] ciosrv instance deleted, returning from event loop" << std::endl;
					return;
				}
				handle_event(clone.get_event());
			}
			//logging::debug << "[rofl][ciosrv][revent] leaving event loop:" << std::endl << clone;
		} else {
			handle_revent(fd);
		}
	} catch (eEventsNotFound& e) {
		// do nothing
	}
}


void
ciosrv::__handle_timeout()
{
	try {
		ctimer timer = timers.get_expired_timer();
		//logging::debug << "[rofl][ciosrv][handle-timeout] timer: " << std::endl << timer;
		handle_timeout(timer.get_opaque());
	} catch (eTimersNotFound& e) {

	}
}


ctimer
ciosrv::get_next_timer()
{
	return timers.get_next_timer();
}


uint32_t
ciosrv::register_timer(int opaque, time_t t)
{
	if (timers.empty())
		cioloop::get_loop().has_timer(this);
	return timers.add_timer(ctimer(this, opaque, t));
}


uint32_t
ciosrv::reset_timer(uint32_t timer_id, time_t t)
{
	return timers.reset(timer_id, t);
}


bool
ciosrv::pending_timer(uint32_t timer_id)
{
	return timers.pending(timer_id);
}


void
ciosrv::cancel_timer(uint32_t timer_id)
{
	timers.cancel(timer_id);
	if (timers.empty())
		cioloop::get_loop().has_no_timer(this);
}


void
ciosrv::cancel_all_timer()
{
	timers.cancel_all();
	cioloop::get_loop().has_no_timer(this);
}





/* static */
void
cioloop::child_sig_handler (int x) {
	logging::debug <<  "[cioloop][child-sig-handler] got signal: " << x << std::endl;
    // signal(SIGCHLD, child_sig_handler);
}



void
cioloop::run_loop()
{
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

		//logging::error << "[rofl][ciosrv][loop] minrfd:" << minrfd << " maxrfd:" << maxrfd << " minwfd:" << minwfd << " maxwfd:" << maxwfd << std::endl << *this;

		FD_ZERO(&readfds);
		{
			RwLock lock(rfds_rwlock, RwLock::RWLOCK_READ);
			for (unsigned int i = minrfd; i < maxrfd; i++) {
				if (NULL != rfds[i]) {
					FD_SET(i, &readfds);
					maxfd = (i > maxfd) ? i : maxfd;
				}
			}
		}

		FD_ZERO(&writefds);
		{
			RwLock lock(wfds_rwlock, RwLock::RWLOCK_READ);
			for (unsigned int i = minwfd; i < maxwfd; i++) {
				if (NULL != wfds[i]) {
					FD_SET(i, &writefds);
					maxfd = (i > maxfd) ? i : maxfd;
				}
			}
		}

		FD_ZERO(&exceptfds);

		std::pair<ciosrv*, ctimer> next_timeout(0, ctimer(NULL, 0, 60));
		{
			std::map<ciosrv*, int> urgent;
			{
				RwLock lock(timers_rwlock, RwLock::RWLOCK_READ);
				for (std::map<ciosrv*, bool>::iterator it = timers.begin(); it != timers.end(); ++it) {
					try {
						ctimer timer((*it).first->get_next_timer());

						if ((*it).first->get_next_timer() < ctimer::now()) {
							//logging::debug << "[rofl][ciosrv][loop] timer:" << (*it).first->get_next_timer();
							//logging::debug << "[rofl][ciosrv][loop]   now:" << ctimer::now();
							//logging::debug << "[rofl][ciosrv][loop] delta:" << timer;
							//(*it).first->handle_timeout(timer.get_opaque());
							urgent[(*it).first] = timer.get_opaque();
							continue;
						}

						timer -= ctimer::now();

						if (timer < next_timeout.second) {
							next_timeout = std::pair<ciosrv*, ctimer>( (*it).first, timer );
						}
					} catch (eTimersNotFound& e) {}
				}
			}
			// conduct urgent timeouts (those with a timer expired before ctimer::now())
			for (std::map<ciosrv*, int>::iterator it = urgent.begin(); it != urgent.end(); ++it) {
				(*it).first->__handle_timeout();
			}
		}


		//logging::debug << "[rofl][cioloop] before select:" << std::endl << *this;

		//logging::debug << "[rofl][cioloop] next-timeout for select:" << std::endl << next_timeout.second;

		// blocking
		if ((rc = pselect(maxfd + 1, &readfds, &writefds, &exceptfds, &(next_timeout.second.get_ts()), &empty_mask)) < 0) {
			switch (errno) {
			case EINTR:
				break;
			default:
				rofl::logging::error << "[rofl][ciosrv] " << eSysCall("pselect()") << std::endl;
				break;
			}

		} else if ((0 == rc)/* || (EINTR == errno)*/) {

			//rofl::logging::debug << "[rofl][ciosrv][cioloop] timeout event" << std::endl;

			next_timeout.first->__handle_timeout();

		} else { // rc > 0

			try {

				for (unsigned int i = 0; i < rfds.size(); i++) {
					if (FD_ISSET(i, &exceptfds) && ((NULL != rfds[i]) || (NULL != wfds[i]))) {
						rfds[i]->handle_xevent(i);
					}
				}

				for (unsigned int i = minwfd; i < maxwfd; i++) {
					if (FD_ISSET(i, &writefds)  && (NULL != wfds[i])) {
						wfds[i]->handle_wevent(i);
					}
				}

				for (unsigned int i = minrfd; i < maxrfd; i++) {
					if (FD_ISSET(i, &readfds) 	 && (NULL != rfds[i])) {
						rfds[i]->__handle_revent(i);
					}
				}


			} catch (RoflException& e) {
				rofl::logging::error << "[rofl][ciosrv] caught RoflException in main loop:" << e << std::endl;
#ifndef NDEBUG
				throw;
#endif
			} catch (...) {
				rofl::logging::error << "[rofl][ciosrv] caught exception in main loop:" << std::endl;
#ifndef NDEBUG
				throw;
#endif
			}
		}

		// clean-up timers map
		if (true) {
			RwLock lock(timers_rwlock, RwLock::RWLOCK_WRITE);
restart:
			for (std::map<ciosrv*, bool>::iterator it = timers.begin(); it != timers.end(); ++it) {
				if ((*it).second == false) {
					timers.erase(it);
					goto restart;
				}
			}
		}

		//logging::debug << "[rofl][cioloop] after select:" << std::endl << *this;
	}
}





