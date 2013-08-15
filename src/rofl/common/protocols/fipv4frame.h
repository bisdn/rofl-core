/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FIPV4FRAME_H
#define FIPV4FRAME_H 1

#include <map>
#include <inttypes.h>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "../fframe.h"
#include "../caddress.h"
#include "../cvastring.h"

namespace rofl
{

// error classes
class eIPv4FrameBase : public eFrameBase {}; // base error class for cpppoepacket
class eIPv4FrameTagNotFound : public eIPv4FrameBase {}; // pppoe tag not found
class eIPv4FrameInvalidSyntax : public eIPv4FrameBase {}; // frame has invalid syntax
class eIPv4FrameInval : public eIPv4FrameBase {}; // invalid parameter


/** pppoe mixin for cpacket
 *
 */
class fipv4frame : public fframe {
public: // static

	/* IPv4 constants and definitions */
	// IPv4 ethernet types
	enum ipv4_ether_t {
		IPV4_ETHER = 0x0800,
	};

	enum ipv4_ip_proto_t {
		IPV4_IP_PROTO = 4,
	};

	// IPv4 header
	struct ipv4_hdr_t {
		uint8_t ihlvers;        // IP header length in 32bit words,
								// TODO: check for options and padding
		uint8_t tos;
		uint16_t length;
		uint16_t ident;
		uint16_t offset_flags;
		uint8_t ttl;
		uint8_t proto;
		uint16_t checksum;
		uint32_t src;
		uint32_t dst;
		uint8_t data[0];
	} __attribute__((packed));

	enum ipv4_flag_t {
		bit_reserved 		= (1 << 0),
		bit_dont_fragment 	= (1 << 1),
		bit_more_fragments	= (1 << 2),
	};

#if 0
	// IPv4 header
	struct ipv4_hdr_t {
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		uint8_t ihl : 4;        // IP header length in 32bit words,
								// TODO: check for options and padding
		uint8_t version : 4;

	#elif __BYTE_ORDER == __BIG_ENDIAN
		uint8_t version : 4;
		uint8_t ihl : 4;        // IP header length in 32bit words,
								// TODO: check for options and padding
	#endif

		uint8_t tos;
		uint16_t length;
		uint16_t ident;
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		uint16_t offset : 13;
		uint16_t flags  :  3;
	#elif __BYTE_ORDER == __BIG_ENDIAN
		uint16_t flags  :  3;
		uint16_t offset : 13;
	#endif
		uint8_t ttl;
		uint8_t proto;
		uint16_t checksum;
		uint32_t src;
		uint32_t dst;
		uint8_t data[0];
	} __attribute__((packed));
#endif

#if 0
	enum ip_flag_t {
		IP_FLAG_RSVD = (1 << 1),
		IP_FLAG_DONT_FRAGMENT = (1 << 2),
		IP_FLAG_MORE_FRAGMENTS = (1 << 3),
	};
#endif

	enum ip_proto_t {
		IPPROTO_ICMP = 1,
		IPPROTO_TCP = 6,
		IPPROTO_UDP = 17,
	};
	/* IPv4 definitions */

public: // methods


	/** constructor
	 *
	 */
	fipv4frame(
			uint8_t* data,
			size_t datalen);


	/** destructor
	 *
	 */
	virtual
	~fipv4frame();


#if 0
	/** insert new pppoe tag
	 *
	 */
	struct fipv4frame::ipv4_option_hdr_t*
	option_insert(enum fpppoeframe::pppoe_tag_t type, char *fmt, ...);

	/** find existing tag
	 *
	 */
	struct fipv4frame::ipv4_option_hdr_t*
	option_find(enum fpppoeframe::pppoe_tag_t) throw (ePPPoEFrameTagNotFound);
#endif

	/** calculate IPv4 header checksum
	 *
	 */
	void
	ipv4_calc_checksum();

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
	validate(uint16_t total_len = 0) throw (eFrameInvalidSyntax);

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
	 */
	void
	set_ipv4_ihl(uint8_t ihl = 5);

	/**
	 */
	uint8_t
	get_ipv4_ihl();

	/**
	 */
	void
	set_ipv4_version(uint8_t version = 4);

	/**
	 */
	uint8_t
	get_ipv4_version();

	/** src in network-byte-order
	 */
	void
	set_ipv4_src(uint32_t src);

	/**
	 */
	void
	set_ipv4_src(caddress const& src) throw (eIPv4FrameInval);

	/**
	 */
	caddress
	get_ipv4_src();

	/** dst in network-byte-order
	 */
	void
	set_ipv4_dst(uint32_t dst);

	/**
	 */
	void
	set_ipv4_dst(caddress const& dst) throw (eIPv4FrameInval);

	/**
	 */
	caddress
	get_ipv4_dst();

	/**
	 */
	void
	set_ipv4_dscp(uint8_t dscp);

	/**
	 */
	uint8_t
	get_ipv4_dscp();

	/**
	 */
	void
	set_ipv4_ecn(uint8_t ecn);

	/**
	 */
	uint8_t
	get_ipv4_ecn();

	/**
	 */
	void
	set_ipv4_ttl(uint8_t ttl);

	/**
	 */
	uint8_t
	get_ipv4_ttl();

	/**
	 */
	void
	dec_ipv4_ttl();

	/**
	 */
	void
	set_ipv4_proto(uint8_t proto);

	/**
	 */
	uint8_t
	get_ipv4_proto();

        /**
         */
        uint16_t
        get_ipv4_length();

	/**
	 */
	void
	set_ipv4_length(uint16_t length);

	/**
	 */
	bool
	has_DF_bit_set();

	/**
	 */
	void
	set_DF_bit();

	/**
	 */
	void
	clear_DF_bit();

	/**
	 */
	bool
	has_MF_bit_set();

	/**
	 */
	void
	set_MF_bit();

	/**
	 */
	void
	clear_MF_bit();

public: // data structures

	// pointer to pppoe header
	struct ipv4_hdr_t *ipv4_hdr;

	// map of options
	//std::map<enum fipv4frame::ipv4_option_t, struct fipv4frame::ipv4_option_hdr_t*> ipv4_options;

	// payload data
	uint8_t *ipv4data;

	// ppp data length
	size_t ipv4datalen;

private: // methods


private: // data structures

	//< info string
	std::string info;
};

}; // end of namespace

#endif
