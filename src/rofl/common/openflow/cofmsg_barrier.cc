#include "cofmsg_barrier.h"

using namespace rofl;

cofmsg_barrier::cofmsg_barrier(
		uint8_t of_version,
		uint8_t type,
		uint32_t xid,
		uint8_t* data,
		size_t datalen) :
	cofmsg(sizeof(struct ofp_header))
{
	body.assign(data, datalen);

	set_version(of_version);
	set_length(sizeof(struct ofp_header));
	set_type(type);
	set_xid(xid);
}



cofmsg_barrier::cofmsg_barrier(
		cmemory *memarea) :
	cofmsg(memarea)
{
	validate();
}



cofmsg_barrier::cofmsg_barrier(
		cofmsg_barrier const& barrier)
{
	*this = barrier;
}



cofmsg_barrier&
cofmsg_barrier::operator= (
		cofmsg_barrier const& barrier)
{
	if (this == &barrier)
		return *this;

	cofmsg::operator =(barrier);

	body = barrier.body;

	return *this;
}



cofmsg_barrier::~cofmsg_barrier()
{

}



void
cofmsg_barrier::reset()
{
	cofmsg::reset();
	body.clear();
}



size_t
cofmsg_barrier::length() const
{
	return (sizeof(struct ofp_header) + body.memlen());
}



void
cofmsg_barrier::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eInval();

	cofmsg::pack(buf, buflen);

	memcpy(buf + sizeof(struct ofp_header), body.somem(), body.memlen());
}



void
cofmsg_barrier::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_barrier::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	switch (get_version()) {
	case OFP10_VERSION:
	case OFP12_VERSION:
	case OFP13_VERSION: {
		if (get_length() > sizeof(struct ofp_header)) {
			body.assign(sobody(), bodylen());
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cmemory&
cofmsg_barrier::get_body()
{
	return body;
}


