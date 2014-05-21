/*
 * crofdpt.cc
 *
 *  Created on: 07.01.2014
 *      Author: andreas
 */

#include "rofl/common/crofdpt.h"

using namespace rofl;

std::map<cdptid, crofdpt*> crofdpt::rofdpts;

/*static*/crofdpt&
crofdpt::get_dpt(
		const cdptid& dpid)
{
	if (crofdpt::rofdpts.find(dpid) == crofdpt::rofdpts.end()) {
		throw eRofDptNotFound();
	}
	return *(crofdpt::rofdpts[dpid]);
}
