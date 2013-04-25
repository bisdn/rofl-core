#include "rofl/common/openflow/cofqueuestats.h"

#ifndef htobe16
#include "../endian_conversion.h"
#endif


using namespace rofl;




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
	case OFP10_VERSION: {
		if (buflen < sizeof(struct ofp10_queue_stats))
			throw eInval();

		struct ofp10_queue_stats *stats = (struct ofp10_queue_stats*)buf;

		stats->port_no		= htobe16((uint16_t)(port_no & 0x0000ffff));
		stats->queue_id		= htobe32(queue_id);

	} break;
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_queue_stats))
			throw eInval();

		struct ofp12_queue_stats *stats = (struct ofp12_queue_stats*)buf;

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
	case OFP10_VERSION: {
		if (buflen < sizeof(struct ofp10_queue_stats))
			throw eInval();

		struct ofp10_queue_stats *stats = (struct ofp10_queue_stats*)buf;

		port_no 		= (uint32_t)be16toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);

	} break;
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_queue_stats))
			throw eInval();

		struct ofp12_queue_stats *stats = (struct ofp12_queue_stats*)buf;

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
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_queue_stats_request));
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_queue_stats_request));
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
				tx_errors(0)
{}



cofqueue_stats_reply::cofqueue_stats_reply(
		uint8_t of_version,
		uint32_t port_no,
		uint32_t queue_id,
		uint64_t tx_bytes,
		uint64_t tx_packets,
		uint64_t tx_errors) :
				of_version(of_version),
				port_no(port_no),
				queue_id(queue_id),
				tx_bytes(tx_bytes),
				tx_packets(tx_packets),
				tx_errors(tx_errors)
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

	of_version 	= stats_reply.of_version;
	port_no		= stats_reply.port_no;
	queue_id	= stats_reply.queue_id;
	tx_bytes	= stats_reply.tx_bytes;
	tx_packets	= stats_reply.tx_packets;
	tx_errors	= stats_reply.tx_errors;

	return *this;
}



void
cofqueue_stats_reply::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case OFP10_VERSION: {
		if (buflen < sizeof(struct ofp10_queue_stats))
			throw eInval();

		struct ofp10_queue_stats *stats = (struct ofp10_queue_stats*)buf;

		stats->port_no		= htobe16((uint16_t)(port_no & 0x0000ffff));
		stats->queue_id		= htobe32(queue_id);
		stats->tx_bytes		= htobe32(tx_bytes);
		stats->tx_packets	= htobe32(tx_packets);
		stats->tx_errors	= htobe32(tx_errors);

	} break;
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_queue_stats))
			throw eInval();

		struct ofp12_queue_stats *stats = (struct ofp12_queue_stats*)buf;

		stats->port_no		= htobe32(port_no);
		stats->queue_id		= htobe32(queue_id);
		stats->tx_bytes		= htobe32(tx_bytes);
		stats->tx_packets	= htobe32(tx_packets);
		stats->tx_errors	= htobe32(tx_errors);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case OFP10_VERSION: {
		if (buflen < sizeof(struct ofp10_queue_stats))
			throw eInval();

		struct ofp10_queue_stats *stats = (struct ofp10_queue_stats*)buf;

		port_no 		= (uint32_t)be16toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);
		tx_bytes		= be32toh(stats->tx_bytes);
		tx_packets		= be32toh(stats->tx_packets);
		tx_errors		= be32toh(stats->tx_errors);

	} break;
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_queue_stats))
			throw eInval();

		struct ofp12_queue_stats *stats = (struct ofp12_queue_stats*)buf;

		port_no 		= be32toh(stats->port_no);
		queue_id		= be32toh(stats->queue_id);
		tx_bytes		= be32toh(stats->tx_bytes);
		tx_packets		= be32toh(stats->tx_packets);
		tx_errors		= be32toh(stats->tx_errors);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofqueue_stats_reply::length() const
{
	switch (of_version) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_queue_stats));
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_queue_stats));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}




