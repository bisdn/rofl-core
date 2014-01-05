/*
 * crofbase.h
 *
 *  Created on: 25.10.2012
 *      Author: andreas
 */

#ifndef CROFBASE_H
#define CROFBASE_H 1

#include <map>
#include <set>
#include <list>
#include <vector>
#include <bitset>
#include <algorithm>
#include <endian.h>
#include <string.h>
#include <time.h>
#ifndef htobe16
	#include "endian_conversion.h"
#endif

#include "ciosrv.h"
#include "croflexception.h"
#include "cvastring.h"
#include "croflexp.h"
#include "csocket.h"
#include "thread_helper.h"
#include "logging.h"
#include "crofdpt.h"
#include "crofdptImpl.h"
#include "crofctl.h"
#include "crofctlImpl.h"
#include "cflowentry.h"
#include "cgroupentry.h"

#include "openflow/openflow.h"

#include "rofl/platform/unix/crandom.h"

#include "openflow/cofport.h"
#include "openflow/cofinst.h"
#include "openflow/cofinstructions.h"
#include "openflow/cofaction.h"
#include "openflow/cofactions.h"
#include "openflow/cofpacketqueuelist.h"
#include "openflow/cofmatch.h"
#include "openflow/cofstats.h"
#include "openflow/extensions/cfsptable.h"
#include "openflow/openflow_rofl_exceptions.h"
#include "openflow/cofhelloelemversionbitmap.h"
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

namespace rofl
{


/* error classes */
class eRofBase						: public RoflException {};   // base error class crofbase
class eRofBaseIsBusy 				: public eRofBase {}; // this FwdElem is already controlled
class eRofBaseNotConnected			: public eRofBase {}; // this instance is not connected to the specified cofdpt/cofctl instance
class eRofBaseNotImpl 				: public eRofBase {}; // this FwdElem's method is not implemented
class eRofBaseNoCtrl 				: public eRofBase {}; // no controlling entity attached to this FwdElem
class eRofBaseNotFound 				: public eRofBase {}; // internal entity not found
class eRofBaseInval					: public eRofBase {}; // invalid parameter (e.g. invalid packet type)
class eRofBaseNotAttached 			: public eRofBase {}; // received command from entity being not attached
class eRofBaseNoRequest 			: public eRofBase {}; // no request packet found for session
class eRofBaseXidInval	 			: public eRofBase {}; // invalid xid in session exchange
class eRofBaseExists				: public eRofBase {}; // fwdelem with either this dpid or dpname already exists
class eRofBaseOFportNotFound 		: public eRofBase {}; // cofport instance not found
class eRofBaseTableNotFound 		: public eRofBase {}; // flow-table not found (e.g. unknown table_id in flow_mod)
class eRofBaseGotoTableNotFound 	: public eRofBase {}; // table-id specified in OFPIT_GOTO_TABLE invalid
class eRofBaseFspSupportDisabled 	: public eRofBase {};




class crofctl;
class crofdpt;



/**
 * \class crofbase
 * \brief Revised OpenFlow Library core class.
 *
 * This class provides functionality for creating OpenFlow
 * endpoints for data path elements or controllers. crofbase
 * provides functions for opening listening sockets and
 * creates instances of \see{ cofctl } and \see{ cofdpt }
 * for handling individual sessions.
 */
class crofbase :
	public ciosrv,
	public csocket_owner,
	public cfsm
{
protected:

	uint32_t					supported_ofp_versions;	/**< bitfield of supported ofp versions */
	cfsptable 					fsptable; 		/**< flowspace registrations table */
	std::set<crofctl*>			ofctl_set;		/**< set of active controller connections */
	std::set<crofdpt*>			ofdpt_set;		/**< set of active data path connections */

public:

	//friend class cport;
	static std::set<crofbase*> 	rofbases; 		/**< set of all active crofbase instances */

public:


	/**
	 * @fn		crofbase
	 * @brief	Constructor for crofbase
	 * @param 	supported_ofp_versions: bitfield of ofp versions to support ((1 << OFP10_VERSION) | (1 << ...))
	 *
	 * Initializes structures for transaction identifiers. xidlock is the rwlock
	 * for manipulating the transaction id maps. xid_start defines the first
	 * transaction id used by crofbase. xid_start is incremented by one for each new
	 * transaction.
	 *
	 * \see xidlock
	 * \see xid_start
	 */
	crofbase(uint32_t supported_ofp_versions = /*(1 << OFP10_VERSION) |*/ (1 << OFP12_VERSION));


	/**
	 * @fn		~crofbase
	 * @brief	Destructor for crofbase
	 *
	 * The destructor shuts down all active connections and listening sockets.
	 *
	 */
	virtual
	~crofbase();



	/**
	 * @brief	Method for waking up this thread from another thread.
	 *
	 */
	void
	wakeup();




	/**
	 * @brief 	enable/disable flowspace registration support in crofbase
	 *
	 * @param enable true: enable flowspace support, false: disable flowspace support
	 */
	void
	nsp_enable(bool enable = true);






public:

	/**
	 * @name	RPC related methods for opening/closing TCP connections and listening sockets
	 */

	/**@{*/

	/**
	 * @fn		rpc_listen_for_dpts
	 * @brief	Opens a listening socket for accepting connection requests from data path elements
	 *
	 * @param addr Address to bind for listening
	 * @param domain Socket domain
	 * @param type Socket type
	 * @param protocol Socket protocol
	 * @param backlog Maximum number of concurrent pending connection requests
	 */
	void
	rpc_listen_for_dpts(
			caddress const& addr = caddress(AF_INET, "0.0.0.0", 6633),
			int domain = PF_INET,
			int type = SOCK_STREAM,
			int protocol = IPPROTO_TCP,
			int backlog = 10);


	/**
	 * @fn		rpc_listen_for_ctls
	 * @brief	Opens a listening socket for accepting connection requests from controllers
	 *
	 * @param addr Address to bind for listening
	 * @param domain Socket domain
	 * @param type Socket type
	 * @param protocol Socket protocol
	 * @param backlog Maximum number of concurrent pending connection requests
	 */
	void
	rpc_listen_for_ctls(
			caddress const& addr = caddress(AF_INET, "0.0.0.0", 6644),
			int domain = PF_INET,
			int type = SOCK_STREAM,
			int protocol = IPPROTO_TCP,
			int backlog = 10);


	/**
	 * @fn	 	rpc_connect_to_ctl
	 * @brief	Connects to a remote controller in data path role.
	 *
	 * Establishes a socket connection to a remote controller entity.
	 * When the connection is successfully established, crofbase calls
	 * method crofbase::handle_ctrl_open().
	 *
	 * \see{ handle_ctrl_open() }
	 *
	 * @param ofp_version OpenFlow version to use for connecting to controller
	 * @param ra Address to connect to
	 * @param domain Socket domain (default: PF_INET)
	 * @param type Socket type (default: SOCK_STREAM)
	 * @param protocol Socket protocol (default: IPPROTO_TCP)
	 */
	void
	rpc_connect_to_ctl(
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
			int reconnect_start_timeout,
			caddress const& ra,
			int domain = PF_INET,
			int type = SOCK_STREAM,
			int protocol = IPPROTO_TCP);


	/**
	 * @fn		rpc_disconnect_from_ctl
	 * @brief 	Closes a connection to a controller entity with a proper shutdown.
	 *
	 * \see{ handle_ctrl_close() }
	 *
	 * @param ctl cofctl instance to be disconnected
	 */
	void
	rpc_disconnect_from_ctl(
			crofctl *ctl);



	/**
	 * @fn		rpc_disconnect_from_ctl
	 * @brief 	Closes a connection to a controller entity with a proper shutdown.
	 *
	 * \see{ handle_ctrl_close() }
	 *
	 * @param ctl cofctl instance to be disconnected
	 */
	void
	rpc_disconnect_from_ctl(
			caddress const& ra);



	/**
	 * @fn	 	rpc_connect_to_dpt
	 * @brief	Connects to a remote data path in controller role.
	 *
	 * Establishes a socket connection to a remote data path element.
	 * When the connection is successfully established, crofbase calls
	 * method crofbase::handle_dpath_open().
	 *
	 * \see{ handle_dpath_open() }
	 *
	 * @param ofp_version OpenFlow version to use for connecting to data path element
	 * @param ra Address to connect to
	 * @param domain Socket domain (default: PF_INET)
	 * @param type Socket type (default: SOCK_STREAM)
	 * @param protocol Socket protocol (default: IPPROTO_TCP)
	 */
	void
	rpc_connect_to_dpt(
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
			int reconnect_start_timeout,
			caddress const& ra,
			int domain = PF_INET,
			int type = SOCK_STREAM,
			int protocol = IPPROTO_TCP);


	/**
	 * @fn		rpc_disconnect_from_dpt
	 * @brief 	Closes a connection to a data path entity with a proper shutdown.
	 *
	 * \see{ handle_dpath_close() }
	 *
	 * @param dpt cofdpt instance to be disconnected
	 */
	void
	rpc_disconnect_from_dpt(
			crofdpt *dpath);



	/**
	 * @fn		rpc_disconnect_from_dpt
	 * @brief 	Closes a connection to a data path entity with a proper shutdown.
	 *
	 * \see{ handle_dpath_close() }
	 *
	 * @param dpt cofdpt instance to be disconnected
	 */
	void
	rpc_disconnect_from_dpt(
			caddress const& ra);



	/**
	 * @brief	Closes all open cofctl, cofdpt and listening socket instances.
	 *
	 */
	void
	rpc_close_all();


	/**
	 * @brief	Called by cofctl instance when connection establishment failed.
	 *
	 * To be overwritten by class deriving from crofbase.
	 */
	virtual void
	rpc_ctl_failed(crofctl *ctl) {};

	/**
	 * @brief	Called by cofdpt instance when connection establishment failed.
	 *
	 * To be overwritten by class deriving from crofbase.
	 */
	virtual void
	rpc_dpt_failed(crofdpt *dpt) {};


	/**@}*/


public:

	/**
	 * @name	Session related methods for data path elements (cofdpt) and controllers (cofctl)
	 */

	/**@{*/

	/**
	 * @brief	returns pointer to cofdpt instance
	 *
	 * @param dpid data path identifier as uint64_t parameter
	 * @throws eRofBaseNotFound { thrown when cofdpt instance not found }
	 * @result pointer to cofdpt instance
	 */
	crofdpt*
	dpt_find(
		uint64_t dpid) throw (eRofBaseNotFound);


	/**
	 * @brief 	returns pointer to cofdpt instance
	 *
	 * @param s_dpid data path identifier as std::string parameter
	 * @throws eRofBaseNotFound { thrown when cofdpt instance not found }
	 * @result pointer to cofdpt instance
	 */
	crofdpt*
	dpt_find(
		std::string s_dpid) throw (eRofBaseNotFound);


	/**
	 * @brief	returns pointer to cofdpt instance
	 *
	 * @param dl_dpid data path MAC address
	 * @throws eRofBaseNotFound { thrown when cofdpt instance not found }
	 * @result pointer to cofdpt instance
	 */
	crofdpt*
	dpt_find(
		cmacaddr dl_dpid) throw (eRofBaseNotFound);


	/**
	 * @brief 	returns pointer to cofdpt instance
	 *
	 * @param dpt pointer to cofdpt instance
	 * @throws eRofBaseNotFound { thrown when cofdpt instance not found }
	 * @result pointer to cofdpt instance
	 */
	crofdpt*
	dpt_find(
			crofdpt* dpt) throw (eRofBaseNotFound);



	/**
	 * @brief	returns pointer to cofctl instance
	 *
	 * @param ctl pointer to cofctl instance
	 * @throws eRofBaseNotFound { thrown when cofctl instance not found }
	 * @result pointer to cofctl instance
	 */
	crofctl*
	ctl_find(
			crofctl* ctl) throw (eRofBaseNotFound);


	/**@}*/


	/**
	 *
	 */
	static uint32_t
	get_ofp_no_buffer(uint8_t ofp_version);

	/**
	 *
	 */
	static uint32_t
	get_ofp_flood_port(uint8_t ofp_version);

	/**
	 *
	 */
	static uint8_t
	get_ofp_command(uint8_t ofp_version, enum openflow::ofp_flow_mod_command const& cmd);

	/**
	 *
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap() { return versionbitmap; };



protected:


	/**
	 * @name Methods for managing cofctl and cofdpt instances
	 */


	/**@{*/

	/**
	 * @brief	creates a new cofctl instance for an existing socket with sockfd newsd.
	 *
	 * This method constructs a new instance of class cofctl for managing a single connection
	 * to a controller. This class is supposed to be overwritten, if a class derived from crofbase
	 * intends to overwrite cofctl and add additional functionality. When the initial HELLO message
	 * exchange in OpenFlow succeeds, method crofbase::handle_ctrl_open() will be called.
	 *
	 * @param owner Pointer to this crofbase instance for callbacks used by the cofctl instance
	 * @param versionbitmap version-bitmap Hello IE containing acceptable OFP versions
	 * @param newsd socket descriptor of new created socket for cofctl instance
	 *
	 */
	virtual crofctl*
	cofctl_factory(
			crofbase* owner,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
			int newsd);


	/**
	 * @brief	Creates a new cofctl instance and tries to connect to a remote controller entity.
	 *
	 * This method constructs a new instance of class cofctl for actively establishing a single connection
	 * to a controller. This class is supposed to be overwritten, if a class derived from crofbase
	 * intends to overwrite cofctl and add additional functionality. cofctl will indefinitely attempt
	 * to connect to the peer entity unless it is removed by calling crofbase::rpf_disconnect_from_ctl().
	 * When connection setup and the initial HELLO message exchange in OpenFlow succeeds, method
	 * crofbase::handle_ctrl_open() will be called.
	 *
	 * @param owner Pointer to this crofbase instance for callbacks used by the cofctl instance
	 * @param ofp_version OpenFlow version to use for connecting to controller
	 * @param ra Remote address to connect to
	 * @param domain socket domain (see man 2 socket for details)
	 * @param type socket type (see man 2 socket for details)
	 * @param protocol socket protocol (see man 2 socket for details)
	 *
	 */
	virtual crofctl*
	cofctl_factory(
			crofbase* owner,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
			int reconnect_start_timeout,
			caddress const& ra,
			int domain,
			int type,
			int protocol);


	/**
	 * @brief	creates a new cofdpt instance for an existing socket with sockfd newsd.
	 *
	 * This method constructs a new instance of class cofdpt for managing a single connection
	 * to a data path element. This class is supposed to be overwritten, if a class derived from crofbase
	 * intends to overwrite cofdpt and add additional functionality. When the initial handshake in OpenFlow
	 * succeeds (FEATURES.request/reply, GET-CONFIG.request/reply, TABLE-STATS.request/reply), method
	 * crofbase::handle_ctrl_open() will be called.
	 *
	 * @param owner Pointer to this crofbase instance for callbacks used by the cofdpt instance
	 * @param versionbitmap version-bitmap Hello IE containing acceptable OFP versions
	 * @param newsd socket descriptor of new created socket for cofdpt instance
	 *
	 */
	virtual crofdpt*
	cofdpt_factory(
			crofbase* owner,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
			int newsd);


	/**
	 * @brief	Creates a new cofdpt instance and tries to connect to a remote data path element.
	 *
	 * This method constructs a new instance of class cofdpt for actively establishing a single connection
	 * to a data path element. This class is supposed to be overwritten, if a class derived from crofbase
	 * intends to overwrite cofdpt and add additional functionality. cofdpt will indefinitely attempt
	 * to connect to the peer entity unless it is removed by calling crofbase::rpf_disconnect_from_dpt().
	 * When the initial handshake in OpenFlow succeeds (FEATURES.request/reply, GET-CONFIG.request/reply,
	 * TABLE-STATS.request/reply), method crofbase::handle_dpath_open() will be called.
	 *
	 * @param owner Pointer to this crofbase instance for callbacks used by the cofdpt instance
	 * @param ofp_version OpenFlow version to use for connecting to data path element
	 * @param ra Remote address to connect to
	 * @param domain socket domain (see man 2 socket for details)
	 * @param type socket type (see man 2 socket for details)
	 * @param protocol socket protocol (see man 2 socket for details)
	 *
	 */
	virtual crofdpt*
	cofdpt_factory(
			crofbase* owner,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
			int reconnect_start_timeout,
			caddress const& ra,
			int domain,
			int type,
			int protocol);



	/**
	 * @brief	called once a new cofdpt instance has been created
	 *
	 * This method is called, once a new datapath element has attached to this controller.
	 * Should be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to new cofdpt instance
	 */
	virtual void
	handle_dpath_open(crofdpt *dpt) {};


	/**
	 * @brief	called once a cofdpt instance has been closed
	 *
	 * Note, that this method is called, once the peer entity has closed the
	 * control connection. Thus, no further commands should be sent via this
	 * session object. This method should be overwritten by derived class for
	 * deallocating any previously allocated resource. Default behaviour: ignores event.
	 * The further fate of the cofdpt instance depends on its creation:
	 * for a listening socket, the cofdpt instance will be destroyed automatically.
	 * for a connecting socket, the cofdpt instance will try to reestablish the connection, unless
	 * told otherwise.
	 *
	 * \see{ rpc_disconnect_from_dpt }
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	handle_dpath_close(crofdpt *dpt) {};



	/**
	 * @brief	called once a new cofctl instance has been created
	 *
	 * This method is called, once a new controller entity has attached to this data path element.
	 * Should be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param ctl pointer to new cofctl instance
	 */
	virtual void
	handle_ctrl_open(crofctl *ctl) {};



	/**
	 * @brief	called once a cofctl instance has been closed
	 *
	 * Note, that this method is called, once the peer entity has closed the
	 * control connection. Thus, no further commands should be sent via this
	 * session object. This method should be overwritten by derived class for
	 * deallocating any previously allocated resource. Default behaviour: ignores event.
	 * The further fate of the cofctl instance depends on its creation:
	 * for a listening socket, the cofctl instance will be destroyed automatically.
	 * for a connecting socket, the cofctl instance will try to reestablish the connection, unless
	 * told otherwise.
	 *
	 * \see{ rpc_disconnect_from_ctl }
	 *
	 * @param ctl pointer to cofctl instance
	 */
	virtual void
	handle_ctrl_close(crofctl *ctl) {};


	/**@}*/


	/**
	 * @brief	called once a cofctl instance has received a role request
	 *
	 *
	 */
	virtual void
	role_request_rcvd(crofctl *ctl, uint32_t role);


protected:

	/**
	 * @name Event handlers
	 *
	 * The following methods should be overwritten by a derived class
	 * in order to get reception notifications for the various OF
	 * packets. While crofbase handles most of the lower layer details,
	 * a derived class must provide higher layer functionality.
	 */

	/**@{*/

	/**
	 * @brief	Called once a FEATURES.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the FEATURES.request was received
	 * @param msg Pointer to cofmsg_features_request message containing the received message
	 */
	virtual void
	handle_features_request(crofctl *ctl, cofmsg_features_request *msg) { delete msg; };



	/**
	 * @brief	Called once a FEATURES.reply message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt Pointer to cofdpt instance from which the FEATURES.reply was received
	 * @param msg Pointer to cofmsg_features_reply message containing the received message
	 */
	virtual void
	handle_features_reply(crofdpt *dpt, cofmsg_features_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a timer expires for a FEATURES.reply message.
	 *
	 * Default behaviour: deletes cofdpt instance, thus effectively closing the control connection.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	handle_features_reply_timeout(crofdpt *dpt);



	/**
	 * @brief	Called once a GET-CONFIG.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the GET-CONFIG.request was received
	 * @param msg Pointer to cofmsg_get_config_request message containing the received message
	 */
	virtual void
	handle_get_config_request(crofctl *ctl, cofmsg_get_config_request *msg) { delete msg; };



	/**
	 * @brief	Called once a GET-CONFIG.reply message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt Pointer to cofdpt instance from which the GET-CONFIG.reply was received
	 * @param msg Pointer to cofmsg_get_config_reply message containing the received message
	 */
	virtual void
	handle_get_config_reply(crofdpt *dpt, cofmsg_get_config_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a timer expires for a GET-CONFIG.reply message.
	 *
	 * Default behaviour: deletes cofdpt instance, thus effectively closing the control connection.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	handle_get_config_reply_timeout(crofdpt *dpt);



	/**
	 * @brief	Called once a STATS.request message was received from a controller entity.
	 *
	 * This method dispatches the received STATS.request by calling individual statistics
	 * handlers. The derived class should overwrite all specific handlers as required.
	 * The non-overwritten ones will generate a proper error message for the controller entity.
	 * Default behaviour: throws an exception of type eBadRequestbadStats.
	 *
	 * @see handle_desc_stats_request
	 * @see handle_table_stats_request
	 * @see handle_port_stats_request
	 * @see handle_flow_stats_request
	 * @see handle_aggregate_stats_request
	 * @see handle_queue_stats_request
	 * @see handle_group_stats_request
	 * @see handle_group_desc_stats_request
	 * @see handle_group_features_stats_request
	 * @see handle_experimenter_stats_request
	 *
	 * @param ctl Pointer to cofctl instance from which the STATS.request was received
	 * @param msg Pointer to cofmsg_get_config_request message containing the received message
	 * @exception eBadRequestBadStat { sends a proper error message to the controller entity }
	 */
	void
	handle_stats_request(crofctl *ctl, cofmsg_stats_request *msg) { throw eBadRequestBadStat(); };


	/**
	 * @brief	Called once a DESC-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the DESC-STATS.request was received
	 * @param msg Pointer to cofmsg_desc_stats_request message containing the received message
	 */
	virtual void
	handle_desc_stats_request(crofctl *ctl, cofmsg_desc_stats_request *msg) { throw eBadRequestBadStat(); };



	/**
	 * @brief	Called once a TABLE-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the TABLE-STATS.request was received
	 * @param msg Pointer to cofmsg_table_stats_request message containing the received message
	 */
	virtual void
	handle_table_stats_request(crofctl *ctl, cofmsg_table_stats_request *msg) { throw eBadRequestBadStat(); };



	/**
	 * @brief	Called once a PORT-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the PORT-STATS.request was received
	 * @param msg Pointer to cofmsg_port_stats_request message containing the received message
	 */
	virtual void
	handle_port_stats_request(crofctl *ctl, cofmsg_port_stats_request *msg) { throw eBadRequestBadStat(); };



	/**
	 * @brief	Called once a FLOW-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the FLOW-STATS.request was received
	 * @param msg Pointer to cofmsg_flow_stats_request message containing the received message
	 */
	virtual void
	handle_flow_stats_request(crofctl *ctl, cofmsg_flow_stats_request *msg) { throw eBadRequestBadStat(); };



	/**
	 * @brief	Called once an AGGREGATE-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the AGGREGATE-STATS.request was received
	 * @param msg Pointer to cofmsg_aggr_stats_request message containing the received message
	 */
	virtual void
	handle_aggregate_stats_request(crofctl *ctl, cofmsg_aggr_stats_request *msg) { throw eBadRequestBadStat(); };



	/**
	 * @brief	Called once a QUEUE-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the QUEUE-STATS.request was received
	 * @param msg Pointer to cofmsg_queue_stats_request message containing the received message
	 */
	virtual void
	handle_queue_stats_request(crofctl *ctl, cofmsg_queue_stats_request *msg) { throw eBadRequestBadStat(); };



	/**
	 * @brief	Called once a GROUP-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the GROUP-STATS.request was received
	 * @param msg Pointer to cofmsg_group_stats_request message containing the received message
	 */
	virtual void
	handle_group_stats_request(crofctl *ctl, cofmsg_group_stats_request *msg) { throw eBadRequestBadStat(); };



	/**
	 * @brief	Called once a GROUP-DESC-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the GROUP-DESC-STATS.request was received
	 * @param msg Pointer to cofmsg_group_desc_stats_request message containing the received message
	 */
	virtual void
	handle_group_desc_stats_request(crofctl *ctl, cofmsg_group_desc_stats_request *msg) { throw eBadRequestBadStat(); };



	/**
	 * @brief	Called once a GROUP-FEATURES-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the GROUP-FEATURES-STATS.request was received
	 * @param msg Pointer to cofmsg_group_features_stats_request message containing the received message
	 */
	virtual void
	handle_group_features_stats_request(crofctl *ctl, cofmsg_group_features_stats_request *msg) { throw eBadRequestBadStat(); };



	/**
	 * @brief	Called once a EXPERIMENTER-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the EXPERIMENTER-STATS.request was received
	 * @param msg Pointer to cofmsg_experimenter_stats_request message containing the received message
	 */
	virtual void
	handle_experimenter_stats_request(crofctl *ctl, cofmsg_experimenter_stats_request *msg) { throw eBadRequestBadStat(); };



	/**
	 * @brief	Called once a STATS.reply message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the STATS.reply message was received.
	 * @param msg pointer to cofmsg_stats_reply message containing the received message
	 */
	virtual void
	handle_stats_reply(crofdpt *dpt, cofmsg_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a timer has expired for a STATS.reply message.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of STATS.request previously sent to data path element.
	 */
	virtual void
	handle_stats_reply_timeout(crofdpt *dpt, uint32_t xid) {};



	/**
	 * @brief	Called once a DESC-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the DESC-STATS.reply message was received.
	 * @param msg pointer to cofmsg_desc_stats_reply message containing the received message
	 */
	virtual void
	handle_desc_stats_reply(crofdpt *dpt, cofmsg_desc_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a TABLE-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the TABLE-STATS.reply message was received.
	 * @param msg pointer to cofmsg_table_stats_reply message containing the received message
	 */
	virtual void
	handle_table_stats_reply(crofdpt *dpt, cofmsg_table_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a PORT-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the PORT-STATS.reply message was received.
	 * @param msg pointer to cofmsg_port_stats_reply message containing the received message
	 */
	virtual void
	handle_port_stats_reply(crofdpt *dpt, cofmsg_port_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a FLOW-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the FLOW-STATS.reply message was received.
	 * @param msg pointer to cofmsg_flow_stats_reply message containing the received message
	 */
	virtual void
	handle_flow_stats_reply(crofdpt *dpt, cofmsg_flow_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once an AGGREGATE-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the AGGREGATE-STATS.reply message was received.
	 * @param msg pointer to cofmsg_aggregate_stats_reply message containing the received message

	 */
	virtual void
	handle_aggregate_stats_reply(crofdpt *dpt, cofmsg_aggr_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a QUEUE-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the QUEUE-STATS.reply message was received.
	 * @param msg pointer to cofmsg_queue_stats_reply message containing the received message
	 */
	virtual void
	handle_queue_stats_reply(crofdpt *dpt, cofmsg_queue_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a GROUP-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the GROUP-STATS.reply message was received.
	 * @param msg pointer to cofmsg_group_stats_reply message containing the received message
	 */
	virtual void
	handle_group_stats_reply(crofdpt *dpt, cofmsg_group_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a GROUP-DESC-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the GROUP-DESC-STATS.reply message was received.
	 * @param msg pointer to cofmsg_group_desc_stats_reply message containing the received message
	 */
	virtual void
	handle_group_desc_stats_reply(crofdpt *dpt, cofmsg_group_desc_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a GROUP-FEATURES-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the GROUP-FEATURES-STATS.reply message was received.
	 * @param msg pointer to cofmsg_group_features_stats_reply message containing the received message
	 */
	virtual void
	handle_group_features_stats_reply(crofdpt *dpt, cofmsg_group_features_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once an EXPERIMENTER-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the EXPERIMENTER-STATS.reply message was received.
	 * @param msg pointer to cofmsg_experimenter_stats_reply message containing the received message
	 */
	virtual void
	handle_experimenter_stats_reply(crofdpt *dpt, cofmsg_experimenter_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a PACKET-OUT.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the PACKET-OUT.message was received
	 * @param msg Pointer to cofmsg_packet_out message containing the received message
	 */
	virtual void
	handle_packet_out(crofctl *ctl, cofmsg_packet_out *msg) { delete msg; };



	/**
	 * @brief	Called once a PACKET-IN.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the PACKET-IN.message was received.
	 * @param msg pointer to cofmsg_packet_in message containing the received message
	 */
	virtual void
	handle_packet_in(crofdpt *dpt, cofmsg_packet_in *msg) { delete msg; };



	/**
	 * @brief	Called once a BARRIER.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the BARRIER.request was received
	 * @param msg Pointer to cofmsg_barrier_request message containing the received message
	 */
	virtual void
	handle_barrier_request(crofctl *ctl, cofmsg_barrier_request *msg) { delete msg; };



	/**
	 * @brief	Called once a BARRIER.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the BARRIER.reply message was received.
	 * @param msg pointer to cofmsg_barrier_reply message containing the received message
	 */
	virtual void
	handle_barrier_reply(crofdpt *dpt, cofmsg_barrier_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a timer has expired for a BARRIER.reply message.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of BARRIER.request previously sent to data path element.
	 */
	virtual void
	handle_barrier_reply_timeout(crofdpt *dpt, uint32_t xid) {};



	/**
	 * @brief	Called once an ERROR.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the ERROR.message was received.
	 * @param msg pointer to cofmsg_error message containing the received message
	 */
	virtual void
	handle_error(crofdpt *dpt, cofmsg_error *msg) { delete msg; };



	/**
	 * @brief	Called once a FLOW-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the FLOW-MOD.message was received
	 * @param msg Pointer to cofmsg_flow_mod message containing the received message
	 */
	virtual void
	handle_flow_mod(crofctl *ctl, cofmsg_flow_mod *msg) { delete msg; };



	/**
	 * @brief	Called once a GROUP-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the GROUP-MOD.message was received
	 * @param msg Pointer to cofmsg_group_mod message containing the received message
	 */
	virtual void
	handle_group_mod(crofctl *ctl, cofmsg_group_mod *msg) { delete msg; };



	/**
	 * @brief	Called once a TABLE-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the TABLE-MOD.message was received
	 * @param msg Pointer to cofmsg_table_mod message containing the received message
	 */
	virtual void
	handle_table_mod(crofctl *ctl, cofmsg_table_mod *msg) { delete msg; };



	/**
	 * @brief	Called once a PORT-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the PORT-MOD.message was received
	 * @param msg Pointer to cofmsg_port_mod message containing the received message
	 */
	virtual void
	handle_port_mod(crofctl *ctl, cofmsg_port_mod *msg) { delete msg; };



	/**
	 * @brief	Called once a FLOW-REMOVED.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the FLOW-REMOVED.message was received.
	 * @param msg pointer to cofmsg_flow_removed message containing the received message
	 */
	virtual void
	handle_flow_removed(crofdpt *dpt, cofmsg_flow_removed *msg) { delete msg; };



	/**
	 * @brief	Called once a PORT-STATUS.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the PORT-STATUS.message was received.
	 * @param msg pointer to cofmsg_port_status message containing the received message
	 */
	virtual void
	handle_port_status(crofdpt *dpt, cofmsg_port_status *msg) { delete msg; };




	/**
	 * @brief	Called once a QUEUE-GET-CONFIG.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the QUEUE-GET-CONFIG.request was received
	 * @param msg Pointer to cofmsg_queue_get_config_request message containing the received message
	 */
	virtual void
	handle_queue_get_config_request(crofctl *ctl, cofmsg_queue_get_config_request *msg) { delete msg; };



	/**
	 * @brief	Called once a QUEUE-GET-CONFIG.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the QUEUE-GET-CONFIG.reply message was received.
	 * @param msg pointer to cofmsg_queue_get_config_reply message containing the received message
	 */
	virtual void
	handle_queue_get_config_reply(crofdpt *dpt, cofmsg_queue_get_config_reply *msg) { delete msg; };



	/**
	 * @brief	Called once an EXPERIMENTER.message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the EXPERIMENTER.message was received
	 * @param msg Pointer to cofmsg_experimenter message containing the received message
	 */
	virtual void
	handle_set_config(crofctl *ctl, cofmsg_set_config *msg) { delete msg; };




	/**
	 * @brief	Called once an EXPERIMENTER.message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the EXPERIMENTER.message was received.
	 * @param msg pointer to cofmsg_experimenter message containing the received message
	 */
	virtual void
	handle_experimenter_message(crofdpt *dpt, cofmsg_experimenter *msg) { delete msg; };



	/**
	 * @brief	Called once an EXPERIMENTER.message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl pointer to cofctl instance from which the EXPERIMENTER.message was received.
	 * @param msg pointer to cofmsg_experimenter message containing the received message
	 */
	virtual void
	handle_experimenter_message(crofctl *ctl, cofmsg_experimenter *msg);



	/**
	 * @brief	Called once a timer has expired for an EXPERIMENTER.message.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of EXPERIMENTER.message previously sent to data path element.
	 */
	virtual void
	handle_get_fsp_reply_timeout(crofdpt *dpt) {};



	/**
	 * @brief	Called once a ROLE.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the ROLE.request was received
	 * @param msg Pointer to cofmsg_role_request message containing the received message
	 */
	virtual void
	handle_role_request(crofctl *ctl, cofmsg_role_request *msg) { delete msg; };



	/**
	 * @brief	Called once a ROLE.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the ROLE.reply message was received.
	 * @param msg pointer to cofmsg_role_reply message containing the received message
	 */
	virtual void
	handle_role_reply(crofdpt *dpt, cofmsg_role_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a timer has expired for a ROLE.reply.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of ROLE.reply message previously sent to data path element.
	 */
	virtual void
	handle_role_reply_timeout(crofdpt *dpt) {};


	/**
	 * @brief	Called once a GET-ASYNC-CONFIG.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the GET-ASYNC-CONFIG.request was received
	 * @param msg Pointer to cofmsg_get_async_config_request message containing the received message
	 */
	virtual void
	handle_get_async_config_request(crofctl *ctl, cofmsg_get_async_config_request *msg) { delete msg; };



	/**
	 * @brief	Called once a GET-ASYNC-CONFIG.reply message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt Pointer to cofdpt instance from which the GET-ASYNC-CONFIG.reply was received
	 * @param msg Pointer to cofmsg_get_async_config_reply message containing the received message
	 */
	virtual void
	handle_get_async_config_reply(crofdpt *dpt, cofmsg_get_async_config_reply *msg) { delete msg; };


	/**
	 * @brief	Called once a timer expires for a GET-ASYNC-CONFIG.reply message.
	 *
	 * Default behaviour: deletes cofdpt instance, thus effectively closing the control connection.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	handle_get_async_config_reply_timeout(crofdpt *dpt);


	/**
	 * @brief	Called once an SET-ASYNC-CONFIG.message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the SET-ASYNC-MESSAGE.message was received
	 * @param msg Pointer to cofmsg_set_async_config message containing the received message
	 */
	virtual void
	handle_set_async_config(crofctl *ctl, cofmsg_set_async_config *msg) { delete msg; };


	/**@}*/




	/**
	 * @name Event handlers overwritten from rofl::ciosrv
	 *
	 * When overwriting these methods for implementing own timers/events, please
	 * make sure to call crofbase::handle_timeout() or crofbase::handle_event()
	 * within the derived handler method.
	 */



	/**@{*/

	/**
	 * @brief	Handle timer events from rofl::ciosrv.
	 *
	 * Timers are used for sending notifications within a single thread.
	 *
	 * @param opaque expired timer type
	 */
	virtual void
	handle_timeout(
		int opaque);


	/**
	 * @brief 	Handle non-timer events from rofl::ciosrv
	 *
	 * Events are used for sending notifications among different threads.
	 *
	 * @param ev event instance
	 */
	virtual void
	handle_event(cevent const& ev);


	/**@}*/







private:

	friend class crofdptImpl;

	friend class crofctlImpl;

public:



	/**
	 * @name	Methods for sending OpenFlow messages
	 *
	 * These methods may be called by a derived class for sending
	 * a specific OF message.
	 */

	/**@{*/

#if 0
	/**
	 * @brief	Sends a BadRequest/BadLen ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_bad_len(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);


	/**
	 * @brief	Sends a BadRequest/BadVersion ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_bad_version(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends a BadRequest/BadType ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_bad_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends a BadRequest/BadStat ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_bad_stat(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends a BadRequest/BadExperimenter ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_bad_experimenter(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends a BadRequest/BadExpType ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_bad_exp_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);


	/**
	 * @brief	Sends a BadRequest/EPerm ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends a BadRequest/BufferEmpty ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_buffer_empty(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);


	/**
	 * @brief	Sends a BadRequest/BufferUnknown ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_buffer_unknown(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends a BadRequest/BadTableId ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_bad_table_id(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);




	/**
	 * @brief	Sends a BadRequest/IsSlave ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_is_slave(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends a BadRequest/BadPort ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_bad_port(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends a BadRequest/BadPacket ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_bad_packet(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);


	/**
	 * @brief	Sends a BadRequest/MultipartBufferOverflow ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_request_multipart_buffer_overflow(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);




	/**
	 * @brief	Sends a BadAction/BadType ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_bad_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends a BadAction/BadLen ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_bad_len(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends a BadAction/BadExperimenter ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_bad_experimenter(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends a BadAction/BadExperimenterType ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_bad_experimenter_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/BadOutPort ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_bad_out_port(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/BadArgument ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_bad_argument(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/EPerm ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/TooMany ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_too_many(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/BadQueue ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_bad_queue(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/BadOutGroup ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_bad_out_group(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/MatchInconsistent ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_match_inconsistent(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/UnsupportedOrder ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_unsupported_order(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/BadTag ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_bad_tag(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/SetLen ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_set_len(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/SetType ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_set_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadAction/SetArgument ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_action_set_argument(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadInst/UnknownInst ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_inst_unknown_inst(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadInst/UnsupInst ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_inst_unsup_inst(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadInst/BadTableId ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_inst_bad_table_id(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadInst/UnsupMetadata ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_inst_unsup_metadata(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadInst/UnsupMetadataMask ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_inst_unsup_metadata_mask(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadInst/UnsupExpInst / BadInst/BadExperimenter ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_inst_bad_experimenter(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadInst/BadExpType ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_inst_bad_exp_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadInst/BadLen ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_inst_bad_len(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadInst/EPerm ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_inst_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadMatch/BadType ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_bad_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadMatch/BadLen ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_bad_len(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadMatch/BadTag ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_bad_tag(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadMatch/BadDlAddrMask ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_bad_dladdr_mask(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadMatch/BadNwAddrMask ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_bad_nwaddr_mask(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadMatch/BadWildcards ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_bad_wildcards(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadMatch/BadField ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_bad_field(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadMatch/BadValue ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_bad_value(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadMatch/BadMask ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_bad_mask(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadMatch/BadPrereq ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_bad_prereq(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);


	/**
	 * @brief	Sends a BadMatch/DupField ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_dup_field(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a BadMatch/EPerm ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_bad_match_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a HelloFailed/Incompatible ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_hello_failed_incompatible(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a HelloFailed/EPerm ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_hello_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a SwitchConfigFailed/BadFlags ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_switch_config_failed_bad_flags(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a SwitchConfigFailed/BadLen ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_switch_config_failed_bad_len(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a SwitchConfigFailed/EPerm ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_switch_config_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a FlowModFailed/Unknown ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_flow_mod_failed_unknown(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a FlowModFailed/TableFull ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_flow_mod_failed_table_full(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a FlowModFailed/BadTableId ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_flow_mod_failed_bad_table_id(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a FlowModFailed/Overlap ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_flow_mod_failed_overlap(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a FlowModFailed/EPerm ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_flow_mod_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a FlowModFailed/BadTimeout ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_flow_mod_failed_bad_timeout(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a FlowModFailed/BadCommand ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_flow_mod_failed_bad_command(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a FlowModFailed/BadFlags ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_flow_mod_failed_bad_flags(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/GroupExists ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_group_exists(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/InvalGroup ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_inval_group(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/WeightUnsupported ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_weight_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/OutOfGroups ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_out_of_groups(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/OutOfBuckets ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_out_of_buckets(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/ChainingUnsupported ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_chaining_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/WatchUnsupported ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_watch_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/Loop ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_loop(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/UnknownGroup ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_unknown_group(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/ChainedGroup ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_chained_group(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/BadType ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_bad_type(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/BadCommand ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_bad_command(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/BadBucket ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_bad_bucket(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/BadWatch ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_bad_watch(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a GroupModFailed/EPerm ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_group_mod_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a PortModFailed/BadPort ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_port_mod_failed_bad_port(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a PortModFailed/BadHwAddr ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_port_mod_failed_bad_hw_addr(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a PortModFailed/BadConfig ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_port_mod_failed_bad_config(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a PortModFailed/BadAdvertise ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_port_mod_failed_bad_advertise(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a PortModFailed/EPerm ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_port_mod_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a TableModFailed/BadTable ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_table_mod_failed_bad_table(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a TableModFailed/BadConfig ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_table_mod_failed_bad_config(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a TableModFailed/EPerm ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_table_mod_failed_eperm(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a RoleRequestFailed/Stale ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_role_request_failed_stale(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a RoleRequestFailed/Unsupported ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_role_request_failed_unsupported(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/**
	 * @brief	Sends a RoleRequestFailed/BadRole ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_role_request_failed_bad_role(
		crofctl *ctl,
		uint32_t xid,
		uint8_t* data = NULL,
		size_t datalen = 0);
#endif

	/**@}*/



private:

	enum crofbase_flag_t {
		NSP_ENABLED = 0x01,
	};

	std::bitset<32> 					fe_flags;
	std::string 						info;			/**< info string */
	pthread_rwlock_t					xidlock;		/**< rwlock variable for transaction ids
															stored in ta_pending_reqs */

	/*
	 * data structures
	 */

	std::map<uint32_t, uint8_t> 		ta_pending_reqs; 	// list of pending requests
	std::set<uint32_t>	 				xids_used;			// list of recently used xids
	size_t 								xid_used_max; 		// reusing xids: max number of currently blocked xid entries stored
	uint32_t 							xid_start; 			// start value xid
#define CPCP_DEFAULT_XID_USED_MAX       16

	/** \enum crofbase::crofbase_event_t
	 *
	 * events defined by crofbase
	 */
	enum crofbase_event_t {
		CROFBASE_EVENT_WAKEUP	= 1, /**< wakeup event used in method \see{ wakeup } */
	};

	/** \enum crofbase::crofbase_timer_t
	 *
	 * timers defined by crofbase
	 */
	enum crofbase_timer_t {
		TIMER_FE_BASE = (0x0020 << 16),	/**< random number for base timer */
		CROFBASE_TIMER_WAKEUP,			/**< timer used for waking up via crofbase::wakeup() */
	};


	/** \enum crofbase::crofbase_rpc_t
	 *
	 * crofbase supports both controller and data path role and is
	 * capable of hosting an arbitrary number of listening sockets
	 * for ctl and dpt role.
	 *
	 * \see rpc
	 */
	enum crofbase_rpc_t { // for cofrpc *rpc[2]; (see below)
		RPC_CTL = 0,	/**< index for std::set<csocket*> in \see{ rpc } for ctls */
		RPC_DPT = 1,	/**< index for std::set<csocket*> in \see{ rpc } for dpts */
	};

	std::set<csocket*>			rpc[2];	/**< two sets of listening sockets for ctl and dpt */

	rofl::openflow::cofhello_elem_versionbitmap		versionbitmap;


private:

	friend class csocket;


	/** Helper method for handling DESCription STATS.requests.
	 * Only used within crofbase internally.
	 */
	void
	send_stats_reply_local();


	/*
	 * transaction ID related methods
	 */


	/** add pending request to transaction queue
	 * - allocates new xid not in xid_used
	 * - adds xid to xid_used
	 * - adds pair(type, xid) to ta_pending_requests
	 */
	uint32_t
	ta_add_request(
			uint8_t type);


	/** remove pending request from transaction queue
	 */
	void
	ta_rem_request(
			uint32_t xid);


	/** return boolean flag for pending request of type x
	 */
	bool
	ta_pending(
			uint32_t xid,
			uint8_t type);


	/** return new xid for asynchronous calls
	 * - adds xid to xid_used
	 * - does not add xid to ta_pending_requests
	 */
	uint32_t
	ta_new_async_xid();


	/** validate incoming reply for transaction
	 * checks for existing type and associated xid
	 * removes request from ta_pending_reqs, if found
	 */
	bool
	ta_validate(
			uint32_t xid,
			uint8_t type) throw (eRofBaseXidInval);


	/** validate a cofpacket, calls ta_validate(xid, type)
	 */
	bool
	ta_validate(
			cofmsg *pack);


	/** returns true if a xid is used by a pending
	 * transaction
	 */
	bool
	ta_active_xid(
			uint32_t xid);


	/*
	 * methods overwritten from csocket_owner
	 */


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




	/*
	 * methods to be called from cofdpt and cofctl
	 */


	/** for use by cofdpt
	 *
	 */
	void
	handle_dpt_open(crofdpt *dpt);

	/** for use by cofdpt
	 *
	 */
	void
	handle_dpt_close(crofdpt *dpt);

	/** for use by cofctl
	 *
	 */
	void
	handle_ctl_open(crofctl *ctl);

	/** for use by cofctl
	 *
	 */
	void
	handle_ctl_close(crofctl *ctl);

	/** get highest support OF protocol version
	 *
	 */
	uint8_t
	get_highest_supported_ofp_version();

	/** check whether a specific ofp version is supported
	 *
	 */
	bool
	is_ofp_version_supported(uint8_t ofp_version);

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofbase const& rofbase) {
		os << "<crofbase ";
		for (std::set<crofctl*>::const_iterator
				it = rofbase.ofctl_set.begin(); it != rofbase.ofctl_set.end(); ++it) {
			os << "    " << (*it) << std::endl;
		}
		for (std::set<crofdpt*>::const_iterator
				it = rofbase.ofdpt_set.begin(); it != rofbase.ofdpt_set.end(); ++it) {
			os << "    " << (*it) << std::endl;
		}
		os << ">";
		return os;
	};
};

}; // end of namespace

#endif


