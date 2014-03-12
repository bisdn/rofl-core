/*
 * cofmsg_port_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PORT_STATS_H_
#define COFMSG_PORT_STATS_H_ 1

#include <vector>

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofportstats.h"

namespace rofl
{




/**
 *
 */
class cofmsg_port_stats_request :
	public cofmsg_stats_request
{
private:

	cofport_stats_request	 	port_stats;

	union {
		uint8_t*						ofhu_port_stats;
		struct rofl::openflow10::ofp_port_stats*		ofhu10_port_stats;
		struct rofl::openflow12::ofp_port_stats*		ofhu12_port_stats;
		struct rofl::openflow13::ofp_port_stats*		ofhu13_port_stats;
	} ofhu;

#define ofh_port_stats   			ofhu.ofhu_port_stats
#define ofh10_port_stats 			ofhu.ofhu10_port_stats
#define ofh12_port_stats 			ofhu.ofhu12_port_stats
#define ofh13_port_stats 			ofhu.ofhu13_port_stats

public:


	/** constructor
	 *
	 */
	cofmsg_port_stats_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			cofport_stats_request const& port_stats = cofport_stats_request());


	/**
	 *
	 */
	cofmsg_port_stats_request(
			cofmsg_port_stats_request const& stats);


	/**
	 *
	 */
	cofmsg_port_stats_request&
	operator= (
			cofmsg_port_stats_request const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_port_stats_request();


	/**
	 *
	 */
	cofmsg_port_stats_request(cmemory *memarea);


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
	cofport_stats_request&
	set_port_stats();

	/**
	 *
	 */
	cofport_stats_request const&
	get_port_stats() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_port_stats_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_port_stats_request >" << std::endl;
		indent i(4);
		os << msg.port_stats;
		return os;
	};
};



/**
 *
 */
class cofmsg_port_stats_reply :
	public cofmsg_stats_reply
{
private:

	std::vector<cofport_stats_reply> 	port_stats;

	union {
		uint8_t*						ofhu_port_stats;
		struct rofl::openflow10::ofp_port_stats*		ofhu10_port_stats;
		struct rofl::openflow12::ofp_port_stats*		ofhu12_port_stats;
		struct rofl::openflow13::ofp_port_stats*		ofhu13_port_stats;
	} ofhu;

#define ofh_port_stats   			ofhu.ofhu_port_stats
#define ofh10_port_stats 			ofhu.ofhu10_port_stats
#define ofh12_port_stats 			ofhu.ofhu12_port_stats
#define ofh13_port_stats 			ofhu.ofhu13_port_stats

public:


	/** constructor
	 *
	 */
	cofmsg_port_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			std::vector<cofport_stats_reply> const& port_stats = std::vector<cofport_stats_reply>(0));


	/**
	 *
	 */
	cofmsg_port_stats_reply(
			cofmsg_port_stats_reply const& stats);


	/**
	 *
	 */
	cofmsg_port_stats_reply&
	operator= (
			cofmsg_port_stats_reply const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_port_stats_reply();


	/**
	 *
	 */
	cofmsg_port_stats_reply(cmemory *memarea);


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
	std::vector<cofport_stats_reply>&
	set_port_stats();

	/**
	 *
	 */
	std::vector<cofport_stats_reply> const&
	get_port_stats() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_port_stats_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_port_stats_reply >" << std::endl;
		indent i(4);
		for (std::vector<cofport_stats_reply>::const_iterator
				it = msg.port_stats.begin(); it != msg.port_stats.end(); ++it) {
			os << (*it);
		}
		return os;
	};
};

} // end of namespace rofl

#endif /* COFMSG_PORT_STATS_H_ */
