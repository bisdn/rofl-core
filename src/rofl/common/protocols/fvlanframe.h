/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FVLANFRAME_H
#define FVLANFRAME_H 1

#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "../fframe.h"
#include "../cmacaddr.h"
#include "../cvastring.h"

namespace rofl
{

class eVlanFrameBase 			: public eFrameBase {};
class eVlanFrameInvalidSyntax 	: public eVlanFrameBase, public eFrameInvalidSyntax {}; // invalid syntax
class eVlanFrameTooShort		: public eVlanFrameInvalidSyntax {};

/** VLAN frame
 *
 *
 */
class fvlanframe : public fframe {
public: // static

	/* Ethernet constants and definitions */

	// VLAN ethernet types
	enum vlan_ether_t {
		VLAN_CTAG_ETHER = 0x8100,
		VLAN_STAG_ETHER = 0x88a8,
		VLAN_ITAG_ETHER = 0x88e7,
	};

	// VLAN header
	struct vlan_hdr_t {
		// tag control identifier (TCI)
		uint8_t byte0;
		uint8_t byte1;
		//uint16_t hdr;			// vid + cfi + pcp
		uint16_t dl_type;  		// ethernet type
	} __attribute__((packed));

#if 0
#if __BYTE_ORDER == __LITTLE_ENDIAN
		uint16_t vid : 12;      // VLAN ID
		uint16_t cfi : 1;       // TODO: handle canonical order appropriately
		uint16_t pcp : 3;       // VLAN priority code point
#elif __BYTE_ORDER == __BIG_ENDIAN
		uint16_t pcp : 3;  		// TCI: PCP (priority code point)
		uint16_t cfi : 1;  		// TCI: CFI (canonical format indicator)
		uint16_t vid : 12; 		// TCI: VID (VLAN identifier)
#endif
#endif


public: // methods

	/** constructor
	 *
	 */
	fvlanframe(
			uint8_t* data,
			size_t datalen);


	/** constructor
	 *
	 */
	fvlanframe(
			size_t len = sizeof(struct fvlanframe::vlan_hdr_t));


	/** destructor
	 *
	 */
	virtual
	~fvlanframe();


public: // overloaded from fframe


	/** reset this frame to a new buffer pointing to at (data, datalen)
	 *
	 */
	virtual void
	reset(
			uint8_t *data, size_t datalen);


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
	validate(uint16_t total_len = 0) throw (eVlanFrameTooShort);

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
	 *
	 */
	void
	set_dl_vlan_id(uint16_t vid);

	/** get dl vlanid (if untagged, returns 0xffff)
	 *
	 */
	uint16_t
	get_dl_vlan_id() const;

	/**
	 *
	 */
	void
	set_dl_vlan_pcp(uint8_t pcp);

	/**
	 *
	 */
	uint8_t
	get_dl_vlan_pcp() const;

	/**
	 *
	 */
	void
	set_dl_vlan_cfi(bool cfi);

	/**
	 *
	 */
	bool
	get_dl_vlan_cfi() const;

	/** set dl type
	 *
	 */
	void
	set_dl_type(uint16_t dl_type) throw (eFrameInval);

	/** get dl type
	 *
	 */
	uint16_t
	get_dl_type() const;

public: // data structures

	// pointer to vlan header
	struct vlan_hdr_t *vlan_hdr;

private: // methods



private: // data structures

	std::string info;

};

}; // end of namespace

#endif
