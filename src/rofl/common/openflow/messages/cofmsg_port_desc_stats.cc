#include "cofmsg_port_desc_stats.h"

using namespace rofl;


cofmsg_port_desc_stats_request::cofmsg_port_desc_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags) :
	cofmsg_stats_request(of_version, xid, 0, flags)
{
	switch (of_version) {
	case openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_MULTIPART_REQUEST);
		set_stats_type(openflow13::OFPMP_PORT_DESC);
		resize(sizeof(struct openflow13::ofp_multipart_request));
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_port_desc_stats_request::cofmsg_port_desc_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea)
{

}



cofmsg_port_desc_stats_request::cofmsg_port_desc_stats_request(
		cofmsg_port_desc_stats_request const& stats)
{
	*this = stats;
}



cofmsg_port_desc_stats_request&
cofmsg_port_desc_stats_request::operator= (
		cofmsg_port_desc_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	return *this;
}



cofmsg_port_desc_stats_request::~cofmsg_port_desc_stats_request()
{

}



void
cofmsg_port_desc_stats_request::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_port_desc_stats_request::resize(size_t len)
{
	return cofmsg_stats::resize(len);
}



size_t
cofmsg_port_desc_stats_request::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow13::ofp_multipart_request));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_desc_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_port_desc_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_port_desc_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow13::ofp_multipart_request)))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}





cofmsg_port_desc_stats_reply::cofmsg_port_desc_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofports const& ports) :
	cofmsg_stats_reply(of_version, xid, 0, flags),
	ports(ports)
{
	switch (of_version) {
	case openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_MULTIPART_REPLY);
		set_stats_type(openflow13::OFPMP_PORT_DESC);
		ofh_pds_reply = cofmsg::soframe();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_port_desc_stats_reply::cofmsg_port_desc_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea),
	ports(get_version())
{
	switch (get_version()) {
	case openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case openflow13::OFP_VERSION: {
		ofh_pds_reply = cofmsg::soframe();
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_port_desc_stats_reply::cofmsg_port_desc_stats_reply(
		cofmsg_port_desc_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_port_desc_stats_reply&
cofmsg_port_desc_stats_reply::operator= (
		cofmsg_port_desc_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ports = stats.ports;

	return *this;
}



cofmsg_port_desc_stats_reply::~cofmsg_port_desc_stats_reply()
{

}



void
cofmsg_port_desc_stats_reply::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_port_desc_stats_reply::resize(size_t len)
{
	return (ofh_pds_reply = cofmsg_stats::resize(len));
}



size_t
cofmsg_port_desc_stats_reply::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return sizeof(struct openflow13::ofp_multipart_reply) + ports.length();
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_desc_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	set_length(length());

	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		ports.pack(buf + sizeof(struct openflow13::ofp_multipart_reply), ports.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_port_desc_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_port_desc_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	ports.clear();
	ports.set_version(get_version());

	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow13::ofp_multipart_reply))) {
			throw eBadSyntaxTooShort();
		}

		ofh_pds_reply = soframe();

		size_t residual = get_length() - sizeof(struct openflow13::ofp_multipart_reply);

		ports.unpack(ofh13_pds_reply->body, residual);
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}





