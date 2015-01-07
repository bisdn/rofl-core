/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FARPV4FRAME_H
#define FARPV4FRAME_H 1

#include <inttypes.h>
#include <endian.h>
#ifndef be32toh
	#include "../endian_conversion.h"
#endif

#include "../fframe.h"
#include "../caddress.h"

namespace rofl
{

class eARPv4FrameBase			: public eFrameInval {};
class eARPv4FrameTooShort		: public eARPv4FrameBase, public eFrameInvalidSyntax {};


class farpv4frame : public fframe {
	static const unsigned int ETH_ADDR_LEN = 6;

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
		uint8_t dl_src[ETH_ADDR_LEN]; 	// source MAC address
		uint32_t ip_src;            	// source IP address
		uint8_t dl_dst[ETH_ADDR_LEN]; 	// destination MAC address
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
	get_hw_addr_type() const;

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
	get_prot_addr_type() const;

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
	get_hw_addr_size() const;

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
	get_prot_hw_addr_size() const;

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
	get_opcode() const;

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
	get_dl_src() const;

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
	get_dl_dst() const;

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
			caddress_in4 const& addr);

	/**
	 *
	 */
	caddress_in4
	get_nw_src() const;

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
			caddress_in4 const& addr);

	/**
	 *
	 */
	caddress_in4
	get_nw_dst() const;


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



public: // data structures

	// pointer to pppoe header
	struct arpv4_hdr_t *arp_hdr;

public:

	friend std::ostream&
	operator<< (std::ostream& os, farpv4frame const& frame) {
		os << dynamic_cast<fframe const&>( frame );
		os << indent(2) << "<farpv4frame ";
			os << "opcode:" << (int)frame.get_opcode() << " ";
			os << "hw-addr-type:" << (int)frame.get_hw_addr_type() << " ";
			os << "hw-addr-size:" << (int)frame.get_hw_addr_size() << " ";
			os << "prot-addr-type:" << (int)frame.get_prot_addr_type() << " ";
			os << "prot-addr-size:" << (int)frame.get_prot_hw_addr_size() << " ";
			os << "dl-dst:" << frame.get_dl_dst() << " ";
			os << "dl-src:" << frame.get_dl_src() << " ";
			os << "nw-dst:" << frame.get_nw_dst() << " ";
			os << "nw-src:" << frame.get_nw_src() << " ";
		os << ">" << std::endl;
		return os;
	};
};

}; // end of namespace

#endif
