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

	body.clear();

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


