/*
 * coftables.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/coftablestatsarray.h"

using namespace rofl::openflow;


coftablestatsarray::coftablestatsarray(uint8_t ofp_version) :
		ofp_version(ofp_version)
{

}


coftablestatsarray::~coftablestatsarray()
{

}


coftablestatsarray::coftablestatsarray(coftablestatsarray const& tables)
{
	*this = tables;
}


coftablestatsarray&
coftablestatsarray::operator= (coftablestatsarray const& tables)
{
	if (this == &tables)
		return *this;

	this->array.clear();

	ofp_version = tables.ofp_version;
	for (std::map<uint8_t, coftable_stats_reply>::const_iterator
			it = tables.array.begin(); it != tables.array.end(); ++it) {
		this->array[it->first] = it->second;
	}

	return *this;
}



coftablestatsarray&
coftablestatsarray::operator+= (coftablestatsarray const& tables)
{
	/*
	 * this may replace existing table descriptions
	 */
	for (std::map<uint8_t, coftable_stats_reply>::const_iterator
			it = tables.array.begin(); it != tables.array.end(); ++it) {
		this->array[it->first] = it->second;
	}

	return *this;
}



size_t
coftablestatsarray::length() const
{
	size_t len = 0;
	for (std::map<uint8_t, coftable_stats_reply>::const_iterator
			it = array.begin(); it != array.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
coftablestatsarray::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<uint8_t, coftable_stats_reply>::iterator
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
coftablestatsarray::unpack(uint8_t *buf, size_t buflen)
{
	array.clear();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow10::ofp_table_stats)) {

			uint8_t table_id = ((struct rofl::openflow10::ofp_table_stats*)buf)->table_id;

			add_table_stats(table_id).unpack(buf, sizeof(struct rofl::openflow10::ofp_table_stats));

			buf += sizeof(struct rofl::openflow10::ofp_table_stats);
			buflen -= sizeof(struct rofl::openflow10::ofp_table_stats);
		}
	} break;
	case rofl::openflow12::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow12::ofp_table_stats)) {

			uint8_t table_id = ((struct rofl::openflow12::ofp_table_stats*)buf)->table_id;

			add_table_stats(table_id).unpack(buf, sizeof(struct rofl::openflow12::ofp_table_stats));

			buf += sizeof(struct rofl::openflow12::ofp_table_stats);
			buflen -= sizeof(struct rofl::openflow12::ofp_table_stats);
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow13::ofp_table_stats)) {

			uint8_t table_id = ((struct rofl::openflow13::ofp_table_stats*)buf)->table_id;

			add_table_stats(table_id).unpack(buf, sizeof(struct rofl::openflow13::ofp_table_stats));

			buf += sizeof(struct rofl::openflow13::ofp_table_stats);
			buflen -= sizeof(struct rofl::openflow13::ofp_table_stats);
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



rofl::coftable_stats_reply&
coftablestatsarray::add_table_stats(uint8_t table_id)
{
	if (array.find(table_id) != array.end()) {
		array.erase(table_id);
	}
	return (array[table_id] = coftable_stats_reply(ofp_version));
}



void
coftablestatsarray::drop_table_stats(uint8_t table_id)
{
	if (array.find(table_id) == array.end()) {
		return;
	}
	array.erase(table_id);
}



rofl::coftable_stats_reply&
coftablestatsarray::set_table_stats(uint8_t table_id)
{
	return (array[table_id] = coftable_stats_reply(ofp_version));
}



rofl::coftable_stats_reply const&
coftablestatsarray::get_table_stats(uint8_t table_id)
{
	if (array.find(table_id) == array.end()) {
		throw;
	}
	return array.at(table_id);
}



bool
coftablestatsarray::has_table_stats(uint8_t table_id)
{
	return (not (array.find(table_id) == array.end()));
}





