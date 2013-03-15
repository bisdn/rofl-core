/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFINSTRUCTIONLIST_H
#define COFINSTRUCTIONLIST_H 1

#include <algorithm>
#include "../cerror.h"
#include "../cvastring.h"
#include "../coflist.h"

#include "cofaction.h"
#include "cofinst.h"

namespace rofl
{

class eInListBase : public cerror {}; // base error class cofinlist
class eInListInval : public eInListBase {}; // invalid parameter
class eInListNotFound : public eInListBase {}; // element not found
class eInListOutOfRange : public eInListBase {}; // out of range



class cofinlist : public coflist<cofinst> {
public: // static methods

	/**
	 */
	static void test();

public: // methods

	/** constructor
	 */
	cofinlist();

	/** destructor
	 */
	virtual
	~cofinlist();

	/**
	 */
	cofinlist(cofinlist const& inlist)
	{
		*this = inlist;
	};

	/**
	 */
	cofinlist& operator= (cofinlist const& inlist)
	{
		if (this == &inlist)
			return *this;
		coflist<cofinst>::operator= (inlist);
		return *this;
	};

	/** stores cofinst instances in this->invec from a packed array struct ofp_instruction (e.g. in struct ofp_flow_mod)
	 */
	std::vector<cofinst>&			// returns reference to this->invec
	unpack(
		struct ofp_instruction *instructions, // parses memory area buckets and creates cofinst instance in this->invec
		size_t inlen)					// length of memory area to be parsed
	throw (eInstructionBadLen);

	/** builds an array of struct ofp_instruction from this->invec
	 */
	struct ofp_instruction*			// returns parameter "struct ofp_instruction *instructions"
	pack(
		struct ofp_instruction *instructions, // pointer to memory area for storing this->invec
		size_t inlen) 					// length of memory area
	throw (eInListInval);

	/** returns required length for array of struct ofp_instruction
	 * for all instructions defined in this->invec
	 */
	size_t
	length() const;

	/** dump info string
	 */
	const char*
	c_str();

	/** find a specific instruction
	 */
	cofinst&
	find_inst(
			enum ofp_instruction_type type)
	throw (eInListNotFound);

private:

	std::string info; // info string

};

}; // end of namespace

#endif
