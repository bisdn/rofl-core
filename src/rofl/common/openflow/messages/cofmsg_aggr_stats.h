/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_aggr_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_AGGR_STATS_H
#define COFMSG_AGGR_STATS_H 1

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofaggrstats.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_aggr_stats_request :
	public cofmsg_stats_request
{
private:

	cofaggr_stats_request 		aggr_stats;

	union {
		uint8_t													*ofhu_aggr_stats;
		struct rofl::openflow10::ofp_aggregate_stats_request 	*ofhu10_aggr_stats;
		struct rofl::openflow12::ofp_aggregate_stats_request	*ofhu12_aggr_stats;
		struct rofl::openflow13::ofp_aggregate_stats_request	*ofhu13_aggr_stats;
	} ofhu;

#define ofh_aggr_stats   			ofhu.ofhu_aggr_stats
#define ofh10_aggr_stats 			ofhu.ofhu10_aggr_stats
#define ofh12_aggr_stats 			ofhu.ofhu12_aggr_stats
#define ofh13_aggr_stats 			ofhu.ofhu13_aggr_stats

public:


	/** constructor
	 *
	 */
	cofmsg_aggr_stats_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			cofaggr_stats_request const& aggr_stats = cofaggr_stats_request());


	/**
	 *
	 */
	cofmsg_aggr_stats_request(
			cofmsg_aggr_stats_request const& stats);


	/**
	 *
	 */
	cofmsg_aggr_stats_request&
	operator= (
			cofmsg_aggr_stats_request const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_aggr_stats_request();


	/**
	 *
	 */
	cofmsg_aggr_stats_request(cmemory *memarea);


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
	cofaggr_stats_request&
	get_aggr_stats();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_aggr_stats_request const& msg) {
		os << indent(0) << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_aggr_stats_request >" << std::endl;
		indent i(4);
		os << msg.aggr_stats;
		return os;
	};
};



/**
 *
 */
class cofmsg_aggr_stats_reply :
	public cofmsg_stats_reply
{
private:

	cofaggr_stats_reply 		aggr_stats;

	union {
		uint8_t*								ofhu_aggr_stats;
		struct rofl::openflow10::ofp_aggregate_stats_reply*		ofhu10_aggr_stats;
		struct rofl::openflow12::ofp_aggregate_stats_reply*		ofhu12_aggr_stats;
		struct rofl::openflow13::ofp_aggregate_stats_reply*		ofhu13_aggr_stats;
	} ofhu;

#define ofh_aggr_stats   			ofhu.ofhu_aggr_stats
#define ofh10_aggr_stats 			ofhu.ofhu10_aggr_stats
#define ofh12_aggr_stats 			ofhu.ofhu12_aggr_stats
#define ofh13_aggr_stats 			ofhu.ofhu13_aggr_stats

public:


	/** constructor
	 *
	 */
	cofmsg_aggr_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			cofaggr_stats_reply const& aggr_stats = cofaggr_stats_reply());


	/**
	 *
	 */
	cofmsg_aggr_stats_reply(
			cofmsg_aggr_stats_reply const& stats);


	/**
	 *
	 */
	cofmsg_aggr_stats_reply&
	operator= (
			cofmsg_aggr_stats_reply const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_aggr_stats_reply();


	/**
	 *
	 */
	cofmsg_aggr_stats_reply(cmemory *memarea);


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
	cofaggr_stats_reply&
	set_aggr_stats();

	/**
	 *
	 */
	cofaggr_stats_reply const&
	get_aggr_stats() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_aggr_stats_reply const& msg) {
		os << indent(0) << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_aggr_stats_reply >" << std::endl;
		indent i(4);
		os << msg.aggr_stats;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_AGGR_STATS_H_ */
