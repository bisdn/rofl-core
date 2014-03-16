/*
 * cofgroups.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofgroupstatsarray.h"

using namespace rofl::openflow;


cofgroupstatsarray::cofgroupstatsarray(uint8_t ofp_version) :
		ofp_version(ofp_version)
{

}


cofgroupstatsarray::~cofgroupstatsarray()
{

}


cofgroupstatsarray::cofgroupstatsarray(cofgroupstatsarray const& groups)
{
	*this = groups;
}


cofgroupstatsarray&
cofgroupstatsarray::operator= (cofgroupstatsarray const& groups)
{
	if (this == &groups)
		return *this;

	this->array.clear();

	ofp_version = groups.ofp_version;
	for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
			it = groups.array.begin(); it != groups.array.end(); ++it) {
		this->array[it->first] = it->second;
	}

	return *this;
}



cofgroupstatsarray&
cofgroupstatsarray::operator+= (cofgroupstatsarray const& groups)
{
	/*
	 * this may replace existing group descriptions
	 */
	for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
			it = groups.array.begin(); it != groups.array.end(); ++it) {
		this->array[it->first] = it->second;
	}

	return *this;
}



size_t
cofgroupstatsarray::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
			it = array.begin(); it != array.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
cofgroupstatsarray::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<uint32_t, cofgroup_stats_reply>::iterator
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
cofgroupstatsarray::unpack(uint8_t *buf, size_t buflen)
{
	array.clear();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow12::ofp_group_stats)) {

			size_t length = be16toh(((struct rofl::openflow12::ofp_group_stats*)buf)->length);

			if (length == 0)
				throw eInval();

			uint32_t group_id = be32toh(((struct rofl::openflow12::ofp_group_stats*)buf)->group_id);

			add_group_stats(group_id).unpack(buf, length);

			buf += length;
			buflen -= length;
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow13::ofp_group_stats)) {

			size_t length = be16toh(((struct rofl::openflow13::ofp_group_stats*)buf)->length);

			if (length == 0)
				throw eInval();

			uint32_t group_id = be32toh(((struct rofl::openflow13::ofp_group_stats*)buf)->group_id);

			add_group_stats(group_id).unpack(buf, length);

			buf += length;
			buflen -= length;
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofgroup_stats_reply&
cofgroupstatsarray::add_group_stats(uint32_t group_id)
{
	if (array.find(group_id) != array.end()) {
		array.erase(group_id);
	}
	return (array[group_id] = cofgroup_stats_reply(ofp_version));
}



void
cofgroupstatsarray::drop_group_stats(uint32_t group_id)
{
	if (array.find(group_id) == array.end()) {
		return;
	}
	array.erase(group_id);
}



cofgroup_stats_reply&
cofgroupstatsarray::set_group_stats(uint32_t group_id)
{
	return (array[group_id] = cofgroup_stats_reply(ofp_version));
}



cofgroup_stats_reply const&
cofgroupstatsarray::get_group_stats(uint32_t group_id)
{
	if (array.find(group_id) == array.end()) {
		throw;
	}
	return array.at(group_id);
}



bool
cofgroupstatsarray::has_group_stats(uint32_t group_id)
{
	return (not (array.find(group_id) == array.end()));
}





