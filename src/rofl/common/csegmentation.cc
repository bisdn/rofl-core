/*
 * csegmentation.cc
 *
 *  Created on: 13.03.2014
 *      Author: andreas
 */

#include "rofl/common/csegmentation.h"

using namespace rofl;


csegmentation::csegmentation()
{

}


csegmentation::~csegmentation()
{

}


csegmentation::csegmentation(
		csegmentation const& seg)
{
	*this = seg;
}


csegmentation&
csegmentation::operator= (csegmentation const& seg)
{
	if (this == &seg)
		return *this;


	return *this;
}



csegmsg&
csegmentation::add_transaction(uint32_t xid)
{
	if (segmsgs.find(xid) != segmsgs.end()) {
		segmsgs.erase(xid);
	}
	return segmsgs[xid];
}



void
csegmentation::drop_transaction(uint32_t xid)
{
	if (segmsgs.find(xid) == segmsgs.end()) {
		return;
	}
	segmsgs.erase(xid);
}



csegmsg&
csegmentation::set_transaction(uint32_t xid)
{
	if (segmsgs.find(xid) == segmsgs.end()) {
		(void)segmsgs[xid];
	}
	return segmsgs[xid];
}



csegmsg const&
csegmentation::get_transaction(uint32_t xid) const
{
	if (segmsgs.find(xid) == segmsgs.end()) {
		throw eSegmentationNotFound();
	}
	return segmsgs.at(xid);
}



bool
csegmentation::has_transaction(uint32_t xid)
{
	return (not (segmsgs.find(xid) == segmsgs.end()));
}



