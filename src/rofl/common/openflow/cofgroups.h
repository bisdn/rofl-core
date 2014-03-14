/*
 * cofgroups.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef COFGROUPS_H_
#define COFGROUPS_H_

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofgroupstats.h"

namespace rofl {
namespace openflow {

class cofgroups {

	uint8_t											ofp_version;
	std::map<uint32_t, cofgroup_stats_reply>		groups;

public:

	/**
	 *
	 */
	cofgroups(uint8_t ofp_version = OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofgroups();

	/**
	 *
	 */
	cofgroups(cofgroups const& groups);

	/**
	 *
	 */
	cofgroups&
	operator= (cofgroups const& groups);

	/**
	 *
	 */
	cofgroups&
	operator+= (cofgroups const& groups);

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
	clear() { groups.clear(); };

public:

	/**
	 *
	 */
	cofgroup_stats_reply&
	add_group(uint32_t group_id);

	/**
	 *
	 */
	void
	drop_group(uint32_t group_id);

	/**
	 *
	 */
	cofgroup_stats_reply&
	set_group(uint32_t group_id);

	/**
	 *
	 */
	cofgroup_stats_reply const&
	get_group(uint32_t group_id);

	/**
	 *
	 */
	bool
	has_group(uint32_t group_id);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroups const& groups) {
		os << "<cofgroups #groups:" << (int)groups.groups.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
				it = groups.groups.begin(); it != groups.groups.end(); ++it) {
			os << it->second;
		}
		return os;
	}
};

}; // end of openflow
}; // end of rofl



#endif /* COFGROUPS_H_ */
