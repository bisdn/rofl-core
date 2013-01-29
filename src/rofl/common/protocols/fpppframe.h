/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FPPPFRAME_H
#define FPPPFRAME_H 1

#include <map>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
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

class ePPPBase 					: public eFrameBase {};
class ePPPInval 				: public ePPPBase {};
class ePPPFrameHdlcDecodeFailed : public eFrameBase {}; // HDLC decoding failed
class ePPPFrameOptionNotFound 	: public eFrameBase {}; // PPP option not found
class ePPPFrameInvalidSyntax 	: public eFrameBase {}; // PPP frame with invalid syntax
class ePPPLcpNotFound 			: public ePPPBase {};
class ePPPLcpOptionNotFound 	: public ePPPBase {};
class ePPPLcpOptionInvalid		: public ePPPBase {};
class ePPPIpcpOptionNotFound	: public ePPPBase {};
class ePPPIpcpOptionInvalid		: public ePPPBase {};


class fppp_lcp_option; // forward declaration, see below
class fppp_ipcp_option; // forward declaration, see below

class fpppframe : public fframe {
public: // static

	// HDLC related definitions
	//
	enum hdlc_code_t {
		HDLC_FRAME_DELIMITER = 0x7e,
		HDLC_DST_ALL = 0xff,
		HDLC_PPP_CONTROL = 0x03, // 0x03 => 0x7d 0x23, check RFC 1662
		HDLC_ESCAPE = 0x7d,
	};

	// PPP frame related definitions
	//

	enum ppp_prot_t {
		PPP_PROT_PADDING 	= 0x0001, // 0x00 0x01 in network byte order
		PPP_PROT_LCP 		= 0xc021, // 0xc0 0x21 in network byte order
		PPP_PROT_PAP 		= 0xc023, // 0xc0 0x23 in network byte order
		PPP_PROT_LQR 		= 0xc025, // 0xc0 0x25 in network byte order
		PPP_PROT_CHAP 		= 0xc223, // 0xc2 0x23 in network byte order
		PPP_PROT_EAP 		= 0xc227, // 0xc2 0x27 in network byte order
		PPP_PROT_IPCP 		= 0x8021, // 0x80 0x21 in network byte order
		PPP_PROT_IPV4 		= 0x0021, // 0x00 0x21 in network byte order
		PPP_PROT_CCP 		= 0x80fd, // 0x80 0xfd in network byte order
	};

	struct ppp_hdr_t {
		uint16_t prot;
		uint8_t data[0];
	} __attribute__((packed));

	// PPP-LCP related definitions
	//
	enum ppp_lcp_code_t {
		PPP_LCP_CONF_REQ = 0x01,
		PPP_LCP_CONF_ACK = 0x02,
		PPP_LCP_CONF_NAK = 0x03,
		PPP_LCP_CONF_REJ = 0x04,
		PPP_LCP_TERM_REQ = 0x05,
		PPP_LCP_TERM_ACK = 0x06,
		PPP_LCP_CODE_REJ = 0x07,
		PPP_LCP_PROT_REJ = 0x08,
		PPP_LCP_ECHO_REQ = 0x09,
		PPP_LCP_ECHO_REP = 0x0a,
		PPP_LCP_DISC_REQ = 0x0b,
	};

	/**
	 * PPP-IPCP related definitions
	 *
	 * Code	Description			References
	 * 0	Vendor Specific.	RFC 2153
	 * 1	Configure-Request.
	 * 2	Configure-Ack.
	 * 3	Configure-Nak.
	 * 4	Configure-Reject.
	 * 5	Terminate-Request.
	 * 6	Terminate-Ack.
	 * 7	Code-Reject.
	 *
	 * (http://en.wikipedia.org/wiki/Internet_Protocol_Control_Protocol)
	 */
	enum ppp_ipcp_code_t {
		PPP_IPCP_VEND_SPC = 0x00,//!< PPP_IPCP_VEND_SPC
		PPP_IPCP_CONF_REQ = 0x01,//!< PPP_IPCP_CONF_REQ
		PPP_IPCP_CONF_ACK = 0x02,//!< PPP_IPCP_CONF_ACK
		PPP_IPCP_CONF_NAK = 0x03,//!< PPP_IPCP_CONF_NAK
		PPP_IPCP_CONF_REJ = 0x04,//!< PPP_IPCP_CONF_REJ
		PPP_IPCP_TERM_REQ = 0x05,//!< PPP_IPCP_TERM_REQ
		PPP_IPCP_TERM_ACK = 0x06,//!< PPP_IPCP_TERM_ACK
		PPP_IPCP_CODE_REJ = 0x07 //!< PPP_IPCP_CODE_REJ
	};

	/* structure for lcp and ipcp */
	struct ppp_lcp_hdr_t {
		uint8_t code;
		uint8_t ident;
		uint16_t length; // includes this header and data
		uint8_t data[0];
	} __attribute__((packed));

	enum ppp_lcp_option_t {
		PPP_LCP_OPT_RESERVED 	= 0x00,
		PPP_LCP_OPT_MRU 		= 0x01,
		PPP_LCP_OPT_ACCM 		= 0x02,
		PPP_LCP_OPT_AUTH_PROT 	= 0x03,
		PPP_LCP_OPT_QUAL_PROT 	= 0x04,
		PPP_LCP_OPT_MAGIC_NUM 	= 0x05,
		PPP_LCP_OPT_PFC 		= 0x07,
		PPP_LCP_OPT_ACFC 		= 0x08,
	};

	/**
	 * Option	Length	Description						References
	 * 1	 			IP-Addresses (deprecated).		RFC 1332
	 * 2		>= 14	IP-Compression-Protocol.		RFC 1332, RFC 3241,
	 * 													RFC 3544
	 * 3		6		IP-Address.						RFC 1332
	 * 4		6		Mobile-IPv4.					RFC 2290
	 * 129		6		Primary DNS Server Address.		RFC 1877
	 * 130		6		Primary NBNS Server Address.	RFC 1877
	 * 131		6		Secondary DNS Server Address.	RFC 1877
	 * 132		6		Secondary NBNS Server Address.	RFC 1877
	 *
	 * (http://en.wikipedia.org/wiki/Internet_Protocol_Control_Protocol#Configuration_Options)
	 */
	enum ppp_ipcp_option_t {
		PPP_IPCP_OPT_IPV4_DEP	= 1,   //!< PPP_IPCP_OPT_IPV4_DEP
		PPP_IPCP_OPT_IP_COMP	= 2,   //!< PPP_IPCP_OPT_IP_COMP
		PPP_IPCP_OPT_IPV4		= 3,   //!< PPP_IPCP_OPT_IPV4		RFC 1332
		PPP_IPCP_OPT_MOB_IPV4	= 4,   //!< PPP_IPCP_OPT_MOB_IPV4 	RFC 2290
		PPP_IPCP_OPT_PRIM_DNS	= 129, //!< PPP_IPCP_OPT_PRIM_DNS
		PPP_IPCP_OPT_PRIM_MBNS	= 130, //!< PPP_IPCP_OPT_PRIM_MBNS
		PPP_IPCP_OPT_SEC_DNS	= 131, //!< PPP_IPCP_OPT_SEC_DNS
		PPP_IPCP_OPT_SEC_MBNS	= 132  //!< PPP_IPCP_OPT_SEC_MBNS
	};

	/* structure for lcp */
	struct ppp_lcp_opt_hdr_t {
		uint8_t option;
		uint8_t length; // includes this header and data
		uint8_t data[0];
	} __attribute__((packed));

	/* structure for ipcp */
	struct ppp_ipcp_opt_hdr_t {
		uint8_t option;
		uint8_t length; // includes this header and data
		uint8_t data[0];
	} __attribute__((packed));

	static uint16_t fcstab[256];

public: // methods


	/** constructor
	 *
	 */
	fpppframe(
			uint8_t *data,
			size_t datalen);


	/** destructor
	 *
	 */
	virtual
	~fpppframe();


	/** asynchronous HDLC decode
	 *
	 */
	void
	hdlc_decode(
			cmemory& decoded,
			uint32_t accm_recv) throw (ePPPFrameHdlcDecodeFailed);


	/** asynchronous HDLC encode
	 *
	 */
	void
	hdlc_encode(
			cmemory& encoded,
			uint32_t *accm_send);

	/** find lcp specific option
	 *
	 */
	fppp_lcp_option*
	lcp_option_find(
			enum ppp_lcp_option_t type) throw (ePPPFrameOptionNotFound);

	/**
	 * find ipcp specific option
	 */
	fppp_ipcp_option*
	ipcp_option_find(
			enum ppp_ipcp_option_t type) throw (ePPPFrameOptionNotFound);

public:

	/**
	 *
	 */
	uint16_t
	get_ppp_prot() const;

	/**
	 *
	 */
	void
	set_ppp_prot(uint16_t code);

	/**
	 *
	 */
	uint8_t
	get_lcp_code() throw (ePPPLcpNotFound);

	/**
	 *
	 */
	void
	set_lcp_code(uint8_t code) throw (ePPPLcpNotFound);

	/**
	 *
	 */
	uint8_t
	get_lcp_ident() throw (ePPPLcpNotFound);

	/**
	 *
	 */
	void
	set_lcp_ident(uint8_t ident) throw (ePPPLcpNotFound);

	/**
	 *
	 */
	uint16_t
	get_lcp_length() throw (ePPPLcpNotFound);

	/**
	 *
	 */
	void
	set_lcp_length(uint16_t len) throw (ePPPLcpNotFound);

	/**
	 *
	 */
	fppp_lcp_option*
	get_lcp_option(enum ppp_lcp_option_t option) throw (ePPPLcpOptionNotFound);

	/**
	 *
	 */
	fppp_ipcp_option*
	get_ipcp_option(enum ppp_ipcp_option_t option) throw (ePPPIpcpOptionNotFound);


public: // overloaded from fframe


	/**
	 *
	 */
	void
	unpack(
			uint8_t *frame,
			size_t framelen) throw (ePPPInval);


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

	struct ppp_hdr_t* ppp_hdr; // PPP header if present
	struct ppp_lcp_hdr_t *ppp_lcp_hdr; // PPP LCP header if present
	struct ppp_lcp_hdr_t *ppp_ipcp_hdr; // PPP IPCP header if present
	std::map<enum fpppframe::ppp_lcp_option_t, fppp_lcp_option*> lcp_options; // LCP options, if any
	std::map<enum fpppframe::ppp_ipcp_option_t, fppp_ipcp_option*> ipcp_options; // IPCP options, if any

private: // methods

	/** CRC16 calculation from RFC1662
	 * Calculate a new fcs given the current fcs and the new data.
	 */
	uint16_t
	pppfcs16(register uint16_t fcs,
			register unsigned char *cp,
			register int len);

	/** CRC16 test function from RFC1662
	 * How to use the fcs
	 */
	void
	tryfcs16(
			register unsigned char *cp,
			register int len);

	/** validate PPP LCP frame
	 *
	 */
	void
	validate_lcp() throw (ePPPFrameInvalidSyntax);

	/** parse PPP LCP options
	 *
	 */
	void
	parse_lcp_options() throw (ePPPFrameInvalidSyntax);

	/**
	 * validate PPP IPCP frame
	 */
	void
	validate_ipcp() throw (ePPPFrameInvalidSyntax);

	/**
	 * parse PPP IPCP options
	 */
	void
	parse_ipcp_options() throw (ePPPFrameInvalidSyntax);

private: // data structures

	std::string info;

};


class fppp_lcp_option : public fframe {
public:
	struct fpppframe::ppp_lcp_opt_hdr_t *hdr;
public:
	fppp_lcp_option(uint8_t *opt, size_t optlen) :
		fframe(opt, optlen), hdr((struct fpppframe::ppp_lcp_opt_hdr_t*)opt) {};
	fppp_lcp_option(fpppframe::ppp_lcp_opt_hdr_t *opt, size_t optlen) :
		fframe((uint8_t*)opt, optlen), hdr(opt) {};
	void validate() throw (ePPPLcpOptionInvalid) {
		if (hdr->length < sizeof(uint16_t)) throw ePPPLcpOptionInvalid();
		switch (hdr->option) {
		case fpppframe::PPP_LCP_OPT_RESERVED: {
			if (hdr->length < 2) throw ePPPLcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_LCP_OPT_MRU: {
			if (hdr->length < 4) throw ePPPLcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_LCP_OPT_ACCM: {
			if (hdr->length < 6) throw ePPPLcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_LCP_OPT_AUTH_PROT: {
			if (hdr->length < 4) throw ePPPLcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_LCP_OPT_QUAL_PROT: {
			if (hdr->length < 4) throw ePPPLcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_LCP_OPT_MAGIC_NUM: {
			if (hdr->length < 6) throw ePPPLcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_LCP_OPT_PFC: {
			if (hdr->length < 2) throw ePPPLcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_LCP_OPT_ACFC: {
			if (hdr->length < 2) throw ePPPLcpOptionInvalid();
		}
			break;
		default: {
			// do nothing
		}
			break;
		}
	};
	uint16_t
	get_mru() throw (ePPPLcpOptionInvalid) {
		if (fpppframe::PPP_LCP_OPT_MRU != hdr->option) throw ePPPLcpOptionInvalid();
		return (be16toh(*(uint16_t*)(hdr->data)));
	};
	uint32_t
	get_accm() throw (ePPPLcpOptionInvalid) {
		if (fpppframe::PPP_LCP_OPT_ACCM != hdr->option) throw ePPPLcpOptionInvalid();
		return (be32toh(*(uint32_t*)(hdr->data)));
	};
	uint32_t
	get_magic_num() throw (ePPPLcpOptionInvalid) {
		if (fpppframe::PPP_LCP_OPT_MAGIC_NUM != hdr->option) throw ePPPLcpOptionInvalid();
		return (be32toh(*(uint32_t*)(hdr->data)));
	};
};



class fppp_ipcp_option : public fframe {
public:
	struct fpppframe::ppp_ipcp_opt_hdr_t *hdr;
public:
	fppp_ipcp_option(uint8_t *opt, size_t optlen) :
		fframe(opt, optlen), hdr((struct fpppframe::ppp_ipcp_opt_hdr_t*)opt) {};
	fppp_ipcp_option(fpppframe::ppp_ipcp_opt_hdr_t *opt, size_t optlen) :
		fframe((uint8_t*)opt, optlen), hdr(opt) {};
	void validate() throw (ePPPIpcpOptionInvalid) {
		if (hdr->length < sizeof(uint16_t)) throw ePPPIpcpOptionInvalid();
		switch (hdr->option) {
		case fpppframe::PPP_IPCP_OPT_IP_COMP: {
			if (hdr->length < 4) throw ePPPIpcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_IPCP_OPT_IPV4: {
			if (hdr->length < 6) throw ePPPIpcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_IPCP_OPT_MOB_IPV4: {
			if (hdr->length < 6) throw ePPPIpcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_IPCP_OPT_PRIM_DNS: {
			if (hdr->length < 6) throw ePPPIpcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_IPCP_OPT_PRIM_MBNS: {
			if (hdr->length < 6) throw ePPPIpcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_IPCP_OPT_SEC_DNS: {
			if (hdr->length < 6) throw ePPPIpcpOptionInvalid();
		}
			break;
		case fpppframe::PPP_IPCP_OPT_SEC_MBNS: {
			if (hdr->length < 6) throw ePPPIpcpOptionInvalid();
		}
			break;

		default: {
			// do nothing
		}
			break;
		}
	};
	uint32_t
	get_ipv4() throw (ePPPIpcpOptionInvalid) {
		if (fpppframe::PPP_IPCP_OPT_IPV4 != hdr->option) throw ePPPIpcpOptionInvalid();
		return (be32toh(*(uint32_t*)(hdr->data)));
	};
	uint32_t
	get_mob_ipv4() throw (ePPPIpcpOptionInvalid) {
		if (fpppframe::PPP_IPCP_OPT_MOB_IPV4 != hdr->option) throw ePPPIpcpOptionInvalid();
		return (be32toh(*(uint32_t*)(hdr->data)));
	};
};



}; // end of namespace

#endif
