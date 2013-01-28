/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofport.h"

using namespace rofl;

/* static */
void
cofport::ports_parse(
		std::map<uint32_t, cofport*>& portsmap,
		struct ofp_port *ports, // ptr to array of ofp_phy_ports
		int portslen) // number of bytes in array
	throw (eOFportMalformed)
{
	//std::map<uint32_t, cofport*> __ports;

	if (portslen == 0)
	{
		//return __ports;
		return;
	}

	// sanity check: portslen must be of size at least of ofp_phy_port
	if (portslen < (int)sizeof(struct ofp_port))
	{
		throw eOFportMalformed();
	}

	// first struct ofp_phy_port
	struct ofp_port *phdr = ports;

	while (portslen > 0)
	{
		if (portslen < (int)sizeof(struct ofp_port))
		{
			throw eOFportMalformed();
		}

		cofport *ofport = new cofport(&portsmap, be32toh(phdr->port_no), phdr, sizeof(struct ofp_port));

		//__ports[ofport->port_no] = ofport;
		portsmap[ofport->port_no] = ofport;

		phdr++;
		portslen -= sizeof(struct ofp_port);
	}
	//return __ports;
}


/*static*/ uint32_t
cofport::ports_get_free_port_no(
	std::map<uint32_t, cofport*> *port_list) throw (eOFportNotFound)
{
	uint32_t port_no = 1;

	while (port_list->find(port_no) != port_list->end())
	{
		port_no++;
		if (port_no == std::numeric_limits<uint32_t>::max())
		{
			throw eOFportNotFound();
		}
	}
	return port_no;
}


cofport::cofport(
	std::map<uint32_t, cofport*> *port_list,
	uint32_t portno,
	struct ofp_port *port,
	size_t port_len) :
			port_no(portno),
			hwaddr(cmacaddr("00:00:00:00:00:00")),
			name(std::string("")),
			config(0),
			state(0),
			curr(0),
			advertised(0),
			supported(0),
			peer(0),
			curr_speed(0),
			max_speed(0),
			port_list(port_list)
{
	reset_stats();
	if ((0 != port) && (port_len >= sizeof(struct ofp_port)))
	{
		unpack(port, port_len);
	}

	if (0 != port_list)
	{
		(*port_list)[port_no] = this;
	}

	WRITELOG(CPORT, DBG, "cofport(%p)::cofport() port_list:%p %s", this, port_list, c_str());
}


cofport::cofport(
		struct ofp_port* port,
		size_t port_len) :
			port_no(0),
			hwaddr(cmacaddr("00:00:00:00:00:00")),
			name(std::string("")),
			config(0),
			state(0),
			curr(0),
			advertised(0),
			supported(0),
			peer(0),
			curr_speed(0),
			max_speed(0),
			port_list(0)
{
	reset_stats();
	if ((0 != port) && (port_len >= sizeof(struct ofp_port)))
	{
		unpack(port, port_len);
	}
	WRITELOG(CPORT, DBG, "cofport(%p)::cofport() port_list:%p %s", this, port_list, c_str());
}


cofport::~cofport()
{
	WRITELOG(CPORT, DBG, "cofport(%p)::~cofport() %s", this, c_str());

	WRITELOG(CPORT, DBG, "cofport(%p)::~cofport() port_list:%p ", this, port_list);

	if (0 != port_list)
	{
		port_list->erase(port_no);
	}
}



cofport::cofport(cofport const& port)
{
	*this = port;
}


cofport&
cofport::operator= (cofport const& port)
{
	if (this == &port)
		return *this;

	reset_stats();

	WRITELOG(CPORT, DBG, "cofport(%p)::operator=() from port:%p", this, &port);

	port_list 		= 0;
	port_no 		= port.port_no;
	hwaddr 			= port.hwaddr;
	name 			= port.name;
	config 			= port.config;
	state 			= port.state;
	curr 			= port.curr;
	advertised 		= port.advertised;
	supported 		= port.supported;
	peer 			= port.peer;
	curr_speed 		= port.curr_speed;
	max_speed 		= port.max_speed;

	return *this;
}



void
cofport::recv_port_mod(
		uint32_t config,
		uint32_t mask,
		uint32_t advertise)
{
	if (mask & OFPPC_PORT_DOWN)
	{
		if (config & OFPPC_PORT_DOWN)
		{
			this->config |= OFPPC_PORT_DOWN;
		}
		else
		{
			this->config &= ~OFPPC_PORT_DOWN;
		}
	}

	if (mask & OFPPC_NO_RECV)
	{
		if (config & OFPPC_NO_RECV)
		{
			this->config |= OFPPC_NO_RECV;
		}
		else
		{
			this->config &= ~OFPPC_NO_RECV;
		}
	}

	if (mask & OFPPC_NO_PACKET_IN)
	{
		if (config & OFPPC_NO_PACKET_IN)
		{
			this->config |= OFPPC_NO_PACKET_IN;
		}
		else
		{
			this->config &= ~OFPPC_NO_PACKET_IN;
		}
	}

	if (mask & OFPPC_NO_FWD)
	{
		if (config & OFPPC_NO_FWD)
		{
			this->config |= OFPPC_NO_FWD;
		}
		else
		{
			this->config &= ~OFPPC_NO_FWD;
		}
	}

	if (0 != advertise)
	{
		this->advertised = advertise;
	}

	WRITELOG(CPORT, DBG, "cofport(%s:%d)::recv_port_mod() config:0x%x advertise:0x%x",
			name.c_str(), port_no, this->config, this->advertised);
}



size_t
cofport::length()
{
	return sizeof(struct ofp_port);
}



const char*
cofport::c_str()
{
	cvastring vas;

	info.assign(vas("cofport(%p): port_no:%d hwaddr:%s name:%s "
			"config:%d state:%d curr:%d advertised:%d "
			"supported:%d peer:%d curr_speed:%d max_speed:%d",
			this,
			port_no,
			hwaddr.c_str(),
			name.c_str(),
			config,
			state,
			curr,
			advertised,
			supported,
			peer,
			curr_speed,
			max_speed));

	return info.c_str();
}


struct ofp_port*
cofport::pack(
	struct ofp_port* port, size_t portlen)
throw (eOFportInval)
{
	if (portlen < sizeof(struct ofp_port))
	{
		throw eOFportInval();
	}

	port->port_no 		= htobe32(port_no);

	memcpy(port->hw_addr, hwaddr.somem(), OFP_ETH_ALEN);
	memset(port->name, 0, OFP_MAX_PORT_NAME_LEN);
	size_t namelen = name.length() > (OFP_MAX_PORT_NAME_LEN - 1) ?
			OFP_MAX_PORT_NAME_LEN - 1 : name.length();
	memcpy(port->name, name.c_str(), namelen);

	port->config 		= htobe32(config);
	port->state 		= htobe32(state);
	port->curr 			= htobe32(curr);
	port->advertised 	= htobe32(advertised);
	port->supported 	= htobe32(supported);
	port->peer 			= htobe32(peer);
	port->curr_speed 	= htobe32(curr_speed);
	port->max_speed 	= htobe32(max_speed);

	return port;
}


struct ofp_port*
cofport::unpack(
	struct ofp_port* port, size_t portlen)
throw (eOFportInval)
{
	if (portlen < sizeof(struct ofp_port))
	{
		throw eOFportInval();
	}

	port_no 	= be32toh(port->port_no);

	hwaddr = cmacaddr(port->hw_addr, OFP_ETH_ALEN);
	//memcpy(hwaddr.somem(), port->hw_addr, OFP_ETH_ALEN);
	name.assign(port->name, OFP_MAX_PORT_NAME_LEN);

	config 		= be32toh(port->config);
	state 		= be32toh(port->state);
	curr 		= be32toh(port->curr);
	advertised 	= be32toh(port->advertised);
	supported 	= be32toh(port->supported);
	peer 		= be32toh(port->peer);
	curr_speed 	= be32toh(port->curr_speed);
	max_speed 	= be32toh(port->max_speed);

	return port;
}





void
cofport::reset_stats()
{
	rx_packets 		= 0;
	tx_packets 		= 0;
	rx_bytes 		= 0;
	tx_bytes 		= 0;
	rx_dropped 		= 0;
	tx_dropped 		= 0;
	rx_errors 		= 0;
	tx_errors 		= 0;
	rx_frame_err 	= 0;
	rx_over_err 	= 0;
	rx_crc_err 		= 0;
	collisions 		= 0;
}


void
cofport::get_port_stats(
		cmemory& body)
{
	cmemory pstats(sizeof(struct ofp_port_stats));
	struct ofp_port_stats* stats = (struct ofp_port_stats*)pstats.somem();

	stats->port_no					= htobe32(port_no);
	stats->rx_packets				= htobe32(rx_packets);
	stats->tx_packets				= htobe32(tx_packets);
	stats->rx_bytes					= htobe32(rx_bytes);
	stats->tx_bytes					= htobe32(tx_bytes);
	stats->rx_dropped				= htobe32(rx_dropped);
	stats->tx_dropped				= htobe32(tx_dropped);
	stats->rx_errors				= htobe32(rx_errors);
	stats->tx_errors				= htobe32(tx_errors);
	stats->rx_frame_err				= htobe32(rx_frame_err);
	stats->rx_over_err				= htobe32(rx_over_err);
	stats->rx_crc_err				= htobe32(rx_crc_err);
	stats->collisions				= htobe32(collisions);

	body += pstats;
}



/*static*/
void
cofport::test()
{
	cofport p1;

	p1.port_no = 1;
	p1.config = 10;
	p1.curr = 2;
	p1.hwaddr = cmacaddr("00:11:11:11:11:11");
	p1.curr_speed = 0xdeadbeef;
	p1.max_speed = 0xdeafdeaf;

	fprintf(stderr, "p1 => %s\n", p1.c_str());

	cmemory mem(sizeof(struct ofp_port));

	p1.pack((struct ofp_port*)mem.somem(), mem.memlen());

	fprintf(stderr, "p1.packed => %s\n", mem.c_str());

	cofport p2;

	p2.unpack((struct ofp_port*)mem.somem(), mem.memlen());

	fprintf(stderr, "p1.unpacked => %s\n", p2.c_str());
}

template class coflist<cofport>;


