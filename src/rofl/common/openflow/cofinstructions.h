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



class cofinstructions : public coflist<cofinst>
{
public:

	uint8_t ofp_version;

	/**
	 */
	static void test();

public: // methods

	/** constructor
	 */
	cofinstructions(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN);

	/** destructor
	 */
	virtual
	~cofinstructions();

	/**
	 */
	cofinstructions(
			cofinstructions const& inlist);

	/**
	 */
	cofinstructions&
	operator= (
			cofinstructions const& inlist);

	/** stores cofinst instances in this->invec from a packed array struct ofp_instruction (e.g. in struct ofp_flow_mod)
	 */
	std::vector<cofinst>&			// returns reference to this->invec
	unpack(
			uint8_t *instructions, // parses memory area buckets and creates cofinst instance in this->invec
			size_t inlen);					// length of memory area to be parsed


	/** builds an array of struct ofp_instruction from this->invec
	 */
	uint8_t*			// returns parameter "struct ofp_instruction *instructions"
	pack(
			uint8_t *instructions, // pointer to memory area for storing this->invec
			size_t inlen); 					// length of memory area


	/** returns required length for array of struct ofp_instruction
	 * for all instructions defined in this->invec
	 */
	size_t
	length() const;

	/** find a specific instruction
	 */
	cofinst&
	find_inst(
			uint8_t type)
	throw (eInListNotFound);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofinstructions const& inlist) {
		os << "<cofinlist ";
			os << "ofp-version:" << (int)inlist.ofp_version << " ";
			os << std::endl;
			for (cofinstructions::const_iterator
					it = inlist.elems.begin(); it != inlist.elems.end(); ++it) {
				os << (*it) << std::endl;
			}
		os << ">";
		return os;
	};
};

}; // end of namespace

#endif
