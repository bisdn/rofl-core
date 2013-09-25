/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFCTL_H
#define COFCTL_H 1

#include <map>
#include <string>

#include "openflow.h"
#include "../cerror.h"
#include "../ciosrv.h"
#include "../cmemory.h"
#include "../crofbase.h"
#include "../cxidstore.h"
#include "../thread_helper.h"
#include "../cvastring.h"
#include "../csocket.h"
#include "../cfsm.h"
#include "../openflow/messages/cofmsg.h"
#include "../openflow/messages/cofmsg_hello.h"
#include "../openflow/messages/cofmsg_echo.h"
#include "../openflow/messages/cofmsg_error.h"
#include "../openflow/messages/cofmsg_features.h"
#include "../openflow/messages/cofmsg_config.h"
#include "../openflow/messages/cofmsg_packet_out.h"
#include "../openflow/messages/cofmsg_packet_in.h"
#include "../openflow/messages/cofmsg_flow_mod.h"
#include "../openflow/messages/cofmsg_flow_removed.h"
#include "../openflow/messages/cofmsg_group_mod.h"
#include "../openflow/messages/cofmsg_table_mod.h"
#include "../openflow/messages/cofmsg_port_mod.h"
#include "../openflow/messages/cofmsg_port_status.h"
#include "../openflow/messages/cofmsg_stats.h"
#include "../openflow/messages/cofmsg_desc_stats.h"
#include "../openflow/messages/cofmsg_flow_stats.h"
#include "../openflow/messages/cofmsg_aggr_stats.h"
#include "../openflow/messages/cofmsg_table_stats.h"
#include "../openflow/messages/cofmsg_port_stats.h"
#include "../openflow/messages/cofmsg_queue_stats.h"
#include "../openflow/messages/cofmsg_group_stats.h"
#include "../openflow/messages/cofmsg_group_desc_stats.h"
#include "../openflow/messages/cofmsg_group_features_stats.h"
#include "../openflow/messages/cofmsg_barrier.h"
#include "../openflow/messages/cofmsg_queue_get_config.h"
#include "../openflow/messages/cofmsg_role.h"
#include "../openflow/messages/cofmsg_experimenter.h"
#include "../openflow/messages/cofmsg_async_config.h"

#include "cofmatch.h"
#include "extensions/cfspentry.h"

namespace rofl
{


class crofbase;


class cofctl :
	public csocket_owner,
	public ciosrv,
	public cfsm,
	public cfspentry_owner,
	public cxidowner
{
public: // methods



	/**
	 * @brief	Constructor for creating new cofctl instance from accepted socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 */
	cofctl(
			crofbase *rofbase) {};



	/**
	 * @brief	Constructor for creating new cofctl instance from accepted socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param newsd socket descriptor
	 * @param ra peer remote address
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 */
	cofctl(
			crofbase *rofbase,
			int newsd,
			caddress const& ra,
			int domain,
			int type,
			int protocol) {};



	/**
	 * @brief	Constructor for creating new cofctl instance for connecting socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param ra peer remote address
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 *
	 */
	cofctl(
			crofbase *rofbase,
			caddress const& ra,
			int domain,
			int type,
			int protocol) {};



	/**
	 * @brief	Destructor.
	 */
	virtual
	~cofctl() {};



	/**
	 * @brief	Returns a c-string with information about this instance.
	 *
	 * @return c-string
	 */
	virtual const char*
	c_str() = 0;



	/**
	 * @brief	Returns true, when the control handshake (HELLO) has been completed.
	 */
	virtual bool
	is_established() const = 0;



	/**
	 * @brief	Returns OpenFlow version negotiated for control connection.
	 */
	virtual uint8_t
	get_version() = 0;



	/**
	 * @brief	Returns cxidtrans instance associated with transaction ID xid.
	 *
	 * @param xid transaction ID
	 */
	virtual cxidtrans&
	transaction(
			uint32_t xid) = 0;



	/**
	 * @brief	Sends an OpenFlow message via this cofctl instance.
	 *
	 * @param msg pointer to cofmsg instance
	 */
	virtual void
	send_message(
			cofmsg *msg) = 0;


	/**
	 *
	 */
	virtual void
	handle_message(
			cmemory *pack) = 0;


	/**
	 * @brief	Returns caddress of connected remote entity.
	 *
	 * @return caddress object obtained from this->socket
	 */
	virtual caddress
	get_peer_addr() = 0;


	/**
	 *
	 */
	virtual uint32_t
	get_role() const = 0;
};


}; // end of namespace

#endif
