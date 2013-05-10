/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cport.h"

using namespace rofl;

/* static */ std::set<cport*> cport::cport_list;

cport::cport(
		cport_owner *owner,
		std::string devname,
		std::string devtype) :
			max_out_queue_size(CPORT_DEFAULT_MAX_OUT_QUEUE_SIZE),
			owner(owner),
			devname(devname),
			devtype(devtype)
{
	cport::cport_list.insert(this);
	WRITELOG(CPORT, DBG, "cport(%s)::cport() %s", devname.c_str(), c_str());

#if 0
	if (owner) {
		owner->port_init(this);
	}
#endif
}



cport::~cport()
{
	WRITELOG(CPORT, DBG, "cport(%p)::~cport() %s", this, c_str());

	cport::cport_list.erase(this);

#if 0
	if (owner) {
		owner->port_destroy(this);

		owner = NULL;
	}
#endif
}



const char*
cport::c_str()
{
	cvastring vas;

	info.assign(vas("cport(%p) name: %s type: %s",
			 this,
			 devname.c_str(),
			 devtype.c_str()
			 ));

	return info.c_str();
}



/* static */
cport*
cport::find(
		std::string const& devname) throw (ePortNotFound)
{
	std::set<cport*>::iterator it;
	if ((it = find_if(cport_list.begin(), cport_list.end(),
			cport::cport_find_by_name(devname))) == cport_list.end())
	{
		throw ePortNotFound();
	}
	return (*it);
}



void
cport::drop_packets()
{
	while (pout_queue.size() >= max_out_queue_size) {
		WRITELOG(CPORT, DBG, "cport(%s)::drop_packets() shrinking pout_queue: %lu",
						devname.c_str(), pout_queue.size());
		delete pout_queue.front(); pout_queue.pop_front();
	}
}



void
cport::enqueue(
		cpacket* pkt)
{
	drop_packets();

	pout_queue.push_back(pkt);

	handle_out_queue(); // call derived cport class
}



void
cport::enqueue(
		std::deque<cpacket*>& pktlist)
{
	WRITELOG(CPORT, DBG, "cport(%s:%p)::enqueue() ",
			devname.c_str(), this);

	drop_packets();

	for (std::deque<cpacket*>::iterator
			it = pktlist.begin(); it != pktlist.end(); ++it) {
		pout_queue.push_back((*it));
	}
	pktlist.clear();

	handle_out_queue();
}





cofport_stats_reply&
cport::get_port_stats()
{
	return port_stats;
}






