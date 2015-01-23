/*
 * cofmsg_port_desc_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PORT_DESC_STATS_H_
#define COFMSG_PORT_DESC_STATS_H_ 1

#include <vector>

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofports.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_port_desc_stats_request :
	public cofmsg_stats_request
{
private:

public:


	/** constructor
	 *
	 */
	cofmsg_port_desc_stats_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0);


	/**
	 *
	 */
	cofmsg_port_desc_stats_request(
			cofmsg_port_desc_stats_request const& stats);


	/**
	 *
	 */
	cofmsg_port_desc_stats_request&
	operator= (
			cofmsg_port_desc_stats_request const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_port_desc_stats_request();


	/**
	 *
	 */
	cofmsg_port_desc_stats_request(cmemory *memarea);


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
	operator<< (std::ostream& os, cofmsg_port_desc_stats_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_port_desc_stats_request >" << std::endl;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "-Port-Desc-Stats-Request- " << cofmsg::str() << " ";
		return ss.str();
	};
};




/**
 *
 */
class cofmsg_port_desc_stats_reply :
	public cofmsg_stats_reply
{
private:

	rofl::openflow::cofports		ports;

	union {
		uint8_t									*ofhu_pds_reply;
		struct openflow13::ofp_multipart_reply	*ofhu13_pds_reply;
	} ofhu;

#define ofh_pds_reply   	ofhu.ofhu_pds_reply
#define ofh13_pds_reply 	ofhu.ofhu13_pds_reply

public:


	/** constructor
	 *
	 */
	cofmsg_port_desc_stats_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			rofl::openflow::cofports const& ports = rofl::openflow::cofports());


	/**
	 *
	 */
	cofmsg_port_desc_stats_reply(
			cofmsg_port_desc_stats_reply const& stats);


	/**
	 *
	 */
	cofmsg_port_desc_stats_reply&
	operator= (
			cofmsg_port_desc_stats_reply const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_port_desc_stats_reply();


	/**
	 *
	 */
	cofmsg_port_desc_stats_reply(cmemory *memarea);


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
	rofl::openflow::cofports&
	set_ports() { return ports; };

	/**
	 *
	 */
	rofl::openflow::cofports const&
	get_ports() const { return ports; };


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_port_desc_stats_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_port_desc_stats_reply >" << std::endl;
		indent i(4);
		os << msg.ports;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "-Port-Desc-Stats-Reply- " << cofmsg::str() << " ";
		return ss.str();
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_PORT_DESC_STATS_H_ */
