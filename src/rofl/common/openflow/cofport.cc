/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofport.h"

using namespace rofl;





/* static */
void
cofport::ports_parse(
		std::map<uint32_t, cofport*>& portsmap,
		struct openflow10::ofp_port *ports, // ptr to array of ofp_phy_ports
		int portslen) // number of bytes in array
	throw (eOFportMalformed)
{
	if (0 == ports) {
		return;
	}

	// sanity check: portslen must be of size at least of openflow10::ofp_port
	if (sizeof(struct openflow10::ofp_port) <= (unsigned int)portslen) {
		throw eOFportMalformed();
	}

	// first struct ofp_phy_port
	struct openflow10::ofp_port *phdr = ports;

	while (portslen > 0) {
		if (portslen < (int)sizeof(struct openflow10::ofp_port)) {
			throw eOFportMalformed();
		}
		cofport *ofport = new cofport(phdr, sizeof(struct openflow10::ofp_port), &portsmap, be16toh(phdr->port_no));
		portsmap[ofport->get_port_no()] = ofport;
		phdr++;
		portslen -= sizeof(struct openflow10::ofp_port);
	}
}



/* static */
void
cofport::ports_parse(
		std::map<uint32_t, cofport*>& portsmap,
		struct openflow12::ofp_port *ports, // ptr to array of openflow12::ofp_ports
		int portslen) // number of bytes in array
	throw (eOFportMalformed)
{
	if (0 == ports) {
		return;
	}

	// sanity check: portslen must be of size at least of openflow12::ofp_port
	if (sizeof(struct openflow12::ofp_port) <= (unsigned int)portslen) {
		throw eOFportMalformed();
	}

	// first struct ofp_phy_port
	struct openflow12::ofp_port *phdr = ports;

	while (portslen > 0) {
		if (portslen < (int)sizeof(struct openflow12::ofp_port)) {
			throw eOFportMalformed();
		}
		cofport *ofport = new cofport(phdr, sizeof(struct openflow12::ofp_port), &portsmap, be32toh(phdr->port_no));
		portsmap[ofport->get_port_no()] = ofport;
		phdr++;
		portslen -= sizeof(struct openflow12::ofp_port);
	}
}



/* static */
void
cofport::ports_parse(
		std::map<uint32_t, cofport*>& portsmap,
		struct openflow13::ofp_port *ports, // ptr to array of openflow13::ofp_ports
		int portslen) // number of bytes in array
	throw (eOFportMalformed)
{
	if (0 == ports) {
		return;
	}

	// sanity check: portslen must be of size at least of openflow13::ofp_port
	if (sizeof(struct openflow13::ofp_port) <= (unsigned int)portslen) {
		throw eOFportMalformed();
	}

	// first struct ofp_phy_port
	struct openflow13::ofp_port *phdr = ports;

	while (portslen > 0) {
		if (portslen < (int)sizeof(struct openflow13::ofp_port)) {
			throw eOFportMalformed();
		}
		cofport *ofport = new cofport(phdr, sizeof(struct openflow13::ofp_port), &portsmap, be32toh(phdr->port_no));
		portsmap[ofport->get_port_no()] = ofport;
		phdr++;
		portslen -= sizeof(struct openflow13::ofp_port);
	}
}



#if 0
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
#endif



cofport::cofport(uint8_t of_version) :
		of_version(of_version),
		port_list(0),
		port_stats(of_version)
{
	reset_stats();
	switch (of_version) {
	case openflow10::OFP_VERSION: memarea.resize(sizeof(struct openflow10::ofp_port)); ofh10_port = (struct openflow10::ofp_port*)memarea.somem(); break;
	case openflow12::OFP_VERSION: memarea.resize(sizeof(struct openflow12::ofp_port)); ofh12_port = (struct openflow12::ofp_port*)memarea.somem(); break;
	case openflow13::OFP_VERSION: memarea.resize(sizeof(struct openflow13::ofp_port)); ofh13_port = (struct openflow13::ofp_port*)memarea.somem(); break;
	default: ofh10_port = (struct openflow10::ofp_port*)0; break;
	}
}



cofport::cofport(
	struct openflow10::ofp_port *port,
	size_t port_len,
	std::map<uint32_t, cofport*> *port_list,
	uint32_t portno) :
			of_version(openflow10::OFP_VERSION),
			port_list(port_list),
			memarea(sizeof(struct openflow10::ofp_port)),
			port_stats(openflow10::OFP_VERSION)
{
	ofh10_port = (struct openflow10::ofp_port*)memarea.somem();

	reset_stats();

	if ((0 != port) && (port_len >= sizeof(struct openflow10::ofp_port))) {
		unpack(port, port_len);
	}

	if (0 != port_list) {
		(*port_list)[portno] = this;
	}
}



cofport::cofport(
	struct openflow12::ofp_port *port,
	size_t port_len,
	std::map<uint32_t, cofport*> *port_list,
	uint32_t portno) :
			of_version(openflow12::OFP_VERSION),
			port_list(port_list),
			memarea(sizeof(struct openflow12::ofp_port)),
			port_stats(openflow12::OFP_VERSION)
{
	ofh12_port = (struct openflow12::ofp_port*)memarea.somem();

	reset_stats();

	if ((0 != port) && (port_len >= sizeof(struct openflow12::ofp_port))) {
		unpack(port, port_len);
	}

	if (0 != port_list) {
		(*port_list)[portno] = this;
	}
}



cofport::cofport(
	struct openflow13::ofp_port *port,
	size_t port_len,
	std::map<uint32_t, cofport*> *port_list,
	uint32_t portno) :
			of_version(openflow13::OFP_VERSION),
			port_list(port_list),
			memarea(sizeof(struct openflow13::ofp_port)),
			port_stats(openflow13::OFP_VERSION)
{
	ofh13_port = (struct openflow13::ofp_port*)memarea.somem();

	reset_stats();

	if ((0 != port) && (port_len >= sizeof(struct openflow13::ofp_port))) {
		unpack(port, port_len);
	}

	if (0 != port_list) {
		(*port_list)[portno] = this;
	}
}



cofport::~cofport()
{
	if ((std::map<uint32_t, cofport*>*)0 != port_list) {

		port_list->erase(get_port_no());
	}
}



cofport::cofport(cofport const& port) :
		port_list(0)
{
	*this = port;
}



cofport::cofport(cofport const& port, std::map<uint32_t, cofport*> *port_list, uint32_t port_no) :
		port_list(port_list)
{
	*this = port;

	if (0 != port_list) {
		(*port_list)[port_no] = this;
	}
}



cofport&
cofport::operator= (cofport const& port)
{
	if (this == &port)
		return *this;

	reset_stats();

	WRITELOG(CPORT, DBG, "cofport(%p)::operator=() from port:%p", this, &port);

	this->of_version 	= port.of_version;
	this->memarea 		= port.memarea;
	this->port_stats	= port.port_stats;

	this->ofh_port		= (uint8_t*)memarea.somem();

	//port_list 		= 0; // keep port_list as it is

	return *this;
}



cofport_stats_reply&
cofport::get_port_stats()
{
	return port_stats;
}



uint32_t
cofport::get_port_no() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return (uint32_t)be16toh((ofh10_port)->port_no);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port->port_no);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port->port_no);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofport::set_port_no(uint32_t port_no)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		ofh10_port->port_no = htobe16((uint16_t)(port_no & 0x0000ffff));
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_port->port_no = htobe32(port_no);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port->port_no = htobe32(port_no);
	} break;
	default:
		throw eBadVersion();
	}
}



cmacaddr
cofport::get_hwaddr() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return cmacaddr(ofh10_port->hw_addr, OFP_ETH_ALEN);
	} break;
	case openflow12::OFP_VERSION: {
		return cmacaddr(ofh12_port->hw_addr, OFP_ETH_ALEN);
	} break;
	case openflow13::OFP_VERSION: {
		return cmacaddr(ofh13_port->hw_addr, OFP_ETH_ALEN);
	} break;
	default:
		throw eBadVersion();
	}
	return cmacaddr("00:00:00:00:00:00");
}


void
cofport::set_hwaddr(cmacaddr const& maddr)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		memcpy(ofh10_port->hw_addr, maddr.somem(), OFP_ETH_ALEN);
	} break;
	case openflow12::OFP_VERSION: {
		memcpy(ofh12_port->hw_addr, maddr.somem(), OFP_ETH_ALEN);
	} break;
	case openflow13::OFP_VERSION: {
		memcpy(ofh13_port->hw_addr, maddr.somem(), OFP_ETH_ALEN);
	} break;
	default:
		throw eBadVersion();
	}
}


std::string
cofport::get_name() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		//return std::string(ofh10_port->name, OFP_MAX_PORT_NAME_LEN);
		return std::string(ofh10_port->name, strlen(ofh10_port->name));
	} break;
	case openflow12::OFP_VERSION: {
		//return std::string(ofh12_port->name, OFP_MAX_PORT_NAME_LEN);
		return std::string(ofh12_port->name, strlen(ofh12_port->name));
	} break;
	case openflow13::OFP_VERSION: {
		//return std::string(ofh13_port->name, OFP_MAX_PORT_NAME_LEN);
		return std::string(ofh13_port->name, strlen(ofh13_port->name));
	} break;
	default:
		throw eBadVersion();
	}
	return std::string("");
}



void
cofport::set_name(std::string name)
{
	size_t len = (name.length() > OFP_MAX_PORT_NAME_LEN) ? OFP_MAX_PORT_NAME_LEN : name.length();

	switch (of_version) {
	case openflow10::OFP_VERSION: {
		memset(ofh10_port->name, 0, OFP_MAX_PORT_NAME_LEN);
		memcpy(ofh10_port->name, name.c_str(), len);
	} break;
	case openflow12::OFP_VERSION: {
		memset(ofh12_port->name, 0, OFP_MAX_PORT_NAME_LEN);
		memcpy(ofh12_port->name, name.c_str(), len);
	} break;
	case openflow13::OFP_VERSION: {
		memset(ofh13_port->name, 0, OFP_MAX_PORT_NAME_LEN);
		memcpy(ofh13_port->name, name.c_str(), len);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofport::get_config() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_port->config);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port->config);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port->config);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofport::set_config(uint32_t config)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		ofh10_port->config = htobe32(config);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_port->config = htobe32(config);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port->config = htobe32(config);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofport::get_state() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_port->state);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port->state);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port->state);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofport::set_state(uint32_t state)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		ofh10_port->state = htobe32(state);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_port->state = htobe32(state);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port->state = htobe32(state);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofport::get_curr() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_port->curr);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port->curr);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port->curr);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofport::set_curr(uint32_t curr)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		ofh10_port->curr = htobe32(curr);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_port->curr = htobe32(curr);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port->curr = htobe32(curr);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofport::get_advertised() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_port->advertised);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port->advertised);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port->advertised);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofport::set_advertised(uint32_t advertised)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		ofh10_port->advertised = htobe32(advertised);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_port->advertised = htobe32(advertised);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port->advertised = htobe32(advertised);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofport::get_supported() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_port->supported);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port->supported);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port->supported);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofport::set_supported(uint32_t supported)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		ofh10_port->supported = htobe32(supported);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_port->supported = htobe32(supported);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port->supported = htobe32(supported);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofport::get_peer() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_port->peer);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port->peer);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port->peer);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofport::set_peer(uint32_t peer)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		ofh10_port->peer = htobe32(peer);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_port->peer = htobe32(peer);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port->peer = htobe32(peer);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofport::get_curr_speed() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port->curr_speed);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port->curr_speed);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofport::set_curr_speed(uint32_t curr_speed)
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		ofh12_port->curr_speed = htobe32(curr_speed);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port->curr_speed = htobe32(curr_speed);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofport::get_max_speed() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_port->max_speed);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_port->max_speed);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofport::set_max_speed(uint32_t max_speed)
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		ofh12_port->max_speed = htobe32(max_speed);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_port->max_speed = htobe32(max_speed);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport::link_state_set_blocked()
{
	switch (of_version) {
	// non-existing for OF 1.0
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		set_state(get_state() | openflow12::OFPPS_BLOCKED);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport::link_state_clr_blocked()
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		set_state(get_state() & ~openflow12::OFPPS_BLOCKED);
	} break;
	default:
		throw eBadVersion();
	}
}



bool
cofport::link_state_is_blocked() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return (get_state() & openflow12::OFPPS_BLOCKED);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport::link_state_set_live()
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		set_state(get_state() | openflow12::OFPPS_LIVE);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport::link_state_clr_live()
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		set_state(get_state() & ~openflow12::OFPPS_LIVE);
	} break;
	default:
		throw eBadVersion();
	}
}



bool
cofport::link_state_is_live() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return (get_state() & openflow12::OFPPS_LIVE);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport::link_state_set_link_down()
{
	switch (of_version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		set_state(get_state() | openflow10::OFPPS_LINK_DOWN);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport::link_state_clr_link_down()
{
	switch (of_version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		set_state(get_state() & ~openflow10::OFPPS_LINK_DOWN);
	} break;
	default:
		throw eBadVersion();
	}
}



bool
cofport::link_state_is_link_down() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: return (get_state() & openflow10::OFPPS_LINK_DOWN); break;
	case openflow12::OFP_VERSION: return (get_state() & openflow12::OFPPS_LINK_DOWN); break;
	case openflow13::OFP_VERSION: return (get_state() & openflow12::OFPPS_LINK_DOWN); break; // FIXME: openflow13::OFPPS_LINK_DOWN, once it's been defined
	default:
		throw eBadVersion();
	}
}



void
cofport::link_state_phy_down()
{
	switch (of_version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		set_state(get_state() | openflow10::OFPPS_LINK_DOWN);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport::link_state_phy_up()
{
	switch (of_version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		set_state(get_state() & ~openflow10::OFPPS_LINK_DOWN);
	} break;
	default:
		throw eBadVersion();
	}
}



bool
cofport::link_state_phy_is_up() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return not (get_state() & openflow10::OFPPS_LINK_DOWN);
	} break;
	default:
		throw eBadVersion();
	}
}



bool
cofport::config_is_port_down() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return (get_config() & openflow10::OFPPS_LINK_DOWN);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport::recv_port_mod(
		uint32_t config,
		uint32_t mask,
		uint32_t advertise)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		recv_port_mod_of10(config, mask, advertise);
	} break;
	case openflow12::OFP_VERSION: {
		recv_port_mod_of12(config, mask, advertise);
	} break;
	case openflow13::OFP_VERSION: {
		recv_port_mod_of13(config, mask, advertise);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofport::recv_port_mod_of10(
		uint32_t config,
		uint32_t mask,
		uint32_t advertise)
{
	if (mask & openflow10::OFPPC_PORT_DOWN) {
		if (config & openflow10::OFPPC_PORT_DOWN) {
			set_config(get_config() |  openflow10::OFPPC_PORT_DOWN);
		} else {
			set_config(get_config() & ~openflow10::OFPPC_PORT_DOWN);
		}
	}

	if (mask & openflow10::OFPPC_NO_STP) {
		if (config & openflow10::OFPPC_NO_STP) {
			set_config(get_config() |  openflow10::OFPPC_NO_STP);
		} else {
			set_config(get_config() & ~openflow10::OFPPC_NO_STP);
		}
	}

	if (mask & openflow10::OFPPC_NO_RECV) {
		if (config & openflow10::OFPPC_NO_RECV) {
			set_config(get_config() |  openflow10::OFPPC_NO_RECV);
		} else {
			set_config(get_config() & ~openflow10::OFPPC_NO_RECV);
		}
	}

	if (mask & openflow10::OFPPC_NO_RECV_STP) {
		if (config & openflow10::OFPPC_NO_RECV_STP) {
			set_config(get_config() |  openflow10::OFPPC_NO_RECV_STP);
		} else {
			set_config(get_config() & ~openflow10::OFPPC_NO_RECV_STP);
		}
	}

	if (mask & openflow10::OFPPC_NO_FLOOD) {
		if (config & openflow10::OFPPC_NO_FLOOD) {
			set_config(get_config() |  openflow10::OFPPC_NO_FLOOD);
		} else {
			set_config(get_config() & ~openflow10::OFPPC_NO_FLOOD);
		}
	}

	if (mask & openflow10::OFPPC_NO_PACKET_IN) {
		if (config & openflow10::OFPPC_NO_PACKET_IN) {
			set_config(get_config() |  openflow10::OFPPC_NO_PACKET_IN);
		} else {
			set_config(get_config() & ~openflow10::OFPPC_NO_PACKET_IN);
		}
	}

	if (mask & openflow10::OFPPC_NO_FWD) {
		if (config & openflow10::OFPPC_NO_FWD) {
			set_config(get_config() |  openflow10::OFPPC_NO_FWD);
		} else {
			set_config(get_config() & ~openflow10::OFPPC_NO_FWD);
		}
	}

	if (0 != advertise) {
		set_advertised(advertise);
	}

	WRITELOG(CPORT, DBG, "cofport(%s:%d)::recv_port_mod() config:0x%x advertise:0x%x",
			get_name().c_str(), get_port_no(), get_config(), get_advertised());
}



void
cofport::recv_port_mod_of12(
		uint32_t config,
		uint32_t mask,
		uint32_t advertise)
{
	if (mask & openflow12::OFPPC_PORT_DOWN) {
		if (config & openflow12::OFPPC_PORT_DOWN) {
			set_config(get_config() |  openflow12::OFPPC_PORT_DOWN);
		} else {
			set_config(get_config() & ~openflow12::OFPPC_PORT_DOWN);
		}
	}

	if (mask & openflow12::OFPPC_NO_RECV) {
		if (config & openflow12::OFPPC_NO_RECV) {
			set_config(get_config() |  openflow12::OFPPC_NO_RECV);
		} else {
			set_config(get_config() & ~openflow12::OFPPC_NO_RECV);
		}
	}

	if (mask & openflow12::OFPPC_NO_PACKET_IN) {
		if (config & openflow12::OFPPC_NO_PACKET_IN) {
			set_config(get_config() |  openflow12::OFPPC_NO_PACKET_IN);
		} else {
			set_config(get_config() & ~openflow12::OFPPC_NO_PACKET_IN);
		}
	}

	if (mask & openflow12::OFPPC_NO_FWD) {
		if (config & openflow12::OFPPC_NO_FWD) {
			set_config(get_config() |  openflow12::OFPPC_NO_FWD);
		} else {
			set_config(get_config() & ~openflow12::OFPPC_NO_FWD);
		}
	}

	if (0 != advertise) {
		set_advertised(advertise);
	}

	WRITELOG(CPORT, DBG, "cofport(%s:%d)::recv_port_mod() config:0x%x advertise:0x%x",
			get_name().c_str(), get_port_no(), get_config(), get_advertised());
}



void
cofport::recv_port_mod_of13(
		uint32_t config,
		uint32_t mask,
		uint32_t advertise)
{
	recv_port_mod_of12(config, mask, advertise);
}



size_t
cofport::length() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: return sizeof(struct openflow10::ofp_port);
	case openflow12::OFP_VERSION: return sizeof(struct openflow12::ofp_port);
	case openflow13::OFP_VERSION: return sizeof(struct openflow13::ofp_port);
	default: throw eBadVersion();
	}
	return 0;
}




template<class T>
T*
cofport::pack(T* port, size_t portlen) const throw (eOFportInval)
{
#if 0
	if (openflow10::OFP_VERSION != of_version) {
		throw eBadVersion();
	}
#endif

	if (portlen < sizeof(T)) {
		throw eOFportInval();
	}

	memcpy(port, memarea.somem(), sizeof(T));

	return port;
}



struct openflow10::ofp_port*
cofport::pack(struct openflow10::ofp_port* port, size_t portlen) const throw (eOFportInval)
{
	if (openflow10::OFP_VERSION != of_version) {
		throw eBadVersion();
	}

	if (portlen < sizeof(struct openflow10::ofp_port)) {
		throw eOFportInval();
	}

	memcpy(port, memarea.somem(), sizeof(struct openflow10::ofp_port));

	return port;
}




struct openflow12::ofp_port*
cofport::pack(struct openflow12::ofp_port* port, size_t portlen) const throw (eOFportInval)
{
	if (openflow12::OFP_VERSION != of_version) {
		throw eBadVersion();
	}

	if (portlen < sizeof(struct openflow12::ofp_port)) {
		throw eOFportInval();
	}

	memcpy(port, memarea.somem(), sizeof(struct openflow12::ofp_port));

	return port;
}



struct openflow13::ofp_port*
cofport::pack(struct openflow13::ofp_port* port, size_t portlen) const throw (eOFportInval)
{
	if (openflow13::OFP_VERSION != of_version) {
		throw eBadVersion();
	}

	if (portlen < sizeof(struct openflow13::ofp_port)) {
		throw eOFportInval();
	}

	memcpy(port, memarea.somem(), sizeof(struct openflow13::ofp_port));

	return port;
}




template<class T>
T*
cofport::unpack(
	T* port, size_t portlen)
throw (eOFportInval)
{
#if 0
	if (openflow10::OFP_VERSION != of_version) {
		throw eBadVersion();
	}
#endif

	if (portlen < sizeof(T)) {
		throw eOFportInval();
	}

	memarea.assign((uint8_t*)port, portlen);
	ofh_port = memarea.somem();

	return port;
}



struct openflow10::ofp_port*
cofport::unpack(
	struct openflow10::ofp_port* port, size_t portlen)
throw (eOFportInval)
{
	if (openflow10::OFP_VERSION != of_version) {
		throw eBadVersion();
	}

	if (portlen < sizeof(struct openflow10::ofp_port)) {
		throw eOFportInval();
	}

	memarea.assign((uint8_t*)port, portlen);
	ofh10_port = (struct openflow10::ofp_port*)memarea.somem();

	return port;
}



struct openflow12::ofp_port*
cofport::unpack(
	struct openflow12::ofp_port* port, size_t portlen)
throw (eOFportInval)
{
	if (openflow12::OFP_VERSION != of_version) {
		throw eBadVersion();
	}

	if (portlen < sizeof(struct openflow12::ofp_port)) {
		throw eOFportInval();
	}

	memarea.assign((uint8_t*)port, portlen);
	ofh12_port = (struct openflow12::ofp_port*)memarea.somem();

	return port;
}



struct openflow13::ofp_port*
cofport::unpack(
	struct openflow13::ofp_port* port, size_t portlen)
throw (eOFportInval)
{
	if (openflow13::OFP_VERSION != of_version) {
		throw eBadVersion();
	}

	if (portlen < sizeof(struct openflow13::ofp_port)) {
		throw eOFportInval();
	}

	memarea.assign((uint8_t*)port, portlen);
	ofh13_port = (struct openflow13::ofp_port*)memarea.somem();

	return port;
}





void
cofport::reset_stats()
{
	port_stats.reset();
#if 0
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
#endif
}

#if 0
void
cofport::get_port_stats(
		cmemory& body)
{
	cmemory pstats(sizeof(struct ofp_port_stats));
	struct ofp_port_stats* stats = (struct ofp_port_stats*)pstats.somem();

	stats->port_no					= htobe32(get_port_no());
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
#endif




uint8_t
cofport::get_version() const
{
	return of_version;
}



/*static*/
void
cofport::test()
{
	cofport p1;

	p1.set_port_no(1);
	p1.set_config(10);
	p1.set_curr(2);
	p1.set_hwaddr(cmacaddr("00:11:11:11:11:11"));
	p1.set_curr_speed(0xdeadbeef);
	p1.set_max_speed(0xdeafdeaf);

	std::cerr << "p1 => " << p1 << std::endl;

	cmemory mem(sizeof(struct openflow10::ofp_port));

	p1.pack((struct openflow10::ofp_port*)mem.somem(), mem.memlen());

	std::cerr << "p1.packed => " << mem << std::endl;

	cofport p2;

	p2.unpack((struct openflow10::ofp_port*)mem.somem(), mem.memlen());

	std::cerr << "p1.unpacked => " << p2 << std::endl;
}

template class coflist<cofport>;


