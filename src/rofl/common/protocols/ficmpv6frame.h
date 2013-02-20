/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ficmpv6frame_H
#define ficmpv6frame_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <endian.h>

#ifndef htobe16
#include "../endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "../fframe.h"
#include "../caddress.h"
#include "../cvastring.h"

namespace rofl
{

class eICMPv6FrameBase 				: public eFrameBase {};
class eICMPv6FrameInvalidSyntax 	: public eICMPv6FrameBase, public eFrameInvalidSyntax {};
class eICMPv6FrameTooShort			: public eICMPv6FrameInvalidSyntax {};



class ficmpv6frame : public fframe {

#define IPV6_ADDR_LEN		16

public:

	/* ICMPv6 constants and definitions */
	struct icmpv6_hdr_t {
		uint8_t 	type;
		uint8_t 	code;
		uint16_t 	checksum;
		uint8_t 	data[0];
	} __attribute__((packed));

	/* for ICMPv6 checksum calculation */
	struct icmpv6_pseudo_hdr_t {
		uint8_t 	src[IPV6_ADDR_LEN];
		uint8_t 	dst[IPV6_ADDR_LEN];
		uint32_t 	icmpv6_len;				// payload length (extension headers + ICMPv6 message)
		uint8_t 	zeros[3];				// = 0
		uint8_t 	nxthdr;					// = 58 (=ICMPV6_IP_PROTO, see below)
	} __attribute__((packed));

	enum icmpv6_ip_proto_t {
		ICMPV6_IP_PROTO = 58,
	};

	enum icmpv6_type_t {
		ICMPV6_TYPE_DESTINATION_UNREACHABLE 		= 1,
		ICMPV6_TYPE_PACKET_TOO_BIG					= 2,
		ICMPV6_TYPE_TIME_EXCEEDED					= 3,
		ICMPV6_TYPE_PARAMETER_PROBLEM				= 4,
		ICMPV6_TYPE_ECHO_REQUEST					= 128,
		ICMPV6_TYPE_ECHO_REPLY						= 129,
		ICMPV6_TYPE_MULTICAST_LISTENER_QUERY		= 130,
		ICMPV6_TYPE_MULTICAST_LISTENER_REPORT		= 131,
		ICMPV6_TYPE_MULTICAST_LISTENER_DONE			= 132,
		ICMPV6_TYPE_ROUTER_SOLICATION				= 133,
		ICMPV6_TYPE_ROUTER_ADVERTISEMENT			= 134,
		ICMPV6_TYPE_NEIGHBOR_SOLICITATION			= 135,
		ICMPV6_TYPE_NEIGHBOR_ADVERTISEMENT			= 136,
		ICMPV6_TYPE_REDIRECT_MESSAGE				= 137,
		ICMPV6_TYPE_ROUTER_RENUMBERING				= 138,
		ICMPV6_TYPE_ICMP_NODE_INFORMATION_QUERY		= 139,
		ICMPV6_TYPE_ICMP_NODE_INFORMATION_RESPONSE	= 140,
		ICMPV6_TYPE_INVERSE_NEIGHBOR_DISCOVERY_SOLICITATION_MESSAGE 	= 141,
		ICMPV6_TYPE_INVERSE_NEIGHBOR_DISCOVERY_ADVERTISEMENT_MESSAGE 	= 142,
		ICMPV6_TYPE_MULTICAST_LISTENER_DISCOVERY_REPORT					= 143,
		ICMPV6_TYPE_HOME_AGENT_ADDRESS_DISCOVERY_REQUEST_MESSAGE		= 144,
		ICMPV6_TYPE_HOME_AGENT_ADDRESS_DISCOVERY_REPLY_MESSAGE			= 145,
		ICMPV6_TYPE_MOBILE_PREFIX_SOLICITATION		= 146,
		ICMPV6_TYPE_MOBILE_PREFIX_ADVERTISEMENT		= 147,
	};

	enum icmpv6_destination_unreachable_code_t {
		ICMPV6_DEST_UNREACH_CODE_NO_ROUTE_TO_DESTINATION					= 0,
		ICMPV6_DEST_UNREACH_CODE_COMMUNICATION_WITH_DESTINATION_ADMINISTRATIVELY_PROHIBITED	= 1,
		ICMPV6_DEST_UNREACH_CODE_BEYOND_SCOPE_OF_SOURCE_ADDRESS				= 2,
		ICMPV6_DEST_UNREACH_CODE_ADDRESS_UNREACHABLE						= 3,
		ICMPV6_DEST_UNREACH_CODE_PORT_UNREACHABLE							= 4,
		ICMPV6_DEST_UNREACH_CODE_SOURCE_ADDRESS_FAILED_INGRESS_EGRESS_POLICY= 5,
		ICMPV6_DEST_UNREACH_CODE_REJECT_ROUTE_TO_DESTINATION				= 6,
		ICMPV6_DEST_UNREACH_CODE_ERROR_IN_SOURCE_ROUTING_HEADER				= 7,
	};

#define DEFAULT_ICMPV6_FRAME_SIZE sizeof(struct icmpv6_hdr_t)

public: // data structures

	struct icmpv6_hdr_t 		*icmpv6_hdr;		// ICMPv6 message header
	uint8_t 					*data; 			// ICMPv6 message body
	size_t 						 datalen;		// ICMPv6 message body length

private: // data structures

	std::string 				 info;			// info string

public:


	/** constructor
	 *
	 */
	ficmpv6frame(
			uint8_t *data,
			size_t datalen);


	/** constructor for creating new icmpv4 frame
	 *
	 */
	ficmpv6frame(
			size_t len = DEFAULT_ICMPV6_FRAME_SIZE);


	/** destructor
	 *
	 */
	virtual
	~ficmpv6frame();


	/** calculate TCP header checksum
	 *
	 */
	void
	icmpv6_calc_checksum();


public: // overloaded from fframe

	/** returns boolean value indicating completeness of the packet
	 */
	virtual bool
	complete();

	/** returns the number of bytes this packet expects from the socket next
	 */
	virtual size_t
	need_bytes();

	/** validate (frame structure)
	 *
	 */
	virtual void
	validate(uint16_t total_len = 0) throw (eICMPv6FrameTooShort);

	/** initialize (set eth_hdr, pppoe_hdr)
	 *
	 */
	virtual void
	initialize();

	/** insert payload
	 *
	 */
	virtual void
	payload_insert(
			uint8_t *data, size_t datalen) throw (eFrameOutOfRange);

	/** get payload
	 *
	 */
	virtual uint8_t*
	payload() const throw (eFrameNoPayload);

	/** get payload length
	 *
	 */
	virtual size_t
	payloadlen() const throw (eFrameNoPayload);

	/** dump info
	 *
	 */
	virtual const char*
	c_str();

public:

	/**
	 *
	 */
	uint8_t
	get_icmpv6_code();

	/**
	 *
	 */
	void
	set_icmpv6_code(uint8_t code);

	/**
	 *
	 */
	uint8_t
	get_icmpv6_type();

	/**
	 *
	 */
	void
	set_icmpv6_type(uint8_t type);


private: // methods

};

}; // end of namespace

#endif
