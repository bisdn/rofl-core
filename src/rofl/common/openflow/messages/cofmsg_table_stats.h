/*
 * cofmsg_table_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_TABLE_STATS_H_
#define COFMSG_TABLE_STATS_H_ 1

#include <vector>

#include "cofmsg_stats.h"
#include "rofl/common/openflow/coftablestats.h"

namespace rofl
{


/**
 *
 */
class cofmsg_table_stats_request :
	public cofmsg_stats
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
	virtual void
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
		os << indent(0) << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << dynamic_cast<cofmsg_stats const&>( msg );
		os << indent(4) << "<cofmsg_table_stats_request >" << std::endl;
		return os;
	};
};


/**
 *
 */
class cofmsg_table_stats_reply :
	public cofmsg_stats
{
private:

	std::vector<coftable_stats_reply> 	table_stats;

	union {
		uint8_t*						ofhu_table_stats;
		struct openflow10::ofp_table_stats*		ofhu10_table_stats;
		struct openflow12::ofp_table_stats*		ofhu12_table_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_table_stats   			ofhu.ofhu_table_stats
#define ofh10_table_stats 			ofhu.ofhu10_table_stats
#define ofh12_table_stats 			ofhu.ofhu12_table_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_table_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			std::vector<coftable_stats_reply> const& table_stats = std::vector<coftable_stats_reply>(0));


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
	virtual void
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
	std::vector<coftable_stats_reply>&
	get_table_stats();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_table_stats_reply const& msg) {
		os << indent(0) << dynamic_cast<cofmsg_stats const&>( msg );
		os << indent(4) << "<cofmsg_table_stats_reply >" << std::endl;
		indent i(6);
		for (std::vector<coftable_stats_reply>::const_iterator
				it = msg.table_stats.begin(); it != msg.table_stats.end(); ++it) {
			os << (*it);
		}
		return os;
	};
};

} // end of namespace rofl

#endif /* COFMSG_TABLE_STATS_H_ */
