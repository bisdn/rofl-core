/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofmeterfeatures.h"

using namespace rofl::openflow;


cofmeter_features_reply::cofmeter_features_reply(
		uint8_t of_version,
		uint8_t *buf,
		size_t buflen) :
				of_version(of_version),
				max_meter(0),
				band_types(0),
				capabilities(0),
				max_bands(0),
				max_color(0)
{
	if ((buflen > 0) && (0 != buf)) {
		unpack(buf, buflen);
	}
}



cofmeter_features_reply::cofmeter_features_reply(
		uint8_t of_version,
		uint32_t max_meter,
		uint32_t band_types,
		uint32_t capabilities,
		uint8_t max_bands,
		uint8_t max_color) :
				of_version(of_version),
				max_meter(max_meter),
				band_types(band_types),
				capabilities(capabilities),
				max_bands(max_bands),
				max_color(max_color)
{}



cofmeter_features_reply::~cofmeter_features_reply()
{}



cofmeter_features_reply::cofmeter_features_reply(
		const cofmeter_features_reply& meter_config)
{
	*this = meter_config;
}



cofmeter_features_reply&
cofmeter_features_reply::operator= (
		const cofmeter_features_reply& reply)
{
	if (this == &reply)
		return *this;

	of_version 		= reply.of_version;
	max_meter		= reply.max_meter;
	band_types		= reply.band_types;
	capabilities	= reply.capabilities;
	max_bands		= reply.max_bands;
	max_color		= reply.max_color;

	return *this;
}



bool
cofmeter_features_reply::operator== (
		cofmeter_features_reply const& reply)
{
	return ((of_version			== reply.of_version) &&
			(max_meter			== reply.max_meter) &&
			(band_types			== reply.band_types) &&
			(capabilities		== reply.capabilities) &&
			(max_bands			== reply.max_bands) &&
			(max_color			== reply.max_color));
}



void
cofmeter_features_reply::clear()
{
	max_meter		= 0;
	band_types		= 0;
	capabilities	= 0;
	max_bands		= 0;
	max_color		= 0;
}



size_t
cofmeter_features_reply::length() const
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_meter_features));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmeter_features_reply::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_features *meter_features =
				(struct rofl::openflow13::ofp_meter_features*)buf;

		meter_features->max_meter		= htobe32(max_meter);
		meter_features->band_types		= htobe32(band_types);
		meter_features->capabilities	= htobe32(capabilities);
		meter_features->max_bands		= max_bands;
		meter_features->max_color		= max_color;

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_features_reply::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_features* meter_features =
				(struct rofl::openflow13::ofp_meter_features*)buf;

		max_meter		= be32toh(meter_features->max_meter);
		band_types		= be32toh(meter_features->band_types);
		capabilities	= be32toh(meter_features->capabilities);
		max_bands		= meter_features->max_bands;
		max_color		= meter_features->max_color;

	} break;
	default:
		throw eBadVersion();
	}
}


