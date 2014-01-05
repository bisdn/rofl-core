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
#include "crofbase.h"
#include "cxidstore.h"
#include "openflow/messages/cofmsg.h"



namespace rofl
{


class crofbase;


class crofctl :
	public cxidowner
{
public: // methods




	/**
	 * @brief	Constructor for creating new cofctl instance from accepted socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 */
	crofctl(
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
	crofctl(
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
	crofctl(
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
	~crofctl() {};



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



	/**
	 * @brief	Returns cxidtrans instance associated with transaction ID xid.
	 *
	 * @param xid transaction ID
	 */
	virtual cxidtrans&
	transaction(
			uint32_t xid) = 0;



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
