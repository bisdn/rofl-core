/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFCTL_H
#define COFCTL_H 1

#include <map>
#include <string>

#include "openflow.h"
#include "../cerror.h"
#include "../cmemory.h"
#include "../crofbase.h"
#include "../cxidstore.h"
#include "../openflow/messages/cofmsg.h"



namespace rofl
{


class crofbase;


class cofctl :
	public cxidowner
{
public: // methods




	/**
	 * @brief	Constructor for creating new cofctl instance from accepted socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 */
	cofctl(
			crofbase *rofbase = (crofbase*)0) {};



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
	 * @param ofp_version OpenFlow version to use
	 * @param ra peer remote address
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 *
	 */
	cofctl(
			crofbase *rofbase,
			uint8_t ofp_version,
			int reconnect_in_seconds,
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
	 * @brief	Returns true, when control entity assume role SLAVE
	 */
	virtual bool
	is_slave() const = 0;



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


	/**
	 *
	 */
	virtual void
	set_role(uint32_t role) = 0;
};


}; // end of namespace

#endif
