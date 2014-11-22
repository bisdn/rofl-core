/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CROFCTL_H
#define CROFCTL_H 1

#include <map>
#include <string>

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



namespace rofl
{

class eRofCtlBase 		: public RoflException {};
class eRofCtlNotFound 	: public eRofCtlBase {};


class crofbase;


class crofctl {
public: // methods

	/**
	 *
	 */
	static crofctl&
	get_ctl(const cctlid& ctlid);

public:

	enum crofctl_flavour_t {
		FLAVOUR_PASSIVE = 1,	// connection was established from peer entity
		FLAVOUR_ACTIVE = 2,		// connection was established actively by us
	};

	/**
	 *
	 */
	enum crofctl_flavour_t
	get_flavour() const { return flavour; };


	/**
	 * @brief	Constructor for creating new cofctl instance from accepted socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 */
	crofctl(enum crofctl_flavour_t flavour) :
			ctlid(cctlid(++crofctl::next_ctlid)), flavour(flavour) {
		crofctl::rofctls[ctlid] = this;
		rofl::logging::debug << "[rofl-common][crofctl] instance creating, ctlid: "
				<< (unsigned long long)ctlid.get_ctlid() << std::endl;
	};

	/**
	 * @brief	Destructor.
	 */
	virtual
	~crofctl() {
		crofctl::rofctls.erase(ctlid);
		rofl::logging::debug << "[rofl-common][crofctl] destroying instance, ctlid: "
				<< (unsigned long long)ctlid.get_ctlid() << std::endl;
	};

public:

	/**
	 * @brief	Returns the controller handle id
	 */
	const cctlid&
	get_ctlid() const { return ctlid; };


public:

	/**
	 *
	 */
	virtual std::list<rofl::cauxid>
	get_conn_index() const = 0;

	/**
	 *
	 */
	virtual rofl::cauxid
	connect(
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& socket_params) = 0;

	/**
	 *
	 */
	virtual void
	disconnect(
			const rofl::cauxid& auxid = 0) = 0;

	/**
	 *
	 */
	virtual void
	reconnect(
			const rofl::cauxid& auxid = 0) = 0;

public:

	/**
	 *
	 * @return
	 */
	virtual crofchan&
	set_channel() = 0;


	/**
	 *
	 * @return
	 */
	virtual crofchan const&
	get_channel() const = 0;

public:


	/**
	 *
	 */
	virtual rofl::openflow::cofasync_config&
	set_async_config() = 0;



	/**
	 *
	 */
	virtual rofl::openflow::cofasync_config const&
	get_async_config() const = 0;



	/**
	 *
	 */
	virtual rofl::openflow::cofrole&
	set_role() = 0;



	/**
	 *
	 */
	virtual rofl::openflow::cofrole const&
	get_role() const = 0;



	/**
	 * @brief	Returns true, when the control handshake (HELLO) has been completed.
	 */
	virtual bool
	is_established() const = 0;



	/**
	 * @brief	Returns true, when control entity assume role SLAVE
	 */
	virtual bool
	is_slave() const = 0;



	/**
	 * @brief	Returns OpenFlow version negotiated for control connection.
	 */
	virtual uint8_t
	get_version() const = 0;




#if 0
	/**
	 *
	 */
	virtual void
	recv_message(
			cofmsg *msg, uint8_t aux_id) = 0;
#endif

	/**
	 * @brief	Returns caddress of connected remote entity.
	 *
	 * @return caddress object obtained from this->socket
	 */
	virtual caddress
	get_peer_addr() = 0;



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
	 * @param portlist list of cofports in this data path, ignored in OpenFlow 1.3
	 */
	virtual void
	send_features_reply(
			const cauxid& auxid,
			uint32_t xid,
			uint64_t dpid,
			uint32_t n_buffers,
			uint8_t n_tables,
			uint32_t capabilities,
			uint8_t of13_auxiliary_id = 0,
			uint32_t of10_actions_bitmap = 0,
			rofl::openflow::cofports const& ports = rofl::openflow::cofports()) = 0;

	/**
	 * @brief	Sends a GET-CONFIG.reply to a controller entity.
	 *
	 * @param xid transaction ID from GET-CONFIG.request
	 * @param flags data path flags
	 * @param miss_send_len default miss_send_len value
	 */
	virtual void
	send_get_config_reply(
			const cauxid& auxid,
			uint32_t xid,
			uint16_t flags,
			uint16_t miss_send_len) = 0;

	/**
	 * @brief	Sends a STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param stats_type one of the OFPST_* constants
	 * @param body body of a STATS.reply
	 * @param bodylen length of STATS.reply body
	 * @param more flag if multiple STATS.reply messages will be sent
	 */
	virtual void
	send_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			uint16_t stats_type,
			uint16_t stats_flags,
			uint8_t *body = NULL,
			size_t bodylen = 0) = 0;

	/**
	 * @brief	Sends a DESC-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param desc_stats body of DESC-STATS.reply
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_desc_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::cofdesc_stats_reply const& desc_stats,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a FLOW-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param flow_stats array of flow_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_flow_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::cofflowstatsarray const& flow_stats,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends an AGGREGATE-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param aggr_stats aggr_stats body
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_aggr_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::cofaggr_stats_reply const& aggr_stats,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a TABLE-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param table_stats array of table_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_table_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::coftablestatsarray const& tablestatsarray,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a PORT-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param port_stats array of port_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_port_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::cofportstatsarray const& portstatsarray,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a QUEUE-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param port_stats array of port_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_queue_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::cofqueuestatsarray const& queuestatsarray,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a GROUP-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param group_stats array of group_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_group_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::cofgroupstatsarray const& groupstatsarray,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a GROUP-DESC-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param group_desc_stats array of group_desc_stats bodies
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_group_desc_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::cofgroupdescstatsarray const& groupdescstatsarray,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a GROUP-FEATURES-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param group_features_stats group_features_stats body
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_group_features_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::cofgroup_features_stats_reply const& group_features_stats,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a TABLE-FEATURES-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param tables tables body
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_table_features_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::coftables const& tables,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a PORT-DESC-STATS.reply to a controller entity.
	 *
	 * @param xid transaction ID from received STATS.request
	 * @param ports cofports body
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_port_desc_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::cofports const& ports,
			uint16_t stats_flags = 0) = 0;

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
	virtual void
	send_experimenter_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			uint32_t exp_id,
			uint32_t exp_type,
			cmemory const& body,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a METER-STATS.reply to a controller entity.
	 *
	 * @param auxid auxiliary connection id
	 * @param xid transaction ID from received STATS.request
	 * @param meter_stats_array instance of type cofmeterstatsarray
	 * @param stats_flags flags for OpenFlow statistics messages, if any (default: 0)
	 */
	virtual void
	send_meter_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofmeterstatsarray& meter_stats_array,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a METER-CONFIG-STATS.reply to a controller entity.
	 *
	 * @param auxid auxiliary connection id
	 * @param xid transaction ID from received STATS.request
	 * @param meter_config_array instance of type cofmeterconfigarray
	 * @param stats_flags flags for OpenFlow statistics messages, if any (default: 0)
	 */
	virtual void
	send_meter_config_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofmeterconfigarray& meter_config_array,
			uint16_t stats_flags = 0) = 0;

	/**
	 * @brief	Sends a METER-FEATURES-STATS.reply to a controller entity.
	 *
	 * @param auxid auxiliary connection id
	 * @param xid transaction ID from received STATS.request
	 * @param meter_config_array instance of type cofmeterfeatures
	 * @param stats_flags flags for OpenFlow statistics messages, if any (default: 0)
	 */
	virtual void
	send_meter_features_stats_reply(
			const cauxid& auxid,
			uint32_t xid,
			const rofl::openflow::cofmeter_features& meter_features,
			uint16_t stats_flags = 0) = 0;

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
	virtual void
	send_packet_in_message(
			const cauxid& auxid,
			uint32_t buffer_id,
			uint16_t total_len,
			uint8_t reason,
			uint8_t table_id,
			uint64_t cookie,
			uint16_t in_port, // for OF1.0
			rofl::openflow::cofmatch &match,
			uint8_t *data,
			size_t datalen) = 0;

	/**
	 * @brief	Sends a BARRIER-reply to a controller entity.
	 *
	 * @param xid transaction ID from received BARRIER.request
	 */
	virtual void
	send_barrier_reply(
			const cauxid& auxid,
			uint32_t xid) = 0;

	/**
	 * @brief	Sends an ERROR.message to a controller entity.
	 *
	 * @param xid transaction ID of request that generated this error message
	 * @param type one of OpenFlow's OFPET_* flags
	 * @param code one of OpenFlow's error codes
	 * @param data first (at least 64) bytes of failed request
	 * @param datalen length of failed request appended to error message
	 */
	virtual void
	send_error_message(
			const cauxid& auxid,
			uint32_t xid,
			uint16_t type,
			uint16_t code,
			uint8_t* data = NULL,
			size_t datalen = 0) = 0;

	/**
	 * @brief 	Sends an EXPERIMENTER.message to a controller entity.
	 *
	 * @param experimenter_id exp_id as assigned by ONF
	 * @param exp_type exp_type as defined by the ONF member
	 * @param body pointer to opaque experimenter message body (optional)
	 * @param bodylen length of body (optional)
	 * @result transaction ID assigned to this request
	 */
	virtual void
	send_experimenter_message(
			const cauxid& auxid,
			uint32_t xid,
			uint32_t experimenter_id,
			uint32_t exp_type,
			uint8_t *body = NULL,
			size_t bodylen = 0) = 0;

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
	virtual void
	send_flow_removed_message(
			const cauxid& auxid,
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
			uint64_t byte_count) = 0;

	/**
	 * @brief	Sends a PORT-STATUS.message to a controller entity.
	 *
	 * @param reason one of OpenFlow's OFPPR_* constants
	 * @param port cofport instance that changed its status
	 */
	virtual void
	send_port_status_message(
			const cauxid& auxid,
			uint8_t reason,
			rofl::openflow::cofport const& port) = 0;

	/**
	 * @brief	Sends a QUEUE-GET-CONFIG.reply to a controller entity.
	 *
	 * @param xid transaction ID from QUEUE-GET-CONFIG.request
	 * @param portno OpenFlow number assigned to port
	 */
	virtual void
	send_queue_get_config_reply(
			const cauxid& auxid,
			uint32_t xid,
			uint32_t portno,
			rofl::openflow::cofpacket_queues const& pql) = 0;


	/**
	 * @brief	Sends a ROLE.reply to a controller entity.
	 *
	 * @param xid transaction ID from associated ROLE.request
	 * @param role defined role from data path
	 * @param generation_id gen_id as defined by OpenFlow
	 */
	virtual void
	send_role_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::cofrole const& role) = 0;

	/**
	 * @brief	Sends a GET-ASYNC-CONFIG.reply to a controller entity.
	 *
	 * @param xid transaction ID from GET-CONFIG.request
	 */
	virtual void
	send_get_async_config_reply(
			const cauxid& auxid,
			uint32_t xid,
			rofl::openflow::cofasync_config const& async_config) = 0;

	/**@}*/

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

	static uint64_t 					next_ctlid;
	static std::map<cctlid, crofctl*> 	rofctls;
	cctlid   							ctlid;
	enum crofctl_flavour_t 				flavour;
};


}; // end of namespace

#include "crofbase.h"

#endif
