/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofports.h"

using namespace rofl::openflow;

cofports::cofports(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{

}



cofports::~cofports()
{
	clear();
}



cofports::cofports(
		uint8_t ofp_version, uint8_t *buf, size_t buflen) :
				ofp_version(ofp_version)
{
	unpack(buf, buflen);
}



cofports::cofports(
		cofports const& ports)
{
	*this = ports;
}


cofports&
cofports::operator= (
		cofports const& ports)
{
	if (this == &ports)
		return *this;

	clear();

	ofp_version = ports.ofp_version;

	for (std::map<uint32_t, cofport*>::const_iterator
			it = ports.ports.begin(); it != ports.ports.end(); ++it) {
		this->ports[it->first] = new cofport(*it->second);
	}

	return *this;
}



cofports&
cofports::operator+= (
		cofports const& ports)
{
	/*
	 * this may replace existing ports in this cofports instance
	 */
	for (std::map<uint32_t, cofport*>::const_iterator
			it = ports.ports.begin(); it != ports.ports.end(); ++it) {
		if (this->ports.find(it->first) != this->ports.end()) {
			delete this->ports[it->first];
		}
		this->ports[it->first] = new cofport(*it->second);
	}

	return *this;
}



void
cofports::clear()
{
	for (std::map<uint32_t, cofport*>::iterator it = ports.begin(); it != ports.end(); ++it) {
		delete it->second;
	}
	ports.clear();
}



size_t
cofports::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofport*>::const_iterator it = ports.begin(); it != ports.end(); ++it) {
		len += (*(it->second)).length();
	}
	return len;
}



void
cofports::pack(
	uint8_t *buf,
	size_t buflen)
{
	if (buflen < length()) {
		throw ePortsInval();
	}

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {
		for (std::map<uint32_t, cofport*>::iterator it = ports.begin(); it != ports.end(); ++it) {
			cofport& port = *(it->second);
			port.pack(buf, sizeof(struct rofl::openflow10::ofp_port));
			buf += sizeof(struct rofl::openflow10::ofp_port);
		}
	} break;
	case rofl::openflow12::OFP_VERSION: {
		for (std::map<uint32_t, cofport*>::iterator it = ports.begin(); it != ports.end(); ++it) {
			cofport& port = *(it->second);
			port.pack(buf, port.length());
			buf += port.length();
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {
		for (std::map<uint32_t, cofport*>::iterator it = ports.begin(); it != ports.end(); ++it) {
			cofport& port = *(it->second);
			port.pack(buf, port.length());
			buf += port.length();
		}
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofports::unpack(
		uint8_t *buf,
		size_t buflen)
{
	clear();

	while (buflen > 0) {

		switch (ofp_version) {
		case rofl::openflow10::OFP_VERSION: {
			if (buflen < (int)sizeof(struct rofl::openflow10::ofp_port))
				return;

			cofport port(ofp_version, buf, sizeof(struct rofl::openflow10::ofp_port));
			if (ports.find(port.get_port_no()) != ports.end()) {
				delete ports[port.get_port_no()];
			}
			ports[port.get_port_no()] = new cofport(port);

			buf += sizeof(struct rofl::openflow10::ofp_port);
			buflen -= sizeof(struct rofl::openflow10::ofp_port);

		} break;
		case rofl::openflow12::OFP_VERSION: {
			if (buflen < (int)sizeof(struct rofl::openflow12::ofp_port))
				return;

			cofport port(ofp_version, buf, sizeof(struct rofl::openflow12::ofp_port));
			if (ports.find(port.get_port_no()) != ports.end()) {
				delete ports[port.get_port_no()];
			}
			ports[port.get_port_no()] = new cofport(port);

			buf += sizeof(struct rofl::openflow12::ofp_port);
			buflen -= sizeof(struct rofl::openflow12::ofp_port);

		} break;
		case rofl::openflow13::OFP_VERSION: {
			if (buflen < (int)sizeof(struct rofl::openflow13::ofp_port))
				return;

			cofport port(ofp_version, buf, sizeof(struct rofl::openflow13::ofp_port));
			if (ports.find(port.get_port_no()) != ports.end()) {
				delete ports[port.get_port_no()];
			}
			ports[port.get_port_no()] = new cofport(port);

			buf += sizeof(struct rofl::openflow13::ofp_port);
			buflen -= sizeof(struct rofl::openflow13::ofp_port);

		} break;
		default:
			throw eBadVersion();
		}
	}
}



cofport&
cofports::add_port(uint32_t portno)
{
	if (ports.find(portno) != ports.end()) {
		ports.erase(portno);
	}
	ports[portno] = new cofport(ofp_version);
	ports[portno]->set_port_no(portno);
	return *(ports[portno]);
}



cofport&
cofports::set_port(uint32_t portno)
{
	if (ports.find(portno) == ports.end()) {
		ports[portno] = new cofport(ofp_version);
		ports[portno]->set_port_no(portno);
	}
	return *(ports[portno]);
}



cofport const&
cofports::get_port(uint32_t portno) const
{
	if (ports.find(portno) == ports.end()) {
		throw ePortNotFound();
	}
	return *(ports.at(portno));
}



void
cofports::drop_port(uint32_t portno)
{
	if (ports.find(portno) == ports.end()) {
		return;
	}
	delete ports[portno];
	ports.erase(portno);
}



bool
cofports::has_port(uint32_t portno)
{
	return (ports.find(portno) != ports.end());
}


