/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofchan.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CROFCHAN_H_
#define CROFCHAN_H_

#include <map>
#include <bitset>
#include <inttypes.h>

#include "rofl/common/croflexception.h"
#include "rofl/common/ciosrv.h"
#include "rofl/common/crofconn.h"
#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/crandom.h"
#include "rofl/common/ctimerid.h"
#include "rofl/common/cauxid.h"

namespace rofl {

class eRofChanBase			: public RoflException {};
class eRofChanNotFound		: public eRofChanBase {};
class eRofChanExists		: public eRofChanBase {};
class eRofChanInval			: public eRofChanBase {};
class eRofChanNotConnected	: public eRofChanBase {};

class crofchan; // forward declaration

class crofchan_env {
public:
	virtual ~crofchan_env() {};
	virtual void handle_established(crofchan *chan) = 0;
	virtual void handle_disconnected(crofchan *chan) = 0;
	virtual void handle_write(crofchan *chan, const cauxid& auxid) = 0;
	virtual void recv_message(crofchan *chan, const cauxid& aux_id, rofl::openflow::cofmsg *msg) = 0;
	virtual uint32_t get_async_xid(crofchan *chan) = 0;
	virtual uint32_t get_sync_xid(crofchan *chan, uint8_t msg_type = 0, uint16_t msg_sub_type = 0) = 0;
	virtual void release_sync_xid(crofchan *chan, uint32_t xid) = 0;
};

class crofchan :
		public crofconn_env,
		public ciosrv
{
	crofchan_env						*env;
	std::map<cauxid, crofconn*>			conns;				// main and auxiliary connections
	rofl::openflow::cofhello_elem_versionbitmap			versionbitmap;		// supported OFP versions
	uint8_t								ofp_version;		// OFP version negotiated
	std::bitset<32>						flags;

	enum crofchan_event_t {
		EVENT_NONE				= 0,
		EVENT_DISCONNECTED		= 1,
		EVENT_ESTABLISHED		= 2,
	};
	std::deque<enum crofchan_event_t> 	events;

	enum crofchan_state_t {
		STATE_DISCONNECTED 		= 1,
		STATE_CONNECT_PENDING	= 2,
		STATE_ESTABLISHED 		= 3,
	};
	enum crofchan_state_t				state;

public:

	/**
	 *
	 */
	crofchan();

	/**
	 *
	 */
	crofchan(
			crofchan_env *env,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap);

	/**
	 *
	 */
	virtual
	~crofchan();

public:

	bool
	is_established() const;

	virtual void
	handle_connect_refused(crofconn *conn);

	virtual void
	handle_connect_failed(crofconn *conn);

	virtual void
	handle_connected(crofconn *conn, uint8_t ofp_version);

	virtual void
	handle_closed(crofconn *conn);

	virtual void
	handle_write(crofconn *conn);

	virtual void
	recv_message(crofconn *conn, rofl::openflow::cofmsg *msg);

	virtual uint32_t
	get_async_xid(crofconn *conn);

	virtual uint32_t
	get_sync_xid(crofconn *conn, uint8_t msg_type = 0, uint16_t msg_sub_type = 0);

	virtual void
	release_sync_xid(crofconn *conn, uint32_t xid);

public:

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap() { return versionbitmap; };

	/**
	 *
	 */
	void
	close();

	/**
	 *
	 */
	unsigned int
	send_message(
			const cauxid& aux_id, rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	cauxid
	get_next_auxid();

	/**
	 *
	 */
	std::list<cauxid>
	get_conn_index() const;

	/**
	 * @brief	Add a new connection while creating a new crofconn instance and do a socket connect.
	 */
	crofconn&
	add_conn(
			const cauxid& aux_id,
			enum rofl::csocket::socket_type_t socket_type,
			cparams const& socket_params);

	/**
	 * @brief 	Add a new connection with an existing crofconn instance obtained from a listening socket.
	 */
	crofconn&
	add_conn(
			const cauxid& aux_id, crofconn* conn);

	/**
	 *
	 */
	crofconn&
	set_conn(
			const cauxid& aux_id);

	/**
	 *
	 */
	crofconn const&
	get_conn(
			const cauxid& aux_id) const;

	/**
	 *
	 */
	void
	drop_conn(
			const cauxid& aux_id);

	/**
	 *
	 */
	bool
	has_conn(
			const cauxid& aux_id) const;


private:

	/**
	 *
	 */
	void
	run_engine(
			enum crofchan_event_t event = EVENT_NONE);

	/**
	 *
	 */
	void
	event_established();

	/**
	 *
	 */
	void
	event_disconnected();

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofchan const& chan) {
		os << indent(0) << "<crofchan established:" << chan.is_established()
				<< " ofp-version: " << (int)chan.ofp_version << " >" << std::endl;
		indent i(2);
		os << chan.versionbitmap;
		for (std::map<cauxid, crofconn*>::const_iterator
				it = chan.conns.begin(); it != chan.conns.end(); ++it) {
			os << *(it->second);
		}
		return os;
	};
};

}; /* namespace rofl */




#endif /* CROFCHAN_H_ */
