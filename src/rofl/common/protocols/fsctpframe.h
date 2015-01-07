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

#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "../fframe.h"
#include "../caddress.h"

namespace rofl
{

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
			caddress_in4 const& ip_src,
			caddress_in4 const& ip_dst,
			uint8_t ip_proto,
			uint16_t length);

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
	 */
	uint16_t
	get_sport() const;

	/**
	 */
	void
	set_sport(uint16_t port);

	/**
	 */
	uint16_t
	get_dport() const;

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

public:

	friend std::ostream&
	operator<< (std::ostream& os, fsctpframe const& frame) {
		os << dynamic_cast<fframe const&>( frame );
		os << indent(2) << "<fsctpframe ";
			os << "dport:" << (int)frame.get_dport() << " ";
			os << "sport:" << (int)frame.get_sport() << " ";
		os << ">" << std::endl;
		return os;
	};
};

}; // end of namespace

#endif /* FSCTPFRAME_H_ */
