/*
 * caddrinfo.cc
 *
 *  Created on: 12.06.2014
 *      Author: andreas
 */

#include "rofl/common/caddrinfo.h"

using namespace rofl;


caddrinfo::caddrinfo() :
				ai_flags(0),
				ai_family(0),
				ai_socktype(0),
				ai_protocol(0)
{

}


caddrinfo::caddrinfo(
		int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const csockaddr& ai_addr) :
				ai_flags(ai_flags),
				ai_family(ai_family),
				ai_socktype(ai_socktype),
				ai_protocol(ai_protocol),
				ai_addr(ai_addr)
{

}



size_t
caddrinfo::length() const
{
	return sizeof(struct addrinfo);
}



void
caddrinfo::pack(struct addrinfo* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval("caddrinfo::pack() buflen too short");

	struct addrinfo* ai = (struct addrinfo*)buf;

	ai->ai_flags = ai_flags;
	ai->ai_family = ai_family;
	ai->ai_socktype = ai_socktype;
	ai->ai_protocol = ai_protocol;
	ai->ai_addr = (struct sockaddr*)ai_addr.somem();
	ai->ai_addrlen = ai_addr.length();
}



void
caddrinfo::unpack(struct addrinfo* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval("caddrinfo::unpack() buflen too short");

	struct addrinfo* ai = (struct addrinfo*)buf;

	ai_flags = ai->ai_flags;
	ai_family = ai->ai_family;
	ai_socktype = ai->ai_socktype;
	ai_protocol = ai->ai_protocol;
	ai_addr.ca_saddr->sa_family = ai_family;
	if ((0 == ai->ai_addr) || (ai->ai_addrlen < ai_addr.length()))
		throw eInval("caddrinfo::unpack() ai_addr_len too short");
	ai_addr.unpack(ai->ai_addr, ai->ai_addrlen);
	ai_addr.salen = ai->ai_addrlen;
}



