/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFINST_H
#define COFINST_H 1

#include <set>
#include <vector>
#include <algorithm>
#include <endian.h>
#include <pthread.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "openflow.h"
#include "../cmemory.h"
#include "../cerror.h"
#include "../cvastring.h"
#include "../thread_helper.h"
#include "cofactions.h"

#include "rofl/platform/unix/csyslog.h"

#include "cofaction.h"

namespace rofl
{

/* error classes */
class eInstructionBase 				: public cerror {}; // error base class for class cofinstruction
class eInstructionInval 			: public eInstructionBase {}; // invalid parameter
class eInstructionBadLen 			: public eInstructionBase {}; // bad length
class eInstructionInvalType 		: public eInstructionBase {}; // invalid instruction type
class eInstructionHeaderInval 		: public eInstructionBase {}; // invalid instruction header
class eInstructionActionNotFound 	: public eInstructionBase {}; // action not found in instruction
class eInstructionBadExperimenter 	: public eInstructionBase {}; // unknown experimenter instruction


class cofinst :
	public csyslog
{
	uint8_t 		ofp_version;

public: // static stuff, enums, constants

	#define COFINST_DEFAULT_LEN	128
	static std::set<cofinst*> cofinst_set;

public: // data structures

	pthread_mutex_t inmutex; // mutex for this cofinst instance
	cofactions actions;	// vector of cofaction instances

	union {
		// generic
		uint8_t												*oinu_generic;

		// OpenFlow 1.2
		struct openflow12::ofp_instruction  				*oinu12_header;
		struct openflow12::ofp_instruction_goto_table		*oinu12_goto_table;
		struct openflow12::ofp_instruction_actions			*oinu12_actions;
		struct openflow12::ofp_instruction_experimenter		*oinu12_experimenter;
		struct openflow12::ofp_instruction_write_metadata	*oinu12_write_metadata;

		// OpenFlow 1.3
		struct openflow13::ofp_instruction  				*oinu13_header;
		struct openflow13::ofp_instruction_goto_table		*oinu13_goto_table;
		struct openflow13::ofp_instruction_actions			*oinu13_actions;
		struct openflow13::ofp_instruction_experimenter		*oinu13_experimenter;
		struct openflow13::ofp_instruction_write_metadata	*oinu13_write_metadata;
		struct openflow13::ofp_instruction_meter			*oinu13_meter;

	} oin_oinu;

#define oin_generic			oin_oinu.oinu_generic			// generic pointer to cmemory::instruction.somem()
#define oin_header 			oin_oinu.oinu12_header			// instruction: plain header
#define oin_goto_table 		oin_oinu.oinu12_goto_table		// instruction: goto table
#define oin_actions 		oin_oinu.oinu12_actions			// instruction: actions
#define oin_experimenter 	oin_oinu.oinu12_experimenter	// instruction: experimenter
#define oin_write_metadata 	oin_oinu.oinu12_write_metadata	// instruction: write metadata
#define oin_meter 			oin_oinu.oinu13_meter			// instruction: meter



public: // methods

	/** constructor
	 */
	cofinst(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN,
			size_t size = COFINST_DEFAULT_LEN);

	/** constructor
	 */
	cofinst(
			uint8_t ofp_version,
			struct openflow::ofp_instruction *instruction,
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
	find_action(uint8_t type);

	/** copy struct ofp_action_header
	 */
	virtual uint8_t*
	pack(uint8_t* buf, size_t buflen);

	/** unpack
	 */
	virtual void
	unpack(uint8_t* buf, size_t buflen);


	/**
	 *
	 */
	uint8_t
	get_version() const
	{
		return ofp_version;
	};

	/**
	 *
	 */
	uint16_t
	get_type() const
	{
		return be16toh(oin_header->type);
	};

	/**
	 *
	 */
	void
	set_type(uint16_t version)
	{
		oin_header->type = htobe16(version);
	};

	/**
	 *
	 */
	uint16_t
	get_length() const
	{
		return be16toh(oin_header->len);
	};

	/**
	 *
	 */
	void
	set_length(uint16_t len)
	{
		oin_header->len = htobe16(len);
	};

private:

	/** copy struct ofp_action_header
	 */
	virtual uint8_t*
	pack_of12(uint8_t* buf, size_t buflen);

	/** unpack
	 */
	virtual void
	unpack_of12(uint8_t* buf, size_t buflen);


protected: // data structures

	cmemory instruction; // memory area with original packes instruction

protected:

	/**
	 *
	 */
	void
	resize(size_t size);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofinst const& inst) {
		os << "<cofinst ";
		switch (inst.get_version()) {
		case openflow12::OFP_VERSION: {
			switch (inst.get_type()) {
			case openflow12::OFPIT_APPLY_ACTIONS:	os << "OFIT-APPLY-ACTIONS" 	<< " "; break;
			case openflow12::OFPIT_WRITE_ACTIONS:	os << "OFIT-WRITE-ACTIONS" 	<< " "; break;
			case openflow12::OFPIT_CLEAR_ACTIONS:	os << "OFIT-CLEAR-ACTIONS" 	<< " ";	break;
			case openflow12::OFPIT_WRITE_METADATA:	os << "OFIT-WRITE-METADATA" << " ";	break;
			case openflow12::OFPIT_GOTO_TABLE:		os << "OFIT-GOTO-TABLE"	    << " "; break;
			default:								os << "OFIT-UNKNOWN"		<< " "; break;
			}
		} break;
		case openflow13::OFP_VERSION: {
			switch (inst.get_type()) {
			case openflow13::OFPIT_APPLY_ACTIONS:	os << "OFIT-APPLY-ACTIONS" 	<< " "; break;
			case openflow13::OFPIT_WRITE_ACTIONS:	os << "OFIT-WRITE-ACTIONS" 	<< " "; break;
			case openflow13::OFPIT_CLEAR_ACTIONS:	os << "OFIT-CLEAR-ACTIONS" 	<< " ";	break;
			case openflow13::OFPIT_WRITE_METADATA:	os << "OFIT-WRITE-METADATA" << " ";	break;
			case openflow13::OFPIT_GOTO_TABLE:		os << "OFIT-GOTO-TABLE"	    << " "; break;
			case openflow13::OFPIT_METER:			os << "OFIT-METER"	   		<< " "; break;
			default:								os << "OFIT-UNKNOWN"		<< " "; break;
			}
		} break;
		default: {
			// do nothing
		} break;
		}
		os << "actions:" << std::endl;
		os << inst.actions << " ";
		os << ">";
		return os;
	};
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
	cofinst_apply_actions(uint8_t ofp_version) : cofinst(ofp_version)
	{
		switch (get_version()) {
		case openflow12::OFP_VERSION: {
			set_type(openflow12::OFPIT_APPLY_ACTIONS);
			set_length(0); // fill this when calling method pack()
		} break;
		case openflow13::OFP_VERSION: {
			set_type(openflow13::OFPIT_APPLY_ACTIONS);
			set_length(0); // fill this when calling method pack()
		} break;
		default:
			logging::warn << "cofinst_apply_actions: OFP version not supported" << std::endl;
			throw eBadVersion();
		}
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
	cofinst_write_actions(uint8_t ofp_version) : cofinst(ofp_version)
	{
		switch (get_version()) {
		case openflow12::OFP_VERSION: {
			set_type(openflow12::OFPIT_WRITE_ACTIONS);
			set_length(0); // fill this when calling method pack()
		} break;
		case openflow13::OFP_VERSION: {
			set_type(openflow13::OFPIT_WRITE_ACTIONS);
			set_length(0); // fill this when calling method pack()
		} break;
		default:
			logging::warn << "cofinst_write_actions: OFP version not supported" << std::endl;
			throw eBadVersion();
		}
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
	cofinst_clear_actions(uint8_t ofp_version) : cofinst(ofp_version)
	{
		switch (get_version()) {
		case openflow12::OFP_VERSION: {
			set_type(openflow12::OFPIT_CLEAR_ACTIONS);
			set_length(0); // fill this when calling method pack()
		} break;
		case openflow13::OFP_VERSION: {
			set_type(openflow13::OFPIT_CLEAR_ACTIONS);
			set_length(0); // fill this when calling method pack()
		} break;
		default:
			logging::warn << "cofinst_clear_actions: OFP version not supported" << std::endl;
			throw eBadVersion();
		}
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
			uint8_t ofp_version,
			uint8_t table_id) :
				cofinst(ofp_version, sizeof(struct openflow::ofp_instruction))
	{
		switch (get_version()) {
		case openflow12::OFP_VERSION: {
			cofinst::resize(sizeof(struct openflow12::ofp_instruction_goto_table));
			set_type(openflow12::OFPIT_GOTO_TABLE);
			set_length(sizeof(struct openflow12::ofp_instruction_goto_table));
			set_table_id(table_id);
		} break;
		case openflow13::OFP_VERSION: {
			cofinst::resize(sizeof(struct openflow13::ofp_instruction_goto_table));
			set_type(openflow13::OFPIT_GOTO_TABLE);
			set_length(sizeof(struct openflow13::ofp_instruction_goto_table));
			set_table_id(table_id);
		} break;
		default:
			logging::warn << "cofinst_goto_table: OFP version not supported" << std::endl;
			throw eBadVersion();
		}
	};
	/** destructor
	 */
	virtual
	~cofinst_goto_table() {};
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
			uint8_t ofp_version,
			uint64_t metadata,
			uint64_t metadata_mask) :
				cofinst(ofp_version, sizeof(struct openflow::ofp_instruction))
	{
		switch (get_version()) {
		case openflow12::OFP_VERSION: {
			cofinst::resize(sizeof(struct openflow12::ofp_instruction_write_metadata));
			set_type(openflow12::OFPIT_WRITE_METADATA);
			set_length(sizeof(struct openflow12::ofp_instruction_write_metadata));
			set_metadata(metadata);
			set_metadata_mask(metadata_mask);
		} break;
		case openflow13::OFP_VERSION: {
			cofinst::resize(sizeof(struct openflow13::ofp_instruction_write_metadata));
			set_type(openflow13::OFPIT_WRITE_METADATA);
			set_length(sizeof(struct openflow13::ofp_instruction_write_metadata));
			set_metadata(metadata);
			set_metadata_mask(metadata_mask);
		} break;
		default:
			logging::warn << "cofinst_write_metadata: OFP version not supported" << std::endl;
			throw eBadVersion();
		}
	};
	/** destructor
	 */
	virtual
	~cofinst_write_metadata() {};


	/**
	 *
	 */
	uint64_t
	get_metadata() const
	{
		return oin_write_metadata->metadata;
	};
	/**
	 *
	 */
	void
	set_metadata(uint64_t metadata)
	{
		oin_write_metadata->metadata = htobe64(metadata);
	};
	/**
	 *
	 */
	uint64_t
	get_metadata_mask() const
	{
		return oin_write_metadata->metadata_mask;
	};
	/**
	 *
	 */
	void
	set_metadata_mask(uint64_t metadata_mask)
	{
		oin_write_metadata->metadata_mask = htobe64(metadata_mask);
	};
};

}; // end of namespace


#endif



