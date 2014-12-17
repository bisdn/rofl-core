/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CROFDPT_H
#define CROFDPT_H 1

#include <map>
#include <set>
#include <algorithm>
#include <inttypes.h>
#include <stdio.h>
#include <strings.h>


#include "rofl/common/croflexception.h"
#include "rofl/common/csocket.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/cdptid.h"
#include "rofl/common/openflow/cofports.h"
#include "rofl/common/openflow/coftables.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofflowmod.h"
#include "rofl/common/openflow/cofgroupmod.h"
#include "rofl/common/crofchan.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/openflow/cofasyncconfig.h"
#include "rofl/common/openflow/cofrole.h"
#include "rofl/common/openflow/cofmeterstats.h"
#include "rofl/common/openflow/cofmeterconfig.h"
#include "rofl/common/openflow/cofmeterfeatures.h"
#include "rofl/common/cauxid.h"
#include "rofl/common/cdpid.h"


namespace rofl {

/* error classes */
class eRofDptBase 					: public RoflException {};
class eRofDptNotFound 				: public eRofDptBase {};

class crofdpt;

class crofdpt_env {
	friend class crofdpt;
	static std::set<crofdpt_env*> rofdpt_envs;
public:

	crofdpt_env() {
		crofdpt_env::rofdpt_envs.insert(this);
	};

	virtual
	~crofdpt_env() {
		crofdpt_env::rofdpt_envs.erase(this);
	};

public:

	/**
	 *
	 */
	virtual void
	handle_dpt_attached(crofdpt& dpt)
	{};

	/**
	 *
	 */
	virtual void
	handle_dpt_detached(crofdpt& dpt)
	{};

	/**
	 * @brief Called upon establishment of a control connection within the control channel
	 */
	virtual void
	handle_conn_established(
			crofdpt& dpt,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief Called upon a peer initiated termination of a control connection within the control channel
	 */
	virtual void
	handle_conn_terminated(
			crofdpt& dpt,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief Called in the event of a connection refused
	 */
	virtual void
	handle_conn_refused(
			crofdpt& dpt,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief Called in the event of a connection failed (except refused)
	 */
	virtual void
	handle_conn_failed(
			crofdpt& dpt,
			const rofl::cauxid& auxid)
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
	handle_write(rofl::crofdpt& dpt, const rofl::cauxid& auxid) {};

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

	/**@}*/
};


/**
 * \class	rofl::openflow::cofdpt
 *
 * This class stores state for an attached data path element
 * including its ports (@see rofl::openflow::cofport). It is tightly bound to
 * class crofbase (@see crofbase). Created upon reception of an
 * OpenFlow HELLO message from the data path element, rofl::openflow::cofdpath
 * acquires all state by sending FEATURES-request, GET-CONFIG-request,
 * and TABLE-STATS-request. It also provides a number of convenience
 * methods for controlling the datapath, e.g. clearing all flowtable
 * or grouptable entries.
 *
 */
class crofdpt {
public: // static

	/**
	 * @brief	Returns reference to crofdpt instance identified by cdptid object.
	 *
	 * @param	dptid - cdptid handle
	 * @throw	eRofDptNotFound
	 * @return	Reference to crofdpt instance
	 */
	static crofdpt&
	get_dpt(const cdptid& dptid);

	/**
	 * @brief	Returns reference to crofdpt instance identified by OpenFlow dpid (uint64_t).
	 *
	 * @param	dpid - OpenFlow dpid
	 * @throw	eRofDptNotFound
	 * @return	Reference to crofdpt instance
	 */
	static crofdpt&
	get_dpt(const cdpid& dpid);

public:

	/**
	 * @brief 	Creates new crofdpt instance.
	 *
	 */
	crofdpt(crofdpt_env* env, const cdptid& dptid, bool remove_on_channel_close) :
		env(env), dptid(dptid), remove_on_channel_close(remove_on_channel_close), dpid(0) {
		crofdpt::rofdpts[dptid] = this;
		rofl::logging::debug << "[rofl-common][crofdpt] instance creating, dptid: "
				<< (unsigned long long)dptid.get_dptid() << std::endl;
	};


	/**
	 * @brief	Destroys crofdpt instance.
	 *
	 */
	virtual
	~crofdpt() {
		rofl::logging::debug << "[rofl-common][crofdpt] destroying instance, dptid: "
				<< (unsigned long long)dptid.get_dptid() << std::endl;
		crofdpt::rofdpts.erase(dptid);
	};




	/**
	 * @brief	Returns cdptid handle for this crofdpt instance.
	 *
	 * @return dpid
	 */
	const cdptid&
	get_dptid() const { return dptid; };


	/**
	 * @brief	Returns OpenFlow dpid for this crofdpt instance.
	 *
	 * @return dpid
	 */
	const cdpid&
	get_dpid() const { return dpid; };

public:

	/**
	 *
	 */
	virtual std::list<rofl::cauxid>
	get_conn_index() const = 0;

	/**
	 *
	 */
	virtual void
	connect(
			const rofl::cauxid& auxid,
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& socket_params) = 0;

	/**
	 *
	 */
	virtual void
	disconnect(
			const rofl::cauxid& auxid) = 0;

	/**
	 *
	 */
	virtual void
	reconnect(
			const rofl::cauxid& auxid) = 0;

	/**
	 *
	 */
	virtual void
	add_connection(
			crofconn* conn) = 0;

public:

	/**
	 * @brief	Returns true, when the control handshake (HELLO) has been completed.
	 */
	virtual bool
	is_established() const = 0;

	/**
	 * @brief 	Returns the OpenFlow protocol version used for this control connection.
	 *
	 * @return OpenFlow version used for this control connection
	 */
	virtual uint8_t
	get_version_negotiated() const = 0;

	/**
	 *
	 * @return
	 */
	virtual const rofl::openflow::cofhello_elem_versionbitmap&
	get_versions_available() const = 0;

	/**
	 * @brief	Returns caddress of connected remote entity.
	 *
	 * @return caddress object obtained from this->socket
	 */
	virtual caddress
	get_peer_addr(
			const rofl::cauxid& auxid) const = 0;

	/**
	 *
	 */
	uint32_t
	get_next_idle_group_id() {
		uint32_t group_id = 1;
		while (groupids.find(group_id) != groupids.end()) {
			group_id++;
		}
		groupids.insert(group_id);
		return group_id;
	};

	/**
	 *
	 */
	void
	release_group_id(uint32_t group_id) {
		groupids.erase(group_id);
	};

	/**
	 *
	 */
	void
	clear_group_ids() {
		groupids.clear();
	};


public:

	/**
	 * @name	Data path property related methods
	 *
	 * These methods provide an interface for querying various properties of the attached data path element.
	 */

	/**@{*/


	/**
	 * @brief	Returns the data path element's hardware address.
	 *
	 * @return hwaddr
	 */
	virtual cmacaddr
	get_hwaddr() const = 0;


	/**
	 * @brief	Returns the data path element's number of buffers for storing data packets.
	 *
	 * @return n_buffers
	 */
	virtual uint32_t
	get_n_buffers() const = 0;


	/**
	 * @brief	Returns the data path element's number of tables in the OpenFlow pipeline.
	 *
	 * @return n_tables
	 */
	virtual uint8_t
	get_n_tables() const = 0;


	/**
	 * @brief	Returns the data path element's capabilities.
	 *
	 * @return capabilities
	 */
	virtual uint32_t
	get_capabilities() const = 0;


	/**
	 * @brief	Returns the data path element's configuration.
	 *
	 * @return config
	 */
	virtual uint16_t
	get_config() const = 0;


	/**
	 * @brief	Returns the data path element's current miss_send_len parameter.
	 *
	 * @return miss_send_len
	 */
	virtual uint16_t
	get_miss_send_len() const = 0;

	/**
	 * @brief	Returns reference to the data path element's rofl::openflow::cofport list.
	 *
	 * @return ports
	 */
	virtual rofl::openflow::cofports&
	set_ports() = 0;


	/**
	 * @brief	Returns reference to the data path element's rofl::openflow::cofport list.
	 *
	 * @return ports
	 */
	virtual rofl::openflow::cofports const&
	get_ports() const = 0;


	/**
	 * @brief	Returns reference to the data path element's rofl::openflow::coftable_stats_reply list.
	 *
	 * @return tables
	 */
	virtual rofl::openflow::coftables&
	set_tables() = 0;


	/**
	 * @brief	Returns reference to the data path element's rofl::openflow::coftable_stats_reply list.
	 *
	 * @return tables
	 */
	virtual rofl::openflow::coftables const&
	get_tables() const = 0;


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
	flow_mod_reset() = 0;


	/**
	 * @brief	Removes all grouptable entries from the attached datapath element.
	 *
	 * Sends a GroupMod-Delete message to the attached datapath element for removing
	 * all grouptable entries.
	 */
	virtual void
	group_mod_reset() = 0;

	/**
	 * @brief	Drops packet identified by buffer-id
	 *
	 * Drops a packet stored on the data path in the buffer identified by buffer-id
	 */
	virtual void
	drop_buffer(const rofl::cauxid& auxid, uint32_t buffer_id, uint32_t inport = rofl::openflow::OFPP_CONTROLLER) {
		rofl::openflow::cofactions actions(get_version_negotiated());
		send_packet_out_message(auxid, buffer_id, inport, actions, NULL, 0);
	};

	/**@}*/

public:


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
	 * @return transaction ID assigned to this request
	 */
	virtual uint32_t
	send_features_request(
			const cauxid& aux_id) = 0;

	/**
	 * @brief	Sends a GET-CONFIG.request to a data path element.
	 *
	 * @return transaction ID assigned to this request
	 */
	virtual uint32_t
	send_get_config_request(
			const cauxid& aux_id) = 0;

	/**
	 * @brief	Sends a TABLE-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this TABLE-STATS.request
	 */
	virtual uint32_t
	send_table_features_stats_request(
			const cauxid& aux_id,
			uint16_t stats_flags) = 0;

	/**
	 * @brief	Sends a STATS.request to a data path element.
	 *
	 * @param stats_type one of the OFPMP_* constants
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param body body of STATS.request
	 * @param bodylen length of STATS.request body
	 * @return transaction ID for this STATS.request
	 */
	virtual uint32_t
	send_stats_request(
			const cauxid& aux_id,
			uint16_t stats_type,
			uint16_t stats_flags,
			uint8_t *body = NULL,
			size_t bodylen = 0) = 0;

	/**
	 * @brief	Sends a DESC-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this DESC-STATS.request
	 */
	virtual uint32_t
	send_desc_stats_request(
			const cauxid& aux_id,
			uint16_t stats_flags) = 0;

	/**
	 * @brief	Sends a FLOW-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param flow_stats_request body of a FLOW-STATS.request
	 * @return transaction ID for this FLOW-STATS.request
	 */
	virtual uint32_t
	send_flow_stats_request(
			const cauxid& aux_id,
			uint16_t stats_flags,
			rofl::openflow::cofflow_stats_request const& flow_stats_request) = 0;

	/**
	 * @brief	Sends a AGGREGATE-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param aggr_stats_request body of an AGGREGATE-STATS.request
	 * @return transaction ID for this AGGREGATE-STATS.request
	 */
	virtual uint32_t
	send_aggr_stats_request(
			const cauxid& aux_id,
			uint16_t flags,
			rofl::openflow::cofaggr_stats_request const& aggr_stats_request) = 0;


	/**
	 * @brief	Sends a TABLE-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this TABLE-STATS.request
	 */
	virtual uint32_t
	send_table_stats_request(
			const cauxid& aux_id,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a FLOW-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param port_stats_request body of a PORT-STATS.request
	 * @return transaction ID for this FLOW-STATS.request
	 */
	virtual uint32_t
	send_port_stats_request(
			const cauxid& aux_id,
			uint16_t stats_flags,
			rofl::openflow::cofport_stats_request const& port_stats_request) = 0;

	/**
	 * @brief	Sends a QUEUE-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param queue_stats_request body of a QUEUE-STATS.request
	 * @return transaction ID for this QUEUE-STATS.request
	 */
	virtual uint32_t
	send_queue_stats_request(
			const cauxid& aux_id,
			uint16_t stats_flags,
			rofl::openflow::cofqueue_stats_request const& queue_stats_request) = 0;

	/**
	 * @brief	Sends a GROUP-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param queue_stats_request body of a GROUP-STATS.request
	 * @return transaction ID for this GROUP-STATS.request
	 */
	virtual uint32_t
	send_group_stats_request(
			const cauxid& aux_id,
			uint16_t stats_flags,
			rofl::openflow::cofgroup_stats_request const& group_stats_request) = 0;

	/**
	 * @brief	Sends a GROUP-DESC-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this AGGREGATE-STATS.request
	 */
	virtual uint32_t
	send_group_desc_stats_request(
			const cauxid& aux_id,
			uint16_t flags = 0) = 0;

	/**
	 * @brief	Sends a GROUP-FEATURES-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this GROUP-FEATURES-STATS.request
	 */
	virtual uint32_t
	send_group_features_stats_request(
			const cauxid& aux_id,
			uint16_t flags) = 0;

	/**
	 * @brief	Sends a PORT-DESC-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this PORT-DESC-STATS.request
	 */
	virtual uint32_t
	send_port_desc_stats_request(
			const cauxid& aux_id,
			uint16_t flags) = 0;

	/**
	 * @brief	Sends an EXPERIMENTER-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param exp_id experimenter ID
	 * @param exp_type user defined type
	 * @param body user defined body
	 * @return transaction ID for this EXPERIMENTER-STATS.request
	 */
	virtual uint32_t
	send_experimenter_stats_request(
			const cauxid& aux_id,
			uint16_t stats_flags,
			uint32_t exp_id,
			uint32_t exp_type,
			cmemory const& body) = 0;

	/**
	 * @brief	Sends a METER-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param mstats meter multipart request
	 * @return transaction ID for this METER-STATS.request
	 */
	virtual uint32_t
	send_meter_stats_request(
			const cauxid& aux_id,
			uint16_t stats_flags,
			const rofl::openflow::cofmeter_stats_request& mstats) = 0;

	/**
	 * @brief	Sends a METER-CONFIG-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @param mstats meter multipart request
	 * @return transaction ID for this METER-CONFIG-STATS.request
	 */
	virtual uint32_t
	send_meter_config_stats_request(
			const cauxid& aux_id,
			uint16_t stats_flags,
			const rofl::openflow::cofmeter_config_request& mstats) = 0;

	/**
	 * @brief	Sends a METER-FEATURES-STATS.request to a data path element.
	 *
	 * @param stats_flags a bitfield with OFPSF_REQ_* flags
	 * @return transaction ID for this METER-FEATURES-STATS.request
	 */
	virtual uint32_t
	send_meter_features_stats_request(
			const cauxid& aux_id,
			uint16_t stats_flags) = 0;

	/**
	 * @brief	Sends a PACKET-OUT.message to a data path element.
	 *
	 * @param buffer_id buffer ID assigned by datapath (-1 if none) in host byte order
	 * @param in_port packet’s in-port (OFPP_NONE if none) in host byte order
	 * @param aclist OpenFlow ActionList
	 * @param data data packet to be sent out (optional)
	 * @param datalen length of data packet (optional)
	 * @result transaction ID assigned to this request
	 */
	virtual uint32_t
	send_packet_out_message(
			const cauxid& aux_id,
			uint32_t buffer_id,
			uint32_t in_port,
			rofl::openflow::cofactions& aclist,
			uint8_t *data = NULL,
			size_t datalen = 0) = 0;

	/**
	 * @brief	Sends a BARRIER.request to a data path element.
	 *
	 * @result transaction ID assigned to this request
	 */
	virtual uint32_t
	send_barrier_request(
			const cauxid& aux_id) = 0;

	/**
	 * @brief	Sends a ROLE.request to a data path element.
	 *
	 * @param role role as defined by OpenFlow
	 * @param generation_id gen_id as defined by OpenFlow
	 */
	virtual uint32_t
	send_role_request(
			const cauxid& aux_id,
			rofl::openflow::cofrole const& role) = 0;

	/**
	 * @brief 	Sends a FLOW-MOD.message to a data path element.
	 *
	 * @param flowentry FlowMod entry
	 */
	virtual uint32_t
	send_flow_mod_message(
			const cauxid& aux_id,
			rofl::openflow::cofflowmod const& flowentry) = 0;

	/**
	 * @brief 	Sends a GROUP-MOD.message to a data path element.
	 *
	 * @param groupentry GroupMod entry
	 */
	virtual uint32_t
	send_group_mod_message(
			const cauxid& aux_id,
			rofl::openflow::cofgroupmod const& groupentry) = 0;

	/**
	 * @brief	Sends a TABLE-MOD.message to a data path element.
	 *
	 * @param table_id ID of table to be reconfigured
	 * @param config new configuration for table
	 */
	virtual uint32_t
	send_table_mod_message(
			const cauxid& aux_id,
			uint8_t table_id,
			uint32_t config) = 0;

	/**
	 * @brief	Sends a PORT-MOD.message to a data path element.
	 *
	 * @param port_no number of port to be updated
	 * @param hwaddr MAC address assigned to port
	 * @param config OpenFlow config parameter
	 * * @param mask OpenFlow mask parameter
	 * * @param advertise OpenFlow advertise parameter
	 */
	virtual uint32_t
	send_port_mod_message(
			const cauxid& aux_id,
			uint32_t port_no,
			cmacaddr const& hwaddr,
			uint32_t config,
			uint32_t mask,
			uint32_t advertise) = 0;

	/**
	 * @brief	Sends a SET-CONFIG.message to a data path element.
	 *
	 * @param flags field of OpenFlow's OFPC_* flags
	 * @param miss_send_len OpenFlow' miss_send_len parameter
	 */
	virtual uint32_t
	send_set_config_message(
			const cauxid& aux_id,
			uint16_t flags,
			uint16_t miss_send_len) = 0;

	/**
	 * @brief	Sends a QUEUE-GET-CONFIG.request to a data path element.
	 *
	 * @param port port to be queried. Should refer to a valid physical port (i.e. < OFPP_MAX)
	 * @result transaction ID assigned to this request
	 */
	virtual uint32_t
	send_queue_get_config_request(
			const cauxid& aux_id,
			uint32_t port) = 0;

	/**
	 * @brief	Sends a GET-ASYNC-CONFIG.request to a data path element.
	 *
	 * @return transaction ID assigned to this request
	 */
	virtual uint32_t
	send_get_async_config_request(
			const cauxid& aux_id) = 0;

	/**
	 * @brief	Sends a SET-ASYNC-CONFIG.message to a data path element.
	 *
	 */
	virtual uint32_t
	send_set_async_config_message(
			const cauxid& aux_id,
			rofl::openflow::cofasync_config const& async_config) = 0;

	/**
	 * @brief	Sends a METER-MOD.message to a data path element.
	 *
	 * @param table_id ID of table to be reconfigured
	 * @param config new configuration for table
	 */
	virtual uint32_t
	send_meter_mod_message(
			const cauxid& auxid,
			uint16_t command,
			uint16_t flags,
			uint32_t meter_id,
			const rofl::openflow::cofmeter_bands& meter_bands) = 0;

	/**
	 * @brief	Sends an ERROR.message to a data path element.
	 *
	 * These messages are used for failed HELLO negotiations and
	 * experimental extensions.
	 *
	 * @param xid transaction ID of reply/notification that generated this error message
	 * @param type one of OpenFlow's OFPET_* flags
	 * @param code one of OpenFlow's error codes
	 * @param data first (at least 64) bytes of failed reply/notification
	 * @param datalen length of failed reply/notification appended to error message
	 */
	virtual void
	send_error_message(
			const cauxid& aux_id,
			uint32_t xid,
			uint16_t type,
			uint16_t code,
			uint8_t* data = NULL,
			size_t datalen = 0) = 0;

	/**
	 * @brief 	Sends an EXPERIMENTER.message to a data path element.
	 *
	 * @param experimenter_id exp_id as assigned by ONF
	 * @param exp_type exp_type as defined by the ONF member
	 * @param body pointer to opaque experimenter message body (optional)
	 * @param bodylen length of body (optional)
	 * @result transaction ID assigned to this request
	 */
	virtual uint32_t
	send_experimenter_message(
			const cauxid& aux_id,
			uint32_t experimenter_id,
			uint32_t exp_type,
			uint8_t *body = NULL,
			size_t bodylen = 0) = 0;

	/**@}*/

public:

	/**
	 *
	 */
	class crofdpt_find_by_dptid {
		cdptid dptid;
	public:
		crofdpt_find_by_dptid(const cdptid& dptid) : dptid(dptid) {};
		bool operator() (const crofdpt* rofdpt) {
			return (rofdpt->get_dptid() == dptid);
		};
	};

	/**
	 *
	 */
	class crofdpt_find_by_dpid {
		uint64_t dpid;
	public:
		crofdpt_find_by_dpid(uint64_t dpid) : dpid(dpid) {};
		bool operator() (const std::pair<cdptid, crofdpt*>& p) {
			return (p.second->get_dpid().get_uint64_t() == dpid);
		};
	};

	bool
	remove_upon_channel_termination() const
	{ return remove_on_channel_close; };

protected:

	/**
	 *
	 */
	void
	set_dpid(
			const cdpid& dpid) {
		this->dpid = dpid;
	};

protected:

	crofdpt_env&
	call_env() {
		if (crofdpt_env::rofdpt_envs.find(env) == crofdpt_env::rofdpt_envs.end()) {
			throw eRofDptNotFound();
		}
		return *env;
	};

private:

	static uint64_t 		next_dptid;
	static std::map<cdptid, crofdpt*>
							rofdpts;

	crofdpt_env*			env;
	cdptid   				dptid;			// handle for this crofdpt instance
	bool					remove_on_channel_close;
	cdpid 					dpid;			// datapath id
	std::set<uint32_t>		groupids;		// allocated groupids on datapath
};

}; // end of namespace

#endif
