/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FPPPOEFRAME_H
#define FPPPOEFRAME_H 1

#include <map>

#ifdef __cplusplus
extern "C" {
#endif

#include <endian.h>

#ifndef htobe16
#include "../endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif


#include "../fframe.h"
#include "../cpacket.h"
#include "../cvastring.h"
#include "cpppoelist.h"

namespace rofl
{

// error classes
class ePPPoEFrameBase 			: public eFrameBase {}; 		// base error class for cpppoepacket
class ePPPoEFrameTagNotFound 	        : public ePPPoEFrameBase {}; 		// pppoe tag not found
class ePPPoEFrameInvalidSyntax 	        : public ePPPoEFrameBase, public eFrameInvalidSyntax {}; 		// frame has invalid syntax
class ePPPoEFrameTooShort		: public ePPPoEFrameInvalidSyntax {};	// length is invalid for PPPoE frame
class ePPPoEFrameInvalVersion	        : public ePPPoEFrameInvalidSyntax {};	// invalid PPPoE version
class ePPPoEFrameInvalType		: public ePPPoEFrameInvalidSyntax {};	// invalid PPPoE type
class ePPPoEFrameInvalCode		: public ePPPoEFrameInvalidSyntax {};	// invalid PPPoE code
class ePPPoEPadiInvalSid		: public ePPPoEFrameInvalidSyntax {};	// invalid sid in PADI
class ePPPoEPadiNoSvcTag		: public ePPPoEFrameInvalidSyntax {};	// no svcname tag in PADI
class ePPPoEPadiInvalTags		: public ePPPoEFrameInvalidSyntax {}; 	// invalid tags size
class ePPPoEPadrInvalSid		: public ePPPoEFrameInvalidSyntax {};	// invalid sid in PADR
class ePPPoEPadrNoSvcTag		: public ePPPoEFrameInvalidSyntax {};	// no svcname tag in PADR
class ePPPoEPadsInvalSid		: public ePPPoEFrameInvalidSyntax {};	// invalid sid in PADS
class ePPPoEPadtInvalSid		: public ePPPoEFrameInvalidSyntax {}; 	// invalid sid in PADT


class cpacket;

/** pppoe mixin for cpacket
 *
 */
class fpppoeframe : public fframe {
public: // static

	/**
	 *
	 */
	static void
	test();

#define DEFAULT_PPPOE_FRAME_SIZE 1492

	/* PPPoE constants and definitions */
	enum pppoe_version_t {
		PPPOE_VERSION = 0x01,	// PPPoE version
	};

	enum pppoe_type_t {
		PPPOE_TYPE = 0x01,	// PPPoE type
	};

	// PPPoE ethernet types
	enum pppoe_ether_t {
		PPPOE_ETHER_DISCOVERY = 0x8863,
		PPPOE_ETHER_SESSION = 0x8864,
	};

	// PPPoE codes
	enum pppoe_code_t {
		PPPOE_CODE_SESSION_DATA = 0x00,
		PPPOE_CODE_PADO = 0x07,
		PPPOE_CODE_PADI = 0x09,
		PPPOE_CODE_PADR = 0x19,
		PPPOE_CODE_PADS = 0x65,
		PPPOE_CODE_PADT = 0xa7,
	};

	// PPPoE tags
	enum pppoe_tag_t {
		PPPOE_TAG_END_OF_LIST = 0x0000,
		PPPOE_TAG_SERVICE_NAME = 0x0101,
		PPPOE_TAG_AC_NAME = 0x0102,
		PPPOE_TAG_HOST_UNIQ = 0x0103,
		PPPOE_TAG_AC_COOKIE = 0x0104,
		PPPOE_TAG_VENDOR_SPECIFIC = 0x0105,
		PPPOE_TAG_RELAY_SESSION_ID = 0x0110,
		PPPOE_TAG_SERVICE_NAME_ERROR = 0x0201,
		PPPOE_TAG_AC_SYSTEM_ERROR = 0x0202,
		PPPOE_TAG_GENERIC_ERROR = 0x0203,
	};

	struct pppoe_hdr_t {
		uint8_t 	verstype;
		uint8_t 	code;
		uint16_t 	sessid;
		uint16_t 	length;
		uint8_t 	data[0];
	} __attribute__((packed));

	struct pppoe_tag_hdr_t {
		uint16_t type;
		uint16_t length;
		uint8_t data[0];
	};

	/* PPPoE stuff */

public: // methods


	/** constructor for parsing received pppoe frame
	 *
	 */
	fpppoeframe(
			uint8_t* data,
			size_t datalen);


	/** constructor for creating new pppoe frame
	 *
	 */
	fpppoeframe(
			size_t len = DEFAULT_PPPOE_FRAME_SIZE);


	/** destructor
	 *
	 */
	virtual
	~fpppoeframe();


#if 0
	/** copy existing pppoe tag into packet
	 *
	 */
	struct fpppoeframe::pppoe_tag_hdr_t*
	tag_copy(struct fpppoeframe::pppoe_tag_hdr_t *copy);

	/** insert new pppoe tag
	 *
	 */
	struct fpppoeframe::pppoe_tag_hdr_t*
	tag_insert(enum fpppoeframe::pppoe_tag_t type, char *fmt, ...);

	/** find existing tag
	 *
	 */
	struct fpppoeframe::pppoe_tag_hdr_t*
	tag_find(enum fpppoeframe::pppoe_tag_t) throw (ePPPoEFrameTagNotFound);
#endif

	/** required length for pppoe frame in packed form
	 *
	 */
	size_t
	length();

	/** pack pppoe frame
	 *
	 */
	cpacket&
	pack(cpacket& packet);

	/** pack pppoe frame
	 *
	 */
	size_t
	pack(uint8_t* frame, size_t framelen) throw (ePPPoEInval);

	/** unpack pppoe frame
	 *
	 */
	void
	unpack(uint8_t *frame, size_t framelen) throw (ePPPoEInval, eFrameInvalidSyntax);

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
	validate(uint16_t total_len = 0) throw (ePPPoEFrameTooShort,
						ePPPoEFrameInvalType,
						ePPPoEFrameInvalVersion,
						ePPPoEFrameInvalCode,
						ePPPoEPadsInvalSid,
						ePPPoEPadtInvalSid,
						ePPPoEPadiNoSvcTag,
						ePPPoEPadrNoSvcTag);


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
	uint16_t
	get_hdr_length();

	/**
	 */
	void
	set_hdr_length(uint16_t len);

	/**
	 */
	uint8_t
	get_pppoe_vers();

	/**
	 */
	void
	set_pppoe_vers(uint8_t version);

	/**
	 */
	uint8_t
	get_pppoe_type();

	/**
	 */
	void
	set_pppoe_type(uint8_t type);

	/**
	 */
	uint8_t
	get_pppoe_code();

	/**
	 */
	void
	set_pppoe_code(uint8_t code);

	/**
	 */
	uint16_t
	get_pppoe_sessid();

	/**
	 */
	void
	set_pppoe_sessid(uint16_t sessid);

	/** calculate PPPoE frame length
	 */
	void
	pppoe_calc_length();


public: // data structures

	// pointer to pppoe header
	struct pppoe_hdr_t *pppoe_hdr;

#if 0
	// map of service tags
	std::map<enum fpppoeframe::pppoe_tag_t, struct fpppoeframe::pppoe_tag_hdr_t*> pppoe_tags;
#endif

	// pppoe tags
	cpppoelist tags;

	// ppp data
	uint8_t *pppdata;

	// ppp data length
	size_t pppdatalen;

private: // methods

	/** validate PPPoE session packet
	 *
	 */
	void
	validate_pppoe_session() throw (ePPPoEFrameInvalCode);

	/** validate PPPoE discovery packet
	 *
	 */
	void
	validate_pppoe_discovery_padi() throw (ePPPoEFrameInvalidSyntax);
	void
	validate_pppoe_discovery_pado() throw (ePPPoEFrameInvalidSyntax);
	void
	validate_pppoe_discovery_padr() throw (ePPPoEFrameInvalidSyntax);
        void
        validate_pppoe_discovery_pads() throw (ePPPoEFrameInvalidSyntax);
        void
        validate_pppoe_discovery_padt() throw (ePPPoEFrameInvalidSyntax);
#if 0
	void
	validate_pppoe_discovery_pads() throw (ePPPoEFrameInvalCode, ePPPoEPadsInvalSid);
	void
	validate_pppoe_discovery_padt() throw (ePPPoEPadtInvalCode, ePPPoEPadtInvalSid);
#endif

#if 0
	/** parse PPPoE tags in PADI, PADO, PADR, PADS, PADT
	 *
	 */
	void
	parse_pppoe_tags() throw (eFrameInvalidSyntax);
#endif

private: // data structures

	//< info string
	std::string info;
};

}; // end of namespace

#endif
