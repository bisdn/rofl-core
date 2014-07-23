/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofflows.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofflowstatsarray.h"

using namespace rofl::openflow;


cofflowstatsarray::cofflowstatsarray(uint8_t ofp_version) :
		ofp_version(ofp_version)
{

}


cofflowstatsarray::~cofflowstatsarray()
{

}


cofflowstatsarray::cofflowstatsarray(cofflowstatsarray const& flows)
{
	*this = flows;
}


cofflowstatsarray&
cofflowstatsarray::operator= (cofflowstatsarray const& flows)
{
	if (this == &flows)
		return *this;

	this->array.clear();

	ofp_version = flows.ofp_version;
	for (std::map<uint32_t, cofflow_stats_reply>::const_iterator
			it = flows.array.begin(); it != flows.array.end(); ++it) {
		this->array[it->first] = it->second;
	}

	return *this;
}



bool
cofflowstatsarray::operator== (cofflowstatsarray const& flows)
{
	if (ofp_version != flows.ofp_version)
		return false;

	if (array.size() != flows.array.size())
		return false;

	for (std::map<uint32_t, cofflow_stats_reply>::const_iterator
			it = flows.array.begin(); it != flows.array.end(); ++it) {
		if (not (array.at(it->first) == it->second))
			return false;
	}

	return true;
}



cofflowstatsarray&
cofflowstatsarray::operator+= (cofflowstatsarray const& flows)
{
	uint32_t flow_id = 0;

	std::map<uint32_t, cofflow_stats_reply>::const_reverse_iterator it;
	if ((it = array.rbegin()) != array.rend()) {
		flow_id = it->first + 1;
	}

	/*
	 * this may replace existing flow descriptions
	 */
	for (std::map<uint32_t, cofflow_stats_reply>::const_iterator
			it = flows.array.begin(); it != flows.array.end(); ++it) {
		array[flow_id++] = it->second;
	}

	return *this;
}



size_t
cofflowstatsarray::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofflow_stats_reply>::const_iterator
			it = array.begin(); it != array.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
cofflowstatsarray::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<uint32_t, cofflow_stats_reply>::iterator
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
cofflowstatsarray::unpack(uint8_t *buf, size_t buflen)
{
	array.clear();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {

		uint32_t flow_id = 0;

		while (buflen >= sizeof(struct rofl::openflow10::ofp_flow_stats)) {

			add_flow_stats(flow_id++).unpack(buf, sizeof(struct rofl::openflow10::ofp_flow_stats));

			buf += sizeof(struct rofl::openflow10::ofp_flow_stats);
			buflen -= sizeof(struct rofl::openflow10::ofp_flow_stats);
		}
	} break;
	case rofl::openflow12::OFP_VERSION: {

		uint32_t flow_id = 0;

		while (buflen >= sizeof(struct rofl::openflow12::ofp_flow_stats)) {

			struct rofl::openflow12::ofp_flow_stats* flow_stats = (struct rofl::openflow12::ofp_flow_stats*)buf;

			uint16_t length = be16toh(flow_stats->length);

			if (length < sizeof(struct rofl::openflow12::ofp_flow_stats))
				throw eInval();

			add_flow_stats(flow_id++).unpack(buf, length);

			buf += length;
			buflen -= length;
		}

	} break;
	case rofl::openflow13::OFP_VERSION: {

		uint32_t flow_id = 0;

		while (buflen >= sizeof(struct rofl::openflow13::ofp_flow_stats)) {

			struct rofl::openflow13::ofp_flow_stats* flow_stats = (struct rofl::openflow13::ofp_flow_stats*)buf;

			uint16_t length = be16toh(flow_stats->length);

			if (length < sizeof(struct rofl::openflow13::ofp_flow_stats))
				throw eInval();

			add_flow_stats(flow_id++).unpack(buf, length);

			buf += length;
			buflen -= length;
		}

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofflow_stats_reply&
cofflowstatsarray::add_flow_stats(uint32_t flow_id)
{
	if (array.find(flow_id) != array.end()) {
		array.erase(flow_id);
	}
	return (array[flow_id] = cofflow_stats_reply(ofp_version));
}



void
cofflowstatsarray::drop_flow_stats(uint32_t flow_id)
{
	if (array.find(flow_id) == array.end()) {
		return;
	}
	array.erase(flow_id);
}



cofflow_stats_reply&
cofflowstatsarray::set_flow_stats(uint32_t flow_id)
{
	if (array.find(flow_id) == array.end()) {
		array[flow_id] = cofflow_stats_reply(ofp_version);
	}
	return array[flow_id];
}



cofflow_stats_reply const&
cofflowstatsarray::get_flow_stats(uint32_t flow_id) const
{
	if (array.find(flow_id) == array.end()) {
		throw eFlowStatsNotFound();
	}
	return array.at(flow_id);
}



bool
cofflowstatsarray::has_flow_stats(uint32_t flow_id)
{
	return (not (array.find(flow_id) == array.end()));
}





