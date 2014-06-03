/*
 * ctimerid.cc
 *
 *  Created on: 16.05.2014
 *      Author: andreas
 */

#include "rofl/common/ctimerid.h"

using namespace rofl;


uint32_t ctimerid::next_timer_id = 0;

uint32_t
ctimerid::get_next_timer_id() {
	return ((++ctimerid::next_timer_id == 0) ? ++ctimerid::next_timer_id : ctimerid::next_timer_id);
}



ctimerid::ctimerid() :
				tid(ctimerid::get_next_timer_id())
{

}



ctimerid::~ctimerid()
{

}



ctimerid::ctimerid(
		ctimerid const& timerid)
{
	*this = timerid;
}



ctimerid&
ctimerid::operator= (
		ctimerid const& timerid)
{
	if (this == &timerid)
		return *this;

	tid = timerid.tid;

	return *this;
}



bool
ctimerid::operator== (
		ctimerid const& timerid)
{
	return (tid == timerid.tid);
}



