/*
 * cofgroupdescs.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofgroupdescs.h"

using namespace rofl::openflow;

cofgroupdescs::cofgroupdescs(uint8_t ofp_version) :
		ofp_version(ofp_version)
{

}


cofgroupdescs::~cofgroupdescs()
{

}


cofgroupdescs::cofgroupdescs(cofgroupdescs const& groupdescs)
{
	*this = groupdescs;
}


cofgroupdescs&
cofgroupdescs::operator= (cofgroupdescs const& groupdescs)
{
	if (this == &groupdescs)
		return *this;

	this->groupdescs.clear();

	ofp_version = groupdescs.ofp_version;
	for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
			it = groupdescs.groupdescs.begin(); it != groupdescs.groupdescs.end(); ++it) {
		this->groupdescs[it->first] = it->second;
	}

	return *this;
}



cofgroupdescs&
cofgroupdescs::operator+= (cofgroupdescs const& groupdescs)
{
	/*
	 * this may replace existing group descriptions
	 */
	for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
			it = groupdescs.groupdescs.begin(); it != groupdescs.groupdescs.end(); ++it) {
		this->groupdescs[it->first] = it->second;
	}

	return *this;
}



size_t
cofgroupdescs::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
			it = groupdescs.begin(); it != groupdescs.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
cofgroupdescs::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<uint32_t, cofgroup_desc_stats_reply>::iterator
				it = groupdescs.begin(); it != groupdescs.end(); ++it) {
			it->second.pack(buf, it->second.length());
			buf += it->second.length();
		}

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofgroupdescs::unpack(uint8_t *buf, size_t buflen)
{
	groupdescs.clear();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow12::ofp_group_desc_stats)) {

			size_t length = be16toh(((struct rofl::openflow12::ofp_group_desc_stats*)buf)->length);

			if (length == 0)
				throw eInval();

			uint32_t group_id = be32toh(((struct rofl::openflow12::ofp_group_desc_stats*)buf)->group_id);

			add_group_desc(group_id).unpack(buf, length);
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



rofl::cofgroup_desc_stats_reply&
cofgroupdescs::add_group_desc(uint32_t group_id)
{
	if (groupdescs.find(group_id) != groupdescs.end()) {
		groupdescs.erase(group_id);
	}
	return (groupdescs[group_id] = cofgroup_desc_stats_reply(ofp_version));
}



void
cofgroupdescs::drop_group_desc(uint32_t group_id)
{
	if (groupdescs.find(group_id) == groupdescs.end()) {
		return;
	}
	groupdescs.erase(group_id);
}



rofl::cofgroup_desc_stats_reply&
cofgroupdescs::set_group_desc(uint32_t group_id)
{
	return (groupdescs[group_id] = cofgroup_desc_stats_reply(ofp_version));
}



rofl::cofgroup_desc_stats_reply const&
cofgroupdescs::get_group_desc(uint32_t group_id)
{
	if (groupdescs.find(group_id) == groupdescs.end()) {
		throw;
	}
	return groupdescs.at(group_id);
}



bool
cofgroupdescs::has_group_desc(uint32_t group_id)
{
	return (not (groupdescs.find(group_id) == groupdescs.end()));
}


