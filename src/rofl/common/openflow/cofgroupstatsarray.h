/*
 * cofgroups.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef COFGROUPSTATSARRAY_H_
#define COFGROUPSTATSARRAY_H_

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofgroupstats.h"

namespace rofl {
namespace openflow {

class cofgroupstatsarray {

	uint8_t											ofp_version;
	std::map<uint32_t, cofgroup_stats_reply>		array;

public:

	/**
	 *
	 */
	cofgroupstatsarray(uint8_t ofp_version = OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofgroupstatsarray();

	/**
	 *
	 */
	cofgroupstatsarray(cofgroupstatsarray const& groups);

	/**
	 *
	 */
	cofgroupstatsarray&
	operator= (cofgroupstatsarray const& groups);

	/**
	 *
	 */
	bool
	operator== (cofgroupstatsarray const& groups);

	/**
	 *
	 */
	cofgroupstatsarray&
	operator+= (cofgroupstatsarray const& groups);

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
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

public:

	/**
	 *
	 */
	cofgroup_stats_reply&
	add_group_stats(uint32_t group_id);

	/**
	 *
	 */
	void
	drop_group_stats(uint32_t group_id);

	/**
	 *
	 */
	cofgroup_stats_reply&
	set_group_stats(uint32_t group_id);

	/**
	 *
	 */
	cofgroup_stats_reply const&
	get_group_stats(uint32_t group_id) const;

	/**
	 *
	 */
	bool
	has_group_stats(uint32_t group_id);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroupstatsarray const& groupstatsarray) {
		os << "<cofgroupstatsarray #groups:" << (int)groupstatsarray.array.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
				it = groupstatsarray.array.begin(); it != groupstatsarray.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}
};

}; // end of openflow
}; // end of rofl



#endif /* COFGROUPS_H_ */
