/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


/*
 * fsctpframe.h
 *
 *  Created on: 07.12.2012
 *      Author: andreas
 */

#ifndef FSCTPFRAME_H
#define FSCTPFRAME_H 1


#include "../fframe.h"
#include "../caddress.h"

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

#include "../cvastring.h"

class eSctpFrameBase 		: public eFrameBase {};
class eSctpFrameTooShort	: public eSctpFrameBase, public eFrameInvalidSyntax {};
class eSctpFrameInvalChksum	: public eSctpFrameBase {};


class fsctpframe : public fframe {
public:
	/* TCP constants and definitions */
	struct sctp_hdr_t {
		uint16_t sport;
		uint16_t dport;
		uint32_t verification_tag;
		uint32_t checksum;
		uint8_t data[0];
	} __attribute__((packed));

	/* SCTP chunk header */
	struct sctp_chunk_hdr_t {
		uint16_t type;
		uint16_t len;
		uint32_t value;
	} __attribute__((packed));

	/* for SCTP checksum calculation */
	struct ip_pseudo_hdr_t {
		uint32_t src;
		uint32_t dst;
		uint8_t reserved;
		uint8_t proto;
		uint16_t len;
	} __attribute__((packed));

	enum sctp_ip_proto_t {
		SCTP_IP_PROTO = 132,
	};

public:


	/** constructor
	 *
	 */
	fsctpframe(
			uint8_t *_data,
			size_t _datalen);


	/** destructor
	 *
	 */
	virtual
	~fsctpframe();


	/** calculate TCP header checksum
	 *
	 */
	void
	sctp_calc_checksum(
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
	validate(uint16_t total_len = 0) throw (eSctpFrameTooShort);

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

public: // data structures

	// pointer to tcp header
	struct sctp_hdr_t *sctp_hdr;

	// udp payload
	uint8_t *data;

	// udp payload length
	size_t datalen;

private: // data structures

	//< info string
	std::string info;

private: // methods

};




#endif /* FSCTPFRAME_H_ */
