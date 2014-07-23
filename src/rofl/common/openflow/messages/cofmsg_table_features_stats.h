/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_table_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_TABLE_FEATURES_H_
#define COFMSG_TABLE_FEATURES_H_ 1

#include <map>

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/coftables.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_table_features_stats_request :
	public cofmsg_stats_request
{
protected:

	rofl::openflow::coftables		tables;

	union {
		uint8_t*											ofhu_generic;
		struct rofl::openflow13::ofp_table_features*		ofhu_table_features;
	} ofh_ofhu;

#define ofh_generic   			ofh_ofhu.ofhu_generic
#define ofh_table_features 		ofh_ofhu.ofhu_table_features

public:


	/** constructor
	 *
	 */
	cofmsg_table_features_stats_request(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			rofl::openflow::coftables const& tables = rofl::openflow::coftables());


	/**
	 *
	 */
	cofmsg_table_features_stats_request(
			cofmsg_table_features_stats_request const& msg);


	/**
	 *
	 */
	cofmsg_table_features_stats_request&
	operator= (
			cofmsg_table_features_stats_request const& msg);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_table_features_stats_request();


	/**
	 *
	 */
	cofmsg_table_features_stats_request(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/**
	 *
	 */
	virtual uint8_t*
	resize(size_t len);


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();

public:

	/**
	 *
	 */
	rofl::openflow::coftables const&
	get_tables() const { return tables; };

	/**
	 *
	 */
	rofl::openflow::coftables&
	set_tables() { return tables; };


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_table_features_stats_request const& msg) {
		os << dynamic_cast<cofmsg_stats_request const&>( msg );
		os << indent(2) << "<cofmsg_table_features_request >" << std::endl;
		indent i(4); os << msg.tables;
		return os;
	};
};








/**
 *
 */
class cofmsg_table_features_stats_reply :
	public cofmsg_stats_reply
{
protected:

	rofl::openflow::coftables		tables;

	union {
		uint8_t*											ofhu_generic;
		struct rofl::openflow13::ofp_table_features*		ofhu_table_features;
	} ofh_ofhu;

#define ofh_generic   			ofh_ofhu.ofhu_generic
#define ofh_table_features 		ofh_ofhu.ofhu_table_features

public:


	/** constructor
	 *
	 */
	cofmsg_table_features_stats_reply(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			rofl::openflow::coftables const tables = rofl::openflow::coftables());


	/**
	 *
	 */
	cofmsg_table_features_stats_reply(
			cofmsg_table_features_stats_reply const& msg);


	/**
	 *
	 */
	cofmsg_table_features_stats_reply&
	operator= (
			cofmsg_table_features_stats_reply const& msg);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_table_features_stats_reply();


	/**
	 *
	 */
	cofmsg_table_features_stats_reply(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/**
	 *
	 */
	virtual uint8_t*
	resize(size_t len);


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();

public:

	/**
	 *
	 */
	rofl::openflow::coftables const&
	get_tables() const { return tables; };

	/**
	 *
	 */
	rofl::openflow::coftables&
	set_tables() { return tables; };


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_table_features_stats_reply const& msg) {
		os << dynamic_cast<cofmsg_stats_reply const&>( msg );
		os << indent(2) << "<cofmsg_table_features_reply >" << std::endl;
		indent i(4); os << msg.tables;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_TABLE_STATS_H_ */
