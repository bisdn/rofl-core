#include "cofmsg_flow_mod.h"

using namespace rofl::openflow;

cofmsg_flow_mod::cofmsg_flow_mod(
		uint8_t ofp_version, uint32_t xid, const cofflowmod& flowmod) :
				cofmsg(ofp_version, xid, rofl::openflow::OFPT_FLOW_MOD),
				flowmod(flowmod)
{
	this->flowmod.set_version(ofp_version);
}



cofmsg_flow_mod::cofmsg_flow_mod(
		cmemory *memarea) :
	cofmsg(memarea),
	flowmod(get_version())
{

}



cofmsg_flow_mod::cofmsg_flow_mod(
		const cofmsg_flow_mod& flowmod)
{
	*this = flowmod;
}



cofmsg_flow_mod&
cofmsg_flow_mod::operator= (
		const cofmsg_flow_mod& msg)
{
	if (this == &msg)
		return *this;

	cofmsg::operator= (msg);

	flowmod	= msg.flowmod;

	return *this;
}



cofmsg_flow_mod::~cofmsg_flow_mod()
{

}



void
cofmsg_flow_mod::reset()
{
	cofmsg::reset();
	flowmod.clear();
}



size_t
cofmsg_flow_mod::length() const
{
	switch (ofh_header->version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return sizeof(struct rofl::openflow::ofp_header) + flowmod.length();
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_mod::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {

		cofmsg::pack(buf, buflen);

		struct rofl::openflow::ofp_header* hdr = (struct rofl::openflow::ofp_header*)buf;

		flowmod.pack(hdr->body, flowmod.length());

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_flow_mod::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_flow_mod::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	flowmod.clear();

	switch (get_version()) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {

		if (cofmsg::framelen() < length())
			throw eInval("cofmsg_flow_mod::validate() framelen too short");

		struct rofl::openflow::ofp_header* hdr = (struct rofl::openflow::ofp_header*)(cofmsg::soframe());

		size_t flowmodlen = cofmsg::framelen() - sizeof(struct rofl::openflow::ofp_header);

		if (flowmodlen > 0) {
			flowmod.unpack(hdr->body, flowmodlen);
		}

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



void
cofmsg_flow_mod::check_prerequisites() const
{
	flowmod.check_prerequisites();
}



