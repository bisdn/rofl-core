/*
 * cofports.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofportstatsarray.h"

using namespace rofl::openflow;


cofportstatsarray::cofportstatsarray(uint8_t ofp_version) :
		ofp_version(ofp_version)
{

}


cofportstatsarray::~cofportstatsarray()
{

}


cofportstatsarray::cofportstatsarray(cofportstatsarray const& ports)
{
	*this = ports;
}


cofportstatsarray&
cofportstatsarray::operator= (cofportstatsarray const& ports)
{
	if (this == &ports)
		return *this;

	this->array.clear();

	ofp_version = ports.ofp_version;
	for (std::map<uint32_t, cofport_stats_reply>::const_iterator
			it = ports.array.begin(); it != ports.array.end(); ++it) {
		this->array[it->first] = it->second;
	}

	return *this;
}



bool
cofportstatsarray::operator== (cofportstatsarray const& ports)
{
	if (ofp_version != ports.ofp_version)
		return false;

	if (array.size() != ports.array.size())
		return false;

	for (std::map<uint32_t, cofport_stats_reply>::const_iterator
				it = ports.array.begin(); it != ports.array.end(); ++it) {
		if (not (array[it->first] == it->second))
			return false;
	}

	return true;
}



cofportstatsarray&
cofportstatsarray::operator+= (cofportstatsarray const& ports)
{
	/*
	 * this may replace existing port descriptions
	 */
	for (std::map<uint32_t, cofport_stats_reply>::const_iterator
			it = ports.array.begin(); it != ports.array.end(); ++it) {
		this->array[it->first] = it->second;
	}

	return *this;
}



size_t
cofportstatsarray::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofport_stats_reply>::const_iterator
			it = array.begin(); it != array.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
cofportstatsarray::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<uint32_t, cofport_stats_reply>::iterator
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
cofportstatsarray::unpack(uint8_t *buf, size_t buflen)
{
	array.clear();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow12::ofp_port_stats)) {

			uint32_t port_id = be32toh(((struct rofl::openflow12::ofp_port_stats*)buf)->port_no);

			add_port_stats(port_id).unpack(buf, sizeof(struct rofl::openflow12::ofp_port_stats));

			buf += sizeof(struct rofl::openflow12::ofp_port_stats);
			buflen -= sizeof(struct rofl::openflow12::ofp_port_stats);
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow13::ofp_port_stats)) {

			uint32_t port_id = be32toh(((struct rofl::openflow13::ofp_port_stats*)buf)->port_no);

			add_port_stats(port_id).unpack(buf, sizeof(struct rofl::openflow13::ofp_port_stats));

			buf += sizeof(struct rofl::openflow13::ofp_port_stats);
			buflen -= sizeof(struct rofl::openflow13::ofp_port_stats);
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofport_stats_reply&
cofportstatsarray::add_port_stats(uint32_t port_id)
{
	if (array.find(port_id) != array.end()) {
		array.erase(port_id);
	}
	return (array[port_id] = cofport_stats_reply(ofp_version));
}



void
cofportstatsarray::drop_port_stats(uint32_t port_id)
{
	if (array.find(port_id) == array.end()) {
		return;
	}
	array.erase(port_id);
}



cofport_stats_reply&
cofportstatsarray::set_port_stats(uint32_t port_id)
{
	if (array.find(port_id) == array.end()) {
		array[port_id] = cofport_stats_reply(ofp_version);
	}
	return array[port_id];
}



cofport_stats_reply const&
cofportstatsarray::get_port_stats(uint32_t port_id)
{
	if (array.find(port_id) == array.end()) {
		throw ePortStatsNotFound();
	}
	return array.at(port_id);
}



bool
cofportstatsarray::has_port_stats(uint32_t port_id)
{
	return (not (array.find(port_id) == array.end()));
}





