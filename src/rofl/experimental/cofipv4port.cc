/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofipv4port.h"

using namespace rofl;

cofipv4port::cofipv4port(
		uint32_t __portno,
		caddress const& __addr,
		caddress const& __mask,
		caddress const& __peer,
		uint32_t __flags) :
		portno(__portno),
		addr(__addr),
		mask(__mask),
		peer(__peer),
		flags(__flags)
{

}


cofipv4port::cofipv4port(
		uint32_t __portno,
		uint32_t __addr,
		uint32_t __mask,
		uint32_t __peer,
		uint32_t __flags) :
		portno(__portno),
		addr(caddress(AF_INET, "0.0.0.0")),
		mask(caddress(AF_INET, "0.0.0.0")),
		peer(caddress(AF_INET, "0.0.0.0")),
		flags(__flags)
{
	addr.ca_s4addr->sin_addr.s_addr = htobe32(__addr);
	mask.ca_s4addr->sin_addr.s_addr = htobe32(__mask);
	peer.ca_s4addr->sin_addr.s_addr = htobe32(__peer);
}


cofipv4port::~cofipv4port()
{

}


const char*
cofipv4port::c_str()
{
	cvastring vas;

	info.assign(vas("cofipv4port(%p) portno:%d addr:%s mask:%s peer:%s flags:%x",
			this,
			portno,
			addr.addr_c_str(),
			mask.addr_c_str(),
			peer.addr_c_str(),
			flags));

	return info.c_str();
}


void
cofipv4port::pack(struct ofp_rofl_ipv4_port *port)
{
	port->flags 	= htobe32(flags);
	port->ipaddr 	= addr.ca_s4addr->sin_addr.s_addr;
	port->mask 		= mask.ca_s4addr->sin_addr.s_addr;
	port->peer 		= peer.ca_s4addr->sin_addr.s_addr;
	port->portno 	= htobe32(portno);
}


void
cofipv4port::unpack(struct ofp_rofl_ipv4_port *port)
{
	flags 	= be32toh(port->flags);
	addr.ca_s4addr->sin_addr.s_addr = port->ipaddr;
	mask.ca_s4addr->sin_addr.s_addr = port->mask;
	peer.ca_s4addr->sin_addr.s_addr = port->peer;
	portno	= be32toh(port->portno);
}

