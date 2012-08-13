/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cutbase.h"

/*static*/std::set<cutbase*> cutbase::utlist;


cutbase::cutbase(std::string name) :
		cfwdelem(name)
{
	cutbase::utlist.insert(this);
}


cutbase::~cutbase()
{
	cutbase::utlist.erase(this);
}


void
cutbase::handle_timeout(int opaque)
{
	switch (opaque) {
	default:
		cport::cport_owner::handle_timeout(opaque);
		cfwdelem::handle_timeout(opaque);
		break;
	}
}


void
cutbase::handle_cport_packet_in(
		cport *port,
		cpacket *pack)
{
	delete pack;
}


void
cutbase::test_reset()
{
	std::map<cofbase*, cofdpath*>::iterator it;
	for (it = ofdpath_list.begin(); it != ofdpath_list.end(); ++it)
	{
		it->second->flow_mod_reset();
		it->second->group_mod_reset();
	}
}


const char*
cutbase::c_str()
{
	cvastring vas;
	info.assign(vas("cunittest(%p)", this));

	return info.c_str();
}


