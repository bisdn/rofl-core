/*
 * cofflows.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef COFFLOWSTATSARRAY_H_
#define COFFLOWSTATSARRAY_H_

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofflowstats.h"

namespace rofl {
namespace openflow {

class cofflowstatsarray {

	uint8_t										ofp_version;
	std::map<uint32_t, cofflow_stats_reply>		array;

public:

	/**
	 *
	 */
	cofflowstatsarray(uint8_t ofp_version = OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofflowstatsarray();

	/**
	 *
	 */
	cofflowstatsarray(cofflowstatsarray const& flows);

	/**
	 *
	 */
	cofflowstatsarray&
	operator= (cofflowstatsarray const& flows);

	/**
	 *
	 */
	bool
	operator== (cofflowstatsarray const& flows);

	/**
	 *
	 */
	cofflowstatsarray&
	operator+= (cofflowstatsarray const& flows);

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

	/**
	 *
	 */
	std::map<uint32_t, cofflow_stats_reply> const&
	get_flow_stats() const { return array; };

	/**
	 *
	 */
	std::map<uint32_t, cofflow_stats_reply>&
	set_flow_stats() { return array; };

public:

	/**
	 *
	 */
	cofflow_stats_reply&
	add_flow_stats(uint32_t flow_id);

	/**
	 *
	 */
	void
	drop_flow_stats(uint32_t flow_id);

	/**
	 *
	 */
	cofflow_stats_reply&
	set_flow_stats(uint32_t flow_id);

	/**
	 *
	 */
	cofflow_stats_reply const&
	get_flow_stats(uint32_t flow_id) const;

	/**
	 *
	 */
	bool
	has_flow_stats(uint32_t flow_id);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofflowstatsarray const& flowstatsarray) {
		os << "<cofflowstatsarray #flows:" << (int)flowstatsarray.array.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint32_t, cofflow_stats_reply>::const_iterator
				it = flowstatsarray.array.begin(); it != flowstatsarray.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}
};

}; // end of openflow
}; // end of rofl



#endif /* COFGROUPS_H_ */
