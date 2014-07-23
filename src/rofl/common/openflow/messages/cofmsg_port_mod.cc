/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_port_mod.h"

using namespace rofl::openflow;

cofmsg_port_mod::cofmsg_port_mod(
		uint8_t of_version,
		uint32_t xid,
		uint32_t port_no,
		cmacaddr const& hwaddr,
		uint32_t config,
		uint32_t mask,
		uint32_t advertise) :
	cofmsg(sizeof(struct openflow::ofp_header))
{
	ofh_port_mod = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_PORT_MOD);
		resize(sizeof(struct openflow10::ofp_port_mod));

		ofh10_port_mod->port_no			= htobe16((uint16_t)(port_no & 0x0000ffff));
		ofh10_port_mod->config			= htobe32(config);
		ofh10_port_mod->mask			= htobe32(mask);
		ofh10_port_mod->advertise		= htobe32(advertise);
		memcpy(ofh10_port_mod->hw_addr, hwaddr.somem(), OFP_ETH_ALEN);
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_PORT_MOD);
		resize(sizeof(struct openflow12::ofp_port_mod));

		ofh12_port_mod->port_no			= htobe32(port_no);
		ofh12_port_mod->config			= htobe32(config);
		ofh12_port_mod->mask			= htobe32(mask);
		ofh12_port_mod->advertise		= htobe32(advertise);
		memcpy(ofh12_port_mod->hw_addr, hwaddr.somem(), OFP_ETH_ALEN);
	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_PORT_MOD);
		resize(sizeof(struct openflow13::ofp_port_mod));

		ofh13_port_mod->port_no			= htobe32(port_no);
		ofh13_port_mod->config			= htobe32(config);
		ofh13_port_mod->mask			= htobe32(mask);
		ofh13_port_mod->advertise		= htobe32(advertise);
		memcpy(ofh13_port_mod->hw_addr, hwaddr.somem(), OFP_ETH_ALEN);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_port_mod::cofmsg_port_mod(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_port_mod = soframe();
}



cofmsg_port_mod::cofmsg_port_mod(
		cofmsg_port_mod const& port_mod)
{
	*this = port_mod;
}



cofmsg_port_mod&
cofmsg_port_mod::operator= (
		cofmsg_port_mod const& port_mod)
{
	if (this == &port_mod)
		return *this;

	cofmsg::operator =(port_mod);

	ofh_port_mod = soframe();

	return *this;
}



cofmsg_port_mod::~cofmsg_port_mod()
{

}



void
cofmsg_port_mod::reset()
{
	cofmsg::reset();
}



uint8_t*
cofmsg_port_mod::resize(size_t len)
{
	return (ofh_port_mod = cofmsg::resize(len));
}



size_t
cofmsg_port_mod::length() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return sizeof(struct openflow10::ofp_port_mod);
	} break;
	case openflow12::OFP_VERSION: {
		return sizeof(struct openflow12::ofp_port_mod);
	} break;
	case openflow13::OFP_VERSION: {
		return sizeof(struct openflow13::ofp_port_mod);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_mod::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct openflow10::ofp_port_mod))
			throw eInval();
		memcpy(buf, soframe(), framelen());
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct openflow12::ofp_port_mod))
			throw eInval();
		memcpy(buf, soframe(), framelen());
	} break;
	case openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct openflow13::ofp_port_mod))
			throw eInval();
		memcpy(buf, soframe(), framelen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_port_mod::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_port_mod::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_port_mod = soframe();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow10::ofp_port_mod))
			throw eBadSyntaxTooShort();
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow12::ofp_port_mod))
			throw eBadSyntaxTooShort();
	} break;
	case openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow13::ofp_port_mod))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint32_t
cofmsg_port_mod::get_port_no() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (uint32_t)be16toh(ofh10_port_mod->port_no);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port_mod->port_no);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port_mod->port_no);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_mod::set_port_no(uint32_t port_no)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_port_mod->port_no = htobe16((uint16_t)(port_no & 0x0000ffff));
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_port_mod->port_no = htobe32(port_no);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port_mod->port_no = htobe32(port_no);
	} break;
	default:
		throw eBadVersion();
	}
}



rofl::cmacaddr
cofmsg_port_mod::get_hwaddr() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return cmacaddr(ofh10_port_mod->hw_addr, OFP_ETH_ALEN);
	} break;
	case openflow12::OFP_VERSION: {
		return cmacaddr(ofh12_port_mod->hw_addr, OFP_ETH_ALEN);
	} break;
	case openflow13::OFP_VERSION: {
		return cmacaddr(ofh13_port_mod->hw_addr, OFP_ETH_ALEN);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_port_mod::set_hwaddr(cmacaddr const& hwaddr)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		memcpy(ofh10_port_mod->hw_addr, hwaddr.somem(), hwaddr.memlen());
	} break;
	case openflow12::OFP_VERSION: {
		memcpy(ofh12_port_mod->hw_addr, hwaddr.somem(), hwaddr.memlen());
	} break;
	case openflow13::OFP_VERSION: {
		memcpy(ofh13_port_mod->hw_addr, hwaddr.somem(), hwaddr.memlen());
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_port_mod::get_config() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_port_mod->config);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port_mod->config);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port_mod->config);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_mod::set_config(uint32_t config)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_port_mod->config = htobe32(config);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_port_mod->config = htobe32(config);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port_mod->config = htobe32(config);
	} break;
	default:
		throw eBadVersion();
	}
}




uint32_t
cofmsg_port_mod::get_mask() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_port_mod->mask);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port_mod->mask);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port_mod->mask);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_mod::set_mask(uint32_t mask)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_port_mod->mask = htobe32(mask);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_port_mod->mask = htobe32(mask);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port_mod->mask = htobe32(mask);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_port_mod::get_advertise() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_port_mod->advertise);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port_mod->advertise);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port_mod->advertise);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_mod::set_advertise(uint32_t advertise)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_port_mod->advertise = htobe32(advertise);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_port_mod->advertise = htobe32(advertise);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port_mod->advertise = htobe32(advertise);
	} break;
	default:
		throw eBadVersion();
	}
}


