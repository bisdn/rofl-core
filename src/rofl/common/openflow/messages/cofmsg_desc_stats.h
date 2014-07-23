/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_desc_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_DESC_STATS_H_
#define COFMSG_DESC_STATS_H_ 1

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofdescstats.h"

namespace rofl {
namespace openflow {


/**
 *
 */
class cofmsg_desc_stats_request :
	public cofmsg_stats_request
{
private:

public:


	/** constructor
	 *
	 */
	cofmsg_desc_stats_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0);


	/**
	 *
	 */
	cofmsg_desc_stats_request(
			cofmsg_desc_stats_request const& stats);


	/**
	 *
	 */
	cofmsg_desc_stats_request&
	operator= (
			cofmsg_desc_stats_request const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_desc_stats_request();


	/**
	 *
	 */
	cofmsg_desc_stats_request(cmemory *memarea);


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

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_desc_stats_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_desc_stats_request >" << std::endl;
		return os;
	};
};



/**
 *
 */
class cofmsg_desc_stats_reply :
	public cofmsg_stats_reply
{
private:

	cofdesc_stats_reply 		desc_stats;

	union {
		uint8_t*								ofhu_desc_stats;
		struct openflow10::ofp_desc_stats*		ofhu10_desc_stats;
		struct openflow12::ofp_desc_stats*		ofhu12_desc_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_desc_stats   			ofhu.ofhu_desc_stats
#define ofh10_desc_stats 			ofhu.ofhu10_desc_stats
#define ofh12_desc_stats 			ofhu.ofhu12_desc_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_desc_stats_reply(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t flags = 0,
			cofdesc_stats_reply const& desc_stats = cofdesc_stats_reply());


	/**
	 *
	 */
	cofmsg_desc_stats_reply(
			cofmsg_desc_stats_reply const& stats);


	/**
	 *
	 */
	cofmsg_desc_stats_reply&
	operator= (
			cofmsg_desc_stats_reply const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_desc_stats_reply();


	/**
	 *
	 */
	cofmsg_desc_stats_reply(cmemory *memarea);


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
	cofdesc_stats_reply&
	get_desc_stats();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_desc_stats_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_desc_stats_reply >" << std::endl;
		indent i(2);
		os << msg.desc_stats;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_DESC_STATS_H_ */
