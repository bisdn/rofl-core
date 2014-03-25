/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFINSTRUCTIONLIST_H
#define COFINSTRUCTIONLIST_H 1

#ifdef __cplusplus
extern "C" {
#endif
#include <assert.h>
#ifdef __cplusplus
}
#endif

#include <map>
#include <algorithm>
#include "rofl/common/croflexception.h"
#include "rofl/common/cvastring.h"
#include "rofl/common/coflist.h"

#include "rofl/common/openflow/cofaction.h"
#include "rofl/common/openflow/cofinstruction.h"

namespace rofl {
namespace openflow {

class eInstructionsBase 		: public RoflException {}; // base error class cofinlist
class eInstructionsInval 		: public eInstructionsBase {}; // invalid parameter
class eInstructionsNotFound 	: public eInstructionsBase {}; // element not found
class eInstructionsOutOfRange	: public eInstructionsBase {}; // out of range



class cofinstructions
{
	std::map<uint16_t, cofinst*> instmap;

public: // iterators

	typedef typename std::map<uint16_t, cofinst*>::iterator iterator;
	typedef typename std::map<uint16_t, cofinst*>::const_iterator const_iterator;
	iterator begin() { return instmap.begin(); }
	iterator end() { return instmap.end(); }
	const_iterator begin() const { return instmap.begin(); }
	const_iterator end() const { return instmap.end(); }

	typedef typename std::map<uint16_t, cofinst*>::reverse_iterator reverse_iterator;
	typedef typename std::map<uint16_t, cofinst*>::const_reverse_iterator const_reverse_iterator;
	reverse_iterator rbegin() { return instmap.rbegin(); }
	reverse_iterator rend() { return instmap.rend(); }

public:

	uint8_t ofp_version;

	/**
	 */
	static void test();

public: // methods

	/** constructor
	 */
	cofinstructions(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN);

	/** destructor
	 */
	virtual
	~cofinstructions();

	/**
	 */
	cofinstructions(
			cofinstructions const& inlist);

	/**
	 */
	cofinstructions&
	operator= (
			cofinstructions const& inlist);

	/**
	 *
 	 */
	bool
	operator== (
			cofinstructions const& inlist);

	/**
	 *
	 */
	cofinst&
	operator[] (
			unsigned int index);

	/**
	 */
	void
	unpack(
			uint8_t *buf, size_t buflen);


	/** builds an array of struct ofp_instruction from this->invec
	 */
	uint8_t*			// returns parameter "struct ofp_instruction *instructions"
	pack(
			uint8_t *instructions, // pointer to memory area for storing this->invec
			size_t inlen); 					// length of memory area


	/** returns required length for array of struct ofp_instruction
	 * for all instructions defined in this->invec
	 */
	size_t
	length() const;


	/**
	 *
	 */
	void
	check_prerequisites() const;


	/**
	 *
	 */
	cofinst&
	add_inst(
			cofinst const& inst);


	/*
	 * Goto-Table
	 */
	cofinst_goto_table&
	add_inst_goto_table();

	cofinst_goto_table&
	set_inst_goto_table();

	cofinst_goto_table&
	get_inst_goto_table() const;

	void
	drop_inst_goto_table();

	bool
	has_inst_goto_table() const;


	/*
	 * Write-Metadata
	 */
	cofinst_write_metadata&
	add_inst_write_metadata();

	cofinst_write_metadata&
	set_inst_write_metadata();

	cofinst_write_metadata&
	get_inst_write_metadata() const;

	void
	drop_inst_write_metadata();

	bool
	has_inst_write_metadata() const;


	/*
	 * Write-Actions
	 */
	cofinst_write_actions&
	add_inst_write_actions();

	cofinst_write_actions&
	set_inst_write_actions();

	cofinst_write_actions&
	get_inst_write_actions() const;

	void
	drop_inst_write_actions();

	bool
	has_inst_write_actions() const;


	/*
	 * Apply-Actions
	 */
	cofinst_apply_actions&
	add_inst_apply_actions();

	cofinst_apply_actions&
	set_inst_apply_actions();

	cofinst_apply_actions&
	get_inst_apply_actions() const;

	void
	drop_inst_apply_actions();

	bool
	has_inst_apply_actions() const;


	/*
	 * Clear-Actions
	 */
	cofinst_clear_actions&
	add_inst_clear_actions();

	cofinst_clear_actions&
	set_inst_clear_actions();

	cofinst_clear_actions&
	get_inst_clear_actions() const;

	void
	drop_inst_clear_actions();

	bool
	has_inst_clear_actions() const;


	/*
	 * Experimenter
	 */
	cofinst_experimenter&
	add_inst_experimenter();

	cofinst_experimenter&
	set_inst_experimenter();

	cofinst_experimenter&
	get_inst_experimenter() const;

	void
	drop_inst_experimenter();

	bool
	has_inst_experimenter() const;


	/*
	 * Meter
	 */
	cofinst_meter&
	add_inst_meter();

	cofinst_meter&
	set_inst_meter();

	cofinst_meter&
	get_inst_meter() const;

	void
	drop_inst_meter();

	bool
	has_inst_meter() const;




	/** find a specific instruction
	 */
	cofinst&
	find_inst(
			uint8_t type);

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	unsigned int
	size() { return instmap.size(); };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofinstructions const& inlist) {
		os << indent(0) << "<cofintructions ofp-version:" << (int)inlist.ofp_version << " >" << std::endl;
		for (std::map<uint16_t, cofinst*>::const_iterator
				it = inlist.instmap.begin(); it != inlist.instmap.end(); ++it) {
			indent i(2);
			switch (it->second->get_type()) {
			case openflow::OFPIT_GOTO_TABLE:
				os << inlist.get_inst_goto_table(); break;
			case openflow::OFPIT_WRITE_METADATA:
				os << inlist.get_inst_write_metadata(); break;
			case openflow::OFPIT_WRITE_ACTIONS:
				os << inlist.get_inst_write_actions(); break;
			case openflow::OFPIT_APPLY_ACTIONS:
				os << inlist.get_inst_apply_actions(); break;
			case openflow::OFPIT_CLEAR_ACTIONS:
				os << inlist.get_inst_clear_actions(); break;
			case openflow::OFPIT_METER:
				os << inlist.get_inst_meter(); break;
			case openflow::OFPIT_EXPERIMENTER:
				os << inlist.get_inst_experimenter(); break;
			default:
				os << *(it->second); break;
			}
		}
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
