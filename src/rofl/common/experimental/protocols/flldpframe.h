/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FLLDPFRAME_H
#define FLLDPFRAME_H 1

#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "../openflow/openflow.h"
#include "../fframe.h"
#include "../cmemory.h"
#include "../cpacket.h"
#include "../cvastring.h"

#include "clldplist.h"

namespace rofl
{

class eLldpFrameInvalidSyntax : public eFrameInvalidSyntax {}; // invalid syntax

/** LLDP frame
 *
 *
 */
class flldpframe : public fframe {
public: // static

	static void test();

	/* Ethernet constants and definitions */

#define DEFAULT_LLDP_FRAME_SIZE 128

	// VLAN ethernet types
	enum lldp_ether_t {
		LLDP_ETHER = 0x88CC,
	};

	struct lldp_hdr_t {
		uint8_t body[0];
	};

public: // methods


	/** constructor
	 *
	 */
	flldpframe(
			uint8_t* data,
			size_t datalen);


	/** constructor
	 *
	 */
	flldpframe(
			size_t len = DEFAULT_LLDP_FRAME_SIZE);


	/** destructor
	 *
	 */
	virtual
	~flldpframe();


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

#if 0
	/** pack LLDP frame: coflist<clldpattr> => C-structure
	 *
	 */
	size_t
	pack(cpacket& packet);
#endif

	size_t
	pack(uint8_t *mem, size_t memlen) throw (eLLDPInval);

	/** unpack LLDP frame: C-structure => coflist<clldpattr>
	 *
	 */
	void
	unpack(uint8_t *mem, size_t memlen) throw (eLLDPInval);

	/**
	 *
	 */
	size_t
	length();

public: // data structures

	clldptlv::lldp_tlv_hdr_t *lldp_hdr; // start of packed LLDP tlvs
	clldplist tlvs; // the list of LLDP tlvs

private: // data structures

	std::string info;
};

}; // end of namespace

#endif
