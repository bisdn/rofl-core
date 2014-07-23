/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmeterbandstatsarray.h
 *
 *  Created on: 25.05.2014
 *      Author: andreas
 */

#ifndef COFMETERBANDSTATSARRAY_H_
#define COFMETERBANDSTATSARRAY_H_

#include <inttypes.h>
#include <iostream>
#include <map>

#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofmeterbandstats.h"
#include "rofl/common/logging.h"

namespace rofl {
namespace openflow {

class cofmeter_band_stats_array {
public:

	/**
	 *
	 */
	cofmeter_band_stats_array(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	~cofmeter_band_stats_array();

	/**
	 *
	 */
	cofmeter_band_stats_array(
			const cofmeter_band_stats_array& array);

	/**
	 *
	 */
	cofmeter_band_stats_array&
	operator= (
			const cofmeter_band_stats_array& array);

	/**
	 *
	 */
	const bool
	operator== (
			const cofmeter_band_stats_array& array) const;

public:

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	void
	set_version(uint8_t of_version) {
		this->of_version = of_version;
		for (std::map<unsigned int, cofmeter_band_stats>::iterator
				it = array.begin(); it != array.end(); ++it) {
			it->second.set_version(of_version);
		}
	};

	/**
	 *
	 */
	uint8_t
	get_version() const { return of_version; };

	/**
	 *
	 */
	std::map<unsigned int, cofmeter_band_stats>&
	set_mbs_array() { return array; };

	/**
	 *
	 */
	const std::map<unsigned int, cofmeter_band_stats>&
	get_mbs_array() const { return array; };

	/**
	 *
	 */
	cofmeter_band_stats&
	add_mbstats(unsigned int index = 0);

	/**
	 *
	 */
	cofmeter_band_stats&
	set_mbstats(unsigned int index = 0);

	/**
	 *
	 */
	void
	drop_mbstats(unsigned int index = 0);

	/**
	 *
	 */
	const cofmeter_band_stats&
	get_mbstats(unsigned int index = 0) const;

	/**
	 *
	 */
	const bool
	has_mbstats(unsigned int index = 0) const;

public:

	/**
	 *
	 */
	size_t
	length() const;

	/**
	 *
	 */
	void
	pack(uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmeter_band_stats_array& array) {
		os << rofl::indent(0) << "<cofmeter_band_stats_array #stats: "
				<< array.get_mbs_array().size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<unsigned int, cofmeter_band_stats>::const_iterator
				it = array.get_mbs_array().begin(); it != array.get_mbs_array().end(); ++it) {
			os << it->second;
		}
		return os;
	};

private:

	uint8_t 									of_version;
	std::map<unsigned int, cofmeter_band_stats> array;

};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFMETERBANDSTATSARRAY_H_ */
