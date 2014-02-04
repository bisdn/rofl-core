/*
 * cfdset.cc
 *
 *  Created on: 26.01.2014
 *      Author: andreas
 */

#include "cfdset.h"

using namespace rofl;

cfdset::cfdset()
{

}


cfdset::cfdset(cfdset const& fdset)
{
	*this = fdset;
}


cfdset::~cfdset()
{

}


cfdset&
cfdset::operator= (cfdset const& fdset)
{
	if (this == &fdset)
		return *this;

	RwLock rwlock1(rwlock, RwLock::RWLOCK_WRITE);
	// TODO: RwLock rwlock2(fdset.rwlock, RwLock::RWLOCK_READ);
	clear();
	insert(fdset.begin(), fdset.end());

	return *this;
}


int
cfdset::set_fds(fd_set* fds)
{
	RwLock(rwlock, RwLock::RWLOCK_READ);
	for (std::map<int, cfdowner*>::const_iterator it = begin(); it != end(); ++it) {
		FD_SET(it->first, fds);
	}
	return (std::max_element(begin(), end())->first) + 1;
}


void
cfdset::add_fd(int fd, cfdowner *owner)
{
	RwLock(rwlock, RwLock::RWLOCK_WRITE);
	(*this)[fd] = owner;
}


void
cfdset::drop_fd(int fd)
{
	RwLock(rwlock, RwLock::RWLOCK_WRITE);
	(*this).erase(fd);
}

