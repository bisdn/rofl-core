/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofgroupstats.h"
 
#ifndef htobe16
#include "../endian_conversion.h"
#endif


using namespace rofl::openflow;




cofgroup_stats_request::cofgroup_stats_request(
		uint8_t of_version) :
				of_version(of_version),
				group_id(0)
{}



cofgroup_stats_request::cofgroup_stats_request(
		uint8_t of_version,
		uint32_t group_id) :
				of_version(of_version),
				group_id(group_id)
{}



cofgroup_stats_request::~cofgroup_stats_request()
{}



cofgroup_stats_request::cofgroup_stats_request(
		cofgroup_stats_request const& stats_request)
{
	*this = stats_request;
}



cofgroup_stats_request&
cofgroup_stats_request::operator= (
		cofgroup_stats_request const& stats_request)
{
	if (this == &stats_request)
		return *this;

	of_version 	= stats_request.of_version;
	group_id	= stats_request.group_id;

	return *this;
}



void
cofgroup_stats_request::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	// no OpenFLow 1.0 group stats
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_group_stats_request))
			throw eInval();

		struct rofl::openflow12::ofp_group_stats_request *stats = (struct rofl::openflow12::ofp_group_stats_request*)buf;

		stats->group_id		= htobe32(group_id);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_group_stats_request))
			throw eInval();

		struct rofl::openflow13::ofp_group_stats_request *stats = (struct rofl::openflow13::ofp_group_stats_request*)buf;

		stats->group_id		= htobe32(group_id);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofgroup_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	// no OpenFLow 1.0 group stats
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_group_stats_request)){
			throw eInval();
		}

		struct rofl::openflow12::ofp_group_stats_request *stats = (struct rofl::openflow12::ofp_group_stats_request*)buf;

		group_id		= be32toh(stats->group_id);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_group_stats_request)){
			throw eInval();
		}

		struct rofl::openflow13::ofp_group_stats_request *stats = (struct rofl::openflow13::ofp_group_stats_request*)buf;

		group_id		= be32toh(stats->group_id);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofgroup_stats_request::length() const
{
	switch (of_version) {
	// no OpenFLow 1.0 group stats
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_group_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_group_stats_request));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



cofgroup_stats_reply::cofgroup_stats_reply(
		uint8_t of_version) :
				of_version(of_version),
				group_id(0),
				ref_count(0),
				packet_count(0),
				byte_count(0),
				duration_sec(0),
				duration_nsec(0),
				bucket_counters(of_version)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

	} break;
	default: {
		throw eBadVersion();
	};
	}
}



cofgroup_stats_reply::cofgroup_stats_reply(
		uint8_t of_version,
		uint32_t group_id,
		uint32_t ref_count,
		uint64_t packet_count,
		uint64_t byte_count,
		uint32_t duration_sec,
		uint32_t duration_nsec) :
				of_version(of_version),
				group_id(group_id),
				ref_count(ref_count),
				packet_count(packet_count),
				byte_count(byte_count),
				duration_sec(duration_sec),
				duration_nsec(duration_nsec),
				bucket_counters(of_version)
{}



cofgroup_stats_reply::~cofgroup_stats_reply()
{}



cofgroup_stats_reply::cofgroup_stats_reply(
		cofgroup_stats_reply const& stats_reply)
{
	*this = stats_reply;
}



cofgroup_stats_reply&
cofgroup_stats_reply::operator= (
		cofgroup_stats_reply const& stats_reply)
{
	if (this == &stats_reply)
		return *this;

	of_version 		= stats_reply.of_version;
	group_id		= stats_reply.group_id;
	ref_count		= stats_reply.ref_count;
	packet_count	= stats_reply.packet_count;
	byte_count		= stats_reply.byte_count;
	duration_sec	= stats_reply.duration_sec;
	duration_nsec	= stats_reply.duration_nsec;
	bucket_counters	= stats_reply.bucket_counters;

	return *this;
}



bool
cofgroup_stats_reply::operator== (
		cofgroup_stats_reply const& stats_reply)
{
	return ((of_version 		== stats_reply.of_version) &&
			(group_id 			== stats_reply.group_id) &&
			(ref_count 			== stats_reply.ref_count) &&
			(packet_count 		== stats_reply.packet_count) &&
			(byte_count 		== stats_reply.byte_count) &&
			(duration_sec 		== stats_reply.duration_sec) &&
			(duration_nsec 		== stats_reply.duration_nsec) &&
			(bucket_counters 	== stats_reply.bucket_counters));
}



size_t
cofgroup_stats_reply::length() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_group_stats) + bucket_counters.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_group_stats) + bucket_counters.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofgroup_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {

		struct rofl::openflow12::ofp_group_stats *stats = (struct rofl::openflow12::ofp_group_stats*)buf;

		stats->length		= htobe16(length());
		stats->group_id		= htobe32(group_id);
		stats->ref_count	= htobe32(ref_count);
		stats->packet_count	= htobe64(packet_count);
		stats->byte_count	= htobe64(byte_count);

		bucket_counters.pack((uint8_t*)(stats->bucket_stats), bucket_counters.length());

	} break;
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_group_stats *stats = (struct rofl::openflow13::ofp_group_stats*)buf;

		stats->length		= htobe16(length());
		stats->group_id		= htobe32(group_id);
		stats->ref_count	= htobe32(ref_count);
		stats->packet_count	= htobe64(packet_count);
		stats->byte_count	= htobe64(byte_count);
		stats->duration_sec	= htobe32(duration_sec);
		stats->duration_nsec= htobe32(duration_nsec);

		bucket_counters.pack((uint8_t*)(stats->bucket_stats), bucket_counters.length());

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofgroup_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	bucket_counters.clear();

	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {

		if (buflen < sizeof(struct rofl::openflow12::ofp_group_stats))
			throw eInval();

		struct rofl::openflow12::ofp_group_stats *stats = (struct rofl::openflow12::ofp_group_stats*)buf;

		if (be16toh(stats->length) > buflen)
			throw eInval();

		group_id		= be32toh(stats->group_id);
		ref_count		= be32toh(stats->ref_count);
		packet_count	= be64toh(stats->packet_count);
		byte_count		= be64toh(stats->byte_count);


		uint16_t length = be16toh(stats->length);

		buf += sizeof(struct rofl::openflow12::ofp_group_stats);
		buflen -= sizeof(struct rofl::openflow12::ofp_group_stats);

		uint32_t bucket_counter_id = 0;

		while (length >= sizeof(struct rofl::openflow12::ofp_bucket_counter)) {
			bucket_counters.add_bucket_counter(bucket_counter_id++).unpack(buf, sizeof(struct rofl::openflow12::ofp_bucket_counter));
			buf += sizeof(struct rofl::openflow12::ofp_bucket_counter);
			length -= sizeof(struct rofl::openflow12::ofp_bucket_counter);
		}

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_group_stats))
			throw eInval();

		struct rofl::openflow13::ofp_group_stats *stats = (struct rofl::openflow13::ofp_group_stats*)buf;

		if (be16toh(stats->length) > buflen)
			throw eInval();

		group_id		= be32toh(stats->group_id);
		ref_count		= be32toh(stats->ref_count);
		packet_count	= be64toh(stats->packet_count);
		byte_count		= be64toh(stats->byte_count);
		duration_sec	= be32toh(stats->duration_sec);
		duration_nsec	= be32toh(stats->duration_nsec);

		uint16_t length = be16toh(stats->length);

		buf += sizeof(struct rofl::openflow13::ofp_group_stats);
		buflen -= sizeof(struct rofl::openflow13::ofp_group_stats);

		uint32_t bucket_counter_id = 0;

		while (length >= sizeof(struct rofl::openflow13::ofp_bucket_counter)) {
			bucket_counters.add_bucket_counter(bucket_counter_id++).unpack(buf, sizeof(struct rofl::openflow13::ofp_bucket_counter));
			buf += sizeof(struct rofl::openflow13::ofp_bucket_counter);
			length -= sizeof(struct rofl::openflow13::ofp_bucket_counter);
		}

	} break;
	default:
		throw eBadVersion();
	}
}





