/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofctl.cc
 *
 *  Created on: 07.01.2014
 *      Author: andreas
 */

#include "rofl/common/crofctl.h"

using namespace rofl;

uint64_t crofctl::next_ctlid = 0;

std::map<cctlid, crofctl*> crofctl::rofctls;

/*static*/crofctl&
crofctl::get_ctl(
		const cctlid& ctlid)
{
	if (crofctl::rofctls.find(ctlid) == crofctl::rofctls.end()) {
		throw eRofCtlNotFound();
	}
	return *(crofctl::rofctls[ctlid]);
}
