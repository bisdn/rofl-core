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
#include <sstream>

#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"

namespace rofl {

/**
 * @brief	Time specification used by class rofl::ctimer object
 * @ingroup common_devel_ioservice
 */
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
			const ctimespec& t) const;

	/**
	 *
	 */
	ctimespec
	operator- (
			const ctimespec& t) const;

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
		os << "sec:" << (long int)timespec.ts.tv_sec << " ";
		os << "nsec:" << (long int)timespec.ts.tv_nsec << " ";
		os << ">" << std::endl;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << (unsigned long int)ts.tv_sec << "s " << (unsigned long int)ts.tv_nsec << "ns ";
		return ss.str();
	};

private:

	static const long CC_TIMER_ONE_SECOND_S = 1;
	static const long CC_TIMER_ONE_SECOND_NS = 1000000000;

	struct timespec		ts;
};

}; // end of namespace rofl

#endif /* CTIMESPEC_H_ */
