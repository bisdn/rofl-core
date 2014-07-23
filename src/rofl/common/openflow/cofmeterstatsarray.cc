/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmeterstatsarray.cc
 *
 *  Created on: 27.05.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofmeterstatsarray.h"

using namespace rofl::openflow;


cofmeterstatsarray::cofmeterstatsarray(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{

}


cofmeterstatsarray::~cofmeterstatsarray()
{

}


cofmeterstatsarray::cofmeterstatsarray(
		const cofmeterstatsarray& mstats)
{
	*this = mstats;
}


cofmeterstatsarray&
cofmeterstatsarray::operator= (
		const cofmeterstatsarray& mstats)
{
	if (this == &mstats)
		return *this;

	this->array.clear();

	ofp_version = mstats.ofp_version;
	for (std::map<unsigned int, cofmeter_stats_reply>::const_iterator
			it = mstats.array.begin(); it != mstats.array.end(); ++it) {
		this->array[it->first] = it->second;
	}

	return *this;
}



bool
cofmeterstatsarray::operator== (
		const cofmeterstatsarray& mstats)
{
	if (ofp_version != mstats.ofp_version)
		return false;

	if (array.size() != mstats.array.size())
		return false;

	for (std::map<unsigned int, cofmeter_stats_reply>::const_iterator
				it = mstats.array.begin(); it != mstats.array.end(); ++it) {
		if (not (array[it->first] == it->second))
			return false;
	}

	return true;
}



cofmeterstatsarray&
cofmeterstatsarray::operator+= (
		const cofmeterstatsarray& mstats)
{
	unsigned int index = 0;

	for (std::map<unsigned int, cofmeter_stats_reply>::const_iterator
			it = mstats.array.begin(); it != mstats.array.end(); ++it) {
		while (array.find(index) != array.end()) {
			index++;
		}
		this->array[index] = it->second;
	}

	return *this;
}



size_t
cofmeterstatsarray::length() const
{
	size_t len = 0;
	for (std::map<unsigned int, cofmeter_stats_reply>::const_iterator
			it = array.begin(); it != array.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
cofmeterstatsarray::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<unsigned int, cofmeter_stats_reply>::iterator
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
cofmeterstatsarray::unpack(uint8_t *buf, size_t buflen)
{
	array.clear();

	if ((0 == buf) || (0 == buflen))
		return;

	unsigned int offset = 0;
	unsigned int index = 0;

	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {

		while ((buflen - offset) >= sizeof(struct rofl::openflow13::ofp_meter_stats)) {

			struct rofl::openflow13::ofp_meter_stats* mstats =
					(struct rofl::openflow13::ofp_meter_stats*)(buf + offset);

			uint16_t mstats_len = be16toh(mstats->len);

			if (mstats_len < sizeof(struct rofl::openflow13::ofp_meter_stats)) {
				throw eInval();
			}

			add_meter_stats(index++).unpack(buf, mstats_len);

			offset += mstats_len;
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofmeter_stats_reply&
cofmeterstatsarray::add_meter_stats(unsigned int index)
{
	if (array.find(index) != array.end()) {
		array.erase(index);
	}
	return (array[index] = cofmeter_stats_reply(ofp_version));
}



void
cofmeterstatsarray::drop_meter_stats(unsigned int index)
{
	if (array.find(index) == array.end()) {
		return;
	}
	array.erase(index);
}



cofmeter_stats_reply&
cofmeterstatsarray::set_meter_stats(unsigned int index)
{
	if (array.find(index) == array.end()) {
		array[index] = cofmeter_stats_reply(ofp_version);
	}
	return array[index];
}



cofmeter_stats_reply const&
cofmeterstatsarray::get_meter_stats(unsigned int index) const
{
	if (array.find(index) == array.end()) {
		throw eRofMeterStatsNotFound();
	}
	return array.at(index);
}



bool
cofmeterstatsarray::has_meter_stats(unsigned int index) const
{
	return (not (array.find(index) == array.end()));
}





