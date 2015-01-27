/*
 * crofqueue.h
 *
 *  Created on: 23.11.2014
 *      Author: andreas
 */

#ifndef CROFQUEUE_H_
#define CROFQUEUE_H_

#include <list>
#include <ostream>

#include "rofl/common/thread_helper.h"
#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/logging.h"

namespace rofl {

class crofqueue {
public:

	/**
	 *
	 */
	crofqueue()
	{};

	/**
	 *
	 */
	~crofqueue() {
		clear();
	};

public:

	/**
	 *
	 */
	bool
	empty() {
		RwLock rwlock(queuelock, RwLock::RWLOCK_READ);
		return queue.empty();
	};

	/**
	 *
	 */
	void
	clear() {
		RwLock rwlock(queuelock, RwLock::RWLOCK_WRITE);
		while (not queue.empty()) {
			delete queue.front();
			queue.pop_front();
		}
	};

	/**
	 *
	 */
	size_t
	store(rofl::openflow::cofmsg* msg) {
		RwLock rwlock(queuelock, RwLock::RWLOCK_WRITE);
		LOGGING_TRACE << "[rofl-common][crofqueue][store] msg: " << std::endl << *msg;
		queue.push_back(msg);
		return queue.size();
	};

	/**
	 *
	 */
	rofl::openflow::cofmsg*
	retrieve() {
		rofl::openflow::cofmsg* msg = (rofl::openflow::cofmsg*)0;
		RwLock rwlock(queuelock, RwLock::RWLOCK_WRITE);
		if (queue.empty()) {
			return msg;
		}
		msg = queue.front(); queue.pop_front();
		LOGGING_TRACE << "[rofl-common][crofqueue][retrieve] msg: " << std::endl << *msg;
		return msg;
	};

	/**
	 *
	 */
	rofl::openflow::cofmsg*
	front() {
		rofl::openflow::cofmsg* msg = (rofl::openflow::cofmsg*)0;
		RwLock rwlock(queuelock, RwLock::RWLOCK_READ);
		if (queue.empty()) {
			return msg;
		}
		msg = queue.front();
		LOGGING_TRACE << "[rofl-common][crofqueue][front] msg: " << std::endl << *msg;
		return msg;
	};

	/**
	 *
	 */
	void
	pop() {
		RwLock rwlock(queuelock, RwLock::RWLOCK_WRITE);
		if (queue.empty()) {
			return;
		}
		queue.pop_front();
		LOGGING_TRACE << "[rofl-common][crofqueue][pop] " << std::endl;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const crofqueue& queue) {
		RwLock rwlock(queue.queuelock, RwLock::RWLOCK_READ);
		os << rofl::indent(0) << "<crofqueue size #" << queue.queue.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::list<rofl::openflow::cofmsg*>::const_iterator
				it = queue.queue.begin(); it != queue.queue.end(); ++it) {
			os << *(*it);
		}
		return os;
	};

private:

	std::list<rofl::openflow::cofmsg*> 	queue;
	mutable PthreadRwLock				queuelock;
};

}; // end of namespace rofl

#endif /* CROFQUEUE_H_ */
