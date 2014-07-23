/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

	this->array.clear();

	ofp_version = groupdescs.ofp_version;
	for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
			it = groupdescs.array.begin(); it != groupdescs.array.end(); ++it) {
		this->array[it->first] = it->second;
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
			it = groupdescs.array.begin(); it != groupdescs.array.end(); ++it) {
		this->array[it->first] = it->second;
	}

	return *this;
}



size_t
cofgroupdescstatsarray::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofgroup_desc_stats_reply>::const_iterator
			it = array.begin(); it != array.end(); ++it) {
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
				it = array.begin(); it != array.end(); ++it) {
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
	array.clear();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow12::ofp_group_desc_stats)) {

			size_t length = be16toh(((struct rofl::openflow12::ofp_group_desc_stats*)buf)->length);

			if (length < sizeof(struct rofl::openflow12::ofp_group_desc_stats))
				throw eInval();

			uint32_t group_id = be32toh(((struct rofl::openflow12::ofp_group_desc_stats*)buf)->group_id);

			add_group_desc_stats(group_id).unpack(buf, length);

			buf += length;
			buflen -= length;
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow13::ofp_group_desc)) {

			size_t length = be16toh(((struct rofl::openflow13::ofp_group_desc*)buf)->length);

			if (length < sizeof(struct rofl::openflow13::ofp_group_desc))
				throw eInval();

			uint32_t group_id = be32toh(((struct rofl::openflow13::ofp_group_desc*)buf)->group_id);

			add_group_desc_stats(group_id).unpack(buf, length);

			buf += length;
			buflen -= length;
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofgroup_desc_stats_reply&
cofgroupdescstatsarray::add_group_desc_stats(uint32_t group_id)
{
	if (array.find(group_id) != array.end()) {
		array.erase(group_id);
	}
	return (array[group_id] = cofgroup_desc_stats_reply(ofp_version));
}



void
cofgroupdescstatsarray::drop_group_desc_stats(uint32_t group_id)
{
	if (array.find(group_id) == array.end()) {
		return;
	}
	array.erase(group_id);
}



cofgroup_desc_stats_reply&
cofgroupdescstatsarray::set_group_desc_stats(uint32_t group_id)
{
	if (array.find(group_id) == array.end()) {
		array[group_id] = cofgroup_desc_stats_reply(ofp_version);
	}
	return array[group_id];
}



cofgroup_desc_stats_reply const&
cofgroupdescstatsarray::get_group_desc_stats(uint32_t group_id) const
{
	if (array.find(group_id) == array.end()) {
		throw eGroupDescStatsNotFound();
	}
	return array.at(group_id);
}



bool
cofgroupdescstatsarray::has_group_desc_stats(uint32_t group_id)
{
	return (not (array.find(group_id) == array.end()));
}


