/*
 * cofmeterbandstatsarray.cc
 *
 *  Created on: 25.05.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofmeterbandstatsarray.h"

using namespace rofl::openflow;


cofmeter_band_stats_array::	cofmeter_band_stats_array(
		uint8_t of_version) :
				of_version(of_version)
{

}



cofmeter_band_stats_array::	~cofmeter_band_stats_array()
{

}



cofmeter_band_stats_array::cofmeter_band_stats_array(
			const cofmeter_band_stats_array& array)
{
	*this = array;
}



cofmeter_band_stats_array&
cofmeter_band_stats_array::operator= (
		const cofmeter_band_stats_array& mbs)
{
	if (this == &mbs)
		return *this;

	clear();

	of_version		= mbs.of_version;
	for (std::map<unsigned int, cofmeter_band_stats>::const_iterator
			it = mbs.array.begin(); it != mbs.array.end(); ++it) {
		array[it->first] = it->second;
	}

	return *this;
}



void
cofmeter_band_stats_array::clear()
{
	array.clear();
}



cofmeter_band_stats&
cofmeter_band_stats_array::add_mbstats(unsigned int index)
{
	if (array.find(index) != array.end()) {
		array.erase(index);
	}
	array[index] = cofmeter_band_stats(of_version);
	return array[index];
}



cofmeter_band_stats&
cofmeter_band_stats_array::set_mbstats(unsigned int index)
{
	if (array.find(index) == array.end()) {
		array[index] = cofmeter_band_stats(of_version);
	}
	return array[index];
}



void
cofmeter_band_stats_array::drop_mbstats(unsigned int index)
{
	if (array.find(index) == array.end()) {
		return;
	}
	array.erase(index);
}



const cofmeter_band_stats&
cofmeter_band_stats_array::get_mbstats(unsigned int index) const
{
	if (array.find(index) == array.end()) {
		throw eRofMeterBandStatsNotFound();
	}
	return array.at(index);
}



const bool
cofmeter_band_stats_array::has_mbstats(unsigned int index) const
{
	return (not (array.find(index) == array.end()));
}



size_t
cofmeter_band_stats_array::length() const
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		size_t len = 0;
		for (std::map<unsigned int, cofmeter_band_stats>::const_iterator
				it = array.begin(); it != array.end(); ++it) {
			len += it->second.length();
		}
		return len;
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band_stats_array::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < length())
			throw eInval();

		unsigned int offset = 0;

		for (std::map<unsigned int, cofmeter_band_stats>::iterator
				it = array.begin(); it != array.end(); ++it) {
			it->second.pack(buf + offset, it->second.length());
			offset += it->second.length();
		}

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band_stats_array::unpack(uint8_t* buf, size_t buflen)
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {

		clear();

		unsigned int offset = 0;
		unsigned int index = 0;

		while ((buflen - offset) >= sizeof(struct rofl::openflow13::ofp_meter_band_stats)) {
			add_mbstats(index).unpack(buf + offset, sizeof(struct rofl::openflow13::ofp_meter_band_stats));
			offset += sizeof(struct rofl::openflow13::ofp_meter_band_stats);
			index++;
		}

	} break;
	default:
		throw eBadVersion();
	}
}



