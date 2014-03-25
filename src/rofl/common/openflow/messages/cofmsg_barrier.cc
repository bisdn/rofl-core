#include "rofl/common/openflow/messages/cofmsg_barrier.h"

using namespace rofl::openflow;

cofmsg_barrier_request::cofmsg_barrier_request(
		uint8_t of_version,
		uint32_t xid,
		uint8_t* data,
		size_t datalen) :
	cofmsg(sizeof(struct openflow::ofp_header)),
	body((size_t)0)
{
	body.assign(data, datalen);

	set_version(of_version);
	set_length(sizeof(struct openflow::ofp_header));
	set_xid(xid);

	switch (of_version) {
	case openflow10::OFP_VERSION:
		set_type(openflow10::OFPT_BARRIER_REQUEST);
	break;
	case openflow12::OFP_VERSION:
		set_type(openflow12::OFPT_BARRIER_REQUEST);
	break;
	case openflow13::OFP_VERSION:
		set_type(openflow13::OFPT_BARRIER_REQUEST);
	break;
	default:
		throw eBadVersion();
	}
}



cofmsg_barrier_request::cofmsg_barrier_request(
		cmemory *memarea) :
	cofmsg(memarea),
	body((size_t)0)
{

}



cofmsg_barrier_request::cofmsg_barrier_request(
		cofmsg_barrier_request const& barrier)
{
	*this = barrier;
}



cofmsg_barrier_request&
cofmsg_barrier_request::operator= (
		cofmsg_barrier_request const& barrier)
{
	if (this == &barrier)
		return *this;

	cofmsg::operator =(barrier);

	body = barrier.body;

	return *this;
}



cofmsg_barrier_request::~cofmsg_barrier_request()
{

}



void
cofmsg_barrier_request::reset()
{
	cofmsg::reset();
	body.clear();
}



size_t
cofmsg_barrier_request::length() const
{
	return (sizeof(struct openflow::ofp_header) + body.memlen());
}



void
cofmsg_barrier_request::pack(uint8_t *buf, size_t buflen)
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
cofmsg_barrier_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_barrier_request::validate()
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



rofl::cmemory&
cofmsg_barrier_request::get_body()
{
	return body;
}




cofmsg_barrier_reply::cofmsg_barrier_reply(
		uint8_t of_version,
		uint32_t xid,
		uint8_t* data,
		size_t datalen) :
	cofmsg(sizeof(struct openflow::ofp_header)),
	body((size_t)0)
{
	body.assign(data, datalen);

	set_version(of_version);
	set_length(sizeof(struct openflow::ofp_header));
	set_xid(xid);

	switch (of_version) {
	case openflow10::OFP_VERSION:
		set_type(openflow10::OFPT_BARRIER_REPLY);
	break;
	case openflow12::OFP_VERSION:
		set_type(openflow12::OFPT_BARRIER_REPLY);
	break;
	case openflow13::OFP_VERSION:
		set_type(openflow13::OFPT_BARRIER_REPLY);
	break;
	default:
		throw eBadVersion();
	}
}



cofmsg_barrier_reply::cofmsg_barrier_reply(
		cmemory *memarea) :
	cofmsg(memarea),
	body((size_t)0)
{

}



cofmsg_barrier_reply::cofmsg_barrier_reply(
		cofmsg_barrier_reply const& barrier)
{
	*this = barrier;
}



cofmsg_barrier_reply&
cofmsg_barrier_reply::operator= (
		cofmsg_barrier_reply const& barrier)
{
	if (this == &barrier)
		return *this;

	cofmsg::operator =(barrier);

	body = barrier.body;

	return *this;
}



cofmsg_barrier_reply::~cofmsg_barrier_reply()
{

}



void
cofmsg_barrier_reply::reset()
{
	cofmsg::reset();
	body.clear();
}



size_t
cofmsg_barrier_reply::length() const
{
	return (sizeof(struct openflow::ofp_header) + body.memlen());
}



void
cofmsg_barrier_reply::pack(uint8_t *buf, size_t buflen)
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
cofmsg_barrier_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_barrier_reply::validate()
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



rofl::cmemory&
cofmsg_barrier_reply::get_body()
{
	return body;
}

