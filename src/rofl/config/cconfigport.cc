/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cconfigport.cc
 *
 *  Created on: Jul 16, 2012
 *      Author: tobi
 */

#include "cconfigport.h"
#include "rofl/platform/unix/cunixenv.h"

/*static */cconfigport&
cconfigport::getInstance()
{
	static cconfigport instance;
	return instance;
}

void
cconfigport::print_port_information(std::string *out)
{
	for (std::set<cport*>::const_iterator iter = cport::cport_list.begin(); iter != cport::cport_list.end(); ++iter) {
		out->append((*iter)->devname);
		out->append("\n");
	}
}


cconfigport::cconfigport()
{

}

bool
cconfigport::is_port(const std::string& port_name) {
	try {
		cport::find(port_name);
	} catch (ePortNotFound &e) {
		return false;
	}
	return true;
}

bool
cconfigport::attach_port_to_dp(const std::string& port_name,
		const std::string& dpname, const uint32_t port_number)
{
	WRITELOG(CCONFIG, ROFL_DBG, "cconfigport::attach_port_to_dp(): "
			"port_name = %s, dpname = %s, port_number = %d",
			port_name.c_str(), dpname.c_str(), port_number);

#if 0
	cport *port = NULL;
	try {
		port = cport::find(port_name);
	} catch (ePortNotFound &e) {
		return false;
	}
#endif

	cfwdelem *fwdelem = NULL;
	try {
		fwdelem = cfwdelem::find_by_name(dpname);
	} catch (eFwdElemNotFound &e) {
		return false;
	}

	fwdelem->port_attach(port_name, port_number);

	return true;
}

cconfigport::~cconfigport()
{

}
