/*
 * crofendpnt.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CROFENDPNT_H_
#define CROFENDPNT_H_

#include <inttypes.h>
#include <stdio.h>
#include <strings.h>
#include <map>
#include <set>
#include <deque>
#include <bitset>
#include <algorithm>

#include "rofl/common/ciosrv.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/csocket.h"
#include "rofl/common/logging.h"
#include "rofl/common/crofqueue.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/croflexception.h"

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/messages/cofmsg_hello.h"
#include "rofl/common/openflow/messages/cofmsg_echo.h"
#include "rofl/common/openflow/messages/cofmsg_error.h"
#include "rofl/common/openflow/messages/cofmsg_features.h"
#include "rofl/common/openflow/messages/cofmsg_config.h"
#include "rofl/common/openflow/messages/cofmsg_packet_out.h"
#include "rofl/common/openflow/messages/cofmsg_packet_in.h"
#include "rofl/common/openflow/messages/cofmsg_flow_mod.h"
#include "rofl/common/openflow/messages/cofmsg_flow_removed.h"
#include "rofl/common/openflow/messages/cofmsg_group_mod.h"
#include "rofl/common/openflow/messages/cofmsg_table_mod.h"
#include "rofl/common/openflow/messages/cofmsg_port_mod.h"
#include "rofl/common/openflow/messages/cofmsg_port_status.h"
#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/messages/cofmsg_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_flow_stats.h"
#include "rofl/common/openflow/messages/cofmsg_aggr_stats.h"
#include "rofl/common/openflow/messages/cofmsg_table_stats.h"
#include "rofl/common/openflow/messages/cofmsg_port_stats.h"
#include "rofl/common/openflow/messages/cofmsg_queue_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_port_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_experimenter_stats.h"
#include "rofl/common/openflow/messages/cofmsg_barrier.h"
#include "rofl/common/openflow/messages/cofmsg_queue_get_config.h"
#include "rofl/common/openflow/messages/cofmsg_role.h"
#include "rofl/common/openflow/messages/cofmsg_experimenter.h"
#include "rofl/common/openflow/messages/cofmsg_experimenter_stats.h"
#include "rofl/common/openflow/messages/cofmsg_async_config.h"
#include "rofl/common/openflow/messages/cofmsg_table_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_mod.h"
#include "rofl/common/openflow/messages/cofmsg_meter_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_config_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_features_stats.h"


namespace rofl {

class crofsock; // forward declaration

class crofsock_env {
public:
	virtual ~crofsock_env() {};
	virtual void handle_connect_refused(crofsock *endpnt) = 0;
	virtual void handle_connect_failed(crofsock *endpnt) = 0;
	virtual void handle_connected(crofsock *endpnt) = 0;
	virtual void handle_closed(crofsock *endpnt) = 0;
	virtual void handle_write(crofsock *endpnt) = 0;
	virtual void recv_message(crofsock *endpnt, rofl::openflow::cofmsg *msg) { delete msg; };
};

class eRofSockBase			: public RoflException {};
class eRofSockTxAgain		: public eRofSockBase {};
class eRofSockMsgTooLarge 	: public eRofSockBase {};

class crofsock :
		public ciosrv,
		public csocket_owner
{
	enum outqueue_type_t {
		QUEUE_OAM  = 0, // Echo.request/Echo.reply
		QUEUE_MGMT = 1, // all remaining packets, except ...
		QUEUE_FLOW = 2, // Flow-Mod/Flow-Removed
		QUEUE_PKT  = 3, // Packet-In/Packet-Out
		QUEUE_MAX,		// do not use
	};

	enum crofsock_event_t {
		EVENT_TXQUEUE		 	= 1,
	};

	enum crofsock_flag_t {
		FLAGS_CONGESTED 		= 1,
		FLAGS_CONGESTION_SOLVED = 2,
	};

	enum crofsock_state_t {
		STATE_INIT				= 0,
		STATE_CLOSED			= 1,
		STATE_CONNECTING		= 2,
		STATE_CONNECTED			= 3,
	};

public:

	/**
	 *
	 */
	crofsock(
			crofsock_env *env);

	/**
	 *
	 */
	virtual ~crofsock();

public:

	/**
	 *
	 */
	void
	accept(enum rofl::csocket::socket_type_t socket_type, cparams const& socket_params, int sd);

	/**
	 *
	 */
	void
	connect(enum rofl::csocket::socket_type_t socket_type, cparams const& socket_params);

	/**
	 *
	 */
	void
	reconnect();

	/**
	 *
	 */
	void
	close();


	/**
	 *
	 */
	csocket const&
	get_socket() const;


	/**
	 *
	 */
	unsigned int
	send_message(
			rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	bool
	is_established() const { return socket->is_established(); };

private:


	/**
	 * @brief	Private copy constructor for suppressing any copy attempt.
	 */
	crofsock(
			const crofsock& endpnt) :
		env(NULL),
		socket(NULL),
		state(STATE_INIT),
		fragment(NULL),
		msg_bytes_read(0),
		max_pkts_rcvd_per_round(DEFAULT_MAX_PKTS_RVCD_PER_ROUND)
	{};

	/**
	 * @brief	Private assignment operator.
	 */
	crofsock&
	operator= (
			const crofsock& endpnt)
	{ return *this; };

private:

	/**
	 *
	 */
	virtual void
	handle_event(
			cevent const &ev);

private:

	/**
	 *
	 */
	virtual void
	handle_listen(
			csocket& socket,
			int newsd) {
		rofl::logging::info << "[rofl-common][sock] new transport connection request received:" << std::endl << *this;
		// this should never happen, as passively opened sockets are handled outside of crofsock
	};

	/**
	 *
	 */
	virtual void
	handle_accepted(
			csocket& socket) {
		rofl::logging::info << "[rofl-common][sock] transport connection established (via accept):" << std::endl << *this;
		state = STATE_CONNECTED;
		// do not call handle_connected() here
	};

	/**
	 *
	 */
	virtual void
	handle_accept_refused(
			csocket& socket) {
		rofl::logging::info << "[rofl-common][sock] accepted transport connection refused:" << std::endl << *this;
		state = STATE_CLOSED;
		// do nothing
	};

	/**
	 *
	 */
	virtual void
	handle_connected(
			csocket& socket) {
		rofl::logging::info << "[rofl-common][sock] transport connection established (via connect):" << std::endl << *this;
		state = STATE_CONNECTED;
		env->handle_connected(this);
	};

	/**
	 *
	 */
	virtual void
	handle_connect_refused(
			csocket& socket) {
		rofl::logging::info << "[rofl-common][sock] transport connection refused:" << std::endl << *this;
		state = STATE_CLOSED;
		env->handle_connect_refused(this);
	};

	/**
	 *
	 */
	virtual void
	handle_connect_failed(
			csocket& socket) {
		rofl::logging::info << "[rofl-common][sock] transport connection failed:" << std::endl << *this;
		state = STATE_CLOSED;
		env->handle_connect_failed(this);
	};

	/**
	 *
	 */
	virtual void
	handle_read(
			csocket& socket);

	/**
	 *
	 */
	virtual void
	handle_write(
			csocket& socket) {
		flags.reset(FLAGS_CONGESTED);
		flags.set(FLAGS_CONGESTION_SOLVED);
		rofl::ciosrv::notify(rofl::cevent(EVENT_TXQUEUE));
		env->handle_write(this);
	};

	/**
	 *
	 */
	virtual void
	handle_closed(
			csocket& socket) {
		close();
	};

private:

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
			cmemory *mem, rofl::openflow::cofmsg **pmsg);

	/**
	 *
	 */
	void
	parse_of12_message(
			cmemory *mem, rofl::openflow::cofmsg **pmsg);

	/**
	 *
	 */
	void
	parse_of13_message(
			cmemory *mem, rofl::openflow::cofmsg **pmsg);

	/**
	 *
	 */
	void
	send_from_queue();

	/**
	 *
	 */
	void
	log_message(
			std::string const& text, rofl::openflow::cofmsg const& pmsg);

	/**
	 *
	 */
	void
	log_of10_message(
			rofl::openflow::cofmsg const& pmsg);

	/**
	 *
	 */
	void
	log_of12_message(
			rofl::openflow::cofmsg const& pmsg);

	/**
	 *
	 */
	void
	log_of13_message(
			rofl::openflow::cofmsg const& pmsg);


public:

	friend std::ostream&
	operator<< (std::ostream& os, crofsock const& rofsock) {
		os << indent(0) << "<crofsock: transport-connection-established: " << rofsock.get_socket().is_established() << ">" << std::endl;
		return os;
	};

private:

	// environment for this crofsock instance
	crofsock_env*				env;
	// socket abstraction towards peer
	csocket*					socket;
	// various flags for this crofsock instance
	std::bitset<32>				flags;
	// connection state
	enum crofsock_state_t		state;

	/*
	 * receiving messages
	 */

	// incomplete fragment message fragment received in last round
	cmemory*					fragment;
	// number of bytes already received for current message fragment
	unsigned int				msg_bytes_read;
	// read not more than this number of packets per round before rescheduling
	unsigned int				max_pkts_rcvd_per_round;
	// default value for max_pkts_rcvd_per_round
	static unsigned int const	DEFAULT_MAX_PKTS_RVCD_PER_ROUND = 16;

	/*
	 * scheduler and txqueues
	 */

	// QUEUE_MAX txqueues
	std::vector<crofqueue>		txqueues;
	// relative scheduling weights for txqueues
	std::vector<unsigned int>	weights;
};

} /* namespace rofl */

#endif /* CROFENDPNT_H_ */
