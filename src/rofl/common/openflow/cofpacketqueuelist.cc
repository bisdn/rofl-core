/*
 * cofpacketqueuelist.cc
 *
 *  Created on: 30.04.2013
 *      Author: andi
 */


#include "cofpacketqueuelist.h"


using namespace rofl;


cofpacket_queue_list::cofpacket_queue_list(
	uint8_t of_version) :
		of_version(of_version)
{

}



cofpacket_queue_list::~cofpacket_queue_list()
{

}



cofpacket_queue_list::cofpacket_queue_list(
	cofpacket_queue_list const& qpl) :
		of_version(qpl.of_version)
{
	*this = qpl;
}



cofpacket_queue_list&
cofpacket_queue_list::operator= (
		cofpacket_queue_list const& qpl)
{
	if (this == &qpl)
		return *this;

	coflist<cofpacket_queue>::operator =(qpl);
	of_version = qpl.of_version;

	return *this;
}



size_t
cofpacket_queue_list::length() const
{
	size_t len = 0;

	// should be the same for openflow10::OFP, openflow12::OFP, openflow13::OFP, ...
	for (coflist<cofpacket_queue>::const_iterator
			it = begin(); it != end(); ++it) {
		len += (*it).length();
	}

	return len;
}



void
cofpacket_queue_list::unpack(
		uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {

		while (buflen > sizeof(struct openflow10::ofp_packet_queue)) {

			struct openflow10::ofp_packet_queue *pq = (struct openflow10::ofp_packet_queue*)buf;

			if (buflen < be16toh(pq->len))
				throw eInval();

			cofpacket_queue packet_queue(of_version);
			packet_queue.unpack(buf, be16toh(pq->len));
			next() = packet_queue;

			buflen -= be16toh(pq->len);
			buf += be16toh(pq->len);
		}

	} break;
	case openflow12::OFP_VERSION: {

		while (buflen > sizeof(struct openflow12::ofp_packet_queue)) {

			struct openflow12::ofp_packet_queue *pq = (struct openflow12::ofp_packet_queue*)buf;

			if (buflen < be16toh(pq->len))
				throw eInval();

			cofpacket_queue packet_queue(of_version);
			packet_queue.unpack(buf, be16toh(pq->len));
			next() = packet_queue;

			buflen -= be16toh(pq->len);
			buf += be16toh(pq->len);
		}

	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}



void
cofpacket_queue_list::pack(
		uint8_t *buf, size_t buflen) const
{
	if (buflen < length())
		throw eInval();

	switch (of_version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION: {

		for (coflist<cofpacket_queue>::const_iterator
				it = begin(); it != end(); ++it) {
			(*it).pack(buf, (*it).length());
			buf += (*it).length();
			buflen -= (*it).length();
		}

	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}



cofpacket_queue&
cofpacket_queue_list::find_packet_queue(
		uint32_t queue_id)
{
	for (coflist<cofpacket_queue>::iterator
			it = begin(); it != end(); ++it) {
		if ((*it).get_queue_id() == queue_id) {
			return (*it);
		}
	}
	throw ePacketQueueNotFound();
}





