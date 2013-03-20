#include "rofl/common/openflow/cofgroupfeaturesstats.h"

using namespace rofl;



cofgroup_features_stats_reply::cofgroup_features_stats_reply(
		uint8_t of_version,
		uint8_t *buf,
		size_t buflen) :
				of_version(of_version),
				types(0),
				capabilities(0)
{
	if ((buflen > 0) && (0 != buf)) {
		unpack(buf, buflen);
	}
}



cofgroup_features_stats_reply::cofgroup_features_stats_reply(
		uint8_t of_version,
		uint32_t types,
		uint32_t capabilities,
		std::vector<uint32_t> const& max_groups,
		std::vector<uint32_t> const& actions) :
				of_version(of_version),
				types(types),
				capabilities(capabilities),
				max_groups(max_groups),
				actions(actions)
{
	if (max_groups.size() < 4)
		throw eInval();
	if (actions.size() < 4)
		throw eInval();
}



cofgroup_features_stats_reply::~cofgroup_features_stats_reply()
{}



cofgroup_features_stats_reply::cofgroup_features_stats_reply(
		cofgroup_features_stats_reply const& flowstats)
{
	*this = flowstats;
}



cofgroup_features_stats_reply&
cofgroup_features_stats_reply::operator= (
		cofgroup_features_stats_reply const& fs)
{
	if (this == &fs)
		return *this;

	of_version 		= fs.of_version;
	types			= fs.types;
	capabilities	= fs.capabilities;
	max_groups		= fs.max_groups;
	actions			= fs.actions;

	return *this;
}



void
cofgroup_features_stats_reply::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		if (buflen < length())
			throw eInval();
		struct ofp12_group_features_stats *fs = (struct ofp12_group_features_stats*)buf;
		fs->types				= htobe32(types);
		fs->capabilities		= htobe32(capabilities);
		for (unsigned int i = 0; i < 4; i++) {
			fs->max_groups[i] 	= htobe32(max_groups[i]);
			fs->actions[i] 		= htobe32(actions[i]);
		}
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofgroup_features_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		if (buflen < sizeof(struct ofp12_group_features_stats_reply))
			throw eInval();

		struct ofp12_group_features_stats* fs = (struct ofp12_group_features_stats*)buf;
		types				= be32toh(fs->types);
		capabilities		= be32toh(fs->capabilities);
		for (unsigned int i = 0; i < 4; i++) {
			max_groups[i] 	= be32toh(fs->max_groups[i]);
			actions[i]		= be32toh(fs->actions[i]);
		}
	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofgroup_features_stats_reply::length() const
{
	switch (of_version) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_group_features_stats));
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_group_features_stats));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



uint8_t
cofgroup_features_stats_reply::get_version() const
{
	return of_version;
}



void
cofgroup_features_stats_reply::set_version(uint8_t of_version)
{
	this->of_version = of_version;
}



uint32_t
cofgroup_features_stats_reply::get_types() const
{
	return types;
}



void
cofgroup_features_stats_reply::set_types(uint32_t types)
{
	this->types = types;
}



uint32_t
cofgroup_features_stats_reply::get_capabilities() const
{
	return capabilities;
}



void
cofgroup_features_stats_reply::set_capabilities(uint32_t capabilities)
{
	this->capabilities = capabilities;
}



std::vector<uint32_t>&
cofgroup_features_stats_reply::get_max_groups()
{
	return max_groups;
}



std::vector<uint32_t>&
cofgroup_features_stats_reply::get_actions()
{
	return actions;
}


