/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFRPC_H
#define COFRPC_H 1

#include <bitset>

#ifdef __cplusplus
extern "C" {
#endif

#include <endian.h>
#include <sys/types.h>
#include <pthread.h>
#include <endian.h>
#ifndef htobe16
#include "../endian_conversion.h"
#endif
#include <sys/types.h>

#ifdef __cplusplus
}
#endif

#include "../cvastring.h"
#include "../thread_helper.h"
#include "../cfsm.h"
#include "../cerror.h"
#include "../csocket.h"
#include "../caddress.h"

#include "cofbase.h"
#include "cofpacket.h"

#include "../../platform/unix/crandom.h"

/* error classes */

class eRpcBase 			: public cerror {}; // base error class for cofrpc
class eRpcAddrInUse 	: public eRpcBase {}; // address in use for socket
class eRpcPackInval 	: public eRpcBase {}; // invalid packet received from TCP socket
class eRpcNoEntity 		: public eRpcBase {}; // no entity for connecting to defined
class eRpcNotAttached 	: public eRpcBase {}; // element not attached
class eRpcIsBusy 		: public eRpcBase {}; // already other entity attached
class eRpcInval 		: public eRpcBase {}; // invalid argument
class eRpcNotConnected 	: public eRpcBase {}; // TCP connection not established



class cofrpc :
	public cofbase,
	public csocket,
	public cfsm
{

	enum cofprc_event_t {
		COFRPC_EVENT_SEND_VIA_TCP = 1,
	};

	// timer types used by cofrpc
	enum cofrpc_timer_t {
		TIMER_RPC_BASE = 0xae44,
		TIMER_RPC_SEND_VIA_TCP 		= ((TIMER_RPC_BASE << 16) | (0x01 << 8)),
		TIMER_RPC_CLOSE_CONNECTION 	= ((TIMER_RPC_BASE << 16) | (0x02 << 8)),
		TIMER_RPC_SEND_ECHO_REQUEST = ((TIMER_RPC_BASE << 16) | (0x03 << 8)),
		TIMER_RPC_SEND_HELLO 		= ((TIMER_RPC_BASE << 16) | (0x04 << 8)),
		TIMER_RPC_RECONNECT 		= ((TIMER_RPC_BASE << 16) | (0x05 << 8)),
		TIMER_RPC_ECHO_REPLY_TIMEOUT = ((TIMER_RPC_BASE << 16) | (0x06 << 8)),
		TIMER_RPC_BUGGY_NOX_DELAY	= ((TIMER_RPC_BASE << 16) | (0x07 << 8)),
		TIMER_RPC_ESTABLISH			= ((TIMER_RPC_BASE << 16) | (0x08 << 8)),
	};

	// states for cofrpc fsm
	enum ofrpc_state_t {
		STATE_RPC_DISCONNECTED		= 0x01,
		STATE_RPC_CONNECTING		= 0x02,
		STATE_RPC_CONNECTED			= 0x03,
		STATE_RPC_ESTABLISHED		= 0x04,
	};

	// north/south RPC endpoint
	int rpc_endpnt;

	// cfwdelem parent instance
	cofbase *entity;

	#define DEFAULT_RPC_ECHO_INTERVAL 30 /* seconds */

public: // static stuff

	// default ECHO time interval
	static int rpc_echo_interval;

	enum ofrpc_endpnt_t {
		OF_RPC_TCP_NORTH_ENDPNT 		= 0x01,
		OF_RPC_TCP_SOUTH_ENDPNT			= 0x02,
	};


public: // methods

	/** constructor
 	 */
	cofrpc(int rpc_endpnt = OF_RPC_TCP_NORTH_ENDPNT,
			cofbase *entity = NULL);

	/** constructor
	 */
	cofrpc(int rpc_endpnt,
			cofbase *entity,
			int sd,
			caddress ra,
			int domain,
			int type,
			int protocol,
			bool active /* socket was established by cconnect()/handle_accepted() */);

	/** destructor
	 */
	virtual ~cofrpc();

#if 0
public: // methods for attaching/detaching other cofbase instances

	/** attach data path
	 */
	virtual void
	dpath_attach(cofbase* dp) {};

	/** detach data path
	 */
	virtual void
	dpath_detach(cofbase* dp) {};

	/** attach controlling entity
	 */
	virtual void
	ctrl_attach(cofbase* dp) throw (eOFbaseIsBusy) {};

	/** detach controlling entity
	 */
	virtual void
	ctrl_detach(cofbase* dp) {};
#endif


public: // methods

	/** create a listening socket
	 */
	void
	clisten(caddress const& la);

	/** connect to remote TCP endpoint
	 */
	void
	cconnect(caddress const& ra);



public: // overloaded from ciosrv

	/**
	 * New incoming connection (listening mode).
	 * Called once accept() returns with a new connection request.
	 * Creates a new instance of ccli().
	 * @param newsd The socket descriptor 'newsd' of the new socket created.
	 * @param ra A reference to the remote address 'ra' of the peer entity.
	 */
	virtual void
	handle_accepted(int newsd, caddress &ra);

	/** handle read events on file descriptor
	 */
	virtual void
	handle_read(int fd);

	/** handle timeout events
	 */
	virtual void
	handle_timeout(int opaque);

	/**
	 *
	 */
	virtual void
	handle_event(cevent const& ev);

public: // overloaded from csocket

	/** socket connected event
	 */
	virtual void
	handle_connected();

	/** socket connection failed event
	 */
	virtual void
	handle_conn_refused();

	/** socket closed event
	 */
	virtual void
	handle_closed(int sd);

	/** dump info string
	 */
	virtual const char*
	c_str();

public: // overloaded from cfwdelem

	// HELLO messages
	//

	/** Send a OF HELLO.message to data path.
	 *
	 */
	virtual void
	fe_down_hello_message(
			cofbase *entity,
			cofpacket *pack);

	/** Send a OF HELLO.message to data path.
	 *
	 */
	virtual void
	fe_up_hello_message(
			cofbase *entity,
			cofpacket *pack);

	// FEATURES request/reply
	//

	/** send a features request
	 */
	virtual void
	fe_down_features_request(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	/** handle features reply
	 */
	virtual void
	fe_up_features_reply(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// GET-CONFIG request/reply
	//

	/** send a get-config request
	 */
	virtual void
	fe_down_get_config_request(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	/** handle get-config reply messages
	 */
	virtual void
	fe_up_get_config_reply(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// STATS request/reply
	//

	/** send stats request
	 */
	virtual void
	fe_down_stats_request(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	/** handle stats reply messages
	 */
	virtual void
	fe_up_stats_reply(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// PACKET-IN message
	//

	/** handle packet-in messages
	 */
	virtual void
	fe_up_packet_in(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// PACKET-OUT message
	//

	/** send a packet out message
	 */
	virtual void
	fe_down_packet_out(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// SET-CONFIG message
	//

	/** send a set-config
	 */
	virtual void
	fe_down_set_config_request(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// BARRIER request/reply
	//

	/** send barrier-request
	 */
	virtual void
	fe_down_barrier_request(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	/** handle barrier reply messages
	 */
	virtual void
	fe_up_barrier_reply(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// ERROR message
	//

	/** handle error messages
	 */
	virtual void
	fe_up_error(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// FLOW-MOD message
	//

	/** send a flow-mod message
	 */
	virtual void
	fe_down_flow_mod(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// GROUP-MOD message
	//

	/** send a group-mod message
	 */
	virtual void
	fe_down_group_mod(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// TABLE-MOD message
	//

	/** send a table-mod message
	 */
	virtual void
	fe_down_table_mod(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// PORT-MOD message
	//

	/** send a port mod request
	 */
	virtual void
	fe_down_port_mod(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// FLOW-REMOVED message
	//

	/** handle flow removed messages
	 */
	virtual void
	fe_up_flow_removed(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// PORT-STATUS message
	//

	/** handle port status messages
	 */
	virtual void
	fe_up_port_status(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// QUEUE-GET-CONFIG request/reply
	//

	/** send a queue-get-config request
	 */
	virtual void
	fe_down_queue_get_config_request(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	/** send a queue-get-config reply
	 */
	virtual void
	fe_up_queue_get_config_reply(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};


	/** Send a OF VENDOR.message to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_experimenter_message(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	/** Send OF VENDOR.message to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_experimenter_message(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	// ROLE request/reply
	//

	/** Send a OF ROLE.request to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_role_request(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};

	/** Send OF ROLE.reply to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_role_reply(
		cofbase *entity,
		cofpacket *pack)
	{
		fe_queue_message(pack);
	};



private: // methods

	/** send hello message via C++
	 */
	void
	send_up_hello_message(
			cofbase *entity,
			bool bye = false);

	/** send hello message via C++
	 */
	void
	send_down_hello_message(
			cofbase *entity,
			bool bye = false);

	/** queue message received from C++ interface to TCP tx queue
	 */
	void
	fe_queue_message(
		cofpacket *pack, bool priority = false);

	/** dequeues outgoing packets from fe_queue and
	 * sends them via the TCP socket
	 */
	void
	send_message_via_tcp();

	/** handle incoming packets from TCP socket
	 */
	void
	handle_tcp(cofpacket *pack);

	/** send hello message via TCP
	 */
	void
	send_hello_message_via_tcp();

	/** handle incoming hello message
	 */
	void
	handle_hello_message_via_tcp(
			cofpacket *pack);

	/** send hello message
	 */
	void
	send_error_message(
		uint16_t type,
		uint16_t code,
		uint8_t* data,
		size_t datalen);

	/** handle error message
	 */
	void
	handle_error(
		cofpacket *pack);

	/** send echo request
	 */
	void
	send_echo_request();


	/** send echo reply
	 */
	void
	send_echo_reply(cofpacket *request);

	/** handle echo reply
	 */
	void
	handle_echo_reply(cofpacket *pack);

	/** handle echo reply timeout
	 */
	void
	handle_echo_reply_timeout();


private: // data structures

	pthread_mutex_t fe_queue_mutex;		// mutex for access to fe_queue
	std::string info; 					// info string
	cofpacket *fragment; 				// packet fragment received in last call to handle_revent
	std::list<cofpacket*> fe_queue; 	// tx queue
	int reconnect_in_seconds; 			// reconnect in x seconds
#define RECONNECT_START_TIMEOUT 5		// start reconnect timeout (default 1s)
#define RECONNECT_MAX_TIMEOUT 60		// max reconnect timeout (default 120s)
	std::bitset<32> reconnect_state;	// reconnect flags
#define RECONNECT_PENDING 0x01
	int reconnect_counter;
	std::bitset<32> flags;				// flags

	enum cofrpc_flag_t {
		COFRPC_FLAG_HELLO_RCVD = (1 << 0),
		COFRPC_FLAG_SERVER_SOCKET = (1 << 1),	// socket spawns new worker sockets upon accept
		COFRPC_FLAG_WORKER_SOCKET = (1 << 2),	// socket maintains active connection (actively or passively created)
		COFRPC_FLAG_ACTIVE_SOCKET = (1 << 4),	// connection was actively established
	};

	/** try a (re)connect
	 */
	void
	try_to_connect(bool reset_timeout = false);

};

#endif
