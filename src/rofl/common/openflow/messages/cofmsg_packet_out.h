/*
 * cofmsg_packet_out.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PACKET_OUT_H_
#define COFMSG_PACKET_OUT_H_ 1

#include "cofmsg.h"
#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/cpacket.h"

namespace rofl
{

/**
 *
 */
class cofmsg_packet_out :
	public cofmsg
{
private:

	cofactions			actions;
	cpacket				packet;

	union {
		uint8_t*						ofhu_packet_out;
		struct openflow10::ofp_packet_out*		ofhu10_packet_out;
		struct openflow12::ofp_packet_out*		ofhu12_packet_out;
		struct openflow13::ofp_packet_out*		ofhu13_packet_out;
	} ofhu;

#define ofh_packet_out   ofhu.ofhu_packet_out
#define ofh10_packet_out ofhu.ofhu10_packet_out
#define ofh12_packet_out ofhu.ofhu12_packet_out
#define ofh13_packet_out ofhu.ofhu13_packet_out

public:


	/** constructor
	 *
	 */
	cofmsg_packet_out(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint32_t buffer_id = 0,
			uint32_t in_port = 0,
			cofactions const& actions = cofactions(),
			uint8_t *data = (uint8_t*)0,
			size_t datalen = 0);


	/**
	 *
	 */
	cofmsg_packet_out(
			cofmsg_packet_out const& packet_out);


	/**
	 *
	 */
	cofmsg_packet_out&
	operator= (
			cofmsg_packet_out const& packet_out);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_packet_out();


	/**
	 *
	 */
	cofmsg_packet_out(cmemory *memarea);


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
	uint32_t
	get_buffer_id() const;

	/**
	 *
	 */
	void
	set_buffer_id(uint32_t buffer_id);

	/**
	 *
	 */
	uint32_t
	get_in_port() const;

	/**
	 *
	 */
	void
	set_in_port(uint32_t in_port);

	/**
	 *
	 */
	cofactions&
	get_actions();

	/**
	 *
	 */
	cpacket&
	get_packet();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_packet_out const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(0) << "<cofmsg_packet_out >" << std::endl;
			os << indent(2) << "<buffer-id:" 	<< (int)msg.get_buffer_id() << " >" << std::endl;
			os << indent(2) << "<in-port:" 		<< (int)msg.get_in_port() 	<< " >" << std::endl;
			os << indent(2) << msg.actions;
			os << indent(2) << msg.packet;
		return os;
	};
};

} // end of namespace rofl

#endif /* COFMSG_PACKET_OUT_H_ */
