/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofinlist.h"

using namespace rofl;

cofinlist::cofinlist()
{
	WRITELOG(COFINST, DBG, "cofinlist(%p)::cofinlist()", this);
}


cofinlist::~cofinlist()
{
	WRITELOG(COFINST, DBG, "cofinlist(%p)::~cofinlist()", this);
}


std::vector<cofinst>&
cofinlist::unpack(
		uint8_t ofp_version,
		struct ofp_instruction *instructions,
		size_t inlen)
throw (eInstructionBadLen)
{
	clear(); // clears bcvec

	// sanity check: bclen must be of size of at least ofp_instruction
	if (inlen < (int)sizeof(struct ofp_instruction))
		return elems;

	// first instruction
	struct ofp_instruction *inhdr = instructions;


	while (inlen > 0)
	{
		if (be16toh(inhdr->len) < sizeof(struct ofp_instruction))
			throw eInstructionBadLen();

		next() = cofinst(ofp_version, inhdr, be16toh(inhdr->len) );

		inlen -= be16toh(inhdr->len);
		inhdr = (struct ofp_instruction*)(((uint8_t*)inhdr) + be16toh(inhdr->len));
	}

	return elems;
}


struct ofp_instruction*
cofinlist::pack(
		uint8_t ofp_version,
		struct ofp_instruction *instructions,
		size_t inlen) const throw (eInListInval)
{
	size_t needed_inlen = length();

	if (inlen < needed_inlen)
		throw eInListInval();

	struct ofp_instruction *inhdr = instructions; // first instruction header

	cofinlist::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		cofinst const& inst = (*it);

		inhdr = (struct ofp_instruction*)
				((uint8_t*)(inst.pack(ofp_version, inhdr, inst.length())) + inst.length());
	}

	return instructions;
}


size_t
cofinlist::length() const
{
	size_t inlen = 0;
	cofinlist::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		inlen += (*it).length();
	}
	return inlen;
}


const char*
cofinlist::c_str()
{
	cvastring vas(4096);
	info.assign(vas("cofinlist(%p) %d instruction(s): ", this, elems.size()));
	cofinlist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		info.append(vas("\n  %s ", (*it).c_str()));
	}

	return info.c_str();
}


cofinst&
cofinlist::find_inst(
		enum ofp_instruction_type type)
throw (eInListNotFound)
{
	cofinlist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		WRITELOG(COFINST, DBG, "cofinlist(%p)::find_inst() %d => %s",
				this, elems.size(), (*it).c_str());
	}

	if ((it = find_if(elems.begin(), elems.end(),
			cofinst_find_type((uint16_t)type))) == elems.end())
	{
		throw eInListNotFound();
	}
	return (*it);
}



void
cofinlist::test()
{
	cofinlist inlist;

	inlist[0] = cofinst_write_actions();
	inlist[0].actions[0] = cofaction_set_field(OFP12_VERSION, coxmatch_ofb_mpls_label(111111));

	fprintf(stderr, "XXX => %s\n", inlist.c_str());

	fprintf(stderr, "--------------------------\n");

	cofinlist inlist2;

	inlist2[0] = cofinst_apply_actions();
	inlist2[0].actions[0] = cofaction_output(OFP12_VERSION, 1);
	inlist2[1] = cofinst_clear_actions();
	inlist2[2] = cofinst_write_actions();
	inlist2[2].actions[0] = cofaction_set_field(OFP12_VERSION, coxmatch_ofb_vlan_vid(1111));
	inlist2[2].actions[1] = cofaction_set_field(OFP12_VERSION, coxmatch_ofb_mpls_tc(7));

	fprintf(stderr, "YYY => %s\n", inlist2.c_str());

	fprintf(stderr, "--------------------------\n");

	inlist2 = inlist;

	fprintf(stderr, "ZZZ => %s\n", inlist2.c_str());

	fprintf(stderr, "--------------------------\n");
}
