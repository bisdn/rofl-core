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
	WRITELOG(COFINST, DBG, "cofinst(%p)::cofinst() [1]", this);
	oin_header = (struct ofp_instruction*)instruction.somem();
	pthread_mutex_init(&inmutex, NULL);
	//cofinst_set.insert(this);
}


cofinst::cofinst(
		uint8_t ofp_version,
		struct ofp_instruction* inhdr,
		size_t inlen) :
				ofp_version(ofp_version),
				actions(ofp_version),
				instruction(inlen)
{
	WRITELOG(COFINST, DBG, "cofinst(%p)::cofinst() [2]", this);
	pthread_mutex_init(&inmutex, NULL);
	unpack(inhdr, inlen);
	//cofinst_set.insert(this);
}


cofinst::cofinst(cofinst const& inst)
{
	pthread_mutex_init(&inmutex, NULL);
	*this = inst;
}


cofinst::~cofinst()
{
	//cofinst_set.erase(this);
	WRITELOG(COFINST, DBG, "cofinst(%p)::~cofinst()", this);
	pthread_mutex_destroy(&inmutex);
}


cofinst&
cofinst::operator= (const cofinst& inst)
{
	if (this == &inst)
		return *this;

	WRITELOG(COFINST, DBG, "cofinst(%p)::operator=() inst: %p", this, &inst);

	this->ofp_version = inst.ofp_version;
	this->actions = inst.actions;
	this->instruction = inst.instruction;
	this->oin_header = (struct ofp_instruction*)(this->instruction.somem());

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
	oin_header = (struct openflow::ofp_instruction*)instruction.somem();
}


struct ofp_instruction*
cofinst::pack(
		struct ofp_instruction* inhdr,
		size_t inlen) const throw (eInstructionInval)
{
	//Lock lock(&inmutex);

	if (inlen < this->length())
		throw eInstructionInval();

	//WRITELOG(CSYSLOG_ACTION, "soaction()=%p actionlen()=%d", soaction(), actionlen());

	inhdr->type = oin_header->type; // set type field, hint: this is already network byte order
	inhdr->len  = htobe16(length()); // set length field

	// step -2- copy all actions into the memory area for instructions APPLY/CLEAR/WRITE_ACTIONS
	switch (be16toh(inhdr->type)) {
	case OFPIT_APPLY_ACTIONS:
	case OFPIT_CLEAR_ACTIONS:
	case OFPIT_WRITE_ACTIONS:
		{
			size_t aclen = this->length() - sizeof(struct ofp_instruction);

			actions.pack(((struct ofp_instruction_actions*)inhdr)->actions, aclen);
		}
		break;

	case OFPIT_WRITE_METADATA:
		{
			if (inlen < sizeof(struct ofp_instruction_write_metadata))
			{
				throw eInstructionInval();
			}
			memcpy(inhdr, oin_header, sizeof(struct ofp_instruction_write_metadata));
		}
		break;

	case OFPIT_GOTO_TABLE:
		{
			if (inlen < sizeof(struct ofp_instruction_goto_table))
			{
				throw eInstructionInval();
			}
			memcpy(inhdr, oin_header, sizeof(struct ofp_instruction_goto_table));
		}
		break;

	case OFPIT_EXPERIMENTER:
		// do nothing for now
		break;
	}

	oin_header->len = htobe16(length());

	return inhdr;
}


void
cofinst::unpack(
		struct ofp_instruction *inhdr,
		size_t inlen) throw (eInstructionBadLen, eInstructionBadExperimenter)
{
	reset();

	//Lock lock(&inmutex);

	if (inlen > instruction.memlen())
	{
		oin_header = (struct ofp_instruction*)instruction.resize(inlen);
	}

	memcpy(instruction.somem(), (uint8_t*)inhdr, inlen);
	oin_header = (struct ofp_instruction*)instruction.somem();


	switch (be16toh(oin_header->type)) {
	case OFPIT_APPLY_ACTIONS:
	case OFPIT_WRITE_ACTIONS:
	case OFPIT_CLEAR_ACTIONS:
	{
		oin_actions = (struct ofp_instruction_actions*)oin_header;

		size_t aclen = inlen - sizeof(struct ofp_instruction);

		if (aclen >= sizeof(struct ofp_action_header))
		{
			actions.unpack(oin_actions->actions, aclen);
		}
		break;
	}
	case OFPIT_WRITE_METADATA:
		oin_write_metadata = (struct ofp_instruction_write_metadata*)oin_header;
		break;
	case OFPIT_GOTO_TABLE:
		oin_goto_table = (struct ofp_instruction_goto_table*)oin_header;
		break;
	case OFPIT_EXPERIMENTER:
		oin_experimenter = (struct ofp_instruction_experimenter*)oin_header;
		switch (be32toh(oin_experimenter->experimenter)) {
		/*
		 * add your known experimenter extensions here
		 */
		default:
			throw eInstructionBadExperimenter();
		}
		break;
	default:
		WRITELOG(COFINST, DBG, "cofinst(%p)::__parse_inst() unknown instruction type %d", this, be16toh(oin_header->type));
		throw eInstructionInvalType();
	}
}


struct ofp_instruction*
cofinst::soinst()
{
	return ((struct ofp_instruction*)instruction.somem());
}


size_t
cofinst::length() const throw (eInstructionInvalType)
{
	switch (be16toh(oin_header->type)) {
	case OFPIT_APPLY_ACTIONS:
	case OFPIT_CLEAR_ACTIONS:
	case OFPIT_WRITE_ACTIONS:
		{
			return (sizeof(struct ofp_instruction_actions) +
								actions.length());
		}
		break;
	case OFPIT_WRITE_METADATA:
		return sizeof(struct ofp_instruction_write_metadata);
		break;
	case OFPIT_GOTO_TABLE:
		return sizeof(struct ofp_instruction_goto_table);
		break;
	case OFPIT_EXPERIMENTER:
		return sizeof(struct ofp_instruction_experimenter);
		break;
	default:
		WRITELOG(COFACTION, DBG, "cofinst(%p)::instlen() unknown instruction type %d", this, be16toh(oin_header->type));
		throw eInstructionInvalType();
	}
}






cofaction&
cofinst::find_action(uint8_t type)
{
	return actions.find_action(type);
}


template class coflist<cofinst>;
