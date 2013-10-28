/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FIEEE80211FRAME_H
#define FIEEE80211FRAME_H 1

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
#include "../cmacaddr.h"
#include "../cvastring.h"

namespace rofl
{

/* IEEE 802.11 defines */

#define WLAN_FC_PVER        0x0003
#define WLAN_FC_TYPE        0x000A
#define WLAN_FC_SUBTYPE     0x00F0
#define WLAN_FC_TODS        0x0100
#define WLAN_FC_FROMDS      0x0200
#define WLAN_FC_MOREFRAG    0x0400
#define WLAN_FC_RETRY       0x0800
#define WLAN_FC_PWRMGT      0x1000
#define WLAN_FC_MOREDATA    0x2000
#define WLAN_FC_ISWEP       0x4000
#define WLAN_FC_ORDER       0x8000

#define WLAN_FC_TYPE_MGMT       0
#define WLAN_FC_TYPE_CTRL       1
#define WLAN_FC_TYPE_DATA       2

#define WLAN_FC_STYPE_QOS   0x08
#define WLAN_FC_STYPE_CONTROL_WRAPPER       7

/** ieee80211 mixin for cpacket
 *
 */
class fieee80211frame : public fframe {
public: // static

	static const uint8_t IEEE80211_HDR_LEN = 24;

public: // methods


	/** constructor
	 *
	 */
	fieee80211frame(
			uint8_t* data,
			size_t datalen);


	/** destructor
	 *
	 */
	virtual
	~fieee80211frame();


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

	/** initialize (set eth_hdr, ieee80211_hdr)
	 *
	 */
	virtual void
	initialize();

#if 0
	/** insert payload
	 *
	 */
	virtual void
	payload_insert(
			uint8_t *data, size_t datalen) throw (eFrameOutOfRange);
#endif

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

	/** Frame Control field
	 */
	void
	set_ieee80211_fc(uint16_t fc);

	/**
	 */
	uint16_t
	get_ieee80211_fc();

	/**  Type Field
	 */
	void
	set_ieee80211_type(uint8_t type);

	/**
	 */
	uint8_t
	get_ieee80211_type();

	/**  SubType Field
	 */
	void
	set_ieee80211_subtype(uint8_t subtype);

	/**
	 */
	uint8_t
	get_ieee80211_subtype();

	/** Frame Control from/to DS bits
	 */
	void
	set_ieee80211_direction(uint8_t direction);

	/**
	 */
	uint8_t
	get_ieee80211_direction();


	/** set address
	 *
	 */
	void
	set_ieee80211_address_1(cmacaddr const& address) throw (eFrameInval);

	/** get address
	 *
	 */
	cmacaddr
	get_ieee80211_address_1() const;

	/** set address
	 *
	 */
	void
	set_ieee80211_address_2(cmacaddr const& address) throw (eFrameInval);

	/** get address
	 *
	 */
	cmacaddr
	get_ieee80211_address_2() const;

	/** set address
	 *
	 */
	void
	set_ieee80211_address_3(cmacaddr const& address) throw (eFrameInval);

	/** get address
	 *
	 */
	cmacaddr
	get_ieee80211_address_3() const;

        /**
         */
        uint16_t
        get_ieee80211_length();

	/**
	 */
	uint8_t
	get_ieee80211_hlen();

public: // data structures

private: // methods


private: // data structures

	// header data
	uint8_t *ieee80211_hdr;

	// payload data
	uint8_t *ieee80211data;

	// ppp data length
	size_t ieee80211datalen;

	//< info string
	std::string info;

	/** Frame Control order bit
	 */
	uint8_t
	get_ieee80211_order();

};

}; // end of namespace

#endif
