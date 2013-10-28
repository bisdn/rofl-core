/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FCAPWAPFRAME_H
#define FCAPWAPFRAME_H 1

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
#include "../cvastring.h"

namespace rofl
{

#define CAPWAP_HEADER_LEN    8

#define CAPWAP_PREAMBLE_MASK htobe32(0xFF000000)
#define CAPWAP_HLEN_SHIFT    19
#define CAPWAP_HLEN_MASK     htobe32(0x00F80000)
#define CAPWAP_RID_SHIFT     14
#define CAPWAP_RID_MASK      htobe32(0x0007C000)
#define CAPWAP_WBID_SHIFT     9
#define CAPWAP_WBID_MASK     htobe32(0x00003E00)
#define CAPWAP_F_MASK        htobe32(0x000001FF)

#define CAPWAP_F_TYPE        htobe32(0x00000100)
#define CAPWAP_F_TYPE_SHIFT   8
#define CAPWAP_F_FRAG        htobe32(0x00000080)
#define CAPWAP_F_LASTFRAG    htobe32(0x00000040)
#define CAPWAP_F_WSI         htobe32(0x00000020)
#define CAPWAP_F_RMAC        htobe32(0x00000010)

/** pppoe mixin for cpacket
 *
 */
class fcapwapframe : public fframe {
public: // static

	/* Capwap constants and definitions */
	// Capwap ethernet types
	enum capwap_wbid_t {
		CAPWAP_WB_802_11 = 1,
		CAPWAP_WB_NOT_SET = 0x80
	};

	enum capwap_f_type_t {
		CAPWAP_F_TYPE_802_3  = 0,
		CAPWAP_F_TYPE_NATIVE  = 0x0100
	};

	enum capwap_payload_t {
		CAPWAP_802_3_PAYLOAD = 1,
		CAPWAP_802_11_PAYLOAD = 2
	};

	static const uint8_t CAPWAP_RID_NOT_SET = 0x80;

	static const uint8_t CAPWAP_HDR_LEN = 8;
	static const uint16_t CAPWAP_DATA_PORT = 5247;

public: // methods


	/** constructor
	 *
	 */
	fcapwapframe(
			uint8_t* data,
			size_t datalen);


	/** destructor
	 *
	 */
	virtual
	~fcapwapframe();


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

	/**
	 */
	void
	set_capwap_rid(uint8_t rid);

	/**
	 */
	uint8_t
	get_capwap_rid();

	/**
	 */
	void
	set_capwap_flags(uint16_t flags);

	/**
	 */
	uint16_t
	get_capwap_flags();

	/**
	 */
	void
	set_capwap_wbid(uint8_t wbid);

	/**
	 */
	uint8_t
	get_capwap_wbid();

        /**
         */
        uint16_t
        get_capwap_length();

	/**
	 */
	void
	set_capwap_hlen(uint8_t hlen);

	/**
	 */
	uint8_t
	get_capwap_hlen();

public: // data structures

private: // methods


private: // data structures

	// header data
	uint8_t *capwap_hdr;

	// payload data
	uint8_t *capwapdata;

	// ppp data length
	size_t capwapdatalen;

	//< info string
	std::string info;
};

}; // end of namespace

#endif
