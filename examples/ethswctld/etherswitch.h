#ifndef ETHERSWITCH_H
#define ETHERSWITCH_H 1

#include <map>
#include <rofl.h>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/crofdpt.h>

#include "cfibtable.h"
#include "cflowtable.h"

//using namespace rofl;

namespace etherswitch{

/**
* A controller that implements normal L2 forwarding (incl. VLANs)
*/ 
class ethswitch : public rofl::crofbase {
public:

	ethswitch(
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap);

	virtual
	~ethswitch();

	virtual void
	handle_dpt_open(
			rofl::crofdpt& dpt);

	virtual void
	handle_dpt_close(
			rofl::crofdpt& dpt);

	virtual void
	handle_packet_in(
			rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_packet_in& msg);

	virtual void
	handle_flow_stats_reply(
			rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_flow_stats_reply& msg);

private:

	/**
	 *
	 */
	virtual void
	handle_timeout(int opaque, void* data = (void*)NULL);


	void
	dump_packet_in(
		rofl::crofdpt& dpt,
		rofl::openflow::cofmsg_packet_in& msg);

	void
	request_flow_stats();

public:

	friend std::ostream&
	operator<< (std::ostream& os, const ethswitch& sw) {
		os << rofl::indent(0) << "<ethswitch dpid: "
				<< rofl::crofdpt::get_dpt(sw.dptid).get_dpid() << " >" << std::endl;
		rofl::indent i(2);
		os << cfibtable::get_fib(sw.dptid);
		os << cflowtable::get_flowtable(sw.dptid);
		return os;
	};

private:

	enum etherswitch_timer_t {
		TIMER_DUMP_FIB = 1,
	};

	rofl::cdptid				dptid;
	rofl::ctimerid 				timer_id_dump_fib;
	unsigned int				dump_fib_interval;
	static const unsigned int	DUMP_FIB_DEFAULT_INTERNAL = 60; // seconds
};

}; // etherswitch namespace

#endif
