/*
 * ofqueueprop.cc
 *
 *  Created on: 29.04.2013
 *      Author: andi
 */

#include "rofl/common/openflow/cofqueueprop.h"

using namespace rofl::openflow;

size_t
cofqueue_prop::length() const
{
	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_queue_prop_header));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_prop::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofqueue_prop::length()) // take length() method from this base class
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct rofl::openflow13::ofp_queue_prop_header* hdr =
				(struct rofl::openflow13::ofp_queue_prop_header*)buf;

		len = length(); // take virtual length() method

		hdr->property	= htobe16(property);
		hdr->len		= htobe16(len);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_prop::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofqueue_prop::length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct rofl::openflow13::ofp_queue_prop_header* hdr =
				(struct rofl::openflow13::ofp_queue_prop_header*)buf;

		property	= be16toh(hdr->property);
		len			= be16toh(hdr->len);

		if (len < sizeof(struct rofl::openflow13::ofp_queue_prop_header))
			throw eInval();

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofqueue_prop_min_rate::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_queue_prop_min_rate));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_prop_min_rate::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofqueue_prop_min_rate::length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		cofqueue_prop::pack(buf, buflen);

		struct rofl::openflow13::ofp_queue_prop_min_rate* hdr =
				(struct rofl::openflow13::ofp_queue_prop_min_rate*)buf;

		hdr->rate	= htobe16(min_rate);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_prop_min_rate::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < sizeof(struct rofl::openflow13::ofp_queue_prop_min_rate))
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		cofqueue_prop::unpack(buf, buflen);

		if (get_length() < sizeof(struct rofl::openflow13::ofp_queue_prop_min_rate))
			throw eInval();

		struct rofl::openflow13::ofp_queue_prop_min_rate* hdr =
				(struct rofl::openflow13::ofp_queue_prop_min_rate*)buf;

		min_rate	= be16toh(hdr->rate);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofqueue_prop_max_rate::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_queue_prop_max_rate));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_prop_max_rate::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofqueue_prop_max_rate::length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		cofqueue_prop::pack(buf, buflen);

		struct rofl::openflow13::ofp_queue_prop_max_rate* hdr =
				(struct rofl::openflow13::ofp_queue_prop_max_rate*)buf;

		hdr->rate	= htobe16(max_rate);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_prop_max_rate::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < sizeof(struct rofl::openflow13::ofp_queue_prop_max_rate))
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		cofqueue_prop::unpack(buf, buflen);

		if (get_length() < sizeof(struct rofl::openflow13::ofp_queue_prop_max_rate))
			throw eInval();

		struct rofl::openflow13::ofp_queue_prop_max_rate* hdr =
				(struct rofl::openflow13::ofp_queue_prop_max_rate*)buf;

		max_rate	= be16toh(hdr->rate);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofqueue_prop_experimenter::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_queue_prop_experimenter) + exp_body.memlen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_prop_experimenter::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofqueue_prop_experimenter::length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		cofqueue_prop::pack(buf, buflen);

		struct rofl::openflow13::ofp_queue_prop_experimenter* hdr =
				(struct rofl::openflow13::ofp_queue_prop_experimenter*)buf;

		hdr->experimenter	= htobe32(exp_id);
		exp_body.pack(hdr->data, exp_body.memlen());

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_prop_experimenter::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < sizeof(struct rofl::openflow13::ofp_queue_prop_experimenter))
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		cofqueue_prop::unpack(buf, buflen);

		if (buflen < get_length())
			throw eInval();

		struct rofl::openflow13::ofp_queue_prop_experimenter* hdr =
				(struct rofl::openflow13::ofp_queue_prop_experimenter*)buf;

		exp_id	= be32toh(hdr->experimenter);
		exp_body.unpack(hdr->data, get_length() - sizeof(struct rofl::openflow13::ofp_queue_prop_experimenter));

	} break;
	default:
		throw eBadVersion();
	}
}


