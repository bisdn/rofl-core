/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmlist.cc
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */


#include "rofl/common/openflow/coxmlist.h"
#include <stdexcept>

using namespace rofl::openflow;

coxmlist::coxmlist()
{

}


coxmlist::~coxmlist()
{
	clear();
}


coxmlist::coxmlist(
		coxmlist const& oxmlist)
{
	*this = oxmlist;
}


coxmlist&
coxmlist::operator= (
		coxmlist const& oxl)
{
	if (this == &oxl)
		return *this;

	clear();

	for (std::map<uint32_t, coxmatch>::const_iterator
			jt = matches.begin(); jt != matches.end(); ++jt) {
		add_match(jt->second);
	}

	return *this;
}




void
coxmlist::clear()
{
	matches.clear();
}



bool
coxmlist::operator== (coxmlist const& oxmlist)
{
	return this->contains(oxmlist, true/*strict*/);
}





void
coxmlist::unpack(
		uint8_t* buf,
		size_t buflen)
{
	clear();

	// sanity check: oxm_len must be of size at least of ofp_oxm_hdr
	if (buflen < (int)sizeof(struct rofl::openflow::ofp_oxm_hdr)) {
		throw eBadMatchBadLen();
	}

	// first instruction
	struct openflow::ofp_oxm_hdr *hdr = (struct openflow::ofp_oxm_hdr*)buf;


	while (buflen > 0) {
		if ((buflen < sizeof(struct openflow::ofp_oxm_hdr)) || (0 == hdr->oxm_length)) {
			return; // not enough bytes to parse an entire ofp_oxm_hdr, possibly padding bytes found
		}

		if (hdr->oxm_length > (sizeof(struct openflow::ofp_oxm_hdr) + buflen))
			throw eBadMatchBadLen();

		coxmatch oxm((uint8_t*)hdr, sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length);

		add_match(oxm);

		buflen -= (sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length);
		hdr = (struct openflow::ofp_oxm_hdr*)(((uint8_t*)hdr) + sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length);
	}
}



void
coxmlist::pack(
		uint8_t* buf, size_t buflen)
{
	if (buflen < length()) {
		throw eBadMatchBadLen();
	}
	for (std::map<uint32_t, coxmatch>::iterator
			jt = matches.begin(); jt != matches.end(); ++jt) {

		coxmatch& match = (matches[jt->first]);

		match.pack(buf, match.length());

		buf += match.length();
	}
}


coxmatch&
coxmlist::add_match(coxmatch const& oxm)
{
	if (matches.find(oxm.get_oxm_id()) != matches.end()) {
		matches.erase(oxm.get_oxm_id());
	}
	return (matches[oxm.get_oxm_id()] = oxm);
}


coxmatch&
coxmlist::add_match(uint32_t oxm_id)
{
	if (matches.find(oxm_id) != matches.end()) {
		matches.erase(oxm_id);
	}
	return (matches[oxm_id]);
}


coxmatch&
coxmlist::set_match(uint32_t oxm_id)
{
	return matches[oxm_id];
}


coxmatch const&
coxmlist::get_match(uint32_t oxm_id) const
{
	if (matches.find(oxm_id) == matches.end()) {
		throw eOxmInval();
	}
	return matches.at(oxm_id);
}


void
coxmlist::drop_match(uint32_t oxm_id)
{
	if (matches.find(oxm_id) == matches.end()) {
		return;
	}
	matches.erase(oxm_id);
}


bool
coxmlist::has_match(uint32_t oxm_id) const
{
	return (not (matches.find(oxm_id) == matches.end()));
}



size_t
coxmlist::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, coxmatch>::const_iterator
			it = matches.begin(); it != matches.end(); ++it) {
		len += it->second.length();
	}
	return len;
}




bool
coxmlist::contains(
	coxmlist const& oxl,
	bool strict /* = false (default) */)
{
	/*
	 * strict:
	 * all elemens in *this and oxl must be identical (number of and value of elements)
	 *
	 * non-strict:
	 * all elements in oxl must be present in *this (value of elements), but *this may contain additional OXM TLVs
	 */

	std::map<uint32_t, coxmatch>&       lmap = matches;
	std::map<uint32_t, coxmatch> const& rmap = oxl.matches;

	if (strict) {
		// strict: # of elems for an ofm_class must be the same in oxl for the specific ofm_class
		if (lmap.size() != rmap.size()) {
			return false;
		}
	} else  /* non-strict*/ {
		// non-strict: # of elems for a class must be larger (or equal) the number of elements in oxl
		if (lmap.size() > rmap.size()) {
			return false;
		}
	}

	// strict: check all TLVs for specific class in oxl.matches => must exist and have same value
	for (std::map<uint32_t, coxmatch>::iterator
			jt = lmap.begin(); jt != lmap.end(); ++jt) {

		coxmatch& lmatch = (jt->second);

		// strict: all OXM TLVs must also exist in oxl
		if (rmap.find(lmatch.get_oxm_field()) == rmap.end()) {
			return false;
		}

		coxmatch const& rmatch = (rmap.find(jt->first)->second);

		// strict: both OXM TLVs must have identical values
		if (lmatch != rmatch) {
			return false;
		}
	}
	return true;
}




bool
coxmlist::is_part_of(
		coxmlist const& oxl,
		uint16_t& exact_hits,
		uint16_t& wildcard_hits,
		uint16_t& missed)
{
	bool result = true;

	std::map<uint32_t, coxmatch>&       lmap = matches;
	std::map<uint32_t, coxmatch> const& rmap = oxl.matches;

	for (std::map<uint32_t, coxmatch>::const_iterator
			jt = rmap.begin(); jt != rmap.end(); ++jt) {

		coxmatch const& rmatch = (jt->second);

		if (lmap.find(rmatch.get_oxm_id()) == lmap.end()) {
			wildcard_hits++; continue;
		}

		if (lmap[jt->first] != rmatch) {
			missed++; result = false;
		}

		exact_hits++;
	}

	return result;
}


