/*
 * gtp_actions.cc
 *
 *  Created on: 02.08.2013
 *      Author: andreas
 */

#include "rofl/common/openflow/experimental/actions/gtp_actions.h"

using namespace rofl::openflow::experimental::gtp;

size_t
cofaction_push_gtp::length() const
{
	return sizeof(struct ofp_action_push_gtp_body);
}



void
cofaction_push_gtp::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_push_gtp_body* hdr = (struct ofp_action_push_gtp_body*)buf;

	hdr->exptype 	= htobe16(exptype);
	hdr->explen 	= htobe16(length());
	hdr->ethertype 	= htobe16(ethertype);
}



void
cofaction_push_gtp::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_push_gtp_body* hdr = (struct ofp_action_push_gtp_body*)buf;

	uint16_t explen = be16toh(hdr->explen);

	if (explen < length())
		throw eInval();

	exptype		= be16toh(hdr->exptype);
	ethertype	= be16toh(hdr->ethertype);
}



size_t
cofaction_pop_gtp::length() const
{
	return sizeof(struct ofp_action_pop_gtp_body);
}



void
cofaction_pop_gtp::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_pop_gtp_body* hdr = (struct ofp_action_pop_gtp_body*)buf;

	hdr->exptype 	= htobe16(exptype);
	hdr->explen 	= htobe16(length());
	hdr->ethertype 	= htobe16(ethertype);
}



void
cofaction_pop_gtp::unpack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	struct ofp_action_pop_gtp_body* hdr = (struct ofp_action_pop_gtp_body*)buf;

	uint16_t explen = be16toh(hdr->explen);

	if (explen < length())
		throw eInval();

	exptype		= be16toh(hdr->exptype);
	ethertype	= be16toh(hdr->ethertype);
}
