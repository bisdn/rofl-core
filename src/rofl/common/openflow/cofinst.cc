/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofinst.h"

using namespace rofl;

std::set<cofinst*> cofinst::cofinst_set;

cofinst::cofinst(
		uint8_t ofp_version,
		size_t size) :
				ofp_version(ofp_version),
				actions(ofp_version),
				instruction(size)
{
	switch (ofp_version) {
	case OFP_VERSION_UNKNOWN: break;
	case openflow12::OFP_VERSION: break;
	case openflow13::OFP_VERSION: break;
	default: throw eBadVersion();
	}
	oin_generic = instruction.somem();
	pthread_mutex_init(&inmutex, NULL);
}


cofinst::cofinst(
		uint8_t ofp_version,
		uint8_t* inhdr,
		size_t inlen) :
				ofp_version(ofp_version),
				actions(ofp_version),
				instruction(inlen)
{
	switch (ofp_version) {
	case OFP_VERSION_UNKNOWN: break;
	case openflow12::OFP_VERSION: break;
	case openflow13::OFP_VERSION: break;
	default: throw eBadVersion();
	}
	pthread_mutex_init(&inmutex, NULL);
	unpack(inhdr, inlen);
}


cofinst::cofinst(cofinst const& inst)
{
	pthread_mutex_init(&inmutex, NULL);
	*this = inst;
}


cofinst::~cofinst()
{
	pthread_mutex_destroy(&inmutex);
}


cofinst&
cofinst::operator= (const cofinst& inst)
{
	if (this == &inst)
		return *this;

	this->ofp_version 	= inst.ofp_version;
	this->actions 		= inst.actions;
	this->instruction 	= inst.instruction;
	this->oin_generic 	= this->instruction.somem();

	return *this;
}


void
cofinst::reset()
{
	instruction.clear();
	actions.clear();
}


void
cofinst::resize(size_t len)
{
	instruction.resize(len);
	oin_generic = instruction.somem();
}


uint8_t*
cofinst::pack(uint8_t* buf, size_t buflen)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: return pack_of12(buf, buflen);
	default: throw eBadVersion();
	}
}


uint8_t*
cofinst::pack_of12(uint8_t* buf, size_t buflen)
{
	if (buflen < this->length())
		throw eInstructionInval();

	struct openflow::ofp_instruction *inhdr = (struct openflow::ofp_instruction*)buf;

	inhdr->type = oin_header->type; // set type field, hint: this is already network byte order
	inhdr->len  = htobe16(length()); // set length field

	// step -2- copy all actions into the memory area for instructions APPLY/CLEAR/WRITE_ACTIONS
	switch (be16toh(inhdr->type)) {
	case openflow12::OFPIT_APPLY_ACTIONS:
	case openflow12::OFPIT_CLEAR_ACTIONS:
	case openflow12::OFPIT_WRITE_ACTIONS: {
		if (buflen < (sizeof(struct openflow12::ofp_instruction_actions) + actions.length())) {
			throw eInstructionInval();
		}
		size_t aclen = this->length() - sizeof(struct openflow::ofp_instruction);
		actions.pack((uint8_t*)((struct openflow12::ofp_instruction_actions*)inhdr)->actions, aclen);
	} break;
	case openflow12::OFPIT_WRITE_METADATA: {
		if (buflen < sizeof(struct openflow12::ofp_instruction_write_metadata)) {
			throw eInstructionInval();
		}
		memcpy(inhdr, oin_header, sizeof(struct openflow12::ofp_instruction_write_metadata));
	} break;
	case openflow12::OFPIT_GOTO_TABLE: {
		if (buflen < sizeof(struct openflow12::ofp_instruction_goto_table)) {
			throw eInstructionInval();
		}
		memcpy(inhdr, oin_header, sizeof(struct openflow12::ofp_instruction_goto_table));
	} break;
	case openflow12::OFPIT_EXPERIMENTER:
		// do nothing for now
		logging::warn << "[rofl][instruction] unsupported experimental instruction type: " << (int)inhdr->type << std::endl;
		break;
	default:
		logging::error << "[rofl][instruction] unsupported instruction type: " << (int)inhdr->type << std::endl;
	}

	oin_header->len = htobe16(length());

	return buf;
}


void
cofinst::unpack(uint8_t* buf, size_t buflen)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: unpack_of12(buf, buflen); break;
	default: throw eBadVersion();
	}
}


void
cofinst::unpack_of12(uint8_t* buf, size_t buflen)
{
	reset();

	if (buflen > instruction.memlen()) {
		oin_generic = instruction.resize(buflen);
	} else {
		oin_generic = instruction.somem();
	}

	memcpy(instruction.somem(), buf, buflen);

	switch (get_type()) {
	case openflow12::OFPIT_APPLY_ACTIONS:
	case openflow12::OFPIT_WRITE_ACTIONS:
	case openflow12::OFPIT_CLEAR_ACTIONS: {
		oin_actions = (struct openflow12::ofp_instruction_actions*)oin_header;
		size_t aclen = buflen - sizeof(struct openflow12::ofp_instruction);
		if (aclen >= sizeof(struct openflow12::ofp_action_header)) {
			actions.unpack((uint8_t*)(oin_actions->actions), aclen);
		}
	} break;
	case openflow12::OFPIT_WRITE_METADATA:
		oin_write_metadata = (struct openflow12::ofp_instruction_write_metadata*)oin_header;
		break;
	case openflow12::OFPIT_GOTO_TABLE:
		oin_goto_table = (struct openflow12::ofp_instruction_goto_table*)oin_header;
		break;
	case openflow12::OFPIT_EXPERIMENTER:
		oin_experimenter = (struct openflow12::ofp_instruction_experimenter*)oin_header;
		logging::warn << "[rofl][instruction] unsupported experimental instruction type: " << (int)get_type() << std::endl;
		switch (be32toh(oin_experimenter->experimenter)) {
		/*
		 * add your known experimenter extensions here
		 */
		default:
			throw eInstructionBadExperimenter();
		}
		break;
	case openflow13::OFPIT_METER:
		oin_meter = (struct openflow13::ofp_instruction_meter*)oin_header;
		// TODO: parse remaining instructions
		break;
	default:
		logging::error << "[rofl][instruction] unsupported instruction type: " << (int)get_type() << std::endl;
		throw eInstructionInvalType();
	}
}


struct openflow::ofp_instruction*
cofinst::soinst()
{
	return ((struct openflow::ofp_instruction*)instruction.somem());
}


size_t
cofinst::length() const
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		switch (get_type()) {
		case openflow12::OFPIT_APPLY_ACTIONS:
		case openflow12::OFPIT_CLEAR_ACTIONS:
		case openflow12::OFPIT_WRITE_ACTIONS:
			return (sizeof(struct openflow12::ofp_instruction_actions) + actions.length());
		case openflow12::OFPIT_WRITE_METADATA:
			return sizeof(struct openflow12::ofp_instruction_write_metadata);
		case openflow12::OFPIT_GOTO_TABLE:
			return sizeof(struct openflow12::ofp_instruction_goto_table);
		case openflow12::OFPIT_EXPERIMENTER:
			return sizeof(struct openflow12::ofp_instruction_experimenter);
		default:
			logging::error << "[rofl][instruction] unsupported instruction type: " << (int)get_type() << std::endl;
			throw eInstructionInvalType();
		}
	} break;
	default:
		throw eBadVersion();
	}
}






cofaction&
cofinst::find_action(uint8_t type)
{
	return actions.find_action(type);
}


template class coflist<cofinst>;
