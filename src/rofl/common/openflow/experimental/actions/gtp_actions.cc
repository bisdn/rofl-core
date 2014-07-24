/*
 * gtp_actions.cc
 *
 *  Created on: 02.08.2013
 *      Author: andreas
 */

#include "rofl/common/openflow/experimental/actions/gtp_actions.h"

using namespace rofl::openflow::experimental::gtp;




size_t
cofaction_experimenter_gtp::length() const
{
	return sizeof(struct ofp_action_exp_gtp_hdr);
}



void
cofaction_experimenter_gtp::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_experimenter_gtp::length())
		throw eInval();

	struct ofp_action_exp_gtp_hdr* hdr = (struct ofp_action_exp_gtp_hdr*)buf;

	hdr->exptype = htobe16(exptype);
	hdr->explen  = htobe16(length());
}



void
cofaction_experimenter_gtp::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_experimenter_gtp::length())
		throw eInval();

	struct ofp_action_exp_gtp_hdr* hdr = (struct ofp_action_exp_gtp_hdr*)buf;

	uint16_t explen = be16toh(hdr->explen);

	if (explen < cofaction_experimenter_gtp::length())
		throw eInval();

	exptype	= be16toh(hdr->exptype);
}





size_t
cofaction_push_gtp::length() const
{
	return sizeof(struct ofp_action_exp_gtp_push_gtp);
}



void
cofaction_push_gtp::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	cofaction_experimenter_gtp::pack(buf, buflen);

	struct ofp_action_exp_gtp_push_gtp* hdr = (struct ofp_action_exp_gtp_push_gtp*)buf;

	hdr->ethertype 	= htobe16(ethertype);
}



void
cofaction_push_gtp::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_exp_gtp_push_gtp* hdr = (struct ofp_action_exp_gtp_push_gtp*)buf;

	cofaction_experimenter_gtp::unpack(buf, buflen);

	uint16_t explen = be16toh(hdr->explen);

	if (explen < length())
		throw eInval();

	ethertype	= be16toh(hdr->ethertype);
}




size_t
cofaction_pop_gtp::length() const
{
	return sizeof(struct ofp_action_exp_gtp_pop_gtp);
}



void
cofaction_pop_gtp::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	cofaction_experimenter_gtp::pack(buf, buflen);

	struct ofp_action_exp_gtp_pop_gtp* hdr = (struct ofp_action_exp_gtp_pop_gtp*)buf;

	hdr->ethertype 	= htobe16(ethertype);
}



void
cofaction_pop_gtp::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_exp_gtp_pop_gtp* hdr = (struct ofp_action_exp_gtp_pop_gtp*)buf;

	cofaction_experimenter_gtp::unpack(buf, buflen);

	uint16_t explen = be16toh(hdr->explen);

	if (explen < length())
		throw eInval();

	ethertype	= be16toh(hdr->ethertype);
}


