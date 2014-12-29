/*
 * cdptid.h
 *
 *  Created on: 19.04.2014
 *      Author: andreas
 */

#ifndef CDPTID_H_
#define CDPTID_H_

#include <inttypes.h>
#include <string>
#include <iostream>
#include <ios>

#include "rofl/common/logging.h"

namespace rofl {

/**
 * @brief	rofl-common's internal datapath handle
 * @ingroup common_devel_workflow
 */
class cdptid {

	uint64_t 	id;
	std::string s_id;

public:

	/**
	 *
	 */
	cdptid(
			uint64_t dptid = 0) :
				id(dptid) {
		std::stringstream sstr; sstr << id;
		s_id = sstr.str();
	};

	/**
	 *
	 */
	~cdptid() {};

	/**
	 *
	 */
	cdptid(
			cdptid const& dptid) {
		*this = dptid;
	};

	/**
	 *
	 */
	cdptid&
	operator= (
			cdptid const& dptid) {
		if (this == &dptid)
			return *this;
		id = dptid.id;
		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			cdptid const& dptid) const {
		return (id == dptid.id);
	};

	/**
	 *
	 */
	bool
	operator!= (
			cdptid const& dptid) const {
		return (id != dptid.id);
	};

	/**
	 *
	 */
	bool
	operator< (
			cdptid const& dptid) const {
		return (id < dptid.id);
	};

public:

	/**
	 *
	 */
	const uint64_t&
	get_dptid() const { return id; };

	/**
	 *
	 */
	const std::string&
	get_dptid_s() const { return s_id; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cdptid& dptid) {
		os << rofl::indent(0) << "<cdptid: " << std::hex << (unsigned long long)dptid.id << std::dec
				<< " (" << dptid.s_id << ")" << " >" << std::endl;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << id;
		return ss.str();
	};
};

}; // end of namespace

#endif /* CDPTID_H_ */
