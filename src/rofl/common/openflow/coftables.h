/*
 * coftables.h
 *
 *  Created on: 16.02.2014
 *      Author: andreas
 */

#ifndef COFTABLES_H_
#define COFTABLES_H_

#include <inttypes.h>

#include <map>

#include "rofl/common/openflow/coftablefeatures.h"
#include "rofl/common/croflexception.h"

#include "rofl/common/openflow/coftablestatsarray.h"

namespace rofl {
namespace openflow {

class eOFTablesBase 		: public RoflException {};
class eOFTablesInval		: public eOFTablesBase {};
class eOFTablesNotFound		: public eOFTablesBase {};

class coftables
{
	uint8_t 									ofp_version;
	std::map<uint8_t, coftable_features>		tables;

public:

	/**
	 *
	 */
	coftables(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~coftables();

	/**
	 *
	 */
	coftables(
			coftables const& tables);

	/**
	 *
	 */
	coftables&
	operator= (
			coftables const& tables);

	/**
	 *
	 */
	coftables&
	operator+= (
			coftables const& tables);


public:

	/** reset packet content
	 *
	 */
	virtual void
	clear();


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(
			uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);


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

	/**
	 *
	 */
	std::map<uint8_t, coftable_features> const&
	get_tables() const { return tables; };

	/**
	 *
	 */
	std::map<uint8_t, coftable_features>&
	set_tables() { return tables; };

	/**
	 *
	 */
	coftable_features&
	add_table(uint8_t table_id);

	/**
	 *
	 */
	void
	drop_table(uint8_t table_id);

	/**
	 *
	 */
	coftable_features&
	get_table(uint8_t table_id);

	/**
	 *
	 */
	coftable_features&
	set_table(uint8_t table_id);

	/**
	 *
	 */
	bool
	has_table(uint8_t table_id);

public:

	/**
	 *
	 */
	static void
	map_tablestatsarray_to_tables(
			rofl::openflow::coftablestatsarray& tablestatsarray, rofl::openflow::coftables& tables);

	/**
	 *
	 */
	static void
	map_tables_to_tablestatsarray(
			rofl::openflow::coftables& tables, rofl::openflow::coftablestatsarray& tablestatsarray);

	/**
	 *
	 */
	static void
	map_match_to_prop_oxm(
			uint64_t match, rofl::openflow::coftable_feature_prop_oxm& prop_oxm);

	/**
	 *
	 */
	static void
	map_prop_oxm_to_match(
			rofl::openflow::coftable_feature_prop_oxm& prop_oxm, uint64_t& match);

	/**
	 *
	 */
	static void
	map_actions_to_prop_actions(
			uint32_t actions, rofl::openflow::coftable_feature_prop_actions& prop_actions);

	/**
	 *
	 */
	static void
	map_prop_actions_to_actions(
		rofl::openflow::coftable_feature_prop_actions& prop_actions, uint32_t& actions);

	/**
	 *
	 */
	static void
	map_instructions_to_prop_instructions(
			uint32_t instructions, rofl::openflow::coftable_feature_prop_instructions& prop_instructions);

	/**
	 *
	 */
	static void
	map_prop_instructions_to_instructions(
			rofl::openflow::coftable_feature_prop_instructions& prop_instructions, uint32_t& instructions);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftables const& tables) {
		os << indent(0) << "<coftables #tables:" << tables.tables.size() << " >" << std::endl;
		indent i(2);
		for (std::map<uint8_t, coftable_features>::const_iterator
				it = tables.tables.begin(); it != tables.tables.end(); ++it) {
			os << it->second;
		}
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl




#endif /* COFTABLES_H_ */
