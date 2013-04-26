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

#ifdef __cplusplus
extern "C" {
#endif
#include "openflow/openflow12.h"
#include "openflow/openflow_rofl.h"
#include <endian.h>
#include <string.h>
#include <time.h>

#ifndef htobe16
#include "endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "ciosrv.h"
#include "cerror.h"
#include "cvastring.h"
#include "croflexp.h"
#include "cphyport.h"
#include "csocket.h"
#include "thread_helper.h"

//#include "rofl/experimental/crib.h"
#include "rofl/platform/unix/crandom.h"

#include "openflow/cofport.h"
#include "openflow/cofdpt.h"
#include "openflow/cofctl.h"
#include "openflow/cofinst.h"
#include "openflow/cofinlist.h"
#include "openflow/cofaction.h"
#include "openflow/cofaclist.h"
#include "openflow/cofmatch.h"
#include "openflow/cflowentry.h"
#include "openflow/cgroupentry.h"
#include "openflow/cofstats.h"
#include "openflow/extensions/cfsptable.h"
#include "openflow/openflow_rofl_exceptions.h"
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
#include "openflow/messages/cofmsg_barrier.h"
#include "openflow/messages/cofmsg_queue_get_config.h"
#include "openflow/messages/cofmsg_role.h"
#include "openflow/messages/cofmsg_experimenter.h"

namespace rofl
{


/* error classes */
class eRofBase						: public cerror {};   // base error class crofbase
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




class cofctl;
class cofdpt;



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

	cfsptable 					fsptable; 		/**< flowspace registrations table */
	std::set<cofctl*>			ofctl_set;		/**< set of active controller connections */
	std::set<cofdpt*>			ofdpt_set;		/**< set of active data path connections */

public:

	//friend class cport;
	static std::set<crofbase*> 	rofbases; 		/**< set of all active crofbase instances */

public:


	/**
	 * @fn		crofbase
	 * @brief	Constructor for crofbase
	 *
	 * Initializes structures for transaction identifiers. xidlock is the rwlock
	 * for manipulating the transaction id maps. xid_start defines the first
	 * transaction id used by crofbase. xid_start is incremented by one for each new
	 * transaction.
	 *
	 * \see xidlock
	 * \see xid_start
	 */
	crofbase();


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
	 * @brief	returns a static c-string with information about this crofbase instance
	 *
	 * @result static c-string
	 */
	virtual const char*
	c_str();


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
	 * @param ra Address to connect to
	 * @param domain Socket domain (default: PF_INET)
	 * @param type Socket type (default: SOCK_STREAM)
	 * @param protocol Socket protocol (default: IPPROTO_TCP)
	 */
	void
	rpc_connect_to_ctl(
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
			cofctl *ctl);



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
	 * @param ra Address to connect to
	 * @param domain Socket domain (default: PF_INET)
	 * @param type Socket type (default: SOCK_STREAM)
	 * @param protocol Socket protocol (default: IPPROTO_TCP)
	 */
	void
	rpc_connect_to_dpt(
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
			cofdpt *dpath);


	/**
	 * @brief	Closes all open cofctl, cofdpt and listening socket instances.
	 *
	 */
	void
	rpc_close_all();


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
	cofdpt*
	dpt_find(
		uint64_t dpid) throw (eRofBaseNotFound);


	/**
	 * @brief 	returns pointer to cofdpt instance
	 *
	 * @param s_dpid data path identifier as std::string parameter
	 * @throws eRofBaseNotFound { thrown when cofdpt instance not found }
	 * @result pointer to cofdpt instance
	 */
	cofdpt*
	dpt_find(
		std::string s_dpid) throw (eRofBaseNotFound);


	/**
	 * @brief	returns pointer to cofdpt instance
	 *
	 * @param dl_dpid data path MAC address
	 * @throws eRofBaseNotFound { thrown when cofdpt instance not found }
	 * @result pointer to cofdpt instance
	 */
	cofdpt*
	dpt_find(
		cmacaddr dl_dpid) throw (eRofBaseNotFound);


	/**
	 * @brief 	returns pointer to cofdpt instance
	 *
	 * @param dpt pointer to cofdpt instance
	 * @throws eRofBaseNotFound { thrown when cofdpt instance not found }
	 * @result pointer to cofdpt instance
	 */
	cofdpt*
	dpt_find(
			cofdpt* dpt) throw (eRofBaseNotFound);



	/**
	 * @brief	returns pointer to cofctl instance
	 *
	 * @param ctl pointer to cofctl instance
	 * @throws eRofBaseNotFound { thrown when cofctl instance not found }
	 * @result pointer to cofctl instance
	 */
	cofctl*
	ctl_find(
			cofctl* ctl) throw (eRofBaseNotFound);


	/**@}*/








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
	 * @param newsd socket descriptor of new created socket for cofctl instance
	 * @param ra Remote address of peer entity connected via socket referenced by newsd
	 * @param domain socket domain (see man 2 socket for details)
	 * @param type socket type (see man 2 socket for details)
	 * @param protocol socket protocol (see man 2 socket for details)
	 *
	 */
	virtual cofctl*
	cofctl_factory(
			crofbase* owner,
			int newsd,
			caddress const& ra,
			int domain,
			int type,
			int protocol);


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
	 * @param ra Remote address to connect to
	 * @param domain socket domain (see man 2 socket for details)
	 * @param type socket type (see man 2 socket for details)
	 * @param protocol socket protocol (see man 2 socket for details)
	 *
	 */
	virtual cofctl*
	cofctl_factory(
			crofbase* owner,
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
	 * @param newsd socket descriptor of new created socket for cofdpt instance
	 * @param ra Remote address of peer entity connected via socket referenced by newsd
	 * @param domain socket domain (see man 2 socket for details)
	 * @param type socket type (see man 2 socket for details)
	 * @param protocol socket protocol (see man 2 socket for details)
	 *
	 */
	virtual cofdpt*
	cofdpt_factory(
			crofbase* owner,
			int newsd,
			caddress const& ra,
			int domain,
			int type,
			int protocol);


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
	 * @param ra Remote address to connect to
	 * @param domain socket domain (see man 2 socket for details)
	 * @param type socket type (see man 2 socket for details)
	 * @param protocol socket protocol (see man 2 socket for details)
	 *
	 */
	virtual cofdpt*
	cofdpt_factory(
			crofbase* owner,
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
	handle_dpath_open(cofdpt *dpt) {};


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
	handle_dpath_close(cofdpt *dpt) {};



	/**
	 * @brief	called once a new cofctl instance has been created
	 *
	 * This method is called, once a new controller entity has attached to this data path element.
	 * Should be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param ctl pointer to new cofctl instance
	 */
	virtual void
	handle_ctrl_open(cofctl *ctl) {};



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
	handle_ctrl_close(cofctl *ctl) {};


	/**@}*/




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
	handle_features_request(cofctl *ctl, cofmsg_features_request *msg) { delete msg; };



	/**
	 * @brief	Called once a FEATURES.reply message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt Pointer to cofdpt instance from which the FEATURES.reply was received
	 * @param msg Pointer to cofmsg_features_reply message containing the received message
	 */
	virtual void
	handle_features_reply(cofdpt *dpt, cofmsg_features_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a timer expires for a FEATURES.reply message.
	 *
	 * Default behaviour: deletes cofdpt instance, thus effectively closing the control connection.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	handle_features_reply_timeout(cofdpt *dpt);



	/**
	 * @brief	Called once a GET-CONFIG.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the GET-CONFIG.request was received
	 * @param msg Pointer to cofmsg_get_config_request message containing the received message
	 */
	virtual void
	handle_get_config_request(cofctl *ctl, cofmsg_get_config_request *msg) { delete msg; };



	/**
	 * @brief	Called once a GET-CONFIG.reply message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt Pointer to cofdpt instance from which the GET-CONFIG.reply was received
	 * @param msg Pointer to cofmsg_get_config_reply message containing the received message
	 */
	virtual void
	handle_get_config_reply(cofdpt *dpt, cofmsg_get_config_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a timer expires for a GET-CONFIG.reply message.
	 *
	 * Default behaviour: deletes cofdpt instance, thus effectively closing the control connection.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	handle_get_config_reply_timeout(cofdpt *dpt);



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
	handle_stats_request(cofctl *ctl, cofmsg_stats_request *msg) { throw eBadRequestBadStat(); };


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
	handle_desc_stats_request(cofctl *ctl, cofmsg_desc_stats_request *msg) { throw eBadRequestBadStat(); };



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
	handle_table_stats_request(cofctl *ctl, cofmsg_table_stats_request *msg) { throw eBadRequestBadStat(); };



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
	handle_port_stats_request(cofctl *ctl, cofmsg_port_stats_request *msg) { throw eBadRequestBadStat(); };



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
	handle_flow_stats_request(cofctl *ctl, cofmsg_flow_stats_request *msg) { throw eBadRequestBadStat(); };



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
	handle_aggregate_stats_request(cofctl *ctl, cofmsg_aggr_stats_request *msg) { throw eBadRequestBadStat(); };



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
	handle_queue_stats_request(cofctl *ctl, cofmsg_queue_stats_request *msg) { throw eBadRequestBadStat(); };



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
	handle_group_stats_request(cofctl *ctl, cofmsg_group_stats_request *msg) { throw eBadRequestBadStat(); };



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
	handle_group_desc_stats_request(cofctl *ctl, cofmsg_group_desc_stats_request *msg) { throw eBadRequestBadStat(); };



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
	handle_group_features_stats_request(cofctl *ctl, cofmsg_group_features_stats_request *msg) { throw eBadRequestBadStat(); };



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
	handle_experimenter_stats_request(cofctl *ctl, cofmsg_stats_request *msg) { throw eBadRequestBadStat(); };



	/**
	 * @brief	Called once a STATS.reply message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the STATS.reply message was received.
	 * @param msg pointer to cofmsg_stats_reply message containing the received message
	 */
	virtual void
	handle_stats_reply(cofdpt *dpt, cofmsg_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a timer has expired for a STATS.reply message.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of STATS.request previously sent to data path element.
	 */
	virtual void
	handle_stats_reply_timeout(cofdpt *dpt, uint32_t xid) {};



	/**
	 * @brief	Called once a DESC-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the DESC-STATS.reply message was received.
	 * @param msg pointer to cofmsg_desc_stats_reply message containing the received message
	 */
	virtual void
	handle_desc_stats_reply(cofdpt *dpt, cofmsg_desc_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a TABLE-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the TABLE-STATS.reply message was received.
	 * @param msg pointer to cofmsg_table_stats_reply message containing the received message
	 */
	virtual void
	handle_table_stats_reply(cofdpt *dpt, cofmsg_table_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a PORT-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the PORT-STATS.reply message was received.
	 * @param msg pointer to cofmsg_port_stats_reply message containing the received message
	 */
	virtual void
	handle_port_stats_reply(cofdpt *dpt, cofmsg_port_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a FLOW-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the FLOW-STATS.reply message was received.
	 * @param msg pointer to cofmsg_flow_stats_reply message containing the received message
	 */
	virtual void
	handle_flow_stats_reply(cofdpt *dpt, cofmsg_flow_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once an AGGREGATE-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the AGGREGATE-STATS.reply message was received.
	 * @param msg pointer to cofmsg_aggregate_stats_reply message containing the received message

	 */
	virtual void
	handle_aggregate_stats_reply(cofdpt *dpt, cofmsg_aggr_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a QUEUE-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the QUEUE-STATS.reply message was received.
	 * @param msg pointer to cofmsg_queue_stats_reply message containing the received message
	 */
	virtual void
	handle_queue_stats_reply(cofdpt *dpt, cofmsg_queue_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a GROUP-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the GROUP-STATS.reply message was received.
	 * @param msg pointer to cofmsg_group_stats_reply message containing the received message
	 */
	virtual void
	handle_group_stats_reply(cofdpt *dpt, cofmsg_group_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a GROUP-DESC-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the GROUP-DESC-STATS.reply message was received.
	 * @param msg pointer to cofmsg_group_desc_stats_reply message containing the received message
	 */
	virtual void
	handle_group_desc_stats_reply(cofdpt *dpt, cofmsg_group_desc_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a GROUP-FEATURES-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the GROUP-FEATURES-STATS.reply message was received.
	 * @param msg pointer to cofmsg_group_features_stats_reply message containing the received message
	 */
	virtual void
	handle_group_features_stats_reply(cofdpt *dpt, cofmsg_group_features_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once an EXPERIMENTER-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the EXPERIMENTER-STATS.reply message was received.
	 * @param msg pointer to cofmsg_experimenter_stats_reply message containing the received message
	 */
	virtual void
	handle_experimenter_stats_reply(cofdpt *dpt, cofmsg_stats_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a PACKET-OUT.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the PACKET-OUT.message was received
	 * @param msg Pointer to cofmsg_packet_out message containing the received message
	 */
	virtual void
	handle_packet_out(cofctl *ctl, cofmsg_packet_out *msg) { delete msg; };



	/**
	 * @brief	Called once a PACKET-IN.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the PACKET-IN.message was received.
	 * @param msg pointer to cofmsg_packet_in message containing the received message
	 */
	virtual void
	handle_packet_in(cofdpt *dpt, cofmsg_packet_in *msg) { delete msg; };



	/**
	 * @brief	Called once a BARRIER.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the BARRIER.request was received
	 * @param msg Pointer to cofmsg_barrier_request message containing the received message
	 */
	virtual void
	handle_barrier_request(cofctl *ctl, cofmsg_barrier_request *msg) { delete msg; };



	/**
	 * @brief	Called once a BARRIER.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the BARRIER.reply message was received.
	 * @param msg pointer to cofmsg_barrier_reply message containing the received message
	 */
	virtual void
	handle_barrier_reply(cofdpt *dpt, cofmsg_barrier_request *msg) { delete msg; };



	/**
	 * @brief	Called once a timer has expired for a BARRIER.reply message.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of BARRIER.request previously sent to data path element.
	 */
	virtual void
	handle_barrier_reply_timeout(cofdpt *dpt, uint32_t xid) {};



	/**
	 * @brief	Called once an ERROR.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the ERROR.message was received.
	 * @param msg pointer to cofmsg_error message containing the received message
	 */
	virtual void
	handle_error(cofdpt *dpt, cofmsg_error *msg) { delete msg; };



	/**
	 * @brief	Called once a FLOW-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the FLOW-MOD.message was received
	 * @param msg Pointer to cofmsg_flow_mod message containing the received message
	 */
	virtual void
	handle_flow_mod(cofctl *ctl, cofmsg_flow_mod *msg) { delete msg; };



	/**
	 * @brief	Called once a GROUP-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the GROUP-MOD.message was received
	 * @param msg Pointer to cofmsg_group_mod message containing the received message
	 */
	virtual void
	handle_group_mod(cofctl *ctl, cofmsg_group_mod *msg) { delete msg; };



	/**
	 * @brief	Called once a TABLE-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the TABLE-MOD.message was received
	 * @param msg Pointer to cofmsg_table_mod message containing the received message
	 */
	virtual void
	handle_table_mod(cofctl *ctl, cofmsg_table_mod *msg) { delete msg; };



	/**
	 * @brief	Called once a PORT-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the PORT-MOD.message was received
	 * @param msg Pointer to cofmsg_port_mod message containing the received message
	 */
	virtual void
	handle_port_mod(cofctl *ctl, cofmsg_port_mod *msg) { delete msg; };



	/**
	 * @brief	Called once a FLOW-REMOVED.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the FLOW-REMOVED.message was received.
	 * @param msg pointer to cofmsg_flow_removed message containing the received message
	 */
	virtual void
	handle_flow_removed(cofdpt *dpt, cofmsg_flow_removed *msg) { delete msg; };



	/**
	 * @brief	Called once a PORT-STATUS.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the PORT-STATUS.message was received.
	 * @param msg pointer to cofmsg_port_status message containing the received message
	 */
	virtual void
	handle_port_status(cofdpt *dpt, cofmsg_port_status *msg) { delete msg; };




	/**
	 * @brief	Called once a QUEUE-GET-CONFIG.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the QUEUE-GET-CONFIG.request was received
	 * @param msg Pointer to cofmsg_queue_get_config_request message containing the received message
	 */
	virtual void
	handle_queue_get_config_request(cofctl *ctl, cofmsg_queue_get_config_request *msg) { delete msg; };



	/**
	 * @brief	Called once a QUEUE-GET-CONFIG.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the QUEUE-GET-CONFIG.reply message was received.
	 * @param msg pointer to cofmsg_queue_get_config_reply message containing the received message
	 */
	virtual void
	handle_queue_get_config_reply(cofdpt *dpt, cofmsg_queue_get_config_reply *msg) { delete msg; };



	/**
	 * @brief	Called once an EXPERIMENTER.message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the EXPERIMENTER.message was received
	 * @param msg Pointer to cofmsg_experimenter message containing the received message
	 */
	virtual void
	handle_set_config(cofctl *ctl, cofmsg_set_config *msg) { delete msg; };




	/**
	 * @brief	Called once an EXPERIMENTER.message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the EXPERIMENTER.message was received.
	 * @param msg pointer to cofmsg_experimenter message containing the received message
	 */
	virtual void
	handle_experimenter_message(cofdpt *dpt, cofmsg_experimenter *msg) { delete msg; };



	/**
	 * @brief	Called once an EXPERIMENTER.message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl pointer to cofctl instance from which the EXPERIMENTER.message was received.
	 * @param msg pointer to cofmsg_experimenter message containing the received message
	 */
	virtual void
	handle_experimenter_message(cofctl *ctl, cofmsg_experimenter *msg);



	/**
	 * @brief	Called once a timer has expired for an EXPERIMENTER.message.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of EXPERIMENTER.message previously sent to data path element.
	 */
	virtual void
	handle_get_fsp_reply_timeout(cofdpt *dpt) {};



	/**
	 * @brief	Called once a ROLE.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the ROLE.request was received
	 * @param msg Pointer to cofmsg_role_request message containing the received message
	 */
	virtual void
	handle_role_request(cofctl *ctl, cofmsg_role_request *msg) { delete msg; };



	/**
	 * @brief	Called once a ROLE.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the ROLE.reply message was received.
	 * @param msg pointer to cofmsg_role_reply message containing the received message
	 */
	virtual void
	handle_role_reply(cofdpt *dpt, cofmsg_role_reply *msg) { delete msg; };



	/**
	 * @brief	Called once a timer has expired for a ROLE.reply.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of ROLE.reply message previously sent to data path element.
	 */
	virtual void
	handle_role_reply_timeout(cofdpt *dpt) {};

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

	friend class cofdpt;

	friend class cofctl;

	/**
	 * @name	Methods for sending OpenFlow messages used solely by cofctl and cofdpt instances
	 *
	 * These methods should only be used by cofctl and cfodpt instances.
	 */

	/**@{*/


	/**
	 * @brief	Sends HELLO.message to peer data path element.
	 *
	 * This method is used only by cofdpt instances and should never be called by classes derived from crofbase.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param body pointer to memory area containing opaque data. will be appended to HELLO message header
	 * @param bodylen size of memory area containing opaque data
	 */
	virtual void
	send_hello_message(
			cofdpt *dpt,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);



	/**
	 * @brief	Sends HELLO.message to peer controller entity.
	 *
	 * This method is used only by cofctl instances and should never be called by classes derived from crofbase.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param body pointer to memory area containing opaque data. will be appended to HELLO message header
	 * @param bodylen size of memory area containing opaque data
	 */
	virtual void
	send_hello_message(
			cofctl *ctl,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);




	/**
	 * @brief	Sends ECHO.request to peer data path element.
	 *
	 * This method is used only by cofdpt instances and should never be called by classes derived from crofbase.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param body pointer to memory area containing opaque data. will be appended to ECHO.request message header
	 * @param bodylen size of memory area containing opaque data
	 */
	virtual void
	send_echo_request(
			cofdpt *dpt,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);



	/**
	 * @brief	Sends ECHO.reply to peer data path element.
	 *
	 * This method is used only by cofdpt instances and should never be called by classes derived from crofbase.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param xid transaction ID of associated ECHO.request
	 * @param body pointer to memory area containing opaque data. will be appended to ECHO.reply message header
	 * @param bodylen size of memory area containing opaque data
	 */
	virtual void
	send_echo_reply(
			cofdpt *dpt,
			uint32_t xid,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);


	/**
	 * @brief	Sends ECHO.request to peer controller entity.
	 *
	 * This method is used only by cofctl instances and should never be called by classes derived from crofbase.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param body pointer to memory area containing opaque data. will be appended to ECHO.request message header
	 * @param bodylen size of memory area containing opaque data
	 */
	virtual void
	send_echo_request(
			cofctl *ctl,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);



	/**
	 * @brief	Sends ECHO.reply to peer controller entity.
	 *
	 * This method is used only by cofctl instances and should never be called by classes derived from crofbase.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of associated ECHO.request
	 * @param body pointer to memory area containing opaque data. will be appended to ECHO.reply message header
	 * @param bodylen size of memory area containing opaque data
	 */
	virtual void
	send_echo_reply(
			cofctl *ctl,
			uint32_t xid,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);

	/**@}*/



protected:



	/**
	 * @name	Methods for sending OpenFlow messages
	 *
	 * These methods may be called by a derived class for sending
	 * a specific OF message.
	 */

	/**@{*/



	/**
	 * @brief	Sends a FEATURES.request to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	send_features_request(
		cofdpt *dpt);



	/**
	 * @brief	Sends a FEATURES.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from FEATURES.request
	 * @param dpid data path ID of this data path element
	 * @param n_buffers number of packet buffers available
	 * @param capabilities data path capabilities (reassembly, etc.)
	 * @param of13_auxiliary_id auxiliary_id used in OpenFlow 1.3, ignored in other versions
	 * @param of10_actions_bitmap bitmap of supported actions for OpenFlow 1.0, ignored in other versions
	 * @param portlist list of cofports in this data path, ignored in OpenFlow 1.3
	 */
	virtual void
	send_features_reply(
			cofctl *ctl,
			uint32_t xid,
			uint64_t dpid,
			uint32_t n_buffers,
			uint8_t n_tables,
			uint32_t capabilities,
			uint8_t of13_auxiliary_id = 0,
			uint32_t of10_actions_bitmap = 0,
			cofportlist const& portlist = cofportlist());



	/**
	 * @brief	Sends a GET-CONFIG.request to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	send_get_config_request(
		cofdpt *dpt);



	/**
	 * @brief	Sends a GET-CONFIG.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from GET-CONFIG.request
	 * @param flags data path flags
	 * @param miss_send_len default miss_send_len value
	 */
	virtual void
	send_get_config_reply(
			cofctl *ctl,
			uint32_t xid,
			uint16_t flags,
			uint16_t miss_send_len);




	/**
	 * @brief	Sends a STATS.request to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param stats_type one of the OFPST_* constants
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param body body of STATS.request
	 * @param bodylen length of STATS.request body
	 * @return transaction ID for this STATS.request
	 */
	virtual uint32_t
	send_stats_request(
		cofdpt *dpt,
		uint16_t stats_type,
		uint16_t stats_flags,
		uint8_t *body = NULL,
		size_t bodylen = 0);



	/**
	 * @brief	Sends a FLOW-STATS.request to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param flow_stats_request body of a FLOW-STATS.request
	 * @return transaction ID for this FLOW-STATS.request
	 */
	virtual uint32_t
	send_flow_stats_request(
		cofdpt *dpt,
		uint16_t stats_flags,
		cofflow_stats_request const& flow_stats_request);



	/**
	 * @brief	Sends a AGGREGATE-STATS.request to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param aggr_stats_request body of an AGGREGATE-STATS.request
	 * @return transaction ID for this AGGREGATE-STATS.request
	 */
	virtual uint32_t
	send_aggr_stats_request(
		cofdpt *dpt,
		uint16_t flags,
		cofaggr_stats_request const& aggr_stats_request);



	/**
	 * @brief	Sends a STATS.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from received STATS.request
	 * @param stats_type one of the OFPST_* constants
	 * @param body body of a STATS.reply
	 * @param bodylen length of STATS.reply body
	 * @param more flag if multiple STATS.reply messages will be sent
	 */
	virtual void
	send_stats_reply(
		cofctl *ctl,
		uint32_t xid,
		uint16_t stats_type,
		uint8_t *body = NULL,
		size_t bodylen = 0,
		bool more = false);



	/**
	 * @brief	Sends a DESC-STATS.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from received STATS.request
	 * @param desc_stats body of DESC-STATS.reply
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_desc_stats_reply(
		cofctl *ctl,
		uint32_t xid,
		cofdesc_stats_reply const& desc_stats,
		bool more = false);



	/**
	 * @brief	Sends a TABLE-STATS.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from received STATS.request
	 * @param table_stats array of table_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_table_stats_reply(
		cofctl *ctl,
		uint32_t xid,
		std::vector<coftable_stats_reply> const& table_stats,
		bool more = false);



	/**
	 * @brief	Sends a PORT-STATS.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from received STATS.request
	 * @param port_stats array of port_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_port_stats_reply(
		cofctl *ctl,
		uint32_t xid,
		std::vector<cofport_stats_reply> const& port_stats,
		bool more = false);



	/**
	 * @brief	Sends a FLOW-STATS.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from received STATS.request
	 * @param flow_stats array of flow_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_flow_stats_reply(
		cofctl *ctl,
		uint32_t xid,
		std::vector<cofflow_stats_reply> const& flow_stats,
		bool more = false);



	/**
	 * @brief	Sends an AGGREGATE-STATS.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from received STATS.request
	 * @param aggr_stats aggr_stats body
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_aggr_stats_reply(
		cofctl *ctl,
		uint32_t xid,
		cofaggr_stats_reply const& aggr_stats,
		bool more = false);



	/**
	 * @brief	Sends a GROUP-STATS.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from received STATS.request
	 * @param group_stats array of group_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_group_stats_reply(
		cofctl *ctl,
		uint32_t xid,
		std::vector<cofgroup_stats_reply> const& group_stats,
		bool more = false);



	/**
	 * @brief	Sends a GROUP-DESC-STATS.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from received STATS.request
	 * @param group_desc_stats array of group_desc_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_group_desc_stats_reply(
		cofctl *ctl,
		uint32_t xid,
		std::vector<cofgroup_desc_stats_reply> const& group_desc_stats,
		bool more = false);



	/**
	 * @brief	Sends a GROUP-FEATURES-STATS.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from received STATS.request
	 * @param group_features_stats group_features_stats body
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_group_features_stats_reply(
		cofctl *ctl,
		uint32_t xid,
		cofgroup_features_stats_reply const& group_features_stats,
		bool more = false);



	/**
	 * @brief	Sends a PACKET-OUT.message to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param buffer_id buffer ID assigned by datapath (-1 if none) in host byte order
	 * @param in_port packet’s in-port (OFPP_NONE if none) in host byte order
	 * @param aclist OpenFlow ActionList
	 * @param data data packet to be sent out (optional)
	 * @param datalen length of data packet (optional)
	 */
	virtual void
	send_packet_out_message(
		cofdpt *dpt,
		uint32_t buffer_id,
		uint32_t in_port,
		cofaclist& aclist,
		uint8_t *data = NULL,
		size_t datalen = 0);




	/**
	 * @brief	Sends a PACKET-IN.message to a controller entity.
	 *
	 * @param ctl cofctl instance to send this message to. If NULL, send to all connected controllers.
	 * @param buffer_id buffer ID assigned by data path
	 * @param total_len Full length of frame
	 * @param reason reason packet is being sent (one of OFPR_* flags)
	 * @param table_id ID of table that generated the PACKET-IN event
	 * @param cookie cookie of FlowMod entry that generated the PACKET-IN event
	 * @param in_port port on which frame was received
	 * @param match match structure generated by data path element for data packet
	 * @param data data packet
	 * @param datalen length of data packet
	 */
	virtual void
	send_packet_in_message(
		cofctl *ctl,
		uint32_t buffer_id,
		uint16_t total_len,
		uint8_t reason,
		uint8_t table_id,
		uint64_t cookie,
		uint16_t in_port, // for OF1.0
		cofmatch &match,
		uint8_t *data,
		size_t datalen);



	/**
	 * @brief	Sends a BARRIER.request to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @result transaction ID assigned to this request
	 */
	virtual uint32_t
	send_barrier_request(
		cofdpt *dpt);



	/**
	 * @brief	Sends a BARRIER-reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from received BARRIER.request
	 */
	virtual void
	send_barrier_reply(
			cofctl *ctl,
			uint32_t xid);



	/**
	 * @brief	Sends an ERROR.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID of request that generated this error message
	 * @param type one of OpenFlow's OFPET_* flags
	 * @param code one of OpenFlow's error codes
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_message(
		cofctl *ctl,
		uint32_t xid,
		uint16_t type,
		uint16_t code,
		uint8_t* data = NULL,
		size_t datalen = 0);



	/**
	 * @brief	Sends an ERROR.message to a data path element.
	 *
	 * These messages are used for failed HELLO negotiations and
	 * experimental extensions.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param xid transaction ID of reply/notification that generated this error message
	 * @param type one of OpenFlow's OFPET_* flags
	 * @param code one of OpenFlow's error codes
	 * @param data first (at least 64) bytes of failed reply/notification
	 * @param datalen length of failed reply/notification appended to error message
	 */
	virtual void
	send_error_message(
		cofdpt *dpt,
		uint32_t xid,
		uint16_t type,
		uint16_t code,
		uint8_t* data = NULL,
		size_t datalen = 0);




	/*
	 * FIXME: to be removed and replaced by cflowentry
	 */

	/**
	 * @brief 	Sends a FLOW-MOD.message to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param match matching structure
	 * @param cookie opaque controller-issued identifier
	 * @param cookie_mask mask of bits to be overwritten/ignored when modifying FlowMod entries
	 * @param table_id destination table ID of new FlowMod entry
	 * @param command one of OpenFlow's command values: Add/Modify/Remove entry
	 * @param idle_timeout idle timeout before discarding this FlowMod entry in seconds
	 * @param hard_timeout hard timeout before discarding this FlowMod entry in seconds
	 * @param priority priority level of flow entry
	 * @param buffer_id buffer slot where to apply this FlowMod entry
	 * @param out_port for OFPFC_DELETE* commands, require matching entries to include this as an output port. A value of OFPP_NONE indicates no restriction
	 * @param flags one of OpenFlow's OFPFF_* flags
	 * @param inlist InstructionList
	 */
	virtual void
	send_flow_mod_message(
		cofdpt *dpt,
		cofmatch& match,
		uint64_t cookie,
		uint64_t cookie_mask,
		uint8_t table_id,
		uint8_t command,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint16_t priority,
		uint32_t buffer_id,
		uint32_t out_port,
		uint32_t out_group,
		uint16_t flags,
		cofinlist& inlist);



	/**
	 * @brief 	Sends a FLOW-MOD.message to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param flowentry FlowMod entry
	 */
	virtual void
	send_flow_mod_message(
			cofdpt *dpt,
			cflowentry& flowentry);




	/**
	 * @brief 	Sends a GROUP-MOD.message to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param groupentry GroupMod entry
	 */
	virtual void
	send_group_mod_message(
			cofdpt *dpt,
			cgroupentry& groupentry);



	/**
	 * @brief	Sends a TABLE-MOD.message to a data path element.
	 *
	 * @param dpt pointer to a cofdpt instance
	 * @param table_id ID of table to be reconfigured
	 * @param config new configuration for table
	 */
	virtual void
	send_table_mod_message(
		cofdpt *dpt,
		uint8_t table_id,
		uint32_t config);




	/**
	 * @brief	Sends a PORT-MOD.message to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param port_no number of port to be updated
	 * @param hwaddr MAC address assigned to port
	 * @param config OpenFlow config parameter
	 * * @param mask OpenFlow mask parameter
	 * * @param advertise OpenFlow advertise parameter
	 */
	virtual void
	send_port_mod_message(
		cofdpt *dpt,
		uint32_t port_no,
		cmacaddr const& hwaddr,
		uint32_t config,
		uint32_t mask,
		uint32_t advertise);



	/**
	 * @brief	Sends a FLOW-REMOVED.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param match match structure defined in FlowMod entry
	 * @param cookie cookie defined in FlowMod entry
	 * @param priority priority level defined in FlowMOd entry
	 * @param reason one of OpenFlow's OFPRR_* constants
	 * @param table_id ID of table from which the FlowMod entry was removed
	 * @param duration_sec time flow was alive in seconds
	 * @param duration_nsec time flow was alive in nanoseconds beyond duration_sec
	 * @param idle_timeout idle timeout from original flow mod
	 * @param idle_timeout hard timeout from original flow mod
	 * @param packet_count number of packets handled by this flow mod
	 * @param byte_count number of bytes handled by this flow mod
	 */
	virtual void
	send_flow_removed_message(
		cofctl *ctl,
		cofmatch& match,
		uint64_t cookie,
		uint16_t priority,
		uint8_t reason,
		uint8_t table_id,
		uint32_t duration_sec,
		uint32_t duration_nsec,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint64_t packet_count,
		uint64_t byte_count);




	/**
	 * @brief	Sends a PORT-STATUS.message to a controller entity.
	 *
	 * @param ctl cofctl instance to send this message to. If NULL, send to all connected controllers.
	 * @param reason one of OpenFlow's OFPPR_* constants
	 * @param port cofport instance that changed its status
	 */
	virtual void
	send_port_status_message(
		cofctl *ctl,
		uint8_t reason,
		cofport const& port);



	/**
	 * @brief	Sends a SET-CONFIG.message to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param flags field of OpenFlow's OFPC_* flags
	 * @param miss_send_len OpenFlow' miss_send_len parameter
	 */
	virtual void
	send_set_config_message(
		cofdpt *dpt,
		uint16_t flags,
		uint16_t miss_send_len);



	/**
	 * @brief	Sends a QUEUE-GET-CONFIG.request to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param port port to be queried. Should refer to a valid physical port (i.e. < OFPP_MAX)
	 */
	virtual void
	send_queue_get_config_request(
		cofdpt *sw,
		uint32_t port);



	// FIXME: add QUEUE-GET-CONFIG.reply body
	/**
	 * @brief	Sends a QUEUE-GET-CONFIG.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from QUEUE-GET-CONFIG.request
	 * @param portno OpenFlow number assigned to port
	 */
	virtual void
	send_queue_get_config_reply(
			cofctl *ctl,
			uint32_t xid,
			uint32_t portno);




	/**
	 * @brief 	Sends an EXPERIMENTER.message to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param experimenter_id exp_id as assigned by ONF
	 * @param exp_type exp_type as defined by the ONF member
	 * @param body pointer to opaque experimenter message body (optional)
	 * @param bodylen length of body (optional)
	 */
	virtual void
	send_experimenter_message(
			cofdpt *dpt,
			uint32_t experimenter_id,
			uint32_t exp_type,
			uint8_t *body = NULL,
			size_t bodylen = 0);



	/**
	 * @brief 	Sends an EXPERIMENTER.message to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param experimenter_id exp_id as assigned by ONF
	 * @param exp_type exp_type as defined by the ONF member
	 * @param body pointer to opaque experimenter message body (optional)
	 * @param bodylen length of body (optional)
	 */
	virtual void
	send_experimenter_message(
			cofctl *ctl,
			uint32_t experimenter_id,
			uint32_t exp_type,
			uint8_t *body = NULL,
			size_t bodylen = 0);




	/**
	 * @brief	Sends a ROLE.request to a data path element.
	 *
	 * @param dpt pointer to cofdpt instance
	 * @param role role as defined by OpenFlow
	 * @param generation_id gen_id as defined by OpenFlow
	 */
	virtual void
	send_role_request(
		cofdpt *dpt,
		uint32_t role,
		uint64_t generation_id);



	/**
	 * @brief	Sends a ROLE.reply to a controller entity.
	 *
	 * @param ctl pointer to cofctl instance
	 * @param xid transaction ID from associated ROLE.request
	 * @param role defined role from data path
	 * @param generation_id gen_id as defined by OpenFlow
	 */
	virtual void
	send_role_reply(
			cofctl *ctl,
			uint32_t xid,
			uint32_t role,
			uint64_t generation_id);


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
		TIMER_FE_DUMP_OFPACKETS,		/**< dumps periodically all existing cofmsg instances */
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
	handle_dpt_open(cofdpt *dpt);

	/** for use by cofdpt
	 *
	 */
	void
	handle_dpt_close(cofdpt *dpt);

	/** for use by cofctl
	 *
	 */
	void
	handle_ctl_open(cofctl *ctl);

	/** for use by cofctl
	 *
	 */
	void
	handle_ctl_close(cofctl *ctl);




};

}; // end of namespace

#endif


