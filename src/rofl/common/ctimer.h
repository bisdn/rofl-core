/*
 * ctimer.h
 *
 *  Created on: 26.01.2014
 *      Author: andreas
 */

#ifndef CTIMER_H_
#define CTIMER_H_

#include <sys/time.h>
#include <time.h>

#include <iostream>

#include "rofl/common/cmemory.h"
#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/ctimerid.h"
#include "rofl/common/ctimespec.h"

namespace rofl {

/**
 * @brief	Environment expected by an instance of class rofl::ctimer
 */
class ctimer_env {
public:
	virtual
	~ctimer_env()
	{};
};

/**
 * @ingroup common_devel_ioservice
 *
 * @brief	Single timer object in rofl-common
 *
 * A single timer instance that binds together the timer value
 * stored in an instance of class rofl::ctimespec, rofl-common's
 * internal timer handle (rofl::ctimerid) and the user defined
 * opaque values for timer type and data segment. Used by classes
 * rofl::ciosrv and rofl::cioloop.
 */
class ctimer {
public:

	/**
	 * @brief	Returns a rofl::ctimer object with internal timespec set to current time.
	 *
	 * @return rofl::ctimer object with current time
	 */
	static
	rofl::ctimer
	now() {
		ctimer timer;
		timer.set_timespec() = ctimespec::now();
		return timer;
	};

public:

	/**
	 * @brief	ctimer destructor
	 */
	virtual
	~ctimer()
	{};

	/**
	 * @brief	ctimer default constructor
	 */
	ctimer() :
		timer_id(),
		env(0),
		timespec(0, 0),
		opaque(0),
		data(0)
	{};

	/**
	 * @brief	ctimer constructor
	 *
	 * @param env environment for this rofl::ctimer instance
	 * @param opaque arbitrary number ćhosen by user, not used by ctimer class
	 * @param timespec absolute time when this timer expires
	 * @param data arbitrary pointer chosen by user, not used by ctimer class
	 */
	ctimer(
			ctimer_env* env,
			int opaque,
			const rofl::ctimespec& timespec,
			void *data = (void*)0) :
				env(env),
				timespec(timespec),
				opaque(opaque),
				data(data)
	{ this->timespec += rofl::ctimespec::now(); };

	/**
	 * @brief	ctimer copy constructor
	 */
	ctimer(
			const rofl::ctimer& timer)
	{ *this = timer; };

public:

	/**
	 * @name	Operators
	 */

	/**@{*/

	/**
	 * @brief	Assignment operator
	 */
	ctimer&
	operator= (
			const rofl::ctimer& timer) {
		if (this == &timer)
			return *this;
		timer_id	= timer.timer_id;
		env			= timer.env;
		timespec	= timer.timespec;
		opaque		= timer.opaque;
		data		= timer.data;
		return *this;
	};

#if 0
	/**
	 *
	 */
	rofl::ctimer
	operator+ (
			const rofl::ctimer& t) {
		ctimer timer;
		timer.timespec = timespec + t.timespec;
		return timer;
	};

	/**
	 *
	 */
	rofl::ctimer
	operator- (
			const rofl::ctimer& t) {
		ctimer timer;
		timer.timespec = timespec - t.timespec;
		return timer;
	};

	/**
	 *
	 */
	rofl::ctimer&
	operator+= (
			const rofl::ctimer& t) {
		timespec += t.timespec;
		return *this;
	};

	/**
	 *
	 */
	rofl::ctimer&
	operator-= (
			const rofl::ctimer& t) {
		timespec -= t.timespec;
		return *this;
	};
#endif

	/**
	 * @brief	Equality operator
	 */
	bool
	operator== (
			const rofl::ctimer& t) const
	{ return not ((*this < t) && (t < *this)); };

	/**
	 * @brief	Inequality operator
	 */
	bool
	operator!= (
			const rofl::ctimer& t) const
	{ return not (*this == t); };

	/**
	 * @brief	Lesser operator
	 */
	bool
	operator< (
			const rofl::ctimer& t) const
	{ return (timespec < t.timespec); };

	/**
	 * @brief	Lesser-or-equal operator
	 */
	bool
	operator<= (
			const rofl::ctimer& t) const
	{ return ((*this < t) || (*this == t)); };

	/**
	 * @brief	Greater operator
	 */
	bool
	operator> (
			const rofl::ctimer& t) const
	{ return not (*this <= t); };

	/**
	 * @brief	Greater-or-equal operator
	 */
	bool
	operator>= (
			const rofl::ctimer& t) const
	{ return ((*this > t) || (*this == t)); };

	/**@}*/

public:

	/**
	 * @name	Methods to access data members
	 */

	/**@{*/

	/**
	 * @brief	Returns pointer to environment of this rofl::ctimer instance
	 */
	ctimer_env*
	get_timer_env() const
	{ return env; };

	/**
	 * @brief	Returns const reference to rofl-common's internal handle assigned to this timer
	 */
	const rofl::ctimerid&
	get_timer_id() const
	{ return timer_id; };

	/**
	 * @brief	Returns opaque timer type chosen by user
	 */
	int
	get_opaque() const
	{ return opaque; };

	/**
	 * @brief	Returns pointer to opaque data segment chosen by user
	 */
	void*
	get_data() const
	{ return data; };

	/**
	 * @brief	Returns reference to rofl::ctimespec instance assigned to this timer
	 */
	rofl::ctimespec&
	set_timespec()
	{ return timespec; };

	/**
	 * @brief	Returns const reference to rofl::ctimespec instance assigned to this timer
	 */
	const rofl::ctimespec&
	get_timespec() const
	{ return timespec; };

	/**@}*/

public:

	class ctimer_find_by_timer_id {
		ctimerid timer_id;
	public:
		ctimer_find_by_timer_id(ctimerid timer_id) : timer_id(timer_id) {};
		bool operator() (const rofl::ctimer& t) {
			return (timer_id == t.get_timer_id());
		};
		bool operator() (ctimer const* t) {
			return (timer_id == t->get_timer_id());
		};
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const rofl::ctimer& timer) {
		ctimer delta = ctimer(timer);
		os << indent(0) << "<ctimer ";
		os << "opaque:" << timer.opaque << " ";
		os << "data:" << timer.data << " ";
		os << ">" << std::endl;
		rofl::indent i(2);
		os << timer.timer_id;
		os << timer.timespec;
		return os;
	};

private:

	static const long CC_TIMER_ONE_SECOND_S  = 1;
	static const long CC_TIMER_ONE_SECOND_NS = 1e9;

	rofl::ctimerid      timer_id;
	rofl::ctimer_env*   env;		// this refers to the ciosrv instance that registered the timer associated with this ctimer instance
	rofl::ctimespec     timespec;
	int                 opaque; 	// can be used as type field by a class deriving from ciosrv => not used by ctimer, ciosrv or cioloop
	void*               data;		// can be used as arbitrary pointer by a class deriving from ciosrv => not used by ctimer, ciosrv or cioloop
};

};

#endif /* CTIMER_H_ */
