/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cconfigcfwdelem.cc
 *
 *  Created on: Jul 16, 2012
 *      Author: tobi
 */

#include "cconfigfwdelem.h"

cconfigfwdelem&
cconfigfwdelem::getInstance() {
	static cconfigfwdelem instance;
	return instance;
}

void
cconfigfwdelem::print_datapath_information(std::string* out) const
{
	for (std::set<cfwdelem*>::const_iterator iter = cfwdelem::fwdelems.begin(); iter != cfwdelem::fwdelems.end(); ++iter) {
		out->append((*iter)->get_dpname());
	}
}

cconfigfwdelem::cconfigfwdelem()
{
}

bool
cconfigfwdelem::is_datapath(const std::string& dpname) const
{
	try {
		cfwdelem::find_by_name(dpname);
	} catch (eFwdElemNotFound &e) {
		return false;
	}
	return true;
}

cconfigfwdelem::~cconfigfwdelem()
{
}
