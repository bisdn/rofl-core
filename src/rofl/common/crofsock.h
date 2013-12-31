/*
 * crofendpnt.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CROFENDPNT_H_
#define CROFENDPNT_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#include <stdio.h>
#include <strings.h>
#ifdef __cplusplus
}
#endif

#include <map>
#include <set>
#include <deque>
#include <algorithm>

#include "ciosrv.h"
#include "cmemory.h"
#include "cfsm.h"
#include "csocket.h"
#include "logging.h"
#include "thread_helper.h"

#include "openflow/messages/cofmsg.h"
#include "openflow/messages/cofmsg_hello.h"
#include "openflow/messages/cofmsg_echo.h"
#include "openflow/messages/cofmsg_error.h"
#include "openflow/messages/cofmsg_features.h"
#include "openflow/messages/cofmsg_config.h"
#include "openflow/messages/cofmsg_packet_out.h"
#include "openflow/messages/cofmsg_packet_in.h"
#include "openflow/messages/cofmsg_flow_mod.h"
#include "openflow/messages/cofmsg_flow_removed.h"
#include "openflow/messages/cofmsg_group_mod.h"
#include "openflow/messages/cofmsg_table_mod.h"
#include "openflow/messages/cofmsg_port_mod.h"
#include "openflow/messages/cofmsg_port_status.h"
#include "openflow/messages/cofmsg_stats.h"
#include "openflow/messages/cofmsg_desc_stats.h"
#include "openflow/messages/cofmsg_flow_stats.h"
#include "openflow/messages/cofmsg_aggr_stats.h"
#include "openflow/messages/cofmsg_table_stats.h"
#include "openflow/messages/cofmsg_port_stats.h"
#include "openflow/messages/cofmsg_queue_stats.h"
#include "openflow/messages/cofmsg_group_stats.h"
#include "openflow/messages/cofmsg_group_desc_stats.h"
#include "openflow/messages/cofmsg_group_features_stats.h"
#include "openflow/messages/cofmsg_experimenter_stats.h"
#include "openflow/messages/cofmsg_barrier.h"
#include "openflow/messages/cofmsg_queue_get_config.h"
#include "openflow/messages/cofmsg_role.h"
#include "openflow/messages/cofmsg_experimenter.h"
#include "openflow/messages/cofmsg_experimenter_stats.h"
#include "openflow/messages/cofmsg_async_config.h"


namespace rofl {

namespace openflow {

class crofsock; // forward declaration

class crofsock_env {
public:
	virtual ~crofsock_env() {};
	virtual void handle_connect_refused(crofsock *endpnt) = 0;
	virtual void handle_open (crofsock *endpnt) = 0;
	virtual void handle_close(crofsock *endpnt) = 0;
	virtual void recv_message(crofsock *endpnt, cofmsg *msg) { delete msg; };
};

class crofsock :
		public ciosrv,
		public csocket_owner
{

	crofsock_env						*env;
	csocket								*socket;		// socket abstraction towards peer
	cmemory								*fragment;
	unsigned int						msg_bytes_read;

	PthreadRwLock						outqueue_rwlock;
	std::deque<cofmsg*>					outqueue;

	enum crofendpnt_event_t {
		CROFENDPNT_EVENT_WAKEUP = 1,
	};

public:

	/**
	 *
	 */
	crofsock(
			crofsock_env *endpnt_owner, int sd, caddress const& ra);


	/**
	 *
	 */
	virtual ~crofsock();


	/**
	 *
	 */
	csocket&
	get_socket();


	/**
	 *
	 */
	void
	send_message(
			cofmsg *msg);

private:


	/**
	 * @brief	Private copy constructor to suppress any copy attempt.
	 */
	crofsock(crofsock const& endpnt) :
		env(NULL),
		socket(NULL),
		fragment(NULL),
		msg_bytes_read(0)
	{ };

	/**
	 * @brief	Private assignment operator.
	 */
	crofsock&
	operator= (crofsock const& endpnt) {
		return *this;
	}


private:


	/**
	 *
	 */
	virtual void
	handle_event(
			cevent const &ev);


	/**
	 *
	 */
	virtual void
	handle_accepted(
			csocket *socket,
			int newsd,
			caddress const& ra);


	/**
	 *
	 */
	virtual void
	handle_connected(
			csocket *socket,
			int sd);


	/**
	 *
	 */
	virtual void
	handle_connect_refused(
			csocket *socket,
			int sd);


	/**
	 *
	 */
	virtual void
	handle_read(
			csocket *socket,
			int sd);


	/**
	 *
	 */
	virtual void
	handle_closed(
			csocket *socket,
			int sd);

	/**
	 *
	 */
	void
	parse_message(
			cmemory *mem);

	/**
	 *
	 */
	void
	parse_of10_message(
			cmemory *mem, cofmsg **pmsg);

	/**
	 *
	 */
	void
	parse_of12_message(
			cmemory *mem, cofmsg **pmsg);

	/**
	 *
	 */
	void
	parse_of13_message(
			cmemory *mem, cofmsg **pmsg);

	/**
	 *
	 */
	void
	send_from_queue();

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofsock const& chan) {

		return os;
	};
};

} /* namespace openflow */

} /* namespace rofl */

#endif /* CROFENDPNT_H_ */
