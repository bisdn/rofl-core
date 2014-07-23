/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmeterbandstats.cc
 *
 *  Created on: 25.05.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofmeterbandstats.h"

using namespace rofl::openflow;

cofmeter_band_stats::cofmeter_band_stats(
		uint8_t of_version) :
				of_version(of_version),
				packet_band_count(0),
				byte_band_count(0)
{

}



cofmeter_band_stats::~cofmeter_band_stats()
{

}



cofmeter_band_stats::cofmeter_band_stats(
		const cofmeter_band_stats& mbs)
{
	*this = mbs;
}



const cofmeter_band_stats&
cofmeter_band_stats::operator= (
		const cofmeter_band_stats& mbs)
{
	if (this == &mbs)
		return *this;

	of_version			= mbs.of_version;
	packet_band_count	= mbs.packet_band_count;
	byte_band_count		= mbs.byte_band_count;

	return *this;
}



const bool
cofmeter_band_stats::operator== (
		const cofmeter_band_stats& mbs) const
{
	return ((packet_band_count == mbs.packet_band_count) && (byte_band_count == mbs.byte_band_count));
}



size_t
cofmeter_band_stats::length() const
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_meter_band_stats));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band_stats::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_band_stats* mbs =
				(struct rofl::openflow13::ofp_meter_band_stats*)buf;

		mbs->packet_band_count	= htobe64(packet_band_count);
		mbs->byte_band_count	= htobe64(byte_band_count);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band_stats::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_band_stats* mbs =
				(struct rofl::openflow13::ofp_meter_band_stats*)buf;

		packet_band_count 	= be64toh(mbs->packet_band_count);
		byte_band_count		= be64toh(mbs->byte_band_count);

	} break;
	default:
		throw eBadVersion();
	}
}


