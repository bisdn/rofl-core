/*
 * cofmsg_packet_in.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PACKET_IN_H_
#define COFMSG_PACKET_IN_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/cpacket.h"

namespace rofl {
namespace openflow {

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

#define OFP10_PACKET_IN_STATIC_HDR_LEN				(sizeof(struct openflow10::ofp_packet_in) - 2)
#define OFP12_PACKET_IN_STATIC_HDR_LEN				(sizeof(struct openflow12::ofp_packet_in) - sizeof(struct openflow12::ofp_match))	// length without struct openflow12::ofp_match
#define OFP13_PACKET_IN_STATIC_HDR_LEN				(sizeof(struct openflow13::ofp_packet_in) - sizeof(struct openflow13::ofp_match))	// length without struct openflow13::ofp_match

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
			uint64_t of13_cookie = 0,	/*OF1.3*/
			uint16_t of10_in_port = 0, 	/*OF1.0*/
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
	set_match() { return match; };

	/**
	 *
	 */
	cofmatch const&
	get_match() const { return match; };

	/**
	 *
	 */
	cpacket&
	set_packet() { return packet; };

	/**
	 *
	 */
	cpacket const&
	get_packet() const { return packet; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_packet_in const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_packet_in >" << std::endl;
		switch (msg.get_version()) {
		case openflow10::OFP_VERSION: {
			os << indent(4) << "<in-port:" 		<< (int)msg.get_in_port() 	<< " >" << std::endl;
			os << indent(4) << "<buffer-id:" 	<< (int)msg.get_buffer_id() << " >" << std::endl;
			os << indent(4) << "<total-len:" 	<< (int)msg.get_total_len() << " >" << std::endl;
			switch (msg.get_reason()) {
			case rofl::openflow10::OFPR_NO_MATCH: {
				os << indent(4) << "<reason: NO-MATCH >" << std::endl;
			} break;
			case rofl::openflow10::OFPR_ACTION: {
				os << indent(4) << "<reason: ACTION >" << std::endl;
			} break;
			default: {
				os << indent(4) << "<reason:" 		<< (int)msg.get_reason() 	<< " >" << std::endl;
			};
			}

		} break;
		case openflow12::OFP_VERSION: {
			os << indent(4) << "<buffer-id:"	<< (int)msg.get_buffer_id() << " >" << std::endl;
			os << indent(4) << "<total-len:" 	<< (int)msg.get_total_len() << " >" << std::endl;
			switch (msg.get_reason()) {
			case rofl::openflow12::OFPR_NO_MATCH: {
				os << indent(4) << "<reason: NO-MATCH >" << std::endl;
			} break;
			case rofl::openflow12::OFPR_ACTION: {
				os << indent(4) << "<reason: ACTION >" << std::endl;
			} break;
			case rofl::openflow12::OFPR_INVALID_TTL: {
				os << indent(4) << "<reason: INVALID-TTL >" << std::endl;
			} break;
			default: {
				os << indent(4) << "<reason:" 		<< (int)msg.get_reason() 	<< " >" << std::endl;
			};
			}
			os << indent(4) << "<table-id:" 	<< (int)msg.get_table_id() 	<< " >" << std::endl;

		} break;
		case openflow13::OFP_VERSION: {
			os << indent(4) << "<buffer-id:" 	<< (int)msg.get_buffer_id() << " >" << std::endl;
			os << indent(4) << "<total-len:" 	<< (int)msg.get_total_len() << " >" << std::endl;
			switch (msg.get_reason()) {
			case rofl::openflow13::OFPR_NO_MATCH: {
				os << indent(4) << "<reason: NO-MATCH >" << std::endl;
			} break;
			case rofl::openflow13::OFPR_ACTION: {
				os << indent(4) << "<reason: ACTION >" << std::endl;
			} break;
			case rofl::openflow13::OFPR_INVALID_TTL: {
				os << indent(4) << "<reason: INVALID-TTL >" << std::endl;
			} break;
			default: {
				os << indent(4) << "<reason:" 		<< (int)msg.get_reason() 	<< " >" << std::endl;
			};
			}
			os << indent(4) << "<table-id:" 	<< (int)msg.get_table_id() 	<< " >" << std::endl;
			os << indent(4) << "<cookie:" 		<< std::hex << (unsigned long long)msg.get_cookie() << std::dec << " >" << std::endl;

		} break;
		default: {
			os << "<unsupported OF version:" 	<< (int)msg.get_version() 	<< " >" << std::endl;
		} break;
		}
		os << indent(2) << msg.match;
		os << indent(2) << msg.packet;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_PACKET_IN_H_ */
