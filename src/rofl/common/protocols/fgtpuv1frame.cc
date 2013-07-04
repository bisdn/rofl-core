/*
 * fgtpuv1frame.cc
 *
 *  Created on: 04.07.2013
 *      Author: andreas
 */


#include <rofl/common/protocols/fgtpuv1frame.h>

using namespace rofl;

fgtpuv1frame::fgtpuv1frame(
			uint8_t* data,
			size_t datalen) :
			fframe(data, datalen)
{

}



fgtpuv1frame::fgtpuv1frame(
			size_t len) :
			fframe(len)
{

}



fgtpuv1frame::~fgtpuv1frame()
{

}



void
fgtpuv1frame::reset(
			uint8_t* data,
			size_t datalen)
{

}



uint8_t
fgtpuv1frame::get_version() const
{
	return 0;
}



void
fgtpuv1frame::set_version(
		uint8_t version)
{

}



bool
fgtpuv1frame::get_pt_flag() const
{
	return 0;
}



void
fgtpuv1frame::set_pt_flag(
		bool pt)
{

}



bool
fgtpuv1frame::get_e_flag() const
{
	return 0;
}



void
fgtpuv1frame::set_e_flag(
		bool e)
{

}



bool
fgtpuv1frame::get_s_flag() const
{
	return 0;
}



void
fgtpuv1frame::set_s_flag(
		bool s)
{

}



bool
fgtpuv1frame::get_pn_flag() const
{
	return 0;
}



void
fgtpuv1frame::set_pn_flag(
		bool pn)
{

}



uint8_t
fgtpuv1frame::get_msg_type() const
{
	return 0;
}



void
fgtpuv1frame::set_msg_type(
		uint8_t msg_type)
{

}



uint16_t
fgtpuv1frame::get_length() const
{
	return 0;
}



void
fgtpuv1frame::set_length(
		uint16_t len)
{

}



uint32_t
fgtpuv1frame::get_teid() const
{
	return 0;
}



void
fgtpuv1frame::set_teid(
		uint32_t teid)
{

}



uint16_t
fgtpuv1frame::get_seq_no() const
{
	return 0;
}



void
fgtpuv1frame::set_seq_no(
		uint16_t seq_no)
{

}



uint8_t
fgtpuv1frame::get_npdu_no() const
{
	return 0;
}



void
fgtpuv1frame::set_npdu_no(
		uint8_t n_pdu_no)
{

}



uint8_t
fgtpuv1frame::get_ext_type() const
{
	return 0;
}



void
fgtpuv1frame::set_ext_type(
		uint8_t ext_type)
{

}



