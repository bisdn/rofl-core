/*
 * fgtpframe.h
 *
 *  Created on: 04.07.2013
 *      Author: andreas
 */

#ifndef FGTPUV1FRAME_H_
#define FGTPUV1FRAME_H_ 1

#ifdef __cplusplus
extern "C" {
#endif
#include "../openflow/openflow.h"
#include <endian.h>
#ifndef htobe16
#include "../endian_conversion.h"
#endif
#ifdef __cplusplus
}
#endif

#include <rofl/common/fframe.h>

namespace rofl
{

class fgtpuv1frame :
		public fframe
{
public:

	enum gtpu_version_t {
		GTPU_VERS_1 = 1,
	};

	// full GTP-Uv1 header with E, S, PN flags set to 1
	struct gtpuv1_e_hdr_t {
		uint8_t 	flags;		// version, PT, T, E, S, PN
		uint8_t		msgtype;
		uint16_t	len;
		uint32_t	teid;
		uint16_t	seqno;
		uint8_t		n_pdu;		// N-PDU number
		uint8_t		nxthdr;		// next extension header type
		uint8_t		data[0];	// pointer to start of data
	};

	// shortened GTP-Uv1 header with S and PN flags set to 1 only
	struct gtpuv1_pn_hdr_t {
		uint8_t 	flags;		// version, PT, T, E, S, PN
		uint8_t		msgtype;
		uint16_t	len;
		uint32_t	teid;
		uint16_t	seqno;
		uint8_t		n_pdu;		// N-PDU number
		uint8_t		data[0];	// pointer to start of data
	};

	// shortened GTP-Uv1 header with S flag set to 1 only
	struct gtpuv1_s_hdr_t {
		uint8_t 	flags;		// version, PT, T, E, S, PN
		uint8_t		msgtype;
		uint16_t	len;
		uint32_t	teid;
		uint16_t	seqno;
		uint8_t		data[0];	// pointer to start of data
	};

	// shortened GTP-Uv1 header with S, PN, E flags set to 0
	struct gtpuv1_short_hdr_t {
		uint8_t 	flags;		// version, PT, T, E, S, PN
		uint8_t		msgtype;
		uint16_t	len;
		uint32_t	teid;
		uint8_t		data[0];	// pointer to start of data
	};

	// typedef the full header
	typedef gtpuv1_e_hdr_t gtpuv1_hdr_t;

	// GTP-Uv1 extension header
	struct gtpuv1_ext_hdr_t {
		uint8_t		extlen;		// length of extension header in bytes
		uint8_t		data[0];	// pointer to start of data
					/* last byte contains next extension header type */
	};

public:


	/**
	 *
	 */
	fgtpuv1frame(
			uint8_t* data,
			size_t datalen);


	/**
	 *
	 */
	fgtpuv1frame(
			size_t len = sizeof(gtpuv1_hdr_t));


	/**
	 *
	 */
	virtual
	~fgtpuv1frame();


	/**
	 *
	 */
	virtual void
	reset(
			uint8_t* data,
			size_t datalen);


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
};

}; // end of namespace

#endif /* FGTPUV1FRAME_H_ */
