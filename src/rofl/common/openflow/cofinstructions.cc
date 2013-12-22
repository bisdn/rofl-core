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
{
	clear(); // clears bcvec

	// sanity check: bclen must be of size of at least ofp_instruction
	if (inlen < (int)sizeof(struct openflow::ofp_instruction))
		return elems;

	// first instruction
	struct openflow::ofp_instruction *inhdr = (struct openflow::ofp_instruction*)instructions;


	while (inlen > 0) {
		if (be16toh(inhdr->len) < sizeof(struct openflow::ofp_instruction))
			throw eInstructionBadLen();

		next() = cofinst(ofp_version, (uint8_t*)inhdr, be16toh(inhdr->len) );

		inlen -= be16toh(inhdr->len);
		inhdr = (struct openflow::ofp_instruction*)(((uint8_t*)inhdr) + be16toh(inhdr->len));
	}

	return elems;
}


uint8_t*
cofinstructions::pack(
		uint8_t *instructions,
		size_t inlen)
{
	size_t needed_inlen = length();

	if (inlen < needed_inlen)
		throw eInstructionsInval();

	struct openflow::ofp_instruction *inhdr = (struct openflow::ofp_instruction*)instructions; // first instruction header

	cofinstructions::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it) {
		cofinst& inst = (*it);

		inhdr = (struct openflow::ofp_instruction*)
				((uint8_t*)(inst.pack((uint8_t*)inhdr, inst.length())) + inst.length());
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



cofinst_goto_table&
cofinstructions::get_inst_goto_table()
{
	if (instructions.find(openflow::OFPIT_GOTO_TABLE) == instructions.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_goto_table*>( instructions[openflow::OFPIT_GOTO_TABLE]));
#endif
	return *(dynamic_cast<cofinst_goto_table*>( instructions[openflow::OFPIT_GOTO_TABLE] ));
}



cofinst_write_metadata&
cofinstructions::get_inst_write_metadata()
{
	if (instructions.find(openflow::OFPIT_WRITE_METADATA) == instructions.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_write_metadata*>( instructions[openflow::OFPIT_WRITE_METADATA]));
#endif
	return *(dynamic_cast<cofinst_write_metadata*>( instructions[openflow::OFPIT_WRITE_METADATA] ));
}



cofinst_write_actions&
cofinstructions::get_inst_write_actions()
{
	if (instructions.find(openflow::OFPIT_WRITE_ACTIONS) == instructions.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_write_actions*>( instructions[openflow::OFPIT_WRITE_ACTIONS]));
#endif
	return *(dynamic_cast<cofinst_write_actions*>( instructions[openflow::OFPIT_WRITE_ACTIONS] ));
}



cofinst_apply_actions&
cofinstructions::get_inst_apply_actions()
{
	if (instructions.find(openflow::OFPIT_APPLY_ACTIONS) == instructions.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_apply_actions*>( instructions[openflow::OFPIT_APPLY_ACTIONS]));
#endif
	return *(dynamic_cast<cofinst_apply_actions*>( instructions[openflow::OFPIT_APPLY_ACTIONS] ));
}



cofinst_clear_actions&
cofinstructions::get_inst_clear_actions()
{
	if (instructions.find(openflow::OFPIT_CLEAR_ACTIONS) == instructions.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_clear_actions*>( instructions[openflow::OFPIT_CLEAR_ACTIONS]));
#endif
	return *(dynamic_cast<cofinst_clear_actions*>( instructions[openflow::OFPIT_CLEAR_ACTIONS] ));
}



cofinst_meter&
cofinstructions::get_inst_meter()
{
	if (instructions.find(openflow::OFPIT_METER) == instructions.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_meter*>( instructions[openflow::OFPIT_METER]));
#endif
	return *(dynamic_cast<cofinst_meter*>( instructions[openflow::OFPIT_METER] ));
}



cofinst&
cofinstructions::find_inst(
		uint8_t type)
throw (eInstructionsNotFound)
{
	cofinstructions::iterator it;
	if ((it = find_if(elems.begin(), elems.end(),
			cofinst_find_type((uint16_t)type))) == elems.end()) {
		throw eInstructionsNotFound();
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
