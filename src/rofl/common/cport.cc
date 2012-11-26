/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cport.h"

/* static */ std::set<cport*> cport::cport_list;



cport::cport(
		cport_owner *owner,
		std::string devname,
		std::string devtype) :
			max_out_queue_size(CPORT_DEFAULT_MAX_OUT_QUEUE_SIZE),
			owner(owner),
			devname(devname),
			devtype(devtype),
			rx_packets(0),
			tx_packets(0),
			rx_bytes(0),
			tx_bytes(0),
			rx_dropped(0),
			tx_dropped(0),
			rx_errors(0),
			tx_errors(0),
			rx_frame_err(0xffffffff),
			rx_over_err(0xffffffff),
			rx_crc_err(0xffffffff),
			collisions(0xffffffff)
{
	cport::cport_list.insert(this);
	WRITELOG(CPORT, DBG, "cport(%p)::cport() %s", this, c_str());
}



cport::~cport()
{
	WRITELOG(CPORT, DBG, "cport(%p)::~cport() %s", this, c_str());

	cport::cport_list.erase(this);

	if (owner)
	{
		owner->port_destroy(this);

		owner = NULL;
	}
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
cport::enqueue(
		cpacket* pkt)
{
	pout_queue.push_back(pkt);

	while (pout_queue.size() >= max_out_queue_size)
	{
		WRITELOG(CPORT, DBG, "cport(%s)::enqueue() shrinking pout_queue: %lu",
						devname.c_str(), pout_queue.size());

		delete pout_queue.front(); pout_queue.pop_front();
	}

	cport::handle_out_queue(); // call derived cport class
}



void
cport::enqueue(
		std::deque<cpacket*>& pktlist)
{

	WRITELOG(CPORT, DBG, "cport(%s:%p)::enqueue() ",
			devname.c_str(), this);


	for (std::deque<cpacket*>::iterator
			it = pktlist.begin(); it != pktlist.end(); ++it)
	{
		pout_queue.push_back((*it));
	}
	pktlist.clear();

	while (pout_queue.size() >= max_out_queue_size)
	{
		WRITELOG(CPORT, DBG, "cport(%s)::enqueue() shrinking pout_queue: %lu",
						devname.c_str(), pout_queue.size());

		delete pout_queue.front(); pout_queue.pop_front();
	}

	cport::handle_out_queue();
}



struct ofp_port_stats*
cport::get_port_stats(
		struct ofp_port_stats* port_stats,
		size_t port_stats_len)
{
	if (!port_stats || (port_stats_len < sizeof(struct ofp_port_stats)))
	{
		return NULL;
	}

	port_stats->rx_packets 	= rx_packets;
	port_stats->tx_packets 	= tx_packets;
	port_stats->rx_bytes 	= rx_bytes;
	port_stats->tx_bytes 	= tx_bytes;
	port_stats->rx_dropped 	= rx_dropped;
	port_stats->tx_dropped 	= tx_dropped;
	port_stats->rx_errors 	= rx_errors;
	port_stats->tx_errors 	= tx_errors;
	port_stats->rx_frame_err = rx_frame_err;
	port_stats->rx_over_err = rx_over_err;
	port_stats->rx_crc_err 	= rx_crc_err;
	port_stats->collisions 	= collisions;

	return port_stats;
}






