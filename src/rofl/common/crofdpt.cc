/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofdpt.cc
 *
 *  Created on: 07.01.2014
 *      Author: andreas
 */

#include "rofl/common/crofdpt.h"

using namespace rofl;

uint64_t crofdpt::next_dptid = 0;

std::map<cdptid, crofdpt*> crofdpt::rofdpts;

/*static*/crofdpt&
crofdpt::get_dpt(
		const cdptid& dptid)
{
	if (crofdpt::rofdpts.find(dptid) == crofdpt::rofdpts.end()) {
		throw eRofDptNotFound();
	}
	return *(crofdpt::rofdpts[dptid]);
}



/*static*/crofdpt&
crofdpt::get_dpt(
		uint64_t dpid)
{
	std::map<cdptid, crofdpt*>::iterator it;
	if ((it = find_if(crofdpt::rofdpts.begin(), crofdpt::rofdpts.end(),
			crofdpt::crofdpt_find_by_dpid(dpid))) == crofdpt::rofdpts.end()) {
		throw eRofDptNotFound();
	}
	return *(it->second);
}


