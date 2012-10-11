/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFDPATH_H
#define COFDPATH_H 1

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
#include "../cfwdtable.h"
#include "../cmemory.h"
#include "../ciosrv.h"
#include "../cerror.h"
#include "../cfwdelem.h"
#include "../cmemory.h"
#include "../cvastring.h"
#include "../cfibentry.h"
#include "../cfsm.h"
#include "../protocols/fetherframe.h"
#include "../cxidstore.h"


#include "cfttable.h"
#include "cgttable.h"
#include "cofpacket.h"
#include "cofport.h"
#include "extensions/cfsptable.h"

class cfwdelem;
class cofbase;
class cfttable;

/* error classes */
class eDataPathIdInUse : public cerror {}; // datapath id already in use
class eDataPathAlreadyAttached : public cerror {}; // cfwdelem *entity is already attached
class eOFswitchBase : public cerror {};
class eOFswitchInvalid : public eOFswitchBase {};
class eOFdpathNotFound : public eOFswitchBase {}; // element not found



/** A class for controlling a single attached data path element in class cfwdelem.
 *
 * This class stores state for an attached data path element
 * including its ports (@see cofport). It is tightly bound to
 * class cfwdelem (@see cfwdelem). Created upon reception of an
 * OpenFlow HELLO message from the data path element, cofdpath
 * acquires all state by sending FEATURES-request, GET-CONFIG-request,
 * and TABLE-STATS-request. It also provides a number of convenience
 * methods for controlling the datapath, e.g. clearing all flowtable
 * or grouptable entries.
 *
 */
class cofdpath :
	public cftentry_owner,
	public cgtentry_owner,
	public cxidowner,
	public cfsm,
	public ciosrv
{

		/* cofdpath timer types */
		enum cofdpath_timer_t {
			COFDPATH_TIMER_BASE = 0x21,
			COFDPATH_TIMER_FEATURES_REQUEST 	= ((COFDPATH_TIMER_BASE) << 16 | (0x01 << 8)),
			COFDPATH_TIMER_FEATURES_REPLY 		= ((COFDPATH_TIMER_BASE) << 16 | (0x02 << 8)),
			COFDPATH_TIMER_GET_CONFIG_REQUEST 	= ((COFDPATH_TIMER_BASE) << 16 | (0x03 << 8)),
			COFDPATH_TIMER_GET_CONFIG_REPLY 	= ((COFDPATH_TIMER_BASE) << 16 | (0x04 << 8)),
			COFDPATH_TIMER_STATS_REQUEST 		= ((COFDPATH_TIMER_BASE) << 16 | (0x05 << 8)),
			COFDPATH_TIMER_STATS_REPLY 			= ((COFDPATH_TIMER_BASE) << 16 | (0x06 << 8)),
			COFDPATH_TIMER_BARRIER_REQUEST 		= ((COFDPATH_TIMER_BASE) << 16 | (0x09 << 8)),
			COFDPATH_TIMER_BARRIER_REPLY 		= ((COFDPATH_TIMER_BASE) << 16 | (0x0a << 8)),
			COFDPATH_TIMER_LLDP_SEND_DISC 		= ((COFDPATH_TIMER_BASE) << 16 | (0x0b << 8)),
		};

		/* cofdpath state types */
		enum cofdpath_state_t {
			DP_STATE_INIT 				= (1 << 0),
			DP_STATE_RUNNING 			= (1 << 1),
		};

#define DEFAULT_DP_FEATURES_REPLY_TIMEOUT 		10
#define DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT 	10
#define DEFAULT_DP_STATS_REPLY_TIMEOUT 			10
#define DEFAULT_DB_BARRIER_REPLY_TIMEOUT 		10


public: // data structures

														// instance belongs to
														// (hosted by parent cfwdelem instance)
		uint64_t 						dpid;			// datapath id
		std::string	 					s_dpid;			// datapath id as std::string
		cmacaddr 						dpmac;			// datapath mac address
		uint32_t 						n_buffers; 		// number of buffer lines
		uint8_t 						n_tables;		// number of tables
		uint32_t 						capabilities;	// capabilities flags

		std::map<uint32_t, cofport*> 	ports;			// list of ports
		uint16_t 						flags;			// 'fragmentation' flags
		uint16_t 						miss_send_len; 	// length of bytes sent to controller

		std::map<uint8_t, cfttable*> 	flow_tables;	// flow_table of this switch instance
		cgttable 						grp_table;		// group_table of this switch instance
		cfwdtable 						fwdtable; 		// forwarding table for attached MAC
														// addresses (Ethernet endpoints)
		cfsptable 						fsptable;		// flowspace registration table



		friend class cfwdelem;

private:

		cfwdelem 						*fwdelem;		// layer-(n) entity
		cofbase 						*entity;		// layer-(n-1) entity
		std::map<cofbase*, cofdpath*> 	*ofswitch_list; // cofswitch map this
		std::map<uint8_t, cxidstore>	 xidstore;		// transaction store

		std::string 	info;							// info string

		int 			features_reply_timeout;
		int 			get_config_reply_timeout;
		int 			stats_reply_timeout;
		int 			barrier_reply_timeout;

public:

	/**
	 * @name	cofdpath
	 * @brief 	constructor of class cofdpath
	 *
	 * Initializes a new instance of class cofdpath for a new
	 * attaching data path element. Starts handshake with datapath
	 * element in order to acquire all of its state. Adds this to
	 * map of cofdpath instances (dpath_list).
	 *
	 * @param[in] fwdelem	The cfwdelem object storing this cofdpath
	 * @param[in] entity The cofbase object representing the datapath element (@see cofrpc or @see cfwdelem)
	 * @param[in] dpath_list pointer to map that contains all cofdpath instances for fwdelem
	 */
	cofdpath(
		cfwdelem *fwdelem,
		cofbase *entity,
		std::map<cofbase*, cofdpath*>* dpath_list);


	/**
	 * @name	~cofdpath
	 * @brief	destructor of class cofdpath
	 *
	 * Informs fwdelem by calling (@see handle_dpath_close).
	 * Deletes all cofport instances (@see ports).
	 * Removes this pointer from fwdelem->ofdpath_list.
	 */
	virtual
	~cofdpath();


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
	 * overloaded from cftentry_owner
	 */


	/**
	 * @name	ftentry_timeout
	 * @brief	Called when a flowtable entry stored in (@see flow_tables) expires.
	 *
	 * Called by an expired cftentry instance. Default behaviour is to ignore
	 * this event.
	 *
	 * @param[in] entry cftentry instance expired.
	 * @param[in] timeout value for expired timer in seconds.
	 */
	virtual void
    ftentry_timeout(
    		cftentry *entry,
    		uint16_t timeout);


	/*
	 *  overloaded from cgtentry_owner
	 */

	/**
	 * @name	gtentry_timeout
	 * @brief	Called when a grouptable entry stored in (@see group_tables) expires.
	 *
	 * Called by an expired gftentry instance. Default behaviour is to ignore
	 * this event. Note: There are no timeput values for group entries in OF
	 * yet.
	 *
	 * @param[in] entry cgtentry instance expired.
	 * @param[in] timeout value for expired timer in seconds.
	 */
	virtual void
    gtentry_timeout(
    		cgtentry *entry,
    		uint16_t timeout);


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
	 * @name	features_request_sent
	 * @brief	Called by cfwdelem when a FEATURES-request was sent.
	 *
	 * Starts an internal timer for the expected FEATURES-reply.
	 */
	void
	features_request_sent();


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
			cofpacket *pack);


	/**
	 * @name	get_config_request_sent
	 * @brief	Called by cfwdelem when a GET-CONFIG-request was sent.
	 *
	 * Starts an internal timer for the expected GET-CONFIG-reply.
	 */
	void
	get_config_request_sent();


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
			cofpacket *pack);


	/**
	 * @name	stats_request_sent
	 * @brief	Called by cfwdelem when a STATS-request was sent.
	 *
	 * Starts an internal timer for the expected STATS-reply.
	 */
	void
	stats_request_sent(
			uint32_t xid);


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
			cofpacket *pack);



	/**
	 * @name	barrier_request_sent
	 * @brief	Called by cfwdelem when a BARRIER-request was sent.
	 *
	 * Starts an internal timer for the expected BARRIER-reply.
	 */
	void
	barrier_request_sent(
			uint32_t xid);


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
			cofpacket *pack);


	/**
	 * @name	flow_mod_sent
	 * @brief	Called by cfwdelem when a FLOW-MOD-message was sent.
	 *
	 * Applies FlowMod message to local flowtables.
	 *
	 * @param[in] pack The OpenFlow message sent.
	 *
	 * @throws eOFdpathNotFound Thrown when the table-id specified in pack cannot be found.
	 */
	void
	flow_mod_sent(
			cofpacket *pack) throw (eOFdpathNotFound);


	/**
	 * @name	flow_rmvd_rcvd
	 * @brief	Called by cfwdelem when a FLOW-MOD-message was sent.
	 *
	 * Applies FlowRmvd message to local flowtables.
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	flow_rmvd_rcvd(
			cofpacket *pack);


	/**
	 * @name	group_mod_sent
	 * @brief	Called by cfwdelem when a GROUP-MOD-message was sent.
	 *
	 * Applies GroupMod message to local grouptables.
	 *
	 * @param[in] pack The OpenFlow message sent.
	 */
	void
	group_mod_sent(
			cofpacket *pack);


	/**
	 * @name	table_mod_sent
	 * @brief	Called by cfwdelem when a TABLE-MOD-message was sent.
	 *
	 * Applies TableMod message to local flowtables.
	 *
	 * @param[in] pack The OpenFlow message sent.
	 */
	void
	table_mod_sent(
			cofpacket *pack);


	/**
	 * @name	port_mod_sent
	 * @brief	Called by cfwdelem when a PORT-MOD-message was sent.
	 *
	 * Applies PortMod message to local cofport instance.
	 *
	 * @param[in] pack The OpenFlow message sent.
	 */
	void
	port_mod_sent(
			cofpacket *pack);


	/** handle PACKET-IN message
	 */
	void
	packet_in_rcvd(
			cofpacket *pack);


	/** handle PORT-STATUS message
	 */
	void
	port_status_rcvd(
			cofpacket *pack);


	/** handle VENDOR message
	 */
	void
	experimenter_message_rcvd(
			cofpacket *pack);


	/** handle ROLE-REPLY messages
	 */
	void
	role_reply_rcvd(
			cofpacket *pack);




private:


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

};

#endif
