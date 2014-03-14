/*
 * cofgroupdescs.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofgroupdescstatsarray.h"

using namespace rofl::openflow;

cofgroupdescstatsarray::cofgroupdescstatsarray(uint8_t ofp_version) :
		ofp_version(ofp_version)
{

}


cofgroupdescstatsarray::~cofgroupdescstatsarray()
{

}


cofgroupdescstatsarray::cofgroupdescstatsarray(cofgroupdescstatsarray const& groupdescs)
{
	*this = groupdescs;
}


cofgroupdescstatsarray&
cofgroupdescstatsarray::operator= (cofgroupdescstatsarray const& groupdescs)
{
	if (this == &groupdescs)
		return *this;

	this->groupdescstatsarray.clear();

	ofp_version = groupdescs.ofp_version;
	for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
			it = groupdescs.groupdescstatsarray.begin(); it != groupdescs.groupdescstatsarray.end(); ++it) {
		this->groupdescstatsarray[it->first] = it->second;
	}

	return *this;
}



cofgroupdescstatsarray&
cofgroupdescstatsarray::operator+= (cofgroupdescstatsarray const& groupdescs)
{
	/*
	 * this may replace existing group descriptions
	 */
	for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
			it = groupdescs.groupdescstatsarray.begin(); it != groupdescs.groupdescstatsarray.end(); ++it) {
		this->groupdescstatsarray[it->first] = it->second;
	}

	return *this;
}



size_t
cofgroupdescstatsarray::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
			it = groupdescstatsarray.begin(); it != groupdescstatsarray.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
cofgroupdescstatsarray::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<uint32_t, cofgroup_desc_stats_reply>::iterator
				it = groupdescstatsarray.begin(); it != groupdescstatsarray.end(); ++it) {
			it->second.pack(buf, it->second.length());
			buf += it->second.length();
		}

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofgroupdescstatsarray::unpack(uint8_t *buf, size_t buflen)
{
	groupdescstatsarray.clear();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow12::ofp_group_desc_stats)) {

			size_t length = be16toh(((struct rofl::openflow12::ofp_group_desc_stats*)buf)->length);

			if (length == 0)
				throw eInval();

			uint32_t group_id = be32toh(((struct rofl::openflow12::ofp_group_desc_stats*)buf)->group_id);

			add_group_desc_stats(group_id).unpack(buf, length);

			buf += length;
			buflen -= length;
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



rofl::cofgroup_desc_stats_reply&
cofgroupdescstatsarray::add_group_desc_stats(uint32_t group_id)
{
	if (groupdescstatsarray.find(group_id) != groupdescstatsarray.end()) {
		groupdescstatsarray.erase(group_id);
	}
	return (groupdescstatsarray[group_id] = cofgroup_desc_stats_reply(ofp_version));
}



void
cofgroupdescstatsarray::drop_group_desc_stats(uint32_t group_id)
{
	if (groupdescstatsarray.find(group_id) == groupdescstatsarray.end()) {
		return;
	}
	groupdescstatsarray.erase(group_id);
}



rofl::cofgroup_desc_stats_reply&
cofgroupdescstatsarray::set_group_desc_stats(uint32_t group_id)
{
	return (groupdescstatsarray[group_id] = cofgroup_desc_stats_reply(ofp_version));
}



rofl::cofgroup_desc_stats_reply const&
cofgroupdescstatsarray::get_group_desc_stats(uint32_t group_id)
{
	if (groupdescstatsarray.find(group_id) == groupdescstatsarray.end()) {
		throw;
	}
	return groupdescstatsarray.at(group_id);
}



bool
cofgroupdescstatsarray::has_group_desc_stats(uint32_t group_id)
{
	return (not (groupdescstatsarray.find(group_id) == groupdescstatsarray.end()));
}


