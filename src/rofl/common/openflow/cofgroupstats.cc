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
		uint8_t of_version,
		unsigned int num_of_bucket_stats) :
				of_version(of_version),
				group_id(0),
				ref_count(0),
				packet_count(0),
				byte_count(0),
				duration_sec(0),
				duration_nsec(0),
				bucket_stats(num_of_bucket_stats * sizeof(struct rofl::openflow12::ofp_bucket_counter))
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
		uint32_t duration_nsec,
		unsigned int num_of_bucket_stats) :
				of_version(of_version),
				group_id(group_id),
				ref_count(ref_count),
				packet_count(packet_count),
				byte_count(byte_count),
				duration_sec(duration_sec),
				duration_nsec(duration_nsec),
				bucket_stats(num_of_bucket_stats * sizeof(struct rofl::openflow12::ofp_bucket_counter))
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
	bucket_stats	= stats_reply.bucket_stats;

	return *this;
}



void
cofgroup_stats_reply::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct rofl::openflow12::ofp_group_stats *stats = (struct rofl::openflow12::ofp_group_stats*)buf;

		stats->length		= htobe16(length());
		stats->group_id		= htobe32(group_id);
		stats->ref_count	= htobe32(ref_count);
		stats->packet_count	= htobe64(packet_count);
		stats->byte_count	= htobe64(byte_count);

		for (unsigned int i = 0; i < (bucket_stats.memlen() / sizeof(struct rofl::openflow12::ofp_bucket_counter)); i++) {
			stats->bucket_stats[i].packet_count = htobe64(((struct rofl::openflow12::ofp_bucket_counter*)bucket_stats.somem())[i].packet_count);
			stats->bucket_stats[i].byte_count   = htobe64(((struct rofl::openflow12::ofp_bucket_counter*)bucket_stats.somem())[i].byte_count);
		}

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

		for (unsigned int i = 0; i < (bucket_stats.memlen() / sizeof(struct rofl::openflow13::ofp_bucket_counter)); i++) {
			stats->bucket_stats[i].packet_count = htobe64(((struct rofl::openflow13::ofp_bucket_counter*)bucket_stats.somem())[i].packet_count);
			stats->bucket_stats[i].byte_count   = htobe64(((struct rofl::openflow13::ofp_bucket_counter*)bucket_stats.somem())[i].byte_count);
		}

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofgroup_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
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

		bucket_stats.resize(be16toh(stats->length) - sizeof(struct rofl::openflow12::ofp_group_stats));
		bucket_stats.clear();
		for (unsigned int i = 0; i < ((be16toh(stats->length) - sizeof(struct rofl::openflow12::ofp_group_stats)) / sizeof(struct rofl::openflow12::ofp_bucket_counter)); i++) {
			((struct rofl::openflow12::ofp_bucket_counter*)bucket_stats.somem())[i].packet_count = be32toh(stats->bucket_stats[i].packet_count);
			((struct rofl::openflow12::ofp_bucket_counter*)bucket_stats.somem())[i].byte_count   = be32toh(stats->bucket_stats[i].byte_count);
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

		bucket_stats.resize(be16toh(stats->length) - sizeof(struct rofl::openflow13::ofp_group_stats));
		bucket_stats.clear();
		for (unsigned int i = 0; i < ((be16toh(stats->length) - sizeof(struct rofl::openflow13::ofp_group_stats)) / sizeof(struct rofl::openflow13::ofp_bucket_counter)); i++) {
			((struct rofl::openflow13::ofp_bucket_counter*)bucket_stats.somem())[i].packet_count = be32toh(stats->bucket_stats[i].packet_count);
			((struct rofl::openflow13::ofp_bucket_counter*)bucket_stats.somem())[i].byte_count   = be32toh(stats->bucket_stats[i].byte_count);
		}

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofgroup_stats_reply::length() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_group_stats) + bucket_stats.memlen());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_group_stats) + bucket_stats.memlen());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}




