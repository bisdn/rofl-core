/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFIBENTRY_H
#define CFIBENTRY_H 1

#include <set>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include "../common/ciosrv.h"
#include "../common/cvastring.h"

namespace rofl
{

class cfibentry_owner; // forward declaration

class cfibentry : public ciosrv {
public: // data structures

	cfibentry_owner *owner;
	std::set<cfibentry*> *fib_table;

	uint64_t from;
	uint64_t to;
	uint32_t port_no;
	uint16_t timeout;

public:
	/** constructor
	 *
	 */
	cfibentry(
			cfibentry_owner *owner,
			std::set<cfibentry*> *fib_table,
			uint64_t from,
			uint64_t to,
			uint32_t port_no,
			uint16_t timeout);

	/** destructor
	 *
	 */
	virtual
	~cfibentry();

	/** refresh entry
	 */
	void
	refresh(uint16_t timeout);

	/** dump info
	 */
	const char*
	c_str();

protected: // overloaded from ciosrv

	/**
	 *
	 */
	virtual void
	handle_timeout(int opaque);

private: // data structures

	enum cfibentry_timer_t {
		TIMER_FIBENTRY_BASE = 0x288,
		TIMER_FIBENTRY_TIMEOUT = ((TIMER_FIBENTRY_BASE << 16) | (0x0001)),
	};

	std::string info;

public: // auxiliary classes

	class cfibentry_find {
		uint64_t from;
		uint64_t to;
		uint32_t port_no;
	public:
		cfibentry_find(uint64_t __from, uint64_t __to, uint32_t __port_no) :
			from(__from), to(__to), port_no(__port_no) {};
		bool operator() (cfibentry const* fe) {
			return ((fe->from == from) && (fe->to == to) && (fe->port_no == port_no));
		};
	};

	class cfibentry_map_port {
		uint64_t from;
		uint64_t to;
	public:
		cfibentry_map_port(uint64_t __from, uint64_t __to) :
			from(__from), to(__to) {};
		bool operator() (cfibentry const* fe) {
			return ((fe->from == from) && (fe->to == to));
		};
	};
};

class cfibentry_owner {
public:
	virtual ~cfibentry_owner() {};
	virtual void fibentry_timeout(cfibentry *fibentry) = 0;
};

}; // end of namespace

#endif
