/*
 * ctimespec.h
 *
 *  Created on: 04.06.2014
 *      Author: andreas
 */

#ifndef CTIMESPEC_H_
#define CTIMESPEC_H_

#include <time.h>

#include <iostream>

#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"

namespace rofl {

class ctimespec {
public:

	/**
	 *
	 */
	ctimespec();

	/**
	 *
	 */
	ctimespec(
			long tv_sec);

	/**
	 *
	 */
	ctimespec(
			long tv_sec, long tv_nsec);

	/**
	 *
	 */
	ctimespec(
			const ctimespec& timespec);

	/**
	 *
	 */
	ctimespec&
	operator= (
			const ctimespec& timespec);

	/**
	 *
	 */
	virtual
	~ctimespec();

public:

	/**
	 *
	 */
	static ctimespec
	now();

	/**
	 *
	 */
	struct timespec&
	set_timespec() { return ts; };

	/**
	 *
	 */
	const struct timespec&
	get_timespec() const { return ts; };

	/**
	 *
	 */
	ctimespec
	operator+ (
			const ctimespec& t);

	/**
	 *
	 */
	ctimespec
	operator- (
			const ctimespec& t);

	/**
	 *
	 */
	ctimespec&
	operator+= (
			const ctimespec& t);

	/**
	 *
	 */
	ctimespec&
	operator-= (
			const ctimespec& t);

	/**
	 *
	 */
	bool
	operator== (
			const ctimespec& t) const;

	/**
	 *
	 */
	bool
	operator!= (
			const ctimespec& t) const;

	/**
	 *
	 */
	bool
	operator< (
			const ctimespec& t) const;

	/**
	 *
	 */
	bool
	operator<= (
			const ctimespec& t) const;

	/**
	 *
	 */
	bool
	operator> (
			const ctimespec& t) const;

	/**
	 *
	 */
	bool
	operator>= (
			const ctimespec& t) const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, const ctimespec& timespec) {
		os << rofl::indent(0) << "<ctimespec ";
		os << "sec:" << timespec.get_timespec().tv_sec << " ";
		os << "nsec:" << timespec.get_timespec().tv_nsec << " ";
		os << ">" << std::endl;
		return os;
	};

private:

#define CC_TIMER_ONE_SECOND_S 1
#define CC_TIMER_ONE_SECOND_NS 1000000000

	struct timespec		ts;
};

}; // end of namespace rofl

#endif /* CTIMESPEC_H_ */
