/*
 * cofmsg_table_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_TABLE_STATS_H_
#define COFMSG_TABLE_STATS_H_ 1

#include <vector>

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/coftablestats.h"
#include "rofl/common/openflow/coftablestatsarray.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_table_stats_request :
	public cofmsg_stats_request
{
private:


public:


	/** constructor
	 *
	 */
	cofmsg_table_stats_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0);


	/**
	 *
	 */
	cofmsg_table_stats_request(
			cofmsg_table_stats_request const& stats);


	/**
	 *
	 */
	cofmsg_table_stats_request&
	operator= (
			cofmsg_table_stats_request const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_table_stats_request();


	/**
	 *
	 */
	cofmsg_table_stats_request(cmemory *memarea);


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
	operator<< (std::ostream& os, cofmsg_table_stats_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		indent i(2);
		os << dynamic_cast<cofmsg_stats const&>( msg );
		os << indent(2) << "<cofmsg_table_stats_request >" << std::endl;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "-Table-Stats-Request- " << cofmsg::str() << " ";
		return ss.str();
	};
};


/**
 *
 */
class cofmsg_table_stats_reply :
	public cofmsg_stats_reply
{
private:

	rofl::openflow::coftablestatsarray		tablestatsarray;

	union {
		uint8_t*							ofhu_table_stats;
		struct openflow10::ofp_table_stats*	ofhu10_table_stats;
		struct openflow12::ofp_table_stats*	ofhu12_table_stats;
		struct openflow13::ofp_table_stats*	ofhu13_table_stats;
	} ofhu;

#define ofh_table_stats   			ofhu.ofhu_table_stats
#define ofh10_table_stats 			ofhu.ofhu10_table_stats
#define ofh12_table_stats 			ofhu.ofhu12_table_stats
#define ofh13_table_stats 			ofhu.ofhu13_table_stats

public:


	/** constructor
	 *
	 */
	cofmsg_table_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			rofl::openflow::coftablestatsarray const& table_stats = rofl::openflow::coftablestatsarray());


	/**
	 *
	 */
	cofmsg_table_stats_reply(
			cofmsg_table_stats_reply const& stats);


	/**
	 *
	 */
	cofmsg_table_stats_reply&
	operator= (
			cofmsg_table_stats_reply const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_table_stats_reply();


	/**
	 *
	 */
	cofmsg_table_stats_reply(cmemory *memarea);


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
	rofl::openflow::coftablestatsarray&
	set_table_stats_array() { return tablestatsarray; };

	/**
	 *
	 */
	rofl::openflow::coftablestatsarray const&
	get_table_stats_array() const { return tablestatsarray; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_table_stats_reply const& msg) {
		os << indent(0) << dynamic_cast<cofmsg_stats const&>( msg );
		os << indent(4) << "<cofmsg_table_stats_reply >" << std::endl;
		indent i(6);
		os << msg.tablestatsarray;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "-Table-Stats-Reply- " << cofmsg::str() << " ";
		return ss.str();
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_TABLE_STATS_H_ */
