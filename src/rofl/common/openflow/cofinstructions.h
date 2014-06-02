/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFINSTRUCTIONS_H
#define COFINSTRUCTIONS_H 1

#include <assert.h>
#include <map>
#include <algorithm>
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/cofinstruction.h"

namespace rofl {
namespace openflow {

class eInstructionsBase 		: public RoflException {};
class eInstructionsInval 		: public eInstructionsBase {};
class eInstructionsNotFound 	: public eInstructionsBase {};
class eInstructionsOutOfRange	: public eInstructionsBase {};



class cofinstructions
{
public:

	/**
	 *
	 */
	cofinstructions(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofinstructions();

	/**
	 *
	 */
	cofinstructions(
			cofinstructions const& instructions);

	/**
	 *
	 */
	cofinstructions&
	operator= (
			cofinstructions const& instructions);

	/**
	 *
 	 */
	bool
	operator== (
			cofinstructions const& instructions);


public:

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) {
		this->ofp_version = ofp_version;
		for (std::map<uint16_t, cofinstruction*>::iterator
				it = instmap.begin(); it != instmap.end(); ++it) {
			it->second->set_version(ofp_version);
		}
	};

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	std::map<uint16_t, cofinstruction*>&
	set_instructions() { return instmap; };

	/**
	 *
	 */
	const std::map<uint16_t, cofinstruction*>&
	get_instructions() const { return instmap; };

public:

	/**
	 *
	 */
	size_t
	length() const;

	/**
	 *
	 */
	void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(
			uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	void
	check_prerequisites() const;

public:

	/*
	 * Generic
	 */
	cofinstruction&
	add_inst(uint16_t type);

	cofinstruction&
	set_inst(uint16_t type);

	const cofinstruction&
	get_inst(uint16_t type) const;

	void
	drop_inst(uint16_t type);

	bool
	has_inst(uint16_t type) const;


	/*
	 * Goto-Table
	 */
	cofinstruction_goto_table&
	add_inst_goto_table();

	cofinstruction_goto_table&
	set_inst_goto_table();

	const cofinstruction_goto_table&
	get_inst_goto_table() const;

	void
	drop_inst_goto_table();

	bool
	has_inst_goto_table() const;


	/*
	 * Write-Metadata
	 */
	cofinstruction_write_metadata&
	add_inst_write_metadata();

	cofinstruction_write_metadata&
	set_inst_write_metadata();

	const cofinstruction_write_metadata&
	get_inst_write_metadata() const;

	void
	drop_inst_write_metadata();

	bool
	has_inst_write_metadata() const;


	/*
	 * Write-Actions
	 */
	cofinstruction_write_actions&
	add_inst_write_actions();

	cofinstruction_write_actions&
	set_inst_write_actions();

	const cofinstruction_write_actions&
	get_inst_write_actions() const;

	void
	drop_inst_write_actions();

	bool
	has_inst_write_actions() const;


	/*
	 * Apply-Actions
	 */
	cofinstruction_apply_actions&
	add_inst_apply_actions();

	cofinstruction_apply_actions&
	set_inst_apply_actions();

	const cofinstruction_apply_actions&
	get_inst_apply_actions() const;

	void
	drop_inst_apply_actions();

	bool
	has_inst_apply_actions() const;


	/*
	 * Clear-Actions
	 */
	cofinstruction_clear_actions&
	add_inst_clear_actions();

	cofinstruction_clear_actions&
	set_inst_clear_actions();

	const cofinstruction_clear_actions&
	get_inst_clear_actions() const;

	void
	drop_inst_clear_actions();

	bool
	has_inst_clear_actions() const;


	/*
	 * Meter
	 */
	cofinstruction_meter&
	add_inst_meter();

	cofinstruction_meter&
	set_inst_meter();

	const cofinstruction_meter&
	get_inst_meter() const;

	void
	drop_inst_meter();

	bool
	has_inst_meter() const;


	/*
	 * Experimenter
	 */
	cofinstruction_experimenter&
	add_inst_experimenter();

	cofinstruction_experimenter&
	set_inst_experimenter();

	const cofinstruction_experimenter&
	get_inst_experimenter() const;

	void
	drop_inst_experimenter();

	bool
	has_inst_experimenter() const;


	/**
	 *
	 */
	void
	clear();



public:

	friend std::ostream&
	operator<< (std::ostream& os, cofinstructions const& inlist) {
		os << indent(0) << "<cofintructions ofp-version:" << (int)inlist.ofp_version << " >" << std::endl;
		for (std::map<uint16_t, cofinstruction*>::const_iterator
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

private:

	uint8_t 						ofp_version;
	std::map<uint16_t, cofinstruction*> 	instmap;

};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
