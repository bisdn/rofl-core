/*
 * fgtpuv1frame.cc
 *
 *  Created on: 04.07.2013
 *      Author: andreas
 */


#include <rofl/common/protocols/fgtpuframe.h>

using namespace rofl;

fgtpuframe::fgtpuframe(
			uint8_t* data,
			size_t datalen) :
			fframe(data, datalen)
{
	gtphu_hdr = soframe();
}



fgtpuframe::fgtpuframe(
			size_t len) :
			fframe(len)
{
	gtphu_hdr = soframe();
}



fgtpuframe::~fgtpuframe()
{

}



void
fgtpuframe::reset(
			uint8_t* data,
			size_t datalen)
{

}



uint8_t
fgtpuframe::get_version() const
{
	return ((gtphu_short_hdr->flags & 0xe0) >> 5);
}



void
fgtpuframe::set_version(
		uint8_t version)
{
	gtphu_short_hdr->flags = (gtphu_short_hdr->flags & 0x1f) | ((version & 0x03) << 5);
}



bool
fgtpuframe::get_pt_flag() const
{
	return (gtphu_short_hdr->flags & GTPU_PT_FLAG);
}



void
fgtpuframe::set_pt_flag(
		bool pt)
{
	if (pt)
		gtphu_short_hdr->flags |=  GTPU_PT_FLAG;
	else
		gtphu_short_hdr->flags &= ~GTPU_PT_FLAG;
}



bool
fgtpuframe::get_e_flag() const
{
	return (gtphu_short_hdr->flags & GTPU_E_FLAG);
}



void
fgtpuframe::set_e_flag(
		bool e)
{
	if (e)
		gtphu_short_hdr->flags |=  GTPU_E_FLAG;
	else
		gtphu_short_hdr->flags &= ~GTPU_E_FLAG;
}



bool
fgtpuframe::get_s_flag() const
{
	return (gtphu_short_hdr->flags & GTPU_S_FLAG);
}



void
fgtpuframe::set_s_flag(
		bool s)
{
	if (s)
		gtphu_short_hdr->flags |=  GTPU_S_FLAG;
	else
		gtphu_short_hdr->flags &= ~GTPU_S_FLAG;
}



bool
fgtpuframe::get_pn_flag() const
{
	return (gtphu_short_hdr->flags & GTPU_PN_FLAG);
}



void
fgtpuframe::set_pn_flag(
		bool pn)
{
	if (pn)
		gtphu_short_hdr->flags |=  GTPU_PN_FLAG;
	else
		gtphu_short_hdr->flags &= ~GTPU_PN_FLAG;
}



uint8_t
fgtpuframe::get_msg_type() const
{
	return gtphu_short_hdr->msgtype;
}



void
fgtpuframe::set_msg_type(
		uint8_t msg_type)
{
	gtphu_short_hdr->msgtype = msg_type;
}



uint16_t
fgtpuframe::get_length() const
{
	return be16toh(gtphu_short_hdr->len);
}



void
fgtpuframe::set_length(
		uint16_t len)
{
	gtphu_short_hdr->msgtype = htobe16(len);
}



uint32_t
fgtpuframe::get_teid() const
{
	return be32toh(gtphu_short_hdr->teid);
}



void
fgtpuframe::set_teid(
		uint32_t teid)
{
	gtphu_short_hdr->teid = htobe32(teid);
}



uint16_t
fgtpuframe::get_seq_no() const
{
	if (not get_s_flag())
		throw eGTPuFrameInval();
	return be16toh(gtphu_s_hdr->seqno);
}



void
fgtpuframe::set_seq_no(
		uint16_t seq_no)
{
	if (not get_s_flag())
		throw eGTPuFrameInval();
	gtphu_s_hdr->seqno = htobe16(seq_no);
}



uint8_t
fgtpuframe::get_npdu_no() const
{
	if (not get_pn_flag())
		throw eGTPuFrameInval();
	return gtphu_pn_hdr->n_pdu_no;
}



void
fgtpuframe::set_npdu_no(
		uint8_t n_pdu_no)
{
	if (not get_pn_flag())
		throw eGTPuFrameInval();
	gtphu_pn_hdr->n_pdu_no = n_pdu_no;
}



uint8_t
fgtpuframe::get_ext_type() const
{
	if (not get_e_flag())
		throw eGTPuFrameInval();
	return gtphu_e_hdr->exthdr;
}



void
fgtpuframe::set_ext_type(
		uint8_t ext_type)
{
	if (not get_e_flag())
		throw eGTPuFrameInval();
	gtphu_e_hdr->exthdr = ext_type;
}



bool
fgtpuframe::complete()
{
	return true;
}



size_t
fgtpuframe::need_bytes()
{
	return 0;
}



void
fgtpuframe::validate(
		uint16_t total_len) throw (eFrameInvalidSyntax)
{

}



void
fgtpuframe::initialize()
{

}



void
fgtpuframe::payload_insert(
		uint8_t *data, size_t datalen) throw (eFrameOutOfRange)
{

}



uint8_t*
fgtpuframe::payload() const throw (eFrameNoPayload)
{
	return 0;
}



size_t
fgtpuframe::payloadlen() const throw (eFrameNoPayload)
{
	return 0;
}



const char*
fgtpuframe::c_str()
{
	return info.c_str();
}


