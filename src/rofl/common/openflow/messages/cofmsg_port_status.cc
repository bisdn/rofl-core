#include "rofl/common/openflow/messages/cofmsg_port_status.h"

using namespace rofl::openflow;

cofmsg_port_status::cofmsg_port_status(
		uint8_t of_version,
		uint32_t xid,
		uint8_t reason,
		rofl::openflow::cofport const& port) :
	cofmsg(sizeof(struct rofl::openflow::ofp_header)),
	port(port)
{
	ofh_port_status = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		set_type(rofl::openflow10::OFPT_PORT_STATUS);
		resize(sizeof(struct rofl::openflow10::ofp_port_status));
		set_length(sizeof(struct rofl::openflow10::ofp_port_status));

		ofh10_port_status->reason			= reason;
		this->port.pack((uint8_t*)&(ofh10_port_status->desc), sizeof(struct rofl::openflow10::ofp_port));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_PORT_STATUS);
		resize(sizeof(struct rofl::openflow12::ofp_port_status));
		set_length(sizeof(struct rofl::openflow12::ofp_port_status));

		ofh12_port_status->reason			= reason;
		this->port.pack((uint8_t*)&(ofh12_port_status->desc), sizeof(struct rofl::openflow12::ofp_port));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_PORT_STATUS);
		resize(sizeof(struct rofl::openflow13::ofp_port_status));
		set_length(sizeof(struct rofl::openflow13::ofp_port_status));

		ofh13_port_status->reason			= reason;
		this->port.pack((uint8_t*)&(ofh13_port_status->desc), sizeof(struct rofl::openflow13::ofp_port));
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_port_status::cofmsg_port_status(
		cmemory *memarea) :
	cofmsg(memarea),
	port(get_version())
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



uint8_t*
cofmsg_port_status::resize(size_t len)
{
	return (ofh_port_status = cofmsg::resize(len));
}



size_t
cofmsg_port_status::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return sizeof(struct rofl::openflow10::ofp_port_status);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return sizeof(struct rofl::openflow12::ofp_port_status);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return sizeof(struct rofl::openflow13::ofp_port_status);
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
	case rofl::openflow10::OFP_VERSION: {
		port.pack((uint8_t*)&(ofh10_port_status->desc), sizeof(struct rofl::openflow10::ofp_port));
		memcpy(buf, soframe(), framelen());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		port.pack((uint8_t*)&(ofh12_port_status->desc), sizeof(struct rofl::openflow12::ofp_port));
		memcpy(buf, soframe(), framelen());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		port.pack((uint8_t*)&(ofh13_port_status->desc), sizeof(struct rofl::openflow13::ofp_port));
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
	case rofl::openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow10::ofp_port_status))
			throw eBadSyntaxTooShort();

		port.unpack((uint8_t*)&(ofh10_port_status->desc), sizeof(struct rofl::openflow10::ofp_port));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow12::ofp_port_status))
			throw eBadSyntaxTooShort();

		port.unpack((uint8_t*)&(ofh12_port_status->desc), sizeof(struct rofl::openflow12::ofp_port));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow13::ofp_port_status))
			throw eBadSyntaxTooShort();

		port.unpack((uint8_t*)&(ofh13_port_status->desc), sizeof(struct rofl::openflow13::ofp_port));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint8_t
cofmsg_port_status::get_reason() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (ofh10_port_status->reason);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (ofh12_port_status->reason);
	} break;
	case rofl::openflow13::OFP_VERSION: {
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
	case rofl::openflow10::OFP_VERSION: {
		ofh10_port_status->reason = (reason);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh12_port_status->reason = (reason);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh13_port_status->reason = (reason);
	} break;
	default:
		throw eBadVersion();
	}
}





