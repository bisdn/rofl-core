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

/**
 * @ingroup common_devel_workflow
 * @brief	Environment expected by a rofl::crofsock object
 */
class crofsock_env {
public:
	virtual ~crofsock_env() {};

protected:

	friend class crofsock;

	virtual void
	handle_connect_refused(crofsock& endpnt) = 0;

	virtual void
	handle_connect_failed(crofsock& endpnt) = 0;

	virtual void
	handle_connected(crofsock& endpnt) = 0;

	virtual void
	handle_closed(crofsock& endpnt) = 0;

	virtual void
	handle_write(crofsock& endpnt) = 0;

	virtual void
	recv_message(crofsock& endpnt, rofl::openflow::cofmsg *msg) = 0;
};

class eRofSockBase			: public RoflException {};
class eRofSockTxAgain		: public eRofSockBase {};
class eRofSockMsgTooLarge 	: public eRofSockBase {};

/**
 * @ingroup common_devel_workflow
 * @brief	A socket capable of talking OpenFlow via TCP and vice versa
 */
class crofsock :
		public ciosrv,
		public csocket_env
{
	enum outqueue_type_t {
		QUEUE_OAM  = 0, // Echo.request/Echo.reply
		QUEUE_MGMT = 1, // all remaining packets, except ...
		QUEUE_FLOW = 2, // Flow-Mod/Flow-Removed
		QUEUE_PKT  = 3, // Packet-In/Packet-Out
		QUEUE_MAX,		// do not use
	};

	enum crofsock_event_t {
		EVENT_NONE				= 0,
		EVENT_CONNECT			= 1,
		EVENT_CONNECT_FAILED	= 2,
		EVENT_CONNECT_REFUSED	= 3,
		EVENT_CONNECTED			= 4,
		EVENT_ACCEPT			= 5,
		EVENT_ACCEPT_REFUSED	= 6,
		EVENT_ACCEPTED			= 7,
		EVENT_PEER_DISCONNECTED		= 8,
		EVENT_LOCAL_DISCONNECT		= 9,
		EVENT_CONGESTION_SOLVED	= 10,
	};

	enum crofsock_flag_t {
		FLAGS_CONGESTED 		= 1,
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
	virtual
	~crofsock();

public:

	/**
	 *
	 */
	void
	accept(
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& socket_params,
			int sd) {
		RwLock rwlock(rofsock_lock, RwLock::RWLOCK_WRITE);
		this->socket_type = socket_type;
		this->socket_params = socket_params;
		this->sd = sd;
		run_engine(EVENT_ACCEPT);
	};

	/**
	 *
	 */
	void
	connect(
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& socket_params) {
		RwLock rwlock(rofsock_lock, RwLock::RWLOCK_WRITE);
		this->socket_type = socket_type;
		this->socket_params = socket_params;
		run_engine(EVENT_CONNECT);
	};

	/**
	 *
	 */
	void
	reconnect() {
		RwLock rwlock(rofsock_lock, RwLock::RWLOCK_WRITE);
		run_engine(EVENT_CONNECT);
	};

	/**
	 *
	 */
	void
	close() {
		RwLock rwlock(rofsock_lock, RwLock::RWLOCK_WRITE);
		run_engine(EVENT_LOCAL_DISCONNECT);
	};

	/**
	 *
	 */
	csocket const&
	get_socket() const
	{ return *socket; /* FIXME */ };

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
	is_established() const
	{ return socket->is_established(); };

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
		max_pkts_rcvd_per_round(DEFAULT_MAX_PKTS_RVCD_PER_ROUND),
		socket_type(rofl::csocket::SOCKET_TYPE_UNKNOWN),
		sd(-1)
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
		// this should never happen, as passively opened sockets are handled outside of crofsock
	};

	/**
	 *
	 */
	virtual void
	handle_accept_refused(
			csocket& socket) {
		run_engine(EVENT_ACCEPT_REFUSED);
	};

	/**
	 *
	 */
	virtual void
	handle_accepted(
			csocket& socket) {
		run_engine(EVENT_ACCEPTED);
	};

	/**
	 *
	 */
	virtual void
	handle_connect_refused(
			csocket& socket) {
		run_engine(EVENT_CONNECT_REFUSED);
	};

	/**
	 *
	 */
	virtual void
	handle_connect_failed(
			csocket& socket) {
		run_engine(EVENT_CONNECT_FAILED);
	};

	/**
	 *
	 */
	virtual void
	handle_connected(
			csocket& socket) {
		run_engine(EVENT_CONNECTED);
	};

	/**
	 *
	 */
	virtual void
	handle_write(
			csocket& socket) {
		run_engine(EVENT_CONGESTION_SOLVED);
	};

	/**
	 *
	 */
	virtual void
	handle_closed(
			csocket& socket) {
		run_engine(EVENT_PEER_DISCONNECTED);
	};

	/**
	 *
	 */
	virtual void
	handle_read(
			csocket& socket);

private:

	/**
	 *
	 */
	void
	run_engine(crofsock_event_t event = EVENT_NONE) {
		if (EVENT_NONE != event) {
			events.push_back(event);
		}

		while (not events.empty()) {
			enum crofsock_event_t event = events.front();
			events.pop_front();

			switch (event) {
			case EVENT_CONNECT:				event_connect();			break;
			case EVENT_CONNECT_FAILED:		event_connect_failed();		break;
			case EVENT_CONNECT_REFUSED: 	event_connect_refused();	break;
			case EVENT_CONNECTED: 			event_connected(); 			break;
			case EVENT_ACCEPT:				event_accept();				break;
			case EVENT_ACCEPT_REFUSED:		event_accept_refused();		break;
			case EVENT_ACCEPTED:			event_accepted();			break;
			case EVENT_PEER_DISCONNECTED:	event_peer_disconnected();	return;
			case EVENT_LOCAL_DISCONNECT:	event_local_disconnect();	return;
			case EVENT_CONGESTION_SOLVED:	event_congestion_solved();	break;

			default: {
				rofl::logging::error << "[rofl-common][crofsock] unknown event seen, internal error" << std::endl << *this;
			};
			}
		}
	}

	/**
	 *
	 */
	void
	event_connect() {
		switch (state) {
		case STATE_INIT:
		case STATE_CLOSED: {
			rofl::logging::debug2 << "[rofl-common][crofsock] EVENT-CONNECT => entering state -connecting-" << std::endl;
			state = STATE_CONNECTING;
			if (socket)
				delete socket;
			ciosrv::cancel_all_timers();
			ciosrv::cancel_all_events();
			(socket = csocket::csocket_factory(socket_type, this))->connect(socket_params);
		} break;
		case STATE_CONNECTING: {
			// do nothing, we are already connecting ...
		} break;
		case STATE_CONNECTED: {
			run_engine(EVENT_LOCAL_DISCONNECT);
			run_engine(EVENT_CONNECT);
		} break;
		default: {

		};
		};
	};

	/**
	 *
	 */
	void
	event_connect_failed() {
		rofl::logging::debug2 << "[rofl-common][crofsock] EVENT-CONNECT-FAILED => entering state -closed-" << std::endl;
		state = STATE_CLOSED;
		if (env) env->handle_connect_failed(*this);
	};

	/**
	 *
	 */
	void
	event_connect_refused() {
		rofl::logging::debug2 << "[rofl-common][crofsock] EVENT-CONNECT-REFUSED => entering state -closed-" << std::endl;
		state = STATE_CLOSED;
		if (env) env->handle_connect_refused(*this);
	};

	/**
	 *
	 */
	void
	event_connected() {
		rofl::logging::debug2 << "[rofl-common][crofsock] EVENT-CONNECTED => entering state -connected-" << std::endl;
		state = STATE_CONNECTED;
		if (env) env->handle_connected(*this);
	};

	/**
	 *
	 */
	void
	event_accept() {
		switch (state) {
		case STATE_INIT:
		case STATE_CLOSED: {
			rofl::logging::debug2 << "[rofl-common][crofsock] EVENT-ACCEPT => entering state -connected-" << std::endl;
			state = STATE_CONNECTED;
			if (socket)
				delete socket;

			(socket = csocket::csocket_factory(socket_type, this))->accept(socket_params, sd);
		} break;
		default: {

		};
		}
	}

	/**
	 *
	 */
	void
	event_accept_refused() {
		rofl::logging::debug2 << "[rofl-common][crofsock] EVENT-ACCEPT-REFUSED => entering state -closed-" << std::endl;
		state = STATE_CLOSED;
	};

	/**
	 *
	 */
	void
	event_accepted() {
		rofl::logging::debug2 << "[rofl-common][crofsock] EVENT-ACCEPTED => entering state -connected-" << std::endl;
		state = STATE_CONNECTED;
		// do not call handle_connected() here
	};

	/**
	 *
	 */
	void
	event_peer_disconnected() {
		rofl::logging::debug2 << "[rofl-common][crofsock] EVENT-PEER-DISCONNECTED => entering state -closed-" << std::endl;
		__close();
		if (env) env->handle_closed(*this);
	};

	/**
	 *
	 */
	void
	event_local_disconnect() {
		rofl::logging::debug2 << "[rofl-common][crofsock] EVENT-LOCAL-DISCONNECT => entering state -closed-" << std::endl;
		__close();
		if (socket) socket->close();
	};

	/**
	 *
	 */
	void
	event_congestion_solved() {
		flags.reset(FLAGS_CONGESTED);
		rofl::ciosrv::notify(rofl::cevent(EVENT_CONGESTION_SOLVED));
	};

private:

	/**
	 * @brief
	 */
	void
	__close() {
		state = STATE_CLOSED;
		if (fragment) {
			delete fragment; fragment = NULL;
		}
		for (std::vector<crofqueue>::iterator
				it = txqueues.begin(); it != txqueues.end(); ++it) {
			(*it).clear();
		}
		ciosrv::cancel_all_timers();
		ciosrv::cancel_all_events();
	};

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

	std::string
	str() const {
		std::stringstream ss;
		if (STATE_INIT == state) {
			ss << "state: -INIT- ";
		} else
		if (STATE_CLOSED == state) {
			ss << "state: -CLOSED- ";
		} else
		if (STATE_CONNECTING == state) {
			ss << "state: -CONNECTING- ";
		} else
		if (STATE_CONNECTED == state) {
			ss << "state: -CONNECTED- ";
		}
		return ss.str();
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
	std::vector<unsigned int>	txweights;

	enum rofl::csocket::socket_type_t
								socket_type;

	rofl::cparams 				socket_params;

	int							sd;

	std::deque<enum crofsock_event_t>
								events;

	PthreadRwLock				rofsock_lock;
};

} /* namespace rofl */

#endif /* CROFENDPNT_H_ */
