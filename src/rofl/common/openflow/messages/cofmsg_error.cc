#include "cofmsg_error.h"

using namespace rofl;

cofmsg_error::cofmsg_error(
		uint8_t of_version,
		uint32_t xid,
		uint16_t err_type,
		uint16_t err_code,
		uint8_t* data,
		size_t datalen) :
	cofmsg(sizeof(struct ofp_error_msg)),
	body(0)
{
	err_msg = (struct ofp_error_msg*)soframe();

	cofmsg::resize(sizeof(struct ofp_error_msg) + datalen);
	body.assign(data, datalen);

	set_version(of_version);
	set_length(sizeof(struct ofp_error_msg) + datalen);
	set_type(OFPT_ERROR);
	set_xid(xid);

	set_err_type(err_type);
	set_err_code(err_code);
}



cofmsg_error::cofmsg_error(
		cmemory *memarea) :
	cofmsg(memarea)
{
	err_msg = (struct ofp_error_msg*)soframe();
}



cofmsg_error::cofmsg_error(
		cofmsg_error const& error)
{
	*this = error;
}



cofmsg_error&
cofmsg_error::operator= (
		cofmsg_error const& error)
{
	if (this == &error)
		return *this;

	cofmsg::operator =(error);

	body = error.body;

	err_msg = (struct ofp_error_msg*)soframe();

	return *this;
}



cofmsg_error::~cofmsg_error()
{

}



void
cofmsg_error::reset()
{
	cofmsg::reset();
	body.clear();
}



size_t
cofmsg_error::length()
{
	return (sizeof(struct ofp_error_msg) + body.memlen());
}



void
cofmsg_error::pack(uint8_t *buf, size_t buflen)
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
cofmsg_error::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_error::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	err_msg = (struct ofp_error_msg*)soframe();

	switch (get_version()) {
	case OFP10_VERSION:
	case OFP12_VERSION:
	case OFP13_VERSION: {
		if (get_length() < sizeof(struct ofp_error_msg))
			throw eBadSyntaxTooShort();
		if (get_length() > sizeof(struct ofp_error_msg)) {
			body.assign(soframe() + sizeof(struct ofp_error_msg), framelen() - sizeof(struct ofp_error_msg));
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint16_t
cofmsg_error::get_err_type() const
{
	return be16toh(err_msg->type);
}



void
cofmsg_error::set_err_type(uint16_t type)
{
	err_msg->type = htobe16(type);
}



uint16_t
cofmsg_error::get_err_code() const
{
	return be16toh(err_msg->code);
}



void
cofmsg_error::set_err_code(uint16_t code)
{
	err_msg->code = htobe16(code);
}



cmemory&
cofmsg_error::get_body()
{
	return body;
}


