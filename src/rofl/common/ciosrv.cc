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



PthreadRwLock 					cioloop::threads_rwlock;
std::map<pthread_t, cioloop*> 	cioloop::threads;


ciosrv::ciosrv() :
		tid(pthread_self())
{
	rfds.insert(pipe.pipefd[0]);
	register_filedesc_r(pipe.pipefd[0]);
}


ciosrv::~ciosrv()
{
	cioloop::get_loop().has_no_timer(this);
	deregister_filedesc_r(pipe.pipefd[0]);
	for (std::set<int>::iterator it = rfds.begin(); it != rfds.end(); ++it) {
		cioloop::get_loop().drop_readfd(this, (*it));
	}
	for (std::set<int>::iterator it = wfds.begin(); it != wfds.end(); ++it) {
		cioloop::get_loop().drop_writefd(this, (*it));
	}
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



	return *this;
}


void
ciosrv::run()
{
	cioloop::get_loop().run();
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
			pipe.recvmsg();
			while (not events.empty()) {
				handle_event(events.get_event());
			}
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

		FD_ZERO(&readfds);
		{
			RwLock lock(rfds_rwlock, RwLock::RWLOCK_READ);
			for (unsigned int i = 0; i < rfds.size(); i++) {
				if (NULL != rfds[i]) {
					FD_SET(i, &readfds);
					maxfd = (i > maxfd) ? i : maxfd;
				}
			}
		}

		FD_ZERO(&writefds);
		{
			RwLock lock(wfds_rwlock, RwLock::RWLOCK_READ);
			for (unsigned int i = 0; i < wfds.size(); i++) {
				if (NULL != wfds[i]) {
					FD_SET(i, &writefds);
					maxfd = (i > maxfd) ? i : maxfd;
				}
			}
		}

		FD_ZERO(&exceptfds);


		std::pair<ciosrv*, ctimer> next_timeout(0, ctimer(NULL, 0, 3600));
		{
			RwLock lock(timers_rwlock, RwLock::RWLOCK_READ);
			for (std::map<ciosrv*, bool>::iterator it = timers.begin(); it != timers.end(); ++it) {
				try {
					ctimer timer = (*it).first->get_next_timer();
					if (timer < next_timeout.second) {
						next_timeout = std::pair<ciosrv*, ctimer>( (*it).first, timer );
					}
				} catch (eTimersNotFound& e) {}
			}
		}


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

			next_timeout.first->__handle_timeout();

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

		} else { // rc > 0

			try {

				while (rc > 0) {

					if (FD_ISSET(rc, &exceptfds) && ((NULL != rfds[rc]) || (NULL != wfds[rc]))) {
						rfds[rc]->handle_xevent(rc);
					}

					if (FD_ISSET(rc, &writefds)  && (NULL != wfds[rc])) {
						rfds[rc]->handle_wevent(rc);
					}

					if (FD_ISSET(rc, &readfds) 	 && (NULL != rfds[rc])) {
						rfds[rc]->__handle_revent(rc);
					}

					--rc;
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
	}
}


/*static*/
void
cioloop::daemonize(
		std::string const& pidfile, std::string const& logfile)
{
	int pipefd[2];	// pipe for daemonizing

	try {
		/*
		 * close all file descriptors excluding stdin, stdout, stderr
		 */
		struct rlimit rlim;
		if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
			throw eSysCall("getrlimit()");
		}
		for (unsigned int i = 3; i < rlim.rlim_cur; i++) {
			close(i);
		}

		/*
		 * create a pipe for signaling successful initialization back from child-2 to parent process
		 */
		if (pipe2(pipefd, 0) < 0) {
			throw eSysCall("pipe2()");
		}

		/*
		 * reset signal handlers to default behaviour
		 */
		for (int i = 1; i < 32; i++) {
			if (i == SIGKILL)
				continue;
			if (i == SIGSTOP)
				continue;
			if (signal(i, SIG_DFL) == SIG_ERR) {
				std::cerr << "i:" << i << std::endl;
				throw eSysCall("signal()");
			}
		}

		/*
		 * reset sigprocmask
		 */
		sigset_t sigset;
		if (sigfillset(&sigset) < 0) {
			throw eSysCall("sigfillset()");
		}
		if (sigprocmask(SIG_UNBLOCK, &sigset, NULL) < 0) {
			throw eSysCall("sigprocmask()");
		}

		/*
		 * environment
		 */
		// we are not setting anything

		/*
		 * fork: create first child
		 */
		pid_t pid1 = fork();
		if (pid1 < 0) {
			throw eSysCall("fork()");
		}
		else if (pid1 > 0) { // parent exit
			uint8_t a;
			read(pipefd[0], &a, sizeof(a)); // blocks, until child-2 writes a byte to pipefd[1]
			switch (a) {
			case 0: // success
				//logging::error << "[rofl][unixenv] daemonizing successful" << std::endl;
				exit(0);
			case 1: // failure
				logging::error << "[rofl][unixenv] daemonizing failed" << std::endl;
				return;
			}
		}

		/*
		 *  call setsid() in the first child
		 */
		pid_t sid;
		if ((sid = setsid()) < 0) { // detach from controlling terminal
			throw eSysCall("setsid()");
		}

		/*
		 * fork: create second child
		 */
		pid_t pid2 = fork();
		if (pid2 < 0) {
			throw eSysCall("fork()");
		}
		else if (pid2 > 0) { // first child exit
			exit(0);
		}

		/*
		 * reset umask
		 */
		umask(0022);

		/*
		 * redirect STDIN, STDOUT, STDERR to logfile
		 */
		int fd = open(logfile.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
		if (fd < 0) {
			throw eSysCall("open()");
		}
		if (dup2(fd, STDIN_FILENO) < 0) {
			throw eSysCall("dup2(): STDIN");
		}
		if (dup2(fd, STDOUT_FILENO) < 0) {
			throw eSysCall("dup2(): STDOUT");
		}
		if (dup2(fd, STDERR_FILENO) < 0) {
			throw eSysCall("dup2(): STDERR");
		}

		/*
		 * change current working directory
		 */
		if (chdir("/") < 0) {
			throw eSysCall("chdir()");
		}

		/*
		 * write pidfile
		 */
		int pidfd = open(pidfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (pidfd < 0) {
			throw eSysCall("open(): pidfile");
		}
		char s_pidno[32];
		memset(s_pidno, 0, sizeof(s_pidno));
		snprintf(s_pidno, sizeof(s_pidno)-1, "%d", (int)getpid());
		if (write(pidfd, s_pidno, strnlen(s_pidno, sizeof(s_pidno))) < 0) {
			throw eSysCall("write(): pidfile");
		}
		close(pidfd);

		/*
		 * notify parent process about successful initialization
		 */
		uint8_t a = 0;
		if (write(pipefd[1], &a, sizeof(a)) < 0) {
			throw eSysCall("write(): pipe");
		}

	} catch (eSysCall& e) {
		uint8_t a = 1;
		if (write(pipefd[1], &a, sizeof(a)) < 0) {
			std::cerr << e << std::endl;
			throw eSysCall("write(): pipe");
		}
	}
}




