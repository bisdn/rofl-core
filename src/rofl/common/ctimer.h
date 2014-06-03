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

namespace rofl {

class ptrciosrv {};

class ctimer {


#define CC_TIMER_ONE_SECOND_S 1
#define CC_TIMER_ONE_SECOND_NS 1000000000

	ctimerid			timer_id;
	ptrciosrv			*ptr;		// this refers to the ciosrv instance that registered the timer associated with this ctimer instance
	struct timespec		ts;
	int					opaque; 	// can be used as type field by a class deriving from ciosrv => not used by ctimer, ciosrv or cioloop
	void*				data;		// can be used as arbitrary pointer by a class deriving from ciosrv => not used by ctimer, ciosrv or cioloop

public:

	/**
	 *
	 */
	ctimer();

	/**
	 *
	 */
	ctimer(ptrciosrv* ptr, int opaque, long tv_sec);

	/**
	 *
	 */
	ctimer(ptrciosrv* ptr, int opaque, long tv_sec, long tv_nsec);

	/**
	 *
	 */
	ctimer(ptrciosrv* ptr, int opaque, long tv_sec, long tv_nsec, void *data);

	/**
	 *
	 */
	ctimer(ctimer const& timer);

	/**
	 *
	 */
	ctimer&
	operator= (ctimer const& timer);

	/**
	 *
	 */
	virtual
	~ctimer();

	/**
	 *
	 */
	static ctimer
	now();

	/**
	 *
	 */
	ctimerid const&
	get_timer_id() const { return timer_id; };

	/**
	 *
	 */
	int
	get_opaque() const { return opaque; };

	/**
	 *
	 */
	void*
	get_data() const { return data; };

	/**
	 *
	 */
	struct timespec&
	get_ts() { return ts; };

	/**
	 *
	 */
	ctimer
	operator+ (ctimer const& t);

	/**
	 *
	 */
	ctimer
	operator- (ctimer const& t);

	/**
	 *
	 */
	ctimer&
	operator+= (ctimer const& t);

	/**
	 *
	 */
	ctimer&
	operator-= (ctimer const& t);

	/**
	 *
	 */
	bool
	operator== (ctimer const& t) const;

	/**
	 *
	 */
	bool
	operator!= (ctimer const& t) const;

	/**
	 *
	 */
	bool
	operator< (ctimer const& t) const;

	/**
	 *
	 */
	bool
	operator<= (ctimer const& t) const;

	/**
	 *
	 */
	bool
	operator> (ctimer const& t) const;

	/**
	 *
	 */
	bool
	operator>= (ctimer const& t) const;

public:

	class ctimer_find_by_timer_id {
		ctimerid timer_id;
	public:
		ctimer_find_by_timer_id(ctimerid timer_id) : timer_id(timer_id) {};
		bool operator() (ctimer const& t) {
			return (timer_id == t.get_timer_id());
		};
		bool operator() (ctimer const* t) {
			return (timer_id == t->get_timer_id());
		};
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, ctimer const& timer) {
		ctimer delta = ctimer(timer);
		os << indent(0) << "<ctimer ";
		os << "opaque:" << timer.opaque << " ";
		os << "sec:" << delta.ts.tv_sec << " ";
		os << "nsec:" << delta.ts.tv_nsec << " ";
		os << "data:" << timer.data << " ";
		os << ">" << std::endl;
		rofl::indent i(2);
		os << timer.timer_id;
		return os;
	};
};

};

#endif /* CTIMER_H_ */
