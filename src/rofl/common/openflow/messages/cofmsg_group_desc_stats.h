/*
 * cofmsg_group_desc_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_GROUP_DESC_STATS_H_
#define COFMSG_GROUP_DESC_STATS_H_ 1

#include <vector>

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofgroupdescstatsarray.h"

namespace rofl {
namespace openflow {

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

	rofl::openflow::cofgroupdescstatsarray 				groupdescstatsarray;

	union {
		uint8_t*										ofhu_group_desc_stats;
		struct rofl::openflow12::ofp_group_desc_stats*	ofhu12_group_desc_stats;
		struct rofl::openflow13::ofp_group_desc*		ofhu13_group_desc_stats;
	} ofhu;

#define ofh_group_desc_stats   			ofhu.ofhu_group_desc_stats
#define ofh12_group_desc_stats 			ofhu.ofhu12_group_desc_stats
#define ofh13_group_desc	 			ofhu.ofhu13_group_desc

public:


	/** constructor
	 *
	 */
	cofmsg_group_desc_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			rofl::openflow::cofgroupdescstatsarray const& groupdescs = rofl::openflow::cofgroupdescstatsarray());


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
	rofl::openflow::cofgroupdescstatsarray&
	set_group_desc_stats_array() { return groupdescstatsarray; };

	/**
	 *
	 */
	rofl::openflow::cofgroupdescstatsarray const&
	get_group_desc_stats_array() const { return groupdescstatsarray; };


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_group_desc_stats_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_group_desc_stats_reply >" << std::endl;
		indent i(4);
		os << msg.groupdescstatsarray;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_GROUP_DESC_STATS_H_ */
