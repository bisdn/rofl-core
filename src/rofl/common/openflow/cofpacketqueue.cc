/*
 * cofpacketqueue.cc
 *
 *  Created on: 30.04.2013
 *      Author: andi
 */

#include "rofl/common/openflow/cofpacketqueue.h"

using namespace rofl::openflow;

cofpacket_queue::cofpacket_queue(
		uint8_t ofp_version,
		uint32_t port_no,
		uint32_t queue_id,
		const cofqueue_props& props) :
				ofp_version(ofp_version),
				port_no(port_no),
				queue_id(queue_id),
				len(0),
				queue_props(props)
{
	queue_props.set_version(ofp_version);
}



cofpacket_queue::~cofpacket_queue()
{

}



cofpacket_queue::cofpacket_queue(
		const cofpacket_queue& pq)
{
	*this = pq;
}



cofpacket_queue&
cofpacket_queue::operator= (
		const cofpacket_queue& pq)
{
	if (this == &pq)
		return *this;

	ofp_version 	= pq.ofp_version;
	port_no			= pq.port_no;
	queue_id		= pq.queue_id;
	queue_props 	= pq.queue_props;

	return *this;
}



size_t
cofpacket_queue::length() const
{
	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_packet_queue) + queue_props.length());
	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_packet_queue) + queue_props.length());
	} break;
	default:
		throw eBadVersion();
 	}
}



void
cofpacket_queue::pack(
			uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofpacket_queue::length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_packet_queue* hdr =
				(struct rofl::openflow10::ofp_packet_queue*)buf;

		len = length();

		hdr->queue_id	= htobe32(queue_id);
		hdr->len		= htobe16(len);

		queue_props.pack((uint8_t*)(hdr->properties), queue_props.length());

	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct rofl::openflow13::ofp_packet_queue* hdr =
				(struct rofl::openflow13::ofp_packet_queue*)buf;

		len = length();

		hdr->queue_id	= htobe32(queue_id);
		hdr->port		= htobe32(port_no);
		hdr->len		= htobe16(len);

		queue_props.pack((uint8_t*)(hdr->properties), queue_props.length());

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofpacket_queue::unpack(
			uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	queue_props.clear();
	queue_props.set_version(ofp_version);

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {

		if (buflen < sizeof(struct rofl::openflow10::ofp_packet_queue))
			throw eInval();

		struct rofl::openflow10::ofp_packet_queue* hdr =
				(struct rofl::openflow10::ofp_packet_queue*)buf;

		queue_id 	= be32toh(hdr->queue_id);
		len			= be16toh(hdr->len);

		if (buflen < len)
			throw eInval();

		queue_props.unpack((uint8_t*)hdr->properties, len - sizeof(struct rofl::openflow10::ofp_packet_queue));

	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < sizeof(struct rofl::openflow13::ofp_packet_queue))
			throw eInval();

		struct rofl::openflow12::ofp_packet_queue* hdr =
				(struct rofl::openflow12::ofp_packet_queue*)buf;

		queue_id 	= be32toh(hdr->queue_id);
		port_no		= be32toh(hdr->port);
		len			= be16toh(hdr->len);

		if (buflen < len)
			throw eInval();

		queue_props.unpack((uint8_t*)hdr->properties, len - sizeof(struct rofl::openflow13::ofp_packet_queue));

	} break;
	default:
		throw eBadVersion();
	}
}






