#include "rofl/common/openflow/experimental/actions/pppoe_actions.h"

using namespace rofl::openflow::experimental::pppoe;

size_t
cofaction_push_pppoe::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION:
		return sizeof(struct rofl::openflow13::ofp_action_push);
	default:
		throw eBadVersion("cofaction_push_pppoe::length() invalid version");
	}
}



void
cofaction_push_pppoe::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_push_pppoe::length())
		throw eInval("cofaction_push_pppoe::pack() buflen too short");

	cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct rofl::openflow13::ofp_action_push* hdr = (struct rofl::openflow13::ofp_action_push*)buf;

		hdr->ethertype = htobe16(eth_type);

	} break;
	default:
		throw eBadVersion("cofaction_push_pppoe::pack() invalid version");
	}
}



void
cofaction_push_pppoe::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_push_pppoe::length())
		throw eInval("cofaction_push_pppoe::unpack() buflen too short");

	cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct rofl::openflow13::ofp_action_push* hdr = (struct rofl::openflow13::ofp_action_push*)buf;

		eth_type = be16toh(hdr->ethertype);

	} break;
	default:
		throw eBadVersion("cofaction_push_pppoe::unpack() invalid version");
	}
}



size_t
cofaction_pop_pppoe::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION:
		return sizeof(struct rofl::openflow13::ofp_action_header);
	default:
		throw eBadVersion("cofaction_pop_pppoe::length() invalid version");
	}
}



void
cofaction_pop_pppoe::pack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_pop_pppoe::length())
		throw eInval("cofaction_pop_pppoe::pack() buflen too short");

	cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

	} break;
	default:
		throw eBadVersion("cofaction_pop_pppoe::pack() invalid version");
	}
}



void
cofaction_pop_pppoe::unpack(
		uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofaction_pop_pppoe::length())
		throw eInval("cofaction_pop_pppoe::unpack() buflen too short");

	cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

	} break;
	default:
		throw eBadVersion("cofaction_pop_pppoe::unpack() invalid version");
	}
}


