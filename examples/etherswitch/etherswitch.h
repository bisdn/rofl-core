#ifndef ETHERSWITCH_H
#define ETHERSWITCH_H 1

#include <map>
#include "rofl/common/cmacaddr.h"
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

public:

	ethswitch();

	virtual
	~ethswitch();

	virtual void
	handle_timeout(int opaque);

	virtual void
	handle_dpath_open(crofdpt& dpt);

	virtual void
	handle_dpath_close(crofdpt& dpt);

	virtual void
	handle_packet_in(crofdpt& dpt, cofmsg_packet_in& msg, uint8_t aux_id = 0);

	virtual void
	handle_flow_stats_reply(crofdpt& dpt, cofmsg_flow_stats_reply& msg, uint8_t aux_id = 0);

private:

	void
	request_flow_stats();
};

}; // end of namespace

#endif
