/*
 * cchannel.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CCHANNEL_H_
#define CCHANNEL_H_

#include <map>
#include <set>
#include <deque>
#include <algorithm>
#include <inttypes.h>
#include <stdio.h>
#include <strings.h>

#include "rofl/common/ciosrv.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/cvastring.h"
#include "rofl/common/cfsm.h"
#include "rofl/common/csocket.h"
#include "rofl/common/logging.h"
#include "rofl/common/thread_helper.h"

#include "openflow/messages/cofmsg.h"
#include "openflow/messages/cofmsg_hello.h"
#include "openflow/messages/cofmsg_echo.h"
#include "openflow/messages/cofmsg_error.h"
#include "openflow/messages/cofmsg_features.h"
#include "openflow/messages/cofmsg_config.h"
#include "openflow/messages/cofmsg_packet_in.h"
#include "openflow/messages/cofmsg_flow_removed.h"
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
#include "openflow/messages/cofmsg_barrier.h"
#include "openflow/messages/cofmsg_queue_get_config.h"
#include "openflow/messages/cofmsg_role.h"
#include "openflow/messages/cofmsg_experimenter.h"
#include "openflow/messages/cofmsg_experimenter_stats.h"
#include "openflow/messages/cofmsg_async_config.h"


namespace rofl {

namespace openflow {

class crofendpnt; // forward declaration

class crofendpnt_owner {
public:
	virtual ~crofendpnt_owner() {};
	virtual void handle_connect_refused(crofendpnt *endpnt) = 0;
	virtual void handle_open (crofendpnt *endpnt) = 0;
	virtual void handle_close(crofendpnt *endpnt) = 0;
	virtual void recv_message(crofendpnt *endpnt, cofmsg *msg) { delete msg; };
};

class crofendpnt :
		public ciosrv,
		public csocket_owner
{

	crofendpnt_owner					*endpnt_owner;
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
	crofendpnt(
			crofendpnt_owner *endpnt_owner, int sd, caddress const& ra);


	/**
	 *
	 */
	virtual ~crofendpnt();


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
	crofendpnt(crofendpnt const& endpnt) :
		endpnt_owner(NULL),
		socket(NULL),
		fragment(NULL),
		msg_bytes_read(0)
	{ };

	/**
	 * @brief	Private assignment operator.
	 */
	crofendpnt&
	operator= (crofendpnt const& endpnt) {
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
	operator<< (std::ostream& os, crofendpnt const& chan) {

		return os;
	};
};

} /* namespace openflow */

} /* namespace rofl */

#endif /* CCHANNEL_H_ */
