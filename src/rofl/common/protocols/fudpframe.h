/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FUDPFRAME_H
#define FUDPFRAME_H 1

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



class fudpframe : public fframe {
public:

	/* UDP constants and definitions */
	struct udp_hdr_t {
		uint16_t sport;
		uint16_t dport;
		uint16_t length;
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

	enum udp_ip_proto_t {
		UDP_IP_PROTO = 17,
	};

public:


	/** constructors
	 *
	 */
	fudpframe(
			uint8_t *data,
			size_t datalen);


	/** destructors
	 *
	 */
	virtual
	~fudpframe();


	/** calculate UDP header checksum
	 *
	 */
	void
	udp_calc_checksum(
			caddress const& ip_src,
			caddress const& ip_dst,
			uint8_t ip_proto,
			uint16_t length);


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
	 */
	uint16_t
	get_sport();

	/**
	 */
	void
	set_sport(uint16_t port);

	/**
	 */
	uint16_t
	get_dport();

	/**
	 */
	void
	set_dport(uint16_t port);

	/**
	 */
	uint16_t
	get_length();

	/**
	 */
	void
	set_length(uint16_t length);

public: // data structures

	// pointer to udp header
	struct udp_hdr_t *udp_hdr;

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
