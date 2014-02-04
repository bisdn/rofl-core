/*
 * cfdset.h
 *
 *  Created on: 26.01.2014
 *      Author: andreas
 */

#ifndef CFDSET_H_
#define CFDSET_H_

#include <sys/select.h>

#include <map>
#include <iostream>
#include <algorithm>

#include "rofl/common/logging.h"
#include "rofl/common/thread_helper.h"

namespace rofl {

class cfdowner {};

class cfdset : public std::map<int, cfdowner*>
{
	PthreadRwLock rwlock;

public:

	/**
	 *
	 */
	cfdset();

	/**
	 *
	 */
	cfdset(cfdset const& fdset);

	/**
	 *
	 */
	virtual
	~cfdset();

	/**
	 *
	 */
	cfdset&
	operator= (cfdset const& fdset);

public:

	/**
	 *
	 */
	int
	set_fds(fd_set* fds);

	/**
	 * @name Management methods for file descriptors
	 */

	/**@{*/

	/**
	 * @brief	Registers a file descriptor.
	 *
	 * @param fd the file descriptor
	 */
	void
	add_fd(int fd, cfdowner *owner);


	/**
	 * @brief	Deregisters a file descriptor
	 *
	 * @param fd the file descriptor
	 */
	void
	drop_fd(int fd);

	/**@}*/

public:

	friend std::ostream&
	operator<< (std::ostream& os, cfdset const& fdset) {
		os << indent(0) << "<cfdset: ";
		for (std::map<int, cfdowner*>::const_iterator
				it = fdset.begin(); it != fdset.end(); ++it) {
			os << it->first << ":" << it->second << " ";
		}
		os << " >" << std::endl;
		return os;
	};
};

};

#endif /* CFDSET_H_ */
