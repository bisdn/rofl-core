/*
 * ctransaction.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "ctransaction.h"

using namespace rofl::openflow;

ctransaction::ctransaction(
		uint32_t xid,
		cclock const& delta) :
				xid(xid),
				since(cclock::now()),
				expires(cclock::now() + delta)
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

	xid 	= ta.xid;
	since	= ta.since;
	expires = ta.expires;

	return *this;
}


