/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cconfigport.h
 *
 *  Created on: Jul 16, 2012
 *      Author: tobi
 */

#ifndef CCONFIGPORT_H_
#define CCONFIGPORT_H_

#include <string>
#include <stdint.h>

#include "rofl/common/cport.h"
#include "rofl/common/cfwdelem.h"
#include "rofl/platform/unix/csyslog.h"

class cconfigport : public csyslog
{
public:

	static cconfigport&
	getInstance();

	void
	print_port_information(std::string *out);

	bool
	is_port(const std::string &port_name);

	/**
	 * Attach a port to a datapath. Both are identified by their names. \var port_number is set before
	 * port is attached. If  \var port_number is NOT specified or 0 a port_number is automatically generated.
	 *
	 * @param portname
	 * @param dpname
	 * @param port_number
	 * @return
	 */
	bool
	attach_port_to_dp(const std::string &portname, const std::string &dpname, const uint32_t port_number = 0);

private:
	cconfigport();
	~cconfigport();
};


#endif /* CCONFIGPORT_H_ */
