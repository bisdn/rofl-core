/*
 * ctimers.h
 *
 *  Created on: 26.01.2014
 *      Author: andreas
 */

#ifndef CTIMERS_H_
#define CTIMERS_H_

#include <set>
#include <algorithm>

#include "rofl/common/ctimer.h"
#include "rofl/common/logging.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/croflexception.h"

namespace rofl {

class eTimersBase 		: public RoflException {};
class eTimersNotFound	: public eTimersBase {};

class ctimers {

	std::multiset<ctimer>	timers;
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
	ctimer
	get_next_timer();

	/**
	 *
	 */
	const ctimerid&
	add_timer(ctimer const& t);

	/**
	 *
	 */
	const ctimerid&
	reset(const ctimerid& timer_id, const ctimespec& t);

	/**
	 *
	 */
	ctimer
	get_expired_timer();

	/**
	 *
	 */
	bool
	pending(const ctimerid& timer_id);

	/**
	 *
	 */
	void
	cancel(const ctimerid& timer_id);

	/**
	 * @brief	cancel all timers
	 */
	void
	cancel_all();


	/**
	 * @brief	cancel all timers
	 */
	void
	clear();


public:

	friend std::ostream&
	operator<< (std::ostream& os, ctimers const& t) {
		os << indent(0) << "<ctimers: >" << std::endl;
		indent i(2);
		for (std::multiset<ctimer>::const_iterator it = t.timers.begin(); it != t.timers.end(); ++it) {
			os << (*it);
		}
		return os;
	};
};

};

#endif /* CTIMERS_H_ */
