#include "cofmsg_port_status.h"

using namespace rofl;

cofmsg_port_status::cofmsg_port_status(
		uint8_t of_version,
		uint32_t xid,
		uint8_t reason,
		cofport const& port) :
	cofmsg(sizeof(struct ofp_header)),
	port(port)
{
	ofh_port_status = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (get_version()) {
	case OFP10_VERSION: {
		set_type(OFPT10_PORT_STATUS);
		resize(sizeof(struct ofp10_port_status));
		set_length(sizeof(struct ofp10_port_status));

		ofh10_port_status->reason			= reason;
		port.pack(&(ofh10_port_status->desc), sizeof(struct ofp10_port));
	} break;
	case OFP12_VERSION: {
		set_type(OFPT12_PORT_STATUS);
		resize(sizeof(struct ofp12_port_status));
		set_length(sizeof(struct ofp12_port_status));

		ofh12_port_status->reason			= reason;
		port.pack(&(ofh12_port_status->desc), sizeof(struct ofp12_port));
	} break;
	case OFP13_VERSION: {
		set_type(OFPT13_PORT_STATUS);
		resize(sizeof(struct ofp13_port_status));
		set_length(sizeof(struct ofp13_port_status));

		ofh13_port_status->reason			= reason;
		port.pack(&(ofh13_port_status->desc), sizeof(struct ofp13_port));
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_port_status::cofmsg_port_status(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_port_status = soframe();
}



cofmsg_port_status::cofmsg_port_status(
		cofmsg_port_status const& port_status)
{
	*this = port_status;
}



cofmsg_port_status&
cofmsg_port_status::operator= (
		cofmsg_port_status const& port_status)
{
	if (this == &port_status)
		return *this;

	cofmsg::operator =(port_status);

	ofh_port_status = soframe();

	port 	= port_status.port;

	return *this;
}



cofmsg_port_status::~cofmsg_port_status()
{

}



void
cofmsg_port_status::reset()
{
	cofmsg::reset();
}



void
cofmsg_port_status::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_port_status = soframe();
}



size_t
cofmsg_port_status::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return sizeof(struct ofp10_port_status);
	} break;
	case OFP12_VERSION: {
		return sizeof(struct ofp12_port_status);
	} break;
	case OFP13_VERSION: {
		return sizeof(struct ofp13_port_status);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_status::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofh_header->version) {
	case OFP10_VERSION: {
		port.pack(&(ofh10_port_status->desc), sizeof(struct ofp10_port));
		memcpy(buf, soframe(), framelen());
	} break;
	case OFP12_VERSION: {
		port.pack(&(ofh12_port_status->desc), sizeof(struct ofp12_port));
		memcpy(buf, soframe(), framelen());
	} break;
	case OFP13_VERSION: {
		port.pack(&(ofh13_port_status->desc), sizeof(struct ofp13_port));
		memcpy(buf, soframe(), framelen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_port_status::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_port_status::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_port_status = soframe();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_port_status))
			throw eBadSyntaxTooShort();

		port.unpack(&(ofh10_port_status->desc), sizeof(struct ofp10_port));
	} break;
	case OFP12_VERSION: {
		if (get_length() < sizeof(struct ofp12_port_status))
			throw eBadSyntaxTooShort();

		port.unpack(&(ofh12_port_status->desc), sizeof(struct ofp12_port));
	} break;
	case OFP13_VERSION: {
		if (get_length() < sizeof(struct ofp13_port_status))
			throw eBadSyntaxTooShort();

		port.unpack(&(ofh13_port_status->desc), sizeof(struct ofp13_port));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint8_t
cofmsg_port_status::get_reason() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (ofh10_port_status->reason);
	} break;
	case OFP12_VERSION: {
		return (ofh12_port_status->reason);
	} break;
	case OFP13_VERSION: {
		return (ofh13_port_status->reason);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_status::set_reason(uint8_t reason)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_port_status->reason = (reason);
	} break;
	case OFP12_VERSION: {
		ofh12_port_status->reason = (reason);
	} break;
	case OFP13_VERSION: {
		ofh13_port_status->reason = (reason);
	} break;
	default:
		throw eBadVersion();
	}
}



cofport&
cofmsg_port_status::get_port()
{
	return port;
}


cofport const&
cofmsg_port_status::get_port_const() const
{
	return port;
}



