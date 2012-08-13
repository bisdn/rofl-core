/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CUTBASE_H
#define CUTBASE_H 1

#include <set>
#include <string>
#include <algorithm>

#include "cfwdelem.h"
#include "cport.h"
#include "cerror.h"
#include "cvastring.h"
#include "openflow/cflowentry.h"

class eUTBase : public cerror {}; // base error class cunittest
class eUTFailed : public eUTBase {}; // unittest failed
class eUTNotFound : public eUTBase {}; // element not found


class cutbase : public cfwdelem, public cport::cport_owner {
public: // static

	static std::set<cutbase*> utlist;

public: // static

	enum unittest_timer_t {
		TIMER_UT_BASE = (0x0031 << 16), // base timer value
	};


public: // data structures



public: // methods

	/** constructor
	 */
	cutbase(std::string name);

	/** destructor
	 */
	virtual
	~cutbase();

	/** start test
	 */
	virtual void
	test_start(std::string name) = 0;

	/** stop test
	 */
	virtual void
	test_stop() = 0;

	/** reset (clear all rules on all data path elements)
	 */
	virtual void
	test_reset();

	/** return description string of all available tests
	 *
	 */
	virtual std::string
	test_descs() = 0;

	/** dump info
	 */
	virtual const char*
	c_str();

public: // overloaded from cfwdelem::ciosrv

	/**
	 */
	virtual void
	handle_timeout(int opaque);

public: // overloaded from cport::cport_owner

	/**
	 */
	virtual void
	handle_cport_packet_in(cport *port, cpacket* pack);

private: // data structures

	std::string info;

};

#endif
