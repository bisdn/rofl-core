/*
 * fl2tpframe.h
 *
 *  Created on: 21.11.2013
 *      Author: andreas
 */

#ifndef FL2TPV3FRAME_H_
#define FL2TPV3FRAME_H_ 1

#include <string>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "../openflow/openflow.h"
#include "../fframe.h"
#include "../cerror.h"


namespace rofl
{

class eL2tpFrameBase 		: public eFrameBase {}; 		// base error class for L2TP related errors
class eL2tpFrameNoCtlMsg 	: public eL2tpFrameBase {};		// method cannot be applied, as L2TP message is no control message
class eL2tpFrameNoDataMsg 	: public eL2tpFrameBase {};		// method cannot be applied, as L2TP message is no data message
class eL2tpFrameInval		: public eL2tpFrameBase {};		// invalid parameter
class eL2tpFrameInComplete	: public eL2tpFrameBase {};		// message is incomplete

/**
 * we support UDP transport mode only for now
 */
class fl2tpv3frame :
		public fframe
{
	std::string		info; // used for method c_str()

public:

	enum l2tpv3_ip_proto_t {
		L2TPV3_IP_PROTO = 115,	// for IP encapsulation
	};

	enum l2tp_version_t {
		L2TP_VERSION_2 = 2, // not supported
		L2TP_VERSION_3 = 3,
	};

	enum l2tp_encap_t {
		L2TP_ENCAP_IP = 1,
		L2TP_ENCAP_UDP = 2,
	};

	#define TBIT_FLAG (1 << 15)
	#define LBIT_FLAG (1 << 14)
	#define SBIT_FLAG (1 << 11)
	#define VERS_MASK (0x000f)

	/* L2TPv3 control message header as defined in RFC3931 for IP encapsulation */
	struct l2tpv3_ctl_hdr_ip_encap_t {
		uint32_t pseudo_session_id; // not really a session ID => for control connections always all bits 0 to indicate a control header
		uint16_t ctlfield;
		uint16_t len;
		uint32_t ccid; // control connection ID
		uint16_t ns; // sequence number for this control message sent
		uint16_t nr; // sequence number for next control message expected
	};

	/* L2TPv3 data message header as defined in RFC3931 for IP encapsulation */
	struct l2tpv3_data_hdr_ip_encap_t {
		uint32_t session_id;
		uint8_t data[0]; // cookie or data
	};

	/* L2TPv3 control message header as defined in RFC3931 for UDP encapsulation */
	struct l2tpv3_ctl_hdr_udp_encap_t {
		uint16_t ctlfield;
		uint16_t len;
		uint32_t ccid; // control connection ID
		uint16_t ns; // sequence number for this control message sent
		uint16_t nr; // sequence number for next control message expected
	} __attribute__((packed));

	/* L2TPv3 data message header as defined in RFC3931 for UDP encapsulation */
	struct l2tpv3_data_hdr_udp_encap_t {
		uint16_t ctlfield;
		uint16_t reserved;
		uint32_t session_id;
		uint8_t data[0]; // cookie or data
	} __attribute__((packed));

	/* default layer2 specific sublayer for L2TPv3 as defined in RFC 3931 section 4.6 */
	struct l2tpv3_l2ssub_data_hdr_t {
		uint8_t flags;
		uint8_t seqno[3];
	} __attribute__((packed));

private:

	enum l2tp_encap_t 	encap_type;		// encapsulation type of L2TPv3: IP or UDP

	// attention! L2TP over IP defines a totally different header than L2TP over UDP, so do not rely on the control bit fields to be available!
	union {
		uint8_t*								l2tpv3u_hdr;
		struct l2tpv3_ctl_hdr_ip_encap_t*		l2tpv3u_ctl_hdr_ip_encap;
		struct l2tpv3_data_hdr_ip_encap_t*		l2tpv3u_data_hdr_ip_encap;
		struct l2tpv3_ctl_hdr_udp_encap_t* 		l2tpv3u_ctl_hdr_udp_encap;
		struct l2tpv3_data_hdr_udp_encap_t*		l2tpv3u_data_hdr_udp_encap;
	} l2tpv3u;

	#define l2tpv3_hdr 					l2tpv3u.l2tpv3u_hdr
	#define l2tpv3_ctl_hdr_ip_encap		l2tpv3u.l2tpv3u_ctl_hdr_ip_encap
	#define l2tpv3_data_hdr_ip_encap	l2tpv3u.l2tpv3u_data_hdr_ip_encap
	#define l2tpv3_ctl_hdr_udp_encap	l2tpv3u.l2tpv3u_ctl_hdr_udp_encap
	#define l2tpv3_data_hdr_udp_encap	l2tpv3u.l2tpv3u_data_hdr_udp_encap

public:

	/**
	 *
	 */
	fl2tpv3frame(
			enum l2tp_encap_t encap_type,
			uint8_t *data,
			size_t datalen);

	/**
	 *
	 */
	fl2tpv3frame(
			enum l2tp_encap_t encap_type,
			size_t len);

	/**
	 *
	 */
	virtual
	~fl2tpv3frame();

	/**
	 *
	 */
	virtual void
	reset(
			enum l2tp_encap_t encap_type,
			uint8_t* data,
			size_t datalen);

	/**
	 * @brief	Returns pointer to begin of this PDU.
	 *
	 */
	virtual uint8_t*
	sopdu() const;


	/**
	 * @brief	Returns length of this PDU
	 *
	 */
	virtual size_t
	pdulen() const;


	/**
	 * @brief	Returns pointer to begin of SDU encapsulated in this PDU (if any).
	 */
	virtual uint8_t*
	sosdu() const;

	/**
	 * @brief	Returns length of this SDU
	 *
	 */
	virtual size_t
	sdulen() const;

public:

	/**
	 * @brief	Return L2TPv3 encapsulation mode
	 */
	std::string
	get_encap() const;

	/**
	 * @brief	Returns true, when the message is a control message
	 */
	bool
	is_ctl_msg() const;

	/**
	 * @brief	Returns L2TPv3 T-bit indicating the message type, true:control packet, false:data packet
	 */
	bool
	get_Tbit() const;

	/**
	 * @brief	Set L2TPv3 T-bit indicating the message type, true: control packet, false:data packet (default). Side effect: sets also the pseudo session id to 0.
	 */
	void
	set_Tbit(bool Tbit = true);

	/**
	 * @brief	Returns L2TPv3 L-bit indicating presence of the message length field (only valid for control messages)
	 */
	bool
	get_Lbit() const;

	/**
	 * @brief	Set L2TPv3 L-bit indicating presence of the message length field (only valid for control messages)
	 */
	void
	set_Lbit(bool Lbit = true);

	/**
	 * @brief	Returns L2TPv3 S-bit indicating presence of the message fields Ns and Nr (only valid for control messages)
	 */
	bool
	get_Sbit() const;

	/**
	 * @brief	Set L2TPv3 L-bit indicating presence of the message fields Ns and Nr (only valid for control messages)
	 */
	void
	set_Sbit(bool Sbit = true);

	/**
	 * @brief	Returns version of L2TP frame: currently only L2TPv3 is supported
	 */
	uint8_t
	get_version() const;

	/**
	 * @brief	Set version of L2TP frame: currently only L2TPv3 is supported
	 */
	void
	set_version(uint8_t version = L2TP_VERSION_3);

	/**
	 * @brief	Return L2TPv3 session ID. The session ID is also present in a control message as a pseudo session id for IP encapsulation.
	 * @throw	eL2tpFrameNoDataMsg
	 */
	uint32_t
	get_session_id() const;

	/**
	 * @brief	Set L2TPv3 session ID. The session ID is also present in a control message as a pseudo session id for IP encapsulation.
	 * @throw	eL2tpFrameNoDataMsg
	 */
	void
	set_session_id(uint32_t sid);


	/*
	 * control message specific
	 */

	/**
	 * @brief	Returns length field from L2TPv3 control message header
	 * @throw	eL2tpFrameNoCtlMsg
	 */
	uint16_t
	get_length() const;

	/**
	 * @brief	Set length field in L2TPv3 control message header
	 * @throw	eL2tpFrameNoCtlMsg
	 */
	void
	set_length(uint16_t len);

	/**
	 * @brief	Return control connection ID field from L2TPv3 control message header
	 * @throw 	eL2tpFrameNoCtlMsg
	 */
	uint32_t
	get_ccid() const;

	/**
	 * @brief	Set control connection ID field in L2TPv3 control message header
	 * @throw	eL2tpFrameNoCtlMsg
	 */
	void
	set_ccid(uint32_t ccid);

	/**
	 * @brief	Returns Ns field from L2TPv3 control message header
	 * @throw	eL2tpFrameNoCtlMsg
	 */
	uint16_t
	get_Ns() const;

	/**
	 * @brief	Set Ns field in L2TPv3 control message header
	 * @throw	eL2tpFrameNoCtlMsg
	 */
	void
	set_Ns(uint16_t Ns);

	/**
	 * @brief	Returns Nr field from L2TPv3 control message header
	 * @throw	eL2tpFrameNoCtlMsg
	 */
	uint16_t
	get_Nr() const;

	/**
	 * @brief	Set Nr field in L2TPv3 control message header
	 * @throw	eL2tpFrameNoCtlMsg
	 */
	void
	set_Nr(uint16_t Nr);

#if 0
	/*
	 * data message specific
	 */


	/**
	 * @brief	Return cookie (optional, must be negotiated during session establishment)
	 * @throw	eL2tpFrameNoDataMsg
	 */
	uint64_t
	get_cookie() const;

	/**
	 * @brief 	Set cookie (optional, must be negotiated during session establishment)
	 * @throw	eL2tpFrameNoDataMsg
	 */
	void
	set_cookie(uint64_t cookie);
#endif

public: // overloaded from fframe


	/**
	 *
	 */
	virtual bool
	complete() const;


	/**
	 *
	 */
	virtual size_t
	need_bytes() const;


	/**
	 *
	 */
	virtual void
	validate(
			uint16_t total_len = 0) const;


	/**
	 *
	 */
	virtual void
	initialize();


	/**
	 *
	 */
	virtual void
	payload_insert(
			uint8_t *data, size_t datalen) throw (eFrameOutOfRange);


	/**
	 *
	 */
	virtual uint8_t*
	payload() const throw (eFrameNoPayload);


	/**
	 *
	 */
	virtual size_t
	payloadlen() const throw (eFrameNoPayload);


	/**
	 *
	 */
	virtual const char*
	c_str();

public:

	friend std::ostream&
	operator<< (std::ostream& os, fl2tpv3frame const& f) {
		os << "<fl2tpv3frame: ";
			os << "vers:" << (int)f.get_version() << " ";
			os << "type:" << ((f.get_Tbit() == true) ? "control " : "data ");
			os << "encap:" << f.get_encap() << " ";
			if (f.get_Tbit() == true/*:control*/) {
				os <<"T:" << (f.get_Tbit() == true ? "1":"0") << " ";
				os <<"L:" << (f.get_Lbit() == true ? "1":"0") << " ";
				os <<"S:" << (f.get_Sbit() == true ? "1":"0") << " ";
				os << "len:" << (int)f.get_length() << " ";
				os << "ccid:" << (int)f.get_ccid() << " ";
				os << "Ns:" << (int)f.get_Ns() << " ";
				os << "Nr:" << (int)f.get_Nr() << " ";
			} else /*false:data*/ {
				os <<"T:" << (f.get_Tbit() == true ? "1":"0") << " ";
				os << "sid:" << (int)f.get_session_id() << " ";
			}
		os << ">";
		return os;
	};
};

}; // end of namespace

#endif /* FL2TPFRAME_H_ */
