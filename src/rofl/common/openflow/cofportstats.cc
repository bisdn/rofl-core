#include "rofl/common/openflow/cofportstats.h"

using namespace rofl::openflow;

cofport_stats_request::cofport_stats_request(
		uint8_t of_version,
		uint8_t *buf,
		size_t buflen) :
				of_version(of_version),
				port_no(0)
{
	if ((buflen > 0) && (0 != buf)) {
		unpack(buf, buflen);
	}
}



cofport_stats_request::cofport_stats_request(
		uint8_t of_version,
		uint32_t port_no) :
				of_version(of_version),
				port_no(port_no)
{}



cofport_stats_request::~cofport_stats_request()
{}



cofport_stats_request::cofport_stats_request(
		cofport_stats_request const& request)
{
	*this = request;
}



cofport_stats_request&
cofport_stats_request::operator= (
		cofport_stats_request const& request)
{
	if (this == &request)
		return *this;

	of_version 	= request.of_version;
	port_no		= request.port_no;

	return *this;
}



void
cofport_stats_request::set_version(uint8_t of_version)
{
	this->of_version = of_version;
}



uint8_t
cofport_stats_request::get_version() const
{
	return of_version;
}



uint32_t
cofport_stats_request::get_portno() const
{
	return port_no;
}



void
cofport_stats_request::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_port_stats_request))
			throw eInval();

		struct rofl::openflow10::ofp_port_stats_request *req = (struct rofl::openflow10::ofp_port_stats_request*)buf;
		req->port_no 	= htobe16((uint16_t)(port_no & 0x0000ffff));

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_port_stats_request))
			throw eInval();

		struct rofl::openflow12::ofp_port_stats_request *req = (struct rofl::openflow12::ofp_port_stats_request*)buf;
		req->port_no 	= htobe32(port_no);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_port_stats_request))
			throw eInval();

		struct rofl::openflow13::ofp_port_stats_request *req = (struct rofl::openflow13::ofp_port_stats_request*)buf;
		req->port_no 	= htobe32(port_no);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_port_stats_request))
			throw eInval();

		struct rofl::openflow10::ofp_port_stats_request *req = (struct rofl::openflow10::ofp_port_stats_request*)buf;
		port_no			= be16toh(req->port_no);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_port_stats_request))
			throw eInval();

		struct rofl::openflow12::ofp_port_stats_request *req = (struct rofl::openflow12::ofp_port_stats_request*)buf;
		port_no			= be32toh(req->port_no);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_port_stats_request))
			throw eInval();

		struct rofl::openflow13::ofp_port_stats_request *req = (struct rofl::openflow13::ofp_port_stats_request*)buf;
		port_no			= be32toh(req->port_no);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofport_stats_request::length() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		return sizeof(struct rofl::openflow10::ofp_port_stats_request);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return sizeof(struct rofl::openflow12::ofp_port_stats_request);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return sizeof(struct rofl::openflow13::ofp_port_stats_request);
	} break;
	default:
		throw eBadVersion();
	}
}



cofport_stats_reply::cofport_stats_reply(
		uint8_t of_version,
		uint8_t *buf,
		size_t buflen) :
				of_version(of_version),
				port_no(0),
				rx_packets(0),
				tx_packets(0),
				rx_bytes(0),
				tx_bytes(0),
				rx_dropped(0),
				tx_dropped(0),
				rx_errors(0),
				tx_errors(0),
				rx_frame_err(0),
				rx_over_err(0),
				rx_crc_err(0),
				collisions(0),
				duration_sec(0),
				duration_nsec(0)
{
	if ((buflen > 0) && (0 != buf)) {
		unpack(buf, buflen);
	}
}



cofport_stats_reply::cofport_stats_reply(
		uint8_t of_version,
		uint32_t port_no,
		uint64_t rx_packets,
		uint64_t tx_packets,
		uint64_t rx_bytes,
		uint64_t tx_bytes,
		uint64_t rx_dropped,
		uint64_t tx_dropped,
		uint64_t rx_errors,
		uint64_t tx_errors,
		uint64_t rx_frame_err,
		uint64_t rx_over_err,
		uint64_t rx_crc_err,
		uint64_t collisions,
		uint32_t duration_sec,
		uint32_t duration_nsec) :
				of_version(of_version),
				port_no(port_no),
				rx_packets(rx_packets),
				tx_packets(tx_packets),
				rx_bytes(rx_bytes),
				tx_bytes(tx_bytes),
				rx_dropped(rx_dropped),
				tx_dropped(tx_dropped),
				rx_errors(rx_errors),
				tx_errors(tx_errors),
				rx_frame_err(rx_frame_err),
				rx_over_err(rx_over_err),
				rx_crc_err(rx_crc_err),
				collisions(collisions),
				duration_sec(duration_sec),
				duration_nsec(duration_nsec)
{}



cofport_stats_reply::~cofport_stats_reply()
{}



cofport_stats_reply::cofport_stats_reply(
		cofport_stats_reply const& port_stats)
{
	*this = port_stats;
}



cofport_stats_reply&
cofport_stats_reply::operator= (
		cofport_stats_reply const& ps)
{
	if (this == &ps)
		return *this;

	of_version 		= ps.of_version;
	port_no			= ps.port_no;
	rx_packets		= ps.rx_packets;
	tx_packets		= ps.tx_packets;
	rx_bytes		= ps.rx_bytes;
	tx_bytes		= ps.tx_bytes;
	rx_dropped		= ps.rx_dropped;
	tx_dropped		= ps.tx_dropped;
	rx_errors		= ps.rx_errors;
	tx_errors		= ps.tx_errors;
	rx_frame_err	= ps.rx_frame_err;
	rx_over_err		= ps.rx_over_err;
	rx_crc_err		= ps.rx_crc_err;
	collisions		= ps.collisions;
	duration_sec	= ps.duration_sec;
	duration_nsec	= ps.duration_nsec;

	return *this;
}



bool
cofport_stats_reply::operator== (
		cofport_stats_reply const& ps)
{
	return ((of_version		== ps.of_version) &&
			(port_no		== ps.port_no) &&
			(rx_packets 	== ps.rx_packets) &&
			(tx_packets 	== ps.tx_packets) &&
			(rx_bytes 		== ps.rx_bytes) &&
			(tx_bytes 		== ps.tx_bytes) &&
			(rx_dropped 	== ps.rx_dropped) &&
			(tx_dropped 	== ps.tx_dropped) &&
			(rx_errors 		== ps.rx_errors) &&
			(tx_errors 		== ps.tx_errors) &&
			(rx_frame_err 	== ps.rx_frame_err) &&
			(rx_over_err 	== ps.rx_over_err) &&
			(rx_crc_err 	== ps.rx_crc_err) &&
			(collisions 	== ps.collisions) &&
			(duration_sec 	== ps.duration_sec) &&
			(duration_nsec 	== ps.duration_nsec));
}



void
cofport_stats_reply::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct rofl::openflow10::ofp_port_stats *ps = (struct rofl::openflow10::ofp_port_stats*)buf;

		ps->port_no			= htobe16((uint16_t)(port_no & 0x0000ffff));
		ps->rx_packets		= htobe64(rx_packets);
		ps->tx_packets		= htobe64(tx_packets);
		ps->rx_bytes		= htobe64(rx_bytes);
		ps->tx_bytes		= htobe64(tx_bytes);
		ps->rx_dropped		= htobe64(rx_dropped);
		ps->tx_dropped		= htobe64(tx_dropped);
		ps->rx_errors		= htobe64(rx_errors);
		ps->tx_errors		= htobe64(tx_errors);
		ps->rx_frame_err	= htobe64(rx_frame_err);
		ps->rx_over_err		= htobe64(rx_over_err);
		ps->rx_crc_err		= htobe64(rx_crc_err);
		ps->collisions		= htobe64(collisions);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct rofl::openflow12::ofp_port_stats *ps = (struct rofl::openflow12::ofp_port_stats*)buf;

		ps->port_no			= htobe32(port_no);
		ps->rx_packets		= htobe64(rx_packets);
		ps->tx_packets		= htobe64(tx_packets);
		ps->rx_bytes		= htobe64(rx_bytes);
		ps->tx_bytes		= htobe64(tx_bytes);
		ps->rx_dropped		= htobe64(rx_dropped);
		ps->tx_dropped		= htobe64(tx_dropped);
		ps->rx_errors		= htobe64(rx_errors);
		ps->tx_errors		= htobe64(tx_errors);
		ps->rx_frame_err	= htobe64(rx_frame_err);
		ps->rx_over_err		= htobe64(rx_over_err);
		ps->rx_crc_err		= htobe64(rx_crc_err);
		ps->collisions		= htobe64(collisions);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct rofl::openflow13::ofp_port_stats *ps = (struct rofl::openflow13::ofp_port_stats*)buf;

		ps->port_no			= htobe32(port_no);
		ps->rx_packets		= htobe64(rx_packets);
		ps->tx_packets		= htobe64(tx_packets);
		ps->rx_bytes		= htobe64(rx_bytes);
		ps->tx_bytes		= htobe64(tx_bytes);
		ps->rx_dropped		= htobe64(rx_dropped);
		ps->tx_dropped		= htobe64(tx_dropped);
		ps->rx_errors		= htobe64(rx_errors);
		ps->tx_errors		= htobe64(tx_errors);
		ps->rx_frame_err	= htobe64(rx_frame_err);
		ps->rx_over_err		= htobe64(rx_over_err);
		ps->rx_crc_err		= htobe64(rx_crc_err);
		ps->collisions		= htobe64(collisions);
		ps->duration_sec	= htobe32(duration_sec);
		ps->duration_nsec	= htobe32(duration_nsec);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_port_stats))
			throw eInval();

		struct rofl::openflow10::ofp_port_stats* ps = (struct rofl::openflow10::ofp_port_stats*)buf;
		port_no			= (uint32_t)be16toh(ps->port_no);
		rx_packets		= be64toh(ps->rx_packets);
		tx_packets		= be64toh(ps->tx_packets);
		rx_bytes		= be64toh(ps->rx_bytes);
		tx_bytes		= be64toh(ps->tx_bytes);
		rx_dropped		= be64toh(ps->rx_dropped);
		tx_dropped		= be64toh(ps->tx_dropped);
		rx_errors		= be64toh(ps->rx_errors);
		tx_errors		= be64toh(ps->tx_errors);
		rx_frame_err	= be64toh(ps->rx_frame_err);
		rx_over_err		= be64toh(ps->rx_over_err);
		rx_crc_err		= be64toh(ps->rx_crc_err);
		collisions		= be64toh(ps->collisions);

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_port_stats))
			throw eInval();

		struct rofl::openflow12::ofp_port_stats* ps = (struct rofl::openflow12::ofp_port_stats*)buf;
		port_no			= be32toh(ps->port_no);
		rx_packets		= be64toh(ps->rx_packets);
		tx_packets		= be64toh(ps->tx_packets);
		rx_bytes		= be64toh(ps->rx_bytes);
		tx_bytes		= be64toh(ps->tx_bytes);
		rx_dropped		= be64toh(ps->rx_dropped);
		tx_dropped		= be64toh(ps->tx_dropped);
		rx_errors		= be64toh(ps->rx_errors);
		tx_errors		= be64toh(ps->tx_errors);
		rx_frame_err	= be64toh(ps->rx_frame_err);
		rx_over_err		= be64toh(ps->rx_over_err);
		rx_crc_err		= be64toh(ps->rx_crc_err);
		collisions		= be64toh(ps->collisions);

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_port_stats))
			throw eInval();

		struct rofl::openflow13::ofp_port_stats* ps = (struct rofl::openflow13::ofp_port_stats*)buf;
		port_no			= be32toh(ps->port_no);
		rx_packets		= be64toh(ps->rx_packets);
		tx_packets		= be64toh(ps->tx_packets);
		rx_bytes		= be64toh(ps->rx_bytes);
		tx_bytes		= be64toh(ps->tx_bytes);
		rx_dropped		= be64toh(ps->rx_dropped);
		tx_dropped		= be64toh(ps->tx_dropped);
		rx_errors		= be64toh(ps->rx_errors);
		tx_errors		= be64toh(ps->tx_errors);
		rx_frame_err	= be64toh(ps->rx_frame_err);
		rx_over_err		= be64toh(ps->rx_over_err);
		rx_crc_err		= be64toh(ps->rx_crc_err);
		collisions		= be64toh(ps->collisions);
		duration_sec	= be32toh(ps->duration_sec);
		duration_nsec	= be32toh(ps->duration_nsec);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofport_stats_reply::length() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_port_stats));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_port_stats));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_port_stats));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofport_stats_reply::reset()
{
	rx_packets 		= 0;
	tx_packets 		= 0;
	rx_bytes 		= 0;
	tx_bytes 		= 0;
	rx_dropped 		= 0;
	tx_dropped 		= 0;
	rx_errors 		= 0;
	tx_errors 		= 0;
	rx_frame_err 	= 0;
	rx_over_err 	= 0;
	rx_crc_err 		= 0;
	collisions 		= 0;
	duration_sec 	= 0;
	duration_nsec 	= 0;
}



void
cofport_stats_reply::set_version(uint8_t of_version)
{
	this->of_version = of_version;
}



uint8_t
cofport_stats_reply::get_version() const
{
	return of_version;
}



uint32_t
cofport_stats_reply::get_port_no() const
{
	return port_no;
}



uint64_t
cofport_stats_reply::get_rx_packets() const
{
	return rx_packets;
}



uint64_t
cofport_stats_reply::get_tx_packets() const
{
	return tx_packets;
}



uint64_t
cofport_stats_reply::get_rx_bytes() const
{
	return rx_bytes;
}



uint64_t
cofport_stats_reply::get_tx_bytes() const
{
	return tx_bytes;
}



uint64_t
cofport_stats_reply::get_rx_dropped() const
{
	return rx_dropped;
}



uint64_t
cofport_stats_reply::get_tx_dropped() const
{
	return tx_dropped;
}



uint64_t
cofport_stats_reply::get_rx_errors() const
{
	return rx_errors;
}



uint64_t
cofport_stats_reply::get_tx_errors() const
{
	return tx_errors;
}



uint64_t
cofport_stats_reply::get_rx_frame_err() const
{
	return rx_frame_err;
}



uint64_t
cofport_stats_reply::get_rx_over_err() const
{
	return rx_over_err;
}



uint64_t
cofport_stats_reply::get_rx_crc_err() const
{
	return rx_crc_err;
}



uint64_t
cofport_stats_reply::get_collisions() const
{
	return collisions;
}



uint32_t
cofport_stats_reply::get_duration_sec() const
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		return duration_sec;
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofport_stats_reply::get_duration_nsec() const
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		return duration_nsec;
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport_stats_reply::set_port_no(uint32_t port_no)
{
	this->port_no = port_no;
}



void
cofport_stats_reply::set_rx_packets(uint64_t rx_packets)
{
	this->rx_packets = rx_packets;
}



void
cofport_stats_reply::set_tx_packets(uint64_t tx_packets)
{
	this->tx_packets = tx_packets;
}



void
cofport_stats_reply::set_rx_bytes(uint64_t rx_bytes)
{
	this->rx_bytes = rx_bytes;
}



void
cofport_stats_reply::set_tx_bytes(uint64_t tx_bytes)
{
	this->tx_bytes = tx_bytes;
}



void
cofport_stats_reply::set_rx_dropped(uint64_t rx_dropped)
{
	this->rx_dropped = rx_dropped;
}



void
cofport_stats_reply::set_tx_dropped(uint64_t tx_dropped)
{
	this->tx_dropped = tx_dropped;
}



void
cofport_stats_reply::set_rx_errors(uint64_t rx_errors)
{
	this->rx_errors = rx_errors;
}



void
cofport_stats_reply::set_tx_errors(uint64_t tx_errors)
{
	this->tx_errors = tx_errors;
}



void
cofport_stats_reply::set_rx_frame_err(uint64_t rx_frame_err)
{
	this->rx_frame_err = rx_frame_err;
}



void
cofport_stats_reply::set_rx_over_err(uint64_t rx_over_err)
{
	this->rx_over_err = rx_over_err;
}



void
cofport_stats_reply::set_rx_crc_err(uint64_t rx_crc_err)
{
	this->rx_crc_err = rx_crc_err;
}



void
cofport_stats_reply::set_collisions(uint64_t collisions)
{
	this->collisions = collisions;
}



void
cofport_stats_reply::set_duration_sec(uint32_t duration_sec)
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		this->duration_sec = duration_sec;
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport_stats_reply::set_duration_nsec(uint32_t duration_nsec)
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		this->duration_nsec = duration_nsec;
	} break;
	default:
		throw eBadVersion();
	}
}


