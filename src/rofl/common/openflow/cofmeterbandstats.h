/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmeterbandstats.h
 *
 *  Created on: 25.05.2014
 *      Author: andreas
 */

#ifndef COFMETERBANDSTATS_H_
#define COFMETERBANDSTATS_H_

#include <inttypes.h>
#include <iostream>

#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/logging.h"

namespace rofl {
namespace openflow {

class eRofMeterBandStatsBase		: public RoflException {};
class eRofMeterBandStatsNotFound	: public eRofMeterBandStatsBase {};

class cofmeter_band_stats {
public:

	/**
	 *
	 */
	cofmeter_band_stats(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN);


	/**
	 *
	 */
	~cofmeter_band_stats();


	/**
	 *
	 */
	cofmeter_band_stats(
			const cofmeter_band_stats& mbs);


	/**
	 *
	 */
	const cofmeter_band_stats&
	operator= (
			const cofmeter_band_stats& mbs);

	/**
	 *
	 */
	const bool
	operator== (
			const cofmeter_band_stats& mbs) const;

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
	pack(uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(uint8_t *buf, size_t buflen);


public:

	/**
	 *
	 */
	void
	set_version(uint8_t of_version) { this->of_version = of_version; };

	/**
	 *
	 */
	uint8_t
	get_version() const { return of_version; };

	/**
	 *
	 */
	void
	set_packet_band_count(uint64_t packet_band_count) {
		this->packet_band_count = packet_band_count;
	};

	/**
	 *
	 */
	uint64_t
	get_packet_band_count() const { return packet_band_count; };

	/**
	 *
	 */
	void
	set_byte_band_count(uint64_t byte_band_count) {
		this->byte_band_count = byte_band_count;
	};

	/**
	 *
	 */
	uint64_t
	get_byte_band_count() const { return byte_band_count; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmeter_band_stats& mbs) {
		os << rofl::indent(0) << "<cofmeter_band_stats "
				<< std::hex
				<< "packet_band_count: 0x" << mbs.get_packet_band_count() << " "
				<< "byte_band_count: 0x" << mbs.get_byte_band_count() << " "
				<< ">"
				<< std::dec << std::endl;
		return os;
	};

private:

	uint8_t			of_version;
	uint64_t		packet_band_count;
	uint64_t		byte_band_count;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFMETERBANDSTATS_H_ */
