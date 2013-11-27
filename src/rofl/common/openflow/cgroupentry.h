/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CGROUPENTRY_H_
#define CGROUPENTRY_H_ 1

#include <string>
#include <vector>

#include "../ciosrv.h"
#include "../caddress.h"
#include "../cerror.h"
#include "../cmemory.h"
#include "../cvastring.h"

#include "cofmatch.h"
#include "cofbclist.h"
#include "cofinst.h"

namespace rofl
{

class eGroupEntryBase : public cerror {};
class eGroupEntryOutOfMem : public eGroupEntryBase {}; // out of memory



class cgroupentry :
		public ciosrv
{
private: // data structures

	cmemory group_mod_area;				// group mod memory area

public: // static methods

	static void test();

public: // data structures

	cofbclist buckets; 					// bucket list
	struct openflow12::ofp_group_mod *group_mod; 	// pointer to group_mod_area (see below)

public: // methods

	/** constructor
	 */
	cgroupentry();

	/** destructor
	 */
	virtual
	~cgroupentry();

	/** assignment operator
	 */
	cgroupentry& operator= (const cgroupentry& fe);

	/** reset groupentry
	 *
	 */
	void
	reset();

	/** dump flow entry info
	 */
	const char*
	c_str();

	/** pack groupentry, i.e. add actions to group_mod structure ready for transmission
	 * @return length of group_mod structure including buckets
	 */
	size_t
	pack();



public: // setter methods for ofp_group_mod structure

	/**
	 *
	 */
	uint16_t
	get_command() const;

	/**
	 *
	 */
	void
	set_command(
			uint16_t command);

	/**
	 *
	 */
	uint8_t
	get_type() const;

	/**
	 *
	 */
	void
	set_type(
			uint8_t type);

	/**
	 *
	 */
	uint32_t
	get_group_id() const;

	/**
	 *
	 */
	void
	set_group_id(
			uint32_t group_id);


public:

	friend std::ostream&
	operator<< (std::ostream& os, cgroupentry const& ge) {
		os << "<cgroupentry ";
			os << "cmd:";
			switch (ge.get_command()) {
			case OFPGC_ADD: 	os << "ADD "; 			break;
			case OFPGC_MODIFY:	os << "MODIFY ";	 	break;
			case OFPGC_DELETE:	os << "DELETE ";		break;
			default:			os << "UNKNOWN ";		break;
			}
			os << "type:";
			switch (ge.get_type()) {
			case OFPGT_ALL:		os << "ALL ";			break;
			case OFPGT_SELECT:	os << "SELECT "; 		break;
			case OFPGT_INDIRECT:os << "INDIRECT ";		break;
			case OFPGT_FF:		os << "FAST-FAILOVER"; 	break;
			default:			os << "UNKNOWN";		break;
			}
			os << "group-id:" << (int)ge.get_group_id() << " ";
			os << "buckets:" << std::endl;
			os << ge.buckets << std::endl;
		os << ">";
		return os;
	};
};

}; // end of namespace

#endif
