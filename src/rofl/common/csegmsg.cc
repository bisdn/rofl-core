/*
 * csegmsg.cc
 *
 *  Created on: 13.03.2014
 *      Author: andreas
 */

#include "rofl/common/csegmsg.h"

using namespace rofl::openflow;

csegmsg::csegmsg(uint32_t xid) :
	xid(xid),
	msg(NULL)
{

}



csegmsg::~csegmsg()
{

}



csegmsg::csegmsg(csegmsg const& msg)
{
	*this = msg;
}



csegmsg&
csegmsg::operator= (csegmsg const& msg)
{
	if (this == &msg)
		return *this;


	return *this;
}



