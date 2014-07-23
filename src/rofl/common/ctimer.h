/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

class ptrciosrv {};

class ctimer {


#define CC_TIMER_ONE_SECOND_S 1
#define CC_TIMER_ONE_SECOND_NS 1000000000

	ctimerid			timer_id;
	ptrciosrv			*ptr;		// this refers to the ciosrv instance that registered the timer associated with this ctimer instance
	ctimespec			timespec;
	int					opaque; 	// can be used as type field by a class deriving from ciosrv => not used by ctimer, ciosrv or cioloop
	void*				data;		// can be used as arbitrary pointer by a class deriving from ciosrv => not used by ctimer, ciosrv or cioloop

public:

	/**
	 *
	 */
	static ctimer
	now();

	/**
	 *
	 */
	ctimer();

	/**
	 *
	 */
	ctimer(ptrciosrv* ptr, int opaque, const ctimespec& timespec, void *data = (void*)0);

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
	ptrciosrv*
	get_ptrciosrv() const { return ptr; };

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
	ctimespec&
	set_timespec() { return timespec; };

	/**
	 *
	 */
	const ctimespec&
	get_timespec() const { return timespec; };

#if 0
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
#endif

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
		os << "data:" << timer.data << " ";
		os << ">" << std::endl;
		rofl::indent i(2);
		os << timer.timer_id;
		os << timer.timespec;
		return os;
	};
};

};

#endif /* CTIMER_H_ */
