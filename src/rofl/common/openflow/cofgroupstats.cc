#include "rofl/common/openflow/cofgroupstats.h"

using namespace rofl;




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
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_group_stats))
			throw eInval();

		struct ofp12_group_stats *stats = (struct ofp12_group_stats*)buf;

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
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_group_stats))
			throw eInval();

		struct ofp12_group_stats *stats = (struct ofp12_group_stats*)buf;

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
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_group_stats_request));
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
				byte_count(0)
{}



cofgroup_stats_reply::cofgroup_stats_reply(
		uint8_t of_version,
		uint32_t group_id,
		uint32_t ref_count,
		uint64_t packet_count,
		uint64_t byte_count,
		unsigned int num_of_bucket_stats) :
				of_version(of_version),
				group_id(group_id),
				ref_count(ref_count),
				packet_count(packet_count),
				byte_count(byte_count),
				bucket_stats(num_of_bucket_stats * sizeof(struct ofp12_bucket_counter))
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
	bucket_stats	= stats_reply.bucket_stats;

	return *this;
}



void
cofgroup_stats_reply::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case OFP12_VERSION: {
		if (buflen < (sizeof(struct ofp12_group_stats) + bucket_stats.memlen()))
			throw eInval();

		struct ofp12_group_stats *stats = (struct ofp12_group_stats*)buf;

		stats->length		= htobe16(sizeof(struct ofp12_group_stats) + bucket_stats.memlen());
		stats->group_id		= htobe32(group_id);
		stats->ref_count	= htobe32(ref_count);
		stats->packet_count	= htobe64(packet_count);
		stats->byte_count	= htobe64(byte_count);

		for (unsigned int i = 0; i < (bucket_stats.memlen() / sizeof(struct ofp12_bucket_counter)); i++) {
			stats->bucket_stats[i].packet_count = htobe64(((struct ofp12_bucket_counter*)bucket_stats.somem())[i].packet_count);
			stats->bucket_stats[i].byte_count   = htobe64(((struct ofp12_bucket_counter*)bucket_stats.somem())[i].byte_count);
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
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_group_stats))
			throw eInval();

		struct ofp12_group_stats *stats = (struct ofp12_group_stats*)buf;

		if (be16toh(stats->length) > buflen)
			throw eInval();

		group_id		= be32toh(stats->group_id);
		ref_count		= be32toh(stats->ref_count);
		packet_count	= be64toh(stats->packet_count);
		byte_count		= be64toh(stats->byte_count);

		bucket_stats.resize(be16toh(stats->length) - sizeof(struct ofp12_group_stats));
		bucket_stats.clear();
		for (unsigned int i = 0; i < ((be16toh(stats->length) - sizeof(struct ofp12_group_stats)) / sizeof(struct ofp12_bucket_counter)); i++) {
			((struct ofp12_bucket_counter*)bucket_stats.somem())[0].packet_count = be32toh(stats->bucket_stats[i].packet_count);
			((struct ofp12_bucket_counter*)bucket_stats.somem())[0].byte_count   = be32toh(stats->bucket_stats[i].byte_count);
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
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_group_stats) + bucket_stats.memlen());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}




