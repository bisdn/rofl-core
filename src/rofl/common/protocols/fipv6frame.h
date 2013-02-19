/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FIPV6FRAME_H
#define FIPV6FRAME_H 1

#include <map>

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
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

// error classes
class eIPv6FrameBase 			: public eFrameBase {}; 	// base error class for cpppoepacket
class eIPv6FrameTagNotFound 	: public eIPv6FrameBase {}; // pppoe tag not found
class eIPv6FrameInvalidSyntax 	: public eIPv6FrameBase {}; // frame has invalid syntax
class eIPv6FrameInval 			: public eIPv6FrameBase {}; // invalid parameter



class fipv6ext : public fframe {

public: // static

	// IPv6 generic extension header
	struct ipv6_ext_hdr_t {
		uint8_t nxthdr;
		uint8_t len;
		uint8_t data[0];
	} __attribute__((packed));

public:

	struct ipv6_ext_hdr_t *exthdr;

public:
	// default constructor
	fipv6ext(struct ipv6_ext_hdr_t* hdr, size_t hdrlen) throw (eIPv6FrameInval) :
		fframe((uint8_t*)hdr, hdrlen),
		exthdr((struct ipv6_ext_hdr_t*)soframe())
	{
		if (hdrlen < 8) {
			throw eIPv6FrameInval();
		}
	};
	// default constructor
	fipv6ext(uint8_t* hdr, size_t hdrlen) throw (eIPv6FrameInval) :
		fframe(hdr, hdrlen),
		exthdr((struct ipv6_ext_hdr_t*)soframe())
	{
		if (hdrlen < 8) {
			throw eIPv6FrameInval();
		}
	};
	// virtual destructor
	virtual
	~fipv6ext() {
	};
	// copy constructor
	fipv6ext(fipv6ext const& ipv6ext) : fframe(ipv6ext.framelen()) {
		*this = ipv6ext;
	};
	// assignment operator
	fipv6ext& operator= (fipv6ext const& ipv6ext) {
		if (this == &ipv6ext)
			return *this;
		fframe::operator= (ipv6ext);
		exthdr = (struct ipv6_ext_hdr_t*)soframe();
		return *this;
	};
};


/** pppoe mixin for cpacket
 *
 */
class fipv6frame : public fframe {

private: // data structures

	std::string 			 info;			//< info string

public: // static

	#define IPV6_ADDR_LEN	16
	#define IPV6_VERSION 	 6

	/* ipv6 constants and definitions */
	// ipv6 ethernet types
	enum ipv6_ether_t {
		IPV6_ETHER = 0x86dd,
	};

	// IPv6 header
	struct ipv6_hdr_t {
		uint8_t bytes[4];      	// version + tc + flow-label
		uint16_t payloadlen;
		uint8_t nxthdr;
		uint8_t hoplimit;
		uint8_t src[IPV6_ADDR_LEN];
		uint8_t dst[IPV6_ADDR_LEN];
		uint8_t data[0];
	} __attribute__((packed));

	enum ip_proto_t {
		IPPROTO_IPV6_HOPOPT 		= 0,
		IPPROTO_ICMP 				= 1,
		IPPROTO_TCP 				= 6,
		IPPROTO_UDP 				= 17,
		IPPROTO_IPV6 				= 41,
		IPPROTO_IPV6_ROUTE			= 43,
		IPPROTO_IPV6_FRAG			= 44,
		IPPROTO_IPV6_ICMP			= 58,
		IPPROTO_IPV6_NONXT			= 59,
		IPPROTO_IPV6_OPTS			= 60,
		IPPROTO_IPV6_MIPV6			= 135,
	};
	/* ipv6 definitions */

public: // data structures

	struct ipv6_hdr_t 		*ipv6_hdr;		// pointer to pppoe header
	uint8_t 				*ipv6data;		// payload data
	size_t 					 ipv6datalen;	// ppp data length
	std::vector<fipv6ext> 	 ipv6exts;		// IPv6 extensions headers

public: // methods


	/** constructor
	 *
	 */
	fipv6frame(
			uint8_t* data,
			size_t datalen);


	/** destructor
	 *
	 */
	virtual
	~fipv6frame();


	/** calculate ipv6 header checksum
	 *
	 */
	void
	ipv6_calc_checksum();


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
	initialize() throw (eIPv6FrameInval);

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
	set_version(uint8_t version = 0b00000110);

	/**
	 */
	uint8_t
	get_version();

	/**
	 */
	void
	set_traffic_class(uint8_t tc);

	/**
	 */
	uint8_t
	get_traffic_class();

	/**
	 */
	void
	set_flow_label(uint32_t flabel);

	/**
	 */
	uint32_t
	get_flow_label();

	/**
	 */
	void
	set_payload_length(uint16_t len);

	/**
	 */
	uint16_t
	get_payload_length();

	/**
	 */
	void
	set_next_header(uint8_t nxthdr);

	/**
	 */
	uint8_t
	get_next_header();

	/**
	 */
	void
	set_hop_limit(uint8_t hops);

	/**
	 */
	uint8_t
	get_hop_limit();

	/** src in network-byte-order
	 */
	void
	set_ipv6_src(uint8_t *somem, size_t memlen) throw (eIPv6FrameInval);

	/** src in network-byte-order
	 */
	void
	set_ipv6_src(cmemory const& src) throw (eIPv6FrameInval);

	/**
	 */
	void
	set_ipv6_src(caddress const& src) throw (eIPv6FrameInval);

	/**
	 */
	caddress
	get_ipv6_src();

	/** dst in network-byte-order
	 */
	void
	set_ipv6_dst(uint8_t *somen, size_t memlen) throw (eIPv6FrameInval);

	/** dst in network-byte-order
	 */
	void
	set_ipv6_dst(cmemory const& dst) throw (eIPv6FrameInval);

	/**
	 */
	void
	set_ipv6_dst(caddress const& dst) throw (eIPv6FrameInval);

	/**
	 */
	caddress
	get_ipv6_dst();



private: // methods


};

}; // end of namespace

#endif
