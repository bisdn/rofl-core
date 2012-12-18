/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FARPV4FRAME_H
#define FARPV4FRAME_H 1

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include "../fframe.h"
#include "../caddress.h"
#include "../cmacaddr.h"
#include "../cvastring.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../openflow/openflow12.h"
#include <endian.h>

#ifndef be32toh
#include "../endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

namespace rofl
{

class eARPv4FrameBase			: public eFrameInval {};
class eARPv4FrameTooShort		: public eARPv4FrameBase, public eFrameInvalidSyntax {};


class farpv4frame : public fframe {
public: // static

	/* ARPv4 constants and definitions */
	enum arpv4_ether_t {
		ARPV4_ETHER = 0x0806,
	};

	enum arpv4_opcodes_t {
		ARPV4_OPCODE_REQUEST = 0x01,
		ARPV4_OPCODE_REPLY = 0x02,
	};

	struct arpv4_hdr_t {
		uint16_t ar_hrd;            	// hardware address format
		uint16_t ar_pro;            	// protocol address format
		uint8_t ar_hln;             	// hardware address length
		uint8_t ar_pln;             	// protocol address length
		uint16_t ar_op;             	// ARP opcode
		uint8_t dl_src[OFP_ETH_ALEN]; 	// source MAC address
		uint32_t ip_src;            	// source IP address
		uint8_t dl_dst[OFP_ETH_ALEN]; 	// destination MAC address
		uint32_t ip_dst;            	// destination IP address
	} __attribute__((packed));
	/* ARPv4 definitions */

#define DEFAULT_ARPV4_FRAME_SIZE sizeof(struct arpv4_hdr_t)

public:


	/** constructor
	 *
	 */
	farpv4frame(
			uint8_t *_data,
			size_t _datalen);


	/** constructor for creating new frames
	 *
	 */
	farpv4frame(
			size_t len = DEFAULT_ARPV4_FRAME_SIZE);


	/** destructor
	 *
	 */
	virtual
	~farpv4frame();

	/**
	 *
	 */
	void
	set_hw_addr_type(
			uint16_t hwaddrtype);

	/**
	 *
	 */
	uint16_t
	get_hw_addr_type();

	/**
	 *
	 */
	void
	set_prot_addr_type(
			uint16_t prothwaddrtype);

	/**
	 *
	 */
	uint16_t
	get_prot_addr_type();

	/**
	 *
	 */
	void
	set_hw_addr_size(
			uint8_t size);

	/**
	 *
	 */
	uint8_t
	get_hw_addr_size();

	/**
	 *
	 */
	void
	set_prot_hw_addr_size(
			uint8_t size);

	/**
	 *
	 */
	uint8_t
	get_prot_hw_addr_size();

	/**
	 *
	 */
	void
	set_opcode(
			uint16_t operation);

	/**
	 *
	 */
	uint16_t
	get_opcode();

	/**
	 *
	 */
	void
	set_dl_src(
			cmacaddr const& dl_src);

	/**
	 *
	 */
	cmacaddr
	get_dl_src();

	/**
	 *
	 */
	void
	set_dl_dst(
			cmacaddr const& dl_dst);

	/**
	 *
	 */
	cmacaddr
	get_dl_dst();

	/** src in network-byte-order
	 *
	 */
	void
	set_nw_src(
			uint32_t src);

	/**
	 *
	 */
	void
	set_nw_src(
			caddress const& addr);

	/**
	 *
	 */
	caddress
	get_nw_src();

	/** dst in network-byte-order
	 *
	 */
	void
	set_nw_dst(
			uint32_t dst);

	/**
	 *
	 */
	void
	set_nw_dst(
			caddress const& addr);

	/**
	 *
	 */
	caddress
	get_nw_dst();


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
	validate(uint16_t total_len = 0) throw (eARPv4FrameTooShort);

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



public: // data structures

	// pointer to pppoe header
	struct arpv4_hdr_t *arp_hdr;

private: // methods


private: // data structures

	//< info string
	std::string info;

};

}; // end of namespace

#endif
