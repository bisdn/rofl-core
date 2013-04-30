/*
 * cofpacketqueue.cc
 *
 *  Created on: 30.04.2013
 *      Author: andi
 */


#include "cofpacketqueue.h"


using namespace rofl;


cofpacket_queue::cofpacket_queue(
	uint8_t of_version) :
		of_version(of_version),
		qpl(of_version)
{
	switch (of_version) {
	case OFP10_VERSION: {
		cmemory::resize(sizeof(struct ofp10_packet_queue));
	} break;
	case OFP12_VERSION: {
		cmemory::resize(sizeof(struct ofp12_packet_queue));
	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}

	ofp_pqueue = somem();
}



cofpacket_queue::~cofpacket_queue()
{

}



cofpacket_queue::cofpacket_queue(
	cofpacket_queue const& pq) :
		of_version(pq.of_version),
		qpl(pq.of_version)
{
	*this = pq;
}



cofpacket_queue&
cofpacket_queue::operator= (
	cofpacket_queue const& pq)
{
	if (this == &pq)
		return *this;

	cmemory::operator= (pq);
	of_version = pq.of_version;
	qpl = pq.qpl;
	ofp_pqueue = somem();

	return *this;
}



size_t
cofpacket_queue::length() const
{
	switch (of_version) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_packet_queue) + qpl.length());
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_packet_queue) + qpl.length());
	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
 	}
	return 0;
}



void
cofpacket_queue::pack(
			uint8_t *buf, size_t buflen) const
{
	if (buflen < length())
		throw eInval();

	switch (of_version) {
	case OFP10_VERSION: {

		memcpy(buf, somem(), sizeof(struct ofp10_packet_queue));
		struct ofp10_packet_queue *pq = (struct ofp10_packet_queue*)buf;
		pq->len = htobe16(length());

		qpl.pack(buf + sizeof(struct ofp10_packet_queue), buflen - sizeof(struct ofp10_packet_queue));

	} break;
	case OFP12_VERSION: {

		memcpy(buf, somem(), sizeof(struct ofp12_packet_queue));
		struct ofp12_packet_queue *pq = (struct ofp12_packet_queue*)buf;
		pq->len = htobe16(length());

		qpl.pack(buf + sizeof(struct ofp12_packet_queue), buflen - sizeof(struct ofp12_packet_queue));

	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}



void
cofpacket_queue::unpack(
			uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case OFP10_VERSION: {

		if (buflen < sizeof(struct ofp10_packet_queue))
			throw eInval();

		struct ofp10_packet_queue *pq = (struct ofp10_packet_queue*)buf;

		if (buflen < be16toh(pq->len))
			throw eInval();

		cmemory::resize(sizeof(struct ofp10_packet_queue));
		ofp_pqueue = somem();

		memcpy(somem(), buf, sizeof(struct ofp10_packet_queue));

		if (be16toh(pq->len) > sizeof(struct ofp10_packet_queue))
			qpl.unpack(buf + sizeof(struct ofp10_packet_queue), be16toh(pq->len));

	} break;
	case OFP12_VERSION: {

		if (buflen < sizeof(struct ofp12_packet_queue))
			throw eInval();

		struct ofp12_packet_queue *pq = (struct ofp12_packet_queue*)buf;

		if (buflen < be16toh(pq->len))
			throw eInval();

		cmemory::resize(sizeof(struct ofp12_packet_queue));
		ofp_pqueue = somem();

		memcpy(somem(), buf, sizeof(struct ofp12_packet_queue));

		if (be16toh(pq->len) > sizeof(struct ofp12_packet_queue))
			qpl.unpack(buf + sizeof(struct ofp12_packet_queue), be16toh(pq->len));

	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}



uint32_t
cofpacket_queue::get_queue_id() const
{
	switch (of_version) {
	case OFP10_VERSION: {
		return be32toh(ofp10_pqueue->queue_id);
	} break;
	case OFP12_VERSION: {
		return be32toh(ofp12_pqueue->queue_id);
	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}



void
cofpacket_queue::set_queue_id(
			uint32_t queue_id)
{
	switch(of_version) {
	case OFP10_VERSION: {
		ofp10_pqueue->queue_id = htobe32(queue_id);
	} break;
	case OFP12_VERSION: {
		ofp12_pqueue->queue_id = htobe32(queue_id);
	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}



uint32_t
cofpacket_queue::get_port() const
{
	switch (of_version) {
	case OFP12_VERSION: {
		return be32toh(ofp12_pqueue->port);
	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}



void
cofpacket_queue::set_port(
			uint32_t port_no)
{
	switch(of_version) {
	case OFP12_VERSION: {
		ofp12_pqueue->port = htobe32(port_no);
	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}



cofqueueproplist&
cofpacket_queue::get_queue_prop_list()
{
	return qpl;
}


