/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


/* cphyport.cc
 *
 *  Created on: 24.08.2012
 *      Author: andreas
 */


#include <cphyport.h>

using namespace rofl;

cphyport::cphyport(
		std::map<uint32_t, cphyport*> *port_list,
		uint32_t port_no) :
	port_list(port_list)
{
	cofport::port_no = port_no;

	if ((0 != port_no) && (0 != port_list))
	{
		(*port_list)[port_no] = this;
	}
}


cphyport::~cphyport()
{
	if (0 != port_list)
	{
		port_list->erase(port_no);
	}
}
