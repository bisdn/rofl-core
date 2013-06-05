/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFINST_H
#define COFINST_H 1

#include <set>
#include <vector>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif
#include "openflow.h"
#include <endian.h>
#include <pthread.h>
#ifndef htobe16
#include "../endian_conversion.h"
#endif
#ifdef __cplusplus
}
#endif

#include "../cmemory.h"
#include "../cerror.h"
#include "../cvastring.h"
#include "../thread_helper.h"
#include "cofaclist.h"

#include "rofl/platform/unix/csyslog.h"

#include "cofaction.h"

namespace rofl
{

/* error classes */
class eInstructionBase : public cerror {}; // error base class for class cofinstruction
class eInstructionInval : public eInstructionBase {}; // invalid parameter
class eInstructionBadLen : public eInstructionBase {}; // bad length
class eInstructionInvalType : public eInstructionBase {}; // invalid instruction type
class eInstructionHeaderInval : public eInstructionBase {}; // invalid instruction header
class eInstructionActionNotFound : public eInstructionBase {}; // action not found in instruction
class eInstructionBadExperimenter : public eInstructionBase {}; // unknown experimenter instruction


class cofinst :
	public csyslog
{
public: // static stuff, enums, constants

	#define COFINST_DEFAULT_LEN	128
	static std::set<cofinst*> cofinst_set;

public: // data structures

	pthread_mutex_t inmutex; // mutex for this cofinst instance
	cofaclist actions;	// vector of cofaction instances

	union { // for OpenFlow 1.1
		struct ofp_instruction* oinu_header;
		struct ofp_instruction_goto_table* oinu_goto_table;
		struct ofp_instruction_actions* oinu_actions;
		struct ofp_instruction_experimenter* oinu_experimenter;
		struct ofp_instruction_write_metadata* oinu_write_metadata;
	} oin_oinu;

#define oin_header oin_oinu.oinu_header					// instruction: plain header
#define oin_goto_table oin_oinu.oinu_goto_table			// instruction: goto table
#define oin_actions oin_oinu.oinu_actions				// instruction: actions
#define oin_experimenter oin_oinu.oinu_experimenter		// instruction: experimenter
#define oin_write_metadata oin_oinu.oinu_write_metadata	// instruction: write metadata



public: // methods

	/** constructor
	 */
	cofinst(
			size_t size = COFINST_DEFAULT_LEN);

	/** constructor
	 */
	cofinst(
			struct ofp_instruction *instruction,
			size_t inlen);

	/** copy constructor
	 */
	cofinst(cofinst const& inst);

	/** destructor
	 */
	virtual
	~cofinst();

	/** assignment operator
	 */
	cofinst&
	operator= (const cofinst& inst);

	/** reset (=clears all actions)
	 */
	void
	reset();

	/** return pointer to ofp_instruction start
	 */
	struct ofp_instruction*
	soinst();

	/** return length of action in bytes
	 */
	size_t
	length() const throw (eInstructionInvalType);

	/** find specific action
	 */
	cofaction&
	find_action(enum ofp_action_type type);

	/** dump info string for this action
	 */
	const char*
	c_str();

	/** copy struct ofp_action_header
	 */
	virtual struct ofp_instruction*
	pack(
		struct ofp_instruction* inhdr,
		size_t inlen) const throw (eInstructionInval);

	/** unpack
	 */
	virtual void
	unpack(
			struct ofp_instruction *inhdr,
			size_t inlen)
		throw (eInstructionBadLen, eInstructionBadExperimenter);


	/**
	 *
	 */
	uint16_t
	get_type()
	{
		return be16toh(oin_header->type);
	};


protected: // data structures

	cmemory instruction; // memory area with original packes instruction

	std::string info; // info string
};

/** predicate for finding cofaction instances of
 * a specific type
 */
class cofinst_find_type {
public:
	cofinst_find_type(uint16_t type) :
		type(type) { };

	bool operator() (cofinst const& inst) {
		return (be16toh(inst.oin_header->type) == type);
	};

	uint16_t type;
};


/** OFPIT_APPLY_ACTIONS
 *
 */
class cofinst_apply_actions : public cofinst {
public:
	/** constructor
	 */
	cofinst_apply_actions()
	{
		oin_header->type = htobe16(OFPIT_APPLY_ACTIONS);
		oin_header->len = htobe16(0); // fill this when calling method pack()
	};
	/** destructor
	 */
	virtual
	~cofinst_apply_actions() {};
};


/** OFPIT_WRITE_ACTIONS
 *
 */
class cofinst_write_actions : public cofinst {
public:
	/** constructor
	 */
	cofinst_write_actions()
	{
		oin_header->type = htobe16(OFPIT_WRITE_ACTIONS);
		oin_header->len = htobe16(0); // fill this when calling method pack()
	};
	/** destructor
	 */
	virtual
	~cofinst_write_actions() {};
};


/** OFPIT_CLEAR_ACTIONS
 *
 */
class cofinst_clear_actions : public cofinst {
public:
	/** constructor
	 */
	cofinst_clear_actions()
	{
		oin_header->type = htobe16(OFPIT_CLEAR_ACTIONS);
		oin_header->len = htobe16(0); // fill this when calling method pack()
	};
	/** destructor
	 */
	virtual
	~cofinst_clear_actions() {};
};


/** OFPIT_GOTO_TABLE
 *
 */
class cofinst_goto_table : public cofinst {
public:
	/** constructor
	 */
	cofinst_goto_table(
			uint8_t table_id) :
				cofinst(sizeof(struct ofp_instruction_goto_table))
	{
		oin_goto_table->type = htobe16(OFPIT_GOTO_TABLE);
		oin_goto_table->len = htobe16(sizeof(struct ofp_instruction_goto_table));
		oin_goto_table->table_id = table_id;
	};
	/** destructor
	 */
	virtual
	~cofinst_goto_table() {};
#if 0
	/**
	 *
	 */
	virtual struct ofp_instruction*
	pack(struct ofp_instruction* inst, size_t instlen) const;
	/**
	 *
	 */
	virtual void
	unpack(struct ofp_instruction* inst, size_t instlen);
#endif
	/**
	 *
	 */
	uint8_t
	get_table_id() const
	{
		return oin_goto_table->table_id;
	};
	/**
	 *
	 */
	void
	set_table_id(uint8_t table_id)
	{
		oin_goto_table->table_id = table_id;
	};
};


/** OFPIT_WRITE_METADATA
 *
 */
class cofinst_write_metadata : public cofinst {
public:
	/** constructor
	 */
	cofinst_write_metadata(
			uint64_t metadata,
			uint64_t metadata_mask) :
				cofinst(sizeof(struct ofp_instruction_write_metadata))
	{
		oin_write_metadata->type = htobe16(OFPIT_WRITE_METADATA);
		oin_write_metadata->len = htobe16(sizeof(struct ofp_instruction_write_metadata));
		oin_write_metadata->metadata = htobe64(metadata);
		oin_write_metadata->metadata_mask = htobe64(metadata_mask);
	};
	/** destructor
	 */
	virtual
	~cofinst_write_metadata() {};
};

}; // end of namespace


#endif
