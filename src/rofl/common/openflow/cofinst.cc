/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofinst.h"

std::set<cofinst*> cofinst::cofinst_set;

cofinst::cofinst(
	size_t size) :
		instruction(size)
{
	WRITELOG(COFINST, DBG, "cofinst(%p)::cofinst() [1]", this);
	oin_header = (struct ofp_instruction*)instruction.somem();
	pthread_mutex_init(&inmutex, NULL);
	//cofinst_set.insert(this);
}


cofinst::cofinst(
	struct ofp_instruction* inhdr,
	size_t inlen) :
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

	this->actions = inst.actions;
	this->instruction = inst.instruction;
	this->oin_header = (struct ofp_instruction*)(this->instruction.somem());

	return *this;
}


void
cofinst::reset()
{
	instruction.clear();
	actions.reset();
}


struct ofp_instruction*
cofinst::pack(
	struct ofp_instruction* inhdr,
	size_t inlen) throw (eInstructionInval)
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
		size_t inlen)
throw (eInstructionBadLen, eInstructionBadExperimenter)
{
	reset();

	//Lock lock(&inmutex);

	if (inlen > instruction.memlen())
	{
		oin_header = (struct ofp_instruction*)instruction.resize(inlen);
	}

	memcpy(instruction.somem(), (uint8_t*)inhdr, inlen);
	oin_header = (struct ofp_instruction*)instruction.somem();

	WRITELOG(COFINST, DBG, "cofinst(%p)::unpack() oin_header: %p instruction: %s",
			this, oin_header, instruction.c_str());

	switch (be16toh(oin_header->type)) {
	case OFPIT_APPLY_ACTIONS:
	case OFPIT_WRITE_ACTIONS:
	case OFPIT_CLEAR_ACTIONS:
	{
		oin_actions = (struct ofp_instruction_actions*)oin_header;

		size_t aclen = inlen - sizeof(struct ofp_instruction);

		WRITELOG(COFINST, DBG, "cofinst(%p)::unpack() oin_header: %p instruction: %s aclen: %d/%d",
				this, oin_header, instruction.c_str(), aclen, sizeof(struct ofp_action_header));

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

	WRITELOG(COFINST, DBG, "cofinst(%p)::unpack() actions: %s",
			this, oin_header, actions.c_str());
}


struct ofp_instruction*
cofinst::soinst()
{
	return ((struct ofp_instruction*)instruction.somem());
}


size_t
cofinst::length() throw (eInstructionInvalType)
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





const char*
cofinst::c_str()
{
	Lock lock(&inmutex);

	cvastring vas(4096);

	switch (be16toh(oin_header->type)) {
	case OFPIT_APPLY_ACTIONS:
		{
			info.assign(vas("cofinst(%p) OFPIT_APPLY_ACTIONS length: %03d \n%s",
					this, length(), actions.c_str()));
		}
		break;
	case OFPIT_WRITE_ACTIONS:
		{
			info.assign(vas("cofinst(%p) OFPIT_WRITE_ACTIONS length: %03d \n%s",
					this, length(), actions.c_str()));
		}
		break;
	case OFPIT_CLEAR_ACTIONS:
		{
			info.assign(vas("cofinst(%p) OFPIT_CLEAR_ACTIONS length: %03d \n%s",
					this, length(), actions.c_str()));
		}
		break;
	case OFPIT_WRITE_METADATA:
		{
			info.assign(vas("cofinst(%p) OFPIT_WRITE_METADATA length: %03d metadata[%lu] mask[%lu]",
					this,
					length(),
					be64toh(oin_write_metadata->metadata),
					be64toh(oin_write_metadata->metadata_mask)));
		}
		break;
	case OFPIT_GOTO_TABLE:
		{
			info.assign(vas("cofinst(%p) OFPIT_GOTO_TABLE length: %03d table_id[%d]",
					this,
					length(),
					oin_goto_table->table_id));
		}
		break;
	default:
		//throw eInstructionInvalType();
		info.assign(vas("cofinst(%p) UNKNOWN INSTRUCTION", this));
		break;
	}
	return info.c_str();
}


cofaction&
cofinst::find_action(enum ofp_action_type type)
{
	return actions.find_action(type);
}


template class coflist<cofinst>;
