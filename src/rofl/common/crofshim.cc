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
crofshim::listen(rofl::caddress const& laddr, int domain, int type, int protocol, int backlog)
{
	if (sockets.find(laddr) != sockets.end()) {
		return;
	}
	(sockets[laddr] = new csocket(this))->listen(laddr, domain, type, protocol, backlog);
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
crofshim::handle_accepted(csocket& socket, int newsd, caddress const& ra)
{
	(new crofsock(this))->accept(newsd);
}



void
crofshim::connect(rofl::caddress const& raddr, int domain, int type, int protocol)
{
	(new crofsock(this))->connect(domain, type, protocol, raddr);
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




