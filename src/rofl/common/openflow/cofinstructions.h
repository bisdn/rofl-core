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



class cofinstructions : public coflist<cofinst>
{
	std::map<int, cofinst*> instructions;

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

	/** stores cofinst instances in this->invec from a packed array struct ofp_instruction (e.g. in struct ofp_flow_mod)
	 */
	std::vector<cofinst>&			// returns reference to this->invec
	unpack(
			uint8_t *instructions, // parses memory area buckets and creates cofinst instance in this->invec
			size_t inlen);					// length of memory area to be parsed


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
	cofinst_goto_table&
	get_inst_goto_table();


	/**
	 *
	 */
	cofinst_write_metadata&
	get_inst_write_metadata();


	/**
	 *
	 */
	cofinst_write_actions&
	get_inst_write_actions();


	/**
	 *
	 */
	cofinst_apply_actions&
	get_inst_apply_actions();


	/**
	 *
	 */
	cofinst_clear_actions&
	get_inst_clear_actions();


	/**
	 *
	 */
	cofinst_meter&
	get_inst_meter();


	/** find a specific instruction
	 */
	cofinst&
	find_inst(
			uint8_t type)
	throw (eInstructionsNotFound);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofinstructions const& inlist) {
		os << "<cofinlist ";
			os << "ofp-version:" << (int)inlist.ofp_version << " ";
			os << std::endl;
			for (cofinstructions::const_iterator
					it = inlist.elems.begin(); it != inlist.elems.end(); ++it) {
				os << (*it) << std::endl;
			}
		os << ">";
		return os;
	};
};

}; // end of namespace

#endif
