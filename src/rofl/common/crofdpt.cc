/*
 * crofdpt.cc
 *
 *  Created on: 07.01.2014
 *      Author: andreas
 */

#include "rofl/common/crofdpt.h"

using namespace rofl;

uint64_t crofdpt::next_dptid = 0;

std::map<uint64_t, crofdpt*> crofdpt::rofdpts;

crofdpt&
crofdpt::get_dpt(
		uint64_t dptid)
{
	if (crofdpt::rofdpts.find(dptid) == crofdpt::rofdpts.end()) {
		throw eRofDptNotFound();
	}
	return *(crofdpt::rofdpts[dptid]);
}
