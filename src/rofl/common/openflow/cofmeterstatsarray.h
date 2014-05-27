/*
 * cofmeterstatsarray.h
 *
 *  Created on: 27.05.2014
 *      Author: andreas
 */

#ifndef COFMETERSTATSARRAY_H_
#define COFMETERSTATSARRAY_H_

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofmeterstats.h"

namespace rofl {
namespace openflow {

class cofmeterstatsarray
{
public:

	/**
	 *
	 */
	cofmeterstatsarray(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofmeterstatsarray();

	/**
	 *
	 */
	cofmeterstatsarray(
			const cofmeterstatsarray& ports);

	/**
	 *
	 */
	cofmeterstatsarray&
	operator= (
			const cofmeterstatsarray& ports);

	/**
	 *
	 */
	bool
	operator== (
			const cofmeterstatsarray& ports);

	/**
	 *
	 */
	cofmeterstatsarray&
	operator+= (
			const cofmeterstatsarray& ports);

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
	set_version(uint8_t ofp_version) {
		this->ofp_version = ofp_version;
		for (std::map<unsigned int, cofmeter_stats_reply>::iterator
				it = array.begin(); it != array.end(); ++it) {
			it->second.set_version(ofp_version);
		}
	};

	/**
	 *
	 */
	const std::map<unsigned int, cofmeter_stats_reply>&
	get_mstats() const { return array; };

	/**
	 *
	 */
	std::map<unsigned int, cofmeter_stats_reply>&
	set_mstats() { return array; };

public:

	/**
	 *
	 */
	cofmeter_stats_reply&
	add_meter_stats(
			unsigned int index = 0);

	/**
	 *
	 */
	void
	drop_meter_stats(
			unsigned int index = 0);

	/**
	 *
	 */
	cofmeter_stats_reply&
	set_meter_stats(
			unsigned int index = 0);

	/**
	 *
	 */
	const cofmeter_stats_reply&
	get_meter_stats(
			unsigned int index = 0) const;

	/**
	 *
	 */
	bool
	has_meter_stats(
			unsigned int index = 0) const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmeterstatsarray const& mstats) {
		os << "<cofmeterstatsarray #meter-stats:" << (int)mstats.array.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<unsigned int, cofmeter_stats_reply>::const_iterator
				it = mstats.array.begin(); it != mstats.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}

private:

	uint8_t											ofp_version;
	std::map<unsigned int, cofmeter_stats_reply>	array;
};

}; // end of openflow
}; // end of rofl



#endif /* COFMETERSTATSARRAY_H_ */
