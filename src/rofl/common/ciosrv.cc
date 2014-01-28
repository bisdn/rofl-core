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
ciosrv::register_timer(int opaque, time_t t)
{
	if (timers.empty())
		cioloop::get_loop().has_timer(this);
	timers.add_timer(ctimer(opaque, t));
}


void
ciosrv::reset_timer(int opaque, time_t t)
{
	timers.reset(opaque);
}


bool
ciosrv::pending_timer(int opaque)
{
	return timers.has(opaque);
}


void
ciosrv::cancel_timer(int opaque)
{
	timers.cancel(opaque);
}


void
ciosrv::cancel_all_timer()
{
	timers.reset();
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
		struct timespec ts = { 0, 0 };
		int maxfd = 0;
		int rc = 0;
		time_t ntimeout = time(NULL) + 60 /* seconds */; // one wakeup every 60seconds

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


		// TODO: cioloop::next_timeout(ntimeout); // get next timeout

		//for (std)

		ts.tv_sec = ((ntimeout - time(NULL)) > 0) ? ntimeout - time(NULL) : 0;

		// call select
		if ((rc = pselect(maxfd + 1, &readfds, &writefds, &exceptfds, &ts, &empty_mask)) < 0) {
			switch (errno) {
			case EINTR:
				break;
			default:
				rofl::logging::error << "[rofl][ciosrv] " << eSysCall("pselect()") << std::endl;
				break;
			}

		} else if ((0 == rc)/* || (EINTR == errno)*/) {

			// TODO: handle timeouts

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































void
ciosrv::register_timer(int opaque, time_t t)
{
	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::register_timer(0x%x, %d)", this, opaque, t);
	{
		//Lock lock(&timer_mutex);

		time_t timeout = time(NULL) + t;

		this->timers_list[timeout].push_back(opaque); // local timers list with timer-type
	}

	//WRITELOG(CIOSRV, DBG, "ciosrv(%p)::register_timer() %s", this, this->c_str());
}


void
ciosrv::reset_timer(int opaque, time_t t)
{
	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::reset_timer(0x%x, %d)", this, opaque, t);
#if 0
	{
		//Lock lock(&timer_mutex);

		// remove all instances of type opaque and set single new one
		for (std::map<time_t, std::set<int> >::iterator
				it = timers_list.begin(); it != timers_list.end(); ++it)
		{
			std::set<int>::iterator jt;
			if ((jt = it->second.find(opaque)) != it->second.end())
			{
				it->second.erase(jt);
			}
		}
	}
#endif

	cancel_timer(opaque);

	register_timer(opaque, t);

	//WRITELOG(CIOSRV, DBG, "ciosrv(%p)::reset_timer() %s", this, this->c_str());
}


bool
ciosrv::pending_timer(int opaque)
{
	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::pending_timer() type:0x%x", this, opaque);
	for (std::map<time_t, std::list<int> >::iterator
			it = timers_list.begin(); it != timers_list.end(); ++it)
	{
		for (std::list<int>::iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt)
		{
			if ((*jt) == opaque)
			{
				return true;
			}
		}
	}
	return false;
}


void
ciosrv::cancel_timer(int opaque)
{
	//Lock lock(&timer_mutex);

	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::cancel_timer(0x%x)", this, opaque);

	for (std::map<time_t, std::list<int> >::iterator
			it = timers_list.begin(); it != timers_list.end(); ++it)
	{
		for (std::list<int>::iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt)
		{
			if ((*jt) == opaque)
			{
				it->second.erase(jt); return;
			}
		}
	}
}


void
ciosrv::cancel_all_timer()
{
	//Lock lock(&timer_mutex);

	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::cancel_all_timer()", this);
	for (std::map<time_t, std::list<int> >::iterator
			it = timers_list.begin(); it != timers_list.end(); ++it)
	{
		it->second.clear();
	}
	timers_list.clear();
}





/*static*/
void
ciosrv::next_timeout(
		time_t &ntimeout)
{
	pthread_t tid = pthread_self();

	//Lock lock(&ciosrv::ciosrv_list_mutex[tid]);

	RwLock lock(&ciosrv::iothread_lock, RwLock::RWLOCK_READ);

	threads[tid]->ciosrv_timeouts.clear();

	for (std::set<ciosrv*>::iterator
			it = ciosrv::threads[tid]->ciosrv_list.begin(); it != ciosrv::threads[tid]->ciosrv_list.end(); ++it)
	{
#if 0
		WRITELOG(CIOSRV, DBG, "ciosrv::next_timeout() ciosrv:%p %s",
				*it,
				(*it)->c_str());
#endif
		if ((*it)->timers_list.empty())
		{
			continue;
		}

		time_t timeout = (*it)->timers_list.begin()->first;
#if 0
		WRITELOG(CIOSRV, DBG, "ciosrv::next_timeout() ciosrv:%p timeout:%ld ntimeout:%ld",
				*it,
				timeout - time(NULL),
				ntimeout - time(NULL));
#endif
		if (timeout < ntimeout)
		{
			ntimeout = timeout;
			threads[tid]->ciosrv_timeouts.clear();
			threads[tid]->ciosrv_timeouts.push_back(*it);

#if 0
			for (std::list<ciosrv*>::iterator
					it = threads[tid]->ciosrv_timeouts.begin(); it != threads[tid]->ciosrv_timeouts.end(); ++it)
			{
				WRITELOG(CIOSRV, DBG, "ciosrv::next_timeout() [1] ciosrv-timeout-list: %p", *it);
			}
#endif
		}
		else if (timeout == ntimeout)
		{
			threads[tid]->ciosrv_timeouts.push_back(*it);
#if 0
			for (std::list<ciosrv*>::iterator
					it = threads[tid]->ciosrv_timeouts.begin(); it != threads[tid]->ciosrv_timeouts.end(); ++it)
			{
				WRITELOG(CIOSRV, DBG, "ciosrv::next_timeout() [2] ciosrv-timeout-list: %p", *it);
			}
#endif
		}
	}

#if 0
	std::string s_ciosrv_timeouts;
	for (std::list<ciosrv*>::iterator
			it = threads[tid]->ciosrv_timeouts.begin(); it != threads[tid]->ciosrv_timeouts.end(); ++it)
	{
		cvastring vas(32);
		s_ciosrv_timeouts.append(vas("%p ", *it));
	}
	WRITELOG(CIOSRV, DBG, "ciosrv::next_timeout() [%d] ciosrv-timeout-list: %s",
			threads[tid]->ciosrv_timeouts.size(),
			s_ciosrv_timeouts.c_str());
#endif

	//WRITELOG(CIOSRV, DBG, "ciosrv::next_timeout() ntimeout:%ld", ntimeout - time(NULL));
}









/*static*/
void
ciosrv::handle_timeouts()
{
	pthread_t tid = pthread_self();

	/*
	 * timeout events
	 */

	std::list<ciosrv*> p_timeouts;
	{
		//Lock lock(&ciosrv::ciosrv_list_mutex[tid]);
		for (std::list<ciosrv*>::iterator
				it = threads[tid]->ciosrv_timeouts.begin();
						it != threads[tid]->ciosrv_timeouts.end(); ++it)
		{
			Lock lock(&(threads[tid]->ciosrv_list_mutex));
			if (ciosrv::threads[tid]->ciosrv_deletion_list.find(*it) != ciosrv::threads[tid]->ciosrv_deletion_list.end())
			{
				WRITELOG(CIOSRV, DBG, "ciosrv::handle_timeouts(): skipping already deleted object %p "
								"(%d deleted in this round)", *it,
								ciosrv::threads[tid]->ciosrv_deletion_list.size());

				continue; // do nothing for this object, as it was already deleted
			}

			p_timeouts.push_back(*it);
		}
		threads[tid]->ciosrv_timeouts.clear();
	}

	/*
	 * enter the time consuming handling of timeout events
	 */

	for (std::list<ciosrv*>::iterator
			it = p_timeouts.begin(); it != p_timeouts.end(); ++it)
	{
		{
			Lock lock(&(threads[tid]->ciosrv_list_mutex));
			if (ciosrv::threads[tid]->ciosrv_deletion_list.find(*it) != ciosrv::threads[tid]->ciosrv_deletion_list.end())
			{
				WRITELOG(CIOSRV, DBG, "ciosrv::handle_timeouts(): skipping already deleted object %p "
								"(%d deleted in this round)", *it,
								ciosrv::threads[tid]->ciosrv_deletion_list.size());

				continue; // do nothing for this object, as it was already deleted
			}
			if (ciosrv::threads[tid]->ciosrv_list.find(*it) == ciosrv::threads[tid]->ciosrv_list.end())
			{
				continue;
			}
		}
		(*it)->__handle_timeout();
	}
}


void
ciosrv::__handle_timeout()
{
	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::__handle_timeout() [1] TIMERS: %s", this, c_str());

	std::list<int> expired_timers;
	{
		//Lock lock(&timer_mutex);
		while (not timers_list.empty())
		{
			std::map<time_t, std::list<int> >::iterator jt = timers_list.begin();

			if (jt->first/*timeout*/ <= time(NULL))
			{
				for (std::list<int>::iterator
						kt = jt->second.begin(); kt != jt->second.end(); ++kt)
				{
					expired_timers.push_back(*kt/*opaque*/);
				}

				timers_list.erase(jt);
			}
			else // timer has not yet expired
			{
				break;
			}
		}
	}

	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::__handle_timeout() [2] TIMERS: %s", this, c_str());

	for (std::list<int>::iterator
			it = expired_timers.begin(); it != expired_timers.end(); ++it)
	{
		WRITELOG(CIOSRV, DBG, "ciosrv(%p)::__handle_timeout() calling timer => type:0x%x",
					this, *it);
		{
			Lock lock(&(threads[tid]->ciosrv_list_mutex));
			if (ciosrv::threads[tid]->ciosrv_deletion_list.find(this) != ciosrv::threads[tid]->ciosrv_deletion_list.end())
			{
				return;
			}
			if (ciosrv::threads[tid]->ciosrv_list.find(this) == ciosrv::threads[tid]->ciosrv_list.end())
			{
				return;
			}
		}
		handle_timeout(*it);
#if 0
		{
			Lock lock(&(threads[tid]->ciosrv_list_mutex));
			if (ciosrv::threads[tid]->ciosrv_deletion_list.find(this) != ciosrv::threads[tid]->ciosrv_deletion_list.end())
			{
				return;
			}
			if (ciosrv::threads[tid]->ciosrv_list.find(this) == ciosrv::threads[tid]->ciosrv_list.end())
			{
				return;
			}
		}
#endif
	}
}

