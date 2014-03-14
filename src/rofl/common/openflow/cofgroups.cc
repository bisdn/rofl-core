/*
 * cofgroups.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofgroups.h"

using namespace rofl::openflow;


cofgroups::cofgroups(uint8_t ofp_version) :
		ofp_version(ofp_version)
{

}


cofgroups::~cofgroups()
{

}


cofgroups::cofgroups(cofgroups const& groups)
{
	*this = groups;
}


cofgroups&
cofgroups::operator= (cofgroups const& groups)
{
	if (this == &groups)
		return *this;

	this->groups.clear();

	ofp_version = groups.ofp_version;
	for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
			it = groups.groups.begin(); it != groups.groups.end(); ++it) {
		this->groups[it->first] = it->second;
	}

	return *this;
}



cofgroups&
cofgroups::operator+= (cofgroups const& groups)
{
	/*
	 * this may replace existing group descriptions
	 */
	for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
			it = groups.groups.begin(); it != groups.groups.end(); ++it) {
		this->groups[it->first] = it->second;
	}

	return *this;
}



size_t
cofgroups::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
			it = groups.begin(); it != groups.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
cofgroups::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<uint32_t, cofgroup_stats_reply>::iterator
				it = groups.begin(); it != groups.end(); ++it) {
			it->second.pack(buf, it->second.length());
			buf += it->second.length();
		}

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofgroups::unpack(uint8_t *buf, size_t buflen)
{
	groups.clear();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow12::ofp_group_stats)) {

			size_t length = be16toh(((struct rofl::openflow12::ofp_group_stats*)buf)->length);

			if (length == 0)
				throw eInval();

			uint32_t group_id = be32toh(((struct rofl::openflow12::ofp_group_stats*)buf)->group_id);

			add_group(group_id).unpack(buf, length);
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



rofl::cofgroup_stats_reply&
cofgroups::add_group(uint32_t group_id)
{
	if (groups.find(group_id) != groups.end()) {
		groups.erase(group_id);
	}
	return (groups[group_id] = cofgroup_stats_reply(ofp_version));
}



void
cofgroups::drop_group(uint32_t group_id)
{
	if (groups.find(group_id) == groups.end()) {
		return;
	}
	groups.erase(group_id);
}



rofl::cofgroup_stats_reply&
cofgroups::set_group(uint32_t group_id)
{
	return (groups[group_id] = cofgroup_stats_reply(ofp_version));
}



rofl::cofgroup_stats_reply const&
cofgroups::get_group(uint32_t group_id)
{
	if (groups.find(group_id) == groups.end()) {
		throw;
	}
	return groups.at(group_id);
}



bool
cofgroups::has_group(uint32_t group_id)
{
	return (not (groups.find(group_id) == groups.end()));
}





