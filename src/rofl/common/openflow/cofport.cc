/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofport.h"

using namespace rofl::openflow;

cofport::cofport(
		uint8_t ofp_version) :
				ofp_version(ofp_version),
				port_stats(ofp_version)
{
	switch (ofp_version) {
	case openflow::OFP_VERSION_UNKNOWN: {
		ofh_port = 0;
	} break;
	case openflow10::OFP_VERSION: {
		resize(sizeof(struct rofl::openflow10::ofp_port));
	} break;
	case openflow12::OFP_VERSION: {
		resize(sizeof(struct rofl::openflow12::ofp_port));
	} break;
	case openflow13::OFP_VERSION: {
		resize(sizeof(struct rofl::openflow13::ofp_port));
	} break;
	default:
		throw eBadVersion();
	}
}



cofport::cofport(
		uint8_t ofp_version,
		uint8_t *buf,
		size_t buflen) :
				ofp_version(ofp_version),
				port_stats(ofp_version)
{
	switch (ofp_version) {
	case openflow10::OFP_VERSION: {
		resize(sizeof(struct rofl::openflow10::ofp_port));
	} break;
	case openflow12::OFP_VERSION: {
		resize(sizeof(struct rofl::openflow12::ofp_port));
	} break;
	case openflow13::OFP_VERSION: {
		resize(sizeof(struct rofl::openflow13::ofp_port));
	} break;
	default:
		throw eBadVersion();
	}

	unpack(buf, buflen);
}



cofport::cofport(
		cofport const& port)
{
	*this = port;
}



cofport&
cofport::operator= (cofport const& port)
{
	if (this == &port)
		return *this;

	ofp_version	= port.ofp_version;
	port_stats 	= port.port_stats;

	cmemory::operator= (port);

	ofh_port = somem();

	return *this;
}



cofport::~cofport()
{

}



uint8_t*
cofport::resize(
		size_t len)
{
	return (ofh_port = cmemory::resize(len));
}



void
cofport::pack(
		uint8_t *buf, size_t buflen)
{
	if (buflen < length()) {
		throw ePortInval();
	}
	memcpy(buf, somem(), length());
}



void
cofport::unpack(
		uint8_t *buf, size_t buflen)
{
	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow10::ofp_port))
			throw ePortInval();
		assign(buf, sizeof(struct rofl::openflow10::ofp_port));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_port))
			throw ePortInval();
		assign(buf, sizeof(struct rofl::openflow12::ofp_port));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_port))
			throw ePortInval();
		assign(buf, sizeof(struct rofl::openflow13::ofp_port));
	} break;
	default:
		throw eBadVersion();
	}
	ofh_port = somem();
}



rofl::openflow::cofport_stats_reply&
cofport::get_port_stats()
{
	return port_stats;
}



uint32_t
cofport::get_port_no() const
{
	switch (ofp_version) {
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
	switch (ofp_version) {
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



rofl::cmacaddr
cofport::get_hwaddr() const
{
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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

	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
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
	switch (ofp_version) {
	case openflow10::OFP_VERSION: return sizeof(struct openflow10::ofp_port);
	case openflow12::OFP_VERSION: return sizeof(struct openflow12::ofp_port);
	case openflow13::OFP_VERSION: return sizeof(struct openflow13::ofp_port);
	default: throw eBadVersion();
	}
	return 0;
}



uint8_t
cofport::get_version() const
{
	return ofp_version;
}




