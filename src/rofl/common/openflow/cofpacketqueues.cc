/*
 * cofpacketqueuelist.cc
 *
 *  Created on: 30.04.2013
 *      Author: andi
 */


#include "rofl/common/openflow/cofpacketqueues.h"


using namespace rofl::openflow;


cofpacket_queues::cofpacket_queues(
	uint8_t ofp_version) :
		ofp_version(ofp_version)
{

}



cofpacket_queues::~cofpacket_queues()
{

}



cofpacket_queues::cofpacket_queues(
		const cofpacket_queues& queues)
{
	*this = queues;
}



cofpacket_queues&
cofpacket_queues::operator= (
		const cofpacket_queues& queues)
{
	if (this == &queues)
		return *this;

	clear();

	ofp_version = queues.ofp_version;
	for (std::map<uint32_t, std::map<uint32_t, cofpacket_queue> >::const_iterator
			it = queues.get_packet_queues().begin(); it != queues.get_packet_queues().end(); ++it) {
		for (std::map<uint32_t, cofpacket_queue>::const_iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt) {
			pqueues[it->first][jt->first] = jt->second;
		}
	}

	return *this;
}



void
cofpacket_queues::clear()
{
	pqueues.clear();
}



cofpacket_queue&
cofpacket_queues::add_pqueue(uint32_t port_no, uint32_t queue_id)
{
	if (pqueues[port_no].find(queue_id) != pqueues[port_no].end()) {
		pqueues[port_no].erase(queue_id);
	}
	pqueues[port_no][queue_id] = cofpacket_queue(ofp_version, port_no, queue_id);
	return pqueues[port_no][queue_id];
}



cofpacket_queue&
cofpacket_queues::set_pqueue(uint32_t port_no, uint32_t queue_id)
{
	if (pqueues[port_no].find(queue_id) == pqueues[port_no].end()) {
		pqueues[port_no][queue_id] = cofpacket_queue(ofp_version, port_no, queue_id);
	}
	return pqueues[port_no][queue_id];
}



const cofpacket_queue&
cofpacket_queues::get_pqueue(uint32_t port_no, uint32_t queue_id) const
{
	if (pqueues.at(port_no).find(queue_id) == pqueues.at(port_no).end()) {
		throw ePacketQueueNotFound();
	}
	return pqueues.at(port_no).at(queue_id);
}



void
cofpacket_queues::drop_pqueue(uint32_t port_no, uint32_t queue_id)
{
	if (pqueues[port_no].find(queue_id) == pqueues[port_no].end()) {
		return;
	}
	pqueues[port_no].erase(queue_id);
}



bool
cofpacket_queues::has_pqueue(uint32_t port_no, uint32_t queue_id) const
{
	return (not (pqueues.at(port_no).find(queue_id) == pqueues.at(port_no).end()));
}



size_t
cofpacket_queues::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, std::map<uint32_t, cofpacket_queue> >::const_iterator
			it = pqueues.begin(); it != pqueues.end(); ++it) {
		for (std::map<uint32_t, cofpacket_queue>::const_iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt) {
			len += jt->second.length();
		}
	}
	return len;
}



void
cofpacket_queues::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<uint32_t, std::map<uint32_t, cofpacket_queue> >::iterator
				it = pqueues.begin(); it != pqueues.end(); ++it) {
			for (std::map<uint32_t, cofpacket_queue>::iterator
					jt = it->second.begin(); jt != it->second.end(); ++jt) {
				jt->second.pack(buf, jt->second.length());
				buf += jt->second.length();
				buflen -= jt->second.length();
			}
		}

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofpacket_queues::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	clear();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {

		while (buflen > sizeof(struct openflow10::ofp_packet_queue)) {

			struct rofl::openflow10::ofp_packet_queue *hdr = (struct rofl::openflow10::ofp_packet_queue*)buf;

			uint16_t len = be16toh(hdr->len);

			if ((buflen < len) || (len == 0))
				throw eInval();

			// we use port_no 0 for all ofp_packet_queue structs received in OFP 1.0
			add_pqueue(/*port-no*/0, be32toh(hdr->queue_id)).unpack(buf, len);

			buflen -= len;
			buf += len;
		}

	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		while (buflen > sizeof(struct openflow13::ofp_packet_queue)) {

			struct rofl::openflow13::ofp_packet_queue *hdr = (struct rofl::openflow13::ofp_packet_queue*)buf;

			uint16_t len = be16toh(hdr->len);

			if ((buflen < len) || (len == 0))
				throw eInval();

			add_pqueue(be32toh(hdr->port), be32toh(hdr->queue_id)).unpack(buf, len);

			buflen -= len;
			buf += len;
		}

	} break;
	default:
		throw eBadVersion();
	}
}





