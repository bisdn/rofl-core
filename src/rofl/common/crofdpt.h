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
#include "rofl/common/openflow/extensions/cfsptable.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofflowmod.h"
#include "rofl/common/openflow/cofgroupmod.h"
#include "rofl/common/crofchan.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/openflow/cofasyncconfig.h"
#include "rofl/common/openflow/cofrole.h"
#include "rofl/common/cauxid.h"


namespace rofl
{

class eRofDptBase 		: public RoflException {};
class eRofDptNotFound 	: public eRofDptBase {};


/* error classes */
class eOFdptBase					: public RoflException {};
class eDataPathIdInUse 				: public eOFdptBase {}; // datapath id already in use
class eDataPathAlreadyAttached 		: public eOFdptBase {}; // crofbase *entity is already attached
class eOFswitchBase 				: public eOFdptBase {};
class eOFswitchInvalid 				: public eOFdptBase {};
class eOFdpathNotFound 				: public eOFdptBase {}; // element not found


class crofbase;


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
class crofdpt
{
public:

	static crofdpt&
	get_dpt(const cdptid& dptid);

	static crofdpt&
	get_dpt(uint64_t dpid);

public:


	/**
	 * @brief 	Default constructor for generating an empty rofl::openflow::cofdpt instance
	 *
	 * @param rofbase pointer to crofbase instance
	 */
	crofdpt() :
		dptid(cdptid(++crofdpt::next_dptid)), dpid(0) {
		crofdpt::rofdpts[dptid] = this;
	};


	/**
	 * @brief	Destructor.
	 *
	 * Deallocates all previously allocated resources for storing data model
	 * exposed by the data path element.
	 */
	virtual
	~crofdpt() {
		crofdpt::rofdpts.erase(dptid);
	};




	/**
	 * @brief	Returns the data path element's data path ID.
	 *
	 * @return dpid
	 */
	virtual const cdptid&
	get_dptid() const { return dptid; };


	/**
	 * @brief	Returns the data path element's data path ID.
	 *
	 * @return dpid
	 */
	const uint64_t&
	get_dpid() const { return dpid; };


	/**
	 * @brief	Returns the data path element's ID string.
	 *
	 * @return s_dpid
	 */
	const std::string&
	get_dpid_s() const { return s_dpid; };


public:


	/**
	 * @brief 	Returns the OpenFlow protocol version used for this control connection.
	 *
	 * @return OpenFlow version used for this control connection
	 */
	virtual uint8_t
	get_version() const = 0;


	/**
	 * @brief	Returns caddress of connected remote entity.
	 *
	 * @return caddress object obtained from this->socket
	 */
	virtual caddress
	get_peer_addr() = 0;


	/**
	 *
	 * @return
	 */
	virtual rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap() = 0;


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
	 * @brief	Returns reference to the data path element's flowspace table.
	 *
	 * @return fsptable
	 */
	virtual rofl::openflow::cfsptable&
	get_fsptable() = 0;


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
	 *
	 */
	virtual void
	connect(
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& socket_params) = 0;


	/**
	 *
	 */
	virtual void
	disconnect() = 0;


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
			rofl::openflow::cofmatch const& ofmatch) = 0;


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
			rofl::openflow::cofmatch const& ofmatch = rofl::openflow::cofmatch()) = 0;

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
	send_features_request(const cauxid& aux_id) = 0;

	/**
	 * @brief	Sends a GET-CONFIG.request to a data path element.
	 *
	 * @return transaction ID assigned to this request
	 */
	virtual uint32_t
	send_get_config_request(const cauxid& aux_id) = 0;

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
			return (p.second->get_dpid() == dpid);
		};
	};

protected:

	/**
	 *
	 */
	void
	set_dpid(uint64_t dpid) {
		std::stringstream sstr; sstr << dpid;
		s_dpid = sstr.str();
	};


private:

	static uint64_t 					next_dptid;

	static std::map<cdptid, crofdpt*> 	rofdpts;

	cdptid   							dptid;			// handle for this crofdpt instance
	uint64_t 							dpid;			// datapath id
	std::string	 						s_dpid;			// datapath id as std::string

};


}; // end of namespace

#include "rofl/common/crofbase.h"

#endif
