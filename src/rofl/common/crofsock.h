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
#include "rofl/common/cfsm.h"
#include "rofl/common/csocket.h"
#include "rofl/common/logging.h"
#include "rofl/common/thread_helper.h"

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


namespace rofl {

class crofsock; // forward declaration

class crofsock_env {
public:
	virtual ~crofsock_env() {};
	virtual void handle_connect_refused(crofsock *endpnt) = 0;
	virtual void handle_connected(crofsock *endpnt) = 0;
	virtual void handle_closed(crofsock *endpnt) = 0;
	virtual void recv_message(crofsock *endpnt, rofl::openflow::cofmsg *msg) { delete msg; };
};

class crofsock :
		public ciosrv,
		public csocket_owner
{

	crofsock_env						*env;
	csocket								*socket;		// socket abstraction towards peer
	cmemory								*fragment;
	unsigned int						msg_bytes_read;

	enum outqueue_type_t {
		QUEUE_MGMT = 0, // all packets, except ...
		QUEUE_FLOW = 1, // Flow-Mod/Flow-Removed
		QUEUE_PKT  = 2, // Packet-In/Packet-Out
		QUEUE_MAX,		// do not use
	};

	struct rofqueue {

		static unsigned int const DEFAULT_OUTQUEUE_SIZE_THRESHOLD = 8;

		PthreadRwLock					rwlock;
		std::deque<rofl::openflow::cofmsg*>				queue;
		unsigned int					limit; // #msgs sent from queue before rescheduling
	public:
		/**
		 *
		 */
		rofqueue(unsigned int limit = DEFAULT_OUTQUEUE_SIZE_THRESHOLD) :
			limit(limit) {};

		/**
		 *
		 */
		~rofqueue() { clear(); };

		/**
		 *
		 */
		void
		clear() {
			RwLock(rwlock, RwLock::RWLOCK_WRITE);
			for (std::deque<rofl::openflow::cofmsg*>::iterator
					it = queue.begin(); it != queue.end(); ++it) {
				delete (*it);
			}
			queue.clear();
		};

		/**
		 *
		 */
		void
		store(rofl::openflow::cofmsg *msg) {
			RwLock(rwlock, RwLock::RWLOCK_WRITE);
			queue.push_back(msg);
		};

		/**
		 *
		 */
		rofl::openflow::cofmsg*
		retrieve() {
			RwLock(rwlock, RwLock::RWLOCK_WRITE);
			if (queue.empty())
				return NULL;
			rofl::openflow::cofmsg *msg = queue.front();
			queue.pop_front();
			return msg;
		};

		/**
		 *
		 */
		unsigned int
		get_limit() {
			return (queue.size() > limit) ? limit : queue.size();
		};

		/**
		 *
		 */
		void
		set_limit(unsigned int limit) { this->limit = limit; };

		/**
		 *
		 */
		size_t
		size() {
			RwLock(rwlock, RwLock::RWLOCK_READ);
			return queue.size();
		};

		/**
		 *
		 */
		bool
		empty() {
			RwLock(rwlock, RwLock::RWLOCK_READ);
			return queue.empty();
		};
	};

	std::vector<rofqueue>				outqueues;
					// 0 => all non-asynchronous messages
					// 1 => queue for Packet-In, Packet-Out, Flow-Mod, Flow-Removed

	enum crofsock_event_t {
		CROFSOCK_EVENT_WAKEUP = 1,
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
	csocket&
	get_socket();


	/**
	 *
	 */
	void
	send_message(
			rofl::openflow::cofmsg *msg);

private:


	/**
	 * @brief	Private copy constructor for suppressing any copy attempt.
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
	handle_listen(
			csocket& socket,
			int newsd);

	/**
	 *
	 */
	virtual void
	handle_accepted(
			csocket& socket);

	/**
	 *
	 */
	virtual void
	handle_accept_refused(
			csocket& socket);

	/**
	 *
	 */
	virtual void
	handle_connected(
			csocket& socket);

	/**
	 *
	 */
	virtual void
	handle_connect_refused(
			csocket& socket);

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
			csocket& socket);

	/**
	 *
	 */
	virtual void
	handle_closed(
			csocket& socket);

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
		os << indent(0) << "<crofsock >" << std::endl;
		{ indent i(2); os << (rofsock.socket); }
		return os;
	};
};

} /* namespace rofl */

#endif /* CROFENDPNT_H_ */
