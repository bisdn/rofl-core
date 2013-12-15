/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofinstructions.h"

using namespace rofl;
using namespace rofl::openflow;

cofinstructions::cofinstructions(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{
}



cofinstructions::cofinstructions(cofinstructions const& inlist)
{
	*this = inlist;
}



cofinstructions&
cofinstructions::operator= (
		cofinstructions const& inlist)
{
	if (this == &inlist)
		return *this;

	this->ofp_version = inlist.ofp_version;
	coflist<cofinst>::operator= (inlist);

	return *this;
}



cofinstructions::~cofinstructions()
{
}


std::vector<cofinst>&
cofinstructions::unpack(
		uint8_t *instructions,
		size_t inlen)
throw (eInstructionBadLen)
{
	clear(); // clears bcvec

	// sanity check: bclen must be of size of at least ofp_instruction
	if (inlen < (int)sizeof(struct openflow::ofp_instruction))
		return elems;

	// first instruction
	struct openflow::ofp_instruction *inhdr = (struct openflow::ofp_instruction*)instructions;


	while (inlen > 0)
	{
		if (be16toh(inhdr->len) < sizeof(struct openflow::ofp_instruction))
			throw eInstructionBadLen();

		next() = cofinst(ofp_version, inhdr, be16toh(inhdr->len) );

		inlen -= be16toh(inhdr->len);
		inhdr = (struct openflow::ofp_instruction*)(((uint8_t*)inhdr) + be16toh(inhdr->len));
	}

	return elems;
}


uint8_t*
cofinstructions::pack(
		uint8_t *instructions,
		size_t inlen) const throw (eInListInval)
{
	size_t needed_inlen = length();

	if (inlen < needed_inlen)
		throw eInListInval();

	struct openflow::ofp_instruction *inhdr = (struct openflow::ofp_instruction*)instructions; // first instruction header

	cofinstructions::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it) {
		cofinst const& inst = (*it);

		inhdr = (struct openflow::ofp_instruction*)
				((uint8_t*)(inst.pack(inhdr, inst.length())) + inst.length());
	}

	return instructions;
}


size_t
cofinstructions::length() const
{
	size_t inlen = 0;
	cofinstructions::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it) {
		inlen += (*it).length();
	}
	return inlen;
}




cofinst&
cofinstructions::find_inst(
		uint8_t type)
throw (eInListNotFound)
{
	cofinstructions::iterator it;
	if ((it = find_if(elems.begin(), elems.end(),
			cofinst_find_type((uint16_t)type))) == elems.end()) {
		throw eInListNotFound();
	}
	return (*it);
}



void
cofinstructions::test()
{
	cofinstructions inlist;

	inlist[0] = cofinst_write_actions(openflow12::OFP_VERSION);
	inlist[0].actions[0] = cofaction_set_field(openflow12::OFP_VERSION, coxmatch_ofb_mpls_label(111111));

	std::cerr << "XXX => " << inlist << std::endl;

	fprintf(stderr, "--------------------------\n");

	cofinstructions inlist2;

	inlist2[0] = cofinst_apply_actions(openflow12::OFP_VERSION);
	inlist2[0].actions[0] = cofaction_output(openflow12::OFP_VERSION, 1);
	inlist2[1] = cofinst_clear_actions(openflow12::OFP_VERSION);
	inlist2[2] = cofinst_write_actions(openflow12::OFP_VERSION);
	inlist2[2].actions[0] = cofaction_set_field(openflow12::OFP_VERSION, coxmatch_ofb_vlan_vid(coxmatch_ofb_vlan_vid::VLAN_TAG_MODE_NORMAL, 1111));
	inlist2[2].actions[1] = cofaction_set_field(openflow12::OFP_VERSION, coxmatch_ofb_mpls_tc(7));

	std::cerr << "YYY => " << inlist2 << std::endl;

	fprintf(stderr, "--------------------------\n");

	inlist2 = inlist;

	std::cerr << "ZZZ => " << inlist2 << std::endl;

	fprintf(stderr, "--------------------------\n");
}
