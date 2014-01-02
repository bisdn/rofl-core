/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cunixport.h"

using namespace rofl;

std::set<cunixport*> cunixport::cunixport_list;

cunixport::cunixport(
		cport_owner *owner,
		std::string devname) :
		csocket(0, PF_UNIX, SOCK_DGRAM, IPPROTO_UDP, 10),
		clinuxport(owner, devname, std::string("vport")),
		baddr(AF_UNIX, devname.c_str())
{
	clisten(baddr, PF_UNIX, SOCK_DGRAM, 0);
	cunixport::cunixport_list.insert(this);
}

cunixport::~cunixport()
{
	cunixport::cunixport_list.erase(this);
}



void
cunixport::handle_read(int fd)
{
	class cpacket *pack = new cpacket();

	try {
		cmemory *mem = new cmemory(1526);

		int rc = read(sd, (void*)mem->somem(), mem->memlen());

		// error occured (or non-blocking)
		if (rc < 0)
		{
			switch (errno) {
			case EAGAIN:
				break;
			default:
				// after this, you should return and do nothing with this object any more
				delete this; // trigger removal of this object
				return;
			}

			throw eSocketReadFailed();

		}
		else if (rc == 0)
		{
			delete this; return;
		}
		else
		{
			owner->enqueue(this, new cpacket(mem));
		}

	} catch (eSocketReadFailed& e) {
		delete pack;

	} catch (ePortNotFound& e) {
		delete pack;
	}

}



cunixport*
cunixport::find(std::string devname)
{
	std::set<cunixport*>::iterator it;
	if ((it = find_if(cunixport::cunixport_list.begin(),
					  cunixport::cunixport_list.end(),
					  cunixport::cunixport_search(devname))) == cunixport::cunixport_list.end())
		return NULL;
	return(*it);
}


void
cunixport::handle_out_queue()
{
	throw ("cunixport::handle_out_queue() implement me!");
}
