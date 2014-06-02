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
#include "rofl/common/thread_helper.h"
#include "rofl/common/openflow/cofactions.h"


namespace rofl {
namespace openflow {

/* error classes */
class eInstructionBase 				: public RoflException {}; // error base class for class cofinstruction
class eInstructionInval 			: public eInstructionBase {}; // invalid parameter
class eInstructionBadLen 			: public eInstructionBase {}; // bad length
class eInstructionInvalType 		: public eInstructionBase {}; // invalid instruction type
class eInstructionHeaderInval 		: public eInstructionBase {}; // invalid instruction header
class eInstructionActionNotFound 	: public eInstructionBase {}; // action not found in instruction
class eInstructionBadExperimenter 	: public eInstructionBase {}; // unknown experimenter instruction


class cofinstruction {
public:

	/**
	 *
	 */
	cofinstruction(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN,
			uint16_t type = 0,
			const rofl::cmemory& body = rofl::cmemory((size_t)0));

	/**
	 *
	 */
	cofinstruction(
			cofinstruction const& inst);

	/**
	 *
	 */
	virtual
	~cofinstruction();

	/**
	 *
	 */
	cofinstruction&
	operator= (
			const cofinstruction& inst);

	/**
	 *
	 */
	bool
	operator== (
			const cofinstruction& inst);


public:

	/**
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	check_prerequisites() const {};

public:

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_type(uint16_t type) { this->type = type; };

	/**
	 *
	 */
	uint16_t
	get_type() const { return type; };


	/**
	 *
	 */
	void
	set_length(uint16_t len) { this->len = len; };

	/**
	 *
	 */
	uint16_t
	get_length() const { return len; };

	/**
	 *
	 */
	rofl::cmemory&
	set_body() { return body; };

	/**
	 *
	 */
	const rofl::cmemory&
	get_body() const { return body; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofinstruction const& inst) {
		os << rofl::indent(0) << "<cofinstruction ";
		os << "type: 0x" << std::hex << (int)inst.get_type() << std::dec << " ";
		os << "length: " << (int)inst.length() << " ";
		os << ">" << std::endl;
		if (not inst.get_body().empty()) {
			rofl::indent i(2); os << inst.get_body();
		}
		return os;
	};

	class cofinst_find_type {
	public:
		cofinst_find_type(uint16_t type) :
			type(type) { };

		bool operator() (cofinstruction const& inst) {
			return (inst.get_type() == type);
		};

		bool operator() (std::pair<uint16_t, cofinstruction*> const& p) {
			return (p.second->get_type() == type);
		};

		uint16_t type;
	};


private:

	uint8_t 			ofp_version;
	uint16_t			type;
	mutable uint16_t	len;
	rofl::cmemory		body;

};



class cofinstruction_actions : public cofinstruction {
public:

	/**
	 *
	 */
	cofinstruction_actions(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t type = 0,
			const rofl::openflow::cofactions& actions = rofl::openflow::cofactions()) :
				cofinstruction(ofp_version, type),
				actions(actions) {
		this->actions.set_version(ofp_version);
	};

	/**
	 *
	 */
	virtual
	~cofinstruction_actions() {};

	/**
	 *
	 */
	cofinstruction_actions(
			const cofinstruction_actions& inst) {
		*this = inst;
	};

	/**
	 *
	 */
	cofinstruction_actions&
	operator= (
			const cofinstruction_actions& inst) {
		if (this == &inst)
			return *this;

		cofinstruction::operator= (inst);
		actions		= inst.actions;

		return *this;
	};

public:

	/**
	 *
	 */
	rofl::openflow::cofactions&
	set_actions() { return actions; };

	/**
	 *
	 */
	const rofl::openflow::cofactions&
	get_actions() const { return actions; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	check_prerequisites() const {
		actions.check_prerequisites();
	};

public:
	friend std::ostream&
	operator<< (std::ostream& os, cofinstruction_actions const& inst) {
		os << rofl::indent(0) << "<cofinstruction_actions >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<cofinstruction const&>( inst ); }
		{ rofl::indent i(4); os << inst.actions; }
		return os;
	};

private:

	rofl::openflow::cofactions		actions;
};



class cofinstruction_apply_actions : public cofinstruction_actions {
public:

	/**
	 *
	 */
	cofinstruction_apply_actions(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			const rofl::openflow::cofactions& actions = rofl::openflow::cofactions()) :
				cofinstruction_actions(ofp_version, rofl::openflow13::OFPIT_APPLY_ACTIONS, actions) {};

	/**
	 *
	 */
	virtual
	~cofinstruction_apply_actions() {};

	/**
	 *
	 */
	cofinstruction_apply_actions(
			const cofinstruction_apply_actions& inst) {
		*this = inst;
	};

	/**
	 *
	 */
	cofinstruction_apply_actions&
	operator= (
			const cofinstruction_apply_actions& inst) {
		if (this == &inst)
			return *this;
		cofinstruction_actions::operator= (inst);
		return *this;
	}

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofinstruction_apply_actions& inst) {
		os << rofl::indent(0) << "<cofinstruction_apply_actions >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofinstruction_actions&>( inst ); }
		return os;
	};
};



class cofinstruction_write_actions : public cofinstruction_actions {
public:

	/**
	 *
	 */
	cofinstruction_write_actions(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			const rofl::openflow::cofactions& actions = rofl::openflow::cofactions()) :
				cofinstruction_actions(ofp_version, rofl::openflow13::OFPIT_WRITE_ACTIONS, actions) {};

	/**
	 *
	 */
	virtual
	~cofinstruction_write_actions() {};

	/**
	 *
	 */
	cofinstruction_write_actions(
			const cofinstruction_write_actions& inst) {
		*this = inst;
	};

	/**
	 *
	 */
	cofinstruction_write_actions&
	operator= (
			const cofinstruction_write_actions& inst) {
		if (this == &inst)
			return *this;
		cofinstruction_actions::operator= (inst);
		return *this;
	}

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofinstruction_write_actions& inst) {
		os << rofl::indent(0) << "<cofinstruction_write_actions >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofinstruction_actions&>( inst ); }
		return os;
	};
};



class cofinstruction_clear_actions : public cofinstruction_actions {
public:

	/**
	 *
	 */
	cofinstruction_clear_actions(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			const rofl::openflow::cofactions& actions = rofl::openflow::cofactions()) :
				cofinstruction_actions(ofp_version, rofl::openflow13::OFPIT_CLEAR_ACTIONS, actions) {};

	/**
	 *
	 */
	virtual
	~cofinstruction_clear_actions() {};

	/**
	 *
	 */
	cofinstruction_clear_actions(
			const cofinstruction_clear_actions& inst) {
		*this = inst;
	};

	/**
	 *
	 */
	cofinstruction_clear_actions&
	operator= (
			const cofinstruction_clear_actions& inst) {
		if (this == &inst)
			return *this;
		cofinstruction_actions::operator= (inst);
		return *this;
	}

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofinstruction_clear_actions& inst) {
		os << rofl::indent(0) << "<cofinstruction_clear_actions >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofinstruction_actions&>( inst ); }
		return os;
	};
};



class cofinstruction_goto_table : public cofinstruction {
public:

	/**
	 *
	 */
	cofinstruction_goto_table(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint8_t table_id = 0) :
				cofinstruction(ofp_version, rofl::openflow13::OFPIT_GOTO_TABLE),
				table_id(table_id) {};

	/**
	 *
	 */
	virtual
	~cofinstruction_goto_table() {};

	/**
	 *
	 */
	cofinstruction_goto_table(
			const cofinstruction_goto_table& inst) {
		*this = inst;
	};

	/**
	 *
	 */
	cofinstruction_goto_table&
	operator= (
			const cofinstruction_goto_table& inst) {
		if (this == &inst)
			return *this;

		cofinstruction::operator= (inst);
		table_id	= inst.table_id;

		return *this;
	};

public:

	/**
	 *
	 */
	uint8_t
	get_table_id() const { return table_id; };

	/**
	 *
	 */
	void
	set_table_id(uint8_t table_id) { this->table_id = table_id; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	check_prerequisites() const;

public:
	friend std::ostream&
	operator<< (std::ostream& os, cofinstruction_goto_table const& inst) {
		os << rofl::indent(0) << "<cofinstruction_goto_table >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofinstruction&>( inst ); }
		os << rofl::indent(4) << "<table-id:" << (int)inst.get_table_id() << " >" << std::endl;
		return os;
	};

public:

	uint8_t		table_id;
};



class cofinstruction_write_metadata : public cofinstruction {
public:
	/**
	 *
	 */
	cofinstruction_write_metadata(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint64_t metadata = 0,
			uint64_t metadata_mask = 0) :
				cofinstruction(ofp_version, rofl::openflow13::OFPIT_WRITE_METADATA),
				metadata(metadata),
				metadata_mask(metadata_mask) {};

	/**
	 *
	 */
	virtual
	~cofinstruction_write_metadata() {};

	/**
	 *
	 */
	cofinstruction_write_metadata(
			const cofinstruction_write_metadata& inst) {
		*this = inst;
	};

	/**
	 *
	 */
	cofinstruction_write_metadata&
	operator= (
			const cofinstruction_write_metadata& inst) {
		if (this == &inst)
			return *this;

		cofinstruction::operator= (inst);
		metadata 		= inst.metadata;
		metadata_mask	= inst.metadata_mask;

		return *this;
	};

public:

	/**
	 *
	 */
	uint64_t
	get_metadata() const { return metadata; };

	/**
	 *
	 */
	void
	set_metadata(uint64_t metadata) { this->metadata = metadata; };

	/**
	 *
	 */
	uint64_t
	get_metadata_mask() const { return metadata_mask; };

	/**
	 *
	 */
	void
	set_metadata_mask(uint64_t metadata_mask) { this->metadata_mask = metadata_mask; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofinstruction_write_metadata const& inst) {
		os << rofl::indent(0) << "<cofinstruction_write_metadata >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofinstruction&>( inst ); }
		os << std::hex;
		os << indent(4) << "<metadata: 0x" << (unsigned long long)inst.get_metadata() << " >" << std::endl;
		os << indent(4) << "<metadata-mask: 0x" << (unsigned long long)inst.get_metadata_mask() << " >" << std::endl;
		os << std::dec;
		return os;
	};

private:

	uint64_t metadata;
	uint64_t metadata_mask;
};






class cofinstruction_experimenter : public cofinstruction {
public:

	/**
	 *
	 */
	cofinstruction_experimenter(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t exp_id = 0,
			const rofl::cmemory& exp_body = rofl::cmemory((size_t)0)) :
				cofinstruction(ofp_version, rofl::openflow13::OFPIT_EXPERIMENTER),
				exp_id(exp_id),
				exp_body(exp_body) {};

	/**
	 *
	 */
	virtual
	~cofinstruction_experimenter() {};

	/**
	 *
	 */
	cofinstruction_experimenter(
			const cofinstruction_experimenter& inst) {
		*this = inst;
	};

	/**
	 *
	 */
	cofinstruction_experimenter&
	operator= (
			const cofinstruction_experimenter& inst) {
		if (this == &inst)
			return *this;

		cofinstruction::operator= (inst);
		exp_id		= inst.exp_id;
		exp_body	= inst.exp_body;

		return *this;
	};

public:

	/**
	 *
	 */
	uint32_t
	get_exp_id() const { return exp_id; };

	/**
	 *
	 */
	void
	set_exp_id(uint32_t exp_id) { this->exp_id = exp_id; };

	/**
	 *
	 */
	const rofl::cmemory&
	get_exp_body() const { return exp_body; };

	/**
	 *
	 */
	rofl::cmemory&
	set_exp_body() { return exp_body; };


	/**
	 * @brief	Shadows cofinst::get_body() intentionally
	 */
	const rofl::cmemory&
	get_body() const { return exp_body; };

	/**
	 * @brief	Shadows cofinst::set_body() intentionally
	 */
	rofl::cmemory&
	set_body() { return exp_body; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofinstruction_experimenter const& inst) {
		os << rofl::indent(0) << "<cofinstruction_experimenter >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofinstruction&>( inst ); }
		os << rofl::indent(4) << "<exp-id: 0x" << std::hex << inst.get_exp_id() << std::dec << " >" << std::endl;
		{ rofl::indent i(4); os << inst.get_exp_body(); }
		return os;
	};

private:

	uint32_t		exp_id;
	rofl::cmemory	exp_body;
};




class cofinstruction_meter : public cofinstruction
{
public:

	/**
	 *
	 */
	cofinstruction_meter(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t meter_id = 0) :
				cofinstruction(ofp_version, rofl::openflow13::OFPIT_METER),
				meter_id(meter_id) {};

	/**
	 *
	 */
	virtual
	~cofinstruction_meter() {};

	/**
	 *
	 */
	cofinstruction_meter(
			 const cofinstruction_meter& inst) {
		*this = inst;
	};

	/**
	 *
	 */
	cofinstruction_meter&
	operator= (
			const cofinstruction_meter& inst) {
		if (this == &inst)
			return *this;

		cofinstruction::operator= (inst);
		meter_id	= inst.meter_id;

		return *this;
	};

public:

	/**
	 *
	 */
	void
	set_meter_id(uint32_t meter_id) { this->meter_id = meter_id; };

	/**
	 *
	 */
	uint32_t
	get_meter_id() const { return meter_id; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofinstruction_meter const& inst) {
		os << "<cofinstruction_meter >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<cofinstruction const&>( inst ); }
		os << rofl::indent(4) << "<meter-id: 0x" << std::hex << inst.get_meter_id() << " >" << std::endl;
		return os;
	};

private:

	uint32_t	meter_id;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif



