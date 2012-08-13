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
	WRITELOG(CCONFIG, DBG, "cconfigport::attach_port_to_dp(): "
			"port_name = %s, dpname = %s, port_number = %d",
			port_name.c_str(), dpname.c_str(), port_number);

	cport *port = NULL;
	try {
		port = cport::find(port_name);
	} catch (ePortNotFound &e) {
		return false;
	}

	cfwdelem *fwdelem = NULL;
	try {
		fwdelem = cfwdelem::find_by_name(dpname);
	} catch (eFwdElemNotFound &e) {
		return false;
	}

	cport::cport_owner *owner = dynamic_cast<cport::cport_owner*>(fwdelem);

	if (NULL != owner) {
		try {
			uint32_t pn = port_number;
			if (0 == port_number) {
				/* autogen a port_number and set it */
				pn = cport::find_free_port_no(&cport::cport_list);
			}

			/* set port_number and attach port */
			if (pn == port->set_port_no(pn)) {
				port->attach(owner);
			} else {
				return false;
			}

		} catch (ePortIsAttached &e) {
			return false;
		}
	} else {
		return false;
	}

	return true;
}

cconfigport::~cconfigport()
{

}
