/*
 * capwap_actions.cc
 *
 *  Created on: 29.06.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/experimental/actions/capwap_actions.h"


using namespace rofl::openflow::experimental::capwap;




size_t
cofaction_experimenter_capwap::length() const
{
	return sizeof(struct ofp_action_exp_capwap_hdr);
}



void
cofaction_experimenter_capwap::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_experimenter_capwap::length())
		throw eInval();

	struct ofp_action_exp_capwap_hdr* hdr = (struct ofp_action_exp_capwap_hdr*)buf;

	hdr->exptype = htobe16(exptype);
	hdr->explen  = htobe16(length());
}



void
cofaction_experimenter_capwap::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_experimenter_capwap::length())
		throw eInval();

	struct ofp_action_exp_capwap_hdr* hdr = (struct ofp_action_exp_capwap_hdr*)buf;

	uint16_t explen = be16toh(hdr->explen);

	if (explen < cofaction_experimenter_capwap::length())
		throw eInval();

	exptype	= be16toh(hdr->exptype);
}





size_t
cofaction_push_capwap::length() const
{
	return sizeof(struct ofp_action_exp_capwap_push_capwap);
}



void
cofaction_push_capwap::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	cofaction_experimenter_capwap::pack(buf, buflen);

	struct ofp_action_exp_capwap_push_capwap* hdr = (struct ofp_action_exp_capwap_push_capwap*)buf;

	hdr->ethertype 	= htobe16(ethertype);
}



void
cofaction_push_capwap::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_exp_capwap_push_capwap* hdr = (struct ofp_action_exp_capwap_push_capwap*)buf;

	cofaction_experimenter_capwap::unpack(buf, buflen);

	uint16_t explen = be16toh(hdr->explen);

	if (explen < length())
		throw eInval();

	ethertype	= be16toh(hdr->ethertype);
}




size_t
cofaction_pop_capwap::length() const
{
	return sizeof(struct ofp_action_exp_capwap_pop_capwap);
}



void
cofaction_pop_capwap::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	cofaction_experimenter_capwap::pack(buf, buflen);

	struct ofp_action_exp_capwap_pop_capwap* hdr = (struct ofp_action_exp_capwap_pop_capwap*)buf;

	hdr->ethertype 	= htobe16(ethertype);
}



void
cofaction_pop_capwap::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_exp_capwap_pop_capwap* hdr = (struct ofp_action_exp_capwap_pop_capwap*)buf;

	cofaction_experimenter_capwap::unpack(buf, buflen);

	uint16_t explen = be16toh(hdr->explen);

	if (explen < length())
		throw eInval();

	ethertype	= be16toh(hdr->ethertype);
}









