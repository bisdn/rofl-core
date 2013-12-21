#include "rofl/common/openflow/cofgroupdescstats.h"

using namespace rofl;




cofgroup_desc_stats_reply::cofgroup_desc_stats_reply(
		uint8_t of_version) :
				of_version(of_version),
				type(0),
				group_id(0)
{}



cofgroup_desc_stats_reply::cofgroup_desc_stats_reply(
		uint8_t of_version,
		uint8_t type,
		uint32_t group_id,
		cofbuckets const& buckets) :
				of_version(of_version),
				type(type),
				group_id(group_id),
				buckets(buckets)
{}



cofgroup_desc_stats_reply::~cofgroup_desc_stats_reply()
{}



cofgroup_desc_stats_reply::cofgroup_desc_stats_reply(
		cofgroup_desc_stats_reply const& stats_reply)
{
	*this = stats_reply;
}



cofgroup_desc_stats_reply&
cofgroup_desc_stats_reply::operator= (
		cofgroup_desc_stats_reply const& stats_reply)
{
	if (this == &stats_reply)
		return *this;

	of_version 		= stats_reply.of_version;
	type			= stats_reply.type;
	group_id		= stats_reply.group_id;
	buckets			= stats_reply.buckets;

	return *this;
}



void
cofgroup_desc_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow12::ofp_group_desc_stats) + buckets.length()))
			throw eInval();

		struct openflow12::ofp_group_desc_stats *stats = (struct openflow12::ofp_group_desc_stats*)buf;

		stats->length		= htobe16(sizeof(struct openflow12::ofp_group_desc_stats) + buckets.length());
		stats->type			= type;
		stats->group_id		= htobe32(group_id);

		buckets.pack((uint8_t*)(stats->buckets), buckets.length());

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofgroup_desc_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct openflow12::ofp_group_desc_stats))
			throw eInval();

		struct openflow12::ofp_group_desc_stats *stats = (struct openflow12::ofp_group_desc_stats*)buf;

		if (be16toh(stats->length) > buflen)
			throw eInval();

		group_id		= be32toh(stats->group_id);
		type			= stats->type;

		buckets.unpack((uint8_t*)stats->buckets, buflen - sizeof(struct openflow12::ofp_group_desc_stats));

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofgroup_desc_stats_reply::length() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_group_desc_stats) + buckets.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}




