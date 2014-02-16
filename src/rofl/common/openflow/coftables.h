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
