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
		RwLock rwlock(queuelock, RwLock::RWLOCK_WRITE);
		while (not queue.empty()) {
			delete queue.front();
			queue.pop_front();
		}
	};

public:

	/**
	 *
	 */
	size_t
	write(rofl::openflow::cofmsg* msg) {
		RwLock rwlock(queuelock, RwLock::RWLOCK_WRITE);
		queue.push_back(msg);
		return queue.size();
	};

	/**
	 *
	 */
	rofl::openflow::cofmsg*
	read() {
		rofl::openflow::cofmsg* msg = (rofl::openflow::cofmsg*)0;
		RwLock rwlock(queuelock, RwLock::RWLOCK_WRITE);
		if (queue.empty()) {
			return msg;
		}
		msg = queue.front(); queue.pop_front();
		return msg;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const crofqueue& queue) {

		return os;
	};

private:

	std::list<rofl::openflow::cofmsg*> 	queue;
	PthreadRwLock						queuelock;
};

}; // end of namespace rofl

#endif /* CROFQUEUE_H_ */
