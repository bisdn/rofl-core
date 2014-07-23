/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coftables.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef COFTABLESTATSARRAY_H_
#define COFTABLESTATSARRAY_H_

#include <iostream>
#include <map>

#include "rofl/common/openflow/coftablestats.h"

namespace rofl {
namespace openflow {

class coftablestatsarray {

	uint8_t										ofp_version;
	std::map<uint8_t, coftable_stats_reply>		array;

public:

	/**
	 *
	 */
	coftablestatsarray(uint8_t ofp_version = OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~coftablestatsarray();

	/**
	 *
	 */
	coftablestatsarray(coftablestatsarray const& tables);

	/**
	 *
	 */
	coftablestatsarray&
	operator= (coftablestatsarray const& tables);

	/**
	 *
	 */
	bool
	operator== (coftablestatsarray const& tables) const;

	/**
	 *
	 */
	coftablestatsarray&
	operator+= (coftablestatsarray const& tables);

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
	std::map<uint8_t, coftable_stats_reply> const&
	get_table_stats() const { return array; };

	/**
	 *
	 */
	std::map<uint8_t, coftable_stats_reply>&
	set_table_stats() { return array; };

public:

	/**
	 *
	 */
	coftable_stats_reply&
	add_table_stats(uint8_t table_id);

	/**
	 *
	 */
	void
	drop_table_stats(uint8_t table_id);

	/**
	 *
	 */
	coftable_stats_reply&
	set_table_stats(uint8_t table_id);

	/**
	 *
	 */
	coftable_stats_reply const&
	get_table_stats(uint8_t table_id) const;

	/**
	 *
	 */
	bool
	has_table_stats(uint8_t table_id);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftablestatsarray const& tablestatsarray) {
		os << rofl::indent(0) << "<coftablestatsarray #tables:" << (int)tablestatsarray.array.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint8_t, coftable_stats_reply>::const_iterator
				it = tablestatsarray.array.begin(); it != tablestatsarray.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}
};

}; // end of openflow
}; // end of rofl



#endif /* COFGROUPS_H_ */
