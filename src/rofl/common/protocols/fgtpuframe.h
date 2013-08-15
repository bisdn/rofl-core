/*
 * fgtpframe.h
 *
 *  Created on: 04.07.2013
 *      Author: andreas
 */

#ifndef FGTPUV1FRAME_H_
#define FGTPUV1FRAME_H_ 1

#include <string>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "../openflow/openflow.h"
#include "../fframe.h"
#include "gtpuext.h"

namespace rofl
{

class eGTPuFrameBase 		: public eFrameBase {};
class eGTPuFrameInval		: public eGTPuFrameBase, public eFrameInval {};


class fgtpuframe :
		public fframe
{
public:

	enum gtpu_version_t {
		GTPU_VERS_1 = 1,
	};

	enum gtpu_udp_port_t {
		GTPU_UDP_PORT = 2152,
	};

	enum gtpu_flag_t {
		GTPU_PT_FLAG = (1 << 4),
		GTPU_E_FLAG  = (1 << 2),
		GTPU_S_FLAG  = (1 << 1),
		GTPU_PN_FLAG = (1 << 0),
	};

	// full GTP-Uv1 header with E, S, PN flags set to 1
	struct gtpu_e_hdr_t {
		uint8_t 	flags;		// version, PT, T, E, S, PN
		uint8_t		msgtype;
		uint16_t	len;
		uint32_t	teid;
		uint16_t	seqno;
		uint8_t		n_pdu_no;	// N-PDU number
		uint8_t		exthdr;		// next extension header type
		uint8_t		data[0];	// pointer to start of data
	};

	// shortened GTP-Uv1 header with S and PN flags set to 1 only
	struct gtpu_pn_hdr_t {
		uint8_t 	flags;		// version, PT, T, E, S, PN
		uint8_t		msgtype;
		uint16_t	len;
		uint32_t	teid;
		uint16_t	seqno;
		uint8_t		n_pdu_no;	// N-PDU number
		uint8_t		data[0];	// pointer to start of data
	};

	// shortened GTP-Uv1 header with S flag set to 1 only
	struct gtpu_s_hdr_t {
		uint8_t 	flags;		// version, PT, T, E, S, PN
		uint8_t		msgtype;
		uint16_t	len;
		uint32_t	teid;
		uint16_t	seqno;
		uint8_t		data[0];	// pointer to start of data
	};

	// shortened GTP-Uv1 header with S, PN, E flags set to 0
	struct gtpu_short_hdr_t {
		uint8_t 	flags;		// version, PT, T, E, S, PN
		uint8_t		msgtype;
		uint16_t	len;
		uint32_t	teid;
		uint8_t		data[0];	// pointer to start of data
	};


private:

	union {
		uint8_t*					gtphu_gtpu_hdr;
		struct gtpu_e_hdr_t* 		gtphu_gtpu_e_hdr;
		struct gtpu_pn_hdr_t*		gtphu_gtpu_pn_hdr;
		struct gtpu_s_hdr_t* 		gtphu_gtpu_s_hdr;
		struct gtpu_short_hdr_t* 	gtphu_gtpu_short_hdr;
	} gtphu;

#define gtphu_hdr		gtphu.gtphu_gtpu_hdr
#define gtphu_e_hdr		gtphu.gtphu_gtpu_e_hdr
#define gtphu_pn_hdr	gtphu.gtphu_gtpu_pn_hdr
#define gtphu_s_hdr		gtphu.gtphu_gtpu_s_hdr
#define gtphu_short_hdr	gtphu.gtphu_gtpu_short_hdr

	std::string	info;

public:


	/**
	 *
	 */
	fgtpuframe(
			uint8_t* data,
			size_t datalen);


	/**
	 *
	 */
	fgtpuframe(
			size_t len = sizeof(struct gtpu_e_hdr_t));


	/**
	 *
	 */
	virtual
	~fgtpuframe();


	/**
	 *
	 */
	virtual void
	reset(
			uint8_t* data,
			size_t datalen);


	/**
	 * @brief	Returns pointer to begin of this PDU.
	 *
	 */
	virtual uint8_t*
	sopdu();


	/**
	 * @brief	Returns length of this PDU
	 *
	 */
	virtual size_t
	pdulen();


	/**
	 * @brief	Returns pointer to begin of SDU encapsulated in this PDU (if any).
	 */
	virtual uint8_t*
	sosdu();



public:


	/**
	 *
	 */
	uint8_t
	get_version() const;


	/**
	 *
	 */
	void
	set_version(
			uint8_t version = GTPU_VERS_1);


	/**
	 *
	 */
	bool
	get_pt_flag() const;


	/**
	 *
	 */
	void
	set_pt_flag(
			bool pt = true);


	/**
	 *
	 */
	bool
	get_e_flag() const;


	/**
	 *
	 */
	void
	set_e_flag(
			bool e = true);


	/**
	 *
	 */
	bool
	get_s_flag() const;


	/**
	 *
	 */
	void
	set_s_flag(
			bool s = true);


	/**
	 *
	 */
	bool
	get_pn_flag() const;


	/**
	 *
	 */
	void
	set_pn_flag(
			bool pn = true);


	/**
	 *
	 */
	uint8_t
	get_msg_type() const;


	/**
	 *
	 */
	void
	set_msg_type(
			uint8_t msg_type);


	/**
	 *
	 */
	uint16_t
	get_length() const;


	/**
	 *
	 */
	void
	set_length(
			uint16_t len);


	/**
	 *
	 */
	uint32_t
	get_teid() const;


	/**
	 *
	 */
	void
	set_teid(
			uint32_t teid);


	/**
	 *
	 */
	uint16_t
	get_seq_no() const;


	/**
	 *
	 */
	void
	set_seq_no(
			uint16_t seq_no);


	/**
	 *
	 */
	uint8_t
	get_npdu_no() const;


	/**
	 *
	 */
	void
	set_npdu_no(
			uint8_t n_pdu_no);


	/**
	 *
	 */
	uint8_t
	get_ext_type() const;


	/**
	 *
	 */
	void
	set_ext_type(
			uint8_t ext_type);


public: // overloaded from fframe


	/**
	 *
	 */
	virtual bool
	complete();


	/**
	 *
	 */
	virtual size_t
	need_bytes();


	/**
	 *
	 */
	virtual void
	validate(
			uint16_t total_len = 0) throw (eFrameInvalidSyntax);


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
};

}; // end of namespace

#endif /* FGTPUV1FRAME_H_ */
