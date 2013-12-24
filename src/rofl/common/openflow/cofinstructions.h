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
#include "../cerror.h"
#include "../cvastring.h"
#include "../coflist.h"

#include "cofaction.h"
#include "cofinst.h"

namespace rofl
{

class eInstructionsBase 		: public cerror {}; // base error class cofinlist
class eInstructionsInval 		: public eInstructionsBase {}; // invalid parameter
class eInstructionsNotFound 	: public eInstructionsBase {}; // element not found
class eInstructionsOutOfRange	: public eInstructionsBase {}; // out of range



class cofinstructions
{
	std::map<enum openflow::ofp_instruction_type, cofinst*> instmap;

public:

	uint8_t ofp_version;

	/**
	 */
	static void test();

public: // methods

	/** constructor
	 */
	cofinstructions(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN);

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


	/*
	 * Goto-Table
	 */
	cofinst_goto_table&
	add_inst_goto_table();

	cofinst_goto_table&
	get_inst_goto_table() const;

	void
	drop_inst_goto_table();


	/*
	 * Write-Metadata
	 */
	cofinst_write_metadata&
	add_inst_write_metadata();

	cofinst_write_metadata&
	get_inst_write_metadata() const;

	void
	drop_inst_write_metadata();


	/*
	 * Write-Actions
	 */
	cofinst_write_actions&
	add_inst_write_actions();

	cofinst_write_actions&
	get_inst_write_actions() const;

	void
	drop_inst_write_actions();


	/*
	 * Apply-Actions
	 */
	cofinst_apply_actions&
	add_inst_apply_actions();

	cofinst_apply_actions&
	get_inst_apply_actions() const;

	void
	drop_inst_apply_actions();


	/*
	 * Clear-Actions
	 */
	cofinst_clear_actions&
	add_inst_clear_actions();

	cofinst_clear_actions&
	get_inst_clear_actions() const;

	void
	drop_inst_clear_actions();


	/*
	 * Meter
	 */
	cofinst_meter&
	add_inst_meter();

	cofinst_meter&
	get_inst_meter() const;

	void
	drop_inst_meter();




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

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofinstructions const& inlist) {
		os << indent(0) << "<cofintructions >" << std::endl;
		for (std::map<enum openflow::ofp_instruction_type, cofinst*>::const_iterator
				it = inlist.instmap.begin(); it != inlist.instmap.end(); ++it) {
			os << indent(2) << *(it->second);
		}
		return os;
	};
};

}; // end of namespace

#endif
