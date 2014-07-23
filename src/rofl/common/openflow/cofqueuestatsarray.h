/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofqueues.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef COFQUEUESTATSARRAY_H_
#define COFQUEUESTATSARRAY_H_

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofqueuestats.h"

namespace rofl {
namespace openflow {

class cofqueuestatsarray {

	uint8_t															ofp_version;
	std::map<uint32_t, std::map<uint32_t, cofqueue_stats_reply>	>	array;

public:

	/**
	 *
	 */
	cofqueuestatsarray(uint8_t ofp_version = OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofqueuestatsarray();

	/**
	 *
	 */
	cofqueuestatsarray(cofqueuestatsarray const& groups);

	/**
	 *
	 */
	cofqueuestatsarray&
	operator= (cofqueuestatsarray const& groups);

	/**
	 *
	 */
	bool
	operator== (cofqueuestatsarray const& groups);

	/**
	 *
	 */
	cofqueuestatsarray&
	operator+= (cofqueuestatsarray const& groups);

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
	std::map<uint32_t, std::map<uint32_t, cofqueue_stats_reply>	> const&
	get_queue_stats() const { return array; };

	/**
	 *
	 */
	std::map<uint32_t, std::map<uint32_t, cofqueue_stats_reply>	>&
	set_queue_stats() { return array; };

public:

	/**
	 *
	 */
	cofqueue_stats_reply&
	add_queue_stats(uint32_t port_no, uint32_t queue_id);

	/**
	 *
	 */
	void
	drop_queue_stats(uint32_t port_no, uint32_t queue_id);

	/**
	 *
	 */
	cofqueue_stats_reply&
	set_queue_stats(uint32_t port_no, uint32_t queue_id);

	/**
	 *
	 */
	cofqueue_stats_reply const&
	get_queue_stats(uint32_t port_no, uint32_t queue_id) const;

	/**
	 *
	 */
	bool
	has_queue_stats(uint32_t port_no, uint32_t queue_id);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofqueuestatsarray const& groupstatsarray) {
		os << "<cofqueuestatsarray #ports:" << (int)groupstatsarray.array.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint32_t, std::map<uint32_t, cofqueue_stats_reply> >::const_iterator
				jt = groupstatsarray.array.begin(); jt != groupstatsarray.array.end(); ++jt) {
			os << rofl::indent(0) << "<portno: " << (int)jt->first << " #queues: " << (int)jt->second.size() << " >" << std::endl;
			rofl::indent j(2);
			for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator
					it = jt->second.begin(); it != jt->second.end(); ++it) {
				os << it->second;
			}
		}
		return os;
	}
};

}; // end of openflow
}; // end of rofl



#endif /* COFGROUPS_H_ */
