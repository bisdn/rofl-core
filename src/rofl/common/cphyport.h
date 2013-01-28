/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


/*
 * cphyport.h
 *
 *  Created on: 24.08.2012
 *      Author: andreas
 */

#ifndef CPHYPORT_H
#define CPHYPORT_H 1

#include <map>

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#ifdef __cplusplus
}
#endif

#include <rofl/platform/unix/csyslog.h>
#include <rofl/common/openflow/cofport.h>

namespace rofl
{

/**
 * base class for all physical ports on an OF enabled element
 *
 *
 */
class cphyport :
	public cofport
{
/*
 * data structures
 */
public:



/*
 * methods
 */
public:

	/**
	 *
	 */
	cphyport(
			std::map<uint32_t, cofport*> *port_list = 0,
			uint32_t port_no = 0) :
				cofport(port_list)
	{
		cofport::port_no = port_no;
	};


	/**
	 *
	 */
	virtual
	~cphyport()
	{

	};

};

}; // end of namespace

#endif /* CPHYPORT_H_ */
