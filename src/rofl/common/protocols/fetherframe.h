/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FETHERFRAME_H
#define FETHERFRAME_H 1

#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "../fframe.h"
#include "../caddress.h"
#include "../cvastring.h"

namespace rofl
{

class eEtherFrameInvalidSyntax : public eFrameInvalidSyntax {}; // invalid syntax

/** Ethernet frame
 *
 *
 */
class fetherframe : public fframe {
	static const unsigned int ETH_ADDR_LEN = 6;

public: // static

#define DEFAULT_ETHER_FRAME_SIZE 1518

	/* Ethernet constants and definitions */

	// Ethernet II header
	struct eth_hdr_t {
		uint8_t dl_dst[ETH_ADDR_LEN];
		uint8_t dl_src[ETH_ADDR_LEN];
		uint16_t dl_type;
		uint8_t data[0];
	} __attribute__((packed));
	
	//Ethernet LLC header
	struct eth_llc_hdr_t {
		uint8_t dl_dst[ETH_ADDR_LEN];
		uint8_t dl_src[ETH_ADDR_LEN];
		uint16_t dl_len;
		uint8_t dl_dsap;
		uint8_t dl_ssap;
		uint8_t dl_control;
		uint8_t dl_vendor_code[3];
		uint16_t dl_type;
		uint8_t data[0];
	}__attribute__((packed));

public: // methods


	/** constructor
	 *
	 */
	fetherframe(
			uint8_t* data,
			size_t datalen);


	/** constructor
	 *
	 */
	fetherframe(
			size_t len = DEFAULT_ETHER_FRAME_SIZE);


	/** destructor
	 *
	 */
	virtual
	~fetherframe();


	/** reset this frame to a new buffer pointing to at (data, datalen)
	 *
	 */
	virtual void
	reset(
			uint8_t *data, size_t datalen);


	/** set dl src
	 *
	 */
	void
	set_dl_src(cmacaddr const& dl_src) throw (eFrameInval);

	/** get dl src
	 *
	 */
	cmacaddr
	get_dl_src() const;

	/** set dl dst
	 *
	 */
	void
	set_dl_dst(cmacaddr const& dl_dst) throw (eFrameInval);

	/** get dl dst
	 *
	 */
	cmacaddr
	get_dl_dst() const;

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
	
	bool
	is_llc_frame() const;

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



public: // data structures

	// pointer to ethernet header
	struct eth_hdr_t *eth_hdr;
	struct eth_llc_hdr_t *eth_llc_hdr;

public:

	friend std::ostream&
	operator<< (std::ostream& os, fetherframe const& frame) {
		os << dynamic_cast<fframe const&>( frame );
		os << rofl::indent(2) << "<fetherframe " << std::endl;
			os << rofl::indent(4) << "<dl-dst >" << std::endl;
			{ rofl::indent i(6); os << frame.get_dl_dst(); }
			os << rofl::indent(4) << "<dl-src >" << std::endl;
			{ rofl::indent i(6); os << frame.get_dl_src(); }
			os << "dl-type:0x" << std::hex << (int)frame.get_dl_type() << std::dec << " ";
		os << ">" << std::endl;
		return os;
	};
};

}; // end of namespace

#endif
