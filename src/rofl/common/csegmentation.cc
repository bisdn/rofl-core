/*
 * csegmentation.cc
 *
 *  Created on: 13.03.2014
 *      Author: andreas
 */

#include "rofl/common/csegmentation.h"

using namespace rofl::openflow;


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

