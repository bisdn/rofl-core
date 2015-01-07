/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FTCPFRAME_H
#define FTCPFRAME_H 1

#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "../fframe.h"
#include "../caddress.h"

namespace rofl
{

class eTcpFrameBase 		: public eFrameBase {};
class eTcpFrameTooShort		: public eTcpFrameBase, public eFrameInvalidSyntax {};
class eTcpFrameInvalChksum	: public eTcpFrameBase {};


class ftcpframe : public fframe {
public:
	/* TCP constants and definitions */
	struct tcp_hdr_t {
		uint16_t sport;
		uint16_t dport;
		uint32_t seqno;
		uint32_t ackno;
#ifdef __BIG_ENDIAN
		uint16_t offset   : 4; // =5 => 5 32bit words (= 20 bytes), >5 => options appended
		uint16_t reserved : 3;
		uint16_t ns       : 1;
		/* byte */
		uint16_t cwr      : 1;
		uint16_t ece	  : 1;
		uint16_t urg	  : 1;
		uint16_t ack	  : 1;
		uint16_t psh	  : 1;
		uint16_t rst	  : 1;
		uint16_t syn	  : 1;
		uint16_t fin	  : 1;
#elif __LITTLE_ENDIAN
		uint16_t cwr      : 1;
		uint16_t ece	  : 1;
		uint16_t urg	  : 1;
		uint16_t ack	  : 1;
		uint16_t psh	  : 1;
		uint16_t rst	  : 1;
		uint16_t syn	  : 1;
		uint16_t fin	  : 1;
		/* byte */
		uint16_t offset   : 4; // =5 => 5 32bit words (= 20 bytes), >5 => options appended
		uint16_t reserved : 3;
		uint16_t ns       : 1;
#endif
		uint16_t wnd;
		uint16_t checksum;
		uint16_t urgent;
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
		TCP_IP_PROTO = 6,
	};

public:


	/** constructor
	 *
	 */
	ftcpframe(
			uint8_t *_data,
			size_t _datalen);


	/** destructor
	 *
	 */
	virtual
	~ftcpframe();


	/** calculate TCP header checksum
	 *
	 */
	void
	tcp_calc_checksum(
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
	struct tcp_hdr_t *tcp_hdr;

	// udp payload
	uint8_t *data;

	// udp payload length
	size_t datalen;

public:

	friend std::ostream&
	operator<< (std::ostream& os, ftcpframe const& frame) {
		os << dynamic_cast<fframe const&>( frame );
		os << indent(2) << "<ftcpframe ";
			os << "sport:" << (int)frame.get_sport() << " ";
			os << "dport:" << (int)frame.get_dport() << " ";
		os << ">" << std::endl;
		return os;
	};
};

}; // end of namespace

#endif
