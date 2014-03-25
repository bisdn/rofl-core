/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CPPPOELIST_H
#define CPPPOELIST_H 1

#include <algorithm>

#include "../croflexception.h"
#include "../fframe.h"
#include "../coflist.h"
#include "../cvastring.h"

#include "cpppoetlv.h"

namespace rofl
{

class ePPPoElistBase : public RoflException {}; // base error class cofinlist
class ePPPoElistInval : public ePPPoElistBase {}; // invalid parameter
class ePPPoElistNotFound : public ePPPoElistBase {}; // element not found
class ePPPoElistOutOfRange : public ePPPoElistBase {}; // out of range
class ePPPoEBadLen : public ePPPoElistBase {}; // bad length



class cpppoelist : public rofl::openflow::coflist<cpppoetlv> {
public: // static methods

	static void test();

public: // methods

	/** constructor
	 */
	cpppoelist();

	/** destructor
	 */
	virtual
	~cpppoelist();

	/** stores cofinst instances in this->invec from a packed array struct ofp_instruction (e.g. in struct ofp_flow_mod)
	 */
	std::vector<cpppoetlv>&				// returns reference to this->invec
	unpack(
		uint8_t* tlvs, // parses memory area buckets and creates cofinst instance in this->invec
		int tlvlen) 					// length of memory area to be parsed
	throw (ePPPoEBadLen);

	/** builds an array of struct ofp_instruction from this->invec
	 */
	uint8_t*	// returns parameter "struct ofp_instruction *instructions"
	pack(
		uint8_t* tlvs, // pointer to memory area for storing this->invec
		size_t tlvlen) 					// length of memory area
	throw (ePPPoElistInval);

	/** returns required length for array of struct ofp_instruction
	 * for all instructions defined in this->invec
	 */
	size_t
	length();

	/** find a specific instruction
	 */
	cpppoetlv&
	find_pppoe_tlv(int type)
	throw (ePPPoElistNotFound);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cpppoelist const& pppoelist) {
		os << "<cpppoelist ";
			os << "TLVs: " << std::endl << dynamic_cast<coflist const&>( pppoelist );
			os << std::endl;
		os << ">";
		return os;
	};

public: // auxiliary classes

	class cpppoetlv_find_by_type {
		int type;
	public:
		cpppoetlv_find_by_type(int __type) : type(__type) {};
		bool operator() (cpppoetlv const& tlv) {
			return (tlv.get_hdr_type() == type);
		};
	};

};

}; // end of namespace

#endif
