/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_experimenter_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_EXPERIMENTER_STATS_H_
#define COFMSG_EXPERIMENTER_STATS_H_ 1

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofgroupfeaturesstats.h"
#include "rofl/common/cmemory.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_experimenter_stats_request :
	public cofmsg_stats_request
{
private:

	cmemory			body;

	union {
		uint8_t*									ofhu_exp_stats;
		struct openflow10::ofp_vendor_stats_header*			ofhu10_exp_stats;
		struct openflow12::ofp_experimenter_stats_header*		ofhu12_exp_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_exp_stats   			ofhu.ofhu_exp_stats
#define ofh10_exp_stats 			ofhu.ofhu10_exp_stats
#define ofh12_exp_stats 			ofhu.ofhu12_exp_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_experimenter_stats_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			uint32_t exp_id = 0,
			uint32_t exp_type = 0,
			cmemory const& body = cmemory(0));


	/**
	 *
	 */
	cofmsg_experimenter_stats_request(
			cofmsg_experimenter_stats_request const& stats);


	/**
	 *
	 */
	cofmsg_experimenter_stats_request&
	operator= (
			cofmsg_experimenter_stats_request const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_experimenter_stats_request();


	/**
	 *
	 */
	cofmsg_experimenter_stats_request(cmemory *memarea);


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
	void
	set_exp_id(uint32_t exp_id);


	/**
	 *
	 */
	uint32_t
	get_exp_id() const;


	/**
	 *
	 */
	void
	set_exp_type(uint32_t exp_type);


	/**
	 *
	 */
	uint32_t
	get_exp_type() const;


	/**
	 *
	 */
	cmemory&
	get_body();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_experimenter_stats_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_experimenter_stats_request >" << std::endl;
		os << indent(4) << "<exp-id:" << (int)msg.get_exp_id() << " >" << std::endl;
		os << indent(4) << "<exp-type:" << (int)msg.get_exp_type() << " >" << std::endl;
		indent i(4);
		os << msg.body;
		return os;
	};
};



/**
 *
 */

/**
 *
 */
class cofmsg_experimenter_stats_reply :
	public cofmsg_stats_reply
{
private:

	cmemory			body;

	union {
		uint8_t*									ofhu_exp_stats;
		struct openflow10::ofp_vendor_stats_header*			ofhu10_exp_stats;
		struct openflow12::ofp_experimenter_stats_header*		ofhu12_exp_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_exp_stats   			ofhu.ofhu_exp_stats
#define ofh10_exp_stats 			ofhu.ofhu10_exp_stats
#define ofh12_exp_stats 			ofhu.ofhu12_exp_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_experimenter_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			uint32_t exp_id = 0,
			uint32_t exp_type = 0,
			cmemory const& body = cmemory(0));


	/**
	 *
	 */
	cofmsg_experimenter_stats_reply(
			cofmsg_experimenter_stats_reply const& stats);


	/**
	 *
	 */
	cofmsg_experimenter_stats_reply&
	operator= (
			cofmsg_experimenter_stats_reply const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_experimenter_stats_reply();


	/**
	 *
	 */
	cofmsg_experimenter_stats_reply(cmemory *memarea);


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
	void
	set_exp_id(uint32_t exp_id);


	/**
	 *
	 */
	uint32_t
	get_exp_id() const;


	/**
	 *
	 */
	void
	set_exp_type(uint32_t exp_type);


	/**
	 *
	 */
	uint32_t
	get_exp_type() const;


	/**
	 *
	 */
	cmemory&
	get_body();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_experimenter_stats_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_experimenter_stats_request >" << std::endl;
		os << indent(4) << "<exp-id:" << (int)msg.get_exp_id() << " >" << std::endl;
		os << indent(4) << "<exp-type:" << (int)msg.get_exp_type() << " >" << std::endl;
		indent i(4);
		os << msg.body;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_GROUP_FEATURES_STATS_H_ */
