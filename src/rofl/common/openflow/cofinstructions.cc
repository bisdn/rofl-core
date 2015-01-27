/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofinstructions.h"
#include <stdexcept>

using namespace rofl::openflow;

cofinstructions::cofinstructions(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{

}



cofinstructions::cofinstructions(
		cofinstructions const& instructions)
{
	*this = instructions;
}



void
cofinstructions::clear()
{
	for (std::map<uint16_t, cofinstruction*>::iterator
			it = instmap.begin(); it != instmap.end(); ++it) {
		delete it->second;
	}
	instmap.clear();
}



cofinstructions&
cofinstructions::operator= (
		cofinstructions const& instructions)
{
	if (this == &instructions)
		return *this;

	clear();

	this->ofp_version = instructions.ofp_version;

	for (std::map<uint16_t, cofinstruction*>::const_iterator
			it = instructions.instmap.begin(); it != instructions.instmap.end(); ++it) {
		try {
			switch (it->first) {
			case rofl::openflow::OFPIT_GOTO_TABLE: {
				add_inst_goto_table() = instructions.get_inst_goto_table();
			} break;
			case rofl::openflow::OFPIT_WRITE_METADATA: {
				add_inst_write_metadata() = instructions.get_inst_write_metadata();
			} break;
			case rofl::openflow::OFPIT_WRITE_ACTIONS: {
				add_inst_write_actions() = instructions.get_inst_write_actions();
			} break;
			case rofl::openflow::OFPIT_APPLY_ACTIONS: {
				add_inst_apply_actions() = instructions.get_inst_apply_actions();
			} break;
			case rofl::openflow::OFPIT_CLEAR_ACTIONS: {
				add_inst_clear_actions() = instructions.get_inst_clear_actions();
			} break;
			case rofl::openflow::OFPIT_METER: {
				add_inst_meter() = instructions.get_inst_meter();
			} break;
			case rofl::openflow::OFPIT_EXPERIMENTER: {
				add_inst_experimenter() = instructions.get_inst_experimenter();
			} break;
			default: {
				instmap[it->first] = new cofinstruction(*(it->second));
			} break;
			}
		} catch (std::out_of_range& e) {
			instmap[it->first] = new cofinstruction(*(it->second));
		}
	}

	return *this;
}



bool
cofinstructions::operator== (
		cofinstructions const& instructions)
{
	if (instmap.size() != instructions.instmap.size()) {
		return false;
	}

	for (std::map<uint16_t, cofinstruction*>::const_iterator
			it = instructions.instmap.begin(); it != instructions.instmap.end(); ++it) {
		if (not (*(instmap[it->first]) == *(it->second))) {
			return false;
		}
	}
	return true;
}



cofinstructions::~cofinstructions()
{
	clear();
}



cofinstruction&
cofinstructions::add_inst(uint16_t type)
{
	switch (type) {
	case rofl::openflow::OFPIT_GOTO_TABLE: {
		add_inst_goto_table();
	} break;
	case rofl::openflow::OFPIT_WRITE_METADATA: {
		add_inst_write_metadata();
	} break;
	case rofl::openflow::OFPIT_WRITE_ACTIONS: {
		add_inst_write_actions();
	} break;
	case rofl::openflow::OFPIT_APPLY_ACTIONS: {
		add_inst_apply_actions();
	} break;
	case rofl::openflow::OFPIT_CLEAR_ACTIONS: {
		add_inst_clear_actions();
	} break;
	case rofl::openflow::OFPIT_METER: {
		add_inst_meter();
	} break;
	case rofl::openflow::OFPIT_EXPERIMENTER: {
		add_inst_experimenter();
	} break;
	default: {
		if (instmap.find(type) != instmap.end()) {
			delete instmap[type];
		}
		instmap[type] = new cofinstruction(ofp_version, type);
	}
	}
	return *dynamic_cast<cofinstruction*>(instmap[type]);
}



cofinstruction&
cofinstructions::set_inst(uint16_t type)
{
	switch (type) {
	case rofl::openflow::OFPIT_GOTO_TABLE: {
		set_inst_goto_table();
	} break;
	case rofl::openflow::OFPIT_WRITE_METADATA: {
		set_inst_write_metadata();
	} break;
	case rofl::openflow::OFPIT_WRITE_ACTIONS: {
		set_inst_write_actions();
	} break;
	case rofl::openflow::OFPIT_APPLY_ACTIONS: {
		set_inst_apply_actions();
	} break;
	case rofl::openflow::OFPIT_CLEAR_ACTIONS: {
		set_inst_clear_actions();
	} break;
	case rofl::openflow::OFPIT_METER: {
		set_inst_meter();
	} break;
	case rofl::openflow::OFPIT_EXPERIMENTER: {
		set_inst_experimenter();
	} break;
	default: {
		if (instmap.find(type) == instmap.end()) {
			instmap[type] = new cofinstruction(ofp_version, type);
		}
	}
	}
	return *dynamic_cast<cofinstruction*>(instmap[type]);
}



const cofinstruction&
cofinstructions::get_inst(uint16_t type) const
{
	if (instmap.find(type) == instmap.end()) {
		throw eInstructionNotFound();
	}
	return *(dynamic_cast<const cofinstruction*>( instmap.at(type) ));
}



void
cofinstructions::drop_inst(uint16_t type)
{
	if (instmap.find(type) == instmap.end()) {
		return;
	}
	delete instmap[type];
	instmap.erase(type);
}



bool
cofinstructions::has_inst(uint16_t type) const
{
	return (not (instmap.find(type) == instmap.end()));
}



cofinstruction_goto_table&
cofinstructions::add_inst_goto_table()
{
	if (instmap.find(rofl::openflow::OFPIT_GOTO_TABLE) != instmap.end()) {
		delete instmap[rofl::openflow::OFPIT_GOTO_TABLE];
	}
	instmap[rofl::openflow::OFPIT_GOTO_TABLE] = new cofinstruction_goto_table(ofp_version);
	return *dynamic_cast<cofinstruction_goto_table*>(instmap[rofl::openflow::OFPIT_GOTO_TABLE]);
}



cofinstruction_goto_table&
cofinstructions::set_inst_goto_table()
{
	if (instmap.find(rofl::openflow::OFPIT_GOTO_TABLE) == instmap.end()) {
		instmap[rofl::openflow::OFPIT_GOTO_TABLE] = new cofinstruction_goto_table(ofp_version);
	}
	return *dynamic_cast<cofinstruction_goto_table*>(instmap[rofl::openflow::OFPIT_GOTO_TABLE]);
}



const cofinstruction_goto_table&
cofinstructions::get_inst_goto_table() const
{
	if (instmap.find(rofl::openflow::OFPIT_GOTO_TABLE) == instmap.end()) {
		throw eInstructionNotFound();
	}
	return *(dynamic_cast<const cofinstruction_goto_table*>( instmap.at(rofl::openflow::OFPIT_GOTO_TABLE) ));
}



void
cofinstructions::drop_inst_goto_table()
{
	if (instmap.find(rofl::openflow::OFPIT_GOTO_TABLE) == instmap.end()) {
		return;
	}
	delete instmap[rofl::openflow::OFPIT_GOTO_TABLE];
	instmap.erase(rofl::openflow::OFPIT_GOTO_TABLE);
}



bool
cofinstructions::has_inst_goto_table() const
{
	return (not (instmap.find(rofl::openflow::OFPIT_GOTO_TABLE) == instmap.end()));
}



cofinstruction_write_metadata&
cofinstructions::add_inst_write_metadata()
{
	if (instmap.find(rofl::openflow::OFPIT_WRITE_METADATA) != instmap.end()) {
		delete instmap[rofl::openflow::OFPIT_WRITE_METADATA];
	}
	instmap[rofl::openflow::OFPIT_WRITE_METADATA] = new cofinstruction_write_metadata(ofp_version);
	return *dynamic_cast<cofinstruction_write_metadata*>(instmap[rofl::openflow::OFPIT_WRITE_METADATA]);
}



cofinstruction_write_metadata&
cofinstructions::set_inst_write_metadata()
{
	if (instmap.find(rofl::openflow::OFPIT_WRITE_METADATA) == instmap.end()) {
		instmap[rofl::openflow::OFPIT_WRITE_METADATA] = new cofinstruction_write_metadata(ofp_version);
	}
	return *dynamic_cast<cofinstruction_write_metadata*>(instmap[rofl::openflow::OFPIT_WRITE_METADATA]);
}



const cofinstruction_write_metadata&
cofinstructions::get_inst_write_metadata() const
{
	if (instmap.find(rofl::openflow::OFPIT_WRITE_METADATA) == instmap.end()) {
		throw eInstructionNotFound();
	}
	return *(dynamic_cast<const cofinstruction_write_metadata*>( instmap.at(rofl::openflow::OFPIT_WRITE_METADATA) ));
}



void
cofinstructions::drop_inst_write_metadata()
{
	if (instmap.find(rofl::openflow::OFPIT_WRITE_METADATA) == instmap.end()) {
		return;
	}
	delete instmap[rofl::openflow::OFPIT_WRITE_METADATA];
	instmap.erase(rofl::openflow::OFPIT_WRITE_METADATA);
}



bool
cofinstructions::has_inst_write_metadata() const
{
	return (not (instmap.find(rofl::openflow::OFPIT_WRITE_METADATA) == instmap.end()));
}



cofinstruction_write_actions&
cofinstructions::add_inst_write_actions()
{
	if (instmap.find(rofl::openflow::OFPIT_WRITE_ACTIONS) != instmap.end()) {
		delete instmap[rofl::openflow::OFPIT_WRITE_ACTIONS];
	}
	instmap[rofl::openflow::OFPIT_WRITE_ACTIONS] = new cofinstruction_write_actions(ofp_version);
	return *dynamic_cast<cofinstruction_write_actions*>(instmap[rofl::openflow::OFPIT_WRITE_ACTIONS]);
}



cofinstruction_write_actions&
cofinstructions::set_inst_write_actions()
{
	if (instmap.find(rofl::openflow::OFPIT_WRITE_ACTIONS) == instmap.end()) {
		instmap[rofl::openflow::OFPIT_WRITE_ACTIONS] = new cofinstruction_write_actions(ofp_version);
	}
	return *dynamic_cast<cofinstruction_write_actions*>(instmap[rofl::openflow::OFPIT_WRITE_ACTIONS]);
}



const cofinstruction_write_actions&
cofinstructions::get_inst_write_actions() const
{
	if (instmap.find(rofl::openflow::OFPIT_WRITE_ACTIONS) == instmap.end()) {
		throw eInstructionNotFound();
	}
	return *(dynamic_cast<const cofinstruction_write_actions*>( instmap.at(rofl::openflow::OFPIT_WRITE_ACTIONS) ));
}



void
cofinstructions::drop_inst_write_actions()
{
	if (instmap.find(rofl::openflow::OFPIT_WRITE_ACTIONS) == instmap.end()) {
		return;
	}
	delete instmap[rofl::openflow::OFPIT_WRITE_ACTIONS];
	instmap.erase(rofl::openflow::OFPIT_WRITE_ACTIONS);
}



bool
cofinstructions::has_inst_write_actions() const
{
	return (not (instmap.find(rofl::openflow::OFPIT_WRITE_ACTIONS) == instmap.end()));
}



cofinstruction_apply_actions&
cofinstructions::add_inst_apply_actions()
{
	if (instmap.find(rofl::openflow::OFPIT_APPLY_ACTIONS) != instmap.end()) {
		delete instmap[rofl::openflow::OFPIT_APPLY_ACTIONS];
	}
	instmap[rofl::openflow::OFPIT_APPLY_ACTIONS] = new cofinstruction_apply_actions(ofp_version);
	return *dynamic_cast<cofinstruction_apply_actions*>(instmap[rofl::openflow::OFPIT_APPLY_ACTIONS]);
}



cofinstruction_apply_actions&
cofinstructions::set_inst_apply_actions()
{
	if (instmap.find(rofl::openflow::OFPIT_APPLY_ACTIONS) == instmap.end()) {
		instmap[rofl::openflow::OFPIT_APPLY_ACTIONS] = new cofinstruction_apply_actions(ofp_version);
	}
	return *dynamic_cast<cofinstruction_apply_actions*>(instmap[rofl::openflow::OFPIT_APPLY_ACTIONS]);
}



const cofinstruction_apply_actions&
cofinstructions::get_inst_apply_actions() const
{
	if (instmap.find(rofl::openflow::OFPIT_APPLY_ACTIONS) == instmap.end()) {
		throw eInstructionNotFound();
	}
	return *(dynamic_cast<const cofinstruction_apply_actions*>( instmap.at(rofl::openflow::OFPIT_APPLY_ACTIONS) ));
}



void
cofinstructions::drop_inst_apply_actions()
{
	if (instmap.find(rofl::openflow::OFPIT_APPLY_ACTIONS) == instmap.end()) {
		return;
	}
	delete instmap[rofl::openflow::OFPIT_APPLY_ACTIONS];
	instmap.erase(rofl::openflow::OFPIT_APPLY_ACTIONS);
}



bool
cofinstructions::has_inst_apply_actions() const
{
	return (not (instmap.find(rofl::openflow::OFPIT_APPLY_ACTIONS) == instmap.end()));
}



cofinstruction_clear_actions&
cofinstructions::add_inst_clear_actions()
{
	if (instmap.find(rofl::openflow::OFPIT_CLEAR_ACTIONS) != instmap.end()) {
		delete instmap[rofl::openflow::OFPIT_CLEAR_ACTIONS];
	}
	instmap[rofl::openflow::OFPIT_CLEAR_ACTIONS] = new cofinstruction_clear_actions(ofp_version);
	return *dynamic_cast<cofinstruction_clear_actions*>(instmap[rofl::openflow::OFPIT_CLEAR_ACTIONS]);
}



cofinstruction_clear_actions&
cofinstructions::set_inst_clear_actions()
{
	if (instmap.find(rofl::openflow::OFPIT_CLEAR_ACTIONS) == instmap.end()) {
		instmap[rofl::openflow::OFPIT_CLEAR_ACTIONS] = new cofinstruction_clear_actions(ofp_version);
	}
	return *dynamic_cast<cofinstruction_clear_actions*>(instmap[rofl::openflow::OFPIT_CLEAR_ACTIONS]);
}



const cofinstruction_clear_actions&
cofinstructions::get_inst_clear_actions() const
{
	if (instmap.find(rofl::openflow::OFPIT_CLEAR_ACTIONS) == instmap.end()) {
		throw eInstructionNotFound();
	}
	return *(dynamic_cast<const cofinstruction_clear_actions*>( instmap.at(rofl::openflow::OFPIT_CLEAR_ACTIONS) ));
}



void
cofinstructions::drop_inst_clear_actions()
{
	if (instmap.find(rofl::openflow::OFPIT_CLEAR_ACTIONS) == instmap.end()) {
		return;
	}
	delete instmap[rofl::openflow::OFPIT_CLEAR_ACTIONS];
	instmap.erase(rofl::openflow::OFPIT_CLEAR_ACTIONS);
}



bool
cofinstructions::has_inst_clear_actions() const
{
	return (not (instmap.find(rofl::openflow::OFPIT_CLEAR_ACTIONS) == instmap.end()));
}



cofinstruction_experimenter&
cofinstructions::add_inst_experimenter()
{
	if (instmap.find(rofl::openflow::OFPIT_EXPERIMENTER) != instmap.end()) {
		delete instmap[rofl::openflow::OFPIT_EXPERIMENTER];
	}
	instmap[rofl::openflow::OFPIT_EXPERIMENTER] = new cofinstruction_experimenter(ofp_version);
	return *dynamic_cast<cofinstruction_experimenter*>(instmap[rofl::openflow::OFPIT_EXPERIMENTER]);
}



cofinstruction_experimenter&
cofinstructions::set_inst_experimenter()
{
	if (instmap.find(rofl::openflow::OFPIT_EXPERIMENTER) == instmap.end()) {
		instmap[rofl::openflow::OFPIT_EXPERIMENTER] = new cofinstruction_experimenter(ofp_version);
	}
	return *dynamic_cast<cofinstruction_experimenter*>(instmap[rofl::openflow::OFPIT_EXPERIMENTER]);
}



const cofinstruction_experimenter&
cofinstructions::get_inst_experimenter() const
{
	if (instmap.find(rofl::openflow::OFPIT_EXPERIMENTER) == instmap.end()) {
		throw eInstructionNotFound();
	}
	return *(dynamic_cast<const cofinstruction_experimenter*>( instmap.at(rofl::openflow::OFPIT_EXPERIMENTER) ));
}



void
cofinstructions::drop_inst_experimenter()
{
	if (instmap.find(rofl::openflow::OFPIT_EXPERIMENTER) == instmap.end()) {
		return;
	}
	delete instmap[rofl::openflow::OFPIT_EXPERIMENTER];
	instmap.erase(rofl::openflow::OFPIT_EXPERIMENTER);
}



bool
cofinstructions::has_inst_experimenter() const
{
	return (not (instmap.find(rofl::openflow::OFPIT_EXPERIMENTER) == instmap.end()));
}



cofinstruction_meter&
cofinstructions::add_inst_meter()
{
	if (instmap.find(rofl::openflow::OFPIT_METER) != instmap.end()) {
		delete instmap[rofl::openflow::OFPIT_METER];
	}
	instmap[rofl::openflow::OFPIT_METER] = new cofinstruction_meter(ofp_version);
	return *dynamic_cast<cofinstruction_meter*>(instmap[rofl::openflow::OFPIT_METER]);
}



cofinstruction_meter&
cofinstructions::set_inst_meter()
{
	if (instmap.find(rofl::openflow::OFPIT_METER) == instmap.end()) {
		instmap[rofl::openflow::OFPIT_METER] = new cofinstruction_meter(ofp_version);
	}
	return *dynamic_cast<cofinstruction_meter*>(instmap[rofl::openflow::OFPIT_METER]);
}



const cofinstruction_meter&
cofinstructions::get_inst_meter() const
{
	if (instmap.find(rofl::openflow::OFPIT_METER) == instmap.end()) {
		throw eInstructionNotFound();
	}
	return *(dynamic_cast<const cofinstruction_meter*>( instmap.at(rofl::openflow::OFPIT_METER) ));
}



void
cofinstructions::drop_inst_meter()
{
	if (instmap.find(rofl::openflow::OFPIT_METER) == instmap.end()) {
		return;
	}
	delete instmap[rofl::openflow::OFPIT_METER];
	instmap.erase(rofl::openflow::OFPIT_METER);
}



bool
cofinstructions::has_inst_meter() const
{
	return (not (instmap.find(rofl::openflow::OFPIT_METER) == instmap.end()));
}



size_t
cofinstructions::length() const
{
	size_t inlen = 0;

	for (std::map<uint16_t, cofinstruction*>::const_iterator
			it = instmap.begin(); it != instmap.end(); ++it) {
		inlen += it->second->length();
	}
	return inlen;
}



void
cofinstructions::pack(
		uint8_t* buf, size_t buflen)
{
	if (buflen < length())
		throw eInstructionsInval();

	for (std::map<uint16_t, cofinstruction*>::iterator
			it = instmap.begin(); it != instmap.end(); ++it) {
		cofinstruction& inst = *(it->second);
		inst.pack(buf, inst.length());
		buf += inst.length();
	}
}



void
cofinstructions::unpack(
		uint8_t* buf, size_t buflen)
{
	clear();

	if (buflen < (int)sizeof(struct rofl::openflow::ofp_instruction))
		return;


	while (buflen > 0) {
		// first instruction
		struct rofl::openflow::ofp_instruction *hdr = (struct rofl::openflow::ofp_instruction*)buf;

		uint16_t type = be16toh(hdr->type);
		uint16_t len = be16toh(hdr->len);

		if (len < sizeof(struct rofl::openflow::ofp_instruction))
			throw eInstructionBadLen();

		if (len > buflen)
			throw eInstructionBadLen();


		switch (type) {
		case rofl::openflow::OFPIT_GOTO_TABLE: {
			add_inst_goto_table().unpack(buf, len);
			buflen -= get_inst_goto_table().length();
			buf += get_inst_goto_table().length();
		} break;
		case rofl::openflow::OFPIT_WRITE_METADATA: {
			add_inst_write_metadata().unpack(buf, len);
			buflen -= get_inst_write_metadata().length();
			buf += get_inst_write_metadata().length();
		} break;
		case rofl::openflow::OFPIT_WRITE_ACTIONS: {
			add_inst_write_actions().unpack(buf, len);
			buflen -= get_inst_write_actions().length();
			buf += get_inst_write_actions().length();
		} break;
		case rofl::openflow::OFPIT_APPLY_ACTIONS: {
			add_inst_apply_actions().unpack(buf, len);
			buflen -= get_inst_apply_actions().length();
			buf += get_inst_apply_actions().length();
		} break;
		case rofl::openflow::OFPIT_CLEAR_ACTIONS: {
			add_inst_clear_actions().unpack(buf, len);
			buflen -= get_inst_clear_actions().length();
			buf += get_inst_clear_actions().length();
		} break;
		case rofl::openflow::OFPIT_METER: {
			add_inst_meter().unpack(buf, len);
			buflen -= get_inst_meter().length();
			buf += get_inst_meter().length();
		} break;
		case rofl::openflow::OFPIT_EXPERIMENTER: {
			add_inst_experimenter().unpack(buf, len);
			buflen -= get_inst_experimenter().length();
			buf += get_inst_experimenter().length();
		} break;
		default:
			LOGGING_WARN << "[rofl][instructions] unknown instruction type:" << type << std::endl;
		}
	}
}



void
cofinstructions::check_prerequisites() const
{
	for (std::map<uint16_t, cofinstruction*>::const_iterator
			it = instmap.begin(); it != instmap.end(); ++it) {
		it->second->check_prerequisites();
	}
}



