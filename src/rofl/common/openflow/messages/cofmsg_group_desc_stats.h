/*
 * cofmsg_group_desc_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_GROUP_DESC_STATS_H_
#define COFMSG_GROUP_DESC_STATS_H_ 1

#include <vector>

#include "cofmsg_stats.h"
#include "rofl/common/openflow/cofgroupdescstats.h"

namespace rofl
{


/**
 *
 */
class cofmsg_group_desc_stats_request :
	public cofmsg_stats_request
{
private:

public:


	/** constructor
	 *
	 */
	cofmsg_group_desc_stats_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0);


	/**
	 *
	 */
	cofmsg_group_desc_stats_request(
			cofmsg_group_desc_stats_request const& stats);


	/**
	 *
	 */
	cofmsg_group_desc_stats_request&
	operator= (
			cofmsg_group_desc_stats_request const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_group_desc_stats_request();


	/**
	 *
	 */
	cofmsg_group_desc_stats_request(cmemory *memarea);


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
	operator<< (std::ostream& os, cofmsg_group_desc_stats_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_group_desc_stats_request >" << std::endl;
		return os;
	};
};




/**
 *
 */
class cofmsg_group_desc_stats_reply :
	public cofmsg_stats_reply
{
private:

	std::vector<cofgroup_desc_stats_reply> 	group_desc_stats;

	union {
		uint8_t*						ofhu_group_desc_stats;
		struct openflow12::ofp_group_desc_stats*	ofhu12_group_desc_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_group_desc_stats   			ofhu.ofhu_group_desc_stats
#define ofh12_group_desc_stats 			ofhu.ofhu12_group_desc_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_group_desc_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			std::vector<cofgroup_desc_stats_reply> const& group_desc_stats = std::vector<cofgroup_desc_stats_reply>(0));


	/**
	 *
	 */
	cofmsg_group_desc_stats_reply(
			cofmsg_group_desc_stats_reply const& stats);


	/**
	 *
	 */
	cofmsg_group_desc_stats_reply&
	operator= (
			cofmsg_group_desc_stats_reply const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_group_desc_stats_reply();


	/**
	 *
	 */
	cofmsg_group_desc_stats_reply(cmemory *memarea);


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
	std::vector<cofgroup_desc_stats_reply>&
	get_group_desc_stats();


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_group_desc_stats_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_group_desc_stats_reply >" << std::endl;
		indent i(4);
		for (std::vector<cofgroup_desc_stats_reply>::const_iterator
				it = msg.group_desc_stats.begin(); it != msg.group_desc_stats.end(); ++it) {
			os << (*it);
		}
		return os;
	};
};

} // end of namespace rofl

#endif /* COFMSG_GROUP_DESC_STATS_H_ */
