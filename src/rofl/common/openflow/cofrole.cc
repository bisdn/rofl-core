/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofrole.cc
 *
 *  Created on: 18.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofrole.h"

using namespace rofl::openflow;


/*static*/int64_t
cofrole::distance(int64_t rcvd_gen_id, int64_t cached_gen_id)
{
	return (rcvd_gen_id - cached_gen_id);
}



cofrole::cofrole(
		uint8_t ofp_version) :
				ofp_version(ofp_version),
				role(rofl::openflow13::OFPCR_ROLE_EQUAL),
				generation_id((int64_t)-1)
{}



cofrole::~cofrole()
{}



cofrole::cofrole(
		cofrole const& role)
{
	*this = role;
}



cofrole&
cofrole::operator= (
		cofrole const& r)
{
	if (this == &r)
		return *this;

	ofp_version		= r.ofp_version;
	role			= r.role;
	generation_id 	= r.generation_id;

	return *this;
}



void
cofrole::clear()
{
	role = 0;
	generation_id = 0;
}



size_t
cofrole::length() const
{
	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return sizeof(struct role_t);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofrole::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct role_t* r = (struct role_t*)buf;

		r->role 			= htobe32(role);
		r->generation_id 	= htobe64(generation_id);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofrole::unpack(uint8_t *buf, size_t buflen)
{
	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct role_t* r = (struct role_t*)buf;

		role 			= be32toh(r->role);
		generation_id 	= be64toh(r->generation_id);

	} break;
	default:
		throw eBadVersion();
	}
}




