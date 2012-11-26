/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cport.h"

/* static */std::set<cport_owner*> cport_owner::cport_owner_list;
/* static */ std::set<cport*> cport::cport_list;

cport::cport(
		std::string devname,
		std::string devtype,
		int port_no) :
	devname(devname),
	port_no(port_no),
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
	collisions(0xffffffff),
	config(0),
	state(0),
	curr(0),
	advertised(0),
	supported(0),
	peer(0),
	curr_speed(0),
	max_speed(0),
	//port_configuration(NULL),
	powner(NULL)
{
	pthread_mutex_init(&queuelock, NULL);
	cport::cport_list.insert(this);
	WRITELOG(CPORT, DBG, "cport(%p)::cport() %s", this, c_str());
}

cport::~cport()
{
	WRITELOG(CPORT, DBG, "cport(%p)::~cport() %s", this, c_str());

	cport::cport_list.erase(this);

	if (powner)
	{
		powner->port_detach(this);

		powner = NULL;
	}


	for (std::deque<cpacket*>::iterator
			it = pout_queue.begin(); it != pout_queue.end(); ++it)
	{
		delete (*it);
	}
	pout_queue.clear();

	pthread_mutex_destroy(&queuelock);
}


cport_owner*
cport::port_owner()
{
	cport_owner::exists(powner);
	return powner;
}


const char*
cport::c_str()
{
	cvastring vas;

#if 0
	info.assign(vas("cport(%p) name[%s] datapath[%s:%s] pout-queue[%u] "
			 "port-no[%d] hw-addr[%.2x:%.2x:%.2x:%.2x:%.2x:%.2x] "
			 "config[0x%x] state[0x%x] curr[0x%x] advertised[0x%x] "
			 "supported[0x%x] peer[0x%x]",
			 this,
			 devname.c_str(),
			 (datapath) ? datapath->dpname.c_str() : "",
			 (datapath) ? datapath->cfwdelem::c_str() : "",
			 (unsigned int)pout_queue.size(),
			 port_no,
			 hw_addr[0], hw_addr[1], hw_addr[2], hw_addr[3], hw_addr[4], hw_addr[5],
			 config,
			 state,
			 curr,
			 advertised,
			 supported,
			 peer));
#endif
	info.assign(vas("cport(%p) name[%s] pout-queue[%u] "
			 "port-no[%d] hw-addr[%s] ",
			 this,
			 devname.c_str(),
			 (unsigned int)pout_queue.size(),
			 port_no,
			 hwaddr.c_str()
			 ));


	return info.c_str();
}


/*static*/
uint32_t
cport::find_free_port_no(std::set<cport*> *port_list)
{
	crandom r(sizeof(uint32_t));

	while (true)
	{
		r.rand(sizeof(uint32_t));
		if (find_if(port_list->begin(), port_list->end(),
				cport_find_by_portno(r.uint32())) == port_list->end())
		{
			return r.uint32();
		}
	}
	return 0; // just to make gcc happy
}


/* static */
cport*
cport::find(const std::string& devname) throw (ePortNotFound)
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
cport::attach(cport_owner *__owner) throw (ePortIsAttached)
{
	if (NULL == __owner)
	{
		return;
	}

	if (NULL != powner)
	{
		throw ePortIsAttached();
	}

	powner = __owner;

	powner->port_attach(this); // insert this cport into cport_owner portlist

	WRITELOG(CPORT, DBG, "cport(%p)::attach() owner: %p", this, powner);

	try {
		cport_owner::exists(powner)->port_attach(this);
	} catch (ePortNotFound& e) { }
}


void
cport::detach() throw (ePortNotAttached)
{
	if (NULL == powner)
	{
		throw ePortNotAttached();
	}

	WRITELOG(CPORT, DBG, "cport(%p)::detach() owner: %p", this, powner);

	try {
		cport_owner::exists(powner)->port_detach(this);
	} catch (ePortNotFound& e) {}
}


void
cport::attached(cport_owner *owner)
{

}


void
cport::detached(cport_owner *owner)
{
	WRITELOG(CPORT, DBG, "cport::detached(): cport: %p owner: %p", this, owner);
	powner = NULL;
}


void
cport::enqueue(cpacket* pack)
{

	WRITELOG(CPORT, DBG, "cport(%s:%p)::enqueue(): port=%u, pack=%p",
			devname.c_str(), this, port_no, pack);

	//fprintf(stdout, "cport ENQUEUE pack:%p\n", pack);

	{
		Lock lock(&queuelock);
		pout_queue.push_back(pack);
		pack->time_cport_enqueue.now();

		WRITELOG(CPORT, DBG, "cport(%s:%p)::enqueue() ENQUEUE pout_queue.size():%d",
						devname.c_str(), this, pout_queue.size());

		// update port statistics
		tx_bytes += pack->length();
		tx_packets++;

		//register_timer(TIMER_CPORT_OUT_QUEUE, 0); // inform derived classes

#ifndef NDEBUG
		{
			Lock lock(&(event_mutex));

			std::string s_events;
			for (std::list<cevent*>::iterator
					it = events.begin(); it != events.end(); ++it)
			{
				cvastring vas(256);
				s_events.append(vas("%s ", (*it)->c_str()));
			}

			WRITELOG(CIOSRV, DBG, "cport(%s:%p)::enqueue() ENQUEUE-ACTUAL-EVENT-LIST: %s",
					devname.c_str(), this, s_events.c_str());
		}
#endif

#if 1
		//if (pflags.test(CPORT_FLAG_POUT_QUEUE) && (pout_queue.size() > 1))
		if (pflags.test(CPORT_FLAG_POUT_QUEUE))
		{
			WRITELOG(CPORT, DBG, "cport(%s:%p)::enqueue() ENQUEUE-FLAG-IS-SET",
					devname.c_str(), this);
			return; // event/timer POUT_QUEUE was already set
		}

		pflags.set(CPORT_FLAG_POUT_QUEUE);
#endif
	}

	if (tid == pthread_self())
	{
		WRITELOG(CPORT, DBG, "cport(%s:%p)::enqueue() ENQUEUE-FLAG-IS-NOT-SET waking up cport via timer",
						devname.c_str(), this);

		reset_timer(CPORT_TIMER_POUT_QUEUE, 0);
	}
	else
	{
		WRITELOG(CPORT, DBG, "cport(%s:%p)::enqueue() ENQUEUE-FLAG-IS-NOT-SET waking up cport via event",
						devname.c_str(), this);

		notify(cevent(CPORT_EVENT_OUT_QUEUE));
	}
}


void
cport::handle_timeout(int opaque)
{
	WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_timeout()",
			devname.c_str(), this);
	WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_timeout()",
			devname.c_str(), this);

	WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_timeout() timer expired: 0x%x",
			devname.c_str(), this, opaque);

	WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_timeout()",
			devname.c_str(), this);
	WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_timeout()",
			devname.c_str(), this);

	switch (opaque) {
	case CPORT_TIMER_POUT_QUEUE:
		//fprintf(stdout, "[7a] => %s ", devname.c_str());
		{
			Lock lock(&queuelock);
			pflags.reset(CPORT_FLAG_POUT_QUEUE);
			WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_timeout() ENQUEUE-RESET-FLAG",
							devname.c_str(), this);
		}
		//fprintf(stdout, "[7b] \n");
		handle_out_queue();
		break;
	default:
		break;
	}
}


void
cport::handle_event(cevent const& ev)
{
	WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_event()",
			devname.c_str(), this);
	WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_event()",
			devname.c_str(), this);

	WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_event() rcvd event: %s",
			devname.c_str(), this, cevent(ev).c_str());

	WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_event()",
			devname.c_str(), this);
	WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_event()",
			devname.c_str(), this);

	switch (ev.cmd) {
	case CPORT_EVENT_OUT_QUEUE:
		//fprintf(stdout, "[8a] => %s ", devname.c_str());
		{
			Lock lock(&queuelock);
			pflags.reset(CPORT_FLAG_POUT_QUEUE);
			WRITELOG(CPORT, DBG, "cport(%s:%p)::handle_event() ENQUEUE-RESET-FLAG",
							devname.c_str(), this);
		}
		//fprintf(stdout, "[8b] \n");
		//fprintf(stdout, "K");
		handle_out_queue();
		break;
	}
}


struct ofp_port_stats*
cport::get_port_stats(struct ofp_port_stats* port_stats, size_t port_stats_len)
{
	if (!port_stats || (port_stats_len < sizeof(struct ofp_port_stats)))
		return NULL;
	port_stats->rx_packets = rx_packets;
	port_stats->tx_packets = tx_packets;
	port_stats->rx_bytes = rx_bytes;
	port_stats->tx_bytes = tx_bytes;
	port_stats->rx_dropped = rx_dropped;
	port_stats->tx_dropped = tx_dropped;
	port_stats->rx_errors = rx_errors;
	port_stats->tx_errors = tx_errors;
	port_stats->rx_frame_err = rx_frame_err;
	port_stats->rx_over_err = rx_over_err;
	port_stats->rx_crc_err = rx_crc_err;
	port_stats->collisions = collisions;

	return port_stats;
}

uint32_t
cport::get_port_no()
{
	return port_no;
}

// TODO maybe set port_no on attach? and release port_no on detach??
const uint32_t
cport::set_port_no(const uint32_t port_no)
{
	WRITELOG(CPORT, DBG, "cport(%s:%p)::set_port_no(port_no=%d): this->port_no=%u",
			devname.c_str(), this, port_no, this->port_no);
	// check if port_no is already set:
	if (this->port_no) {
		return this->port_no;
	} else {
		return (this->port_no = port_no);
	}
}

void
cport::set_config(
		uint32_t config,
		uint32_t mask,
		uint32_t advertise)
{
	if (mask & OFPPC_PORT_DOWN)
	{
		if (config & OFPPC_PORT_DOWN)
		{
			WRITELOG(CPORT, DBG, "cport(%s:%d)::set_config()   set -PORT-DOWN-",
						devname.c_str(), port_no);
			this->config |= OFPPC_PORT_DOWN;
		}
		else
		{
			WRITELOG(CPORT, DBG, "cport(%s:%d)::set_config() reset -PORT-DOWN-",
						devname.c_str(), port_no);
			this->config &= ~OFPPC_PORT_DOWN;
		}
	}

	if (mask & OFPPC_NO_RECV)
	{
		if (config & OFPPC_NO_RECV)
		{
			WRITELOG(CPORT, DBG, "cport(%s:%d)::set_config()   set -NO-RECV-",
									devname.c_str(), port_no);
			this->config |= OFPPC_NO_RECV;
		}
		else
		{
			WRITELOG(CPORT, DBG, "cport(%s:%d)::set_config() reset -NO-RECV-",
									devname.c_str(), port_no);
			this->config &= ~OFPPC_NO_RECV;
		}
	}

	if (mask & OFPPC_NO_FWD)
	{
		if (config & OFPPC_NO_FWD)
		{
			WRITELOG(CPORT, DBG, "cport(%s:%d)::set_config()   set -NO-FWD-",
									devname.c_str(), port_no);
			this->config |= OFPPC_NO_FWD;
		}
		else
		{
			WRITELOG(CPORT, DBG, "cport(%s:%d)::set_config() reset -NO-FWD-",
									devname.c_str(), port_no);
			this->config &= ~OFPPC_NO_FWD;
		}
	}

	if (mask & OFPPC_NO_PACKET_IN)
	{
		if (config & OFPPC_NO_PACKET_IN)
		{
			WRITELOG(CPORT, DBG, "cport(%s:%d)::set_config()   set -NO-PACKET-IN-",
									devname.c_str(), port_no);
			this->config |= OFPPC_NO_PACKET_IN;
		}
		else
		{
			WRITELOG(CPORT, DBG, "cport(%s:%d)::set_config() reset -NO-PACKET-IN-",
									devname.c_str(), port_no);
			this->config &= ~OFPPC_NO_PACKET_IN;
		}
	}

	/*
	 * advertise
	 */
	if (0 != advertise)
	{
		WRITELOG(CPORT, DBG, "cport(%s:%d)::set_config() advertise:0x%x",
								devname.c_str(), port_no, advertise);
		this->advertised = advertise;
	}
}

uint32_t
cport::get_config()
{
	return config;
}

uint32_t
cport::get_curr()
{
	return curr;
}

uint32_t
cport::get_advertised()
{
	return advertised;
}

uint32_t
cport::get_supported()
{
	return supported;
}

uint32_t
cport::get_peer()
{
	return peer;
}

/*
 * cport owner related methods
 */

cport_owner::cport_owner()
{
	WRITELOG(CPORT, DBG, "cport_owner(%p)::cport_owner() -constructor-", this);
	cport_owner::cport_owner_list.insert(this);
}


cport_owner::~cport_owner()
{
	cport_owner::cport_owner_list.erase(this);
	WRITELOG(CPORT, DBG, "cport_owner(%p)::~cport_owner() -destructor-", this);

	for (std::map<cport*, std::deque<cpacket*> >::iterator
			it = pin_queue.begin(); it != pin_queue.end(); ++it)
	{
		std::deque<cpacket*>& pin_queue = it->second;

		for (std::deque<cpacket*>::iterator
				jt = pin_queue.begin(); jt != pin_queue.end(); ++jt)
		{
			delete (*jt);
		}
	}
}


const char*
cport_owner::c_str()
{
	cvastring vas;
	info.assign(vas("cport_owner(%p): =>", this));

	std::set<cport*>::iterator it;
	for (it = port_list.begin(); it != port_list.end(); ++it)
	{
		info.append(vas("\n  %s", (*it)->c_str()));
	}
	return info.c_str();
}


void
cport_owner::port_attach(cport *port)
{
	std::set<cport*>::iterator it;
	if ((it = port_list.find(port)) == port_list.end())
	{
		port_list.insert(port);
		//fprintf(stdout, "cport(%s)::port_attach()",	port->devname.c_str());
		WRITELOG(CPORT, DBG, "cport(%s)::port_attach()",
				port->devname.c_str());
		port->attached(this);

		handle_port_attach(port);
	}

	//fprintf(stdout, "cport_owner(%p): %s\n", this, c_str());
}


void
cport_owner::port_detach(cport *port)
{
	port_list.erase(port);

	for (std::deque<cpacket*>::iterator
			it = pin_queue[port].begin(); it != pin_queue[port].end(); ++it)
	{
		delete (*it);
	}
	pin_queue.erase(port);
	handle_port_detach(port);
}


void
cport_owner::handle_timeout(
		int opaque)
{
	switch (opaque) {
	case CPORT_OWNER_TIMER_PACKET_IN:
	{
		__handle_cport_packet_in();
		break;
	}
	default:
		break;
	}
}


void
cport_owner::handle_event(
		cevent const& ev)
{
	switch (ev.cmd) {
	case CPORT_OWNER_EVENT_PIN_QUEUE:
	{
		__handle_cport_packet_in();
		break;
	}
	default:
		break;
	}
}


void
cport_owner::__handle_cport_packet_in()
{
	flags.reset(CPORT_OWNER_FLAG_PIN_QUEUE);

	for (std::deque<cport*>::iterator
			it = pin_cports.begin(); it != pin_cports.end(); ++it)
	{
		cport *port = (*it);


		//fprintf(stdout, "cport_owner(%p)::handle_cport_packet_in() "
		//		"pin_queue[%s].size():%lu\n", this, port->devname.c_str(), pin_queue[port].size());

		handle_cport_packet_in(port, pin_queue[port]);

		pin_queue[port].clear();
	}
	pin_cports.clear(); // TODO: rethink thread-safety
}


void
cport_owner::store(cport *port, cpacket *pack)
{
	try {
		//fprintf(stdout, "cport_owner ENQUEUE pack:%p\n", pack);
		if ((cpacket*)0 == pack)
		{
			return;
		}

		port->rx_bytes += pack->length();
		port->rx_packets++;

		std::set<cport*>::iterator it;
		if (port_list.find(port) == port_list.end())
		{
			fprintf(stdout, "\n\n ==> cport_owner(%p)::enqueue() exception => %s\n", this, c_str());
			throw ePortNotFound();
		}

		pin_queue[port].push_back(pack);

	} catch (ePortNotFound& e) {
		delete pack;

	}
}


void
cport_owner::enqueue(cport *port)
{
	pin_cports.push_back(port);

	if (flags.test(CPORT_OWNER_FLAG_PIN_QUEUE))
	{
		return;
	}

	flags.set(CPORT_OWNER_FLAG_PIN_QUEUE);

	if (tid == pthread_self())
	{
		register_timer(CPORT_OWNER_TIMER_PACKET_IN, 0); // initiate packet handling
	}
	else
	{
		notify(cevent(CPORT_OWNER_EVENT_PIN_QUEUE));
	}
}




/*static*/
cport_owner*
exists(cport_owner *owner) throw (ePortNotFound)
{
	if (cport_owner::cport_owner_list.find(owner) == cport_owner::cport_owner_list.end())
	{
		throw ePortNotFound();
	}
	return owner;
};
