#ifndef ETHERSWITCH_H
#define ETHERSWITCH_H 1

#include <inttypes.h>
#include <map>

#include <rofl.h>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/crofdpt.h>
#include <rofl/platform/unix/cunixenv.h>
#include <rofl/platform/unix/cdaemon.h>
#include <rofl/common/cparams.h>

#include "cfibtable.h"
#include "cflowtable.h"

namespace rofl {
namespace examples {
namespace ethswctld {



/**
 * @ingroup common_howto_ethswctld
 *
 * @brief	A very simple controller for forwarding Ethernet flows.
 *
 * A simple controller application capable of switching Ethernet
 * frames in a flow-based manner.
 */
class cetherswitch : public rofl::crofbase {
public:

	/**
	 * @brief	Static main routine for class cetherswitch
	 *
	 * Runs main event loop. Does not return.
	 *
	 * @param argc number of arguments given to main function
	 * @param argv array of pointers to arguments given to main function
	 */
	static
	int
	run(
			int argc, char** argv);

private:

	/**
	 * @brief	cetherswitch constructor
	 */
	cetherswitch(
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap);

	/**
	 * @brief	cetherswitch destructor
	 */
	virtual
	~cetherswitch();


	/** @cond EXAMPLES */

private:

	/**
	 * @brief	Called after establishing the associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been established, i.e., its main connection has been accepted by the remote site.
	 *
	 * @param dpt datapath instance
	 */
	virtual void
	handle_dpt_open(
			rofl::crofdpt& dpt);

	/**
	 * @brief	Called after termination of associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been terminated, i.e., its main connection has been closed from the
	 * remote site. The rofl::crofdpt instance itself is not destroyed, unless
	 * its 'remove_on_channel_close' flag has been set to true during its
	 * construction.
	 *
	 * @param dpt datapath instance
	 */
	virtual void
	handle_dpt_close(
			const rofl::cdptid& dptid);

	/**
	 * @brief	OpenFlow Packet-In message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_packet_in(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_packet_in& msg);

	/**
	 * @brief	OpenFlow Flow-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_stats_reply& msg);

	/**
	 * @brief	Timer expired while waiting for OpenFlow Flow-Stats-Reply message.
	 *
	 * No Flow-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_flow_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid);

	/** @endcond */

private:

	/**
	 * @brief	Handler for timer events.
	 *
	 * To be overwritten by derived class. Default behaviour: event is ignored.
	 *
	 * @param opaque expired timer type
	 * @param data pointer to opaque data
	 */
	virtual void
	handle_timeout(
			int opaque,
			void* data = (void*)NULL);

	/**
	 * @brief	Dump an Ethernet frame received via an OpenFlow Packet-In message.
	 */
	void
	dump_packet_in(
		rofl::crofdpt& dpt,
		rofl::openflow::cofmsg_packet_in& msg);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cetherswitch& sw) {
		try {
			os << rofl::indent(0) << "<ethswitch dpid: "
					<< rofl::crofdpt::get_dpt(sw.dptid).get_dpid().str() << " >" << std::endl;
		} catch (rofl::eRofDptNotFound& e) {
			os << rofl::indent(0) << "<ethswitch dptid: " << sw.dptid << " >" << std::endl;
		}
		rofl::indent i(2);
		try {
			os << cfibtable::set_fib(sw.dptid);
			os << cflowtable::get_flowtable(sw.dptid);
		} catch (rofl::examples::ethswctld::exceptions::eFlowNotFound& e) {};
		return os;
	};

private:

	enum cetherswitch_timer_t {
		TIMER_DUMP_FIB          = 1,
		TIMER_GET_FLOW_STATS    = 2,
	};

	rofl::cdptid                dptid;

	rofl::ctimerid              timer_id_dump_fib;
	unsigned int                dump_fib_interval;
	static const unsigned int   DUMP_FIB_DEFAULT_INTERVAL = 60; // seconds

	rofl::ctimerid              timer_id_get_flow_stats;
	unsigned int                get_flow_stats_interval;
	static const unsigned int   GET_FLOW_STATS_DEFAULT_INTERVAL = 30; // seconds
};

}; // namespace ethswctld
}; // namespace examples
}; // namespace rofl

#endif
