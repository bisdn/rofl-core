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
	std::map<uint32_t, cofgroup_desc_stats_reply> 	groupdescstatsarray;

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
	void
	clear() { groupdescstatsarray.clear(); };

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
	get_group_desc_stats(uint32_t group_id);

	/**
	 *
	 */
	bool
	has_group_desc_stats(uint32_t group_id);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroupdescstatsarray const& groupdescstatsarray) {
		os << rofl::indent(0) << "<cofgroupdescstatsarray #groups:" << (int)groupdescstatsarray.groupdescstatsarray.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
				it = groupdescstatsarray.groupdescstatsarray.begin(); it != groupdescstatsarray.groupdescstatsarray.end(); ++it) {
			os << it->second;
		}
		return os;
	}
};

}; // end of openflow
}; // end of rofl




#endif /* COFGROUPDESCS_H_ */
