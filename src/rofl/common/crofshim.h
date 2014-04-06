/*
 * crofshim.h
 *
 *  Created on: 22.01.2014
 *      Author: andi
 */

#ifndef CROFSHIM_H_
#define CROFSHIM_H_

#include <map>

#include "rofl/common/croflexception.h"
#include "rofl/common/crofsock.h"
#include "rofl/common/csocket.h"
#include "rofl/common/caddress.h"

namespace rofl {
namespace openflow {

class crofshim :
		public crofsock_env,
		public csocket_owner
{
	std::map<rofl::caddress, csocket*>	sockets;		// listening csocket instances
	unsigned int 						rofsockid;		// crofsock identifiers
	std::map<unsigned int, crofsock*> 	rofsocks;		// crofsock instances

public:

	/**
	 *
	 */
	crofshim();

	/**
	 *
	 */
	virtual
	~crofshim();

	/**
	 *
	 */
	void
	connect(enum rofl::csocket::socket_type_t socket_type, cparams const& socket_params);

	/**
	 *
	 */
	void
	listen(enum rofl::csocket::socket_type_t socket_type, cparams const& socket_params, int backlog = 10);

	/**
	 *
	 */
	void
	close(rofl::caddress const& laddr);

protected:

	/**
	 *
	 */
	virtual void
	handle_message(
			crofsock *rofsock, cofmsg *msg) { /* to be overwritten */ delete msg; };

	/**
	 *
	 */
	virtual void
	send_message(
			crofsock *rofsock, cofmsg *msg) { rofsock->send_message(msg); };

private:

	virtual void
	handle_connect_refused(crofsock *rofsock);

	virtual void
	handle_connected(crofsock *rofsock);

	virtual void
	handle_closed(crofsock *rofsock);

	virtual void
	recv_message(crofsock *rofsock, cofmsg *msg) { handle_message(rofsock, msg); }

private:

	virtual void
	handle_accepted(csocket& socket, int newsd, caddress const& ra);

	virtual void
	handle_connected(csocket& socket) {};

	virtual void
	handle_connect_refused(csocket& socket) {};

	virtual void
	handle_read(csocket& socket) {};

	virtual void
	handle_closed(csocket& socket) {};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* CROFSHIM_H_ */

