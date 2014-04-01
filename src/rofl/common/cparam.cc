/*
 * cparam.cc
 *
 *  Created on: 01.04.2014
 *      Author: andreas
 */

#include "rofl/common/cparam.h"

using namespace rofl;


cparam::cparam()
{}



cparam::~cparam()
{}



cparam::cparam(std::string const& param) :
		param(param)
{}



cparam::cparam(cparam const& param)
{
	*this = param;
}



cparam&
cparam::operator= (cparam const& p)
{
	if (this == &p)
		return *this;

	param = p.param;

	return *this;
}



