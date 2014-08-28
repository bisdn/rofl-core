/*
 * cdpid.h
 *
 *  Created on: 05.08.2014
 *      Author: andreas
 */

#ifndef CDPID_HPP_
#define CDPID_HPP_

#include <inttypes.h>
#include <iostream>
#include <string>
#include <sstream>

#include "logging.h"

namespace rofl {

class cdpid {
public:

	/**
	 *
	 */
	cdpid() :
		dpid(0) {};

	/**
	 *
	 */
	explicit cdpid(uint64_t dpid) :
			dpid(dpid) {
		std::stringstream sstr; sstr << dpid;
		s_dpid = sstr.str();
	};

	/**
	 *
	 */
	cdpid(const cdpid& dpid) { *this = dpid; };

	/**
	 *
	 */
	cdpid&
	operator= (const cdpid& dpid) {
		if (this == &dpid)
			return *this;
		this->dpid = dpid.dpid;
		this->s_dpid = dpid.s_dpid;
		return *this;
	};

	/**
	 *
	 */
	bool
	operator< (const cdpid& dpid) const {
		return (this->dpid < dpid.dpid);
	};

	/**
	 *
	 */
	bool
	operator== (const cdpid& dpid) const {
		return (this->dpid == dpid.dpid);
	};

	/**
	 *
	 */
	bool
	operator!= (const cdpid& dpid) const {
		return (this->dpid != dpid.dpid);
	};

public:

	/**
	 *
	 */
	uint64_t
	get_uint64_t() const { return dpid; };

	/**
	 *
	 */
	const std::string&
	str() const { return s_dpid; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cdpid& dpid) {
		os << rofl::indent(0) << "<cdpid "
				<< (unsigned long long)dpid.get_uint64_t() << " >" << std::endl;
		return os;
	};

private:

	uint64_t 	dpid;
	std::string	s_dpid;
};

};

#endif /* CDPID_HPP_ */
