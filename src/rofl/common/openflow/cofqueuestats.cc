/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofqueuestats.h"

#ifndef htobe16
#include "../endian_conversion.h"
#endif


using namespace rofl::openflow;




cofqueue_stats_request::cofqueue_stats_request(
		uint8_t of_version) :
				of_version(of_version),
				port_no(0),
				queue_id(0)
{}



cofqueue_stats_request::cofqueue_stats_request(
		uint8_t of_version,
		uint32_t port_no,
		uint32_t queue_id) :
				of_version(of_version),
				port_no(port_no),
				queue_id(queue_id)
{}



cofqueue_stats_request::~cofqueue_stats_request()
{}



cofqueue_stats_request::cofqueue_stats_request(
		cofqueue_stats_request const& stats_request)
{
	*this = stats_request;
}



cofqueue_stats_request&
cofqueue_stats_request::operator= (
		cofqueue_stats_request const& stats_request)
{
	if (this == &stats_request)
		return *this;

	of_version 	= stats_request.of_version;
	port_no		= stats_request.port_no;
	queue_id	= stats_request.queue_id;

	return *this;
}



void
cofqueue_stats_request::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_queue_stats_request))
			throw eInval();

		struct rofl::openflow10::ofp_queue_stats_request *stats = (struct rofl::openflow10::ofp_queue_stats_request*)buf;

		stats->port_no		= htobe16((uint16_t)(port_no & 0x0000ffff));
		stats->queue_id		= htobe32(queue_id);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_queue_stats_request))
			throw eInval();

		struct rofl::openflow12::ofp_queue_stats_request *stats = (struct rofl::openflow12::ofp_queue_stats_request*)buf;

		stats->port_no		= htobe32(port_no);
		stats->queue_id		= htobe32(queue_id);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_queue_stats_request))
			throw eInval();

		struct rofl::openflow13::ofp_queue_stats_request *stats = (struct rofl::openflow13::ofp_queue_stats_request*)buf;

		stats->port_no		= htobe32(port_no);
		stats->queue_id		= htobe32(queue_id);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_queue_stats_request))
			throw eInval();

		struct rofl::openflow10::ofp_queue_stats_request *stats = (struct rofl::openflow10::ofp_queue_stats_request*)buf;

		port_no 		= (uint32_t)be16toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_queue_stats_request))
			throw eInval();

		struct rofl::openflow12::ofp_queue_stats_request *stats = (struct rofl::openflow12::ofp_queue_stats_request*)buf;

		port_no 		= be32toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_queue_stats_request))
			throw eInval();

		struct rofl::openflow13::ofp_queue_stats_request *stats = (struct rofl::openflow13::ofp_queue_stats_request*)buf;

		port_no 		= be32toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofqueue_stats_request::length() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_queue_stats_request));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_queue_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_queue_stats_request));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



cofqueue_stats_reply::cofqueue_stats_reply(
		uint8_t of_version) :
				of_version(of_version),
				port_no(0),
				queue_id(0),
				tx_bytes(0),
				tx_packets(0),
				tx_errors(0),
				duration_sec(0),
				duration_nsec(0)
{}



cofqueue_stats_reply::cofqueue_stats_reply(
		uint8_t of_version,
		uint32_t port_no,
		uint32_t queue_id,
		uint64_t tx_bytes,
		uint64_t tx_packets,
		uint64_t tx_errors,
		uint32_t duration_sec,
		uint32_t duration_nsec) :
				of_version(of_version),
				port_no(port_no),
				queue_id(queue_id),
				tx_bytes(tx_bytes),
				tx_packets(tx_packets),
				tx_errors(tx_errors),
				duration_sec(duration_sec),
				duration_nsec(duration_nsec)
{}



cofqueue_stats_reply::~cofqueue_stats_reply()
{}



cofqueue_stats_reply::cofqueue_stats_reply(
		cofqueue_stats_reply const& stats_reply)
{
	*this = stats_reply;
}



cofqueue_stats_reply&
cofqueue_stats_reply::operator= (
		cofqueue_stats_reply const& stats_reply)
{
	if (this == &stats_reply)
		return *this;

	of_version 		= stats_reply.of_version;
	port_no			= stats_reply.port_no;
	queue_id		= stats_reply.queue_id;
	tx_bytes		= stats_reply.tx_bytes;
	tx_packets		= stats_reply.tx_packets;
	tx_errors		= stats_reply.tx_errors;
	duration_sec 	= stats_reply.duration_sec;
	duration_nsec	= stats_reply.duration_nsec;

	return *this;
}



bool
cofqueue_stats_reply::operator== (
		cofqueue_stats_reply const& stats)
{
	return ((of_version 	== stats.of_version) &&
			(port_no 		== stats.port_no) &&
			(queue_id 		== stats.queue_id) &&
			(tx_bytes 		== stats.tx_bytes) &&
			(tx_packets 	== stats.tx_packets) &&
			(tx_errors 		== stats.tx_errors) &&
			(duration_sec 	== stats.duration_sec) &&
			(duration_nsec 	== stats.duration_nsec));
}



void
cofqueue_stats_reply::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_queue_stats))
			throw eInval();

		struct rofl::openflow10::ofp_queue_stats *stats = (struct rofl::openflow10::ofp_queue_stats*)buf;

		stats->port_no		= htobe16((uint16_t)(port_no & 0x0000ffff));
		stats->queue_id		= htobe32(queue_id);
		stats->tx_bytes		= htobe64(tx_bytes);
		stats->tx_packets	= htobe64(tx_packets);
		stats->tx_errors	= htobe64(tx_errors);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_queue_stats))
			throw eInval();

		struct rofl::openflow12::ofp_queue_stats *stats = (struct rofl::openflow12::ofp_queue_stats*)buf;

		stats->port_no		= htobe32(port_no);
		stats->queue_id		= htobe32(queue_id);
		stats->tx_bytes		= htobe64(tx_bytes);
		stats->tx_packets	= htobe64(tx_packets);
		stats->tx_errors	= htobe64(tx_errors);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_queue_stats))
			throw eInval();

		struct rofl::openflow13::ofp_queue_stats *stats = (struct rofl::openflow13::ofp_queue_stats*)buf;

		stats->port_no		= htobe32(port_no);
		stats->queue_id		= htobe32(queue_id);
		stats->tx_bytes		= htobe64(tx_bytes);
		stats->tx_packets	= htobe64(tx_packets);
		stats->tx_errors	= htobe64(tx_errors);
		stats->duration_sec	= htobe32(duration_sec);
		stats->duration_nsec= htobe32(duration_nsec);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_queue_stats))
			throw eInval();

		struct rofl::openflow10::ofp_queue_stats *stats = (struct rofl::openflow10::ofp_queue_stats*)buf;

		port_no 		= (uint32_t)be16toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);
		tx_bytes		= be64toh(stats->tx_bytes);
		tx_packets		= be64toh(stats->tx_packets);
		tx_errors		= be64toh(stats->tx_errors);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_queue_stats))
			throw eInval();

		struct rofl::openflow12::ofp_queue_stats *stats = (struct rofl::openflow12::ofp_queue_stats*)buf;

		port_no 		= be32toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);
		tx_bytes		= be64toh(stats->tx_bytes);
		tx_packets		= be64toh(stats->tx_packets);
		tx_errors		= be64toh(stats->tx_errors);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_queue_stats))
			throw eInval();

		struct rofl::openflow13::ofp_queue_stats *stats = (struct rofl::openflow13::ofp_queue_stats*)buf;

		port_no 		= be32toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);
		tx_bytes		= be64toh(stats->tx_bytes);
		tx_packets		= be64toh(stats->tx_packets);
		tx_errors		= be64toh(stats->tx_errors);
		duration_sec	= be32toh(stats->duration_sec);
		duration_nsec	= be32toh(stats->duration_nsec);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofqueue_stats_reply::length() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_queue_stats));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_queue_stats));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_queue_stats));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}




