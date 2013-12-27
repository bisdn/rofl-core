/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CLLDPLIST_H
#define CLLDPLIST_H 1

#include <algorithm>

#include "../cerror.h"
#include "../cvastring.h"
#include "../coflist.h"

#include "clldptlv.h"

#include "clldptlv.h"

namespace rofl
{

class eLLDPlistBase : public RoflException {}; // base error class cofinlist
class eLLDPlistInval : public eLLDPlistBase {}; // invalid parameter
class eLLDPlistNotFound : public eLLDPlistBase {}; // element not found
class eLLDPlistOutOfRange : public eLLDPlistBase {}; // out of range
class eLLDPBadLen : public eLLDPlistBase {}; // bad length


class clldplist : public coflist<clldptlv> {
public: // static methods

	static void test();

public: // methods

	/** constructor
	 */
	clldplist();

	/** destructor
	 */
	virtual
	~clldplist();

	/** stores cofinst instances in this->invec from a packed array struct ofp_instruction (e.g. in struct ofp_flow_mod)
	 */
	std::vector<clldptlv>&				// returns reference to this->invec
	unpack(
		uint8_t* tlvs, // parses memory area buckets and creates cofinst instance in this->invec
		int tlvlen) 					// length of memory area to be parsed
	throw (eLLDPBadLen);

	/** builds an array of struct ofp_instruction from this->invec
	 */
	uint8_t*	// returns parameter "struct ofp_instruction *instructions"
	pack(
		uint8_t* tlvs, // pointer to memory area for storing this->invec
		size_t tlvlen) 					// length of memory area
	throw (eLLDPlistInval);

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
	clldptlv&
	find_lldp_tlv(int type)
	throw (eLLDPlistNotFound);


private:

	std::string info; // info string

public: // auxiliary classes

	class clldptlv_find_by_type {
		int type;
	public:
		clldptlv_find_by_type(int __type) : type(__type) {};
		bool operator() (clldptlv const& tlv) {
			return (tlv.get_hdr_type() == type);
		};
	};

};

}; // end of namespace

#endif
