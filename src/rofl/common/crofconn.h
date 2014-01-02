/*
 * crofchan.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CROFCHAN_H_
#define CROFCHAN_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include "crofsock.h"
#include "openflow/cofhelloelems.h"
#include "openflow/cofhelloelemversionbitmap.h"

namespace rofl {
namespace openflow {

class crofconn; // forward declaration

class crofconn_env {
public:
	virtual ~crofconn_env() {};
	virtual void recv_message(cofmsg *msg) = 0;
};


class crofconn :
		public crofsock_env
{
	crofconn_env 					*env;
	uint8_t							auxiliary_id;
	crofsock						*rofsock;
	cofhello_elem_versionbitmap		versionbitmap; 			// supported OF versions by this entity
	cofhello_elem_versionbitmap		versionbitmap_peer;		// supported OF versions by peer entity


	enum msg_type_t {
		OFPT_HELLO = 0,
		OFPT_ERROR = 1,
		OFPT_ECHO_REQUEST = 2,
		OFPT_ECHO_REPLY = 3,
	};

public:

	/**
	 *
	 */
	crofconn(
			crofconn_env *env,
			uint8_t auxiliary_id,
			int sd,
			caddress const& ra);

	/**
	 *
	 */
	virtual ~crofconn();

private:

	virtual void
	handle_connect_refused(crofsock *rofsock);

	virtual void
	handle_open (crofsock *rofsock);

	virtual void
	handle_close(crofsock *rofsock);

	virtual void
	recv_message(crofsock *rofsock, cofmsg *msg);

private:


	/**
	 *
	 */
	void
	hello_rcvd(
			cofmsg *msg);

	/**
	 *
	 */
	void
	echo_request_rcvd(
			cofmsg *msg);

	/**
	 *
	 */
	void
	echo_reply_rcvd(
			cofmsg *msg);

	/**
	 *
	 */
	void
	error_rcvd(
			cofmsg *msg);

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofconn const& conn) {

		return os;
	};
};

}; /* namespace openflow */
}; /* namespace rofl */

#endif /* CROFCHAN_H_ */
