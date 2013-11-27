/*
 * cofmsg_packet_in.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PACKET_IN_H_
#define COFMSG_PACKET_IN_H_ 1

#include "cofmsg.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/cpacket.h"

namespace rofl
{

/**
 *
 */
class cofmsg_packet_in :
	public cofmsg
{
private:

	cofmatch			match;
	cpacket				packet;

	union {
		uint8_t*						ofhu_packet_in;
		struct openflow10::ofp_packet_in*			ofhu10_packet_in;
		struct openflow12::ofp_packet_in*			ofhu12_packet_in;
		struct openflow13::ofp_packet_in*			ofhu13_packet_in;
	} ofhu;

#define ofh_packet_in   ofhu.ofhu_packet_in
#define ofh10_packet_in ofhu.ofhu10_packet_in
#define ofh12_packet_in ofhu.ofhu12_packet_in
#define ofh13_packet_in ofhu.ofhu13_packet_in

#define openflow10::OFP_PACKET_IN_STATIC_HDR_LEN				(sizeof(struct openflow10::ofp_packet_in))
#define openflow12::OFP_PACKET_IN_STATIC_HDR_LEN				(sizeof(struct openflow12::ofp_packet_in) - sizeof(struct openflow12::ofp_match))	// length without struct openflow12::ofp_match
#define openflow13::OFP_PACKET_IN_STATIC_HDR_LEN				(sizeof(struct openflow13::ofp_packet_in) - sizeof(struct openflow13::ofp_match))	// length without struct openflow13::ofp_match

public:


	/** constructor
	 *
	 */
	cofmsg_packet_in(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint32_t buffer_id = 0,
			uint16_t total_len = 0,
			uint8_t  reason = 0,
			uint8_t  table_id = 0,
			uint64_t cookie = 0,
			uint16_t in_port = 0, /*OF1.0*/
			cofmatch const& match = cofmatch(),
			uint8_t *data = (uint8_t*)0,
			size_t datalen = 0);


	/**
	 *
	 */
	cofmsg_packet_in(
			cofmsg_packet_in const& packet_in);


	/**
	 *
	 */
	cofmsg_packet_in&
	operator= (
			cofmsg_packet_in const& packet_in);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_packet_in();


	/**
	 *
	 */
	cofmsg_packet_in(cmemory *memarea);


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
	uint16_t
	get_total_len() const;

	/**
	 *
	 */
	void
	set_total_len(uint16_t total_len);

	/**
	 *
	 */
	uint8_t
	get_reason() const;

	/**
	 *
	 */
	void
	set_reason(uint8_t reason);

	/**
	 *
	 */
	uint8_t
	get_table_id() const;

	/**
	 *
	 */
	void
	set_table_id(uint8_t table_id);

	/** OF1.0
	 *
	 */
	uint16_t
	get_in_port() const;

	/** OF1.0
	 *
	 */
	void
	set_in_port(uint16_t port_no);

	/** OF1.3
	 *
	 */
	uint64_t
	get_cookie() const;

	/** OF1.3
	 *
	 */
	void
	set_cookie(uint64_t cookie);

	/**
	 *
	 */
	cofmatch&
	get_match();

	/**
	 *
	 */
	cofmatch const&
	get_match_const() const { return match; };

	/**
	 *
	 */
	cpacket&
	get_packet();

	/**
	 *
	 */
	cpacket const&
	get_packet_const() const { return packet; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_packet_in const& msg) {
		os << "<cofmsg_packet_in " << dynamic_cast<cofmsg const&>( msg ) << " ";
		switch (msg.get_version()) {
		case openflow10::OFP_VERSION: {
			os << "in-port:" << (int)msg.get_in_port() << " ";
			os << "buffer-id:" << (int)msg.get_buffer_id() << " ";
			os << "total-len:" << (int)msg.get_total_len() << " ";
			os << "reason:" << (int)msg.get_reason() << " ";
			os << "table-id:" << (int)msg.get_table_id() << " ";
			os << "match:" << msg.get_match_const() << " ";
			os << "packet:" << msg.get_packet_const() << " ";

		} break;
		case openflow12::OFP_VERSION: {
			os << "buffer-id:" << (int)msg.get_buffer_id() << " ";
			os << "total-len:" << (int)msg.get_total_len() << " ";
			os << "reason:" << (int)msg.get_reason() << " ";
			os << "table-id:" << (int)msg.get_table_id() << " ";
			os << "match:" << msg.get_match_const() << " ";
			os << "packet:" << msg.get_packet_const() << " ";

		} break;
		case openflow13::OFP_VERSION: {
			os << "buffer-id:" << (int)msg.get_buffer_id() << " ";
			os << "total-len:" << (int)msg.get_total_len() << " ";
			os << "reason:" << (int)msg.get_reason() << " ";
			os << "table-id:" << (int)msg.get_table_id() << " ";
			os << "cookie:" << (int)msg.get_cookie() << " ";
			os << "match:" << msg.get_match_const() << " ";
			os << "packet:" << msg.get_packet_const() << " ";

		} break;
		default: {
			os << "unsupported OF version:" << (int)msg.get_version() << " ";
		} break;
		}
		os << ">";
		return os;
	};
};

} // end of namespace rofl

#endif /* COFMSG_PACKET_IN_H_ */
