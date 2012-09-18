/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FICMPV4FRAME_H
#define FICMPV4FRAME_H 1

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


class ficmpv4frame : public fframe {
public:

	/* ICMPv4 constants and definitions */
	struct icmpv4_hdr_t {
		uint8_t type;
		uint8_t code;
		uint16_t checksum;
		uint8_t data[0];
	} __attribute__((packed));

	/* for UDP checksum calculation */
	struct ip_pseudo_hdr_t {
		uint32_t src;
		uint32_t dst;
		uint8_t reserved;
		uint8_t proto;
		uint16_t len;
	} __attribute__((packed));

	enum tcp_ip_proto_t {
		ICMPV4_IP_PROTO = 1,
	};

	enum icmpv4_type_t {
		ICMP_TYPE_DESTINATION_UNREACHABLE = 3,
		ICMP_TYPE_ECHO_REPLY = 0,
		ICMP_TYPE_ECHO_REQUEST = 8,
	};

	enum icmpv4_code_t {
		ICMP_CODE_HOST_UNREACHABLE = 1,
		ICMP_CODE_NO_CODE = 0,
		ICMP_CODE_DATAGRAM_TOO_BIG = 4,
	};

#define DEFAULT_ICMPV4_FRAME_SIZE sizeof(struct icmpv4_hdr_t)

public:


	/** constructor
	 *
	 */
	ficmpv4frame(
			uint8_t *data,
			size_t datalen);


	/** constructor for creating new icmpv4 frame
	 *
	 */
	ficmpv4frame(
			size_t len = DEFAULT_ICMPV4_FRAME_SIZE);


	/** destructor
	 *
	 */
	virtual
	~ficmpv4frame();


	/** calculate TCP header checksum
	 *
	 */
	void
	icmpv4_calc_checksum();


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
	validate() throw (eFrameInvalidSyntax);

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
	payload() throw (eFrameNoPayload);

	/** get payload length
	 *
	 */
	virtual size_t
	payloadlen() throw (eFrameNoPayload);

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
	get_icmp_code();

	/**
	 *
	 */
	void
	set_icmp_code(uint8_t code);

	/**
	 *
	 */
	uint8_t
	get_icmp_type();

	/**
	 *
	 */
	void
	set_icmp_type(uint8_t type);

public: // data structures

	// pointer to tcp header
	struct icmpv4_hdr_t *icmp_hdr;

	// udp payload
	uint8_t *data;

	// udp payload length
	size_t datalen;

private: // data structures

	//< info string
	std::string info;

private: // methods

};

#endif
