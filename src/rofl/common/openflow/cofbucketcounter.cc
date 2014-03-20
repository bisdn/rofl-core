/*
 * cofbucketcounter.cc
 *
 *  Created on: 16.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofbucketcounter.h"

using namespace rofl::openflow;

cofbucket_counter::cofbucket_counter(
			uint8_t ofp_version) :
					ofp_version(ofp_version),
					packet_count(0),
					byte_count(0)
{}


cofbucket_counter::~cofbucket_counter()
{}


cofbucket_counter::cofbucket_counter(
			cofbucket_counter const& bc)
{
	*this = bc;
}


cofbucket_counter&
cofbucket_counter::operator= (
			cofbucket_counter const& bc)
{
	if (this == &bc)
		return *this;

	ofp_version		= bc.ofp_version;
	packet_count	= bc.packet_count;
	byte_count		= bc.byte_count;

	return *this;
}


bool
cofbucket_counter::operator== (
			cofbucket_counter const& bc)
{
	return ((ofp_version 	== bc.ofp_version) &&
			(packet_count 	== bc.packet_count) &&
			(byte_count 	== bc.byte_count));
}


size_t
cofbucket_counter::length() const
{
	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION: {
		return sizeof(struct rofl::openflow12::ofp_bucket_counter);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return sizeof(struct rofl::openflow13::ofp_bucket_counter);
	} break;
	default: {
		throw eBadVersion();
	};
	}
}


void
cofbucket_counter::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION: {
		struct rofl::openflow12::ofp_bucket_counter* bc = (struct rofl::openflow12::ofp_bucket_counter*)buf;
		bc->packet_count 	= htobe64(packet_count);
		bc->byte_count 		= htobe64(byte_count);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		struct rofl::openflow13::ofp_bucket_counter* bc = (struct rofl::openflow13::ofp_bucket_counter*)buf;
		bc->packet_count 	= htobe64(packet_count);
		bc->byte_count 		= htobe64(byte_count);
	} break;
	default: {
		throw eBadVersion();
	};
	}
}


void
cofbucket_counter::unpack(uint8_t* buf, size_t buflen)
{
	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION: {
		struct rofl::openflow12::ofp_bucket_counter* bc = (struct rofl::openflow12::ofp_bucket_counter*)buf;
		packet_count		= be64toh(bc->packet_count);
		byte_count			= be64toh(bc->byte_count);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		struct rofl::openflow13::ofp_bucket_counter* bc = (struct rofl::openflow13::ofp_bucket_counter*)buf;
		packet_count		= be64toh(bc->packet_count);
		byte_count			= be64toh(bc->byte_count);
	} break;
	default: {
		throw eBadVersion();
	};
	}
}


