/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "rofl/common/openflow/cofaction.h"

using namespace rofl::openflow;


size_t
cofaction::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow::ofp_action) + body.memlen());
	} break;
	default:
		throw eBadVersion("cofaction::length() invalid version");
	}
}



void
cofaction::pack(
		uint8_t* buf, size_t buflen)
{
	len = length(); // use final length

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction::length())
		throw eInval("cofaction::pack() buflen too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct rofl::openflow::ofp_action* hdr = (struct rofl::openflow::ofp_action*)buf;

		hdr->type	= htobe16(type);
		hdr->len	= htobe16(len);

		if (body.length() > 0) {
			body.pack(hdr->body, body.length());
		}

	} break;
	default:
		throw eBadVersion("cofaction::pack() invalid version");
	}
}



void
cofaction::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	body.resize(0);

	if (buflen < cofaction::length())
		throw eInval("cofaction::unpack() buflen too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct rofl::openflow::ofp_action* hdr = (struct rofl::openflow::ofp_action*)buf;

		type	= be16toh(hdr->type);
		len		= be16toh(hdr->len);

		if (len < sizeof(struct rofl::openflow::ofp_action))
			throw eBadActionBadLen();

		if (len > sizeof(struct rofl::openflow::ofp_action)) {
			body.unpack(hdr->body, len - sizeof(struct rofl::openflow::ofp_action));
		}

	} break;
	default:
		throw eBadVersion("cofaction::unpack() invalid version");
	}
}



void
cofaction_output::check_prerequisites() const
{
	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (0 == port_no) {
			throw eBadActionBadOutPort();
		}
	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		if ((rofl::openflow13::OFPP_ANY == port_no) || (0 == port_no)) {
			throw eBadActionBadOutPort();
		}
	} break;
	}
}



size_t
cofaction_output::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_output);
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION:
		return sizeof(struct rofl::openflow13::ofp_action_output);
	default:
		throw eBadVersion("cofaction_output::length() invalid version");
	}
}



void
cofaction_output::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_output::length())
		throw eInval("cofaction_output::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_output* hdr = (struct rofl::openflow10::ofp_action_output*)buf;

		hdr->port		= htobe16((uint16_t)port_no);
		hdr->max_len	= htobe16(max_len);

	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct rofl::openflow13::ofp_action_output* hdr = (struct rofl::openflow13::ofp_action_output*)buf;

		hdr->port		= htobe32(port_no);
		hdr->max_len	= htobe16(max_len);

	} break;
	default:
		throw eBadVersion("cofaction_output::pack() invalid version");
	}
}



void
cofaction_output::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_output::length())
		throw eInval("cofaction_output::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_output* hdr = (struct rofl::openflow10::ofp_action_output*)buf;

		port_no 	= be16toh(hdr->port);
		max_len		= be16toh(hdr->max_len);

	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct rofl::openflow13::ofp_action_output* hdr = (struct rofl::openflow13::ofp_action_output*)buf;

		port_no		= be32toh(hdr->port);
		max_len		= be32toh(hdr->max_len);

	} break;
	default:
		throw eBadVersion("cofaction_output::unpack() invalid version");
	}
}





size_t
cofaction_set_vlan_vid::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_vlan_vid);
	default:
		throw eBadVersion("cofaction_set_vlan_vid::length() invalid version");
	}
}



void
cofaction_set_vlan_vid::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_vlan_vid::length())
		throw eInval("cofaction_set_vlan_vid::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_vlan_vid* hdr = (struct rofl::openflow10::ofp_action_vlan_vid*)buf;

		hdr->vlan_vid	= htobe16(vlan_vid);

	} break;
	default:
		throw eBadVersion("cofaction_set_vlan_vid::pack() invalid version");
	}
}



void
cofaction_set_vlan_vid::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_vlan_vid::length())
		throw eInval("cofaction_set_vlan_vid::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_vlan_vid* hdr = (struct rofl::openflow10::ofp_action_vlan_vid*)buf;

		vlan_vid	= be16toh(hdr->vlan_vid);

	} break;
	default:
		throw eBadVersion("cofaction_set_vlan_vid::unpack() invalid version");
	}
}



size_t
cofaction_set_vlan_pcp::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_vlan_pcp);
	default:
		throw eBadVersion("cofaction_set_vlan_pcp::length() invalid version");
	}
}



void
cofaction_set_vlan_pcp::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_vlan_pcp::length())
		throw eInval("cofaction_set_vlan_pcp::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_vlan_pcp* hdr = (struct rofl::openflow10::ofp_action_vlan_pcp*)buf;

		hdr->vlan_pcp	= vlan_pcp;

	} break;
	default:
		throw eBadVersion("cofaction_set_vlan_pcp::pack() invalid version");
	}
}



void
cofaction_set_vlan_pcp::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_vlan_pcp::length())
		throw eInval("cofaction_set_vlan_pcp::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_vlan_pcp* hdr = (struct rofl::openflow10::ofp_action_vlan_pcp*)buf;

		vlan_pcp	= hdr->vlan_pcp;

	} break;
	default:
		throw eBadVersion("cofaction_set_vlan_pcp::unpack() invalid version");
	}
}



size_t
cofaction_strip_vlan::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_header);
	default:
		throw eBadVersion("cofaction_strip_vlan::length() invalid version");
	}
}



void
cofaction_strip_vlan::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_strip_vlan::length())
		throw eInval("cofaction_strip_vlan::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

	} break;
	default:
		throw eBadVersion("cofaction_strip_vlan::pack() invalid version");
	}
}



void
cofaction_strip_vlan::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_strip_vlan::length())
		throw eInval("cofaction_strip_vlan::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

	} break;
	default:
		throw eBadVersion("cofaction_strip_vlan::unpack() invalid version");
	}
}



size_t
cofaction_set_dl_src::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_dl_addr);
	default:
		throw eBadVersion("cofaction_set_dl_src::length() invalid version");
	}
}



void
cofaction_set_dl_src::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_dl_src::length())
		throw eInval("cofaction_set_dl_src::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_dl_addr* hdr = (struct rofl::openflow10::ofp_action_dl_addr*)buf;

		macaddr.pack(hdr->dl_addr, OFP_ETH_ALEN);

	} break;
	default:
		throw eBadVersion("cofaction_set_dl_src::pack() invalid version");
	}
}



void
cofaction_set_dl_src::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_dl_src::length())
		throw eInval("cofaction_set_dl_src::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_dl_addr* hdr = (struct rofl::openflow10::ofp_action_dl_addr*)buf;

		macaddr.unpack(hdr->dl_addr, OFP_ETH_ALEN);

	} break;
	default:
		throw eBadVersion("cofaction_set_dl_src::unpack() invalid version");
	}
}



size_t
cofaction_set_dl_dst::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_dl_addr);
	default:
		throw eBadVersion("cofaction_set_dl_dst::length() invalid version");
	}
}



void
cofaction_set_dl_dst::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_dl_dst::length())
		throw eInval("cofaction_set_dl_dst::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_dl_addr* hdr = (struct rofl::openflow10::ofp_action_dl_addr*)buf;

		macaddr.pack(hdr->dl_addr, OFP_ETH_ALEN);

	} break;
	default:
		throw eBadVersion("cofaction_set_dl_dst::pack() invalid version");
	}
}



void
cofaction_set_dl_dst::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_dl_dst::length())
		throw eInval("cofaction_set_dl_dst::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_dl_addr* hdr = (struct rofl::openflow10::ofp_action_dl_addr*)buf;

		macaddr.unpack(hdr->dl_addr, OFP_ETH_ALEN);

	} break;
	default:
		throw eBadVersion("cofaction_set_dl_dst::unpack() invalid version");
	}
}



size_t
cofaction_set_nw_src::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_nw_addr);
	default:
		throw eBadVersion("cofaction_set_nw_src::length() invalid version");
	}
}



void
cofaction_set_nw_src::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_nw_src::length())
		throw eInval("cofaction_set_nw_src::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_nw_addr* hdr = (struct rofl::openflow10::ofp_action_nw_addr*)buf;

		hdr->nw_addr = addr.ca_s4addr->sin_addr.s_addr;

	} break;
	default:
		throw eBadVersion("cofaction_set_nw_src::pack() invalid version");
	}
}



void
cofaction_set_nw_src::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_nw_src::length())
		throw eInval("cofaction_set_nw_src::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_nw_addr* hdr = (struct rofl::openflow10::ofp_action_nw_addr*)buf;

		addr.resize(sizeof(struct sockaddr_in));
		addr.ca_s4addr->sin_family = AF_INET;
		addr.ca_s4addr->sin_addr.s_addr = hdr->nw_addr;

	} break;
	default:
		throw eBadVersion("cofaction_set_nw_src::unpack() invalid version");
	}
}



size_t
cofaction_set_nw_dst::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_nw_addr);
	default:
		throw eBadVersion("cofaction_set_nw_dst::length() invalid version");
	}
}



void
cofaction_set_nw_dst::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_nw_dst::length())
		throw eInval("cofaction_set_nw_dst::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_nw_addr* hdr = (struct rofl::openflow10::ofp_action_nw_addr*)buf;

		hdr->nw_addr = addr.ca_s4addr->sin_addr.s_addr;

	} break;
	default:
		throw eBadVersion("cofaction_set_nw_dst::pack() invalid version");
	}
}



void
cofaction_set_nw_dst::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_nw_dst::length())
		throw eInval("cofaction_set_nw_dst::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_nw_addr* hdr = (struct rofl::openflow10::ofp_action_nw_addr*)buf;

		addr.resize(sizeof(struct sockaddr_in));
		addr.ca_s4addr->sin_family = AF_INET;
		addr.ca_s4addr->sin_addr.s_addr = hdr->nw_addr;

	} break;
	default:
		throw eBadVersion("cofaction_set_nw_dst::unpack() invalid version");
	}
}



size_t
cofaction_set_nw_tos::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_nw_tos);
	default:
		throw eBadVersion("cofaction_set_nw_tos::length() invalid version");
	}
}



void
cofaction_set_nw_tos::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_nw_tos::length())
		throw eInval("cofaction_set_nw_tos::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_nw_tos* hdr = (struct rofl::openflow10::ofp_action_nw_tos*)buf;

		hdr->nw_tos	= nw_tos;

	} break;
	default:
		throw eBadVersion("cofaction_set_nw_tos::pack() invalid version");
	}
}



void
cofaction_set_nw_tos::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_nw_tos::length())
		throw eInval("cofaction_set_nw_tos::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_nw_tos* hdr = (struct rofl::openflow10::ofp_action_nw_tos*)buf;

		nw_tos = hdr->nw_tos;

	} break;
	default:
		throw eBadVersion("cofaction_set_nw_tos::unpack() invalid version");
	}
}



size_t
cofaction_set_tp_src::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_tp_port);
	default:
		throw eBadVersion("cofaction_set_tp_src::length() invalid version");
	}
}



void
cofaction_set_tp_src::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_tp_src::length())
		throw eInval("cofaction_set_tp_src::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_tp_port* hdr = (struct rofl::openflow10::ofp_action_tp_port*)buf;

		hdr->tp_port = htobe16(tp_src);

	} break;
	default:
		throw eBadVersion("cofaction_set_tp_src::pack() invalid version");
	}
}



void
cofaction_set_tp_src::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_tp_src::length())
		throw eInval("cofaction_set_tp_src::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_tp_port* hdr = (struct rofl::openflow10::ofp_action_tp_port*)buf;

		tp_src = be16toh(hdr->tp_port);

	} break;
	default:
		throw eBadVersion("cofaction_set_tp_src::unpack() invalid version");
	}
}



size_t
cofaction_set_tp_dst::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_tp_port);
	default:
		throw eBadVersion("cofaction_set_tp_dst::length() invalid version");
	}
}



void
cofaction_set_tp_dst::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_tp_dst::length())
		throw eInval("cofaction_set_tp_dst::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_tp_port* hdr = (struct rofl::openflow10::ofp_action_tp_port*)buf;

		hdr->tp_port = htobe16(tp_dst);

	} break;
	default:
		throw eBadVersion("cofaction_set_tp_dst::pack() invalid version");
	}
}



void
cofaction_set_tp_dst::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_set_tp_dst::length())
		throw eInval("cofaction_set_tp_dst::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_tp_port* hdr = (struct rofl::openflow10::ofp_action_tp_port*)buf;

		tp_dst = be16toh(hdr->tp_port);

	} break;
	default:
		throw eBadVersion("cofaction_set_tp_dst::unpack() invalid version");
	}
}



size_t
cofaction_enqueue::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_enqueue);
	default:
		throw eBadVersion("cofaction_enqueue::length() invalid version");
	}
}



void
cofaction_enqueue::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_enqueue::length())
		throw eInval("cofaction_enqueue::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_enqueue* hdr = (struct rofl::openflow10::ofp_action_enqueue*)buf;

		hdr->port		= htobe16(port_no);
		hdr->queue_id	= htobe32(queue_id):

	} break;
	default:
		throw eBadVersion("cofaction_enqueue::pack() invalid version");
	}
}



void
cofaction_enqueue::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_enqueue::length())
		throw eInval("cofaction_enqueue::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_enqueue* hdr = (struct rofl::openflow10::ofp_action_enqueue*)buf;

		port_no		= be16toh(hdr->port);
		queue_id	= be32toh(hdr->queue_id);

	} break;
	default:
		throw eBadVersion("cofaction_enqueue::unpack() invalid version");
	}
}



size_t
cofaction_vendor::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
		return sizeof(struct rofl::openflow10::ofp_action_vendor_header) + exp_body.length();
	default:
		throw eBadVersion("cofaction_vendor::length() invalid version");
	}
}



void
cofaction_vendor::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_vendor::length())
		throw eInval("cofaction_vendor::pack() buflen too short");

	cofaction::pack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_vendor_header* hdr = (struct rofl::openflow10::ofp_action_vendor_header*)buf;

		hdr->vendor		= htobe32(exp_id);

		exp_body.pack(hdr->data, exp_body.length())

	} break;
	default:
		throw eBadVersion("cofaction_vendor::pack() invalid version");
	}
}



void
cofaction_vendor::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	exp_body.resize(0);

	if (buflen < cofaction_vendor::length())
		throw eInval("cofaction_vendor::unpack() buflen too short");

	cofaction::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_action_vendor_header* hdr = (struct rofl::openflow10::ofp_action_vendor_header*)buf;

		exp_id		= be32toh(hdr->vendor);

		if (get_length() > sizeof(struct rofl::openflow10::ofp_action_vendor_header)) {
			exp_body.unpack(hdr->data, get_length() > sizeof(struct rofl::openflow10::ofp_action_vendor_header));
		}

	} break;
	default:
		throw eBadVersion("cofaction_vendor::unpack() invalid version");
	}
}





#if 0


coxmatch
cofaction::get_oxm() const
{
	if (openflow::OFPAT_SET_FIELD != get_type()) {
		throw eActionInvalType();
	}

	struct openflow::ofp_oxm_hdr* oxm_hdr = (struct openflow::ofp_oxm_hdr*)oac_12set_field->field;
	size_t oxm_len = be16toh(oac_12set_field->len) - 2 * sizeof(uint16_t); // without action header

	coxmatch oxm((uint8_t*)oxm_hdr, oxm_len);

	return oxm;
}


#endif


