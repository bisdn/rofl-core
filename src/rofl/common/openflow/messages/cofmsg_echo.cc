#include "cofmsg_echo.h"

using namespace rofl;

cofmsg_echo_request::cofmsg_echo_request(
		uint8_t of_version,
		uint32_t xid,
		uint8_t* data,
		size_t datalen) :
	cofmsg(sizeof(struct openflow::ofp_header)),
	body(0)
{
	body.assign(data, datalen);

	set_version(of_version);
	set_length(sizeof(struct openflow::ofp_header));
	set_xid(xid);

	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_ECHO_REQUEST);
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_ECHO_REQUEST);
	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_ECHO_REQUEST);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_echo_request::cofmsg_echo_request(
		cmemory *memarea) :
	cofmsg(memarea),
	body(0)
{

}



cofmsg_echo_request::cofmsg_echo_request(
		cofmsg_echo_request const& echo)
{
	*this = echo;
}



cofmsg_echo_request&
cofmsg_echo_request::operator= (
		cofmsg_echo_request const& echo)
{
	if (this == &echo)
		return *this;

	cofmsg::operator =(echo);

	body = echo.body;

	return *this;
}



cofmsg_echo_request::~cofmsg_echo_request()
{

}



void
cofmsg_echo_request::reset()
{
	cofmsg::reset();
	body.clear();
}



size_t
cofmsg_echo_request::length() const
{
	return (sizeof(struct openflow::ofp_header) + body.memlen());
}



void
cofmsg_echo_request::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eInval();

	cofmsg::pack(buf, buflen);

	memcpy(buf + sizeof(struct openflow::ofp_header), body.somem(), body.memlen());
}



void
cofmsg_echo_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_echo_request::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	switch (get_version()) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		if (get_length() > sizeof(struct openflow::ofp_header)) {
			body.assign(sobody(), bodylen());
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cmemory&
cofmsg_echo_request::get_body()
{
	return body;
}






cofmsg_echo_reply::cofmsg_echo_reply(
		uint8_t of_version,
		uint32_t xid,
		uint8_t* data,
		size_t datalen) :
	cofmsg(sizeof(struct openflow::ofp_header)),
	body(0)
{
	body.assign(data, datalen);

	set_version(of_version);
	set_length(sizeof(struct openflow::ofp_header));
	set_xid(xid);

	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_ECHO_REPLY);
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_ECHO_REPLY);
	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_ECHO_REPLY);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_echo_reply::cofmsg_echo_reply(
		cmemory *memarea) :
	cofmsg(memarea),
	body(0)
{

}



cofmsg_echo_reply::cofmsg_echo_reply(
		cofmsg_echo_reply const& echo)
{
	*this = echo;
}



cofmsg_echo_reply&
cofmsg_echo_reply::operator= (
		cofmsg_echo_reply const& echo)
{
	if (this == &echo)
		return *this;

	cofmsg::operator =(echo);

	body = echo.body;

	return *this;
}



cofmsg_echo_reply::~cofmsg_echo_reply()
{

}



void
cofmsg_echo_reply::reset()
{
	cofmsg::reset();
	body.clear();
}



size_t
cofmsg_echo_reply::length() const
{
	return (sizeof(struct openflow::ofp_header) + body.memlen());
}



void
cofmsg_echo_reply::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eInval();

	cofmsg::pack(buf, buflen);

	memcpy(buf + sizeof(struct openflow::ofp_header), body.somem(), body.memlen());
}



void
cofmsg_echo_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_echo_reply::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	switch (get_version()) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		if (get_length() > sizeof(struct openflow::ofp_header)) {
			body.assign(sobody(), bodylen());
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cmemory&
cofmsg_echo_reply::get_body()
{
	return body;
}


