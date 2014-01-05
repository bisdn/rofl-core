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
#include "rofl/common/crofbase.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/cxidstore.h"
#include "rofl/common/openflow/cofport.h"
#include "rofl/common/openflow/extensions/cfsptable.h"
#include "openflow/openflow.h"
#include "openflow/messages/cofmsg.h"



namespace rofl
{


/* error classes */
class eOFdptBase					: public RoflException {};
class eDataPathIdInUse 				: public eOFdptBase {}; // datapath id already in use
class eDataPathAlreadyAttached 		: public eOFdptBase {}; // crofbase *entity is already attached
class eOFswitchBase 				: public eOFdptBase {};
class eOFswitchInvalid 				: public eOFdptBase {};
class eOFdpathNotFound 				: public eOFdptBase {}; // element not found


class crofbase;


/**
 * \class	cofdpt
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
class crofdpt :
	public cxidowner
{
public:



	/**
	 * @brief 	Default constructor for generating an empty cofdpt instance
	 *
	 * @param rofbase pointer to crofbase instance
	 */
	crofdpt(
			crofbase *rofbase = (crofbase*)0) {};




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
	crofdpt(
			crofbase *rofbase,
			int newsd,
			caddress const& ra,
			int domain,
			int type,
			int protocol) {};



	/**
	 * @brief 	Constructor for creating a new cofdpt instance and actively connecting to a data path element.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param ofp_version OpenFlow version to use
	 * @param ra peer address of control connection
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 */
	crofdpt(
			crofbase *rofbase,
			uint8_t ofp_version,
			int reconnect_start_timeout,
			caddress const& ra,
			int domain,
			int type,
			int protocol) {};



	/**
	 * @brief	Destructor.
	 *
	 * Deallocates all previously allocated resources for storing data model
	 * exposed by the data path element.
	 */
	virtual
	~crofdpt() {};



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
			uint32_t port_no) throw (eOFdpathNotFound) = 0;


	/**
	 * @brief 	Find a cofport instance based on OpenFlow port name (e.g. eth0).
	 *
	 * @return pointer to cofport instance
	 * @throws eOFdpathNotFound if port could not be found
	 */
	virtual cofport*
	find_cofport(
			std::string port_name) throw (eOFdpathNotFound) = 0;


	/**
	 * @brief 	Find a cofport instance based on OpenFlow port hardware address.
	 *
	 * @return pointer to cofport instance
	 * @throws eOFdpathNotFound if port could not be found
	 */
	virtual cofport*
	find_cofport(
			cmacaddr const& maddr) throw (eOFdpathNotFound) = 0;


	/**
	 * @brief	Returns the data path element's data path ID.
	 *
	 * @return dpid
	 */
	virtual uint64_t
	get_dpid() const = 0;


	/**
	 * @brief	Returns the data path element's ID string.
	 *
	 * @return s_dpid
	 */
	virtual std::string
	get_dpid_s() const = 0;


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
	virtual cfsptable&
	get_fsptable() = 0;


	/**
	 * @brief	Returns reference to the data path element's cofport list.
	 *
	 * @return ports
	 */
	virtual std::map<uint32_t, cofport*>&
	get_ports() = 0;


	/**
	 * @brief	Returns reference to cofport instance assigned to portno.
	 *
	 * if portno is invalid, an exeception of type eOFdpathNotFound is thrown.
	 *
	 * @return reference to cofport instance assigned to specified portno
	 */
	virtual cofport&
	get_port(uint32_t portno) = 0;


	/**
	 * @brief	Returns reference to the data path element's coftable_stats_reply list.
	 *
	 * @return tables
	 */
	virtual std::map<uint8_t, coftable_stats_reply>&
	get_tables() = 0;


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
			cofmatch const& ofmatch) = 0;


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
			cofmatch const& ofmatch = cofmatch()) = 0;

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
};


}; // end of namespace

#endif
