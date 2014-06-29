/*
 * pppoe_actions.cc
 *
 *  Created on: 02.08.2013
 *      Author: andreas
 */

#include "rofl/common/openflow/experimental/actions/pppoe_actions.h"

using namespace rofl::openflow::experimental::pppoe;


size_t
cofaction_experimenter_pppoe::length() const
{
	return sizeof(struct ofp_action_exp_pppoe_hdr);
}



void
cofaction_experimenter_pppoe::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_experimenter_pppoe::length())
		throw eInval();

	struct ofp_action_exp_pppoe_hdr* hdr = (struct ofp_action_exp_pppoe_hdr*)buf;

	hdr->exptype = htobe16(exptype);
	hdr->explen  = htobe16(length());
}



void
cofaction_experimenter_pppoe::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_experimenter_pppoe::length())
		throw eInval();

	struct ofp_action_exp_pppoe_hdr* hdr = (struct ofp_action_exp_pppoe_hdr*)buf;

	uint16_t explen = be16toh(hdr->explen);

	if (explen < cofaction_experimenter_pppoe::length())
		throw eInval();

	exptype	= be16toh(hdr->exptype);
}





size_t
cofaction_push_pppoe::length() const
{
	return sizeof(struct ofp_action_exp_pppoe_push_pppoe);
}



void
cofaction_push_pppoe::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	cofaction_experimenter_pppoe::pack(buf, buflen);

	struct ofp_action_exp_pppoe_push_pppoe* hdr = (struct ofp_action_exp_pppoe_push_pppoe*)buf;

	hdr->ethertype 	= htobe16(ethertype);
}



void
cofaction_push_pppoe::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_exp_pppoe_push_pppoe* hdr = (struct ofp_action_exp_pppoe_push_pppoe*)buf;

	cofaction_experimenter_pppoe::unpack(buf, buflen);

	uint16_t explen = be16toh(hdr->explen);

	if (explen < length())
		throw eInval();

	ethertype	= be16toh(hdr->ethertype);
}




size_t
cofaction_pop_pppoe::length() const
{
	return sizeof(struct ofp_action_exp_pppoe_pop_pppoe);
}



void
cofaction_pop_pppoe::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	cofaction_experimenter_pppoe::pack(buf, buflen);

	struct ofp_action_exp_pppoe_pop_pppoe* hdr = (struct ofp_action_exp_pppoe_pop_pppoe*)buf;

	hdr->ethertype 	= htobe16(ethertype);
}



void
cofaction_pop_pppoe::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_exp_pppoe_pop_pppoe* hdr = (struct ofp_action_exp_pppoe_pop_pppoe*)buf;

	cofaction_experimenter_pppoe::unpack(buf, buflen);

	uint16_t explen = be16toh(hdr->explen);

	if (explen < length())
		throw eInval();

	ethertype	= be16toh(hdr->ethertype);
}


