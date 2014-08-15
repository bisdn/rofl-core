#ifndef ETHERSWITCH_H
#define ETHERSWITCH_H 1

#include <map>
#include <rofl.h>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/crofdpt.h>

#include "cfib.h"

using namespace rofl;

namespace etherswitch{

/**
* A controller that implements normal L2 forwarding (incl. VLANs)
*/ 
class ethswitch : public crofbase
{

private:

	enum etherswitch_timer_t {
		ETHSWITCH_TIMER_BASE 			= ((0x6271)),
		ETHSWITCH_TIMER_FIB 			= ((ETHSWITCH_TIMER_BASE) + 1),
		ETHSWITCH_TIMER_FLOW_STATS 		= ((ETHSWITCH_TIMER_BASE) + 2),
		ETHSWITCH_TIMER_FLOW_MOD_DELETE_ALL 	= ((ETHSWITCH_TIMER_BASE) + 3),
	};

public:

	ethswitch(rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap);

	virtual
	~ethswitch();

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
	dump_packet_in(
		rofl::crofdpt& dpt,
		rofl::openflow::cofmsg_packet_in& msg,
		rofl::cmacaddr& eth_src,
		rofl::cmacaddr& eth_dst);

	void
	install_drop_flowmod(
		crofdpt& dpt,
		const cauxid& auxid,
		rofl::openflow::cofmsg_packet_in& msg);

	void
	install_flood_flowmod(
		crofdpt& dpt,
		const cauxid& auxid,
		rofl::openflow::cofmsg_packet_in& msg);

	rofl_result_t	
	install_fwd_flowmod(
		crofdpt& dpt,
		const cauxid& auxid,
		rofl::openflow::cofmsg_packet_in& msg,
		rofl::cmacaddr& eth_src,
		rofl::cmacaddr& eth_dst);


	void
	request_flow_stats();
};

}; // etherswitch namespace

#endif
