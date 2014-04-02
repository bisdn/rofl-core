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

#include "rofl/common/ciosrv.h"
#include "rofl/common/crofsock.h"
#include "rofl/common/openflow/cofhelloelems.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/crandom.h"
#include "rofl/common/csegmentation.h"

namespace rofl {

class eRofConnBase 					: public RoflException {};
class eRofConnXidSpaceExhausted		: public eRofConnBase {};
class eRofConnBusy					: public eRofConnBase {}; // connection already established

class crofconn; // forward declaration

class crofconn_env {
public:
	virtual ~crofconn_env() {};
	virtual void handle_connect_refused(crofconn *conn) = 0;
	virtual void handle_connected(crofconn *conn, uint8_t ofp_version) = 0;
	virtual void handle_closed(crofconn *conn) = 0;
	virtual void recv_message(crofconn *conn, rofl::openflow::cofmsg *msg) = 0;
	virtual uint32_t get_async_xid(crofconn *conn) = 0;
	virtual uint32_t get_sync_xid(crofconn *conn, uint8_t msg_type = 0, uint16_t msg_sub_type = 0) = 0;
	virtual void release_sync_xid(crofconn *conn, uint32_t xid) = 0;
};


class crofconn :
		public crofsock_env,
		public ciosrv
{
	crofconn_env 					*env;
	uint64_t						dpid;
	uint8_t							auxiliary_id;
	crofsock						rofsock;
	rofl::openflow::cofhello_elem_versionbitmap		versionbitmap; 			// supported OFP versions by this entity
	rofl::openflow::cofhello_elem_versionbitmap		versionbitmap_peer;		// supported OFP versions by peer entity
	uint8_t							ofp_version;			// negotiated OFP version
	std::bitset<32>					flags;
	std::map<int, uint32_t>			timer_ids;				// timer-ids obtained from ciosrv
	csegmentation					sar;					// segmentation and reassembly for multipart messages
	size_t							fragmentation_threshold;// maximum number of bytes for a multipart message before being fragmented

	static unsigned int const DEFAULT_FRAGMENTATION_THRESHOLD = 65535;
	static unsigned int const DEFAULT_ETHERNET_MTU_SIZE = 1500;

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
		EVENT_CONNECTED 		= 1,
		EVENT_DISCONNECTED 		= 2,
		EVENT_HELLO_RCVD 		= 3,
		EVENT_HELLO_EXPIRED		= 4,
		EVENT_FEATURES_RCVD		= 5,
		EVENT_FEATURES_EXPIRED	= 6,
		EVENT_ECHO_RCVD			= 7,
		EVENT_ECHO_EXPIRED		= 8,
	};
	std::deque<enum crofconn_event_t> 	events;

	enum crofconn_state_t {
		STATE_DISCONNECTED 		= 1,
		STATE_CONNECT_PENDING	= 2,
		STATE_WAIT_FOR_HELLO	= 3,
		STATE_WAIT_FOR_FEATURES = 4,
		STATE_ESTABLISHED 		= 5,
	};
	enum crofconn_state_t				state;

	enum crofconn_timer_t {
		TIMER_WAIT_FOR_HELLO	= 1,
		TIMER_WAIT_FOR_FEATURES = 2,
		TIMER_SEND_ECHO			= 3,
		TIMER_WAIT_FOR_ECHO		= 4,
	};

	enum crofconn_flags_t {
		FLAGS_PASSIVE			= 1,
	};

#define DEFAULT_HELLO_TIMEOUT	5
#define DEFAULT_ECHO_TIMEOUT 	5
#define DEFAULT_ECHO_INTERVAL	10

public:

	unsigned int					hello_timeout;
	unsigned int					echo_timeout;
	unsigned int					echo_interval;

public:

	/**
	 * controller mode
	 */
	crofconn(
			crofconn_env *env,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap);

	/**
	 *
	 */
	virtual ~crofconn();

public:

	/**
	 *
	 */
	void
	accept(enum rofl::csocket::socket_type_t socket_type, int newsd);

	/**
	 * @brief	Instruct crofsock instance to connect to peer using specified parameters.
	 */
	void
	connect(uint8_t aux_id, enum rofl::csocket::socket_type_t socket_type, int domain, int type, int protocol, caddress const& raddr);

	/**
	 * @brief	Instruct crofsock instance to connect to peer using specified parameters.
	 */
	void
	connect(uint8_t aux_id, enum rofl::csocket::socket_type_t socket_type, cparams const& socket_params);

	/**
	 * @brief	Instruct crofsock instance to reconnect to previously connected peer.
	 */
	void
	reconnect();

	/**
	 * @brief	Instruct crofsock instance to close connection to peer.
	 */
	void
	close();

	/**
	 * @brief	Returns whether this connection is established
	 */
	bool
	is_established() const { return (STATE_ESTABLISHED == state); }

	/**
	 * @brief	Returns true when this connection has been actively established
	 */
	bool
	is_actively_established() const { return not flags.test(FLAGS_PASSIVE); };

	/**
	 * @brief	Returns a reference to the versionbitmap announced by this entity
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap() { return versionbitmap; };

	/**
	 * @brief	Returns a reference to the versionbitmap seen from the peer
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap_peer() { return versionbitmap_peer; };

	/**
	 * @brief	Returns the negotiated OFP version (or OFP_UNKNOWN)
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 * @brief	Returns data path id assigned to this connection
	 */
	uint64_t
	get_dpid() const { return dpid; };

	/**
	 * @brief	Return auxialiary_id
	 */
	uint8_t
	get_aux_id() const { return auxiliary_id; };

	/**
	 * @brief
	 */
	crofsock&
	get_rofsocket() { return rofsock; };

	/**
	 * @brief	Send OFP message via socket
	 */
	void
	send_message(rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	void
	set_env(crofconn_env* env) { this->env = env; };

private:

	virtual void
	handle_connect_refused(crofsock *rofsock);

	virtual void
	handle_connected (crofsock *rofsock);

	virtual void
	handle_closed(crofsock *rofsock);

	virtual void
	recv_message(crofsock *rofsock, rofl::openflow::cofmsg *msg);

private:

	/**
	 *
	 */
	virtual void
	handle_timeout(int opaque, void *data = (void*)0);

	/**
	 *
	 */
	void
	run_engine(enum crofconn_event_t event = EVENT_NONE);

	/**
	 *
	 */
	void
	event_connected();

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
	void
	fragment_and_send_message(
			rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	void
	fragment_table_features_stats_request(
			rofl::openflow::cofmsg_table_features_stats_request *msg);

	/**
	 *
	 */
	void
	fragment_flow_stats_reply(
			rofl::openflow::cofmsg_flow_stats_reply *msg);

	/**
	 *
	 */
	void
	fragment_table_stats_reply(
			rofl::openflow::cofmsg_table_stats_reply *msg);

	/**
	 *
	 */
	void
	fragment_port_stats_reply(
			rofl::openflow::cofmsg_port_stats_reply *msg);

	/**
	 *
	 */
	void
	fragment_queue_stats_reply(
			rofl::openflow::cofmsg_queue_stats_reply *msg);

	/**
	 *
	 */
	void
	fragment_group_stats_reply(
			rofl::openflow::cofmsg_group_stats_reply *msg);

	/**
	 *
	 */
	void
	fragment_group_desc_stats_reply(
			rofl::openflow::cofmsg_group_desc_stats_reply *msg);

	/**
	 *
	 */
	void
	fragment_table_features_stats_reply(
			rofl::openflow::cofmsg_table_features_stats_reply *msg);

	/**
	 *
	 */
	void
	fragment_port_desc_stats_reply(
			rofl::openflow::cofmsg_port_desc_stats_reply *msg);

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofconn const& conn) {
		os << indent(0) << "<crofconn ofp-version:" << (int)conn.ofp_version
				<< " aux-id:" << (int)conn.auxiliary_id << " >" << std::endl;
		if (conn.state == STATE_DISCONNECTED) {
			os << indent(2) << "<state: -DISCONNECTED- >" << std::endl;
		}
		else if (conn.state == STATE_WAIT_FOR_HELLO) {
			os << indent(2) << "<state: -WAIT-FOR-HELLO- >" << std::endl;
		}
		else if (conn.state == STATE_WAIT_FOR_FEATURES) {
			os << indent(2) << "<state: -WAIT-FOR-FEATURES- >" << std::endl;
		}
		else if (conn.state == STATE_ESTABLISHED) {
			os << indent(2) << "<state: -ESTABLISHED- >" << std::endl;
		}

		{ indent i(2); os << (conn.rofsock); }
		os << indent(2) << "<versionbitmap-local: >" << std::endl;
		{ indent i(4); os << conn.versionbitmap; }
		os << indent(2) << "<versionbitmap-remote: >" << std::endl;
		{ indent i(4); os << conn.versionbitmap_peer; }
		return os;
	};
};

}; /* namespace rofl */

#endif /* CROFCHAN_H_ */
