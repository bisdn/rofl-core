/*
 * wlan_actions.cc
 *
 *  Created on: 29.06.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/experimental/actions/wlan_actions.h"

using namespace rofl::openflow::experimental::wlan;

size_t
cofaction_experimenter_wlan::length() const
{
	return sizeof(struct ofp_action_exp_wlan_hdr);
}



void
cofaction_experimenter_wlan::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_experimenter_wlan::length())
		throw eInval();

	struct ofp_action_exp_wlan_hdr* hdr = (struct ofp_action_exp_wlan_hdr*)buf;

	hdr->exptype = htobe16(exptype);
	hdr->explen  = htobe16(length());
}



void
cofaction_experimenter_wlan::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_experimenter_wlan::length())
		throw eInval();

	struct ofp_action_exp_wlan_hdr* hdr = (struct ofp_action_exp_wlan_hdr*)buf;

	uint16_t explen = be16toh(hdr->explen);

	if (explen < cofaction_experimenter_wlan::length())
		throw eInval();

	exptype	= be16toh(hdr->exptype);
}





size_t
cofaction_push_wlan::length() const
{
	return sizeof(struct ofp_action_exp_wlan_push_wlan);
}



void
cofaction_push_wlan::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	cofaction_experimenter_wlan::pack(buf, buflen);

	struct ofp_action_exp_wlan_push_wlan* hdr = (struct ofp_action_exp_wlan_push_wlan*)buf;

	hdr->ethertype 	= htobe16(ethertype);
}



void
cofaction_push_wlan::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_exp_wlan_push_wlan* hdr = (struct ofp_action_exp_wlan_push_wlan*)buf;

	cofaction_experimenter_wlan::unpack(buf, buflen);

	uint16_t explen = be16toh(hdr->explen);

	if (explen < length())
		throw eInval();

	ethertype	= be16toh(hdr->ethertype);
}




size_t
cofaction_pop_wlan::length() const
{
	return sizeof(struct ofp_action_exp_wlan_pop_wlan);
}



void
cofaction_pop_wlan::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	cofaction_experimenter_wlan::pack(buf, buflen);

	struct ofp_action_exp_wlan_pop_wlan* hdr = (struct ofp_action_exp_wlan_pop_wlan*)buf;

	hdr->ethertype 	= htobe16(ethertype);
}



void
cofaction_pop_wlan::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_exp_wlan_pop_wlan* hdr = (struct ofp_action_exp_wlan_pop_wlan*)buf;

	cofaction_experimenter_wlan::unpack(buf, buflen);

	uint16_t explen = be16toh(hdr->explen);

	if (explen < length())
		throw eInval();

	ethertype	= be16toh(hdr->ethertype);
}



