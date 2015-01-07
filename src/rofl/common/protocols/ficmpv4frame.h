/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FICMPV4FRAME_H
#define FICMPV4FRAME_H 1

#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "../fframe.h"
#include "../caddress.h"

namespace rofl
{

class eICMPv4FrameBase 				: public eFrameBase {};
class eICMPv4FrameInvalidSyntax 	: public eICMPv4FrameBase, public eFrameInvalidSyntax {};
class eICMPv4FrameTooShort			: public eICMPv4FrameInvalidSyntax {};



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
	icmpv4_calc_checksum(uint16_t length);


public: // overloaded from fframe

	/** returns boolean value indicating completeness of the packet
	 */
	virtual bool
	complete() const;

	/** returns the number of bytes this packet expects from the socket next
	 */
	virtual size_t
	need_bytes() const;

	/** validate (frame structure)
	 *
	 */
	virtual void
	validate(uint16_t total_len = 0) const;

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

public:

	/**
	 *
	 */
	uint8_t
	get_icmp_code() const;

	/**
	 *
	 */
	void
	set_icmp_code(uint8_t code);

	/**
	 *
	 */
	uint8_t
	get_icmp_type() const;

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

public:

	friend std::ostream&
	operator<< (std::ostream& os, ficmpv4frame const& frame) {
		os << dynamic_cast<fframe const&>( frame );
		os << indent(2) << "<ficmpv4frame ";
			os << "code:" << (int)frame.get_icmp_code() << " ";
			os << "type:" << (int)frame.get_icmp_type() << " ";
		os << ">" << std::endl;
		return os;
	};
};

}; // end of namespace

#endif
