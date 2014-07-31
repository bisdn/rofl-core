/*
 * cofdptImpl.h
 *
 *  Created on: 25.09.2013
 *      Author: andreas
 */

#ifndef COFDPTIMPL_H_
#define COFDPTIMPL_H_

#include <map>
#include <set>
#include <algorithm>
#include <inttypes.h>
#include <stdio.h>
#include <strings.h>

#include "rofl/common/ciosrv.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/cvastring.h"
#include "rofl/common/cfsm.h"
#include "rofl/common/protocols/fetherframe.h"
#include "rofl/common/csocket.h"

#include "../openflow/messages/cofmsg.h"
#include "../openflow/messages/cofmsg_hello.h"
#include "../openflow/messages/cofmsg_echo.h"
#include "../openflow/messages/cofmsg_error.h"
#include "../openflow/messages/cofmsg_features.h"
#include "../openflow/messages/cofmsg_config.h"
#include "../openflow/messages/cofmsg_packet_in.h"
#include "../openflow/messages/cofmsg_flow_removed.h"
#include "../openflow/messages/cofmsg_port_status.h"
#include "../openflow/messages/cofmsg_stats.h"
#include "../openflow/messages/cofmsg_desc_stats.h"
#include "../openflow/messages/cofmsg_flow_stats.h"
#include "../openflow/messages/cofmsg_aggr_stats.h"
#include "../openflow/messages/cofmsg_table_stats.h"
#include "../openflow/messages/cofmsg_port_stats.h"
#include "../openflow/messages/cofmsg_queue_stats.h"
#include "../openflow/messages/cofmsg_group_stats.h"
#include "../openflow/messages/cofmsg_group_desc_stats.h"
#include "../openflow/messages/cofmsg_group_features_stats.h"
#include "../openflow/messages/cofmsg_barrier.h"
#include "../openflow/messages/cofmsg_queue_get_config.h"
#include "../openflow/messages/cofmsg_role.h"
#include "../openflow/messages/cofmsg_experimenter.h"
#include "../openflow/messages/cofmsg_async_config.h"

#include "rofl/common/openflow/cofdpt.h"

namespace rofl
{

/**
 * \class	cofdptImpl
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
class cofdptImpl :
	public cofdpt,
	public csocket_owner,
	public ciosrv,
	public cfsm
{

private: // data structures
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
			COFDPT_TIMER_GET_ASYNC_CONFIG_REQUEST = ((COFDPT_TIMER_BASE) << 16 | (0x0f << 8)),
			COFDPT_TIMER_GET_ASYNC_CONFIG_REPLY = ((COFDPT_TIMER_BASE) << 16 | (0x10 << 8)),
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

#define DEFAULT_DP_FEATURES_REPLY_TIMEOUT 			10
#define DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT 		10
#define DEFAULT_DP_STATS_REPLY_TIMEOUT 				10
#define DEFAULT_DP_BARRIER_REPLY_TIMEOUT 			10
#define DEFAULT_DP_GET_ASYNC_CONFIG_REPLY_TIMEOUT	10

		std::bitset<32>                 flags;

		uint64_t 						dpid;			// datapath id
		std::string	 					s_dpid;			// datapath id as std::string
		cmacaddr 						hwaddr;			// datapath mac address
		uint32_t 						n_buffers; 		// number of buffer lines
		uint8_t 						n_tables;		// number of tables
		uint32_t 						capabilities;	// capabilities flags

		std::map<uint8_t, coftable_stats_reply> tables;	// map of tables: table_id:coftable_stats_reply
		std::map<uint32_t, cofport*> 	ports;			// map of ports
		std::bitset<32> 				dptflags;		// 'fragmentation' flags
		uint16_t						config;
		uint16_t 						miss_send_len; 	// length of bytes sent to controller

		cfsptable 						fsptable;		// flowspace registration table


		csocket							*socket;		// TCP socket towards data path element
		crofbase 						*rofbase;		// layer-(n) entity
		std::map<uint8_t, cxidstore>	 xidstore;		// transaction store

		std::string 					 info;			// info string
		cmemory							*fragment;		// fragment of OF packet rcvd on fragment during last call(s)
		size_t							 msg_bytes_read; // bytes already read for current message
		int								 reconnect_start_timeout;
		int 							 reconnect_in_seconds; 	// reconnect in x seconds
		int 							 reconnect_counter;
		int 							 rpc_echo_interval;		// default ECHO time interval
		uint8_t							 ofp_version;	// OpenFlow version negotiated

		int 							 features_reply_timeout;
		int 							 get_config_reply_timeout;
		int 							 stats_reply_timeout;
		int 							 barrier_reply_timeout;
		int 							 get_async_config_reply_timeout;


public:

	/**
	 * @brief 	Default constructor for generating an empty cofdpt instance
	 *
	 * @param rofbase pointer to crofbase instance
	 */
	cofdptImpl(
			crofbase *rofbase);

	/**
	 * @brief 	Constructor for accepted incoming connection on socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param newsd socket descriptor of new established control connection socket
	 * @param ra peer address of control connection
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 */
	cofdptImpl(
			crofbase *rofbase,
			int newsd,
			caddress const& ra,
			int domain,
			int type,
			int protocol);

	/**
	 * @brief 	Constructor for accepted incoming connection on socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param newsd socket descriptor of new established control connection socket
	 * @param ra peer address of control connection
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
     * @param ofp_version OpenFlow protocol version expected
	 */
	cofdptImpl(
			crofbase *rofbase,
			int newsd,
			caddress const& ra,
			int domain,
			int type,
			int protocol,
            uint8_t ofp_version);

	/**
	 * @brief 	Constructor for creating a new cofdpt instance and actively connecting to a data path element.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param ra peer address of control connection
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 */
	cofdptImpl(
			crofbase *rofbase,
			uint8_t ofp_version,
			int reconnect_start_timeout,
			caddress const& ra,
			int domain,
			int type,
			int protocol);



	/**
	 * @brief	Destructor.
	 *
	 * Deallocates all previously allocated resources for storing data model
	 * exposed by the data path element.
	 */
	virtual
	~cofdptImpl();



	/**
	 * @brief 	Returns the OpenFlow protocol version used for this control connection.
	 *
	 * @return OpenFlow version used for this control connection
	 */
	virtual uint8_t
	get_version();




	/**
	 * @brief	Send an OpenFlow message to the data path element managed by this cofdpt instance.
	 *
	 * @param msg an OpenFlow message
	 */
	virtual void
	send_message(
			cofmsg *msg);



	/**
	 * @brief	Returns a C-string with a description of this cofdpt instance.
	 *
	 * @return C-string
	 */
	virtual const char*
	c_str();


	/**
	 * @brief	Returns caddress of connected remote entity.
	 *
	 * @return caddress object obtained from this->socket
	 */
	virtual caddress
	get_peer_addr();


public:

	/**
	 * @name	Data path property related methods
	 *
	 * These methods provide an interface for querying various properties of the attached data path element.
	 */

	/**@{*/

	/**
	 * @brief 	Find a cofport instance based on OpenFlow port number.
	 *
	 * @return pointer to cofport instance
	 * @throws eOFdpathNotFound if port could not be found
	 */
	virtual cofport*
	find_cofport(
			uint32_t port_no) throw (eOFdpathNotFound);


	/**
	 * @brief 	Find a cofport instance based on OpenFlow port name (e.g. eth0).
	 *
	 * @return pointer to cofport instance
	 * @throws eOFdpathNotFound if port could not be found
	 */
	virtual cofport*
	find_cofport(
			std::string port_name) throw (eOFdpathNotFound);


	/**
	 * @brief 	Find a cofport instance based on OpenFlow port hardware address.
	 *
	 * @return pointer to cofport instance
	 * @throws eOFdpathNotFound if port could not be found
	 */
	virtual cofport*
	find_cofport(
			cmacaddr const& maddr) throw (eOFdpathNotFound);


	/**
	 * @brief	Returns the data path element's data path ID.
	 *
	 * @return dpid
	 */
	virtual uint64_t
	get_dpid() const { return dpid; };


	/**
	 * @brief	Returns the data path element's ID string.
	 *
	 * @return s_dpid
	 */
	virtual std::string
	get_dpid_s() const { return s_dpid; };


	/**
	 * @brief	Returns the data path element's hardware address.
	 *
	 * @return hwaddr
	 */
	virtual cmacaddr
	get_hwaddr() const { return hwaddr; };


	/**
	 * @brief	Returns the data path element's number of buffers for storing data packets.
	 *
	 * @return n_buffers
	 */
	virtual uint32_t
	get_n_buffers() const { return n_buffers; };


	/**
	 * @brief	Returns the data path element's number of tables in the OpenFlow pipeline.
	 *
	 * @return n_tables
	 */
	virtual uint8_t
	get_n_tables() const { return n_tables; };


	/**
	 * @brief	Returns the data path element's capabilities.
	 *
	 * @return capabilities
	 */
	virtual uint32_t
	get_capabilities() const { return capabilities; };


	/**
	 * @brief	Returns the data path element's configuration.
	 *
	 * @return config
	 */
	virtual uint16_t
	get_config() const { return config; };


	/**
	 * @brief	Returns the data path element's current miss_send_len parameter.
	 *
	 * @return miss_send_len
	 */
	virtual uint16_t
	get_miss_send_len() const { return miss_send_len; };


	/**
	 * @brief	Returns reference to the data path element's flowspace table.
	 *
	 * @return fsptable
	 */
	virtual cfsptable&
	get_fsptable() { return fsptable; };


	/**
	 * @brief	Returns reference to the data path element's cofport list.
	 *
	 * @return ports
	 */
	virtual std::map<uint32_t, cofport*>&
	get_ports() { return ports; };


	/**
	 * @brief	Returns reference to cofport instance assigned to portno.
	 *
	 * if portno is invalid, an exeception of type eOFdpathNotFound is thrown.
	 *
	 * @return reference to cofport instance assigned to specified portno
	 */
	virtual cofport&
	get_port(uint32_t portno) {
		if (ports.find(portno) == ports.end())
			throw eOFdpathNotFound();
		return *(ports[portno]);
	};


	/**
	 * @brief	Returns reference to the data path element's coftable_stats_reply list.
	 *
	 * @return tables
	 */
	virtual std::map<uint8_t, coftable_stats_reply>&
	get_tables() { return tables; };


	/**@}*/


public:


	/**
	 * @name Flowspace management methods
	 *
	 * ROFL contains a set of extensions that allows a controller to express parts
	 * of the overall namespace he is willing to control. The flowspace registration
	 * contains an OpenFlow match structure. Currently, a registration is hard state,
	 * i.e. it will be removed only when explicitly requested by the controller or
	 * the control connection between controller and data path is lost.
	 *
	 * Please note: this is going to change in a future revision by a soft state approach!
	 */

	/**@{*/

	/**
	 * @brief 	Makes a new flowspace registration at the data path element.
	 *
	 * This method registers a flowspace on the attached datapath element.
	 * Calling this method multiple times results in several flowspace
	 * registrations.
	 *
	 * @param ofmatch the flowspace definition to be registered
	 */
	virtual void
	fsp_open(
			cofmatch const& ofmatch);


	/**
	 * @brief 	Removes a flowspace registration from the attached data path element.
	 *
	 * This method deregisters a flowspace on the attached datapath element.
	 * The default argument is an empty (= all wildcard ofmatch) and removes
	 * all active flowspace registrations from the datapath element.
	 *
	 * @param ofmatch the flowspace definition to be removed
	 */
	virtual void
	fsp_close(
			cofmatch const& ofmatch = cofmatch());

	/**@}*/


public:

	/**
	 * @name	FlowMod management methods
	 *
	 * These methods provide a simple to use interface for managing FlowMod
	 * entries.
	 *
	 * Please note: these methods are subject to change in future revisions.
	 */

	/**@{*/

	/**
	 * @brief	Removes all flowtable entries from the attached datapath element.
	 *
	 * Sends a FlowMod-Delete message to the attached datapath element for removing
	 * all flowtable entries.
	 */
	virtual void
	flow_mod_reset();


	/**
	 * @brief	Removes all grouptable entries from the attached datapath element.
	 *
	 * Sends a GroupMod-Delete message to the attached datapath element for removing
	 * all grouptable entries.
	 */
	virtual void
	group_mod_reset();

	/**@}*/

private:

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
	 * @brief	Called by cfwdelem when a STATS-reply was received.
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
	 * @name	table_stats_reply_rcvd
	 * @brief	Called by cofdpt when a TABLE-STATS-reply was received.
	 *
	 * Stores parameters received in internal variables.
	 * Calls method fwdelem->handle_table_stats_reply().
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	table_stats_reply_rcvd(
			cofmsg_table_stats_reply *msg);


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


	/**
	 * @name	get_async_config_request_sent
	 * @brief	Called by crofbase when a GET-ASYNC-CONFIG-request was sent.
	 *
	 * Starts an internal timer for the expected GET-ASYNC-CONFIG-reply.
	 */
	void
	get_async_config_request_sent(
			cofmsg *msg);


	/**
	 * @name	get_async_config_reply_rcvd
	 * @brief	Called by crofbase when a GET-ASYNC-CONFIG-reply was received.
	 *
	 * Cancels the internal timer waiting for GET-ASYNC-CONFIG-reply.
	 * Stores parameters received in internal variables.
	 * Starts timer for sending a TABLE-STATS-request.
	 *
	 * @param[in] pack The OpenFlow message received.
	 */
	void
	get_async_config_reply_rcvd(
			cofmsg_get_async_config_reply *msg);



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


	/** handle GET-ASYNC-CONFIG reply timeout
	 */
	void
	handle_get_async_config_reply_timeout();


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

#endif /* COFDPTIMPL_H_ */
