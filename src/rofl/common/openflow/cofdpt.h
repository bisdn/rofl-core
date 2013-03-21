/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFDPT_H
#define COFDPT_H 1

#include <map>
#include <set>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "openflow12.h"
#include <stdio.h>
#include <strings.h>

#ifdef __cplusplus
}
#endif

#include "../ciosrv.h"
#include "../cmemory.h"
#include "../ciosrv.h"
#include "../cerror.h"
#include "../crofbase.h"
#include "../cmemory.h"
#include "../cvastring.h"
#include "../cfsm.h"
#include "../protocols/fetherframe.h"
#include "../cxidstore.h"
#include "../csocket.h"
#include "cofport.h"
#include "extensions/cfsptable.h"

#if 0
#include "cfttable.h"
#include "cgttable.h"
#endif
#include "cofmsg.h"
#include "cofmsg_hello.h"
#include "cofmsg_echo.h"
#include "cofmsg_error.h"
#include "cofmsg_features.h"
#include "cofmsg_config.h"
#include "cofmsg_packet_in.h"
#include "cofmsg_flow_removed.h"
#include "cofmsg_port_status.h"
#include "cofmsg_stats.h"
#include "cofmsg_desc_stats.h"
#include "cofmsg_flow_stats.h"
#include "cofmsg_aggr_stats.h"
#include "cofmsg_table_stats.h"
#include "cofmsg_port_stats.h"
#include "cofmsg_queue_stats.h"
#include "cofmsg_group_stats.h"
#include "cofmsg_group_desc_stats.h"
#include "cofmsg_group_features_stats.h"
#include "cofmsg_barrier.h"
#include "cofmsg_queue_get_config.h"
#include "cofmsg_role.h"
#include "cofmsg_experimenter.h"


namespace rofl
{


/* error classes */
class eOFdptBase					: public cerror {};
class eDataPathIdInUse 				: public eOFdptBase {}; // datapath id already in use
class eDataPathAlreadyAttached 		: public eOFdptBase {}; // crofbase *entity is already attached
class eOFswitchBase 				: public eOFdptBase {};
class eOFswitchInvalid 				: public eOFdptBase {};
class eOFdpathNotFound 				: public eOFdptBase {}; // element not found


class crofbase;


/** A class for controlling a single attached data path element in class crofbase.
 *
 * This class stores state for an attached data path element
 * including its ports (@see cofport). It is tightly bound to
 * class crofbase (@see crofbase). Created upon reception of an
 * OpenFlow HELLO message from the data path element, cofdpath
 * acquires all state by sending FEATURES-request, GET-CONFIG-request,
 * and TABLE-STATS-request. It also provides a number of convenience
 * methods for controlling the datapath, e.g. clearing all flowtable
 * or grouptable entries.
 *
 */
class cofdpt :
	public csocket_owner,
	public ciosrv,
	public cfsm,
	public cxidowner
{
private:


		/* cofdpt timer types */
		enum cofdpt_timer_t {
			COFDPT_TIMER_BASE = 0xc721,
			COFDPT_TIMER_RECONNECT		= ((COFDPT_TIMER_BASE) << 16 | (0x01 << 8)),
			COFDPT_TIMER_FEATURES_REQUEST 	= ((COFDPT_TIMER_BASE) << 16 | (0x02 << 8)),
			COFDPT_TIMER_FEATURES_REPLY 	= ((COFDPT_TIMER_BASE) << 16 | (0x03 << 8)),
			COFDPT_TIMER_GET_CONFIG_REQUEST = ((COFDPT_TIMER_BASE) << 16 | (0x04 << 8)),
			COFDPT_TIMER_GET_CONFIG_REPLY 	= ((COFDPT_TIMER_BASE) << 16 | (0x05 << 8)),
			COFDPT_TIMER_STATS_REQUEST 	= ((COFDPT_TIMER_BASE) << 16 | (0x06 << 8)),
			COFDPT_TIMER_STATS_REPLY 	= ((COFDPT_TIMER_BASE) << 16 | (0x07 << 8)),
			COFDPT_TIMER_BARRIER_REQUEST 	= ((COFDPT_TIMER_BASE) << 16 | (0x08 << 8)),
			COFDPT_TIMER_BARRIER_REPLY 	= ((COFDPT_TIMER_BASE) << 16 | (0x09 << 8)),
			COFDPT_TIMER_LLDP_SEND_DISC 	= ((COFDPT_TIMER_BASE) << 16 | (0x0a << 8)),
			COFDPT_TIMER_ECHO_REQUEST	= ((COFDPT_TIMER_BASE) << 16 | (0x0b << 8)),
			COFDPT_TIMER_ECHO_REPLY	        = ((COFDPT_TIMER_BASE) << 16 | (0x0c << 8)),
			COFDPT_TIMER_SEND_ECHO_REQUEST	= ((COFDPT_TIMER_BASE) << 16 | (0x0d << 8)),
			COFDPT_TIMER_SEND_HELLO         = ((COFDPT_TIMER_BASE) << 16 | (0x0e << 8)),
		};

		/* cofdpt state types */
		enum cofdpt_state_t {
			COFDPT_STATE_INIT 				= (1 << 0),
			COFDPT_STATE_DISCONNECTED		= (1 << 1),
			COFDPT_STATE_WAIT_FEATURES 		= (1 << 2), // waiting for FEATURE-REPLY
			COFDPT_STATE_WAIT_GET_CONFIG	= (1 << 3), // waiting for GET-CONFIG-REPLY
			COFDPT_STATE_WAIT_TABLE_STATS	= (1 << 4), // waiting for TABLE-STATS-REPLY
			COFDPT_STATE_CONNECTED			= (1 << 5),
		};

		/* cofdpt flags */
		enum cofdpt_flag_t {
			COFDPT_FLAG_ACTIVE_SOCKET		= (1 << 0),
			COFDPT_FLAG_HELLO_RCVD          = (1 << 1),
			COFDPT_FLAG_HELLO_SENT			= (1 << 2),
		};

#define DEFAULT_DP_FEATURES_REPLY_TIMEOUT 		10
#define DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT 	10
#define DEFAULT_DP_STATS_REPLY_TIMEOUT 			10
#define DEFAULT_DB_BARRIER_REPLY_TIMEOUT 		10

		std::bitset<32>                          flags;

public: // data structures

														// instance belongs to
														// (hosted by parent crofbase instance)
		uint64_t 						dpid;			// datapath id
		std::string	 					s_dpid;			// datapath id as std::string
		cmacaddr 						dpmac;			// datapath mac address
		uint32_t 						n_buffers; 		// number of buffer lines
		uint8_t 						n_tables;		// number of tables
		uint32_t 						capabilities;	// capabilities flags

		std::map<uint32_t, cofport*> 	ports;			// list of ports
		std::bitset<32> 				dptflags;		// 'fragmentation' flags
		uint16_t						offlags;
		uint16_t 						miss_send_len; 	// length of bytes sent to controller

		cfsptable 						fsptable;		// flowspace registration table

protected:

		csocket							*socket;		// TCP socket towards data path element

private:

		crofbase 						*rofbase;		// layer-(n) entity
		std::map<uint8_t, cxidstore>	 xidstore;		// transaction store

		std::string 					 info;			// info string
		cmemory							*fragment;		// fragment of OF packet rcvd on fragment during last call(s)
		int 							 reconnect_in_seconds; 	// reconnect in x seconds
		int 							 reconnect_counter;
		int 							 rpc_echo_interval;		// default ECHO time interval
		uint8_t							 version;		// OpenFlow version negotiated

		int 							 features_reply_timeout;
		int 							 get_config_reply_timeout;
		int 							 stats_reply_timeout;
		int 							 barrier_reply_timeout;

public:

	/** constructor (TCP accept)
	 */
	cofdpt(
			crofbase *rofbase,
			int newsd,
			caddress const& ra,
			int domain,
			int type,
			int protocol);


	/** constructor (TCP connect)
	 */
	cofdpt(
			crofbase *rofbase,
			caddress const& ra,
			int domain,
			int type,
			int protocol);



	/**
	 * @name	~cofdpath
	 * @brief	destructor of class cofdpath
	 *
	 * Informs fwdelem by calling (@see handle_dpath_close).
	 * Deletes all cofport instances (@see ports).
	 * Removes this pointer from fwdelem->ofdpath_list.
	 */
	virtual
	~cofdpt();


	/**
	 *
	 */
	uint8_t
	get_version();


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


public:


	/**
	 *
	 */
	void
	send_message(
			cofmsg *pack);



	/**
	 * @name	find_cofport
	 * @brief 	Find a cofport instance by port number
	 *
	 * Returns pointer to a cofport instance based on port number or
	 * throws an exception, if port was not found.
	 *
	 * @return cofport Pointer to cofport instance containing all state of sought port.
	 *
	 * @throws eOFdpathNotFound port could not be found
	 */
	cofport*
	find_cofport(
			uint32_t port_no) throw (eOFdpathNotFound);


	/**
	 * @name	find_cofport
	 * @brief 	Find a cofport instance by port name
	 *
	 * Returns pointer to a cofport instance based on port name or
	 * throws an exception, if port was not found.
	 *
	 * @return cofport Pointer to cofport instance containing all state of sought port.
	 *
	 * @throws eOFdpathNotFound port could not be found
	 */
	cofport*
	find_cofport(
			std::string port_name) throw (eOFdpathNotFound);


	/**
	 * @name	find_cofport
	 * @brief 	Find a cofport instance by port name
	 *
	 * Returns pointer to a cofport instance based on port name or
	 * throws an exception, if port was not found.
	 *
	 * @return cofport Pointer to cofport instance containing all state of sought port.
	 *
	 * @throws eOFdpathNotFound port could not be found
	 */
	cofport*
	find_cofport(
			cmacaddr const& maddr) throw (eOFdpathNotFound);


	/**
	 * @name	fsp_open
	 * @brief 	Registers a flowspace at the attached datapath element.
	 *
	 * This method registers a flowspace on the attached datapath element.
	 * Calling this method multiple times results in several flowspace
	 * registrations.
	 *
	 * @param[in] ofmatch The flowspace definition to be registered.
	 */
	void
	fsp_open(
			cofmatch const& ofmatch);


	/**
	 * @name	fsp_close
	 * @brief 	Registers a flowspace at the attached datapath element.
	 *
	 * This method deregisters a flowspace on the attached datapath element.
	 * The default argument is an empty (= all wildcard ofmatch) and removes
	 * all active flowspace registrations from the datapath element.
	 *
	 * @param[in] ofmatch The flowspace definition to be deregistered.
	 */
	void
	fsp_close(
			cofmatch const& ofmatch = cofmatch());



	/**
	 * @name	flow_mod_reset
	 * @brief	Removes all flowtable entries from the attached datapath element.
	 *
	 * Sends a FlowMod-Delete message to the attached datapath element for removing
	 * all flowtable entries.
	 */
	void
	flow_mod_reset();


	/**
	 * @name	group_mod_reset
	 * @brief	Removes all grouptable entries from the attached datapath element.
	 *
	 * Sends a GroupMod-Delete message to the attached datapath element for removing
	 * all grouptable entries.
	 */
	void
	group_mod_reset();



	/*
	 * overloaded from ciosrv
	 */

	/**
	 * @name 	handle_timeout
	 * @brief	handler for timeout events
	 *
	 * This virtual method is overloaded from (@see ciosrv) and
	 * is called upon expiration of a timer.
	 *
	 * @param[in] opaque The integer value specifying the type of the expired timer.
	 */
	void
	handle_timeout(
		int opaque);



	/**
	 * @name	c_str
	 * @brief	Returns a C-string with a description of this cofdpath instance.
	 *
	 * @return const char Pointer to constant C-string (derived from cofdpath::info).
	 */
	const char*
	c_str();



protected:


	/**
	 *
	 */
	void
	hello_rcvd(
			cofmsg_hello *msg);


	/**
	 *
	 */
	void
	echo_request_sent(
			cofmsg *msg);


	/**
	 *
	 */
	void
	echo_request_rcvd(
			cofmsg_echo_request *msg);


	/**
	 *
	 */
	void
	echo_reply_rcvd(
			cofmsg_echo_reply *msg);


	/** handle incoming vendor message (ROFL extensions)
	 */
	void
	experimenter_rcvd(
			cofmsg_experimenter *msg);


	/**
	 * @name	features_request_sent
	 * @brief	Called by crofbase when a FEATURES-request was sent.
	 *
	 * Starts an internal timer for the expected FEATURES-reply.
	 */
	void
	features_request_sent(
			cofmsg *msg);


	/**
	 * @name	features_reply_rcvd
	 * @brief	Called by cfwdekem when a FEATURES-reply was received.
	 *
	 * Cancels the internal timer waiting for FEATURES-reply.
	 * Stores parameters received in internal variables including ports.
	 * Starts timer for sending a GET-CONFIG-request.
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	features_reply_rcvd(
			cofmsg_features_reply *msg);


	/**
	 * @name	get_config_request_sent
	 * @brief	Called by crofbase when a GET-CONFIG-request was sent.
	 *
	 * Starts an internal timer for the expected GET-CONFIG-reply.
	 */
	void
	get_config_request_sent(
			cofmsg *msg);


	/**
	 * @name	get_config_reply_rcvd
	 * @brief	Called by cfwdekem when a GET-CONFIG-reply was received.
	 *
	 * Cancels the internal timer waiting for GET-CONFIG-reply.
	 * Stores parameters received in internal variables.
	 * Starts timer for sending a TABLE-STATS-request.
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	get_config_reply_rcvd(
			cofmsg_get_config_reply *msg);


	/**
	 * @name	stats_request_sent
	 * @brief	Called by crofbase when a STATS-request was sent.
	 *
	 * Starts an internal timer for the expected STATS-reply.
	 */
	void
	stats_request_sent(
			cofmsg *pack);


	/**
	 * @name	stats_reply_rcvd
	 * @brief	Called by cfwdekem when a STATS-reply was received.
	 *
	 * Cancels the internal timer waiting for STATS-reply.
	 * Stores parameters received in internal variables.
	 * Calls method fwdelem->handle_dpath_open().
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	stats_reply_rcvd(
			cofmsg_stats *msg);



	/**
	 * @name	barrier_request_sent
	 * @brief	Called by crofbase when a BARRIER-request was sent.
	 *
	 * Starts an internal timer for the expected BARRIER-reply.
	 */
	void
	barrier_request_sent(
			cofmsg *pack);


	/**
	 * @name	barrier_reply_rcvd
	 * @brief	Called by cfwdekem when a BARRIER-reply was received.
	 *
	 * Cancels the internal timer waiting for STATS-reply.
	 * Calls method fwdelem->handle_barrier_reply().
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	barrier_reply_rcvd(
			cofmsg_barrier_reply *msg);


	/**
	 * @name	flow_mod_sent
	 * @brief	Called by crofbase when a FLOW-MOD-message was sent.
	 *
	 * Applies FlowMod message to local flowtables.
	 *
	 * @param[in] pack The OpenFlow message sent.
	 *
	 * @throws eOFdpathNotFound Thrown when the table-id specified in pack cannot be found.
	 */
	void flow_mod_sent(
			cofmsg* msg);

	/**
	 * @name	flow_rmvd_rcvd
	 * @brief	Called by crofbase when a FLOW-MOD-message was sent.
	 *
	 * Applies FlowRmvd message to local flowtables.
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	flow_rmvd_rcvd(
			cofmsg_flow_removed *msg);


	/**
	 * @name	group_mod_sent
	 * @brief	Called by crofbase when a GROUP-MOD-message was sent.
	 *
	 * Applies GroupMod message to local grouptables.
	 *
	 * @param[in] pack The OpenFlow message sent.
	 */
	void
	group_mod_sent(
			cofmsg *pack);


	/**
	 * @name	table_mod_sent
	 * @brief	Called by crofbase when a TABLE-MOD-message was sent.
	 *
	 * Applies TableMod message to local flowtables.
	 *
	 * @param[in] pack The OpenFlow message sent.
	 */
	void
	table_mod_sent(
			cofmsg *pack);


	/**
	 * @name	port_mod_sent
	 * @brief	Called by crofbase when a PORT-MOD-message was sent.
	 *
	 * Applies PortMod message to local cofport instance.
	 *
	 * @param[in] pack The OpenFlow message sent.
	 */
	void
	port_mod_sent(
			cofmsg *pack);


	/** handle PACKET-IN message
	 */
	void
	packet_in_rcvd(
			cofmsg_packet_in *msg);


	/** handle PORT-STATUS message
	 */
	void
	port_status_rcvd(
			cofmsg_port_status *msg);



	/**
	 *
	 */
	void
	role_request_sent(
			cofmsg *pack);


	/** handle ROLE-REPLY messages
	 *
	 */
	void
	role_reply_rcvd(
			cofmsg_role_reply *msg);


	/**
	 *
	 */
	void
	queue_get_config_request_sent(
			cofmsg *pack);


	/**
	 *
	 */
	void
	queue_get_config_reply_rcvd(
			cofmsg_queue_get_config_reply *msg);



private:


	/** handle ECHO reply timeout
	 */
	void
	handle_echo_reply_timeout();

	/** handle FEATURES reply timeout
	 */
	void
	handle_features_reply_timeout();


	/** handle GET-CONFIG reply timeout
	 */
	void
	handle_get_config_reply_timeout();


	/** handle STATS reply timeout
	 */
	void
	handle_stats_reply_timeout();


	/** handle BARRIER reply timeout
	 */
	void
	handle_barrier_reply_timeout();


private:


	/**
	 *
	 */
	void
	handle_message(
			cmemory *mem);


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
			cofmsg *msg);
};

}; // end of namespace

#endif
