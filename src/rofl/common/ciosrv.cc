/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ciosrv.h"

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

pthread_rwlock_t ciosrv::iothread_lock;
std::map<pthread_t, ciosrv::ciothread*> ciosrv::threads;
bool ciosrv::keep_on = true;


ciosrv::ciosrv() :
		tid(pthread_self()),
		tv_mem(sizeof(struct timeval))
{
	if (threads.find(tid) == threads.end()) {
		ciosrv::init(); // initialize ciothread structure for this thread if necessary
	}

	threads[tid]->ciosrv_elements.insert(this);

	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::ciosrv()", this);

	pthread_mutex_init(&event_mutex, NULL);
	pthread_mutex_init(&timer_mutex, NULL);

	tv = (struct timeval*)tv_mem.somem();
	tv->tv_sec 	= 3600;
	tv->tv_usec = 0;

	{
		//Lock lock(&ciosrv::ciosrv_list_mutex[tid]);
		ciosrv::threads[tid]->ciosrv_insertion_list.insert(this);
	}
}


ciosrv::~ciosrv()
{
	//cancel_all_timer(); // not really necessary

	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::~ciosrv()", this);

	// magic: we add our this pointer to ciosrv_deletion_list, so that ciosrv::handle can check for our removal
	{
		//Lock lock(&ciosrv::ciosrv_list_mutex[tid]);
		Lock tlock(&(threads[tid]->ciosrv_list_mutex));
		ciosrv::threads[tid]->ciosrv_deletion_list.insert(this);
		//ciosrv_list[tid].erase(this);

		RwLock lock(&ciosrv::iothread_lock, RwLock::RWLOCK_READ);

restart1:
		for (std::map<int, ciosrv*>::iterator
				it = threads[tid]->rfds.begin(); it != threads[tid]->rfds.end(); ++it)
		{
			if (it->second == this)
			{
				threads[tid]->rfds.erase(it); // invalidates iodata[tid]->rfds iterator
				goto restart1;
			}
		}

restart2:
		for (std::map<int, ciosrv*>::iterator
				it = threads[tid]->wfds.begin(); it != threads[tid]->wfds.end(); ++it)
		{
			if (it->second == this)
			{
				threads[tid]->wfds.erase(it);  // invalidates iodata[tid]->rfds iterator
				goto restart2;
			}
		}

restart3:
		for (std::list<ciosrv*>::iterator
				it = threads[tid]->ciosrv_timeouts.begin();
						it != threads[tid]->ciosrv_timeouts.end(); ++it)
		{
			if ((*it) == this)
			{
				threads[tid]->ciosrv_timeouts.erase(it);
				goto restart3;
			}
		}
	}

	pthread_mutex_destroy(&timer_mutex);
	pthread_mutex_destroy(&event_mutex);

	threads[tid]->ciosrv_elements.erase(this);

	if (threads[tid]->ciosrv_elements.empty()) {
		//fprintf(stderr, "ciosrv: NEEDS DESTRUCTION, tid: 0x%x\n", (int)tid);
		ciosrv::destroy();
	} else {
		//fprintf(stderr, "ciosrv: ALREADY DESTROYED, tid: 0x%x\n", (int)tid);
	}

	tid = 0;
}



ciosrv::ciosrv(ciosrv const& iosrv) :
		tid(pthread_self()),
		tv_mem(sizeof(struct timeval))

{
	if (threads.find(tid) == threads.end()) {
		ciosrv::init(); // initialize ciothread structure for this thread if necessary
	}

	threads[tid]->ciosrv_elements.insert(this);

	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::ciosrv()", this);

	pthread_mutex_init(&event_mutex, NULL);
	pthread_mutex_init(&timer_mutex, NULL);

	tv = (struct timeval*)tv_mem.somem();
	tv->tv_sec 	= 3600;
	tv->tv_usec = 0;

	{
		//Lock lock(&ciosrv::ciosrv_list_mutex[tid]);
		ciosrv::threads[tid]->ciosrv_insertion_list.insert(this);
	}
}



const char*
ciosrv::c_str()
{
	cvastring vas(256);

	RwLock lock(&ciosrv::iothread_lock, RwLock::RWLOCK_READ);

	info.assign(vas("ciosrv(%p) rfds: ", this));
	for (std::map<int, ciosrv*>::iterator
			it = threads[tid]->rfds.begin(); it != threads[tid]->rfds.end(); ++it)
	{
		if (it->second != this)
		{
			continue;
		}
		info.append(vas("%d ", it->first));
	}

	info.append(" wfds: ");
	for (std::map<int, ciosrv*>::iterator
			it = threads[tid]->wfds.begin(); it != threads[tid]->wfds.end(); ++it)
	{
		if (it->second != this)
		{
			continue;
		}
		info.append(vas("%d ", it->first));
	}

	info.append(" timers: ");
	//Lock lock_t(&timer_mutex);
	for (std::map<time_t, std::list<int> >::iterator
			it = timers_list.begin(); it != timers_list.end(); ++it)
	{
		for (std::list<int>::iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt)
		{
			info.append(vas("[time:%ld => type:0x%x] ",
					it->first - time(NULL),
					(*jt)));
		}
	}

	info.append(" events: ");
	Lock lock_e(&event_mutex);
	for (std::list<cevent*>::iterator it = events.begin(); it != events.end(); ++it)
	{
		info.append(vas("[event:0x%x => %s]", *it, (*it)->c_str()));
	}

	return info.c_str();
}


void
ciosrv::notify(cevent const& ev)
{
	try {
		WRITELOG(CIOSRV, DBG, "ciosrv(%p)::notify() from thread: 0x%x to thread: 0x%x cmd:0x%x",
				this, pthread_self(), tid, ev.cmd);

		{
			WRITELOG(CIOSRV, DBG, "ciosrv(%p)::notify() locking local event mutex",
					this);

			Lock lock(&event_mutex); // lock this->events
#if 0
			std::list<cevent*>::iterator it;
			if ((it = find_if(events.begin(), events.end(),
					cevent::cevent_find_by_cmd(ev.cmd))) == events.end())
			{
				// store event on this event list
				events.push_back(new cevent(ev));
			}
			else
			{
				WRITELOG(CIOSRV, DBG, "ciosrv(%p)::notify() from thread: 0x%x to "
						"thread: 0x%x - event already scheduled",
						this, pthread_self(), tid);
			}
#endif
			events.push_back(new cevent(ev));

			WRITELOG(CIOSRV, DBG, "ciosrv(%p)::notify() unlocking local event mutex",
					this);
#if 0
			std::string s_events;
			for (std::list<cevent*>::iterator
					it = new_events.begin(); it != new_events.end(); ++it)
			{
				cvastring vas(256);
				s_events.append(vas("%s ", (*it)->c_str()));
			}

			WRITELOG(CIOSRV, DBG, "ciosrv(%p)::notify() ACTUAL-EVENT-LIST: %s",
					this, s_events.c_str());
#endif
		}

		/*
		 * wakeup thread via pipe
		 */

		{
			RwLock lock(&(ciosrv::threads[tid]->wakeup_rwlock), RwLock::RWLOCK_WRITE); // for locking ciosrv_wakeup[tid]

			// insert this to list of entities with pending events
			ciosrv::threads[tid]->ciosrv_wakeup.insert(this);

#if 0
			std::string s_wakeup;
			for (std::set<ciosrv*>::iterator
					it = ciosrv::ciosrv_wakeup[tid].begin();
						it != ciosrv::ciosrv_wakeup[tid].end(); ++it)
			{
				cvastring vas(64);
				s_wakeup.append(vas("0x%x ", *it));
			}
			WRITELOG(CIOSRV, DBG, "ciosrv(%p)::notify() UPDATED-WAKEUP-LIST %s",
					this, s_wakeup.c_str());
#endif

			RwLock lock2(&ciosrv::iothread_lock, RwLock::RWLOCK_READ);

			if (threads[tid]->flags.test(CIOSRV_FLAG_WAKEUP_CALLED))
			{
				WRITELOG(CIOSRV, DBG, "ciosrv(%p)::notify() wakeup call is pending", this);
				return;
			}

			WRITELOG(CIOSRV, DBG, "ciosrv(%p)::notify() send new wake-up call", this);
			threads[tid]->flags.set(CIOSRV_FLAG_WAKEUP_CALLED);
		}

		// wake up this thread from select()
		ciosrv::threads[tid]->pipe->writemsg();


	} catch (eLockWouldBlock& e) {
		WRITELOG(CIOSRV, DBG, "ciosrv(%p)::notify() from thread: 0x%x "
				"to thread: 0x%x, ignoring event: %s",
				this, pthread_self(), tid, cevent(ev).c_str());

#if 0
	} catch (eIoSvcNotFound& e) {
		WRITELOG(CIOSRV, DBG, "ciosrv(%p)::notify() from thread: 0x%x "
				"to thread: 0x%x, ciosrv:%p already deleted",
				this, pthread_self(), tid, this);
#endif
	}
}


void
ciosrv::register_filedesc_r(int fd)
{
	RwLock lock(&ciosrv::iothread_lock, RwLock::RWLOCK_READ);

	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::register_filedesc_r() fd=%d", this, fd);
	threads[tid]->rfds[fd] = this;
}

void
ciosrv::deregister_filedesc_r(int fd)
{
	RwLock lock(&ciosrv::iothread_lock, RwLock::RWLOCK_READ);

	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::deregister_filedesc_r() fd=%d", this, fd);
	threads[tid]->rfds.erase(fd);
}

void
ciosrv::register_filedesc_w(int fd)
{
	RwLock lock(&ciosrv::iothread_lock, RwLock::RWLOCK_READ);

	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::register_filedesc_w() fd=%d", this, fd);
	threads[tid]->wfds[fd] = this;
}

void
ciosrv::deregister_filedesc_w(int fd)
{
	RwLock lock(&ciosrv::iothread_lock, RwLock::RWLOCK_READ);

	WRITELOG(CIOSRV, DBG, "ciosrv(%p)::deregister_filedesc_w() fd=%d", this, fd);
	threads[tid]->wfds.erase(fd);
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
ciosrv::fdset(int& maxfd,
			  fd_set* readfds,
			  fd_set* writefds)
{
	std::set<ciosrv*>::iterator it;
	FD_ZERO(readfds);
	FD_ZERO(writefds);
	maxfd = 0;

	pthread_t tid = pthread_self();

	RwLock lock(&ciosrv::iothread_lock, RwLock::RWLOCK_READ);

	// wakeup pipe
	FD_SET(threads[tid]->pipe->pipefd[0], readfds);
	maxfd = (threads[tid]->pipe->pipefd[0] > maxfd) ? threads[tid]->pipe->pipefd[0] : maxfd;


	{
		//Lock lock(&ciosrv::ciosrv_list_mutex[tid]);

		// add ciosrv instances from ciosrv_insertion_list to ciosrv_list
		for (it = ciosrv::threads[tid]->ciosrv_insertion_list.begin();
				it != ciosrv::threads[tid]->ciosrv_insertion_list.end(); ++it)
		{
			//WRITELOG(CIOSRV, DBG, "ciosrv::fdset() inserting %p", (*it));
			ciosrv::threads[tid]->ciosrv_list.insert(*it);
		}
		ciosrv::threads[tid]->ciosrv_insertion_list.clear();

		// remove ciosrv instances from ciosrv_list defined in ciosrv_deletion_list
		for (it = ciosrv::threads[tid]->ciosrv_deletion_list.begin();
				it != ciosrv::threads[tid]->ciosrv_deletion_list.end(); ++it)
		{
			//WRITELOG(CIOSRV, DBG, "ciosrv::fdset() deleting %p", (*it));
			ciosrv::threads[tid]->ciosrv_list.erase(*it);
		}
		ciosrv::threads[tid]->ciosrv_deletion_list.clear();




#if 0
		for (it = ciosrv::ciosrv_list[tid].begin();
				it != ciosrv::ciosrv_list[tid].end(); ++it)
		{
			ciosrv* cio = (*it);
			if (ciosrv_deletion_list[tid].find(cio) != ciosrv_deletion_list[tid].end())
			{
				throw eDebug();
				continue;
			}
			WRITELOG(CIOSRV, DBG, "%s", cio->c_str());
		}
#endif

	}


	/*
	 *
	 */

	//dump_fdsets();

	//Lock lock(&ciosrv::ciosrv_list_mutex[tid]);

	for (std::map<int, ciosrv*>::iterator
			iit = threads[tid]->rfds.begin(); iit != threads[tid]->rfds.end(); ++iit)
	{
		//WRITELOG(CIOSRV, DBG, "ciosrv::fdset() read-fd:%d", iit->first);
		FD_SET(iit->first, readfds);
		maxfd = (iit->first > maxfd) ? iit->first : maxfd;
	}

	for (std::map<int, ciosrv*>::iterator
			iit = threads[tid]->wfds.begin(); iit != threads[tid]->wfds.end(); ++iit)
	{
		//WRITELOG(CIOSRV, DBG, "ciosrv::fdset() write-fd:%d", iit->first);
		FD_SET(iit->first, writefds);
		maxfd = (iit->first > maxfd) ? iit->first : maxfd;
	}

	maxfd++;
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
ciosrv::handle(int rc,
			   fd_set* readfds,
			   fd_set* writefds,
			   fd_set* exceptfds)
{
	std::set<ciosrv*>::iterator it;

	pthread_t tid = pthread_self();

#ifndef NDEBUG
	//dump_active_fdsets(rc, readfds, writefds, exceptfds);
#endif

	//handle_events(rc, readfds, writefds, exceptfds);

#if 0
	std::set<ciosrv*> ciosvces;
	{
		WRITELOG(CIOSRV, DBG, "ciosrv(0x%x)::handle() locking ciosrv_list mutex",
				tid);

		Lock lock(&ciosrv::ciosrv_list_mutex[tid]);
		for (it = ciosrv::ciosrv_list[tid].begin(); it != ciosrv::ciosrv_list[tid].end(); ++it)
		{
			ciosvces.insert(*it);
		}

		WRITELOG(CIOSRV, DBG, "ciosrv(0x%x)::handle() unlocking ciosrv_list mutex",
				tid);
	}
#endif

	WRITELOG(CIOSRV, DBG, "ciosrv::handle() got file descriptors:%d", rc);

	// events on file/socket descriptors
	//
	//

	if (rc > 0)
	{
		// use local copy of fdsets
		// cio->rfds and cio->wfds may be changed within the handle_Xevent methods

		if (FD_ISSET(threads[tid]->pipe->pipefd[0], readfds))
		{
			handle_events(rc, readfds, exceptfds);
		}

		// make temporary copy of rfds
		std::list<int> rfds;
		for (std::map<int, ciosrv*>::iterator
				it = threads[tid]->rfds.begin(); it != threads[tid]->rfds.end(); ++it)
		{
			rfds.push_back(it->first);
		}

		handle_rfds(rfds, rc, readfds, exceptfds);

		// make temporary copy of wfds
		std::list<int> wfds;
		for (std::map<int, ciosrv*>::iterator
				it = threads[tid]->wfds.begin(); it != threads[tid]->wfds.end(); ++it)
		{
			wfds.push_back(it->first);
		}

		handle_wfds(wfds, rc, writefds, exceptfds);
	}
	else if ((rc == 0) || (EINTR == errno))
	{
		handle_timeouts();
	}
}



/*static*/
void
ciosrv::handle_rfds(
		std::list<int>& rfds,
		int rc,
		fd_set *readfds,
		fd_set *exceptfds)
{
	pthread_t tid = pthread_self();

	while (not rfds.empty()) {
next_element:
		int fd = rfds.front(); rfds.pop_front();

		if (threads[tid]->rfds.find(fd) == threads[tid]->rfds.end()) {
			goto next_element;
		}

		ciosrv *cio = threads[tid]->rfds[fd];

		// check whether cio is still a valid object
		if (threads[tid]->ciosrv_deletion_list.find(cio) != threads[tid]->ciosrv_deletion_list.end()) {
			WRITELOG(CIOSRV, DBG, "ciosrv::handle_rfds(): skipping already deleted object %p "
							"(%d deleted in this round)", cio,
							threads[tid]->ciosrv_deletion_list.size());

			threads[tid]->rfds.erase(fd);
			goto next_element; // do nothing for this object, as it was already deleted
		}


		if (FD_ISSET(fd, readfds))
		{
			WRITELOG(CIOSRV, DBG, "ciosrv::handle_rfds(): handle r/x events read-fds:  %d", fd);
			cio->handle_revent(fd);
		}
		if (FD_ISSET(fd, exceptfds))
		{
			WRITELOG(CIOSRV, DBG, "ciosrv::handle_rfds(): handle r/x events excp-fds:  %d", fd);
			cio->handle_xevent(fd);
		}
	}
}



/*static*/
void
ciosrv::handle_wfds(
		std::list<int>& wfds,
		int rc,
		fd_set *writefds,
		fd_set *exceptfds)
{
	pthread_t tid = pthread_self();

	while (not wfds.empty()) {
next_element:
		int fd = wfds.front(); wfds.pop_front();

		if (threads[tid]->wfds.find(fd) == threads[tid]->wfds.end()) {
			goto next_element;
		}

		// check whether cio is still a valid object
		ciosrv *cio = threads[tid]->wfds[fd];

		if (threads[tid]->ciosrv_deletion_list.find(cio) != threads[tid]->ciosrv_deletion_list.end()) {
			WRITELOG(CIOSRV, DBG, "ciosrv::handle_wfds(): skipping already deleted object %p "
							"(%d deleted in this round)", cio,
							threads[tid]->ciosrv_deletion_list.size());

			threads[tid]->wfds.erase(fd);
			goto next_element; // do nothing for this object, as it was already deleted
		}


		if (FD_ISSET(fd, writefds))
		{
			WRITELOG(CIOSRV, DBG, "ciosrv::handle_wfds(): handle w/x events write-fds:  %d", fd);
			cio->handle_wevent(fd);
		}
		if (FD_ISSET(fd, exceptfds))
		{
			WRITELOG(CIOSRV, DBG, "ciosrv::handle_wfds(): handle w/x events excp-fds:  %d", fd);
			cio->handle_xevent(fd);
		}
	}
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


/*static*/
void
ciosrv::handle_events(int rc,
			   fd_set* readfds,
			   fd_set* exceptfds)
{


	pthread_t tid = pthread_self();

	{
		RwLock lock(&ciosrv::threads[tid]->wakeup_rwlock, RwLock::RWLOCK_WRITE); // for locking ciosrv_wakeup[tid]
		ciosrv::threads[tid]->flags.reset(CIOSRV_FLAG_WAKEUP_CALLED);
	}

	WRITELOG(CIOSRV, DBG, "ciosrv(0x%x)::handle_events()", tid);

#if 0
	{
		RwLock lock(&ciosrv::wakeup_rwlock[tid], RwLock::RWLOCK_READ); // for locking ciosrv_wakeup[tid]

		std::string s_wakeup;
		for (std::set<ciosrv*>::iterator
				ct = ciosrv_wakeup[tid].begin(); ct != ciosrv_wakeup[tid].end(); ++ct)
		{
			cvastring vas(32);
			s_wakeup.append(vas("0x%x ", *ct));
		}
		WRITELOG(CIOSRV, DBG, "ciosrv()::handle_events() "
				"WAKEUP-LIST [1] => %s",
				s_wakeup.c_str());
	}
#endif

	/*
	 * create temporary copy from ciosrv_wakeup list
	 */

	std::vector<ciosrv*> wakeup_list;
	{
		RwLock lock(&ciosrv::threads[tid]->wakeup_rwlock, RwLock::RWLOCK_READ); // for locking ciosrv_wakeup[tid]

		for (std::set<ciosrv*>::iterator ct = ciosrv::threads[tid]->ciosrv_wakeup.begin();
				ct != ciosrv::threads[tid]->ciosrv_wakeup.end(); ++ct)
		{
			{
				//Lock lock(&ciosrv::ciosrv_list_mutex[tid]); // for locking ciosrv_deletion_list[tid]
				{
					Lock lock(&(threads[tid]->ciosrv_list_mutex));
					if (ciosrv::threads[tid]->ciosrv_deletion_list.find(*ct) != ciosrv::threads[tid]->ciosrv_deletion_list.end())
					{
						WRITELOG(CIOSRV, DBG, "ciosrv()::handle_events() ciosrv:0x%x already deleted, ignoring", *ct);
						continue;
					}
				}

				try {
					ciosrv_exists(*ct);
				} catch (eIoSvcNotFound& e) {
					continue;
				}
			}

			WRITELOG(CIOSRV, DBG, "ciosrv()::handle_events() "
					"LIST thread:0x%x pushing on wakeup_list => ciosrv:%s",
					tid, (*ct)->c_str());

			wakeup_list.push_back(*ct);
		}

		ciosrv::threads[tid]->ciosrv_wakeup.clear(); // clear ciosrv_wakeup list for this thread tid
	}

#if 0
	{
		//RwLock lock(&ciosrv::ciosrv_wakeup_rwlock[tid], RwLock::RWLOCK_READ); // for locking ciosrv_wakeup[tid]

		std::string s_wakeup;
		for (std::vector<ciosrv*>::iterator
				ct = wakeup_list.begin(); ct != wakeup_list.end(); ++ct)
		{
			cvastring vas(32);
			s_wakeup.append(vas("0x%x ", *ct));
		}
		WRITELOG(CIOSRV, DBG, "ciosrv()::handle_events() "
				"WAKEUP-LIST [2] => %s",
				s_wakeup.c_str());
	}
#endif





	/*
	 * iterate over all ciosrv instances with a new event
	 */

	std::vector<ciosrv*>::iterator it;
	for (it = wakeup_list.begin(); it != wakeup_list.end(); ++it)
	{
		WRITELOG(CIOSRV, DBG, "ciosrv()::handle_events() for object ciosrv:0x%x", *it);

		{
			Lock lock(&(threads[tid]->ciosrv_list_mutex));
			//Lock lock(&ciosrv::ciosrv_list_mutex[tid]);
			if (ciosrv::threads[tid]->ciosrv_deletion_list.find(*it) != ciosrv::threads[tid]->ciosrv_deletion_list.end())
			{
				WRITELOG(CIOSRV, DBG, "ciosrv()::handle_events() ciosrv:0x%x already deleted, ignoring", *it);
				continue;
			}
		}

		ciosrv* cio = (*it);


		/*
		 * make temporary copy of cio->events
		 */

		std::vector<cevent*> events;
		{
			Lock lock(&(cio->event_mutex));
			//copy(cio->events.begin(), cio->events.end(), events.begin());
			for (std::list<cevent*>::iterator lt = cio->events.begin();
					lt != cio->events.end(); ++lt)
			{
				events.push_back(*lt);
			}
			cio->events.clear();
		}

		for (std::vector<cevent*>::iterator et = events.begin(); et != events.end(); ++et)
		{
			cevent* evt = (*et);

			WRITELOG(CIOSRV, DBG, "ciosrv()::handle_events() ciosrv:0x%x event: %s", *it, evt->c_str());

			cio->handle_event(*evt);

			delete evt;

			//Lock lock(&ciosrv::ciosrv_list_mutex[tid]);
			{
				Lock lock(&(threads[tid]->ciosrv_list_mutex));
				if (ciosrv::threads[tid]->ciosrv_deletion_list.find(cio) != ciosrv::threads[tid]->ciosrv_deletion_list.end())
				{
					WRITELOG(CIOSRV, DBG, "ciosrv()::handle_events() ciosrv:0x%x already deleted, ignoring", *it);
					break;
				}
			}
		}

	}
	// end of block removes wakeup_list

	/*
	 * clear the wakeup pipe
	 */

	WRITELOG(CIOSRV, DBG, "ciosrv()::handle_events() pipe wakeup on thread: 0x%x", pthread_self());

	threads[tid]->pipe->recvmsg(); // clear pipe readfd


	WRITELOG(CIOSRV, DBG, "ciosrv(0x%x)::handle_events() ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
			tid);
}


/* static */void
ciosrv::child_sig_handler (int x) {
	WRITELOG(CIOSRV, WARN, "got signal: %d", x);
    // signal(SIGCHLD, child_sig_handler);
}


void
ciosrv::init()
{
	if (ciosrv::threads.empty())
	{
		pthread_rwlock_init(&ciosrv::iothread_lock, 0);
	}

	RwLock lock(&ciosrv::iothread_lock, RwLock::RWLOCK_WRITE);

	/*
	 * allocate per-thread data structures
	 */

	pthread_t tid = pthread_self();

	if (threads.find(tid) == threads.end())
	{
		threads[tid] = new ciothread();
	}
}



void
ciosrv::destroy()
{
	pthread_t tid = pthread_self();

	if (ciosrv::threads.find(tid) != ciosrv::threads.end())
	{
          RwLock lock(&ciosrv::iothread_lock, RwLock::RWLOCK_WRITE);

          /*
           * deallocate per-thread data structures
           */

          delete threads[tid]; threads.erase(tid);
        }

	if (ciosrv::threads.empty())
	{
		pthread_rwlock_destroy(&ciosrv::iothread_lock);
	}
}



void
ciosrv::run()
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

	pthread_t tid = pthread_self();

	if (threads.find(tid) == threads.end()) {
		ciosrv::init();
	}

	/*
	 * the infinite loop ...
	 */

	while (keep_on)
	{
		fd_set readfds;
		fd_set writefds;
		fd_set exceptfds;
		struct timespec ts = { 0, 0 };
		int maxfd;
		int rc;
		time_t ntimeout = time(NULL) + 60 /* seconds */; // one wakeup every 60seconds

		// initialize exceptfds, because it is not set by ciosrv::fdset()
		FD_ZERO(&exceptfds);

		ciosrv::fdset(maxfd, &readfds, &writefds); // get all file descriptors
		ciosrv::next_timeout(ntimeout); // get next timeout

		ts.tv_sec = ((ntimeout - time(NULL)) > 0) ? ntimeout - time(NULL) : 0;

		// call select
		if ((rc = pselect(maxfd + 1, &readfds, &writefds, &exceptfds, &ts, &empty_mask)) < 0)
		{
			WRITELOG(CIOSRV, WARN, "ciosrv::run() error in pselect() errno:%d %s",
					errno, strerror(errno));

			// handle error conditions
			switch (errno) {
			case EINTR:
				WRITELOG(CIOSRV, WARN, "pselect() got interrupted");
				goto handle_packets;
				break;
			default:
#ifndef NDEBUG
				throw eIoSvcRunError(); // for debugging: handle error
#endif
				break;
			}
		}
		else
		{

handle_packets:			// handle incoming events

#ifndef NDEBUG
			try {
				ciosrv::handle(rc, &readfds, &writefds, &exceptfds);
			} catch (cerror& e) {
				//fprintf(stderr, "exception\n");
				//throw;
			}
#else
			try {
				ciosrv::handle(rc, &readfds, &writefds, &exceptfds);
			} catch (cerror& e) {
				//fprintf(stderr, "exception\n");
				//throw;
			}
#endif

		}
	}
}



/*static*/ciosrv*
ciosrv::ciosrv_exists(ciosrv* iosrv) throw (eIoSvcNotFound)
{
	pthread_t tid = pthread_self();

	if (ciosrv::threads[tid]->ciosrv_list.find(iosrv) == ciosrv::threads[tid]->ciosrv_list.end())
		throw eIoSvcNotFound();
	return iosrv;
}



/* static */void
ciosrv::dump_fdsets()
{
	pthread_t tid = pthread_self();

	std::string info("ciosrv");
	cvastring vas(256);

	for (std::set<ciosrv*>::iterator
			ht = ciosrv::threads[tid]->ciosrv_list.begin();
	      ht != ciosrv::threads[tid]->ciosrv_list.end(); ++ht)
	{
		info.append(vas("%s\n",(*ht)->c_str()));
	}

	WRITELOG(CIOSRV, DBG, "%s", info.c_str());
}


/* static */void
ciosrv::dump_active_fdsets(
		int rc,
		fd_set* readfds,
		fd_set* writefds,
		fd_set* exceptfds)
{
	pthread_t tid = pthread_self();

	std::string s_rfdset;
	std::string s_xrfdset;
	std::string s_wfdset;
	std::string s_xwfdset;

	std::set<ciosrv*>::iterator it;

	Lock lock(&(ciosrv::threads[tid]->ciosrv_list_mutex));


	if (rc > 0)
	{
		cvastring vas;

		RwLock lock(&ciosrv::iothread_lock, RwLock::RWLOCK_READ);

		for (std::map<int, ciosrv*>::iterator
				iit = threads[tid]->rfds.begin(); iit != threads[tid]->rfds.end(); ++iit)
		{
			if (FD_ISSET(iit->first, readfds))
			{
				s_rfdset.append(vas(" %d", (iit->first)));
			}
			if (FD_ISSET(iit->first, exceptfds))
			{
				s_xrfdset.append(vas(" %d", (iit->first)));
			}

		}

		for (std::map<int, ciosrv*>::iterator
				iit = threads[tid]->wfds.begin(); iit != threads[tid]->wfds.end(); ++iit)
		{
			if (FD_ISSET(iit->first, writefds))
			{
				s_wfdset.append(vas(" %d", (iit->first)));
			}
			if (FD_ISSET(iit->first, exceptfds))
			{
				s_xwfdset.append(vas(" %d", (iit->first)));
			}
		}
	}

	WRITELOG(CIOSRV, DBG, "ciosrv::dump_active_fdsets() "
			"rfds: %s wfds: %s efds: %s",
			s_rfdset.c_str(),
			s_wfdset.c_str(),
			s_xwfdset.c_str());
}
