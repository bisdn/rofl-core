/*
 * crofdpt.cc
 *
 *  Created on: 07.01.2014
 *      Author: andreas
 */

#include "rofl/common/crofdpt.h"

using namespace rofl;

std::map<uint64_t, crofdpt*> crofdpt::rofdpts;

crofdpt&
crofdpt::get_dpt(
		uint64_t dpid)
{
	if (crofdpt::rofdpts.find(dpid) == crofdpt::rofdpts.end()) {
		throw eRofDptNotFound();
	}
	return *(crofdpt::rofdpts[dpid]);
}
