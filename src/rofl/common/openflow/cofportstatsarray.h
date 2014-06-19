/*
 * cofports.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef COFPORTSTATSARRAY_H_
#define COFPORTSTATSARRAY_H_

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofportstats.h"

namespace rofl {
namespace openflow {

class cofportstatsarray {

	uint8_t										ofp_version;
	std::map<uint32_t, cofport_stats_reply>		array;

public:

	/**
	 *
	 */
	cofportstatsarray(uint8_t ofp_version = OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofportstatsarray();

	/**
	 *
	 */
	cofportstatsarray(cofportstatsarray const& ports);

	/**
	 *
	 */
	cofportstatsarray&
	operator= (cofportstatsarray const& ports);

	/**
	 *
	 */
	bool
	operator== (cofportstatsarray const& ports);

	/**
	 *
	 */
	cofportstatsarray&
	operator+= (cofportstatsarray const& ports);

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
	std::map<uint32_t, cofport_stats_reply> const&
	get_port_stats() const { return array; };

	/**
	 *
	 */
	std::map<uint32_t, cofport_stats_reply>&
	set_port_stats() { return array; };

public:

	/**
	 *
	 */
	cofport_stats_reply&
	add_port_stats(uint32_t port_id);

	/**
	 *
	 */
	void
	drop_port_stats(uint32_t port_id);

	/**
	 *
	 */
	cofport_stats_reply&
	set_port_stats(uint32_t port_id);

	/**
	 *
	 */
	cofport_stats_reply const&
	get_port_stats(uint32_t port_id) const;

	/**
	 *
	 */
	bool
	has_port_stats(uint32_t port_id);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofportstatsarray const& portstatsarray) {
		os << rofl::indent(0) << "<cofportstatsarray #ports:" << (int)portstatsarray.array.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint32_t, cofport_stats_reply>::const_iterator
				it = portstatsarray.array.begin(); it != portstatsarray.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}
};

}; // end of openflow
}; // end of rofl



#endif /* COFGROUPS_H_ */
