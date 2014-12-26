/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CROFCTL_H
#define CROFCTL_H 1

#include <map>
#include <string>
#include <bitset>

#include "openflow/openflow.h"
#include "croflexception.h"
#include "cmemory.h"
#include "rofl/common/cctlid.h"
#include "rofl/common/csocket.h"
#include "rofl/common/crofchan.h"
#include "openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofport.h"
#include "rofl/common/openflow/cofports.h"
#include "rofl/common/openflow/coftables.h"
#include "rofl/common/openflow/cofasyncconfig.h"
#include "rofl/common/openflow/cofrole.h"
#include "rofl/common/openflow/cofdescstats.h"
#include "rofl/common/openflow/cofaggrstats.h"
#include "rofl/common/openflow/cofgroupfeaturesstats.h"
#include "rofl/common/openflow/cofpacketqueues.h"
#include "rofl/common/openflow/cofflowstatsarray.h"
#include "rofl/common/openflow/coftablestatsarray.h"
#include "rofl/common/openflow/cofportstatsarray.h"
#include "rofl/common/openflow/cofqueuestatsarray.h"
#include "rofl/common/openflow/cofgroupstatsarray.h"
#include "rofl/common/openflow/cofgroupdescstatsarray.h"
#include "rofl/common/openflow/cofmeterstatsarray.h"
#include "rofl/common/openflow/cofmeterconfigarray.h"
#include "rofl/common/openflow/cofmeterfeatures.h"
#include "rofl/common/cauxid.h"

#include "rofl/common/ciosrv.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/logging.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/ctransactions.h"


namespace rofl {

class eRofCtlBase 		: public RoflException {
public:
	eRofCtlBase(const std::string& __arg) : RoflException(__arg) {};
};
class eRofCtlNotFound 	: public eRofCtlBase {
public:
	eRofCtlNotFound(const std::string& __arg) : eRofCtlBase(__arg) {};
};

class crofctl;

/**
 * @brief	Interface for an environment for rofl::crofctl.
 *
 * This class defines the interface to the environment required
 * by an instance of class rofl::crofctl. Its API comprises two
 * groups of methods:
 *
 * 1. Methods for receiving OpenFlow control channel and
 * connections related notifications.
 * 2. Methods for receiving OpenFlow messages, once the control
 * channel has been established.
 *
 * Overwrite any of these methods for receiving certain event
 * notifications from the associated rofl::crofctl instance.
 */
class crofctl_env {
	friend class crofctl;
	static std::set<crofctl_env*> rofctl_envs;
public:

	/**
	 * @brief	rofl::crofctl_env constructor
	 */
	crofctl_env()
	{ crofctl_env::rofctl_envs.insert(this); };

	/**
	 * @brief	rofl::crofctl_env destructor
	 */
	virtual
	~crofctl_env()
	{ crofctl_env::rofctl_envs.erase(this); };

protected:

	/**
	 * @name 	Event handlers for management notifications for controller entities
	 *
	 * Overwrite any of these methods for receiving controller related event notifications.
	 */

	/**@{*/

	/**
	 * @brief	Called after establishing the associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been established, i.e., its main connection has been accepted by the remote site.
	 *
	 * @param ctl controller instance
	 */
	virtual void
	handle_chan_established(
			rofl::crofctl& ctl)
	{};

	/**
	 * @brief	Called after termination of associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been terminated, i.e., its main connection has been closed from the
	 * remote site. The rofl::crofctl instance itself is not destroyed, unless
	 * its 'remove_on_channel_close' flag has been set to true during its
	 * construction.
	 *
	 * @param ctl controller instance
	 */
	virtual void
	handle_chan_terminated(
			rofl::crofctl& ctl)
	{};

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been established.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_established(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been terminated by the peer entity.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_terminated(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief 	Called when an attempt to establish a control connection has been refused.
	 *
	 * This event occurs when the C-library's connect() system call fails
	 * with the ECONNREFUSED error code. This indicates typically a problem on
	 * the remote site.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_refused(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief 	Called when an attempt to establish a control connection has been failed.
	 *
	 * This event occurs when some failure occures while calling the underlying
	 * C-library connect() system call, e.g., no route to destination, etc. This may
	 * indicate a local configuration problem inside or outside of the application.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_failed(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief	Called when a congestion situation on the control connection has been solved.
	 *
	 * A control channel may face congestion situations when insufficient bandwidth
	 * on the underlying IP path or some backpressuring by the remote site requires
	 * to throttle the overall message transmission rate on a control connection.
	 * A congestion situation is indicated by the return values obtained from the various
	 * send-methods defined within rofl::crofctl. A solved congestion situation is
	 * indicated by calling this method. Note that ROFL will store OpenFlow messages
	 * even under congestion, thus filling up its internal buffers until no further
	 * memory is available for doing so. It is up to the application designer to
	 * throttle transmission of further messages according to the channel capacity.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_writable(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

	/**@}*/

	/**
	 * @brief	called once a cofctl instance has received a role request
	 *
	 *
	 */
	virtual void
	role_request_rcvd(
			rofl::crofctl& ctl, uint32_t role, uint64_t rcvd_generation_id)
	{};

protected:

	/**
	 * @name 	Event handlers for OpenFlow message notifications received from controller entities
	 *
	 * Overwrite any of these methods for receiving messages from the attached controller entity.
	 * Once this method terminates, rofl will destroy the message object. If you want to save
	 * a message or parts of it, you must create a local copy. All messages are internally
	 * allocated on the heap and both allocation and deallocation is handled by rofl-common.
	 * Do not delete any of these messages or its internals.
	 *
	 * Higher logic implemented on top of crofbase and acting in datapath role should handle
	 * incoming requests, e.g., sending back a reply to the controller entity. rofl-common does
	 * not generate reply messages automatically. You may throw any of the OpenFlow related exceptions
	 * defined by rofl-common within these handler methods. rofl-common will detect such exceptions
	 * and generate appropriate error messages for transmission to the peer entity.
	 */

	/**@{*/

	/**
	 * @brief	OpenFlow Features-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_features_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_request& msg)
	{};

	/**
	 * @brief	OpenFlow Get-Config-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_config_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_config_request& msg)
	{};

	/**
	 * @brief	OpenFlow Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	void
	handle_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Desc-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_desc_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_desc_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Table-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Port-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Flow-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Aggregate-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_aggregate_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_aggr_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Queue-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Group-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Group-Desc-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_desc_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_desc_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Group-Features-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_features_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_features_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Meter-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Meter-Config-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_config_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_config_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Meter-Features-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_features_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_features_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Table-Features-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_features_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_features_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Port-Desc-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_desc_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_desc_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Experimenter-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Packet-Out message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_packet_out(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_packet_out& msg)
	{};

	/**
	 * @brief	OpenFlow Barrier-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_barrier_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_barrier_request& msg)
	{};

	/**
	 * @brief	OpenFlow Flow-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_mod& msg)
	{};

	/**
	 * @brief	OpenFlow Group-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_mod& msg)
	{};

	/**
	 * @brief	OpenFlow Table-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_mod& msg)
	{};

	/**
	 * @brief	OpenFlow Port-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_mod& msg)
	{};

	/**
	 * @brief	OpenFlow Queue-Get-Config-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_get_config_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_get_config_request& msg)
	{};

	/**
	 * @brief	OpenFlow Set-Config message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_set_config(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_set_config& msg)
	{};

	/**
	 * @brief	OpenFlow Experimenter message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_message(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter& msg)
	{};

	/**
	 * @brief	OpenFlow error message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_error_message(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_error& msg)
	{};

	/**
	 * @brief	OpenFlow Role-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_role_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_role_request& msg)
	{};

	/**
	 * @brief	OpenFlow Get-Async-Config-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_async_config_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_async_config_request& msg)
	{};

	/**
	 * @brief	OpenFlow Set-Async-Config message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_set_async_config(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_set_async_config& msg)
	{};

	/**
	 * @brief	OpenFlow Meter-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_mod& msg)
	{};

	/**@}*/
};



/**
 * @brief	Class representing a remote controller entity
 *
 * This class encapsulates properties of a single remote controller entity
 * including the OpenFlow control channel, its role and the set of
 * asynchronous event notifications to be sent to this controller.
 * Its public API offers methods to manage
 * the OpenFlow control channel, i.e. CRUD methods for individual control
 * connections.
 *
 * rofl::crofctl expects an instance of class rofl::crofctl_env as surrounding
 * environment and sends various notifications via this interface. Class
 * rofl::crofbase implements this interface and may be used as base class
 * for advanced applications. However, you may implement rofl::crofctl_env
 * directly as well.
 *
 */
class crofctl :
		public rofl::crofchan_env,
		public rofl::ctransactions_env,
		public ciosrv
{
	enum crofctl_timer_t {
		TIMER_RUN_ENGINE       = 0,
	};

	enum crofctl_event_t {
		EVENT_NONE             = 0,
		EVENT_CHAN_TERMINATED  = 1,
		EVENT_CONN_TERMINATED  = 2,
		EVENT_CONN_REFUSED     = 3,
		EVENT_CONN_FAILED      = 4,
	};

	enum crofctl_flag_t {
		FLAG_ENGINE_IS_RUNNING = (1 << 0),
	};

public:

	/**
	 * @brief	Returns reference to rofl::crofctl instance identified by rofl-common's internal identifier.
	 *
	 * @param ctlid rofl-common's internal datapath identifier
	 * @throw eRofCtlNotFound when no object matches the controller identifier
	 * @return reference to rofl::crofctl instance for given identifier
	 */
	static rofl::crofctl&
	get_ctl(
			const rofl::cctlid& ctlid);

	/**
	 * @brief 	crofctl constructor
	 *
	 * @param env pointer to rofl::crofctl_env instance defining the environment for this object
	 * @param ctlid rofl-common's internal identifier for this instance
	 * @param remove_on_channel_close when set to true, this indicates to remove this object after the control channel has been terminated
	 * @param versionbitmap OpenFlow version bitmap
	 */
	crofctl(
			crofctl_env* env,
			const cctlid& ctlid,
			bool remove_on_channel_close,
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap) :
				env(env),
				ctlid(ctlid),
				rofchan(this, versionbitmap),
				transactions(this),
				remove_on_channel_close(remove_on_channel_close),
				async_config_role_default_template(rofl::openflow13::OFP_VERSION),
				async_config(rofl::openflow13::OFP_VERSION) {
		rofl::logging::debug << "[rofl-common][crofctl] "
				<< "instance created, ctlid: " << ctlid.str() << std::endl;
		init_async_config_role_default_template();
		async_config = get_async_config_role_default_template();
		crofctl::rofctls[ctlid] = this;
	};

	/**
	 * @brief	crofctl destructor
	 *
	 * Closes all control connections and does a general clean-up.
	 */
	virtual
	~crofctl() {
		rofl::logging::debug << "[rofl-common][crofctl] "
				<< "instance destroyed, ctlid: " << ctlid.str() << std::endl;
		crofctl::rofctls.erase(ctlid);
	};

	/**
	 * @brief	Returns rofl-common's internal rofl::cctlid identifier for this instance
	 *
	 * @return internal controller entity identifier
	 */
	const rofl::cctlid&
	get_ctlid() const
	{ return ctlid; };

public:

	/**
	 * @name	Methods for connection management
	 *
	 * This is a group of methods for typical CRUD like operations on control
	 * connections for the OpenFlow control channel. You may create an arbitrary
	 * number of control connections to a controller entity. Control connections
	 * may be closed or reconnected.
	 */

	/**@{*/

	/**
	 * @brief	Returns a list of connection identifiers of all existing control connections
	 *
	 * The list contains all connections independent from their current status.
	 *
	 * @return list of connection identifiers
	 */
	std::list<rofl::cauxid>
	get_conn_index() const
	{ return rofchan.get_conn_index(); };

	/**
	 * @brief	Establishes a new control connection to a remote
	 * controller entity with the given control connection identifier
	 *
	 * An already existing control connection with the specified control
	 * connection identifier is replaced by this new control connection
	 * instance. You may select any arbitrary control connection
	 * identifier. However, care must be taken for the main connection (auxid: 0):
	 * (Re-)Connecting the main connection leads to an implicit termination of
	 * all existing control connections in OpenFlow.
	 *
	 * @param auxid control connection identifier
	 * @param socket_type one of the socket types defined in rofl::csocket
	 * @param socket_params a set of parameters for the selected socket type
	 */
	void
	connect(
			const rofl::cauxid& auxid,
			enum rofl::csocket::socket_type_t socket_type,
			const rofl::cparams& socket_params)
	{ rofchan.add_conn(auxid, socket_type, socket_params); };

	/**
	 * @brief	Terminates an existing control connection with given identifier.
	 *
	 * When the main control connection (auxid: 0) is closed, this also terminates all
	 * other existing control connections.
	 *
	 * @param auxid control connection identifier
	 */
	void
	disconnect(
			const rofl::cauxid& auxid)
	{ rofchan.drop_conn(auxid); };

	/**
	 * @brief	Add an existing rofl::crofconn instance created on heap to this object.
	 *
	 * This method is used for attaching an already existing rofl::crofconn instance
	 * to this rofl::crofctl instance. Do not call this method, unless you know what
	 * you are doing.
	 *
	 * @param conn pointer to rofl::crofconn instance allocated on heap
	 */
	void
	add_connection(
			crofconn* conn) {
		if (NULL == conn) {
			return;
		}
		rofchan.add_conn(conn->get_aux_id(), conn);
	};

	/**@}*/

public:

	/**
	 * @name	Methods related to control channel state
	 */

	/**@{*/

	/**
	 * @brief	Returns true, when the control handshake (HELLO) has been completed.
	 */
	bool
	is_established() const
	{ return rofchan.is_established(); };

	/**
	 * @brief 	Returns the OpenFlow protocol version used for this control connection.
	 *
	 * @return OpenFlow version used for this control connection
	 */
	uint8_t
	get_version_negotiated() const
	{ return rofchan.get_version(); };

	/**
	 * @brief	Returns the defined OpenFlow version bitmap for this instance.
	 *
	 * @return OpenFlow version bitmap
	 */	const rofl::openflow::cofhello_elem_versionbitmap&
	get_versions_available() const
	{ return rofchan.get_versionbitmap(); };

	/**
	 * @brief	Returns true, when this instance should be destroyed when its crofchan has closed
	 */
	bool
	remove_on_channel_termination() const
	{ return remove_on_channel_close; };

	/**
	 * @brief	Returns caddress of connected remote entity for given connection identifier.
	 *
	 * @return caddress object obtained from this->socket
	 */
	rofl::caddress
	get_peer_addr(const rofl::cauxid& auxid) const
	{ return rofchan.get_conn(auxid).get_rofsocket().get_socket().get_raddr(); };

	/**@}*/

public:

	/**
	 * @name	Methods granting access to the controller entity's basic properties
	 */

	/**@{*/

	/**
	 * @brief	Returns a reference to the current role object of this controller entity.
	 */
	rofl::openflow::cofrole&
	set_role()
	{ return role; };

	/**
	 * @brief	Returns a const reference to the current role object of this controller entity.
	 */
	const rofl::openflow::cofrole&
	get_role() const
	{ return role; };

	/**
	 * @brief	Returns true, when the control entity is in role -slave-.
	 */
	bool
	is_slave() const;

	/**
	 * @brief	Returns a reference to the current asynchronous event configuration of this controller entity.
	 */
	rofl::openflow::cofasync_config&
	set_async_config()
	{ return async_config; };

	/**
	 * @brief	Returns a const reference to the current asynchronous event configuration of this controller entity.
	 */
	const rofl::openflow::cofasync_config&
	get_async_config() const
	{ return async_config; };

	/**
	 * @brief	Returns a const reference to the default asynchronous event configuration for controller entities defined by OpenFlow.
	 */
	const rofl::openflow::cofasync_config&
	get_async_config_role_default_template() const
	{ return async_config_role_default_template; };

	/**@}*/

public:

	/**
	 * @name	Methods for sending OpenFlow messages
	 *
	 * These methods may be called by a derived class for sending
	 * a specific OpenFlow message.
	 */

	/**@{*/

	/**
	 * @brief	Sends OpenFlow Features-Reply message to attached controller entity.
	 *
	 * @param auxid controller connection identifier
	 * @param xid OpenFlow transaction identifier
	 * @param dpid OpenFlow datapath identifier
	 * @param n_buffers number of buffers available in this datapath element's I/O subsystem
	 * @param n_tables number of tables available in this datapath element's pipeline
	 * @param capabilities OpenFlow capabilities
	 * @param of13_auxiliary_id control connection identifier since OpenFlow 1.3
	 * @param of10_actions_bitmap set of available actions for OpenFlow 1.0 only
	 * @param ports OpenFlow list of ports
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	void
	send_features_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			uint64_t dpid,
			uint32_t n_buffers,
			uint8_t n_tables,
			uint32_t capabilities,
			uint8_t of13_auxiliary_id = 0,
			uint32_t of10_actions_bitmap = 0,
			const rofl::openflow::cofports& ports = rofl::openflow::cofports());

	/**
	 * @brief	Sends OpenFlow Get-Config-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param flags datapath element flags
	 * @param miss_send_len default miss_send_len value
	 */
	void
	send_get_config_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			uint16_t flags,
			uint16_t miss_send_len);

	/**
	 * @brief	Sends OpenFlow Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param stats_type one of the OFPST_* constants
	 * @param body body of a STATS.reply
	 * @param bodylen length of STATS.reply body
	 * @param more flag if multiple STATS.reply messages will be sent
	 */
	void
	send_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			uint16_t stats_type,
			uint16_t stats_flags,
			uint8_t *body = NULL,
			size_t bodylen = 0);

	/**
	 * @brief	Sends OpenFlow Desc-Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param desc_stats body of DESC-STATS.reply
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_desc_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofdesc_stats_reply& desc_stats,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Flow-Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param flow_stats array of flow_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_flow_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofflowstatsarray& flow_stats,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Aggregate-Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param aggr_stats aggr_stats body
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_aggr_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofaggr_stats_reply& aggr_stats,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Table-Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param table_stats array of table_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_table_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::coftablestatsarray& tablestatsarray,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Port-Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param port_stats array of port_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_port_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofportstatsarray& portstatsarray,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Queue-Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param port_stats array of port_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_queue_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofqueuestatsarray& queuestatsarray,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Group-Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param group_stats array of group_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_group_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofgroupstatsarray& groupstatsarray,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Group-Desc-Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param group_desc_stats array of group_desc_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_group_desc_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofgroupdescstatsarray& groupdescs,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Group-Features-Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param group_features_stats group_features_stats body
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_group_features_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofgroup_features_stats_reply& group_features_stats,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Table-Features-Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param tables tables body
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_table_features_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::coftables& tables,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Port-Desc-Stats-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param ports ports body
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_port_desc_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofports& ports,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Experimenter-Stats-Reply message to attached controller entity.
	 *
	 * @param auxid auxiliary connection id
	 * @param xid OpenFlow transaction identifier
	 * @param exp_id experimenter ID
	 * @param exp_type user defined type
	 * @param body start of user defined body
	 * @param bodylen length of user defined body
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_experimenter_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			uint32_t exp_id,
			uint32_t exp_type,
			const cmemory& body,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Meter-Stats-Reply message to attached controller entity.
	 *
	 * @param auxid auxiliary connection id
	 * @param xid OpenFlow transaction identifier
	 * @param meter_stats_array instance of type cofmeterstatsarray
	 * @param stats_flags flags for OpenFlow statistics messages, if any (default: 0)
	 */
	void
	send_meter_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofmeterstatsarray& meter_stats_array,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Meter-Config-Stats-Reply message to attached controller entity.
	 *
	 * @param auxid auxiliary connection id
	 * @param xid OpenFlow transaction identifier
	 * @param meter_config_array instance of type cofmeterconfigarray
	 * @param stats_flags flags for OpenFlow statistics messages, if any (default: 0)
	 */
	void
	send_meter_config_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofmeterconfigarray& meter_config_array,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Meter-Features-Stats-Reply message to attached controller entity.
	 *
	 * @param auxid auxiliary connection id
	 * @param xid OpenFlow transaction identifier
	 * @param meter_config_array instance of type cofmeterfeatures
	 * @param stats_flags flags for OpenFlow statistics messages, if any (default: 0)
	 */
	void
	send_meter_features_stats_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofmeter_features& meter_features,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends OpenFlow Packet-In message to attached controller entity.
	 *
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
	void
	send_packet_in_message(
			const rofl::cauxid& auxid,
			uint32_t buffer_id,
			uint16_t total_len,
			uint8_t reason,
			uint8_t table_id,
			uint64_t cookie,
			uint16_t in_port, // for OF1.0
			const rofl::openflow::cofmatch& match,
			uint8_t *data,
			size_t datalen);

	/**
	 * @brief	Sends OpenFlow Barrier-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 */
	void
	send_barrier_reply(
			const rofl::cauxid& auxid,
			uint32_t xid);

	/**
	 * @brief	Sends OpenFlow Error message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param type one of OpenFlow's OFPET_* flags
	 * @param code one of OpenFlow's error codes
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	void
	send_error_message(
			const rofl::cauxid& auxid,
			uint32_t xid,
			uint16_t type,
			uint16_t code,
			uint8_t* data = NULL,
			size_t datalen = 0);

	/**
	 * @brief	Sends OpenFlow Experimenter message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param experimenter_id exp_id as assigned by ONF
	 * @param exp_type exp_type as defined by the ONF member
	 * @param body pointer to opaque experimenter message body (optional)
	 * @param bodylen length of body (optional)
	 * @result transaction ID assigned to this request
	 */
	void
	send_experimenter_message(
			const rofl::cauxid& auxid,
			uint32_t xid,
			uint32_t experimenter_id,
			uint32_t exp_type,
			uint8_t *body = NULL,
			size_t bodylen = 0);

	/**
	 * @brief	Sends OpenFlow Flow-Removed message to attached controller entity.
	 *
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
	void
	send_flow_removed_message(
			const rofl::cauxid& auxid,
			const rofl::openflow::cofmatch& match,
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
	 * @brief	Sends OpenFlow Port-Status message to attached controller entity.
	 *
	 * @param reason one of OpenFlow's OFPPR_* constants
	 * @param port rofl::openflow::cofport instance that changed its status
	 */
	void
	send_port_status_message(
			const rofl::cauxid& auxid,
			uint8_t reason,
			const rofl::openflow::cofport& port);

	/**
	 * @brief	Sends OpenFlow Queue-Get-Config-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param portno OpenFlow number assigned to port
	 */
	void
	send_queue_get_config_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			uint32_t portno,
			const rofl::openflow::cofpacket_queues& queues);


	/**
	 * @brief	Sends OpenFlow Role-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 * @param role defined role from data path
	 * @param generation_id gen_id as defined by OpenFlow
	 */
	void
	send_role_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofrole& role);

	/**
	 * @brief	Sends OpenFlow Get-Async-Config-Reply message to attached controller entity.
	 *
	 * @param xid OpenFlow transaction identifier
	 */
	void
	send_get_async_config_reply(
			const rofl::cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofasync_config& async_config);

	/**@}*/


public:

	friend std::ostream&
	operator<< (std::ostream& os, const crofctl& ctl) {
		os << indent(0) << "<crofctl ";
		os << "ctlid:0x" << ctl.ctlid.str() << " ";
		os << ">" << std::endl;
		rofl::indent i(2);
		os << ctl.rofchan;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "ctlid: " << get_ctlid().get_ctlid_s() << " ";
		return ss.str();
	};

private:

	virtual void
	handle_conn_established(
			crofchan& chan,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
						<< " control connection established, auxid: " << auxid.str() << std::endl;
		call_env().handle_conn_established(*this, auxid);

		if (auxid == rofl::cauxid(0)) {
			rofl::logging::info << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
							<< " OFP control channel established, " << chan.str() << std::endl;

			call_env().handle_chan_established(*this); // main connection
		}
	};

	virtual void
	handle_conn_terminated(
			crofchan& chan,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
						<< " control connection terminated, auxid: " << auxid.str() << std::endl;
		rofl::RwLock rwlock(conns_terminated_rwlock, rofl::RwLock::RWLOCK_WRITE);
		conns_terminated.push_back(auxid);
		push_on_eventqueue(EVENT_CONN_TERMINATED);

		if (auxid == rofl::cauxid(0)) {
			rofl::logging::info << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
					<< " OFP control channel terminated, " << chan.str() << std::endl;
			transactions.clear();
			push_on_eventqueue(EVENT_CHAN_TERMINATED);
		}
	};

	virtual void
	handle_conn_refused(
			crofchan& chan,
			const rofl::cauxid& auxid) {
		rofl::RwLock rwlock(conns_refused_rwlock, rofl::RwLock::RWLOCK_WRITE);
		conns_refused.push_back(auxid);
		push_on_eventqueue(EVENT_CONN_REFUSED);
	};

	virtual void
	handle_conn_failed(
			crofchan& chan,
			const rofl::cauxid& auxid) {
		rofl::RwLock rwlock(conns_failed_rwlock, rofl::RwLock::RWLOCK_WRITE);
		conns_failed.push_back(auxid);
		push_on_eventqueue(EVENT_CONN_FAILED);
	};

	virtual void
	handle_write(
			rofl::crofchan& chan,
			const rofl::cauxid& auxid)
	{ call_env().handle_conn_writable(*this, auxid); };

	virtual void
	recv_message(
			rofl::crofchan& chan,
			const rofl::cauxid& aux_id,
			rofl::openflow::cofmsg *msg);

	virtual uint32_t
	get_async_xid(
			rofl::crofchan& chan)
	{ return transactions.get_async_xid(); };

	virtual uint32_t
	get_sync_xid(
			rofl::crofchan& chan,
			uint8_t msg_type = 0,
			uint16_t msg_sub_type = 0)
	{ return transactions.add_ta(cclock(/*secs=*/5), msg_type, msg_sub_type); };

	virtual void
	release_sync_xid(
			rofl::crofchan& chan,
			uint32_t xid)
	{ transactions.drop_ta(xid); };

	virtual void
	ta_expired(
			rofl::ctransactions& tas,
			rofl::ctransaction& ta)
	{};

public:

	/**
	 *
	 */
	class crofctl_find_by_ctlid {
		cctlid ctlid;
	public:
		crofctl_find_by_ctlid(const cctlid& ctlid) : ctlid(ctlid) {};
		bool operator() (const crofctl* rofctl) {
			return (rofctl->get_ctlid() == ctlid);
		};
	};

private:

	virtual void
	handle_timeout(
			int opaque, void *data = (void*)0);

	crofctl_env&
	call_env() {
		if (crofctl_env::rofctl_envs.find(env) == crofctl_env::rofctl_envs.end()) {
			throw eRofCtlNotFound("rofl::crofctl::call_env() environment not found");
		}
		return *env;
	};

	void
	init_async_config_role_default_template();

	void
	check_role();

	void
	push_on_eventqueue(
			enum crofctl_event_t event = EVENT_NONE) {
		if (EVENT_NONE != event) {
			events.push_back(event);
		}
		if (not flags.test(FLAG_ENGINE_IS_RUNNING)) {
			register_timer(TIMER_RUN_ENGINE, rofl::ctimespec(0));
		}
	};

	void
	work_on_eventqueue();

	void
	event_chan_terminated();

	void
	event_conn_established();

	void
	event_conn_terminated();

	void
	event_conn_refused();

	void
	event_conn_failed();

private:

	void
	experimenter_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter* msg);

	void
	error_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_error* msg);

	void
	features_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_request* msg);

	void
	get_config_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_config_request* msg);

	void
	set_config_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_set_config* msg);

	void
	packet_out_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_packet_out* msg);

	void
	flow_mod_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_mod* msg);

	void
	group_mod_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_mod* msg);

	void
	port_mod_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_mod* msg);

	void
	table_mod_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_mod* msg);

	void
	meter_mod_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_mod* msg);

	void
	stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_stats* msg);

	void
	desc_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_desc_stats_request* msg);

	void
	table_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_stats_request* msg);

	void
	port_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_stats_request* msg);

	void
	flow_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_stats_request* msg);

	void
	aggregate_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_aggr_stats_request* msg);

	void
	queue_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_stats_request* msg);

	void
	group_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_stats_request* msg);

	void
	group_desc_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_desc_stats_request* msg);

	void
	group_features_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_features_stats_request* msg);

	void
	meter_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_stats_request* msg);

	void
	meter_config_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_config_stats_request* msg);

	void
	meter_features_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_features_stats_request* msg);

	void
	table_features_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_features_stats_request* msg);

	void
	port_desc_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_desc_stats_request* msg);

	void
	experimenter_stats_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter_stats_request* msg);

	void
	role_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_role_request* msg);

	void
	barrier_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_barrier_request* msg);

	void
	queue_get_config_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_get_config_request* msg);

	void
	get_async_config_request_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_async_config_request* msg);

	void
	set_async_config_rcvd(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_set_async_config* msg);

private:

	static std::map<cctlid, crofctl*> rofctls;

	// environment
	rofl::crofctl_env*      env;
	// handle for this crofctl instance
	rofl::cctlid            ctlid;
	// OFP control channel
	rofl::crofchan          rofchan;
	// pending OFP transactions
	rofl::ctransactions     transactions;

	bool                    remove_on_channel_close;
	rofl::openflow::cofasync_config async_config_role_default_template;
	rofl::openflow::cofasync_config async_config;
	rofl::openflow::cofrole role;

	std::deque<enum crofctl_event_t> events;

	PthreadRwLock           conns_terminated_rwlock;
	std::list<rofl::cauxid> conns_terminated;
	PthreadRwLock           conns_refused_rwlock;
	std::list<rofl::cauxid> conns_refused;
	PthreadRwLock           conns_failed_rwlock;
	std::list<rofl::cauxid> conns_failed;

	std::bitset<32>         flags;
};

}; // end of namespace

#endif
