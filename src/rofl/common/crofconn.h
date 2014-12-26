/*
 * crofchan.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CROFCONN_H_
#define CROFCONN_H_

#include <inttypes.h>
#include <bitset>
#include <set>

#include "rofl/common/ciosrv.h"
#include "rofl/common/crofsock.h"
#include "rofl/common/openflow/cofhelloelems.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/crandom.h"
#include "rofl/common/csegmentation.h"
#include "rofl/common/ctimerid.h"
#include "rofl/common/cauxid.h"
#include "rofl/common/cthread.h"
#include "rofl/common/crofqueue.h"

namespace rofl {

class eRofConnBase 					: public RoflException {};
class eRofConnXidSpaceExhausted		: public eRofConnBase {};
class eRofConnBusy					: public eRofConnBase {}; // connection already established
class eRofConnNotFound				: public eRofConnBase {};

class crofconn; // forward declaration

/**
 * @addtogroup common_workflow
 * @brief	Environment expected by a rofl::crofconn object
 */
class crofconn_env {
	static std::set<crofconn_env*> rofconn_envs;
public:

	/**
	 *
	 */
	static crofconn_env&
	set_env(crofconn_env* env) {
		if (crofconn_env::rofconn_envs.find(env) == crofconn_env::rofconn_envs.end()) {
			throw eRofConnNotFound();
		}
		return *(env);
	};

	/**
	 *
	 */
	static bool
	has_env(crofconn_env* env) {
		return (not (crofconn_env::rofconn_envs.find(env) == crofconn_env::rofconn_envs.end()));
	};

public:

	/**
	 *
	 */
	crofconn_env() {
		crofconn_env::rofconn_envs.insert(this);
	};

	/**
	 *
	 */
	virtual ~crofconn_env() {
		crofconn_env::rofconn_envs.erase(this);
	};

protected:

	friend class crofconn;

	/**
	 *
	 */
	virtual void
	handle_connect_refused(crofconn& conn) = 0;

	/**
	 *
	 */
	virtual void
	handle_connect_failed(crofconn& conn) = 0;

	/**
	 *
	 */
	virtual void
	handle_connected(crofconn& conn, uint8_t ofp_version) = 0;

	/**
	 *
	 */
	virtual void
	handle_closed(crofconn& conn) = 0;

	/**
	 *
	 */
	virtual void
	handle_write(crofconn& conn) = 0;

	/**
	 *
	 */
	virtual void
	recv_message(crofconn& conn, rofl::openflow::cofmsg *msg) = 0;

	/**
	 *
	 */
	virtual uint32_t
	get_async_xid(crofconn& conn) = 0;

	/**
	 *
	 */
	virtual uint32_t
	get_sync_xid(crofconn& conn, uint8_t msg_type = 0, uint16_t msg_sub_type = 0) = 0;

	/**
	 *
	 */
	virtual void
	release_sync_xid(crofconn& conn, uint32_t xid) = 0;
};




/**
 * @addtogroup common_workflow
 * @brief	A single OpenFlow control connection
 */
class crofconn :
		public crofsock_env,
		public ciosrv,
		public rofl::common::cthread
{
	enum outqueue_type_t {
		QUEUE_OAM  = 0, // Echo.request/Echo.reply
		QUEUE_MGMT = 1, // all remaining packets, except ...
		QUEUE_FLOW = 2, // Flow-Mod/Flow-Removed
		QUEUE_PKT  = 3, // Packet-In/Packet-Out
		QUEUE_MAX,		// do not use
	};

	enum msg_type_t {
		OFPT_HELLO = 0,
		OFPT_ERROR = 1,
		OFPT_ECHO_REQUEST = 2,
		OFPT_ECHO_REPLY = 3,
		OFPT_FEATURES_REPLY = 6,
		OFPT_MULTIPART_REQUEST = 18,
		OFPT_MULTIPART_REPLY = 19,
	};

	enum crofconn_event_t {
		EVENT_NONE				= 0,
		EVENT_RECONNECT			= 1,
		EVENT_TCP_CONNECTED 	= 2,
		EVENT_DISCONNECTED 		= 3,
		EVENT_HELLO_RCVD 		= 4,
		EVENT_HELLO_EXPIRED		= 5,
		EVENT_FEATURES_RCVD		= 6,
		EVENT_FEATURES_EXPIRED	= 7,
		EVENT_ECHO_RCVD			= 8,
		EVENT_ECHO_EXPIRED		= 9,
		EVENT_NEED_LIFE_CHECK	= 10,
		//
		EVENT_RXQUEUE			= 11,
		EVENT_CONNECT_FAILED	= 13,
		EVENT_CONNECT_REFUSED	= 14,
		EVENT_LOCAL_DISCONNECT	= 15,
		EVENT_CONGESTION_SOLVED	= 16,
		EVENT_PEER_DISCONNECTED	= 17,	// socket was closed by peer entity
	};

	enum crofconn_state_t {
		STATE_INIT				= 0,
		STATE_DISCONNECTED 		= 1,
		STATE_CONNECT_PENDING	= 2,
		STATE_ACCEPT_PENDING	= 3,
		STATE_WAIT_FOR_HELLO	= 4,
		STATE_WAIT_FOR_FEATURES = 5,
		STATE_CONNECTED 		= 6,
	};

	enum crofconn_timer_t {
		TIMER_NEXT_RECONNECT	= 1,
		TIMER_WAIT_FOR_HELLO	= 2,
		TIMER_WAIT_FOR_FEATURES = 3,
		TIMER_NEED_LIFE_CHECK	= 4,
		TIMER_WAIT_FOR_ECHO		= 5,
	};

	enum crofconn_flags_t {
		FLAGS_PASSIVE			= 1,
		FLAGS_CONNECT_REFUSED	= 2,
		FLAGS_CONNECT_FAILED	= 3,
		FLAGS_LOCAL_DISCONNECT	= 4,
		FLAGS_RECONNECTING		= 5,
		FLAGS_RXQUEUE_CONSUMING = 6,
		FLAGS_CONGESTED			= 7,
		FLAGS_PEER_DISCONNECTED	= 8,
	};

public:

	enum crofconn_flavour_t {
		FLAVOUR_UNSPECIFIED		= 0,
		FLAVOUR_CTL 			= 1,
		FLAVOUR_DPT				= 2,
	};

public:

	/**
	 * controller mode
	 */
	crofconn(
			crofconn_env *env,
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap);

	/**
	 *
	 */
	virtual ~crofconn();

public:

	/**
	 *
	 */
	enum crofconn_flavour_t
	get_flavour() const
	{ return flavour; };

	/**
	 *
	 */
	void
	accept(
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& socket_params,
			int newsd,
			enum crofconn_flavour_t flavour);

	/**
	 * @brief	Instruct crofsock instance to connect to peer using specified parameters.
	 */
	void
	connect(
			const cauxid& aux_id,
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& socket_params);

	/**
	 * @brief	Instruct crofsock instance to reconnect to previously connected peer.
	 */
	void
	reconnect(
			bool reset_backoff_timer = false);

	/**
	 * @brief	Instruct crofsock instance to close connection to peer.
	 */
	void
	close();

	/**
	 * @brief	Returns whether this connection is established
	 */
	bool
	is_established() const
	{ return (STATE_CONNECTED == state); }

	/**
	 * @brief	Returns true when this connection has been actively established
	 */
	bool
	is_actively_established() const
	{ return not flags.test(FLAGS_PASSIVE); };

	/**
	 * @brief	Returns true, when the underlying TCP connection is congested
	 */
	bool
	is_congested() const
	{ return flags.test(FLAGS_CONGESTED); };

	/**
	 * @brief	Returns a reference to the versionbitmap announced by this entity
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap()
	{ return versionbitmap; };

	/**
	 * @brief	Returns a reference to the versionbitmap seen from the peer
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap_peer()
	{ return versionbitmap_peer; };

	/**
	 * @brief	Returns the negotiated OFP version (or OFP_UNKNOWN)
	 */
	uint8_t
	get_version() const
	{ return ofp_version; };

	/**
	 * @brief	Returns data path id assigned to this connection
	 */
	uint64_t
	get_dpid() const
	{ return dpid; };

	/**
	 * @brief	Return auxialiary_id
	 */
	cauxid const&
	get_aux_id() const
	{ return auxiliary_id; };

	/**
	 * @brief
	 */
	crofsock const&
	get_rofsocket() const
	{ return *rofsock; };

	/**
	 * @brief	Send OFP message via socket
	 */
	unsigned int
	send_message(
			rofl::openflow::cofmsg *msg)
	{ return fragment_and_send_message(msg); };

	/**
	 *
	 */
	void
	set_env(
			crofconn_env* env)
	{ this->env = env; };

	/**
	 *
	 */
	void
	set_max_backoff(
			const ctimespec& timespec);

protected:

	virtual void
	init_thread();

	virtual void
	release_thread();

private:

	virtual void
	handle_connect_refused(
			crofsock& rofsock) {
		rofl::logging::warn << "[rofl-common][crofconn] transport connection: connect refused " << std::endl;
		rofl::ciosrv::notify(rofl::cevent(EVENT_CONNECT_REFUSED));
	};

	virtual void
	handle_connect_failed(
			crofsock& rofsock) {
		rofl::logging::debug << "[rofl-common][crofconn] transport connection: connect failed " << std::endl;
		rofl::ciosrv::notify(rofl::cevent(EVENT_CONNECT_FAILED));
	};

	virtual void
	handle_connected (
			crofsock& rofsock) {
		rofl::logging::debug << "[rofl-common][crofconn] transport connection established " << std::endl;
		rofl::ciosrv::notify(rofl::cevent(EVENT_TCP_CONNECTED));
	};

	virtual void
	handle_closed(
			crofsock& rofsock) {
		rofl::logging::debug << "[rofl-common][crofconn] transport connection closed " << std::endl;
		rofl::ciosrv::notify(rofl::cevent(EVENT_PEER_DISCONNECTED));
	};

	virtual void
	handle_write(
			crofsock& rofsock) {
		rofl::logging::debug << "[rofl-common][crofconn] transport connection congested " << std::endl;
		rofl::ciosrv::notify(rofl::cevent(EVENT_CONGESTION_SOLVED));
	};

	virtual void
	recv_message(
			crofsock& rofsock,
			rofl::openflow::cofmsg *msg);

private:

	/**
	 *
	 */
	void
	handle_messages();

	/**
	 *
	 */
	void
	send_message_to_env(
			rofl::openflow::cofmsg* msg);

	/**
	 *
	 */
	virtual void
	handle_timeout(
			int opaque,
			void *data = (void*)0);

	/**
	 *
	 */
	virtual void
	handle_event(
			const cevent& ev) {
		switch (ev.get_cmd()) {
		case EVENT_RXQUEUE: {
			handle_messages();
		} break;
		case EVENT_TCP_CONNECTED: {
			flags.reset(FLAGS_RECONNECTING);
			run_engine(EVENT_TCP_CONNECTED);
		} break;
		case EVENT_CONNECT_FAILED: {
			flags.set(FLAGS_CONNECT_FAILED);
			run_engine(EVENT_DISCONNECTED);
		} break;
		case EVENT_CONNECT_REFUSED: {
			flags.set(FLAGS_CONNECT_REFUSED);
			run_engine(EVENT_DISCONNECTED);
		} break;
		case EVENT_LOCAL_DISCONNECT: {
			flags.set(FLAGS_LOCAL_DISCONNECT);
			run_engine(EVENT_DISCONNECTED);
		} break;
		case EVENT_CONGESTION_SOLVED: {
			flags.reset(FLAGS_CONGESTED);
			crofconn_env::set_env(env).handle_write(*this);
		} break;
		case EVENT_PEER_DISCONNECTED: {
			flags.set(FLAGS_PEER_DISCONNECTED);
			run_engine(EVENT_DISCONNECTED);
		} break;
		}
	};

	/**
	 *
	 */
	void
	run_engine(
			enum crofconn_event_t event = EVENT_NONE);

	/**
	 *
	 */
	void
	event_reconnect();

	/**
	 *
	 */
	void
	event_tcp_connected();

	/**
	 *
	 */
	void
	event_disconnected();

	/**
	 *
	 */
	void
	event_hello_rcvd();

	/**
	 *
	 */
	void
	event_hello_expired();

	/**
	 *
	 */
	void
	event_features_rcvd();

	/**
	 *
	 */
	void
	event_features_expired();

	/**
	 *
	 */
	void
	event_echo_rcvd();

	/**
	 *
	 */
	void
	event_echo_expired();

	/**
	 *
	 */
	void
	event_need_life_check();

	/**
	 *
	 */
	void
	action_send_hello_message();

	/**
	 *
	 */
	void
	action_send_features_request();

	/**
	 *
	 */
	void
	action_disconnect();

	/**
	 *
	 */
	void
	action_send_echo_request();

	/**
	 *
	 */
	void
	backoff_reconnect(
			bool reset_timeout = false);

private:

	/**
	 *
	 */
	void
	hello_rcvd(
			rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	void
	echo_request_rcvd(
			rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	void
	echo_reply_rcvd(
			rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	void
	error_rcvd(
			rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	void
	features_reply_rcvd(
			rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	unsigned int
	fragment_and_send_message(
			rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	unsigned int
	fragment_table_features_stats_request(
			rofl::openflow::cofmsg_table_features_stats_request *msg);

	/**
	 *
	 */
	unsigned int
	fragment_flow_stats_reply(
			rofl::openflow::cofmsg_flow_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_table_stats_reply(
			rofl::openflow::cofmsg_table_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_port_stats_reply(
			rofl::openflow::cofmsg_port_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_queue_stats_reply(
			rofl::openflow::cofmsg_queue_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_group_stats_reply(
			rofl::openflow::cofmsg_group_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_group_desc_stats_reply(
			rofl::openflow::cofmsg_group_desc_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_table_features_stats_reply(
			rofl::openflow::cofmsg_table_features_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_port_desc_stats_reply(
			rofl::openflow::cofmsg_port_desc_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_meter_stats_reply(
			rofl::openflow::cofmsg_meter_stats_reply *msg);

	/**
	 *
	 */
	unsigned int
	fragment_meter_config_stats_reply(
			rofl::openflow::cofmsg_meter_config_stats_reply *msg);

	/**
	 *
	 */
	void
	timer_start(
			crofconn_timer_t type, const ctimespec& timespec);

	/**
	 *
	 */
	void
	timer_stop(
			crofconn_timer_t type);

	/**
	 *
	 */
	void
	timer_start_next_reconnect() {
		timer_start(TIMER_NEXT_RECONNECT, reconnect_timespec);
	};

	/**
	 *
	 */
	void
	timer_stop_next_reconnect() {
		timer_stop(TIMER_NEXT_RECONNECT);
	};

	/**
	 *
	 */
	void
	timer_start_life_check() {
		timer_start(TIMER_NEED_LIFE_CHECK, echo_interval);
	};

	/**
	 *
	 */
	void
	timer_stop_life_check() {
		timer_stop(TIMER_NEED_LIFE_CHECK);
	};

	/**
	 *
	 */
	void
	timer_start_wait_for_hello() {
		timer_start(TIMER_WAIT_FOR_HELLO, hello_timeout);
	};

	/**
	 *
	 */
	void
	timer_stop_wait_for_hello() {
		timer_stop(TIMER_WAIT_FOR_HELLO);
	};

	/**
	 *
	 */
	void
	timer_start_wait_for_features() {
		timer_start(TIMER_WAIT_FOR_FEATURES, echo_interval);
	};

	/**
	 *
	 */
	void
	timer_stop_wait_for_features() {
		timer_stop(TIMER_WAIT_FOR_FEATURES);
	};

	/**
	 *
	 */
	void
	timer_start_wait_for_echo() {
		timer_start(TIMER_WAIT_FOR_ECHO, echo_timeout);
	};

	/**
	 *
	 */
	void
	timer_stop_wait_for_echo() {
		timer_stop(TIMER_WAIT_FOR_ECHO);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const crofconn& conn) {
		os << indent(0) << "<crofconn ofp-version:" << (int)conn.ofp_version
				<< " OFP-transport-connection-established: " << (bool)(conn.state == STATE_CONNECTED)
				<< " >" << std::endl;
		{ rofl::indent i(2); os << conn.get_aux_id(); }
		if (conn.state == STATE_DISCONNECTED) {
			os << indent(2) << "<state: -DISCONNECTED- >" << std::endl;
		}
		else if (conn.state == STATE_CONNECT_PENDING) {
			os << indent(2) << "<state: -CONNECT-PENDING- >" << std::endl;
		}
		else if (conn.state == STATE_ACCEPT_PENDING) {
			os << indent(2) << "<state: -ACCEPT-PENDING- >" << std::endl;
		}
		else if (conn.state == STATE_WAIT_FOR_HELLO) {
			os << indent(2) << "<state: -WAIT-FOR-HELLO- >" << std::endl;
		}
		else if (conn.state == STATE_WAIT_FOR_FEATURES) {
			os << indent(2) << "<state: -WAIT-FOR-FEATURES- >" << std::endl;
		}
		else if (conn.state == STATE_CONNECTED) {
			os << indent(2) << "<state: -ESTABLISHED- >" << std::endl;
		}
		{ os << rofl::indent(2) << "<current-backoff: >" << std::endl; rofl::indent i(4); os << conn.reconnect_timespec; };
		{ os << rofl::indent(2) << "<max-backoff: >" << std::endl; rofl::indent i(4); os << conn.max_backoff; };
#if 0
		os << indent(2) << "<versionbitmap-local: >" << std::endl;
		{ indent i(4); os << conn.versionbitmap; }
		os << indent(2) << "<versionbitmap-remote: >" << std::endl;
		{ indent i(4); os << conn.versionbitmap_peer; }
#endif
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		if (state == STATE_DISCONNECTED) {
			ss << "state: -DISCONNECTED- ";
		}
		else if (state == STATE_CONNECT_PENDING) {
			ss << "state: -CONNECT-PENDING- ";
			ss << "backoff: " << reconnect_timespec.str();
		}
		else if (state == STATE_ACCEPT_PENDING) {
			ss << "state: -ACCEPT-PENDING- ";
		}
		else if (state == STATE_WAIT_FOR_HELLO) {
			ss << "state: -WAIT-FOR-HELLO- ";
		}
		else if (state == STATE_WAIT_FOR_FEATURES) {
			ss << "state: -WAIT-FOR-FEATURES- ";
		}
		else if (state == STATE_CONNECTED) {
			ss << "state: -ESTABLISHED- ";
		}
		return ss.str();
	};

private:

	crofconn_env* 		env;
	uint64_t			dpid;
	cauxid				auxiliary_id;
	crofsock*			rofsock;
	rofl::openflow::cofhello_elem_versionbitmap
						versionbitmap; 			// supported OFP versions by this entity
	rofl::openflow::cofhello_elem_versionbitmap
						versionbitmap_peer;		// supported OFP versions by peer entity
	uint8_t				ofp_version;			// negotiated OFP version
	std::bitset<32>		flags;
	csegmentation		sar;					// segmentation and reassembly for multipart messages
	size_t				fragmentation_threshold;// maximum number of bytes for a multipart message before being fragmented

	static unsigned int const
						DEFAULT_FRAGMENTATION_THRESHOLD = 65535;
	static unsigned int const
						DEFAULT_ETHERNET_MTU_SIZE = 1500;
	ctimespec			max_backoff;
	ctimespec			reconnect_start_timeout;
	ctimespec			reconnect_timespec; 	// reconnect in x seconds
	ctimespec			reconnect_variance;
	int 				reconnect_counter;

	static const int 	CROFCONN_RECONNECT_START_TIMEOUT_IN_NSECS 	= 10000000;	// start reconnect timeout (default 10ms)
	static const int 	CROFCONN_RECONNECT_VARIANCE_IN_NSECS 		= 10000000; // reconnect variance (default 10ms)

	enum crofconn_flavour_t
						flavour;
	enum rofl::csocket::socket_type_t
						socket_type;
	rofl::cparams
						socket_params;
	int					newsd;
	std::deque<enum crofconn_event_t>
						events;
	enum crofconn_state_t
						state;
	std::map<crofconn_timer_t, ctimerid>
						timer_ids;				// timer-ids obtained from ciosrv

	std::vector<crofqueue>
						rxqueues;				// queues for receiving messages from crofsock instance => // QUEUE_MAX txqueues

	std::vector<unsigned int>
						rxweights;				// relative scheduling weights for txqueues

	rofl::crofqueue		dlqueue;				// delay queue, used for storing asynchronous messages during connection setup

	static const int 	DEFAULT_HELLO_TIMEOUT = 5;
	static const int 	DEFAULT_ECHO_TIMEOUT = 60;
	static const int 	DEFAULT_ECHO_INTERVAL = 60;

public:

	unsigned int		hello_timeout;
	unsigned int		echo_timeout;
	unsigned int		echo_interval;

};

}; /* namespace rofl */

#endif /* CROFCHAN_H_ */
