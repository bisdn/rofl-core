#include "rofl/common/openflow/messages/cofmsg_hello.h"

using namespace rofl::openflow;

cofmsg_hello::cofmsg_hello(
		uint8_t of_version,
		uint32_t xid,
		uint8_t* data,
		size_t datalen) :
	cofmsg(sizeof(struct openflow::ofp_header)),
	body((size_t)0)
{
	body.assign(data, datalen);

	set_version(of_version);
	set_length(sizeof(struct openflow::ofp_header) + datalen);
	set_xid(xid);

	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_HELLO);
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_HELLO);
	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_HELLO);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_hello::cofmsg_hello(
		cmemory *memarea) :
	cofmsg(memarea),
	body((size_t)0)
{

}



cofmsg_hello::cofmsg_hello(
		cofmsg_hello const& hello)
{
	*this = hello;
}



cofmsg_hello&
cofmsg_hello::operator= (
		cofmsg_hello const& hello)
{
	if (this == &hello)
		return *this;

	cofmsg::operator =(hello);

	body = hello.body;

	return *this;
}



cofmsg_hello::~cofmsg_hello()
{

}



void
cofmsg_hello::reset()
{
	cofmsg::reset();
	body.clear();
}



size_t
cofmsg_hello::length() const
{
	return (sizeof(struct openflow::ofp_header) + body.memlen());
}



void
cofmsg_hello::pack(uint8_t *buf, size_t buflen)
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
cofmsg_hello::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_hello::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	switch (get_version()) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION: {
		if (get_length() > sizeof(struct openflow::ofp_header)) {
			body.assign(sobody(), bodylen());
		}
	} break;
	case openflow13::OFP_VERSION: {
		if (get_length() > sizeof(struct openflow::ofp_header)) {
			body.assign(sobody(), bodylen());
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



rofl::cmemory&
cofmsg_hello::get_body()
{
	return body;
}


