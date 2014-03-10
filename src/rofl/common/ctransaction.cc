/*
 * ctransaction.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "ctransaction.h"

using namespace rofl::openflow;

ctransaction::ctransaction() :
				xid(0),
				since(cclock::now()),
				expires(cclock::now()),
				msg_type(0),
				msg_sub_type(0)
{

}


ctransaction::ctransaction(
		uint32_t xid,
		cclock const& delta,
		uint8_t msg_type,
		uint16_t msg_sub_type) :
				xid(xid),
				since(cclock::now()),
				expires(cclock::now() + delta),
				msg_type(msg_type),
				msg_sub_type(msg_sub_type)
{

}



ctransaction::~ctransaction()
{

}



ctransaction::ctransaction(
		ctransaction const& ta)
{
	*this = ta;
}



ctransaction&
ctransaction::operator= (
		ctransaction const& ta)
{
	if (this == &ta)
		return *this;

	xid 			= ta.xid;
	since			= ta.since;
	expires 		= ta.expires;
	msg_type 		= ta.msg_type;
	msg_sub_type 	= ta.msg_sub_type;

	return *this;
}


