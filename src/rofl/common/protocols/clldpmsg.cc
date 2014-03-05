/*
 * clldpmsg.cc
 *
 *  Created on: 04.03.2014
 *      Author: andreas
 */

#include "rofl/common/protocols/clldpmsg.h"

using namespace rofl::protocol::lldp;


clldpmsg::clldpmsg()
{
	set_eth_dst() = cmacaddr("01:80:c2:00:00:00"); // TODO: other destination addresses defined by 802.1AB
}


clldpmsg::~clldpmsg()
{

}


clldpmsg::clldpmsg(clldpmsg const& msg)
{
	*this = msg;
}


clldpmsg&
clldpmsg::operator= (clldpmsg const& msg)
{
	if (this == &msg)
		return *this;

	attrs = msg.attrs;

	return *this;
}


size_t
clldpmsg::length() const
{
	return (sizeof(sizeof(struct lldp_hdr_t)) + attrs.length());
}


void
clldpmsg::pack(uint8_t *buf, size_t buflen)
{
	if (buflen < length()) {
		throw eLLDPMsgInval();
	}

	struct lldp_hdr_t *hdr = (struct lldp_hdr_t*)buf;
	memcpy(hdr->eth_dst, eth_dst.somem(), eth_dst.memlen());
	memcpy(hdr->eth_src, eth_src.somem(), eth_src.memlen());
	hdr->eth_type = htobe16(0x88CC);
	attrs.pack(buf + sizeof(struct lldp_hdr_t), buflen - sizeof(struct lldp_hdr_t));
}


void
clldpmsg::unpack(uint8_t *buf, size_t buflen)
{
	if (buflen < sizeof(struct lldp_hdr_t)) {
		throw eLLDPMsgInval();
	}

	struct lldp_hdr_t *hdr = (struct lldp_hdr_t*)buf;
	memcpy(eth_dst.somem(), hdr->eth_dst, eth_dst.memlen());
	memcpy(eth_src.somem(), hdr->eth_src, eth_src.memlen());
	if (be16toh(hdr->eth_type) != 0x88CC) {
		throw eLLDPMsgInval();
	}
	attrs.unpack(buf + sizeof(struct lldp_hdr_t), buflen - sizeof(struct lldp_hdr_t));
}

