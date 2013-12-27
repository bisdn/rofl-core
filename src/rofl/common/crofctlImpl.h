/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFCTLIMPL_H
#define COFCTLIMPL_H 1

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif


#include <map>
#include <string>

#include "cfsm.h"
#include "ciosrv.h"
#include "csocket.h"
#include "cvastring.h"
#include "thread_helper.h"
#include "logging.h"
#include "openflow/cofmatch.h"
#include "openflow/extensions/cfspentry.h"
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
#include "openflow/messages/cofmsg_async_config.h"

#include "rofl/common/crofctl.h"

namespace rofl
{


class cofctlImpl :
	public cofctl,
	public csocket_owner,
	public ciosrv,
	public cfsm,
	public cfspentry_owner
{

private: // data structures

	friend class crofbase;

	uint64_t						ctid;					// controller id (TODO: unique hash value per controller connection)
	crofbase 						*rofbase;				// parent crofbase instance
	std::bitset<32> 				flags;					// config: flags
	uint16_t 						miss_send_len;			// config: miss_send_len
	std::set<cofmatch*> 			nspaces;				// list of cofmatch structures depicting controlled namespace
	bool 							role_initialized;		// true, when role values have been initialized properly
	uint16_t 						role;					// role of this controller instance
	uint64_t 						cached_generation_id;	// generation-id used by role requests
	csocket							*socket;				// TCP socket towards controller
	cxidstore						xidstore;
	cmemory							*fragment;				// fragment of OF packet rcvd on fragment during last call(s)
	size_t							msg_bytes_read; 		// bytes already read for current message
	int								reconnect_start_timeout;
	int 							reconnect_in_seconds; 	// reconnect in x seconds
	int 							reconnect_counter;
	int 							rpc_echo_interval;		// default ECHO time interval
	unsigned int 					echo_reply_timeout;		// timeout value for missing ECHO.replies
	uint8_t							ofp_version;			// OpenFlow version negotiated for this session

	enum cofctl_flag_t {
		COFCTL_FLAG_HELLO_RCVD 			= (1 << 0),
		COFCTL_FLAG_HELLO_SENT			= (1 << 1),
		COFCTL_FLAG_SERVER_SOCKET 		= (1 << 2),			// socket spawns new worker sockets upon accept
		COFCTL_FLAG_WORKER_SOCKET 		= (1 << 3),			// socket maintains active connection (actively or passively created)
		COFCTL_FLAG_ACTIVE_SOCKET 		= (1 << 4),			// connection was actively established
		COFCTL_FLAG_RECONNECT_PENDING	= (1 << 5),
	};

	// timer types used by cofrpc
	enum cofctl_timer_t {
		COFCTL_TIMER_BASE = 0xae44,
		COFCTL_TIMER_SEND_VIA_TCP 		= ((COFCTL_TIMER_BASE << 16) | (0x01 << 8)),
		COFCTL_TIMER_CLOSE_CONNECTION 	= ((COFCTL_TIMER_BASE << 16) | (0x02 << 8)),
		COFCTL_TIMER_SEND_ECHO_REQUEST 	= ((COFCTL_TIMER_BASE << 16) | (0x03 << 8)),
		COFCTL_TIMER_SEND_HELLO 		= ((COFCTL_TIMER_BASE << 16) | (0x04 << 8)),
		COFCTL_TIMER_RECONNECT 			= ((COFCTL_TIMER_BASE << 16) | (0x05 << 8)),
		COFCTL_TIMER_ECHO_REPLY_TIMEOUT = ((COFCTL_TIMER_BASE << 16) | (0x06 << 8)),
		COFCTL_TIMER_BUGGY_NOX_DELAY	= ((COFCTL_TIMER_BASE << 16) | (0x07 << 8)),
		COFCTL_TIMER_ESTABLISH			= ((COFCTL_TIMER_BASE << 16) | (0x08 << 8)),
	};

	// states for cofrpc fsm
	enum cofctl_state_t {
		STATE_CTL_DISCONNECTED			= 0x01,
		STATE_CTL_CONNECTING			= 0x02,
		STATE_CTL_CONNECTED				= 0x03,
		STATE_CTL_ESTABLISHED			= 0x04,
	};


#define RECONNECT_START_TIMEOUT 5							// start reconnect timeout (default 1s)
#define DEFAULT_RPC_ECHO_INTERVAL 30 /* seconds */
#define DEFAULT_ECHO_TIMEOUT 8 /* seconds */
#define RECONNECT_MAX_TIMEOUT 60							// max reconnect timeout (default 120s)

public: // methods



	/**
	 * @brief	Constructor for creating new cofctl instance from accepted socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 */
	cofctlImpl(
			crofbase *rofbase);




	/**
	 * @brief	Constructor for creating new cofctl instance from accepted socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param newsd socket descriptor
	 * @param ra peer remote address
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 */
	cofctlImpl(
			crofbase *rofbase,
			int newsd,
			caddress const& ra,
			int domain,
			int type,
			int protocol);



	/**
	 * @brief	Constructor for creating new cofctl instance for connecting socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param ra peer remote address
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 *
	 */
	cofctlImpl(
			crofbase *rofbase,
			uint8_t ofp_version,
			int reconnect_start_timeout,
			caddress const& ra,
			int domain,
			int type,
			int protocol);



	/**
	 * @brief	Destructor.
	 */
	virtual
	~cofctlImpl();


	/**
	 * @brief	Returns true, when the control handshake (HELLO) has been completed.
	 */
	virtual bool
	is_established() const;



	/**
	 * @brief	Returns true, when the control entity is in role SLAVE
	 */
	virtual bool
	is_slave() const;



	/**
	 * @brief	Returns OpenFlow version negotiated for control connection.
	 */
	virtual uint8_t
	get_version();



	/**
	 * @brief	Returns cxidtrans instance associated with transaction ID xid.
	 *
	 * @param xid transaction ID
	 */
	virtual cxidtrans&
	transaction(
			uint32_t xid);



	/**
	 * @brief	Sends an OpenFlow message via this cofctl instance.
	 *
	 * @param msg pointer to cofmsg instance
	 */
	virtual void
	send_message(
			cofmsg *msg);


	/**
	 *
	 */
	virtual void
	parse_message(
			cmemory *mem);



	/**
	 * @brief	Returns caddress of connected remote entity.
	 *
	 * @return caddress object obtained from this->socket
	 */
	virtual caddress
	get_peer_addr();


	/**
	 *
	 */
	virtual uint32_t
	get_role() const;


	/**
	 *
	 */
	virtual void
	set_role(uint32_t role);


private:


	void
	parse_of10_message(
			cmemory *mem, cofmsg **pmsg);

	void
	parse_of12_message(
			cmemory *mem, cofmsg **pmsg);

	void
	parse_of13_message(
			cmemory *mem, cofmsg **pmsg);


	/**
	 *
	 */
	void
	hello_rcvd(cofmsg_hello *pack);


	/**
	 *
	 */
	void
	echo_request_sent(cofmsg *pack);


	/**
	 *
	 */
	void
	echo_request_rcvd(cofmsg_echo_request *pack);


	/**
	 *
	 */
	void
	echo_reply_rcvd(cofmsg_echo_reply *pack);


	/** handle incoming vendor message (ROFL extensions)
	 */
	void
	experimenter_rcvd(cofmsg_experimenter *pack);

	/** handle incoming FEATURE requests
	 */
	void
	features_request_rcvd(cofmsg_features_request *pack);

	/**
	 *
	 */
	void
	features_reply_sent(cofmsg *pack);

	/**
	 *
	 */
	void
	get_config_request_rcvd(cofmsg_get_config_request *pack);

	/**
	 *
	 */
	void
	get_config_reply_sent(cofmsg *pack);

	/**
	 *
	 */
	void
	set_config_rcvd(cofmsg_set_config *pack);

	/** handle incoming PACKET-OUT messages
	 */
	void
	packet_out_rcvd(cofmsg_packet_out *pack);

	/** handle incoming FLOW-MOD messages
	 */
	void
	flow_mod_rcvd(cofmsg_flow_mod *pack);

	/** handle incoming GROUP-MOD messages
	 */
	void
	group_mod_rcvd(cofmsg_group_mod *pack);

	/** handle incoming PORT-MOD messages
	 */
	void
	port_mod_rcvd(cofmsg_port_mod *pack);

	/** handle incoming TABLE-MOD messages
	 */
	void
	table_mod_rcvd(cofmsg_table_mod *pack);

	/** STATS-REQUEST received
	 *
	 */
	void
	stats_request_rcvd(cofmsg_stats *pack);

	void
	desc_stats_request_rcvd(cofmsg_desc_stats_request* msg);
	void
	table_stats_request_rcvd(cofmsg_table_stats_request* msg);
	void
	port_stats_request_rcvd(cofmsg_port_stats_request* msg);
	void
	flow_stats_request_rcvd(cofmsg_flow_stats_request* msg);
	void
	aggregate_stats_request_rcvd(cofmsg_aggr_stats_request* msg);
	void
	queue_stats_request_rcvd(cofmsg_queue_stats_request* msg);
	void
	group_stats_request_rcvd(cofmsg_group_stats_request* msg);
	void
	group_desc_stats_request_rcvd(cofmsg_group_desc_stats_request* msg);
	void
	group_features_stats_request_rcvd(cofmsg_group_features_stats_request* msg);
	void
	experimenter_stats_request_rcvd(cofmsg_experimenter_stats_request* msg);

	/**
	 *
	 */
	void
	stats_reply_sent(cofmsg *pack);

	/** handle incoming ROLE-REQUEST messages
	 */
	void
	role_request_rcvd(cofmsg_role_request *pack);

	/**
	 *
	 */
	void
	role_reply_sent(cofmsg *pack);

	/** handle incoming BARRIER request
	 */
	void
	barrier_request_rcvd(cofmsg_barrier_request *pack);

	/** BARRIER reply sent back
	 *
	 */
	void
	barrier_reply_sent(cofmsg *pack);

	/**
	 *
	 */
	void
	queue_get_config_request_rcvd(cofmsg_queue_get_config_request *pack);

	/**
	 *
	 */
	void
	queue_get_config_reply_sent(cofmsg *pack);

	/**
	 */
	void
	get_async_config_request_rcvd(cofmsg_get_async_config_request *pack);

	/**
	 */
	void
	set_async_config_rcvd(cofmsg_set_async_config *pack);

	/**
	 *
	 */
	void
	get_async_config_reply_sent(cofmsg *pack);


	/**
	 *
	 */
	void
	send_error_is_slave(cofmsg *pack);


	/**
	 *
	 */
	virtual void
	handle_timeout(
			int opaque);



private:


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


private:


	/**
	 *
	 */
	void
	check_role();


	/** handle ECHO reply timeout
	 */
	void
	handle_echo_reply_timeout();


	/**
	 *
	 */
	void
	try_to_connect(
			bool reset_timeout = false);


	/**
	 *
	 */
	void
	send_message_via_socket(
			cofmsg *pack);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofctlImpl const& ctl) {
		os << "<cofctlImpl ";
		os << "ctid:0x" << std::hex << (unsigned long long)(ctl.ctid) << std::dec << " ";
		os << "state:" << (enum cofctl_state_t)ctl.fsm_state << " ";
		os << "remote:" << ctl.socket->raddr << " ";
		os << "local:" << ctl.socket->laddr << " ";
		os << ">";
		return os;
	};
};


}; // end of namespace

#endif
