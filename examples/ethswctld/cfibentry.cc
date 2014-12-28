/*
 * cfibentry.cc
 *
 *  Created on: 15.07.2013
 *      Author: andreas
 */

#include "cfibentry.h"

using namespace etherswitch;

cfibentry::cfibentry(
		cfibentry_env *fibenv,
		const rofl::cdptid& dptid,
		const rofl::caddress_ll& hwaddr,
		uint32_t port_no) :
		fibenv(fibenv),
		dptid(dptid),
		port_no(port_no),
		hwaddr(hwaddr),
		entry_timeout(CFIBENTRY_DEFAULT_TIMEOUT),
		expiration_timer_id()
{
	expiration_timer_id = register_timer(CFIBENTRY_ENTRY_EXPIRED, entry_timeout);
	rofl::logging::notice << "[cfibentry] created" << std::endl << *this;
}


cfibentry::~cfibentry()
{
	rofl::logging::notice << "[cfibentry] deleted" << std::endl << *this;
}



void
cfibentry::handle_timeout(int opaque, void* data)
{
	switch (opaque) {
	case CFIBENTRY_ENTRY_EXPIRED: {
		fibenv->fib_timer_expired(hwaddr);
	} return;
	}
}



void
cfibentry::set_port_no(uint32_t port_no)
{
	if (port_no != this->port_no) {
		this->port_no = port_no;
		fibenv->fib_port_update(*this);
	}

	try {
		reset_timer(expiration_timer_id, entry_timeout);
	} catch (rofl::eTimersBase& e) {
		expiration_timer_id = register_timer(CFIBENTRY_ENTRY_EXPIRED, entry_timeout);
	}
}




