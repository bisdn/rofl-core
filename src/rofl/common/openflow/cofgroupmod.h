/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFGROUPMOD_H_
#define COFGROUPMOD_H_ 1

#include <string>
#include <vector>

#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofbuckets.h"
#include "rofl/common/openflow/cofinstruction.h"

namespace rofl {
namespace openflow {

class eGroupEntryBase 		: public RoflException {};
class eGroupEntryOutOfMem 	: public eGroupEntryBase {};

class cofgroupmod
{
	uint8_t ofp_version;

public:

	union {
		uint8_t								*gmu_grp_mod;
		struct openflow12::ofp_group_mod 	*gmu12_grp_mod; 	// pointer to group_mod_area (see below)
		struct openflow13::ofp_group_mod	*gmu13_grp_mod;
	} ofgm_gmu;

#define grp_mod 		ofgm_gmu.gmu_grp_mod
#define of12_grp_mod	ofgm_gmu.gmu12_grp_mod
#define of13_grp_mod	ofgm_gmu.gmu13_grp_mod

private: // data structures

	cmemory group_mod_area;				// group mod memory area


public: // data structures

	cofbuckets buckets; 					// bucket list


public: // methods

	/**
	 * @brief	constructor
	 */
	cofgroupmod(uint8_t ofp_version);


	/**
	 * @brief	destructor
	 */
	virtual
	~cofgroupmod();


	/**
	 * @brief	assignment operator
	 */
	cofgroupmod&
	operator= (
			const cofgroupmod& fe);


	/**
	 * @brief	reset instance
	 */
	void
	reset();


	/**
	 *
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


	/**
	 *
	 */
	cofbuckets&
	get_buckets() { return buckets; };


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroupmod const& ge) {
		os << "<cgroupentry ";
			os << "cmd:";
			switch (ge.ofp_version) {
			case openflow12::OFP_VERSION: {
				switch (ge.get_command()) {
				case openflow12::OFPGC_ADD: 	os << "ADD "; 			break;
				case openflow12::OFPGC_MODIFY:	os << "MODIFY ";	 	break;
				case openflow12::OFPGC_DELETE:	os << "DELETE ";		break;
				default:						os << "UNKNOWN ";		break;
				}
				os << "type:";
				switch (ge.get_type()) {
				case openflow12::OFPGT_ALL:		os << "ALL ";			break;
				case openflow12::OFPGT_SELECT:	os << "SELECT "; 		break;
				case openflow12::OFPGT_INDIRECT:os << "INDIRECT ";		break;
				case openflow12::OFPGT_FF:		os << "FAST-FAILOVER"; 	break;
				default:						os << "UNKNOWN";		break;
				}
			} break;
			case openflow13::OFP_VERSION: {
				switch (ge.get_command()) {
				case openflow13::OFPGC_ADD: 	os << "ADD "; 			break;
				case openflow13::OFPGC_MODIFY:	os << "MODIFY ";	 	break;
				case openflow13::OFPGC_DELETE:	os << "DELETE ";		break;
				default:						os << "UNKNOWN ";		break;
				}
				os << "type:";
				switch (ge.get_type()) {
				case openflow13::OFPGT_ALL:		os << "ALL ";			break;
				case openflow13::OFPGT_SELECT:	os << "SELECT "; 		break;
				case openflow13::OFPGT_INDIRECT:os << "INDIRECT ";		break;
				case openflow13::OFPGT_FF:		os << "FAST-FAILOVER"; 	break;
				default:						os << "UNKNOWN";		break;
				}
			} break;
			default:
				throw eBadVersion();
			}
			os << "group-id:" << (int)ge.get_group_id() << " >" << std::endl;
			os << indent(2) << "<buckets: >" << std::endl;
			indent i(4);
			os << ge.buckets;
		return os;
	};
};

}; // end of namespace
}; // end of namespace

#endif
