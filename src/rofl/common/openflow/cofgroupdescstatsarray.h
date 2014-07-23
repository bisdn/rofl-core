/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofgroupdescs.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef COFGROUPDESCSTATSARRAY_H_
#define COFGROUPDESCSTATSARRAY_H_

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofgroupdescstats.h"
#include "rofl/common/logging.h"

namespace rofl {
namespace openflow {

class cofgroupdescstatsarray {

	uint8_t 										ofp_version;
	std::map<uint32_t, cofgroup_desc_stats_reply> 	array;

public:

	/**
	 *
	 */
	cofgroupdescstatsarray(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofgroupdescstatsarray();

	/**
	 *
	 */
	cofgroupdescstatsarray(cofgroupdescstatsarray const& groupdescs);

	/**
	 *
	 */
	cofgroupdescstatsarray&
	operator= (cofgroupdescstatsarray const& groupdescs);

	/**
	 *
	 */
	cofgroupdescstatsarray&
	operator+= (cofgroupdescstatsarray const& groupdescs);

public:

	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	size_t
	size() const { return array.size(); };

	/**
	 *
	 */
	void
	clear() { array.clear(); };

	/**
	 *
	 */
	std::map<uint32_t, cofgroup_desc_stats_reply> const&
	get_group_desc_stats() const { return array; };

	/**
	 *
	 */
	std::map<uint32_t, cofgroup_desc_stats_reply>&
	set_group_desc_stats() { return array; };

public:

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	add_group_desc_stats(uint32_t group_id);

	/**
	 *
	 */
	void
	drop_group_desc_stats(uint32_t group_id);

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	set_group_desc_stats(uint32_t group_id);

	/**
	 *
	 */
	cofgroup_desc_stats_reply const&
	get_group_desc_stats(uint32_t group_id) const;

	/**
	 *
	 */
	bool
	has_group_desc_stats(uint32_t group_id);

public:

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroupdescstatsarray const& groupdescstatsarray) {
		os << rofl::indent(0) << "<cofgroupdescstatsarray #groups:" << (int)groupdescstatsarray.array.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
				it = groupdescstatsarray.array.begin(); it != groupdescstatsarray.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}
};

}; // end of openflow
}; // end of rofl




#endif /* COFGROUPDESCS_H_ */
