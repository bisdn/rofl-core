/*
 * cctlid.h
 *
 *  Created on: 19.04.2014
 *      Author: andreas
 */

#ifndef CCTLID_H_
#define CCTLID_H_

#include <inttypes.h>
#include <string>
#include <iostream>
#include <ios>

#include "rofl/common/logging.h"

namespace rofl {

class cctlid {

	uint64_t 	id;
	std::string s_id;

public:

	/**
	 *
	 */
	cctlid(
			uint64_t ctlid = 0) :
				id(ctlid) {
		std::stringstream sstr; sstr << id;
		s_id = sstr.str();
	};

	/**
	 *
	 */
	~cctlid() {};

	/**
	 *
	 */
	cctlid(
			cctlid const& ctlid) {
		*this = ctlid;
	};

	/**
	 *
	 */
	cctlid&
	operator= (
			cctlid const& ctlid) {
		if (this == &ctlid)
			return *this;
		id = ctlid.id;
		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			cctlid const& ctlid) const {
		return (id == ctlid.id);
	};

	/**
	 *
	 */
	bool
	operator!= (
			cctlid const& ctlid) const {
		return (id != ctlid.id);
	};

	/**
	 *
	 */
	bool
	operator< (
			cctlid const& ctlid) const {
		return (id < ctlid.id);
	};

public:

	/**
	 *
	 */
	const uint64_t&
	get_ctlid() const { return id; };

	/**
	 *
	 */
	const std::string&
	get_ctlid_s() const { return s_id; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cctlid& ctlid) {
		os << rofl::indent(0) << "<cctlid:" << std::hex << (unsigned long long)ctlid.id << std::dec
				<< "(" << ctlid.s_id << ")" << " >" << std::endl;
		return os;
	};
};

}; // end of namespace

#endif /* CCTLID_H_ */
