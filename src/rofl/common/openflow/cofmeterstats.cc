/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofmeterstats.h"

using namespace rofl::openflow;

cofmeter_stats_request::cofmeter_stats_request(
		uint8_t of_version,
		uint8_t *buf,
		size_t buflen) :
				of_version(of_version),
				meter_id(0)
{
	if ((buflen > 0) && (0 != buf)) {
		unpack(buf, buflen);
	}
}



cofmeter_stats_request::cofmeter_stats_request(
		uint8_t of_version,
		uint32_t meter_id) :
				of_version(of_version),
				meter_id(meter_id)
{}



cofmeter_stats_request::~cofmeter_stats_request()
{}



cofmeter_stats_request::cofmeter_stats_request(
		const cofmeter_stats_request& request)
{
	*this = request;
}



cofmeter_stats_request&
cofmeter_stats_request::operator= (
		const cofmeter_stats_request& request)
{
	if (this == &request)
		return *this;

	of_version 	= request.of_version;
	meter_id	= request.meter_id;

	return *this;
}



const bool
cofmeter_stats_request::operator== (
		const cofmeter_stats_request& meter_stats_request) const
{
	return (meter_id == meter_stats_request.meter_id);
}



size_t
cofmeter_stats_request::length() const
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		return sizeof(struct rofl::openflow13::ofp_meter_multipart_request);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_stats_request::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_multipart_request *req =
				(struct rofl::openflow13::ofp_meter_multipart_request*)buf;

		req->meter_id	= htobe32(meter_id);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_multipart_request *req =
				(struct rofl::openflow13::ofp_meter_multipart_request*)buf;

		meter_id	= be32toh(req->meter_id);

	} break;
	default:
		throw eBadVersion();
	}
}



cofmeter_stats_reply::cofmeter_stats_reply(
		uint8_t of_version,
		uint8_t *buf,
		size_t buflen) :
				of_version(of_version),
				meter_id(0),
				flow_count(0),
				packet_in_count(0),
				byte_in_count(0),
				duration_sec(0),
				duration_nsec(0),
				mbstats(of_version)
{
	if ((buflen > 0) && (0 != buf)) {
		unpack(buf, buflen);
	}
}



cofmeter_stats_reply::cofmeter_stats_reply(
		uint8_t of_version,
		uint32_t meter_id,
		uint32_t flow_count,
		uint64_t packet_in_count,
		uint64_t byte_in_count,
		uint32_t duration_sec,
		uint32_t duration_nsec,
		const rofl::openflow::cofmeter_band_stats_array& mbstats) :
				of_version(of_version),
				meter_id(meter_id),
				flow_count(flow_count),
				packet_in_count(packet_in_count),
				byte_in_count(byte_in_count),
				duration_sec(duration_sec),
				duration_nsec(duration_nsec)
{}



cofmeter_stats_reply::~cofmeter_stats_reply()
{}



cofmeter_stats_reply::cofmeter_stats_reply(
		const cofmeter_stats_reply& meter_stats)
{
	*this = meter_stats;
}



cofmeter_stats_reply&
cofmeter_stats_reply::operator= (
		const cofmeter_stats_reply& reply)
{
	if (this == &reply)
		return *this;

	of_version 		= reply.of_version;
	meter_id		= reply.meter_id;
	flow_count		= reply.flow_count;
	packet_in_count	= reply.packet_in_count;
	byte_in_count	= reply.byte_in_count;
	duration_sec	= reply.duration_sec;
	duration_nsec	= reply.duration_nsec;
	mbstats			= reply.mbstats;

	return *this;
}



bool
cofmeter_stats_reply::operator== (
		cofmeter_stats_reply const& reply)
{
	return ((of_version			== reply.of_version) &&
			(meter_id			== reply.meter_id) &&
			(flow_count			== reply.flow_count) &&
			(packet_in_count	== reply.packet_in_count) &&
			(byte_in_count		== reply.byte_in_count) &&
			(duration_sec 		== reply.duration_sec) &&
			(duration_nsec 		== reply.duration_nsec) &&
			(mbstats			== reply.mbstats));
}



void
cofmeter_stats_reply::clear()
{
	meter_id		= 0;
	flow_count		= 0;
	packet_in_count	= 0;
	byte_in_count	= 0;
	duration_sec 	= 0;
	duration_nsec 	= 0;
	mbstats.clear();
}



size_t
cofmeter_stats_reply::length() const
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_meter_stats) + mbstats.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmeter_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_stats *meter_stats =
				(struct rofl::openflow13::ofp_meter_stats*)buf;

		meter_stats->meter_id			= htobe32(meter_id);
		meter_stats->len				= htobe16(length());
		meter_stats->flow_count			= htobe32(flow_count);
		meter_stats->packet_in_count	= htobe64(packet_in_count);
		meter_stats->byte_in_count		= htobe64(byte_in_count);
		meter_stats->duration_sec		= htobe32(duration_sec);
		meter_stats->duration_nsec		= htobe32(duration_nsec);

		mbstats.pack((uint8_t*)(meter_stats->band_stats), mbstats.length());

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		mbstats.clear();

		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_meter_stats* meter_stats =
				(struct rofl::openflow13::ofp_meter_stats*)buf;

		if (be16toh(meter_stats->len) < sizeof(struct rofl::openflow13::ofp_meter_stats)) {
			throw eInval();
		}

		meter_id		= be32toh(meter_stats->meter_id);
		flow_count		= be32toh(meter_stats->flow_count);
		packet_in_count	= be64toh(meter_stats->packet_in_count);
		byte_in_count	= be64toh(meter_stats->byte_in_count);
		duration_sec	= be32toh(meter_stats->duration_sec);
		duration_nsec	= be32toh(meter_stats->duration_nsec);


		uint16_t mbstats_len = be16toh(meter_stats->len) - sizeof(struct rofl::openflow13::ofp_meter_stats);

		if (mbstats_len > 0) {
			mbstats.unpack((uint8_t*)(meter_stats->band_stats), mbstats_len);
		}

	} break;
	default:
		throw eBadVersion();
	}
}


