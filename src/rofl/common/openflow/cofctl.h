/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFCTL_H
#define COFCTL_H 1

#include <map>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
#include "openflow12.h"
#ifdef __cplusplus
}
#endif

#include "../cerror.h"
#include "../ciosrv.h"
#include "../cmemory.h"
#include "../crofbase.h"
#include "../cxidstore.h"
#include "../thread_helper.h"
#include "../cvastring.h"
#include "../csocket.h"
#include "../cfsm.h"
#include <rofl/common/openflow/cofpacket.h>

#include "cofmatch.h"
#include "extensions/cfspentry.h"

namespace rofl
{


class eOFctlBase 			: public cerror {};
class eOFctlPortNotFound 	: public eOFctlBase {};


class crofbase;



class cofctl :
	public csocket_owner,
	public ciosrv,
	public cfsm,
	public cfspentry_owner,
	public cxidowner
{
public: // data structures

	crofbase 						*rofbase;				// parent crofbase instance
	std::bitset<32> 				flags;					// config: flags
	uint16_t 						miss_send_len;			// config: miss_send_len
	std::set<cofmatch*> 			nspaces;				// list of cofmatch structures depicting controlled namespace
	bool 							role_initialized;		// true, when role values have been initialized properly
	uint16_t 						role;					// role of this controller instance
	uint64_t 						cached_generation_id;	// generation-id used by role requests

protected:

	csocket							*socket;				// TCP socket towards controller

private: // data structures

#define RECONNECT_START_TIMEOUT 5							// start reconnect timeout (default 1s)
#define DEFAULT_RPC_ECHO_INTERVAL 30 /* seconds */
#define RECONNECT_MAX_TIMEOUT 60							// max reconnect timeout (default 120s)


	cxidstore						xidstore;
	std::string 					info;					// info string
	cofpacket						*fragment;				// fragment of OF packet rcvd on fragment during last call(s)
	int 							reconnect_in_seconds; 	// reconnect in x seconds
	int 							reconnect_counter;
	int 							rpc_echo_interval;		// default ECHO time interval

	enum cofctl_flag_t {
		COFCTL_FLAG_HELLO_RCVD 			= (1 << 0),
		COFCTL_FLAG_SERVER_SOCKET 		= (1 << 1),			// socket spawns new worker sockets upon accept
		COFCTL_FLAG_WORKER_SOCKET 		= (1 << 2),			// socket maintains active connection (actively or passively created)
		COFCTL_FLAG_ACTIVE_SOCKET 		= (1 << 3),			// connection was actively established
		COFCTL_FLAG_RECONNECT_PENDING	= (1 << 4),
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

public: // methods

	/** constructor (TCP accept)
	 */
	cofctl(
			crofbase *rofbase,
			int newsd,
			caddress const& ra,
			int domain,
			int type,
			int protocol);


	/** constructor (TCP connect)
	 */
	cofctl(
			crofbase *rofbase,
			caddress const& ra,
			int domain,
			int type,
			int protocol);


	/** destructor
	 */
	virtual
	~cofctl();


	/** return info string
	 */
	const char*
	c_str();


	/**
	 *
	 */
	cxidtrans&
	transaction(
			uint32_t xid);


	/**
	 *
	 */
	void
	send_message(
			cofpacket *pack);


protected:


	/**
	 *
	 */
	void
	hello_rcvd(cofpacket *pack);


	/**
	 *
	 */
	void
	echo_request_sent(cofpacket *pack);


	/**
	 *
	 */
	void
	echo_request_rcvd(cofpacket *pack);


	/**
	 *
	 */
	void
	echo_reply_rcvd(cofpacket *pack);


	/** handle incoming vendor message (ROFL extensions)
	 */
	void
	experimenter_rcvd(cofpacket *pack);

	/** handle incoming FEATURE requests
	 */
	void
	features_request_rcvd(cofpacket *pack);

	/**
	 *
	 */
	void
	features_reply_sent(cofpacket *pack);

	/**
	 *
	 */
	void
	get_config_request_rcvd(cofpacket *pack);

	/**
	 *
	 */
	void
	get_config_reply_sent(cofpacket *pack);

	/**
	 *
	 */
	void
	set_config_rcvd(cofpacket *pack);

	/** handle incoming PACKET-OUT messages
	 */
	void
	packet_out_rcvd(cofpacket *pack);

	/** handle incoming FLOW-MOD messages
	 */
	void
	flow_mod_rcvd(cofpacket *pack);

	/** handle incoming GROUP-MOD messages
	 */
	void
	group_mod_rcvd(cofpacket *pack);

	/** handle incoming PORT-MOD messages
	 */
	void
	port_mod_rcvd(cofpacket *pack) throw (eOFctlPortNotFound);

	/** handle incoming TABLE-MOD messages
	 */
	void
	table_mod_rcvd(cofpacket *pack);

	/** STATS-REQUEST received
	 *
	 */
	void
	stats_request_rcvd(cofpacket *pack);

	/**
	 *
	 */
	void
	stats_reply_sent(cofpacket *pack);

	/** handle incoming ROLE-REQUEST messages
	 */
	void
	role_request_rcvd(cofpacket *pack);

	/**
	 *
	 */
	void
	role_reply_sent(cofpacket *pack);

	/** handle incoming BARRIER request
	 */
	void
	barrier_request_rcvd(cofpacket *pack);

	/** BARRIER reply sent back
	 *
	 */
	void
	barrier_reply_sent(cofpacket *pack);

	/**
	 *
	 */
	void
	queue_get_config_request_rcvd(cofpacket *pack);

	/**
	 *
	 */
	void
	queue_get_config_reply_sent(cofpacket *pack);

protected: // methods


	/**
	 *
	 */
	void
	send_error_is_slave(cofpacket *pack);


protected:


	/**
	 *
	 */
	virtual void
	handle_timeout(
			int opaque);



public:


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



	/** handle ECHO reply timeout
	 */
	void
	handle_echo_reply_timeout();


	/**
	 *
	 */
	void
	handle_message(
			cofpacket *pack);


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
			cofpacket *pack);
};

}; // end of namespace

#endif
