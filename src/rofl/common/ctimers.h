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

/**
 * @ingroup common_devel_ioservice
 *
 * @brief	List of timers objects of class rofl::ctimer
 */
class ctimers {
public:

	/**
	 * @brief	ctimers destructor
	 */
	virtual
	~ctimers()
	{};

	/**
	 * @brief	ctimers default constructor
	 */
	ctimers()
	{};

	/**
	 * @brief	ctimers copy constructor
	 */
	ctimers(
			const rofl::ctimers& timers)
	{ *this = timers; };

public:

	/**
	 * @name 	Operators
	 */

	/**@{*/

	/**
	 * @brief	Assignment operator
	 */
	ctimers&
	operator= (
			const rofl::ctimers& t) {
		if (this == &t)
			return *this;
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		timers.clear();
		timers.insert(t.timers.begin(), t.timers.end());
		return *this;
	};

	/**@}*/

public:

	/**
	 * @name	Managing timers
	 */

	/**@{*/

	/**
	 * @brief	Checks for an empty list.
	 */
	bool
	empty() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		return (timers.empty());
	};

	/**
	 * @brief	Returns a copy of the next expiring timer in this timer list
	 *
	 * @exception eTimersNotFound timer list is empty
	 */
	rofl::ctimer
	get_next_timer() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (timers.empty())
			throw eTimersNotFound();
		std::multiset<ctimer>::iterator first = timers.begin();
		ctimer timer(*first);
		return timer;
	};

	/**
	 * @brief	Inserts a new timer into the timer list
	 *
	 * @return rofl-common's timer handle
	 */
	const rofl::ctimerid&
	add_timer(
			const rofl::ctimer& t) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		std::multiset<ctimer>::iterator it = timers.insert(t);
		return it->get_timer_id();
	};

	/**
	 * @brief	Resets an existing timer identifier by its handle with a new timeout value
	 *
	 * Please note, the returned timer handle differs from the one previously identifying
	 * the timer!
	 *
	 * @param timer_id handle to existing timer
	 * @param timespec new timeout value
	 * @return const reference to new timer handle
	 *
	 * @exception eTimersNotFound timer handle not found
	 */
	const rofl::ctimerid&
	reset(
			const rofl::ctimerid& timer_id,
			const rofl::ctimespec& timespec) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		std::multiset<ctimer>::iterator it;
		if ((it = find_if(timers.begin(), timers.end(), ctimer::ctimer_find_by_timer_id(timer_id))) == timers.end()) {
			throw eTimersNotFound();
		}
		const ctimerid& timerid = timers.insert(ctimer(it->get_timer_env(), it->get_opaque(), timespec))->get_timer_id();
		timers.erase(it);
		return timerid;
	};

	/**
	 * @brief	Returns the next timer from timer list, when it has already expired.
	 *
	 * This method is used for reading in a loop all timers that have already expired.
	 * If no or no further timers have expired, an exception is thrown.
	 *
	 * @return copy of rofl::ctimer instance that has expired
	 * @exception eTimersNotFound no (or no further) expired timer in timer list
	 */
	rofl::ctimer
	get_expired_timer() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		if (timers.empty()) {
			throw eTimersNotFound();
		}
		ctimer now = ctimer::now();

		std::multiset<ctimer>::iterator it = timers.begin();

		ctimer timer = *(it);

		if (timer > now) {
			throw eTimersNotFound();
		}

		timers.erase(it);

		return timer;
	};

	/**
	 * @brief	Returns a boolean value indicating whether this timers list has an urgent timer.
	 *
	 * @return boolean value
	 */
	bool
	has_expired_timer() const {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		if (timers.empty()) {
			return false;
		}
		if (*(timers.begin()) < ctimer::now()) {
			return true;
		}
		return false;
	};

	/**
	 * @brief	Checks whether a certain timer identified by the given handle is still pending
	 *
	 * @param timer_id timer handle
	 * @return true, when the timer still exists
	 */
	bool
	pending(
			const rofl::ctimerid& timer_id) {
		RwLock lock(rwlock, RwLock::RWLOCK_READ);
		std::multiset<ctimer>::iterator it;
		if ((it = find_if(timers.begin(), timers.end(), ctimer::ctimer_find_by_timer_id(timer_id))) == timers.end()) {
			return false;
		}
		return true;
	};

	/**
	 * @brief	Removes a timer identified by the given timer handle from this timer list
	 *
	 * @param timer_id timer handle
	 */
	void
	cancel(
			const ctimerid& timer_id) {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		std::multiset<ctimer>::iterator it;
		if ((it = find_if(timers.begin(), timers.end(), ctimer::ctimer_find_by_timer_id(timer_id))) == timers.end()) {
			return;
		}
		timers.erase(it);
	};

	/**
	 * @brief	Removes all timers from this timer list
	 */
	void
	clear() {
		RwLock lock(rwlock, RwLock::RWLOCK_WRITE);
		timers.clear();
	};

	/**@}*/

public:

	friend std::ostream&
	operator<< (std::ostream& os, const rofl::ctimers& t) {
		os << indent(0) << "<ctimers: >" << std::endl;
		indent i(2);
		for (std::multiset<ctimer>::const_iterator it = t.timers.begin(); it != t.timers.end(); ++it) {
			os << (*it);
		}
		return os;
	};

private:

	std::multiset<rofl::ctimer>	timers;
	mutable PthreadRwLock       rwlock;
};

};

#endif /* CTIMERS_H_ */
