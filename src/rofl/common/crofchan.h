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

/**
 * @class Environment expected by a crofchan instance
 *
 */
class crofchan_env {
public:

	/**
	 * @brief	destructor
	 */
	virtual
	~crofchan_env() {};

	/**
	 * @brief	Called once the channel is connected and operational.
	 *
	 * Note: This method is executed in the thread context of the underlying
	 * socket instance.
	 *
	 * @param chan crofchan instance
	 */
	virtual void
	handle_established(crofchan *chan) = 0;

	/**
	 * @brief	Called upon loss of the control connection
	 *
	 * Note: This method is executed in the thread context of the underlying
	 * socket instance.
	 *
	 * @param chan crofchan instance
	 */
	virtual void
	handle_disconnected(crofchan *chan) = 0;

	/**
	 * @brief	Called after a congestion situation has been resolved.
	 *
	 * Note: This method is executed in the thread context of the underlying
	 * socket instance.
	 *
	 * @param chan crofchan instance
	 * @param auxid auxiliary connection that is usable again
	 */
	virtual void
	handle_write(crofchan *chan, const cauxid& auxid) = 0;

	/**
	 * @brief	Called upon reception of an OpenFlow message by the peer entity.
	 *
	 * Note: This method is executed in the thread context of the underlying
	 * socket instance.
	 *
	 * @param chan crofchan instance
	 * @param auxid auxiliary connection
	 * @param msg pointer to cofmsg instance
	 */
	virtual void
	recv_message(crofchan *chan, const cauxid& aux_id, rofl::openflow::cofmsg *msg) = 0;

	/**
	 * @brief	Acquires an OpenFlow transaction ID for an asynchronous message.
	 *
	 * @param chan crofchan instance
	 */
	virtual uint32_t
	get_async_xid(crofchan *chan) = 0;

	/**
	 * @brief	Acquires an OpenFlow transaction ID for a synchronous message.
	 *
	 * @param chan crofchan instance
	 */
	virtual uint32_t
	get_sync_xid(crofchan *chan, uint8_t msg_type = 0, uint16_t msg_sub_type = 0) = 0;

	/**
	 * @brief	Releases a synchronous transaction ID after reception of an OpenFlow reply.
	 *
	 * @param chan crofchan instance
	 */
	virtual void
	release_sync_xid(crofchan *chan, uint32_t xid) = 0;
};



class crofchan :
		public crofconn_env
{
	enum crofchan_event_t {
		EVENT_NONE				= 0,
		EVENT_DISCONNECTED		= 1,
		EVENT_ESTABLISHED		= 2,
	};

	enum crofchan_state_t {
		STATE_DISCONNECTED 		= 1,
		STATE_CONNECT_PENDING	= 2,
		STATE_ESTABLISHED 		= 3,
	};

public:

	/**
	 *
	 */
	crofchan() :
		env(NULL),
		ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN),
		state(STATE_DISCONNECTED)
	{};

	/**
	 *
	 */
	crofchan(
			crofchan_env *env,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap) :
				env(env),
				versionbitmap(versionbitmap),
				ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN),
				state(STATE_DISCONNECTED)
	{};

	/**
	 *
	 */
	virtual
	~crofchan() {
		close();
	};

public:

	/**
	 *
	 */
	bool
	is_established() const;

public:

	/**
	 *
	 */
	virtual void
	handle_connect_refused(crofconn *conn) {
		rofl::logging::warn << "[rofl-common][crofchan] OFP transport "
				<< "connection refused. " << conn->str() << std::endl;
		conn->reconnect();
	};

	/**
	 *
	 */
	virtual void
	handle_connect_failed(crofconn *conn) {
		rofl::logging::warn << "[rofl-common][crofchan] OFP transport "
				"connection failed. " << conn->str() << std::endl;
		conn->reconnect();
	};

	/**
	 *
	 */
	virtual void
	handle_connected(crofconn *conn, uint8_t ofp_version);

	/**
	 *
	 */
	virtual void
	handle_closed(crofconn *conn);

	/**
	 *
	 */
	virtual void
	handle_write(crofconn *conn) {
		env->handle_write(this, conn->get_aux_id());
	};

	virtual void
	recv_message(crofconn *conn, rofl::openflow::cofmsg *msg) {
		env->recv_message(this, conn->get_aux_id(), msg);
	};

	/**
	 *
	 */
	virtual uint32_t
	get_async_xid(crofconn *conn) {
		return env->get_async_xid(this);
	};

	/**
	 *
	 */
	virtual uint32_t
	get_sync_xid(crofconn *conn, uint8_t msg_type = 0, uint16_t msg_sub_type = 0) {
		return env->get_sync_xid(this, msg_type, msg_sub_type);
	};

	/**
	 *
	 */
	virtual void
	release_sync_xid(crofconn *conn, uint32_t xid) {
		env->release_sync_xid(this, xid);
	};

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

private:

	// owner of this crofchan instance
	crofchan_env						*env;
	// main and auxiliary connections
	std::map<cauxid, crofconn*>			conns;
	// supported OFP versions
	rofl::openflow::cofhello_elem_versionbitmap
										versionbitmap;
	// OFP version negotiated
	uint8_t								ofp_version;
	// state related flags
	std::bitset<32>						flags;
	// event queue
	std::deque<enum crofchan_event_t> 	events;
	// current state
	enum crofchan_state_t				state;
};

}; /* namespace rofl */

#endif /* CROFCHAN_H_ */
