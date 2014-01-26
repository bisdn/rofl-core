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

namespace rofl {

class ctimer {

#define CC_TIMER_ONE_SECOND_S 1
#define CC_TIMER_ONE_SECOND_NS 1000000000

	struct timespec		ts;
	int					type;
	cmemory				opaque;

public:

	/**
	 *
	 */
	ctimer();

	/**
	 *
	 */
	ctimer(int type, long tv_sec);

	/**
	 *
	 */
	ctimer(int type, long tv_sec, long tv_nsec);

	/**
	 *
	 */
	ctimer(int type, long tv_sec, long tv_nsec, cmemory const& opaque);

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

	friend std::ostream&
	operator<< (std::ostream& os, ctimer const& timer) {
		os << indent(0) << "<ctimer ";
		os << "type:" << timer.type << " ";
		os << "sec:" << timer.ts.tv_sec << " ";
		os << "nsec:" << timer.ts.tv_nsec << " ";
		os << ">" << std::endl;
		indent i(2);
		if (timer.opaque.memlen() > 0) {
			os << timer.opaque;
		}
		return os;
	};
};

};

#endif /* CTIMER_H_ */
