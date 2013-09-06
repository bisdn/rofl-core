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
#include "../cmacaddr.h"
#include "../cvastring.h"

namespace rofl
{

class eEtherFrameInvalidSyntax : public eFrameInvalidSyntax {}; // invalid syntax

/** Ethernet frame
 *
 *
 */
class fetherframe : public fframe {
public: // static

#define DEFAULT_ETHER_FRAME_SIZE 1518

	/* Ethernet constants and definitions */

	// Ethernet II header
	struct eth_hdr_t {
		uint8_t dl_dst[OFP_ETH_ALEN];
		uint8_t dl_src[OFP_ETH_ALEN];
		uint16_t dl_type;
		uint8_t data[0];
	} __attribute__((packed));

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



public: // data structures

	// pointer to ethernet header
	struct eth_hdr_t *eth_hdr;

private: // methods



private: // data structures

	std::string info;

};

}; // end of namespace

#endif
