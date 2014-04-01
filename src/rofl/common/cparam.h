/*
 * cparam.h
 *
 *  Created on: 01.04.2014
 *      Author: andreas
 */

#ifndef CPARAM_H_
#define CPARAM_H_

#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>

#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"

namespace rofl {

class eParamBase		: public RoflException {};
class eParamInval		: public eParamBase {};
class eParamNotFound	: public eParamBase {};

class cparam {

	std::string		param;

public:

	/**
	 *
	 */
	cparam();

	/**
	 *
	 */
	virtual
	~cparam();

	/**
	 *
	 */
	cparam(
			std::string const& param);

	/**
	 *
	 */
	cparam(
			cparam const& param);

	/**
	 *
	 */
	cparam&
	operator= (
			cparam const& param);

	/**
	 *
	 */
	bool
	operator== (
			cparam const& param) const;

public:

	/**
	 *
	 */
	std::string&
	set_string() { return param; };

	/**
	 *
	 */
	std::string const&
	get_string() const { return param; };

	/**
	 *
	 */
	void
	set_int(int value);

	/**
	 *
	 */
	int
	get_int() const;

	/**
	 *
	 */
	void
	set_uint(unsigned int value);

	/**
	 *
	 */
	unsigned int
	get_uint() const;

	/**
	 *
	 */
	void
	set_bool(bool value);

	/**
	 *
	 */
	bool
	get_bool() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cparam const& param) {
		os << rofl::indent(0) << "<cparam param: \"" << param.param << "\" >" << std::endl;
		return os;
	};
};

}; // end of namespace rofl

#endif /* CPARAM_H_ */
