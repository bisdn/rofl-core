/*
 * cofqueues.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofqueuestatsarray.h"

using namespace rofl::openflow;


cofqueuestatsarray::cofqueuestatsarray(uint8_t ofp_version) :
		ofp_version(ofp_version)
{

}


cofqueuestatsarray::~cofqueuestatsarray()
{

}


cofqueuestatsarray::cofqueuestatsarray(cofqueuestatsarray const& queues)
{
	*this = queues;
}


cofqueuestatsarray&
cofqueuestatsarray::operator= (cofqueuestatsarray const& queues)
{
	if (this == &queues)
		return *this;

	this->array.clear();

	ofp_version = queues.ofp_version;
	for (std::map<uint32_t, std::map<uint32_t, cofqueue_stats_reply> >::const_iterator
			it = queues.array.begin(); it != queues.array.end(); ++it) {
		for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator
					jt = it->second.begin(); jt != it->second.end(); ++jt) {
			this->array[it->first][jt->first] = jt->second;
		}
	}

	return *this;
}



bool
cofqueuestatsarray::operator== (cofqueuestatsarray const& queues)
{
	if (ofp_version != queues.ofp_version)
		return false;

	if (array.size() != queues.array.size())
		return false;

	for (std::map<uint32_t, std::map<uint32_t, cofqueue_stats_reply> >::const_iterator
				it = queues.array.begin(); it != queues.array.end(); ++it) {

		if (array[it->first].size() != it->second.size())
			return false;

		for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator
					jt = it->second.begin(); jt != it->second.end(); ++jt) {
			if (not (array[it->first][jt->first] == jt->second))
				return false;
		}
	}

	return true;
}



cofqueuestatsarray&
cofqueuestatsarray::operator+= (cofqueuestatsarray const& queues)
{
	/*
	 * this may replace existing queue descriptions
	 */
	for (std::map<uint32_t, std::map<uint32_t, cofqueue_stats_reply> >::const_iterator
			it = queues.array.begin(); it != queues.array.end(); ++it) {
		for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator
					jt = it->second.begin(); jt != it->second.end(); ++jt) {
			this->array[it->first][jt->first] = jt->second;
		}
	}

	return *this;
}



size_t
cofqueuestatsarray::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, std::map<uint32_t, cofqueue_stats_reply> >::const_iterator
			it = array.begin(); it != array.end(); ++it) {
		for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator
					jt = it->second.begin(); jt != it->second.end(); ++jt) {
			len += jt->second.length();
		}
	}
	return len;
}



void
cofqueuestatsarray::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<uint32_t, std::map<uint32_t, cofqueue_stats_reply> >::const_iterator
				it = array.begin(); it != array.end(); ++it) {
			for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator
						jt = it->second.begin(); jt != it->second.end(); ++jt) {
				jt->second.pack(buf, jt->second.length());
				buf += jt->second.length();
			}
		}

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueuestatsarray::unpack(uint8_t *buf, size_t buflen)
{
	array.clear();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow12::ofp_queue_stats)) {

			uint32_t port_no  = be32toh(((struct rofl::openflow12::ofp_queue_stats*)buf)->port_no);
			uint32_t queue_id = be32toh(((struct rofl::openflow12::ofp_queue_stats*)buf)->queue_id);

			add_queue_stats(port_no, queue_id).unpack(buf, sizeof(struct rofl::openflow12::ofp_queue_stats));

			buf += sizeof(struct rofl::openflow12::ofp_queue_stats);
			buflen -= sizeof(struct rofl::openflow12::ofp_queue_stats);
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow13::ofp_queue_stats)) {

			uint32_t port_no  = be32toh(((struct rofl::openflow13::ofp_queue_stats*)buf)->port_no);
			uint32_t queue_id = be32toh(((struct rofl::openflow13::ofp_queue_stats*)buf)->queue_id);

			add_queue_stats(port_no, queue_id).unpack(buf, sizeof(struct rofl::openflow13::ofp_queue_stats));

			buf += sizeof(struct rofl::openflow13::ofp_queue_stats);
			buflen -= sizeof(struct rofl::openflow13::ofp_queue_stats);
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofqueue_stats_reply&
cofqueuestatsarray::add_queue_stats(uint32_t port_no, uint32_t queue_id)
{
	if (array[port_no].find(queue_id) != array[port_no].end()) {
		array[port_no].erase(queue_id);
	}
	return (array[port_no][queue_id] = cofqueue_stats_reply(ofp_version));
}



void
cofqueuestatsarray::drop_queue_stats(uint32_t port_no, uint32_t queue_id)
{
	if (array[port_no].find(queue_id) == array[port_no].end()) {
		return;
	}
	array[port_no].erase(queue_id);
}



cofqueue_stats_reply&
cofqueuestatsarray::set_queue_stats(uint32_t port_no, uint32_t queue_id)
{
	if (array[port_no].find(queue_id) == array[port_no].end()) {
		array[port_no][queue_id] = cofqueue_stats_reply(ofp_version);
	}
	return array[port_no][queue_id];
}



cofqueue_stats_reply const&
cofqueuestatsarray::get_queue_stats(uint32_t port_no, uint32_t queue_id)
{
	if (array.find(port_no) == array.end()) {
		throw eQueueStatsNotFound();
	}
	if (array.at(port_no).find(queue_id) == array.at(port_no).end()) {
		throw eQueueStatsNotFound();
	}
	return array.at(port_no).at(queue_id);
}



bool
cofqueuestatsarray::has_queue_stats(uint32_t port_no, uint32_t queue_id)
{
	return (not (array[port_no].find(queue_id) == array[port_no].end()));
}





