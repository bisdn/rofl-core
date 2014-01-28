/*
 * crofchan.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CROFCHAN_H_
#define CROFCHAN_H_

#include <map>

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include "croflexception.h"
#include "ciosrv.h"
#include "crofconn.h"
#include "openflow/messages/cofmsg.h"
#include "openflow/cofhelloelemversionbitmap.h"

namespace rofl {
namespace openflow {

class eRofChanBase			: public RoflException {};
class eRofChanNotFound		: public eRofChanBase {};
class eRofChanExists		: public eRofChanBase {};
class eRofChanInval			: public eRofChanBase {};

class crofchan; // forward declaration

class crofchan_env {
public:
	virtual ~crofchan_env() {};
	virtual void handle_established(crofchan *chan) = 0;
	virtual void handle_disconnected(crofchan *chan) = 0;
	virtual void recv_message(crofchan *chan, uint8_t aux_id, cofmsg *msg) = 0;
	virtual uint32_t get_async_xid(crofchan *chan) = 0;
	virtual uint32_t get_sync_xid(crofchan *chan, uint8_t msg_type = 0, uint16_t msg_sub_type = 0) = 0;
	virtual void release_sync_xid(crofchan *chan, uint32_t xid) = 0;
};

class crofchan :
		public crofconn_env,
		public ciosrv
{
	crofchan_env						*env;
	std::map<uint8_t, crofconn*>		conns;				// main and auxiliary connections
	cofhello_elem_versionbitmap			versionbitmap;		// supported OFP versions
	uint8_t								ofp_version;		// OFP version negotiated

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

	enum crofchan_timer_t {
		TIMER_RECONNECT			= 1,
	};

	int									reconnect_start_timeout;
	int 								reconnect_in_seconds; 	// reconnect in x seconds
	int 								reconnect_counter;

#define CROFCHAN_RECONNECT_START_TIMEOUT 1				// start reconnect timeout (default 1s)

public:

	/**
	 *
	 */
	crofchan(
			crofchan_env *env,
			cofhello_elem_versionbitmap const& versionbitmap);

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
	handle_connected(crofconn *conn, uint8_t ofp_version);

	virtual void
	handle_closed(crofconn *conn);

	virtual void
	recv_message(crofconn *conn, cofmsg *msg);

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
	cofhello_elem_versionbitmap&
	get_versionbitmap() { return versionbitmap; };

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	void
	add_conn(uint8_t aux_id, int domain, int type, int protocol, caddress const& ra);

	/**
	 *
	 */
	void
	add_conn(crofconn* conn, uint8_t aux_id);

	/**
	 *
	 */
	crofconn&
	get_conn(uint8_t aux_id);

	/**
	 *
	 */
	void
	drop_conn(uint8_t aux_id);

	/**
	 *
	 */
	void
	send_message(cofmsg *msg, uint8_t aux_id = 0);

private:

	/**
	 *
	 */
	void
	run_engine(enum crofchan_event_t event = EVENT_NONE);

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

	/**
	 *
	 */
	void
	backoff_reconnect(
			bool reset_timeout = false);

	/**
	 *
	 */
	virtual void
	handle_timeout(int opaque, void *data = (void*)0);

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofchan const& chan) {
		os << indent(0) << "<crofchan established:" << chan.is_established()
				<< " ofp-version: " << (int)chan.ofp_version << " >" << std::endl;
		indent i(2);
		os << chan.versionbitmap;
		for (std::map<uint8_t, crofconn*>::const_iterator
				it = chan.conns.begin(); it != chan.conns.end(); ++it) {
			os << *(it->second);
		}
		return os;
	};
};

}; /* namespace openflow */
}; /* namespace rofl */




#endif /* CROFCHAN_H_ */
