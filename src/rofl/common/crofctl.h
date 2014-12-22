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

class eRofCtlBase 		: public RoflException {};
class eRofCtlNotFound 	: public eRofCtlBase {};

class crofctl;

class crofctl_env {
	friend class crofctl;
	static std::set<crofctl_env*> rofctl_envs;
public:

	crofctl_env()
	{ crofctl_env::rofctl_envs.insert(this); };

	virtual
	~crofctl_env()
	{ crofctl_env::rofctl_envs.erase(this); };

public:

	/**
	 *
	 */
	virtual void
	handle_ctl_attached(
			crofctl& ctl)
	{};

	/**
	 *
	 */
	virtual void
	handle_ctl_detached(
			crofctl& ctl)
	{};

	/**
	 * @brief Called upon establishment of a control connection within the control channel
	 */
	virtual void
	handle_conn_established(
			crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief Called upon a peer initiated termination of a control connection within the control channel
	 */
	virtual void
	handle_conn_terminated(
			crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief Called in the event of a connection refused
	 */
	virtual void
	handle_conn_refused(
			crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief Called in the event of a connection failed (except refused)
	 */
	virtual void
	handle_conn_failed(
			crofctl& ctl,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief	called once a cofctl instance has received a role request
	 *
	 *
	 */
	virtual void
	role_request_rcvd(
			rofl::crofctl& ctl, uint32_t role, uint64_t rcvd_generation_id)
	{};

public:

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
};













class crofctl :
		public rofl::crofchan_env,
		public rofl::ctransactions_env,
		public ciosrv
{
private: // data structures

	enum crofctl_timer_t {
		TIMER_RUN_ENGINE							= 0,
	};

	enum crofctl_state_t {
		STATE_INIT 									= 0,
		STATE_DISCONNECTED							= 1,
		STATE_ESTABLISHED							= 2,
	};

	enum crofctl_event_t {
		EVENT_NONE									= 0,
		EVENT_CHAN_TERMINATED						= 1,
		EVENT_CONN_TERMINATED						= 3,
		EVENT_CONN_REFUSED							= 4,
		EVENT_CONN_FAILED							= 5,
	};

	enum crofctl_flag_t {
		FLAG_ENGINE_IS_RUNNING 						= (1 << 0),
	};

public: // methods

	/**
	 *
	 */
	static crofctl&
	get_ctl(const cctlid& ctlid);

public:

	/**
	 * @brief	Constructor for creating new cofctl instance from accepted socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 */
	crofctl(
			crofctl_env* env,
			const cctlid& ctlid,
			bool remove_on_channel_close,
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap) :
				env(env),
				ctlid(ctlid),
				remove_on_channel_close(remove_on_channel_close),
				async_config_role_default_template(rofl::openflow13::OFP_VERSION),
				state(STATE_INIT),
				rofchan(this, versionbitmap),
				transactions(this),
				async_config(rofl::openflow13::OFP_VERSION) {
		rofl::logging::debug << "[rofl-common][crofctl] "
				<< "instance created, ctlid: " << ctlid.str() << std::endl;
		init_async_config_role_default_template();
		async_config = get_async_config_role_default_template();
		crofctl::rofctls[ctlid] = this;
	};

	/**
	 * @brief	Destructor.
	 */
	virtual
	~crofctl() {
		rofl::logging::debug << "[rofl-common][crofctl] "
				<< "instance destroyed, ctlid: " << ctlid.str() << std::endl;
		crofctl::rofctls.erase(ctlid);
	};

public:

	/**
	 *
	 */
	std::list<rofl::cauxid>
	get_conn_index() const
	{ return rofchan.get_conn_index(); };

	/**
	 *
	 */
	void
	connect(
			const rofl::cauxid& auxid,
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& socket_params)
	{ rofchan.add_conn(auxid, socket_type, socket_params); };

	/**
	 *
	 */
	void
	disconnect(
			const rofl::cauxid& auxid)
	{ rofchan.drop_conn(auxid); };

	/**
	 *
	 */
	void
	reconnect(
			const rofl::cauxid& auxid) {
		rofchan.set_conn(auxid).close();
		rofchan.set_conn(auxid).reconnect();
	};

	/**
	 *
	 */
	void
	add_connection(
			crofconn* conn) {
		if (NULL == conn) {
			return;
		}
		rofchan.add_conn(conn->get_aux_id(), conn);
	};

public:

	/**
	 * @brief	Returns the controller handle id
	 */
	const cctlid&
	get_ctlid() const
	{ return ctlid; };

	/**
	 * @brief	Returns true, when the OFP control channel is up
	 */
	bool
	is_established() const
	{ return rofchan.is_established(); };

	/**
	 * @brief	Returns OpenFlow version negotiated for control connection.
	 */
	uint8_t
	get_version_negotiated() const
	{ return rofchan.get_version(); };

	/**
	 * @brief	Returns a reference to rofchan's cofhello_elem_versionbitmap instance
	 */
	const rofl::openflow::cofhello_elem_versionbitmap&
	get_versions_available() const
	{ return rofchan.get_versionbitmap(); };

	/**
	 *
	 */
	caddress
	get_peer_addr(const rofl::cauxid& auxid) const
	{ return rofchan.get_conn(auxid).get_rofsocket().get_socket().get_raddr(); };

	/**
	 * @brief	Returns true, when the control entity is in role SLAVE
	 */
	bool
	is_slave() const;

	/**
	 * @brief	Returns true, when this instance should be destroyed when its crofchan has closed
	 */
	bool
	remove_upon_channel_termination() const
	{ return remove_on_channel_close; };

	/**
	 *
	 */
	rofl::openflow::cofasync_config&
	set_async_config()
	{ return async_config; };

	/**
	 *
	 */
	const rofl::openflow::cofasync_config&
	get_async_config() const
	{ return async_config; };

	/**
	 *
	 */
	rofl::openflow::cofrole&
	set_role()
	{ return role; };

	/**
	 *
	 */
	const rofl::openflow::cofrole&
	get_role() const
	{ return role; };

	/**
	 *
	 */
	rofl::openflow::cofasync_config&
	set_async_config_role_default_template()
	{ return async_config_role_default_template; };

	/**
	 *
	 */
	const rofl::openflow::cofasync_config&
	get_async_config_role_default_template() const
	{ return async_config_role_default_template; };

public:


	/**
	 * @name	Methods for sending OpenFlow messages
	 *
	 * These methods may be called by a derived class for sending
	 * a specific OF message.
	 */

	/**@{*/

	/**
	 * @brief	Sends a FEATURES.reply to a controller entity.
	 *
	 * @param xid transaction ID from FEATURES.request
	 * @param dpid data path ID of this data path element
	 * @param n_buffers number of packet buffers available
	 * @param capabilities data path capabilities (reassembly, etc.)
	 * @param of13_auxiliary_id auxiliary_id used in OpenFlow 1.3, ignored in other versions
	 * @param of10_actions_bitmap bitmap of supported actions for OpenFlow 1.0, ignored in other versions
	 * @param portlist list of rofl::openflow::cofports in this data path, ignored in OpenFlow 1.3
	 */
	void
	send_features_reply(
			const cauxid& auxid,
			uint32_t xid,
			uint64_t dpid,
			uint32_t n_buffers,
			uint8_t n_tables,
			uint32_t capabilities,
			uint8_t of13_auxiliary_id = 0,
			uint32_t of10_actions_bitmap = 0,
			const rofl::openflow::cofports& ports = rofl::openflow::cofports());

	/**
	 * @brief	Sends a GET-CONFIG.reply to a controller entity.
	 *
	 * @param xid transaction ID from GET-CONFIG.request
	 * @param flags data path flags
	 * @param miss_send_len default miss_send_len value
	 */
	void
	send_get_config_reply(
			const cauxid& auxid,
			uint32_t xid,
			uint16_t flags,
			uint16_t miss_send_len);

	/**
	 * @brief	Sends a STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param stats_type one of the OFPST_* constants
	 * @param body body of a STATS.reply
	 * @param bodylen length of STATS.reply body
	 * @param more flag if multiple STATS.reply messages will be sent
	 */
	void
	send_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			uint16_t stats_type,
			uint16_t stats_flags,
			uint8_t *body = NULL,
			size_t bodylen = 0);

	/**
	 * @brief	Sends a DESC-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param desc_stats body of DESC-STATS.reply
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_desc_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofdesc_stats_reply& desc_stats,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a FLOW-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param flow_stats array of flow_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_flow_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofflowstatsarray& flow_stats,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends an AGGREGATE-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param aggr_stats aggr_stats body
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_aggr_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofaggr_stats_reply& aggr_stats,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a TABLE-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param table_stats array of table_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_table_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::coftablestatsarray& tablestatsarray,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a PORT-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param port_stats array of port_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_port_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofportstatsarray& portstatsarray,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a QUEUE-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param port_stats array of port_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_queue_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofqueuestatsarray& queuestatsarray,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a GROUP-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param group_stats array of group_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_group_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofgroupstatsarray& groupstatsarray,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a GROUP-DESC-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param group_desc_stats array of group_desc_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_group_desc_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofgroupdescstatsarray& groupdescs,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a GROUP-FEATURES-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param group_features_stats group_features_stats body
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_group_features_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofgroup_features_stats_reply& group_features_stats,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a TABLE-FEATURES-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param tables tables body
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_table_features_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::coftables& tables,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a PORT-DESC-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param ports ports body
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_port_desc_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofports& ports,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends an EXPERIMENTER-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param exp_id experimenter ID
	 * @param exp_type user defined type
	 * @param body start of user defined body
	 * @param bodylen length of user defined body
	 * @param more flag if multiple STATS replies will be sent
	 */
	void
	send_experimenter_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			uint32_t exp_id,
			uint32_t exp_type,
			const cmemory& body,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a METER-STATS.reply to a controller entity.
	 *
	 * @param auxid auxiliary connection id
	 * @param xid transaction ID from received STATS.request
	 * @param meter_stats_array instance of type cofmeterstatsarray
	 * @param stats_flags flags for OpenFlow statistics messages, if any (default: 0)
	 */
	void
	send_meter_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofmeterstatsarray& meter_stats_array,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a METER-CONFIG-STATS.reply to a controller entity.
	 *
	 * @param auxid auxiliary connection id
	 * @param xid transaction ID from received STATS.request
	 * @param meter_config_array instance of type cofmeterconfigarray
	 * @param stats_flags flags for OpenFlow statistics messages, if any (default: 0)
	 */
	void
	send_meter_config_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofmeterconfigarray& meter_config_array,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a METER-FEATURES-STATS.reply to a controller entity.
	 *
	 * @param auxid auxiliary connection id
	 * @param xid transaction ID from received STATS.request
	 * @param meter_config_array instance of type cofmeterfeatures
	 * @param stats_flags flags for OpenFlow statistics messages, if any (default: 0)
	 */
	void
	send_meter_features_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofmeter_features& meter_features,
			uint16_t stats_flags = 0);

	/**
	 * @brief	Sends a PACKET-IN.message to a controller entity.
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
			const cauxid& auxid,
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
	 * @brief	Sends a BARRIER-reply to a controller entity.
	 *
	 * @param xid transaction ID from received BARRIER.request
	 */
	void
	send_barrier_reply(
			const cauxid& auxid,
			uint32_t xid);

	/**
	 * @brief	Sends an ERROR.message to a controller entity.
	 *
	 * @param xid transaction ID of request that generated this error message
	 * @param type one of OpenFlow's OFPET_* flags
	 * @param code one of OpenFlow's error codes
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	void
	send_error_message(
			const cauxid& auxid,
			uint32_t xid,
			uint16_t type,
			uint16_t code,
			uint8_t* data = NULL,
			size_t datalen = 0);

	/**
	 * @brief 	Sends an EXPERIMENTER.message to a controller entity.
	 *
	 * @param experimenter_id exp_id as assigned by ONF
	 * @param exp_type exp_type as defined by the ONF member
	 * @param body pointer to opaque experimenter message body (optional)
	 * @param bodylen length of body (optional)
	 * @result transaction ID assigned to this request
	 */
	void
	send_experimenter_message(
			const cauxid& auxid,
			uint32_t xid,
			uint32_t experimenter_id,
			uint32_t exp_type,
			uint8_t *body = NULL,
			size_t bodylen = 0);

	/**
	 * @brief	Sends a FLOW-REMOVED.message to a controller entity.
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
			const cauxid& auxid,
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
	 * @brief	Sends a PORT-STATUS.message to a controller entity.
	 *
	 * @param reason one of OpenFlow's OFPPR_* constants
	 * @param port rofl::openflow::cofport instance that changed its status
	 */
	void
	send_port_status_message(
			const cauxid& auxid,
			uint8_t reason,
			const rofl::openflow::cofport& port);

	/**
	 * @brief	Sends a QUEUE-GET-CONFIG.reply to a controller entity.
	 *
	 * @param xid transaction ID from QUEUE-GET-CONFIG.request
	 * @param portno OpenFlow number assigned to port
	 */
	void
	send_queue_get_config_reply(
			const cauxid& auxid,
			uint32_t xid,
			uint32_t portno,
			const rofl::openflow::cofpacket_queues& queues);


	/**
	 * @brief	Sends a ROLE.reply to a controller entity.
	 *
	 * @param xid transaction ID from associated ROLE.request
	 * @param role defined role from data path
	 * @param generation_id gen_id as defined by OpenFlow
	 */
	void
	send_role_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofrole& role);

	/**
	 * @brief	Sends a GET-ASYNC-CONFIG.reply to a controller entity.
	 *
	 * @param xid transaction ID from GET-CONFIG.request
	 */
	void
	send_get_async_config_reply(
			const cauxid& auxid,
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

protected:

	/**
	 * @brief Called upon establishment of a control connection within the control channel
	 */
	virtual void
	handle_conn_established(
			crofchan& chan,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
						<< " control connection established, auxid: " << auxid.str() << std::endl;

		state = STATE_ESTABLISHED;
		call_env().handle_conn_established(*this, auxid);

		if (auxid == rofl::cauxid(0)) {
			rofl::logging::info << "[rofl-common][crofctl] ctlid:0x" << ctlid.str()
							<< " OFP control channel established, " << chan.str() << std::endl;

			call_env().handle_ctl_attached(*this); // main connection
		}
	};

	/**
	 * @brief Called upon a peer initiated termination of a control connection within the control channel
	 */
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

	/**
	 * @brief Called in the event of a connection refused
	 */
	virtual void
	handle_conn_refused(
			crofchan& chan,
			const rofl::cauxid& auxid) {
		rofl::RwLock rwlock(conns_refused_rwlock, rofl::RwLock::RWLOCK_WRITE);
		conns_refused.push_back(auxid);
		push_on_eventqueue(EVENT_CONN_REFUSED);
	};

	/**
	 * @brief Called in the event of a connection failed (except refused)
	 */
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
			const cauxid& auxid)
	{ call_env().handle_write(*this, auxid); };

	virtual void
	recv_message(
			rofl::crofchan& chan,
			const cauxid& aux_id,
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

protected:

	virtual void
	ta_expired(
			rofl::ctransactions& tas,
			rofl::ctransaction& ta)
	{};

protected:

	virtual void
	handle_timeout(
			int opaque, void *data = (void*)0);

private:

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
			const cauxid& auxid,
			rofl::openflow::cofmsg_experimenter *pack);

	void
	error_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_error *pack);

	void
	features_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_features_request *pack);

	void
	get_config_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_get_config_request *pack);

	void
	set_config_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_set_config *pack);

	void
	packet_out_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_packet_out *pack);

	void
	flow_mod_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_flow_mod *pack);

	void
	group_mod_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_group_mod *pack);

	void
	port_mod_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_port_mod *pack);

	void
	table_mod_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_table_mod *pack);

	void
	meter_mod_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_meter_mod *pack);

	void
	stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_stats *pack);

	void
	desc_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_desc_stats_request *msg);

	void
	table_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_table_stats_request *msg);

	void
	port_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_port_stats_request *msg);

	void
	flow_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_flow_stats_request *msg);

	void
	aggregate_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_aggr_stats_request *msg);

	void
	queue_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_queue_stats_request *msg);

	void
	group_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_group_stats_request *msg);

	void
	group_desc_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_group_desc_stats_request *msg);

	void
	group_features_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_group_features_stats_request *msg);

	void
	meter_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_meter_stats_request *msg);

	void
	meter_config_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_meter_config_stats_request *msg);

	void
	meter_features_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_meter_features_stats_request *msg);

	void
	table_features_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_table_features_stats_request *msg);

	void
	port_desc_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_port_desc_stats_request *msg);

	void
	experimenter_stats_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_experimenter_stats_request *msg);

	void
	role_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_role_request *pack);

	void
	barrier_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_barrier_request *pack);

	void
	queue_get_config_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_queue_get_config_request *pack);

	void
	get_async_config_request_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_get_async_config_request *pack);

	void
	set_async_config_rcvd(
			const cauxid& auxid,
			rofl::openflow::cofmsg_set_async_config *pack);

private:

	/**
	 *
	 */
	void
	check_role();

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

protected:

	crofctl_env&
	call_env() {
		if (crofctl_env::rofctl_envs.find(env) == crofctl_env::rofctl_envs.end()) {
			throw eRofCtlNotFound();
		}
		return *env;
	};

private:

	/**
	 *
	 */
	void
	init_async_config_role_default_template();

private:

	static std::map<cctlid, crofctl*>
							rofctls;

	crofctl_env*			env;
	cctlid   				ctlid;
	bool					remove_on_channel_close;
	rofl::openflow::cofasync_config
							async_config_role_default_template;

	crofctl_state_t			state;
	crofchan				rofchan;
	ctransactions			transactions;
	rofl::openflow::cofasync_config
							async_config;
	rofl::openflow::cofrole	role;

	std::deque<enum crofctl_event_t>
							events;

	PthreadRwLock			conns_terminated_rwlock;
	std::list<rofl::cauxid> conns_terminated;
	PthreadRwLock			conns_refused_rwlock;
	std::list<rofl::cauxid> conns_refused;
	PthreadRwLock			conns_failed_rwlock;
	std::list<rofl::cauxid> conns_failed;

	std::bitset<32>			flags;
};

}; // end of namespace

#endif
