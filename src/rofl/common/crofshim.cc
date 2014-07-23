/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofshim.cc
 *
 *  Created on: 22.01.2014
 *      Author: andi
 */

#include "rofl/common/crofshim.h"

using namespace rofl::openflow;

crofshim::crofshim() :
		rofsockid(0)
{

}



crofshim::~crofshim()
{
	for (std::map<rofl::caddress, csocket*>::iterator
			it = sockets.begin(); it != sockets.end(); ++it) {
		delete it->second;
	}
	for (std::map<unsigned int, crofsock*>::iterator
			it = rofsocks.begin(); it != rofsocks.end(); ++it) {
		delete it->second;
	}
}



void
crofshim::listen(enum rofl::csocket::socket_type_t socket_type, cparams const& socket_params, int backlog)
{
#if 0
	if (sockets.find(laddr) != sockets.end()) {
		return;
	}
	(sockets[laddr] = csocket::csocket_factory(socket_type, this))->listen(socket_params, backlog);
#endif
}



void
crofshim::close(rofl::caddress const& laddr)
{
	if (sockets.find(laddr) == sockets.end()) {
		return;
	}
	delete sockets[laddr];
	sockets.erase(laddr);
}



void
crofshim::handle_listen(csocket& socket, int newsd)
{
	(new crofsock(this))->accept(socket.get_socket_type(), socket.get_socket_params(), newsd);
}



void
crofshim::connect(enum rofl::csocket::socket_type_t socket_type, cparams const& socket_params)
{
	(new crofsock(this))->connect(socket_type, socket_params);
}



void
crofshim::handle_connect_refused(crofsock *rofsock)
{
	delete rofsock;
}



void
crofshim::handle_connected(crofsock *rofsock)
{
	while (rofsocks.find(rofsockid) != rofsocks.end()) {
		++rofsockid;
	}
	rofsocks[rofsockid] = rofsock;
}



void
crofshim::handle_closed(crofsock *rofsock)
{
	for (std::map<unsigned int, crofsock*>::iterator
			it = rofsocks.begin(); it != rofsocks.end(); ++it) {
		if (it->second == rofsock) {
			delete rofsock;
			rofsocks.erase(it);
			return;
		}
	}
}




