/*
 * ctimers.h
 *
 *  Created on: 26.01.2014
 *      Author: andreas
 */

#ifndef CTIMERS_H_
#define CTIMERS_H_

#include <set>

#include "rofl/common/ctimer.h"
#include "rofl/common/logging.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/croflexception.h"

namespace rofl {

class eTimersBase 		: public RoflException {};
class eTimersNotFound	: public eTimersBase {};

class ctimers {

	std::set<ctimer>	timers;
	PthreadRwLock 		rwlock;

public:

	/**
	 *
	 */
	ctimers();

	/**
	 *
	 */
	ctimers(ctimers const& timers);

	/**
	 *
	 */
	ctimers&
	operator= (ctimers const& timers);

	/**
	 *
	 */
	virtual
	~ctimers();

public:

	/**
	 *
	 */
	bool
	empty();

	/**
	 *
	 */
	void
	add_timer(ctimer const& t);

	/**
	 *
	 */
	ctimer
	get_expired_timer();

	/**
	 *
	 */
	bool
	has(int opaque);

	/**
	 *
	 */
	void
	cancel(int opaque);

	/**
	 * @brief	cancel all timers
	 */
	void
	reset();

	/**
	 *
	 */
	void
	reset(int opaque);
};

};

#endif /* CTIMERS_H_ */
