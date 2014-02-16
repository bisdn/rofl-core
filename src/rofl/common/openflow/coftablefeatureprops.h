/*
 * coftablefeatureprops.h
 *
 *  Created on: 04.02.2014
 *      Author: andi
 */

#ifndef COFTABLEFEATUREPROPS_H_
#define COFTABLEFEATUREPROPS_H_

#include <assert.h>
#include <inttypes.h>

#include <map>
#include <ostream>
#include <algorithm>

#include "rofl/common/openflow/coftablefeatureprop.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/logging.h"

namespace rofl {
namespace openflow {

class eOFTableFeaturePropsBase		: public RoflException {};
class eOFTableFeaturePropsInval		: public eOFTableFeaturePropsBase {};
class eOFTableFeaturePropsNotFound	: public eOFTableFeaturePropsBase {};

class coftable_feature_props {

	uint8_t ofp_version;
	std::map<uint16_t, coftable_feature_prop*> tfprops;

public:

	/**
	 *
	 */
	coftable_feature_props(uint8_t ofp_version = OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~coftable_feature_props();

	/**
	 *
	 */
	coftable_feature_props(
			coftable_feature_props const& tfps);

	/**
	 *
	 */
	coftable_feature_props&
	operator= (
			coftable_feature_props const& tfps);

	/**
	 *
	 */
	void
	clear();

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
	pack(uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

	/*
	 * OFPTFPF_INSTRUCTIONS
	 */
	coftable_feature_prop_instructions&
	add_tfp_instructions() {
		return dynamic_cast<coftable_feature_prop_instructions&>( add_tfp(rofl::openflow13::OFPTFPT_INSTRUCTIONS) );
	};

	coftable_feature_prop_instructions&
	set_tfp_instructions() {
		return dynamic_cast<coftable_feature_prop_instructions&>( set_tfp(rofl::openflow13::OFPTFPT_INSTRUCTIONS) );
	};

	coftable_feature_prop_instructions&
	get_tfp_instructions() {
		return dynamic_cast<coftable_feature_prop_instructions&>( get_tfp(rofl::openflow13::OFPTFPT_INSTRUCTIONS) );
	};

	void
	drop_tfp_instructions() {
		drop_tfp(rofl::openflow13::OFPTFPT_INSTRUCTIONS);
	};

	bool
	has_tfp_instructions() {
		return has_tfp(rofl::openflow13::OFPTFPT_INSTRUCTIONS);
	};

	/*
	 * OFPTFPF_INSTRUCTIONS_MISS
	 */
	coftable_feature_prop_instructions&
	add_tfp_instructions_miss() {
		return dynamic_cast<coftable_feature_prop_instructions&>( add_tfp(rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS) );
	};

	coftable_feature_prop_instructions&
	set_tfp_instructions_miss() {
		return dynamic_cast<coftable_feature_prop_instructions&>( set_tfp(rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS) );
	};

	coftable_feature_prop_instructions&
	get_tfp_instructions_miss() {
		return dynamic_cast<coftable_feature_prop_instructions&>( get_tfp(rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS) );
	};

	void
	drop_tfp_instructions_miss() {
		drop_tfp(rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS);
	};

	bool
	has_tfp_instructions_miss() {
		return has_tfp(rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS);
	};



	/*
	 * OFPTFPF_NEXT_TABLES
	 */
	coftable_feature_prop_next_tables&
	add_tfp_next_tables() {
		return dynamic_cast<coftable_feature_prop_next_tables&>( add_tfp(rofl::openflow13::OFPTFPT_NEXT_TABLES) );
	};

	coftable_feature_prop_next_tables&
	set_tfp_next_tables() {
		return dynamic_cast<coftable_feature_prop_next_tables&>( set_tfp(rofl::openflow13::OFPTFPT_NEXT_TABLES) );
	};

	coftable_feature_prop_next_tables&
	get_tfp_next_tables() {
		return dynamic_cast<coftable_feature_prop_next_tables&>( get_tfp(rofl::openflow13::OFPTFPT_NEXT_TABLES) );
	};

	void
	drop_tfp_next_tables() {
		drop_tfp(rofl::openflow13::OFPTFPT_NEXT_TABLES);
	};

	bool
	has_tfp_next_tables() {
		return has_tfp(rofl::openflow13::OFPTFPT_NEXT_TABLES);
	};

	/*
	 * OFPTFPF_NEXT_TABLES_MISS
	 */
	coftable_feature_prop_next_tables&
	add_tfp_next_tables_miss() {
		return dynamic_cast<coftable_feature_prop_next_tables&>( add_tfp(rofl::openflow13::OFPTFPT_NEXT_TABLES_MISS) );
	};

	coftable_feature_prop_next_tables&
	set_tfp_next_tables_miss() {
		return dynamic_cast<coftable_feature_prop_next_tables&>( set_tfp(rofl::openflow13::OFPTFPT_NEXT_TABLES_MISS) );
	};

	coftable_feature_prop_next_tables&
	get_tfp_next_tables_miss() {
		return dynamic_cast<coftable_feature_prop_next_tables&>( get_tfp(rofl::openflow13::OFPTFPT_NEXT_TABLES_MISS) );
	};

	void
	drop_tfp_next_tables_miss() {
		drop_tfp(rofl::openflow13::OFPTFPT_NEXT_TABLES_MISS);
	};

	bool
	has_tfp_next_tables_miss() {
		return has_tfp(rofl::openflow13::OFPTFPT_NEXT_TABLES_MISS);
	};




	/*
	 * OFPTFPF_WRITE_ACTIONS
	 */
	coftable_feature_prop_actions&
	add_tfp_write_actions() {
		return dynamic_cast<coftable_feature_prop_actions&>( add_tfp(rofl::openflow13::OFPTFPT_WRITE_ACTIONS) );
	};

	coftable_feature_prop_actions&
	set_tfp_write_actions() {
		return dynamic_cast<coftable_feature_prop_actions&>( set_tfp(rofl::openflow13::OFPTFPT_WRITE_ACTIONS) );
	};

	coftable_feature_prop_actions&
	get_tfp_write_actions() {
		return dynamic_cast<coftable_feature_prop_actions&>( get_tfp(rofl::openflow13::OFPTFPT_WRITE_ACTIONS) );
	};

	void
	drop_tfp_write_actions() {
		drop_tfp(rofl::openflow13::OFPTFPT_WRITE_ACTIONS);
	};

	bool
	has_tfp_write_actions() {
		return has_tfp(rofl::openflow13::OFPTFPT_WRITE_ACTIONS);
	};

	/*
	 * OFPTFPF_WRITE_ACTIONS_MISS
	 */
	coftable_feature_prop_actions&
	add_tfp_write_actions_miss() {
		return dynamic_cast<coftable_feature_prop_actions&>( add_tfp(rofl::openflow13::OFPTFPT_WRITE_ACTIONS_MISS) );
	};

	coftable_feature_prop_actions&
	set_tfp_write_actions_miss() {
		return dynamic_cast<coftable_feature_prop_actions&>( set_tfp(rofl::openflow13::OFPTFPT_WRITE_ACTIONS_MISS) );
	};

	coftable_feature_prop_actions&
	get_tfp_write_actions_miss() {
		return dynamic_cast<coftable_feature_prop_actions&>( get_tfp(rofl::openflow13::OFPTFPT_WRITE_ACTIONS_MISS) );
	};

	void
	drop_tfp_write_actions_miss() {
		drop_tfp(rofl::openflow13::OFPTFPT_WRITE_ACTIONS_MISS);
	};

	bool
	has_tfp_write_actions_miss() {
		return has_tfp(rofl::openflow13::OFPTFPT_WRITE_ACTIONS_MISS);
	};





	/*
	 * OFPTFPF_APPLY_ACTIONS
	 */
	coftable_feature_prop_actions&
	add_tfp_apply_actions() {
		return dynamic_cast<coftable_feature_prop_actions&>( add_tfp(rofl::openflow13::OFPTFPT_APPLY_ACTIONS) );
	};

	coftable_feature_prop_actions&
	set_tfp_apply_actions() {
		return dynamic_cast<coftable_feature_prop_actions&>( set_tfp(rofl::openflow13::OFPTFPT_APPLY_ACTIONS) );
	};

	coftable_feature_prop_actions&
	get_tfp_apply_actions() {
		return dynamic_cast<coftable_feature_prop_actions&>( get_tfp(rofl::openflow13::OFPTFPT_APPLY_ACTIONS) );
	};

	void
	drop_tfp_apply_actions() {
		drop_tfp(rofl::openflow13::OFPTFPT_APPLY_ACTIONS);
	};

	bool
	has_tfp_apply_actions() {
		return has_tfp(rofl::openflow13::OFPTFPT_APPLY_ACTIONS);
	};

	/*
	 * OFPTFPF_APPLY_ACTIONS_MISS
	 */
	coftable_feature_prop_actions&
	add_tfp_apply_actions_miss() {
		return dynamic_cast<coftable_feature_prop_actions&>( add_tfp(rofl::openflow13::OFPTFPT_APPLY_ACTIONS_MISS) );
	};

	coftable_feature_prop_actions&
	set_tfp_apply_actions_miss() {
		return dynamic_cast<coftable_feature_prop_actions&>( set_tfp(rofl::openflow13::OFPTFPT_APPLY_ACTIONS_MISS) );
	};

	coftable_feature_prop_actions&
	get_tfp_apply_actions_miss() {
		return dynamic_cast<coftable_feature_prop_actions&>( get_tfp(rofl::openflow13::OFPTFPT_APPLY_ACTIONS_MISS) );
	};

	void
	drop_tfp_apply_actions_miss() {
		drop_tfp(rofl::openflow13::OFPTFPT_APPLY_ACTIONS_MISS);
	};

	bool
	has_tfp_apply_actions_miss() {
		return has_tfp(rofl::openflow13::OFPTFPT_APPLY_ACTIONS_MISS);
	};





	/*
	 * OFPTFPF_MATCH
	 */
	coftable_feature_prop_oxm&
	add_tfp_match() {
		return dynamic_cast<coftable_feature_prop_oxm&>( add_tfp(rofl::openflow13::OFPTFPT_MATCH) );
	};

	coftable_feature_prop_oxm&
	set_tfp_match() {
		return dynamic_cast<coftable_feature_prop_oxm&>( set_tfp(rofl::openflow13::OFPTFPT_MATCH) );
	};

	coftable_feature_prop_oxm&
	get_tfp_match() {
		return dynamic_cast<coftable_feature_prop_oxm&>( get_tfp(rofl::openflow13::OFPTFPT_MATCH) );
	};

	void
	drop_tfp_match() {
		drop_tfp(rofl::openflow13::OFPTFPT_MATCH);
	};

	bool
	has_tfp_match() {
		return has_tfp(rofl::openflow13::OFPTFPT_MATCH);
	};




	/*
	 * OFPTFPF_WILDCARDS
	 */
	coftable_feature_prop_oxm&
	add_tfp_wildcards() {
		return dynamic_cast<coftable_feature_prop_oxm&>( add_tfp(rofl::openflow13::OFPTFPT_WILDCARDS) );
	};

	coftable_feature_prop_oxm&
	set_tfp_wildcards() {
		return dynamic_cast<coftable_feature_prop_oxm&>( set_tfp(rofl::openflow13::OFPTFPT_WILDCARDS) );
	};

	coftable_feature_prop_oxm&
	get_tfp_wildcards() {
		return dynamic_cast<coftable_feature_prop_oxm&>( get_tfp(rofl::openflow13::OFPTFPT_WILDCARDS) );
	};

	void
	drop_tfp_wildcards() {
		drop_tfp(rofl::openflow13::OFPTFPT_WILDCARDS);
	};

	bool
	has_tfp_wildcards() {
		return has_tfp(rofl::openflow13::OFPTFPT_WILDCARDS);
	};




	/*
	 * OFPTFPF_WRITE_SETFIELD
	 */
	coftable_feature_prop_oxm&
	add_tfp_write_setfield() {
		return dynamic_cast<coftable_feature_prop_oxm&>( add_tfp(rofl::openflow13::OFPTFPT_WRITE_SETFIELD) );
	};

	coftable_feature_prop_oxm&
	set_tfp_write_setfield() {
		return dynamic_cast<coftable_feature_prop_oxm&>( set_tfp(rofl::openflow13::OFPTFPT_WRITE_SETFIELD) );
	};

	coftable_feature_prop_oxm&
	get_tfp_write_setfield() {
		return dynamic_cast<coftable_feature_prop_oxm&>( get_tfp(rofl::openflow13::OFPTFPT_WRITE_SETFIELD) );
	};

	void
	drop_tfp_write_setfield() {
		drop_tfp(rofl::openflow13::OFPTFPT_WRITE_SETFIELD);
	};

	bool
	has_tfp_write_setfield() {
		return has_tfp(rofl::openflow13::OFPTFPT_WRITE_SETFIELD);
	};

	/*
	 * OFPTFPF_WRITE_SETFIELD_MISS
	 */
	coftable_feature_prop_oxm&
	add_tfp_write_setfield_miss() {
		return dynamic_cast<coftable_feature_prop_oxm&>( add_tfp(rofl::openflow13::OFPTFPT_WRITE_SETFIELD_MISS) );
	};

	coftable_feature_prop_oxm&
	set_tfp_write_setfield_miss() {
		return dynamic_cast<coftable_feature_prop_oxm&>( set_tfp(rofl::openflow13::OFPTFPT_WRITE_SETFIELD_MISS) );
	};

	coftable_feature_prop_oxm&
	get_tfp_write_setfield_miss() {
		return dynamic_cast<coftable_feature_prop_oxm&>( get_tfp(rofl::openflow13::OFPTFPT_WRITE_SETFIELD_MISS) );
	};

	void
	drop_tfp_write_setfield_miss() {
		drop_tfp(rofl::openflow13::OFPTFPT_WRITE_SETFIELD_MISS);
	};

	bool
	has_tfp_write_setfield_miss() {
		return has_tfp(rofl::openflow13::OFPTFPT_WRITE_SETFIELD_MISS);
	};





	/*
	 * OFPTFPF_APPLY_SETFIELD
	 */
	coftable_feature_prop_oxm&
	add_tfp_apply_setfield() {
		return dynamic_cast<coftable_feature_prop_oxm&>( add_tfp(rofl::openflow13::OFPTFPT_APPLY_SETFIELD) );
	};

	coftable_feature_prop_oxm&
	set_tfp_apply_setfield() {
		return dynamic_cast<coftable_feature_prop_oxm&>( set_tfp(rofl::openflow13::OFPTFPT_APPLY_SETFIELD) );
	};

	coftable_feature_prop_oxm&
	get_tfp_apply_setfield() {
		return dynamic_cast<coftable_feature_prop_oxm&>( get_tfp(rofl::openflow13::OFPTFPT_APPLY_SETFIELD) );
	};

	void
	drop_tfp_apply_setfield() {
		drop_tfp(rofl::openflow13::OFPTFPT_APPLY_SETFIELD);
	};

	bool
	has_tfp_apply_setfield() {
		return has_tfp(rofl::openflow13::OFPTFPT_APPLY_SETFIELD);
	};

	/*
	 * OFPTFPF_APPLY_SETFIELD_MISS
	 */
	coftable_feature_prop_oxm&
	add_tfp_apply_setfield_miss() {
		return dynamic_cast<coftable_feature_prop_oxm&>( add_tfp(rofl::openflow13::OFPTFPT_APPLY_SETFIELD_MISS) );
	};

	coftable_feature_prop_oxm&
	set_tfp_apply_setfield_miss() {
		return dynamic_cast<coftable_feature_prop_oxm&>( set_tfp(rofl::openflow13::OFPTFPT_APPLY_SETFIELD_MISS) );
	};

	coftable_feature_prop_oxm&
	get_tfp_apply_setfield_miss() {
		return dynamic_cast<coftable_feature_prop_oxm&>( get_tfp(rofl::openflow13::OFPTFPT_APPLY_SETFIELD_MISS) );
	};

	void
	drop_tfp_apply_setfield_miss() {
		drop_tfp(rofl::openflow13::OFPTFPT_APPLY_SETFIELD_MISS);
	};

	bool
	has_tfp_apply_setfield_miss() {
		return has_tfp(rofl::openflow13::OFPTFPT_APPLY_SETFIELD_MISS);
	};


	/*
	 * TODO: OFPTFPT_EXPERIMENTER / OFPTFPT_EXPERIMENTER_MISS
	 */


private:

	/*
	 * OFPTFPF_XXX / OFPTFPF_XXX_MISS
	 */
	coftable_feature_prop&
	add_tfp(unsigned int type);

	coftable_feature_prop&
	set_tfp(unsigned int type);

	coftable_feature_prop&
	get_tfp(unsigned int type);

	void
	drop_tfp(unsigned int type);

	bool
	has_tfp(unsigned int type);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftable_feature_props const& tfps) {
		os << indent(0) << "<coftable_feature_props ofp-version:" << (int)tfps.get_version() << " >" << std::endl;
		for (std::map<uint16_t, coftable_feature_prop*>::const_iterator
				it = tfps.tfprops.begin(); it != tfps.tfprops.end(); ++it) {
			indent i(2);

			unsigned int type = it->first;

			switch (type) {
			case rofl::openflow13::OFPTFPT_INSTRUCTIONS:
			case rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS: {
				os << dynamic_cast<coftable_feature_prop_instructions const&>( *(it->second) );
			} break;
			case rofl::openflow13::OFPTFPT_NEXT_TABLES:
			case rofl::openflow13::OFPTFPT_NEXT_TABLES_MISS: {
				os << dynamic_cast<coftable_feature_prop_next_tables const&>( *(it->second) );
			} break;
			case rofl::openflow13::OFPTFPT_WRITE_ACTIONS:
			case rofl::openflow13::OFPTFPT_WRITE_ACTIONS_MISS:
			case rofl::openflow13::OFPTFPT_APPLY_ACTIONS:
			case rofl::openflow13::OFPTFPT_APPLY_ACTIONS_MISS: {
				os << dynamic_cast<coftable_feature_prop_actions const&>( *(it->second) );
			} break;
			case rofl::openflow13::OFPTFPT_MATCH:
			case rofl::openflow13::OFPTFPT_WILDCARDS:
			case rofl::openflow13::OFPTFPT_WRITE_SETFIELD:
			case rofl::openflow13::OFPTFPT_WRITE_SETFIELD_MISS:
			case rofl::openflow13::OFPTFPT_APPLY_SETFIELD:
			case rofl::openflow13::OFPTFPT_APPLY_SETFIELD_MISS: {
				os << dynamic_cast<coftable_feature_prop_oxm const&>( *(it->second) );
			} break;
			default: {
				os << dynamic_cast<coftable_feature_prop const&>( *(it->second) );
			} break;
			}
		}
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFTABLEFEATUREPROPS_H_ */
