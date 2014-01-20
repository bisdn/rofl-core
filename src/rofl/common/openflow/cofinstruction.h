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

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/cvastring.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/openflow/cofactions.h"

#include "rofl/platform/unix/csyslog.h"

#include "rofl/common/openflow/cofaction.h"

namespace rofl
{

/* error classes */
class eInstructionBase 				: public RoflException {}; // error base class for class cofinstruction
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
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN,
			size_t size = COFINST_DEFAULT_LEN);

	/** constructor
	 */
	cofinst(
			uint8_t ofp_version,
			uint8_t* instruction,
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
	struct openflow::ofp_instruction*
	soinst();

	/** return length of action in bytes
	 */
	size_t
	length() const;

	/**
	 *
	 */
	cofactions&
	get_actions() { return actions; };


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

	cmemory instruction; // memory area with original packed instruction

protected:

	/**
	 *
	 */
	void
	resize(size_t size);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofinst const& inst) {
		os << indent(0) << "<cofinst ";
		os << "type:" << (int)inst.get_type() << " ";
		os << "length:" << (int)inst.get_length() << " ";
		os << ">" << std::endl;
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

	bool operator() (std::pair<uint16_t, cofinst*> const& p) {
		return (be16toh(p.second->oin_header->type) == type);
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
		switch (ofp_version) {
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
	/** constructor
	 */
	cofinst_apply_actions(
			cofinst const& inst) :
				cofinst(inst) {};
	/** constructor
	 */
	cofinst_apply_actions(
			uint8_t ofp_version,
			uint8_t *buf, size_t buflen) :
				cofinst(ofp_version, buflen) {
		unpack(buf, buflen);
	};
	/** destructor
	 */
	virtual
	~cofinst_apply_actions() {};
public:
	friend std::ostream&
	operator<< (std::ostream& os, cofinst_apply_actions const& inst) {
		os << dynamic_cast<cofinst const&>( inst );
		indent i1(2);
		os << indent() << "<cofinst_apply_actions >" << std::endl;
		indent i2(2);
		os << inst.actions;
		return os;
	};
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
		switch (ofp_version) {
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
	/** constructor
	 */
	cofinst_write_actions(
			cofinst const& inst) :
				cofinst(inst) {};
	/** constructor
	 */
	cofinst_write_actions(
			uint8_t ofp_version,
			uint8_t *buf, size_t buflen) :
				cofinst(ofp_version, buflen) {
		unpack(buf, buflen);
	};
	/** destructor
	 */
	virtual
	~cofinst_write_actions() {};
public:
	friend std::ostream&
	operator<< (std::ostream& os, cofinst_write_actions const& inst) {
		os << dynamic_cast<cofinst const&>( inst );
		indent i1(2);
		os << indent() << "<cofinst_write_actions >" << std::endl;
		indent i2(2);
		os << inst.actions;
		return os;
	};
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
		switch (ofp_version) {
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
	/** constructor
	 */
	cofinst_clear_actions(
			cofinst const& inst) :
				cofinst(inst) {};
	/** constructor
	 */
	cofinst_clear_actions(
			uint8_t ofp_version,
			uint8_t *buf, size_t buflen) :
				cofinst(ofp_version, buflen) {
		unpack(buf, buflen);
	};
	/** destructor
	 */
	virtual
	~cofinst_clear_actions() {};
public:
	friend std::ostream&
	operator<< (std::ostream& os, cofinst_clear_actions const& inst) {
		os << dynamic_cast<cofinst const&>( inst );
		os << indent(2) << "<cofinst_clear_actions >" << std::endl;
		return os;
	};
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
			uint8_t table_id = 0) :
				cofinst(ofp_version, sizeof(struct openflow::ofp_instruction))
	{
		switch (ofp_version) {
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
	/** constructor
	 */
	cofinst_goto_table(
			cofinst const& inst) :
				cofinst(inst) {};
	/** constructor
	 */
	cofinst_goto_table(
			uint8_t ofp_version,
			uint8_t *buf, size_t buflen) :
				cofinst(ofp_version, buflen) {
		unpack(buf, buflen);
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
public:
	friend std::ostream&
	operator<< (std::ostream& os, cofinst_goto_table const& inst) {
		os << dynamic_cast<cofinst const&>( inst );
		os << indent(2) << "<cofinst_goto_table >" << std::endl;
		os << indent(4) << "<table-id:" << (int)inst.get_table_id() << " >" << std::endl;
		return os;
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
			uint64_t metadata = 0,
			uint64_t metadata_mask = 0) :
				cofinst(ofp_version, sizeof(struct openflow::ofp_instruction))
	{
		switch (ofp_version) {
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
	/** constructor
	 */
	cofinst_write_metadata(
			cofinst const& inst) :
				cofinst(inst) {};
	/** constructor
	 */
	cofinst_write_metadata(
			uint8_t ofp_version,
			uint8_t *buf, size_t buflen) :
				cofinst(ofp_version, buflen) {
		unpack(buf, buflen);
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
public:
	friend std::ostream&
	operator<< (std::ostream& os, cofinst_write_metadata const& inst) {
		os << dynamic_cast<cofinst const&>( inst );
		os << indent(2) << "<cofinst_write_metadata >" << std::endl;
		os << indent(4) << "<metadata:" << (unsigned long long)inst.get_metadata() << " >" << std::endl;
		os << indent(4) << "<metadata-mask:" << (unsigned long long)inst.get_metadata_mask() << " >" << std::endl;
		return os;
	};
};

class cofinst_experimenter : public cofinst {
public:
	/** constructor
	 */
	cofinst_experimenter(
			uint8_t ofp_version) :
				cofinst(ofp_version, sizeof(struct openflow::ofp_instruction))
	{

	};
	/** constructor
	 */
	cofinst_experimenter(
			cofinst const& inst) :
				cofinst(inst) {};
	/** constructor
	 */
	cofinst_experimenter(
			uint8_t ofp_version,
			uint8_t *buf, size_t buflen) :
				cofinst(ofp_version, buflen) {
		unpack(buf, buflen);
	};
public:
	friend std::ostream&
	operator<< (std::ostream& os, cofinst_experimenter const& inst) {
		os << dynamic_cast<cofinst const&>( inst );
		os << indent(2) << "<cofinst_experimenter >" << std::endl;
		return os;
	};
};

class cofinst_meter : public cofinst {
public:
	/** constructor
	 */
	cofinst_meter(
			uint8_t ofp_version) :
				cofinst(ofp_version, sizeof(struct openflow::ofp_instruction))
	{

	};
	/** constructor
	 */
	cofinst_meter(
			cofinst const& inst) :
				cofinst(inst) {};
	/** constructor
	 */
	cofinst_meter(
			uint8_t ofp_version,
			uint8_t *buf, size_t buflen) :
				cofinst(ofp_version, buflen) {
		unpack(buf, buflen);
	};
public:
	friend std::ostream&
	operator<< (std::ostream& os, cofinst_meter const& inst) {
		os << dynamic_cast<cofinst const&>( inst );
		os << indent(2) << "<cofinst_meter >" << std::endl;
		return os;
	};
};

}; // end of namespace


#endif



