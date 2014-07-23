/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETHERSWITCH_H
#define ETHERSWITCH_H 1

#include <map>
#include "rofl/common/caddress.h"
#include "rofl/common/crofbase.h"
#include "rofl/common/crofdpt.h"

#include <cfib.h>

using namespace rofl;

namespace etherswitch
{

class ethswitch :
		public crofbase
{
private:

#if 0
	struct fibentry_t {
		uint32_t 		port_no;	// port where a certain is attached
		time_t 			timeout;	// timeout event for this FIB entry
	};

	// a very simple forwarding information base
	std::map<crofdpt*, std::map<uint16_t, std::map<cmacaddr, struct fibentry_t> > > fib;

	unsigned int 		fib_check_timeout; 		// periodic timeout for removing expired FIB entries
	unsigned int		flow_stats_timeout;		// periodic timeout for requesting flow stats
	unsigned int		fm_delete_all_timeout;	// periodic purging of all FLOW-MODs
#endif

	enum etherswitch_timer_t {
		ETHSWITCH_TIMER_BASE 					= ((0x6271)),
		ETHSWITCH_TIMER_FIB 					= ((ETHSWITCH_TIMER_BASE) + 1),
		ETHSWITCH_TIMER_FLOW_STATS 				= ((ETHSWITCH_TIMER_BASE) + 2),
		ETHSWITCH_TIMER_FLOW_MOD_DELETE_ALL 	= ((ETHSWITCH_TIMER_BASE) + 3),
	};

	rofl::crofdpt *dpt;

public:

	ethswitch(rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap);

	virtual
	~ethswitch();

	virtual void
	handle_timeout(int opaque, void *data = (void*)0);

	virtual void
	handle_dpt_open(crofdpt& dpt);

	virtual void
	handle_dpt_close(crofdpt& dpt);

	virtual void
	handle_packet_in(crofdpt& dpt, const cauxid& auxid, rofl::openflow::cofmsg_packet_in& msg);

	virtual void
	handle_flow_stats_reply(crofdpt& dpt, const cauxid& auxid, rofl::openflow::cofmsg_flow_stats_reply& msg);

private:

	void
	request_flow_stats();
};

}; // end of namespace

#endif
