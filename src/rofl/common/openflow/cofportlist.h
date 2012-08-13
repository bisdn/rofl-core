/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFPORTLIST_H
#define COFPORTLIST_H 1

#include <algorithm>

#include "../cvastring.h"
#include "../cerror.h"
#include "../coflist.h"

#include "cofaction.h"
#include "cofport.h"

class ePortListBase : public cerror {}; // base error class cofinlist
class ePortListInval : public ePortListBase {}; // invalid parameter
class ePortListNotFound : public ePortListBase {}; // element not found
class ePortListOutOfRange : public ePortListBase {}; // out of range



class cofportlist : public coflist<cofport> {
public: // static methods

	/**
	 */
	static void test();

public: // methods

	/** constructor
	 */
	cofportlist();

	/** destructor
	 */
	virtual
	~cofportlist();

	/**
	 */
	cofportlist(cofportlist const& portlist)
	{
		*this = portlist;
	};

	/**
	 */
	cofportlist& operator= (cofportlist const& portlist)
	{
		if (this == &portlist)
			return *this;
		coflist<cofport>::operator= (portlist);
		return *this;
	};

	/** stores cofinst instances in this->invec from a packed array struct ofp_instruction (e.g. in struct ofp_flow_mod)
	 */
	std::vector<cofport>&			// returns reference to this->invec
	unpack(
		struct ofp_port *ports, // parses memory area buckets and creates cofinst instance in this->invec
		size_t portlen)					// length of memory area to be parsed
	throw (ePortListInval);

	/** builds an array of struct ofp_instruction from this->invec
	 */
	struct ofp_port*			// returns parameter "struct ofp_instruction *instructions"
	pack(
		struct ofp_port *ports, // pointer to memory area for storing this->invec
		size_t portlen) 					// length of memory area
	throw (ePortListInval);

	/** returns required length for array of struct ofp_instruction
	 * for all instructions defined in this->invec
	 */
	size_t
	length();

	/** dump info string
	 */
	const char*
	c_str();

	/** find a specific instruction
	 */
	cofport&
	find_port(
			uint32_t port_no)
	throw (ePortListNotFound);

private:

	std::string info; // info string

};

#endif
