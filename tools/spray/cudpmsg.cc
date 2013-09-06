/*
 * cudpmsg.cc
 *
 *  Created on: 01.09.2013
 *      Author: andreas
 */

#include "cudpmsg.h"

using namespace spray;

cudpmsg::cudpmsg(size_t msg_len) :
		rofl::cmemory(msg_len),
		msg_hdr((struct udp_msg_hdr_t*)somem())
{

}


cudpmsg::~cudpmsg()
{

}



uint8_t*
cudpmsg::resize(size_t msglen)
{
	cmemory::resize(msglen);
	msg_hdr = (struct udp_msg_hdr_t*)somem();
	return somem();
}



void
cudpmsg::set_type(uint8_t type)
{
	msg_hdr->type = type;
}



uint8_t
cudpmsg::get_type() const
{
	return msg_hdr->type;
}



void
cudpmsg::set_seqno(uint32_t seqno)
{
	msg_hdr->seqno = htobe32(seqno);
}



uint32_t
cudpmsg::get_seqno() const
{
	return be32toh(msg_hdr->seqno);
}


