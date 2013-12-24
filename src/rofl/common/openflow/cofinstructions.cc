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



void
cofinstructions::clear()
{
	for (std::map<enum openflow::ofp_instruction_type, cofinst*>::iterator
			it = instmap.begin(); it != instmap.end(); ++it) {
		delete it->second;
	}
	instmap.clear();
}



cofinstructions&
cofinstructions::operator= (
		cofinstructions const& inlist)
{
	if (this == &inlist)
		return *this;

	clear();

	this->ofp_version = inlist.ofp_version;

	for (std::map<enum openflow::ofp_instruction_type, cofinst*>::const_iterator
			it = inlist.instmap.begin(); it != inlist.instmap.end(); ++it) {
		try {
			switch (it->first) {
			case openflow::OFPIT_GOTO_TABLE: 		add_inst_goto_table() 		= inlist.get_inst_goto_table(); 	break;
			case openflow::OFPIT_WRITE_METADATA: 	add_inst_write_metadata() 	= inlist.get_inst_write_metadata();	break;
			case openflow::OFPIT_WRITE_ACTIONS:		add_inst_write_actions() 	= inlist.get_inst_write_actions(); 	break;
			case openflow::OFPIT_APPLY_ACTIONS:		add_inst_apply_actions() 	= inlist.get_inst_apply_actions(); 	break;
			case openflow::OFPIT_CLEAR_ACTIONS:		add_inst_clear_actions() 	= inlist.get_inst_clear_actions();	break;
			case openflow::OFPIT_METER:				add_inst_meter() 			= inlist.get_inst_meter();			break;
			default:								instmap[it->first] = new cofinst(*(it->second));				break;
			}
		} catch (std::out_of_range& e) {
			instmap[it->first] = new cofinst(*(it->second));
		}
	}

	return *this;
}



cofinstructions::~cofinstructions()
{
	clear();
}


void
cofinstructions::unpack(
		uint8_t *buf,
		size_t buflen)
{
	clear(); // clears bcvec

	// sanity check: bclen must be of size of at least ofp_instruction
	if (buflen < (int)sizeof(struct openflow::ofp_instruction))
		return;

	// first instruction
	struct openflow::ofp_instruction *inhdr = (struct openflow::ofp_instruction*)buf;


	while (buflen > 0) {
		if (be16toh(inhdr->len) < sizeof(struct openflow::ofp_instruction))
			throw eInstructionBadLen();

		cofinst inst(ofp_version, (uint8_t*)inhdr, be16toh(inhdr->len));

		switch (be16toh(inhdr->type)) {
		case openflow::OFPIT_GOTO_TABLE:
			add_inst_goto_table() = cofinst_goto_table(inst); break;
		case openflow::OFPIT_WRITE_METADATA:
			add_inst_write_metadata() = cofinst_write_metadata(inst); break;
		case openflow::OFPIT_WRITE_ACTIONS:
			add_inst_write_actions() = cofinst_write_actions(inst); break;
		case openflow::OFPIT_APPLY_ACTIONS:
			add_inst_apply_actions() = cofinst_apply_actions(inst); break;
		case openflow::OFPIT_CLEAR_ACTIONS:
			add_inst_clear_actions() = cofinst_clear_actions(inst); break;
		case openflow::OFPIT_METER:
			add_inst_meter() = cofinst_meter(inst); break;
		default:
			instmap[be16toh(inhdr->type)] = new cofinst(ofp_version, (uint8_t*)inhdr, be16toh(inhdr->len));
		}

		buflen -= be16toh(inhdr->len);
		inhdr = (struct openflow::ofp_instruction*)(((uint8_t*)inhdr) + be16toh(inhdr->len));
	}
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

	for (std::map<enum openflow::ofp_instruction_type, cofinst*>::iterator
			it = instmap.begin(); it != instmap.end(); ++it) {
		cofinst& inst = *(it->second);
		inhdr = (struct openflow::ofp_instruction*)
				((uint8_t*)(inst.pack((uint8_t*)inhdr, inst.length())) + inst.length());
	}

	return instructions;
}



size_t
cofinstructions::length() const
{
	size_t inlen = 0;

	for (std::map<enum openflow::ofp_instruction_type, cofinst*>::const_iterator
			it = instmap.begin(); it != instmap.end(); ++it) {
		inlen += it->second->length();
	}
	return inlen;
}



cofinst_goto_table&
cofinstructions::add_inst_goto_table()
{
	if (instmap.find(openflow::OFPIT_GOTO_TABLE) != instmap.end())
		delete instmap[openflow::OFPIT_GOTO_TABLE];
	instmap[openflow::OFPIT_GOTO_TABLE] = new cofinst_goto_table(ofp_version);
	return *dynamic_cast<cofinst_goto_table*>(instmap[openflow::OFPIT_GOTO_TABLE]);
}



cofinst_goto_table&
cofinstructions::get_inst_goto_table() const
{
	// may throw std::out_of_range
	if (instmap.find(openflow::OFPIT_GOTO_TABLE) == instmap.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_goto_table*>( instmap.at(openflow::OFPIT_GOTO_TABLE) ));
#endif
	return *(dynamic_cast<cofinst_goto_table*>( instmap.at(openflow::OFPIT_GOTO_TABLE) ));
}



void
cofinstructions::drop_inst_goto_table()
{
	if (instmap.find(openflow::OFPIT_GOTO_TABLE) == instmap.end())
		return;
	delete instmap[openflow::OFPIT_GOTO_TABLE];
	instmap.erase(openflow::OFPIT_GOTO_TABLE);
}



cofinst_write_metadata&
cofinstructions::add_inst_write_metadata()
{
	if (instmap.find(openflow::OFPIT_WRITE_METADATA) != instmap.end())
		delete instmap[openflow::OFPIT_WRITE_METADATA];
	instmap[openflow::OFPIT_WRITE_METADATA] = new cofinst_write_metadata(ofp_version);
	return *dynamic_cast<cofinst_write_metadata*>(instmap[openflow::OFPIT_WRITE_METADATA]);
}



cofinst_write_metadata&
cofinstructions::get_inst_write_metadata() const
{
	// may throw std::out_of_range
	if (instmap.find(openflow::OFPIT_WRITE_METADATA) == instmap.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_write_metadata*>( instmap.at(openflow::OFPIT_WRITE_METADATA)));
#endif
	return *(dynamic_cast<cofinst_write_metadata*>( instmap.at(openflow::OFPIT_WRITE_METADATA) ));
}



void
cofinstructions::drop_inst_write_metadata()
{
	if (instmap.find(openflow::OFPIT_WRITE_METADATA) == instmap.end())
		return;
	delete instmap[openflow::OFPIT_WRITE_METADATA];
	instmap.erase(openflow::OFPIT_WRITE_METADATA);
}



cofinst_write_actions&
cofinstructions::add_inst_write_actions()
{
	if (instmap.find(openflow::OFPIT_WRITE_ACTIONS) != instmap.end())
		delete instmap[openflow::OFPIT_WRITE_ACTIONS];
	instmap[openflow::OFPIT_WRITE_ACTIONS] = new cofinst_write_actions(ofp_version);
	return *dynamic_cast<cofinst_write_actions*>(instmap[openflow::OFPIT_WRITE_ACTIONS]);
}



cofinst_write_actions&
cofinstructions::get_inst_write_actions() const
{
	// may throw std::out_of_range
	if (instmap.find(openflow::OFPIT_WRITE_ACTIONS) == instmap.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_write_actions*>( instmap.at(openflow::OFPIT_WRITE_ACTIONS)));
#endif
	return *(dynamic_cast<cofinst_write_actions*>( instmap.at(openflow::OFPIT_WRITE_ACTIONS) ));
}



void
cofinstructions::drop_inst_write_actions()
{
	if (instmap.find(openflow::OFPIT_WRITE_ACTIONS) == instmap.end())
		return;
	delete instmap[openflow::OFPIT_WRITE_ACTIONS];
	instmap.erase(openflow::OFPIT_WRITE_ACTIONS);
}



cofinst_apply_actions&
cofinstructions::add_inst_apply_actions()
{
	if (instmap.find(openflow::OFPIT_APPLY_ACTIONS) != instmap.end())
		delete instmap[openflow::OFPIT_APPLY_ACTIONS];
	instmap[openflow::OFPIT_APPLY_ACTIONS] = new cofinst_apply_actions(ofp_version);
	return *dynamic_cast<cofinst_apply_actions*>(instmap[openflow::OFPIT_APPLY_ACTIONS]);
}



cofinst_apply_actions&
cofinstructions::get_inst_apply_actions() const
{
	// may throw std::out_of_range
	if (instmap.find(openflow::OFPIT_APPLY_ACTIONS) == instmap.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_apply_actions*>( instmap.at(openflow::OFPIT_APPLY_ACTIONS)));
#endif
	return *(dynamic_cast<cofinst_apply_actions*>( instmap.at(openflow::OFPIT_APPLY_ACTIONS) ));
}



void
cofinstructions::drop_inst_apply_actions()
{
	if (instmap.find(openflow::OFPIT_APPLY_ACTIONS) == instmap.end())
		return;
	delete instmap[openflow::OFPIT_APPLY_ACTIONS];
	instmap.erase(openflow::OFPIT_APPLY_ACTIONS);
}



cofinst_clear_actions&
cofinstructions::add_inst_clear_actions()
{
	if (instmap.find(openflow::OFPIT_CLEAR_ACTIONS) != instmap.end())
		delete instmap[openflow::OFPIT_CLEAR_ACTIONS];
	instmap[openflow::OFPIT_CLEAR_ACTIONS] = new cofinst_clear_actions(ofp_version);
	return *dynamic_cast<cofinst_clear_actions*>(instmap[openflow::OFPIT_CLEAR_ACTIONS]);
}



cofinst_clear_actions&
cofinstructions::get_inst_clear_actions() const
{
	// may throw std::out_of_range
	if (instmap.find(openflow::OFPIT_CLEAR_ACTIONS) == instmap.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_clear_actions*>( instmap.at(openflow::OFPIT_CLEAR_ACTIONS)));
#endif
	return *(dynamic_cast<cofinst_clear_actions*>( instmap.at(openflow::OFPIT_CLEAR_ACTIONS) ));
}



void
cofinstructions::drop_inst_clear_actions()
{
	if (instmap.find(openflow::OFPIT_CLEAR_ACTIONS) == instmap.end())
		return;
	delete instmap[openflow::OFPIT_CLEAR_ACTIONS];
	instmap.erase(openflow::OFPIT_CLEAR_ACTIONS);
}



cofinst_meter&
cofinstructions::add_inst_meter()
{
	if (instmap.find(openflow::OFPIT_METER) != instmap.end())
		delete instmap[openflow::OFPIT_METER];
	instmap[openflow::OFPIT_METER] = new cofinst_meter(ofp_version);
	return *dynamic_cast<cofinst_meter*>(instmap[openflow::OFPIT_METER]);
}



cofinst_meter&
cofinstructions::get_inst_meter() const
{
	// may throw std::out_of_range
	if (instmap.find(openflow::OFPIT_METER) == instmap.end())
		throw eInstructionsNotFound();
#ifndef NDEBUG
	assert(dynamic_cast<cofinst_meter*>( instmap.at(openflow::OFPIT_METER)));
#endif
	return *(dynamic_cast<cofinst_meter*>( instmap.at(openflow::OFPIT_METER) ));
}



void
cofinstructions::drop_inst_meter()
{
	if (instmap.find(openflow::OFPIT_METER) == instmap.end())
		return;
	delete instmap[openflow::OFPIT_METER];
	instmap.erase(openflow::OFPIT_METER);
}



cofinst&
cofinstructions::find_inst(uint8_t type)
{
	std::map<enum openflow::ofp_instruction_type, cofinst*>::iterator it;
	if ((it = find_if(instmap.begin(), instmap.end(),
			cofinst_find_type((uint16_t)type))) == instmap.end()) {
		throw eInstructionsNotFound();
	}
	return *(it->second);
}



void
cofinstructions::test()
{
	cofinstructions inlist(openflow12::OFP_VERSION);

	inlist.add_inst_write_actions().get_actions().next() = cofaction_set_field(openflow12::OFP_VERSION, coxmatch_ofb_mpls_label(111111));

	std::cerr << "XXX => " << inlist << std::endl;

	fprintf(stderr, "--------------------------\n");

	cofinstructions inlist2;

	inlist2.add_inst_apply_actions().get_actions().next() = cofaction_output(openflow12::OFP_VERSION, 1);
	inlist2.add_inst_clear_actions();
	inlist2.add_inst_write_actions().get_actions().next() = cofaction_set_field(openflow12::OFP_VERSION, coxmatch_ofb_vlan_vid(coxmatch_ofb_vlan_vid::VLAN_TAG_MODE_NORMAL, 1111));
	inlist2.add_inst_write_actions().get_actions().next() = cofaction_set_field(openflow12::OFP_VERSION, coxmatch_ofb_mpls_tc(7));

	std::cerr << "YYY => " << inlist2 << std::endl;

	fprintf(stderr, "--------------------------\n");

	inlist2 = inlist;

	std::cerr << "ZZZ => " << inlist2 << std::endl;

	fprintf(stderr, "--------------------------\n");
}
