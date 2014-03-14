/*
 * cofgroupdescs.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef COFGROUPDESCS_H_
#define COFGROUPDESCS_H_

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofgroupdescstats.h"
#include "rofl/common/logging.h"

namespace rofl {
namespace openflow {

class cofgroupdescs {

	uint8_t 										ofp_version;
	std::map<uint32_t, cofgroup_desc_stats_reply> 	groupdescs;

public:

	/**
	 *
	 */
	cofgroupdescs(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofgroupdescs();

	/**
	 *
	 */
	cofgroupdescs(cofgroupdescs const& groupdescs);

	/**
	 *
	 */
	cofgroupdescs&
	operator= (cofgroupdescs const& groupdescs);

	/**
	 *
	 */
	cofgroupdescs&
	operator+= (cofgroupdescs const& groupdescs);

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
	clear() { groupdescs.clear(); };

public:

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	add_group_desc(uint32_t group_id);

	/**
	 *
	 */
	void
	drop_group_desc(uint32_t group_id);

	/**
	 *
	 */
	cofgroup_desc_stats_reply&
	set_group_desc(uint32_t group_id);

	/**
	 *
	 */
	cofgroup_desc_stats_reply const&
	get_group_desc(uint32_t group_id);

	/**
	 *
	 */
	bool
	has_group_desc(uint32_t group_id);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofgroupdescs const& groupdescs) {

		return os;
	}
};

}; // end of openflow
}; // end of rofl




#endif /* COFGROUPDESCS_H_ */
