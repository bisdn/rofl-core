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
#include <bitset>

#include "rofl/common/ciosrv.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/logging.h"
#include "rofl/common/crofchan.h"
#include "rofl/common/ctransactions.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/csocket.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/cdptid.h"
#include "rofl/common/cauxid.h"
#include "rofl/common/cdpid.h"

#include "rofl/common/openflow/cofports.h"
#include "rofl/common/openflow/coftables.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofflowmod.h"
#include "rofl/common/openflow/cofgroupmod.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/openflow/cofasyncconfig.h"
#include "rofl/common/openflow/cofrole.h"
#include "rofl/common/openflow/cofmeterstats.h"
#include "rofl/common/openflow/cofmeterconfig.h"
#include "rofl/common/openflow/cofmeterfeatures.h"
#include "rofl/common/openflow/cofpacketqueue.h"
#include "rofl/common/openflow/cofmeterbands.h"

namespace rofl {

/* error classes */
class eRofDptBase      : public RoflException {
public:
	eRofDptBase(const std::string& __arg) : RoflException(__arg) {};
};
class eRofDptNotFound  : public eRofDptBase {
public:
	eRofDptNotFound(const std::string& __arg) : eRofDptBase(__arg) {};
};

class crofdpt;

class crofdpt_env {
	friend class crofdpt;
	static std::set<crofdpt_env*> rofdpt_envs;
public:

	crofdpt_env()
	{ crofdpt_env::rofdpt_envs.insert(this); };

	virtual
	~crofdpt_env()
	{ crofdpt_env::rofdpt_envs.erase(this); };

protected:

	/**
	 * @name 	Event handlers for management notifications for datapath elements
	 *
	 * Overwrite any of these methods for receiving datapath related event notifications.
	 */

	/**@{*/

	/**
	 * @brief	Called after establishing the associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been established, i.e., its main connection has been accepted by the remote site.
	 *
	 * @param dpt datapath instance
	 */
	virtual void
	handle_chan_established(
			rofl::crofdpt& dpt)
	{};

	/**
	 * @brief	Called after termination of associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been terminated, i.e., its main connection has been closed from the
	 * remote site.
	 *
	 * @param dpt datapath instance
	 */
	virtual void
	handle_chan_terminated(
			rofl::crofdpt& dpt)
	{};

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been established.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_established(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been terminated by the peer entity.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_terminated(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief 	Called when an attempt to establish a control connection has been refused.
	 *
	 * This event occurs when the C-library's connect() system call fails
	 * with the ECONNREFUSED error code. This indicates typically a problem on
	 * the remote site.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_refused(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief 	Called when an attempt to establish a control connection has been failed.
	 *
	 * This event occurs when some failure occures while calling the underlying
	 * C-library connect() system call, e.g., no route to destination, etc. This may
	 * indicate a local configuration problem inside or outside of the application.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_failed(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{};

	/**
	 * @brief	Called when a congestion situation on the control connection has been solved.
	 *
	 * A control channel may face congestion situations when insufficient bandwidth
	 * on the underlying IP path or some backpressuring by the remote site requires
	 * to throttle the overall message transmission rate on a control connection.
	 * A congestion situation is indicated by the return values obtained from the various
	 * send-methods defined within rofl::crofdpt. A solved congestion situation is
	 * indicated by calling this method. Note that ROFL will store OpenFlow messages
	 * even under congestion, thus filling up its internal buffers until no further
	 * memory is available for doing so. It is up to the application designer to
	 * throttle transmission of further messages according to the channel capacity.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_writable(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid)
	{};

	/**@}*/

protected:

	/**
	 * @name 	Event handlers for OpenFlow message notifications received from datapath elements
	 *
	 * Overwrite any of these methods for receiving messages from the attached datapath element.
	 * Once this method terminates, rofl will destroy the message object. If you want to save
	 * a message or parts of it, you must create a local copy. All messages are internally
	 * allocated on the heap and both allocation and deallocation is handled by rofl-common.
	 * Do not delete any of these messages or its internals.
	 */

	/**@{*/

	/**
	 * @brief	OpenFlow Features-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_features_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Features-Reply message.
	 *
	 * No Features-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_features_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Get-Config-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_config_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_config_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Get-Config-Reply message.
	 *
	 * No Get-Config-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_get_config_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Stats-Reply message.
	 *
	 * No Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 * @param stats_type statistics message subtype
	 */
	virtual void
	handle_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid,
			uint8_t stats_type)
	{};

	/**
	 * @brief	OpenFlow Desc-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_desc_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_desc_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Desc-Stats-Reply message.
	 *
	 * No Desc-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_desc_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Table-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Table-Stats-Reply message.
	 *
	 * No Table-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_table_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Port-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Port-Stats-Reply message.
	 *
	 * No Port-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_port_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Flow-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Flow-Stats-Reply message.
	 *
	 * No Flow-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_flow_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Aggregate-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_aggregate_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_aggr_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Aggregate-Stats-Reply message.
	 *
	 * No Aggregate-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_aggregate_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Queue-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Queue-Stats-Reply message.
	 *
	 * No Queue-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_queue_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Group-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Group-Stats-Reply message.
	 *
	 * No Group-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_group_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Group-Desc-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_desc_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_desc_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Group-Desc-Stats-Reply message.
	 *
	 * No Group-Desc-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_group_desc_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Group-Features-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_features_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_features_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Group-Features-Stats-Reply message.
	 *
	 * No Group-Features-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_group_features_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Meter-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Meter-Stats-Reply message.
	 *
	 * No Meter-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_meter_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Meter-Config-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_config_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_config_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Meter-Config-Stats-Reply message.
	 *
	 * No Meter-Config-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_meter_config_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Meter-Features-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_features_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_features_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Meter-Features-Stats-Reply message.
	 *
	 * No Meter-Features-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_meter_features_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Table-Features-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_features_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_features_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Table-Features-Stats-Reply message.
	 *
	 * No Table-Features-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_table_features_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Port-Desc-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_desc_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_desc_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Port-Desc-Stats-Reply message.
	 *
	 * No Port-Desc-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_port_desc_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Experimenter-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Experimenter-Stats-Reply message.
	 *
	 * No Experimenter-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_experimenter_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Packet-In message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_packet_in(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_packet_in& msg)
	{};

	/**
	 * @brief	OpenFlow Barrier-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_barrier_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_barrier_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Barrier-Reply message.
	 *
	 * No Barrier-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_barrier_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Flow-Removed message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_removed(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_removed& msg)
	{};

	/**
	 * @brief	OpenFlow Port-Status-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_status(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_status& msg)
	{};

	/**
	 * @brief	OpenFlow Queue-Get-Config-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_get_config_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_get_config_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Table-Stats-Reply message.
	 *
	 * No Table-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_queue_get_config_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Error message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_error_message(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_error& msg)
	{};

	/**
	 * @brief	OpenFlow Experimenter message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_message(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Experimenter message.
	 *
	 * No Experimenter message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_experimenter_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Role-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_role_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_role_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Role-Reply message.
	 *
	 * No Role-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_role_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Get-Async-Config-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_async_config_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_async_config_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Get-Async-Config-Reply message.
	 *
	 * No Get-Async-Config-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_get_async_config_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**@}*/
};


/**
 * @brief	Class representing a remote datapath element
 *
 * This class encapsulates properties of a single remote datapath element
 * including the OpenFlow control channel, its lists of ports and tables,
 * and general configuration. Its public API offers methods to manage
 * the OpenFlow control channel, i.e. CRUD methods for individual control
 * connections. Once the OpenFlow control channel has been established,
 * rofl::crofdpt will query the datapath element for its internal configuration
 * (ports, tables and general configuration) and exposes these data to any
 * higher logic. Note that no periodic updates on these data is done by
 * this instance and re-querying the datapath must be initiated by higher
 * logic.
 *
 * rofl::crofdpt expects an instance of class rofl::crofdpt_env as surrounding
 * environment and sends various notifications via this interface. Class
 * rofl::crofbase implements this interface and may be used as base class
 * for advanced applications. However, you may implement rofl::crofdpt_env
 * directly as well.
 *
 */
class crofdpt :
		public rofl::crofchan_env,
		public rofl::ctransactions_env,
		public ciosrv
{
private: // data structures

	enum crofdpt_timer_t {
		TIMER_RUN_ENGINE                            = 0,
	};

	enum crofdpt_state_t {
		STATE_INIT                                  = 0,
		STATE_DISCONNECTED                          = 1,
		STATE_CONNECTED                             = 2,
		STATE_FEATURES_RCVD                         = 3,
		STATE_GET_CONFIG_RCVD                       = 4,
		STATE_TABLE_FEATURES_RCVD                   = 5,
		STATE_ESTABLISHED                           = 6,
	};

	enum crofdpt_event_t {
		EVENT_NONE                                  = 0,
		EVENT_DISCONNECTED                          = 1,
		EVENT_CONNECTED                             = 2,
		EVENT_CONN_TERMINATED                       = 3,
		EVENT_CONN_REFUSED                          = 4,
		EVENT_CONN_FAILED                           = 5,
		EVENT_FEATURES_REPLY_RCVD                   = 6,
		EVENT_FEATURES_REQUEST_EXPIRED              = 7,
		EVENT_GET_CONFIG_REPLY_RCVD                 = 8,
		EVENT_GET_CONFIG_REQUEST_EXPIRED            = 9,
		EVENT_TABLE_STATS_REPLY_RCVD                = 10,
		EVENT_TABLE_STATS_REQUEST_EXPIRED           = 11,
		EVENT_TABLE_FEATURES_STATS_REPLY_RCVD       = 12,
		EVENT_TABLE_FEATURES_STATS_REQUEST_EXPIRED  = 13,
		EVENT_PORT_DESC_STATS_REPLY_RCVD            = 14,
		EVENT_PORT_DESC_STATS_REQUEST_EXPIRED       = 15,
	};

	enum crofdpt_flag_t {
		FLAG_ENGINE_IS_RUNNING                      = (1 << 0),
	};

public:

	/**
	 * @brief	Returns reference to rofl::crofdpt instance identified by rofl-common's internal identifier.
	 *
	 * @param dptid rofl-common's internal datapath identifier
	 * @throw eRofDptNotFound when no object matches the datapath identifier
	 * @return reference to rofl::crofdpt instance for given identifier
	 */
	static rofl::crofdpt&
	get_dpt(
			const rofl::cdptid& dptid);

	/**
	 * @brief	Returns reference to rofl::crofdpt instance identified by OpenFlow's datapath identifier.
	 *
	 * @param dpid OpenFlow datapath identifier
	 * @throw eRofDptNotFound when no object matches the datapath identifier
	 * @return reference to rofl::crofdpt instance for given identifier
	 */
	static rofl::crofdpt&
	get_dpt(
			const rofl::cdpid& dpid);

	/**
	 * @brief 	crofdpt constructor
	 *
	 * @param env pointer to rofl::crofdpt_env instance defining the environment for this object
	 * @param remove_on_channel_close when set to true, this indicates to remove this object after the control channel has been terminated
	 * @param versionbitmap OpenFlow version bitmap
	 * @param dpid OpenFlow datapath element identifier (optional)
	 */
	crofdpt(
			rofl::crofdpt_env* env,
			const rofl::cdptid& dptid,
			bool remove_on_channel_close,
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
			const rofl::cdpid& dpid = rofl::cdpid(0)) :
				env(env),
				dptid(dptid),
				rofchan(this, versionbitmap),
				transactions(this),
				remove_on_channel_close(remove_on_channel_close),
				dpid(dpid),
				hwaddr(cmacaddr("00:00:00:00:00:00")),
				n_buffers(0),
				n_tables(0),
				capabilities(0),
				config(0),
				miss_send_len(0),
				state(STATE_INIT) {
		crofdpt::rofdpts[dptid] = this;
		rofl::logging::debug << "[rofl-common][crofdpt] "
				<< "instance created, dptid: " << dptid.str() << std::endl;
	};

	/**
	 * @brief	crofdpt destructor
	 *
	 * Closes all control connections and does a general clean-up.
	 */
	virtual
	~crofdpt() {
		rofl::logging::debug << "[rofl-common][crofdpt] "
				<< "instance destroyed, dptid: " << dptid.str() << std::endl;
		crofdpt::rofdpts.erase(dptid);
		rofchan.close();
		transactions.clear();
	};

	/**
	 * @brief	Returns rofl-common's internal rofl::cdptid identifier for this instance
	 *
	 * @return internal datapath element identifier (not DPID)
	 */
	const rofl::cdptid&
	get_dptid() const
	{ return dptid; };

public:

	/**
	 * @name	Methods for connection management
	 *
	 * This is a group of methods for typical CRUD like operations on control
	 * connections for the OpenFlow control channel. You may create an arbitrary
	 * number of control connections to a datapath element in addition to those
	 * created by the datapath element (as long as there is a listening socket
	 * implemented on the datapath side). Control connections may be closed or
	 * reconnected.
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
	 * datapath element with the given control connection identifier
	 *
	 * An already existing control connection with the specified control
	 * connection identifier is replaced by this new control connection
	 * instance. If the main control connection (auxid: 0) is reconnected,
	 * this rofl::crofdpt instance drops all collected state including
	 * pending transactions. You may select any arbitrary control connection
	 * identifier. However, care must be taken for the main connection (auxid: 0):
	 * (Re-)Connecting the main connection leads to an implicit termination of
	 * all existing control connections in OpenFlow.
	 *
	 * @param auxid control connection identifier
	 * @param socket_type one of the socket types defined in @see rofl::csocket
	 * @param socket_params a set of parameters for the selected socket type
	 */
	void
	connect(
			const rofl::cauxid& auxid,
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& socket_params) {
		if (rofl::cauxid(0) == auxid) {
			rofchan.close();
			transactions.clear();
			tables.clear();
			ports.clear();
		}
		rofchan.add_conn(auxid, socket_type, socket_params);
	};

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
			rofl::cauxid auxid) { // make a copy here, rather than using a const-reference
		rofchan.drop_conn(auxid);
		call_env().handle_conn_terminated(*this, auxid);
		if (rofl::cauxid(0) == auxid) {
			call_env().handle_chan_terminated(*this);
		}
	};

	/**
	 * @brief	Add an existing rofl::crofconn instance created on heap to this object.
	 *
	 * This method is used for attaching an already existing rofl::crofconn instance
	 * to this rofl::crofdpt instance. Do not call this method, unless you know what
	 * you are doing.
	 *
	 * @param conn pointer to rofl::crofconn instance allocated on heap
	 */
	void
	add_connection(
			crofconn* conn)  {
		if (NULL == conn) {
			return;
		}
		if (rofl::cauxid(0) == conn->get_aux_id()) {
			dpid = rofl::cdpid(conn->get_dpid());
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
	 *
	 * @return
	 */
	const rofl::openflow::cofhello_elem_versionbitmap&
	get_versions_available() const
	{ return rofchan.get_versionbitmap(); };

	/**
	 * @brief	Returns true, when this instance should be destroyed when its crofchan has closed
	 */
	bool
	remove_on_channel_termination() const
	{ return remove_on_channel_close; };

	/**
	 * @brief	Returns caddress of connected remote entity.
	 *
	 * @return caddress object obtained from this->socket
	 */
	caddress
	get_peer_addr(
			const rofl::cauxid& auxid) const
	{ return rofchan.get_conn(auxid).get_rofsocket().get_socket().get_raddr(); };

	/**@}*/

public:

	/**
	 * @name	Methods granting access to the datapath element's basic properties
	 */

	/**@{*/

	/**
	 * @brief	Returns OpenFlow datapath identifier for this instance
	 *
	 * @return OpenFlow datapath identifier
	 */
	const cdpid&
	get_dpid() const
	{ return dpid; };

	/**
	 * @brief	Returns the datapath element's hardware address.
	 *
	 * @return hwaddr
	 */
	const rofl::caddress_ll&
	get_hwaddr() const
	{ return hwaddr; };

	/**
	 * @brief	Returns the datapath element's number of buffers for storing data packets.
	 *
	 * @return n_buffers
	 */
	uint32_t
	get_n_buffers() const
	{ return n_buffers; };

	/**
	 * @brief	Returns the datapath element's number of tables in the OpenFlow pipeline.
	 *
	 * @return n_tables
	 */
	uint8_t
	get_n_tables() const
	{ return n_tables; };

	/**
	 * @brief	Returns the datapath element's capabilities.
	 *
	 * @return capabilities
	 */
	uint32_t
	get_capabilities() const
	{ return capabilities; };

	/**
	 * @brief	Returns the datapath element's configuration.
	 *
	 * @return config
	 */
	uint16_t
	get_config() const
	{ return config; };

	/**
	 * @brief	Returns the datapath element's current miss_send_len parameter.
	 *
	 * @return miss_send_len
	 */
	uint16_t
	get_miss_send_len() const
	{ return miss_send_len; };

	/**
	 * @brief	Returns const reference to the datapath element's port list.
	 *
	 * The ports list is updated by this rofl::crofdpt instance whenever a
	 * Port-Status or Port-Desc-Stats-Reply message is received. However,
	 * rofl-common won't send periodic Port-Desc-Stats-Request messages, so
	 * it is up to the higher layer logic to emit such messages.
	 *
	 * @return const reference to ports list
	 */
	const rofl::openflow::cofports&
	get_ports() const
	{ return ports; };

	/**
	 * @brief	Returns const reference to the datapath element's tables list.
	 *
	 * The tables list is updated by this rofl::crofdpt instance whenever a
	 * Table-Stats-Reply (OFP v1.2) or Table-Features-Stats-Reply (OFP v1.3)
	 * message is received. However, rofl-common won't send periodic requests, so
	 * it is up to the higher layer logic to emit such messages.
	 *
	 * @return const reference to tables list
	 */
	const rofl::openflow::coftables&
	get_tables() const
	{ return tables; };

	/**@}*/

public:

	/**
	 * @name	Methods for group table entry identifier management
	 *
	 * This group of methods provides simple functionality for assigning
	 * group table entry identifiers for various higher logic entities,
	 * e.g., for creating new group table entries. This is a simple storage
	 * for uint32_t values and does no pre-checks, whether a group table
	 * identifier is already in use or can be actually used at all on the
	 * datapath element.
	 */

	/**@{*/

	/**
	 * @brief	Returns the next idle group table identifier.
	 *
	 * @todo There is no overflow checking implemented yet.
	 * @return group table identifier
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
	 * @brief	Releases a previously allocated group table identifier.
	 *
	 * @param group_id group table identifier to be returned to pool of idle identifiers
	 */
	void
	release_group_id(uint32_t group_id)
	{ groupids.erase(group_id); };

	/**
	 * @brief	Resets the pool of all previously allocated group table identifiers.
	 */
	void
	clear_group_ids()
	{ groupids.clear(); };

	/**@}*/

public:

	/**
	 * @name	Methods for purging and resetting a datapath element
	 */

	/**@{*/

	/**
	 * @brief	Removes all flow-table entries from the attached datapath element.
	 *
	 * Sends a Flow-Mod-Delete message to the attached datapath element for removing
	 * all flow-table entries.
	 */
	void
	flow_mod_reset();

	/**
	 * @brief	Removes all group-table entries from the attached datapath element.
	 *
	 * Sends a Group-Mod-Delete message to the attached datapath element for removing
	 * all group-table entries.
	 */
	void
	group_mod_reset();

	/**
	 * @brief	Drops packet identified by buffer-id from the attached datapath element.
	 *
	 * Drops a packet stored on the datapath element identified by the given buffer-id
	 * by sending a Packet-Out message with an empty action list.
	 *
	 * @param auxid control connection identifier
	 * @param buffer_id identifier of the packet slot to be removed
	 * @param inport field of the Packet-Out message sent to the datapath element
	 */
	void
	drop_buffer(
			const rofl::cauxid& auxid,
			uint32_t buffer_id,
			uint32_t inport = rofl::openflow::OFPP_CONTROLLER) {
		rofl::openflow::cofactions actions(get_version_negotiated());
		send_packet_out_message(auxid, buffer_id, inport, actions, NULL, 0);
	};

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
	 * @brief	Sends OpenFlow Features-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_features_request(
			const rofl::cauxid& auxid,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Get-Config-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_get_config_request(
			const rofl::cauxid& auxid,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Table-Features-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_table_features_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_type OpenFlow statistics subtype
	 * @param stats_flags OpenFlow statistics flags
	 * @param body start of message payload
	 * @param bodylen length of message payload
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_type,
			uint16_t stats_flags,
			uint8_t *body = NULL,
			size_t bodylen = 0,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Desc-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_desc_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Flow-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_flow_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			const rofl::openflow::cofflow_stats_request& flow_stats_request,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Aggregate-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param aggr_stats_request OpenFlow Aggregate-Stats-Request payload
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_aggr_stats_request(
			const rofl::cauxid& auxid,
			uint16_t flags,
			const rofl::openflow::cofaggr_stats_request& aggr_stats_request,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));


	/**
	 * @brief	Sends OpenFlow Table-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_table_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags = 0,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Port-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param port_stats_request OpenFlow Port-Stats-Request payload
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_port_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			const rofl::openflow::cofport_stats_request& port_stats_request,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Queue-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param queue_stats_request OpenFlow Queue-Stats-Request payload
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_queue_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			const rofl::openflow::cofqueue_stats_request& queue_stats_request,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Group-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param group_stats_request OpenFlow Group-Stats-Request payload
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_group_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			const rofl::openflow::cofgroup_stats_request& group_stats_request,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Group-Desc-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_group_desc_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags = 0,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Group-Features-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_group_features_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Port-Desc-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_port_desc_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Experimenter-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param exp_id OpenFlow experimenter identifier
	 * @param exp_type OpenFlow experimenter type
	 * @param body experimenter statistics payload
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_experimenter_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			uint32_t exp_id,
			uint32_t exp_type,
			const cmemory& body,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Meter-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param meter_stats_request OpenFlow Meter-Stats-Request payload
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_meter_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			const rofl::openflow::cofmeter_stats_request& meter_stats_request,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Meter-Config-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param meter_config_stats_request OpenFlow Meter-Config-Stats-Request payload
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_meter_config_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			const rofl::openflow::cofmeter_config_request& meter_config_stats_request,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Meter-Features-Stats-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param stats_flags OpenFlow statistics flags
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_meter_features_stats_request(
			const rofl::cauxid& auxid,
			uint16_t stats_flags,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Packet-Out message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param buffer_id OpenFlow packet buffer identifier
	 * @param in_port incoming port for OpenFlow matches
	 * @param actions OpenFlow actions list
	 * @param data start of packet frame
	 * @param datalen length of packet frame
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_packet_out_message(
			const rofl::cauxid& auxid,
			uint32_t buffer_id,
			uint32_t in_port,
			const rofl::openflow::cofactions& actions,
			uint8_t *data = NULL,
			size_t datalen = 0);

	/**
	 * @brief	Sends OpenFlow Barrier-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_barrier_request(
			const rofl::cauxid& auxid,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Role-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param role OpenFlow role definition
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_role_request(
			const rofl::cauxid& auxid,
			const rofl::openflow::cofrole& role,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Flow-Mod message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param flowmod OpenFlow flow mod entry
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_flow_mod_message(
			const rofl::cauxid& auxid,
			const rofl::openflow::cofflowmod& flowmod);

	/**
	 * @brief	Sends OpenFlow Group-Mod message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param groupmod OpenFlow group mod entry
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_group_mod_message(
			const rofl::cauxid& auxid,
			const rofl::openflow::cofgroupmod& groupmod);

	/**
	 * @brief	Sends OpenFlow Table-Mod message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param table_id OpenFlow table identifier
	 * @param config OpenFlow table configuration
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_table_mod_message(
			const rofl::cauxid& auxid,
			uint8_t table_id,
			uint32_t config);

	/**
	 * @brief	Sends OpenFlow Port-Mod message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param portno OpenFlow port number
	 * @param hwaddr hardware address assigned to this port
	 * @param config OpenFlow port configuration
	 * @param mask OpenFlow port mask
	 * @param OpenFlow port advertise field
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_port_mod_message(
			const rofl::cauxid& auxid,
			uint32_t port_no,
			const rofl::caddress_ll& hwaddr,
			uint32_t config,
			uint32_t mask,
			uint32_t advertise);

	/**
	 * @brief	Sends OpenFlow Set-Config message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param flags OpenFlow datapath element's flags
	 * @param miss_send_len OpenFlow miss_send_len parameter
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_set_config_message(
			const rofl::cauxid& auxid,
			uint16_t flags,
			uint16_t miss_send_len);

	/**
	 * @brief	Sends OpenFlow Queue-Get-Config-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param portno OpenFlow port number
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_queue_get_config_request(
			const rofl::cauxid& auxid,
			uint32_t port,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Get-Async-Config-Request message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_get_async_config_request(
			const rofl::cauxid& auxid,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**
	 * @brief	Sends OpenFlow Set-Async-Config message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param async_config OpenFlow async_config payload
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_set_async_config_message(
			const rofl::cauxid& auxid,
			const rofl::openflow::cofasync_config& async_config);

	/**
	 * @brief	Sends OpenFlow Meter-Mod message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param command OpenFlow Meter-Mod command
	 * @param flags OpenFlow Meter-Mod flags
	 * @param meter_id OpenFlow Meter-Mod identifier
	 * @param meter_bands OpenFlow meterbands
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_meter_mod_message(
			const rofl::cauxid& auxid,
			uint16_t command,
			uint16_t flags,
			uint32_t meter_id,
			const rofl::openflow::cofmeter_bands& meter_bands);

	/**
	 * @brief	Sends OpenFlow Error message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param xid OpenFlow transaction identifier
	 * @param type OpenFlow error type
	 * @param code OpenFlow error code
	 * @param data start of error message payload
	 * @param datalen length of error message payload
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
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
	 * @brief	Sends OpenFlow Experimenter message to attached datapath element.
	 *
	 * @param auxid controller connection identifier
	 * @param exp_id OpenFlow experimenter identifier
	 * @param exp_type OpenFlow experimenter type
	 * @param body start of experimenter message payload
	 * @param bodylen length of experimenter message payload
	 * @param timeout until this request expires
	 * @return OpenFlow transaction ID assigned to this request
	 * @exception rofl::eRofBaseNotConnected
	 * @exception rofl::eRofBaseCongested
	 */
	uint32_t
	send_experimenter_message(
			const rofl::cauxid& auxid,
			uint32_t exp_id,
			uint32_t exp_type,
			uint8_t *body = NULL,
			size_t bodylen = 0,
			const rofl::cclock& timeout = rofl::cclock(/*seconds=*/DEFAULT_REQUEST_TIMEOUT));

	/**@}*/

public:

	/**
	 * @brief 	Predicate for finding a rofl::crofdpt instance by its rofl::cdptid.
	 */
	class crofdpt_find_by_dptid {
		cdptid dptid;
	public:
		crofdpt_find_by_dptid(const rofl::cdptid& dptid) : dptid(dptid) {};
		bool operator() (const crofdpt* rofdpt) {
			return (rofdpt->get_dptid() == dptid);
		};
	};

	/**
	 * @brief 	Predicate for finding a rofl::crofdpt instance by its rofl::cdpid.
	 */
	class crofdpt_find_by_dpid {
		uint64_t dpid;
	public:
		crofdpt_find_by_dpid(uint64_t dpid) : dpid(dpid) {};
		bool operator() (const std::pair<cdptid, crofdpt*>& p) {
			return (p.second->get_dpid().get_uint64_t() == dpid);
		};
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const crofdpt& dpt) {
		os << indent(0) << "<crofdpt >";
		{ rofl::indent i(2); os << dpt.get_dptid(); }
		{ rofl::indent i(2); os << dpt.rofchan; }
		switch (dpt.state) {
		case STATE_INIT: {
			os << indent(2) << "<state: -INIT- >" << std::endl;
		} break;
		case STATE_CONNECTED: {
			os << indent(2) << "<state: -CONNECTED- >" << std::endl;
		} break;
		case STATE_DISCONNECTED: {
			os << indent(2) << "<state: -DISCONNECTED- >" << std::endl;
		} break;
		case STATE_ESTABLISHED: {
			os << indent(2) << "<state: -ESTABLISHED- >" << std::endl;
		} break;
		case STATE_FEATURES_RCVD: {
			os << indent(2) << "<state: -FEATURES-RCVD- >" << std::endl;
		} break;
		case STATE_GET_CONFIG_RCVD: {
			os << indent(2) << "<state: -GET-CONFIG-RCVD- >" << std::endl;
		} break;
		case STATE_TABLE_FEATURES_RCVD: {
			os << indent(2) << "<state: -TABLE-FEATURES-RCVD- >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<state: -UNKNOWN- >" << std::endl;
		} break;
		}
		os << indent(2) << "<hwaddr: " << dpt.hwaddr.str() << " >" << std::endl;
		os << indent(2) << "<#buffers: " << (int)dpt.n_buffers << " >" << std::endl;
		os << indent(2) << "<#tables: " << (int)dpt.n_tables << " >" << std::endl;
		os << indent(2) << "<capabilities: " << std::hex << (int)dpt.capabilities << std::dec << " >" << std::endl;
		os << indent(2) << "<config: " << std::hex << (int)dpt.config << std::dec << " >" << std::endl;
		os << indent(2) << "<miss-send-len: " << (int)dpt.miss_send_len << " >" << std::endl;
		rofl::indent j(2);
		os << dpt.tables;
		os << dpt.ports;
		os << dpt.transactions;
		return os;
	};

	/**
	 * @brief	Returns a one-liner string with basic information about this instance.
	 */
	std::string
	str() const {
		std::stringstream ss;
		ss << "dpid: " << get_dpid().str() << " ";
		switch (state) {
		case STATE_INIT: {
			ss << "state: -init- ";
		} break;
		case STATE_CONNECTED: {
			ss << "state: -connected- ";
		} break;
		case STATE_DISCONNECTED: {
			ss << "state: -disconnected- ";
		} break;
		case STATE_ESTABLISHED: {
			ss << "state: -established- ";
		} break;
		case STATE_FEATURES_RCVD: {
			ss << "state: -features-rcvd- ";
		} break;
		case STATE_GET_CONFIG_RCVD: {
			ss << "state: -get-config-rcvd- ";
		} break;
		case STATE_TABLE_FEATURES_RCVD: {
			ss << "state: -table-features-rcvd- ";
		} break;
		default: {
			ss << "state: -unknown- ";
		} break;
		}
		return ss.str();
	};

private:

	crofdpt_env&
	call_env() {
		if (crofdpt_env::rofdpt_envs.find(env) == crofdpt_env::rofdpt_envs.end()) {
			throw eRofDptNotFound("rofl::crofdpt::call_env() environment not found");
		}
		return *env;
	};

	virtual void
	handle_conn_established(
			crofchan& chan,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofdpt] dptid: " << dptid.str()
						<< " control connection established, auxid: " << auxid.str() << std::endl;

		call_env().handle_conn_established(*this, auxid);

		if (auxid == rofl::cauxid(0)) {
			rofl::logging::info << "[rofl-common][crofdpt] dpid: " << std::hex << get_dpid().str() << std::dec
					<< " OFP control channel established, " << chan.str() << std::endl;
			push_on_eventqueue(EVENT_CONNECTED);
		}
	};

	virtual void
	handle_conn_terminated(
			crofchan& chan,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofdpt] dptid: " << dptid.str()
						<< " control connection terminated, auxid: " << auxid.str() << std::endl;

		rofl::RwLock rwlock(conns_terminated_rwlock, rofl::RwLock::RWLOCK_WRITE);
		conns_terminated.push_back(auxid);
		push_on_eventqueue(EVENT_CONN_TERMINATED);

		if (auxid == rofl::cauxid(0)) {
			rofl::logging::info << "[rofl-common][crofdpt] dptid: " << dptid.str()
					<< " OFP control channel terminated, " << chan.str() << std::endl;
			transactions.clear();
			push_on_eventqueue(EVENT_DISCONNECTED);
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
	handle_write(crofchan& chan, const rofl::cauxid& auxid)
	{ call_env().handle_conn_writable(*this, auxid); };

	virtual void
	recv_message(crofchan& chan, const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	virtual uint32_t
	get_async_xid(crofchan& chan)
	{ return transactions.get_async_xid(); };

	virtual uint32_t
	get_sync_xid(crofchan& chan, uint8_t msg_type = 0, uint16_t msg_sub_type = 0)
	{ return transactions.add_ta(cclock(/*secs=*/5), msg_type, msg_sub_type); };

	virtual void
	release_sync_xid(crofchan& chan, uint32_t xid)
	{ return transactions.drop_ta(xid); };

private:

	virtual void
	ta_expired(rofl::ctransactions& tas, rofl::ctransaction& ta);

private:

	virtual void
	handle_timeout(
		int opaque, void *data = (void*)0);

private:

	void
	push_on_eventqueue(
			enum crofdpt_event_t event = EVENT_NONE) {
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
	event_disconnected();

	void
	event_connected();

	void
	event_conn_terminated();

	void
	event_conn_refused();

	void
	event_conn_failed();

	void
	event_features_reply_rcvd();

	void
	event_features_request_expired(
			uint32_t xid = 0);

	void
	event_get_config_reply_rcvd();

	void
	event_get_config_request_expired(
			uint32_t xid = 0);

	void
	event_table_stats_reply_rcvd();

	void
	event_table_stats_request_expired(
			uint32_t xid = 0);

	void
	event_table_features_stats_reply_rcvd();

	void
	event_table_features_stats_request_expired(
			uint32_t xid = 0);

	void
	event_port_desc_reply_rcvd();

	void
	event_port_desc_request_expired(
			uint32_t xid = 0);

private:

	void
	experimenter_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	error_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	features_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	get_config_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	multipart_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	desc_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	table_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	port_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	flow_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	aggregate_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	queue_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	group_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	group_desc_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	group_features_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	meter_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	meter_config_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	meter_features_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	table_features_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	port_desc_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	experimenter_stats_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	barrier_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	port_mod_sent(
			rofl::openflow::cofmsg *pack);

	void
	packet_in_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	flow_removed_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	port_status_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	role_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	queue_get_config_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

	void
	get_async_config_reply_rcvd(
			const rofl::cauxid& auxid, rofl::openflow::cofmsg *msg);

private:

	static std::map<rofl::cdptid, crofdpt*> rofdpts;

	// environment
	rofl::crofdpt_env*      env;
	// handle for this crofdpt instance
	rofl::cdptid            dptid;
	// OFP control channel
	rofl::crofchan          rofchan;
	// pending OFP transactions
	rofl::ctransactions     transactions;

	bool                    remove_on_channel_close;
	// allocated groupids on datapath
	std::set<uint32_t>      groupids;

	// datapath identifier
	rofl::cdpid             dpid;
	// datapath hardware address
	rofl::caddress_ll       hwaddr;
	// number of buffer lines
	uint32_t                n_buffers;
	// number of tables
	uint8_t                 n_tables;
	// capabilities field
	uint32_t                capabilities;
	// list of tables
	rofl::openflow::coftables tables;
	// list of ports
	rofl::openflow::cofports ports;
	uint16_t                config;
	uint16_t                miss_send_len;

	unsigned int            state;
	std::deque<enum crofdpt_event_t>
                            events;

	PthreadRwLock           conns_terminated_rwlock;
	std::list<rofl::cauxid> conns_terminated;
	PthreadRwLock           conns_refused_rwlock;
	std::list<rofl::cauxid> conns_refused;
	PthreadRwLock           conns_failed_rwlock;
	std::list<rofl::cauxid> conns_failed;

	std::bitset<32>         flags;

	static const time_t     DEFAULT_REQUEST_TIMEOUT = 5; // seconds
};

}; // end of namespace

#endif
