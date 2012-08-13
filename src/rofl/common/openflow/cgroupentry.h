/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CGROUPENTRY_H
#define CGROUPENTRY_H 1

#include <string>
#include <vector>

#include "../ciosrv.h"
#include "../caddress.h"
#include "../cerror.h"
#include "../cmemory.h"
#include "../cvastring.h"

#include "cgtentry.h"
#include "cofmatch.h"
#include "cofbclist.h"
#include "cofinst.h"

class eGroupEntryOutOfMem : public eGroupEntryBase {}; // out of memory



class cgroupentry : public ciosrv {
public: // static methods

	static void test();

public: // data structures

	cofbclist buckets; 					// bucket list
	struct ofp_group_mod *group_mod; 	// pointer to group_mod_area (see below)

public: // methods

	/** constructor
	 */
	cgroupentry();

	/** destructor
	 */
	virtual
	~cgroupentry();

	/** assignment operator
	 */
	cgroupentry& operator= (const cgroupentry& fe);

	/** reset groupentry
	 *
	 */
	void
	reset();

	/** dump flow entry info
	 */
	const char*
	c_str();

	/** pack groupentry, i.e. add actions to group_mod structure ready for transmission
	 * @return length of group_mod structure including buckets
	 */
	size_t
	pack();



public: // setter methods for ofp_group_mod structure

	/**
	 */
	void
	set_command(uint16_t command);
	/**
	 */
	void set_type(uint8_t type);
	/**
	 */
	void set_group_id(uint32_t group_id);


private: // data structures

	std::string info;				// info string
	cmemory group_mod_area;			// group mod memory area
};


#endif
