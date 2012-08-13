/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cfibentry.h"

cfibentry::cfibentry(
		cfibentry_owner *__owner,
		std::set<cfibentry*> *__fib_table,
		uint64_t __from,
		uint64_t __to,
		uint32_t __port_no,
		uint16_t __timeout) :
		owner(__owner),
		fib_table(__fib_table),
		from(__from),
		to(__to),
		port_no(__port_no),
		timeout(2 * __timeout)
{
	register_timer(TIMER_FIBENTRY_TIMEOUT, timeout);

	WRITELOG(CFIBENTRY, DBG, "cfibentry(%p)::cfibentry() %s", this, c_str());

	fib_table->insert(this);
}


cfibentry::~cfibentry()
{
	fib_table->erase(this);

	WRITELOG(CFIBENTRY, DBG, "cfibentry(%p)::~cfibentry()", this);

	owner->fibentry_timeout(this);
}


void
cfibentry::handle_timeout(int opaque)
{
	switch (opaque) {
	case TIMER_FIBENTRY_TIMEOUT:
		WRITELOG(CFIBENTRY, DBG, "cfibentry(%p)::~handle_timeout() -FIBENTRY-TIMEOUT-", this);
		delete this;
		return;
	}
}


void
cfibentry::refresh(uint16_t __timeout)
{
	WRITELOG(CFIBENTRY, DBG, "cfibentry(%p)::refresh() %s", this, c_str());
	timeout = __timeout;
	reset_timer(TIMER_FIBENTRY_TIMEOUT, timeout);
}


const char*
cfibentry::c_str()
{
	cvastring vas;
	info.assign(vas("cfibentry(%p): from=0x%llx to=0x%llx via port=%d timeout=%d",
			this, from, to, port_no, timeout));
	return info.c_str();
}

