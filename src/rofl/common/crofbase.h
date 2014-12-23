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

#include "rofl/common/ciosrv.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/csocket.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/logging.h"
#include "rofl/common/crofdpt.h"
#include "rofl/common/crofctl.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/crandom.h"

namespace rofl {


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
class eRofBaseCongested				: public eRofBase {}; // control channel is congested, dropping messages



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
	public crofconn_env,
	public ctransactions_env,
	public crofctl_env,
	public crofdpt_env
{
	/**
	 * @enum crofbase::crofbase_rpc_t
	 *
	 * crofbase supports both controller and data path role and is
	 * capable of hosting an arbitrary number of listening sockets
	 * for ctl and dpt role.
	 */
	enum crofbase_rpc_t {
		RPC_CTL 				= 0,
		RPC_DPT 				= 1,
	};

	/**
	 * @enum rofl::crofbase::crofbase_event_t
	 *
	 * event types defined by crofbase for feeding the event-queue
	 */
	enum crofbase_event_t {
		EVENT_NONE				= 0,
		EVENT_CTL_DETACHED		= 1,
		EVENT_DPT_DETACHED		= 2,
	};

	/**
	 * @enum rofl::crofbase::crofbase_timer_t
	 *
	 * timer types defined by crofbase
	 */
	enum crofbase_timer_t {
		TIMER_NONE				= 0,
		TIMER_RUN_ENGINE		= 1,
	};

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
	crofbase(
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap =
					rofl::openflow::cofhello_elem_versionbitmap());

	/**
	 * @fn		~crofbase
	 * @brief	Destructor for crofbase
	 *
	 * The destructor shuts down all active connections and listening sockets.
	 *
	 */
	virtual
	~crofbase();

public:

	/**
	 * @name	RPC related methods for opening/closing TCP connections and listening sockets
	 */

	/**@{*/

	/**
	 * @fn		rpc_listen_for_dpts
	 * @brief	Opens a listening socket for accepting connection requests from data path elements
	 *
	 * @param socket_type one of the constants defined in csocket.h, e.g. SOCKET_TYPE_PLAIN
	 * @param params set of parameters used for creating a listening socket
	 */
	virtual void
	rpc_listen_for_dpts(
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& params);

	/**
	 * @fn		rpc_listen_for_ctls
	 * @brief	Opens a listening socket for accepting connection requests from controllers
	 *
	 * @param socket_type one of the constants defined in csocket.h, e.g. SOCKET_TYPE_PLAIN
	 * @param params set of parameters used for creating a listening socket
	 */
	virtual void
	rpc_listen_for_ctls(
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& params);

	/**
	 * @brief	Closes all open cofctl, cofdpt and listening socket instances.
	 *
	 */
	virtual void
	rpc_close_all();

	/**@}*/

public:

	/**
	 * @name	Session related methods for data path elements (cofdpt) and controllers (cofctl)
	 */

	/**@{*/

	/**
	 *
	 */
	rofl::cdptid
	get_idle_dptid() const {
		uint64_t id = 0;
		while (has_dpt(rofl::cdptid(id))) {
			id++;
		}
		return rofl::cdptid(id);
	};

	/**
	 *
	 */
	void
	drop_dpts() {
		for (std::map<rofl::cdptid, crofdpt*>::iterator
				it = rofdpts.begin(); it != rofdpts.end(); ++it) {
			delete it->second;
		}
		rofdpts.clear();
	};

	/**
	 * @brief	returns reference to crofdpt instance
	 *
	 * @param dptid data path identifier as uint64_t parameter
	 * @throws eRofBaseNotFound { thrown when cofdpt instance not found }
	 * @result reference to crofdpt instance
	 */
	crofdpt&
	add_dpt(
		const cdptid& dptid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false) {
		if (rofdpts.find(dptid) != rofdpts.end()) {
			delete rofdpts[dptid];
			rofdpts.erase(dptid);
		}
		rofdpts[dptid] = new crofdpt(this, dptid, remove_on_channel_close, versionbitmap);
		return *(rofdpts[dptid]);
	};

	/**
	 *
	 */
	crofdpt&
	set_dpt(
		const cdptid& dptid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false) {
		if (rofdpts.find(dptid) == rofdpts.end()) {
			rofdpts[dptid] = new crofdpt(this, dptid, remove_on_channel_close, versionbitmap);
		}
		return *(rofdpts[dptid]);
	};

	/**
	 *
	 */
	crofdpt&
	set_dpt(
			const cdptid& dptid) {
		if (rofdpts.find(dptid) == rofdpts.end()) {
			throw eRofBaseNotFound();
		}
		return *(rofdpts[dptid]);
	};

	/**
	 *
	 */
	const crofdpt&
	get_dpt(
			const cdptid& dptid) const {
		if (rofdpts.find(dptid) == rofdpts.end()) {
			throw eRofBaseNotFound();
		}
		return *(rofdpts.at(dptid));
	};

	/**
	 * @brief	returns reference to crofdpt instance
	 *
	 * @param dptid data path identifier as uint64_t parameter
	 * @throws eRofBaseNotFound { thrown when cofdpt instance not found }
	 * @result reference to crofdpt instance
	 */
	void
	drop_dpt(
		cdptid dptid) { // make a copy here, do not use a const reference
		if (rofdpts.find(dptid) == rofdpts.end()) {
			return;
		}
		delete rofdpts[dptid];
		rofdpts.erase(dptid);
	};

	/**
	 * @brief	returns reference to crofdpt instance
	 *
	 * @param dptid data path identifier as uint64_t parameter
	 * @throws eRofBaseNotFound { thrown when cofdpt instance not found }
	 * @result reference to crofdpt instance
	 */
	bool
	has_dpt(
		const cdptid& dptid) const {
		return (not (rofdpts.find(dptid) == rofdpts.end()));
	};

public:

	/**
	 *
	 */
	rofl::cctlid
	get_idle_ctlid() const {
		uint64_t id = 0;
		while (has_ctl(rofl::cctlid(id))) {
			id++;
		}
		return rofl::cctlid(id);
	};

	/**
	 *
	 */
	void
	drop_ctls() {
		for (std::map<rofl::cctlid, crofctl*>::iterator
				it = rofctls.begin(); it != rofctls.end(); ++it) {
			delete it->second;
		}
		rofctls.clear();
	};

	/**
	 * @brief	returns reference to crofctl instance
	 *
	 * @param ctlid data path identifier as uint64_t parameter
	 * @throws eRofBaseNotFound { thrown when cofctl instance not found }
	 * @result reference to crofctl instance
	 */
	crofctl&
	add_ctl(
		const cctlid& ctlid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false) {
		if (rofctls.find(ctlid) != rofctls.end()) {
			delete rofctls[ctlid];
			rofctls.erase(ctlid);
		}
		rofctls[ctlid] = new crofctl(this, ctlid, remove_on_channel_close, versionbitmap);
		return *(rofctls[ctlid]);
	};

	/**
	 *
	 */
	crofctl&
	set_ctl(
		const cctlid& ctlid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false) {
		if (rofctls.find(ctlid) == rofctls.end()) {
			rofctls[ctlid] = new crofctl(this, ctlid, remove_on_channel_close, versionbitmap);
		}
		return *(rofctls[ctlid]);
	};

	/**
	 *
	 */
	crofctl&
	set_ctl(
			const cctlid& ctlid) {
		if (rofctls.find(ctlid) == rofctls.end()) {
			throw eRofBaseNotFound();
		}
		return *(rofctls[ctlid]);
	};

	/**
	 *
	 */
	const crofctl&
	get_ctl(
			const cctlid& ctlid) const {
		if (rofctls.find(ctlid) == rofctls.end()) {
			throw eRofBaseNotFound();
		}
		return *(rofctls.at(ctlid));
	};

	/**
	 * @brief	returns reference to crofctl instance
	 *
	 * @param ctlid data path identifier as uint64_t parameter
	 * @throws eRofBaseNotFound { thrown when cofctl instance not found }
	 * @result reference to crofctl instance
	 */
	void
	drop_ctl(
		cctlid ctlid) { // make a copy here, do not use a const reference
		if (rofctls.find(ctlid) == rofctls.end()) {
			return;
		}
		delete rofctls[ctlid];
		rofctls.erase(ctlid);
	};

	/**
	 * @brief	returns reference to crofctl instance
	 *
	 * @param ctlid data path identifier as uint64_t parameter
	 * @throws eRofBaseNotFound { thrown when cofctl instance not found }
	 * @result reference to crofctl instance
	 */
	bool
	has_ctl(
		const cctlid& ctlid) const {
		return (not (rofctls.find(ctlid) == rofctls.end()));
	};

	/**@}*/

public:

	/**
	 *
	 */
	static uint32_t
	get_ofp_no_buffer(
			uint8_t ofp_version);

	/**
	 *
	 */
	static uint32_t
	get_ofp_controller_port(
			uint8_t ofp_version);

	/**
	 *
	 */
	static uint32_t
	get_ofp_flood_port(
			uint8_t ofp_version);

	/**
	 *
	 */
	static uint8_t
	get_ofp_command(
			uint8_t ofp_version,
			const enum openflow::ofp_flow_mod_command& cmd);

	/**
	 * @brief return versionbitmap defined for this crofbase instance
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap()
	{ return versionbitmap; };

	/**
	 * @brief get highest support OF protocol version
	 */
	uint8_t
	get_highest_supported_ofp_version() const
	{ return versionbitmap.get_highest_ofp_version(); };

	/**
	 * @brief check whether a specific ofp version is supported
	 */
	bool
	is_ofp_version_supported(
			uint8_t ofp_version) const
	{ return versionbitmap.has_ofp_version(ofp_version); };

protected:

	/**
	 * @name Methods for managing cofctl and cofdpt instances
	 */

	/**@{*/

public:

	/**
	 * @brief	called once a new cofdpt instance has been created
	 *
	 * This method is called, once a new datapath element has attached to this controller.
	 * Should be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to new cofdpt instance
	 */
	virtual void
	handle_dpt_open(rofl::crofdpt& dpt)
	{};

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
	handle_dpt_close(rofl::crofdpt& dpt)
	{};

	/**
	 * @brief Called upon establishment of a control connection within the control channel
	 */
	virtual void
	handle_conn_established(
			crofdpt& dpt,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] "
				<< "connection established, auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief Called upon a peer initiated termination of a control connection within the control channel
	 */
	virtual void
	handle_conn_terminated(
			crofdpt& dpt,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] "
				<< "connection terminated, auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief Called in the event of a connection refused
	 */
	virtual void
	handle_conn_refused(
			crofdpt& dpt,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] "
				<< "connection refused, auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief Called in the event of a connection failed (except refused)
	 */
	virtual void
	handle_conn_failed(
			crofdpt& dpt,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] "
				<< "connection failed, auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief	called once a new cofctl instance has been created
	 *
	 * This method is called, once a new controller entity has attached to this data path element.
	 * Should be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param ctl pointer to new cofctl instance
	 */
	virtual void
	handle_ctl_open(crofctl& ctl)
	{};

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
	handle_ctl_close(crofctl& ctl)
	{};

	/**
	 * @brief Called upon establishment of a control connection within the control channel
	 */
	virtual void
	handle_conn_established(
			crofctl& ctl,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] "
				<< "connection established, auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief Called upon a peer initiated termination of a control connection within the control channel
	 */
	virtual void
	handle_conn_terminated(
			crofctl& ctl,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] "
				<< "connection terminated, auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief Called in the event of a connection refused
	 */
	virtual void
	handle_conn_refused(
			crofctl& ctl,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] "
				<< "connection refused, auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief Called in the event of a connection failed (except refused)
	 */
	virtual void
	handle_conn_failed(
			crofctl& ctl,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] "
				<< "connection failed, auxid: " << auxid.str() << std::endl;
	};

	/**@}*/

	/**
	 * @brief	called once a cofctl instance has received a role request
	 *
	 *
	 */
	virtual void
	role_request_rcvd(crofctl& ctl, uint32_t role, uint64_t rcvd_generation_id);

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
	 *
	 * @param ctl
	 * @param auxid
	 */
	virtual void
	handle_write(rofl::crofctl& ctl, const rofl::cauxid& auxid) {};

	/**
	 *
	 * @param ctl
	 * @param auxid
	 */
	virtual void
	handle_write(rofl::crofdpt& dpt, const rofl::cauxid& auxid) {};

	/**
	 * @brief	Called once a FEATURES.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the FEATURES.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_features_request message containing the received message
	 */
	virtual void
	handle_features_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_features_request& msg) {};

	/**
	 * @brief	Called once a FEATURES.reply message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt Pointer to cofdpt instance from which the FEATURES.reply was received
	 * @param msg Pointer to rofl::openflow::cofmsg_features_reply message containing the received message
	 */
	virtual void
	handle_features_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_features_reply& msg) {};

	/**
	 * @brief	Called once a timer expires for a FEATURES.reply message.
	 *
	 * Default behaviour: deletes cofdpt instance, thus effectively closing the control connection.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	handle_features_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a GET-CONFIG.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the GET-CONFIG.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_get_config_request message containing the received message
	 */
	virtual void
	handle_get_config_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_get_config_request& msg) {};

	/**
	 * @brief	Called once a GET-CONFIG.reply message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt Pointer to cofdpt instance from which the GET-CONFIG.reply was received
	 * @param msg Pointer to rofl::openflow::cofmsg_get_config_reply message containing the received message
	 */
	virtual void
	handle_get_config_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_get_config_reply& msg) {};

	/**
	 * @brief	Called once a timer expires for a GET-CONFIG.reply message.
	 *
	 * Default behaviour: deletes cofdpt instance, thus effectively closing the control connection.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	handle_get_config_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

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
	 * @param msg Pointer to rofl::openflow::cofmsg_get_config_request message containing the received message
	 * @exception eBadRequestBadStat { sends a proper error message to the controller entity }
	 */
	void
	handle_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a STATS.reply message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_stats_reply message containing the received message
	 */
	virtual void
	handle_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_stats_reply& msg) {};

	/**
	 * @brief	Called once a timer has expired for a STATS.reply message.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of STATS.request previously sent to data path element.
	 */
	virtual void
	handle_multipart_reply_timeout(rofl::crofdpt& dpt, uint32_t xid, uint8_t msg_sub_type) {};

	/**
	 * @brief	Called once a DESC-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the DESC-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_desc_stats_request message containing the received message
	 */
	virtual void
	handle_desc_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_desc_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a DESC-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the DESC-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_desc_stats_reply message containing the received message
	 */
	virtual void
	handle_desc_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_desc_stats_reply& msg) {};

	/**
	 * @brief	Called once a DESC-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_desc_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a TABLE-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the TABLE-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_table_stats_request message containing the received message
	 */
	virtual void
	handle_table_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_table_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a TABLE-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the TABLE-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_table_stats_reply message containing the received message
	 */
	virtual void
	handle_table_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_table_stats_reply& msg) {};

	/**
	 * @brief	Called once a TABLE-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_table_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a PORT-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the PORT-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_port_stats_request message containing the received message
	 */
	virtual void
	handle_port_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_port_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a PORT-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the PORT-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_port_stats_reply message containing the received message
	 */
	virtual void
	handle_port_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_port_stats_reply& msg) {};

	/**
	 * @brief	Called once a PORT-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_port_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a FLOW-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the FLOW-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_flow_stats_request message containing the received message
	 */
	virtual void
	handle_flow_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_flow_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a FLOW-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the FLOW-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_flow_stats_reply message containing the received message
	 */
	virtual void
	handle_flow_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_flow_stats_reply& msg) {};

	/**
	 * @brief	Called once a FLOW-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_flow_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once an AGGREGATE-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the AGGREGATE-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_aggr_stats_request message containing the received message
	 */
	virtual void
	handle_aggregate_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_aggr_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once an AGGREGATE-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the AGGREGATE-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_aggregate_stats_reply message containing the received message

	 */
	virtual void
	handle_aggregate_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_aggr_stats_reply& msg) {};

	/**
	 * @brief	Called once a AGGREGATE-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_aggregate_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a QUEUE-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the QUEUE-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_queue_stats_request message containing the received message
	 */
	virtual void
	handle_queue_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_queue_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a QUEUE-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the QUEUE-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_queue_stats_reply message containing the received message
	 */
	virtual void
	handle_queue_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_queue_stats_reply& msg) {};

	/**
	 * @brief	Called once a QUEUE-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_queue_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a GROUP-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the GROUP-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_group_stats_request message containing the received message
	 */
	virtual void
	handle_group_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_group_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a GROUP-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the GROUP-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_group_stats_reply message containing the received message
	 */
	virtual void
	handle_group_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_group_stats_reply& msg) {};

	/**
	 * @brief	Called once a GROUP-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_group_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a GROUP-DESC-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the GROUP-DESC-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_group_desc_stats_request message containing the received message
	 */
	virtual void
	handle_group_desc_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_group_desc_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a GROUP-DESC-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the GROUP-DESC-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_group_desc_stats_reply message containing the received message
	 */
	virtual void
	handle_group_desc_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_group_desc_stats_reply& msg) {};

	/**
	 * @brief	Called once a GROUP-DESC-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_group_desc_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a GROUP-FEATURES-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the GROUP-FEATURES-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_group_features_stats_request message containing the received message
	 */
	virtual void
	handle_group_features_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_group_features_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a GROUP-FEATURES-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the GROUP-FEATURES-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_group_features_stats_reply message containing the received message
	 */
	virtual void
	handle_group_features_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_group_features_stats_reply& msg) {};

	/**
	 * @brief	Called once a GROUP-FEATURES-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_group_features_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a METER-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the METER-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_meter_stats_request message containing the received message
	 */
	virtual void
	handle_meter_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_meter_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a METER-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the METER-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_meter_stats_reply message containing the received message
	 */
	virtual void
	handle_meter_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_meter_stats_reply& msg) {};

	/**
	 * @brief	Called once a METER-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_meter_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a METER-CONFIG-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the METER-CONFIG-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_meter_config_stats_request message containing the received message
	 */
	virtual void
	handle_meter_config_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_meter_config_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a METER-CONFIG-STATS.reply message was received.
	 *
	 * To be overwritten by derived class.
	 *
	 * @param dpt pointer to cofdpt instance from which the METER-CONFIG-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_meter_config_stats_reply message containing the received message
	 */
	virtual void
	handle_meter_config_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_meter_config_stats_reply& msg) {};

	/**
	 * @brief	Called once a METER-CONFIG-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_meter_config_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a METER-FEATURES-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the METER-FEATURES-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_meter_features_stats_request message containing the received message
	 */
	virtual void
	handle_meter_features_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_meter_features_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a METER-FEATURES-STATS.reply message was received.
	 *
	 * To be overwritten by derived class.
	 *
	 * @param dpt pointer to cofdpt instance from which the METER-FEATURES-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_meter_features_stats_reply message containing the received message
	 */
	virtual void
	handle_meter_features_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_meter_features_stats_reply& msg) {};

	/**
	 * @brief	Called once a METER-FEATURES-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_meter_features_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a TABLE-FEATURES-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the TABLE-FEATURES-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_table_features_request message containing the received message
	 */
	virtual void
	handle_table_features_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_table_features_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a TABLE-FEATURES-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the TABLE-FEATURES-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_table_features_reply message containing the received message
	 */
	virtual void
	handle_table_features_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_table_features_stats_reply& msg) {};

	/**
	 * @brief	Called once a TABLE-FEATURES-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_table_features_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a PORT-DESC-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the PORT-DESC-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_port_desc_stats_request message containing the received message
	 */
	virtual void
	handle_port_desc_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_port_desc_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once a PORT-DESC-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the PORT-DESC-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_port_desc_stats_reply message containing the received message
	 */
	virtual void
	handle_port_desc_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_port_desc_stats_reply& msg) {};

	/**
	 * @brief	Called once a PORT-DESC-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_port_desc_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a EXPERIMENTER-STATS.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: throws eBadRequestBadStat resulting in removal
	 * of msg from heap and generation of proper error message sent to controller entity.
	 *
	 * @param ctl Pointer to cofctl instance from which the EXPERIMENTER-STATS.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_experimenter_stats_request message containing the received message
	 */
	virtual void
	handle_experimenter_stats_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_experimenter_stats_request& msg) { throw eBadRequestBadStat(); };

	/**
	 * @brief	Called once an EXPERIMENTER-STATS.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the EXPERIMENTER-STATS.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_experimenter_stats_reply message containing the received message
	 */
	virtual void
	handle_experimenter_stats_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_experimenter_stats_reply& msg) {};

	/**
	 * @brief	Called once an EXPERIMENTER-STATS.request expires.
	 *
	 * @param dpt data path handle
	 * @param xid transaction id of associated request
	 */
	virtual void
	handle_experimenter_stats_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a PACKET-OUT.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the PACKET-OUT.message was received
	 * @param msg Pointer to rofl::openflow::cofmsg_packet_out message containing the received message
	 */
	virtual void
	handle_packet_out(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_packet_out& msg) {};

	/**
	 * @brief	Called once a PACKET-IN.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the PACKET-IN.message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_packet_in message containing the received message
	 */
	virtual void
	handle_packet_in(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_packet_in& msg) {};

	/**
	 * @brief	Called once a BARRIER.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the BARRIER.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_barrier_request message containing the received message
	 */
	virtual void
	handle_barrier_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_barrier_request& msg) {};

	/**
	 * @brief	Called once a BARRIER.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the BARRIER.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_barrier_reply message containing the received message
	 */
	virtual void
	handle_barrier_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_barrier_reply& msg) {};

	/**
	 * @brief	Called once a timer has expired for a BARRIER.reply message.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of BARRIER.request previously sent to data path element.
	 */
	virtual void
	handle_barrier_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a FLOW-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the FLOW-MOD.message was received
	 * @param msg Pointer to rofl::openflow::cofmsg_flow_mod message containing the received message
	 */
	virtual void
	handle_flow_mod(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_flow_mod& msg) {};

	/**
	 * @brief	Called once a GROUP-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the GROUP-MOD.message was received
	 * @param msg Pointer to rofl::openflow::cofmsg_group_mod message containing the received message
	 */
	virtual void
	handle_group_mod(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_group_mod& msg) {};

	/**
	 * @brief	Called once a TABLE-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the TABLE-MOD.message was received
	 * @param msg Pointer to rofl::openflow::cofmsg_table_mod message containing the received message
	 */
	virtual void
	handle_table_mod(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_table_mod& msg) {};

	/**
	 * @brief	Called once a PORT-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the PORT-MOD.message was received
	 * @param msg Pointer to rofl::openflow::cofmsg_port_mod message containing the received message
	 */
	virtual void
	handle_port_mod(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_port_mod& msg) {};

	/**
	 * @brief	Called once a FLOW-REMOVED.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the FLOW-REMOVED.message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_flow_removed message containing the received message
	 */
	virtual void
	handle_flow_removed(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_flow_removed& msg) {};

	/**
	 * @brief	Called once a PORT-STATUS.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the PORT-STATUS.message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_port_status message containing the received message
	 */
	virtual void
	handle_port_status(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_port_status& msg) {};

	/**
	 * @brief	Called once a QUEUE-GET-CONFIG.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the QUEUE-GET-CONFIG.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_queue_get_config_request message containing the received message
	 */
	virtual void
	handle_queue_get_config_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_queue_get_config_request& msg) {};

	/**
	 * @brief	Called once a QUEUE-GET-CONFIG.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the QUEUE-GET-CONFIG.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_queue_get_config_reply message containing the received message
	 */
	virtual void
	handle_queue_get_config_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_queue_get_config_reply& msg) {};

	/**
	 * @brief	Called once a timer expires for a GET-CONFIG.reply message.
	 *
	 * Default behaviour: deletes cofdpt instance, thus effectively closing the control connection.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	handle_queue_get_config_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once an EXPERIMENTER.message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the EXPERIMENTER.message was received
	 * @param msg Pointer to rofl::openflow::cofmsg_experimenter message containing the received message
	 */
	virtual void
	handle_set_config(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_set_config& msg) {};

	/**
	 * @brief	Called once an EXPERIMENTER.message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the EXPERIMENTER.message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_experimenter message containing the received message
	 */
	virtual void
	handle_experimenter_message(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_experimenter& msg) {};

	/**
	 * @brief	Called once an EXPERIMENTER.message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl pointer to cofctl instance from which the EXPERIMENTER.message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_experimenter message containing the received message
	 */
	virtual void
	handle_experimenter_message(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_experimenter& msg) {};

	/**
	 * @brief	Called once an ERROR.message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the EXPERIMENTER.message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_experimenter message containing the received message
	 */
	virtual void
	handle_error_message(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_error& msg) {};

	/**
	 * @brief	Called once an ERROR.message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl pointer to cofctl instance from which the EXPERIMENTER.message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_experimenter message containing the received message
	 */
	virtual void
	handle_error_message(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_error& msg) {};

	/**
	 * @brief	Called once a timer expires for a GET-CONFIG.reply message.
	 *
	 * Default behaviour: deletes cofdpt instance, thus effectively closing the control connection.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	handle_experimenter_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a timer has expired for an EXPERIMENTER.message.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of EXPERIMENTER.message previously sent to data path element.
	 */
	virtual void
	handle_get_fsp_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a ROLE.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the ROLE.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_role_request message containing the received message
	 */
	virtual void
	handle_role_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_role_request& msg) {};

	/**
	 * @brief	Called once a ROLE.reply message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt pointer to cofdpt instance from which the ROLE.reply message was received.
	 * @param msg pointer to rofl::openflow::cofmsg_role_reply message containing the received message
	 */
	virtual void
	handle_role_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_role_reply& msg) {};

	/**
	 * @brief	Called once a timer has expired for a ROLE.reply.
	 *
	 * To be overwritten by derived class. Default behaviour: ignores event.
	 *
	 * @param dpt pointer to cofdpt instance where the timeout occured.
	 * @param xid transaction ID of ROLE.reply message previously sent to data path element.
	 */
	virtual void
	handle_role_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once a GET-ASYNC-CONFIG.request message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the GET-ASYNC-CONFIG.request was received
	 * @param msg Pointer to rofl::openflow::cofmsg_get_async_config_request message containing the received message
	 */
	virtual void
	handle_get_async_config_request(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_get_async_config_request& msg) {};

	/**
	 * @brief	Called once a GET-ASYNC-CONFIG.reply message was received from a data path element.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param dpt Pointer to cofdpt instance from which the GET-ASYNC-CONFIG.reply was received
	 * @param msg Pointer to rofl::openflow::cofmsg_get_async_config_reply message containing the received message
	 */
	virtual void
	handle_get_async_config_reply(rofl::crofdpt& dpt, const rofl::cauxid& auxid, rofl::openflow::cofmsg_get_async_config_reply& msg) {};

	/**
	 * @brief	Called once a timer expires for a GET-ASYNC-CONFIG.reply message.
	 *
	 * Default behaviour: deletes cofdpt instance, thus effectively closing the control connection.
	 *
	 * @param dpt pointer to cofdpt instance
	 */
	virtual void
	handle_get_async_config_reply_timeout(rofl::crofdpt& dpt, uint32_t xid) {};

	/**
	 * @brief	Called once an SET-ASYNC-CONFIG.message was received from a controller entity.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the SET-ASYNC-MESSAGE.message was received
	 * @param msg Pointer to rofl::openflow::cofmsg_set_async_config message containing the received message
	 */
	virtual void
	handle_set_async_config(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_set_async_config& msg) {};

	/**
	 * @brief	Called once a METER-MOD.message was received.
	 *
	 * To be overwritten by derived class. Default behavior: removes msg from heap.
	 *
	 * @param ctl Pointer to cofctl instance from which the METER-MOD.message was received
	 * @param msg Pointer to rofl::openflow::cofmsg_meter_mod message containing the received message
	 */
	virtual void
	handle_meter_mod(rofl::crofctl& ctl, const rofl::cauxid& auxid, rofl::openflow::cofmsg_meter_mod& msg) {};

	/**@}*/

protected:

	/**
	 *
	 */
	void
	send_packet_in_message(
			const rofl::cauxid& auxid,
			uint32_t buffer_id,
			uint16_t total_len,
			uint8_t reason,
			uint8_t table_id,
			uint64_t cookie,
			uint16_t in_port, // for OF1.0
			rofl::openflow::cofmatch &match,
			uint8_t *data,
			size_t datalen);

	/**
	 *
	 */
	void
	send_flow_removed_message(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmatch& match,
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
	 *
	 */
	void
	send_port_status_message(
			const rofl::cauxid& auxid,
			uint8_t reason,
			rofl::openflow::cofport const& port);


public:

	friend std::ostream&
	operator<< (std::ostream& os, crofbase const& rofbase) {
		os << "<crofbase >" << std::endl;
		for (std::map<cctlid, crofctl*>::const_iterator
				it = rofbase.rofctls.begin(); it != rofbase.rofctls.end(); ++it) {
			rofl::indent i(2);
			os << it->first;
		}
		for (std::map<cdptid, crofdpt*>::const_iterator
				it = rofbase.rofdpts.begin(); it != rofbase.rofdpts.end(); ++it) {
			rofl::indent i(2);
			os << it->first;
		}
		return os;
	};


protected:

	virtual void
	handle_connect_refused(
			crofconn& conn);

	virtual void
	handle_connect_failed(
			crofconn& conn);

	virtual void
	handle_connected(
			crofconn& conn,
			uint8_t ofp_version);

	virtual void
	handle_closed(
			crofconn& conn)
	{};

	virtual void
	handle_write(
			crofconn& conn)
	{};

	virtual void
	recv_message(
			crofconn& conn,
			rofl::openflow::cofmsg *msg)
	{ delete msg; };

	virtual uint32_t
	get_async_xid(
			crofconn& conn)
	{ return transactions.get_async_xid(); };

	virtual uint32_t
	get_sync_xid(
			crofconn& conn,
			uint8_t msg_type = 0,
			uint16_t msg_sub_type = 0)
	{ return transactions.add_ta(cclock(5, 0), msg_type, msg_sub_type); };

	virtual void
	release_sync_xid(
			crofconn& conn,
			uint32_t xid)
	{ transactions.drop_ta(xid); };

protected:

	virtual void
	ta_expired(ctransactions& tas, ctransaction& ta)
	{};

protected:

	virtual void
	handle_listen(
			csocket& socket,
			int newsd);

	virtual void
	handle_accepted(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_accept_refused(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_connected(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_connect_refused(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_connect_failed(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_read(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_write(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_closed(
			csocket& socket);

private:

	/**
	 * @brief called from crofdpt instance, when peer entity has attached
	 */
	virtual void
	handle_dpt_attached(crofdpt& dpt) {
		handle_dpt_open(dpt);
	};

	/**
	 * @brief called from crofdpt instance, when peer entity has detached
	 */
	virtual void
	handle_dpt_detached(crofdpt& dpt) {
		// destroy crofdpt object, when is was created upon an incoming connection from a peer entity
		if (dpt.remove_upon_channel_termination()) {
			dpts_detached.insert(dpt.get_dptid());
			push_on_eventqueue(EVENT_DPT_DETACHED);
		} else {
			handle_dpt_close(dpt);
		}
	};

	/**
	 * @brief called from crofctl instance, when peer entity has attached
	 */
	virtual void
	handle_ctl_attached(crofctl& ctl) {
		handle_ctl_open(ctl);
	};

	/**
	 * @brief called from crofctl instance, when peer entity has detached
	 */
	virtual void
	handle_ctl_detached(crofctl& ctl) {
		// destroy crofctl object, when is was created upon an incoming connection from a peer entity
		if (ctl.remove_upon_channel_termination()) {
			ctls_detached.insert(ctl.get_ctlid());
			push_on_eventqueue(EVENT_CTL_DETACHED);
		} else {
			handle_ctl_close(ctl);
		}
	};

	/**
	 * @brief handle timeout events from rofl::ciosrv
	 */
	virtual void
	handle_timeout(
			int opaque, void* data = (void*)0) {
		switch (opaque) {
		case TIMER_RUN_ENGINE: {
			work_on_eventqueue();
		} break;
		default: {
			// do nothing
		};
		}
	};

	/**
	 * @brief register new event
	 */
	void
	push_on_eventqueue(
			enum crofbase_event_t event = EVENT_NONE) {
		if (EVENT_NONE != event) {
			eventqueue.push_back(event);
		}
		register_timer(TIMER_RUN_ENGINE, rofl::ctimespec(/*second(s)=*/0));
	};

	/**
	 * @brief run crofbase's internal state engine
	 */
	void
	work_on_eventqueue() {
		while (not eventqueue.empty()) {
			crofbase_event_t event = eventqueue.front();
			eventqueue.pop_front();
			switch (event) {
			case EVENT_CTL_DETACHED: {
				event_ctls_detached();
			} break;
			case EVENT_DPT_DETACHED: {
				event_dpts_detached();
			} break;
			default: {
				// do nothing for unknown event types
			};
			}
		}
	};

	/**
	 * @brief handle events of type CTL-DETACHED
	 */
	void
	event_ctls_detached() {
		for (std::set<rofl::cctlid>::iterator
				it = ctls_detached.begin(); it != ctls_detached.end(); ++it) {
			handle_ctl_close(rofl::crofctl::get_ctl(*it));
			rofl::logging::info << "[rofl-common][crofbase] "
					<< "dropping crofctl instance, ctlid:" << it->str() << std::endl;
			drop_ctl(*it);
		}
		ctls_detached.clear();
	};

	/**
	 * @brief handle events of type DPT-DETACHED
	 */
	void
	event_dpts_detached() {
		for (std::set<rofl::cdptid>::iterator
				it = dpts_detached.begin(); it != dpts_detached.end(); ++it) {
			handle_dpt_close(rofl::crofdpt::get_dpt(*it));
			rofl::logging::info << "[rofl-common][crofbase] "
					<< "dropping crofdpt instance, dptid:" << it->str() << std::endl;
			drop_dpt(*it);
		}
		dpts_detached.clear();
	};

protected:

	/**< set of active controller connections */
	std::map<cctlid, crofctl*>		rofctls;

	/**< set of active data path connections */
	std::map<cdptid, crofdpt*>		rofdpts;

private:

	/**< set of all active crofbase instances */
	static std::set<crofbase*> 		rofbases;

	/**< two sets of listening sockets for ctl and dpt */
	std::set<csocket*>				listening_sockets[2];

	// supported OpenFlow versions
	rofl::openflow::cofhello_elem_versionbitmap
									versionbitmap;

	// pending OpenFlow transactions
	ctransactions					transactions;

	// generation_id used for roles initially defined?
	bool							generation_is_defined;

	// cached generation_id as defined by OpenFlow
	uint64_t						cached_generation_id;

	// event-queue
	std::list<enum crofbase_event_t>
									eventqueue;

	// set of crofdpt instances in detached state
	std::set<rofl::cdptid>			dpts_detached;

	// set of crofctl instances in detached state
	std::set<rofl::cctlid>			ctls_detached;
};

}; // end of namespace

#endif


