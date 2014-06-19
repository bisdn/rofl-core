/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "caddress.h"
#include "rofl/datapath/pipeline/common/large_types.h"

using namespace rofl;


size_t
caddress::length() const
{
	return cmemory::length();
}



void
caddress::pack(
		uint8_t* buf, size_t buflen)
{
	if (buflen < caddress::length())
		throw eAddressInval("caddress::pack() buflen too short");
	cmemory::pack(buf, buflen);
}



void
caddress::unpack(
		uint8_t* buf, size_t buflen)
{
	if (buflen < caddress::length())
		throw eAddressInval("caddress::unpack() buflen too short");
	cmemory::unpack(buf, buflen);
}



void
caddress_ll::str2addr(
		const std::string& addr)
{
#if 0
	sscanf(addr.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",
			(unsigned int*)&((*this)[0]),
			(unsigned int*)&((*this)[1]),
			(unsigned int*)&((*this)[2]),
			(unsigned int*)&((*this)[3]),
			(unsigned int*)&((*this)[4]),
			(unsigned int*)&((*this)[5]));
#endif
	unsigned int v[6];
	sscanf(addr.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",
			&v[0],
			&v[1],
			&v[2],
			&v[3],
			&v[4],
			&v[5]);
	(*this)[0] = v[0] & 0x000000ff;
	(*this)[1] = v[1] & 0x000000ff;
	(*this)[2] = v[2] & 0x000000ff;
	(*this)[3] = v[3] & 0x000000ff;
	(*this)[4] = v[4] & 0x000000ff;
	(*this)[5] = v[5] & 0x000000ff;
}



std::string
caddress_ll::addr2str() const
{
	char buf[18];
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
			(*this)[0],
			(*this)[1],
			(*this)[2],
			(*this)[3],
			(*this)[4],
			(*this)[5]);
	return std::string(buf);
}



bool
caddress_ll::is_multicast() const
{
	if ((*this)[0] & 0x01)
		return true;
	return false;
}



bool
caddress_ll::is_broadcast() const
{
	if (*this == caddress_ll("ff:ff:ff:ff:ff:ff"))
		return true;
	return false;
}



bool
caddress_ll::is_null() const
{
	if (*this == caddress_ll("00:00:00:00:00:00"))
		return true;
	return false;
}



uint64_t
caddress_ll::get_mac() const
{
	uint64_t mac = 0;

	((uint8_t*)&mac)[5] = (*this)[0];
	((uint8_t*)&mac)[4] = (*this)[1];
	((uint8_t*)&mac)[3] = (*this)[2];
	((uint8_t*)&mac)[2] = (*this)[3];
	((uint8_t*)&mac)[1] = (*this)[4];
	((uint8_t*)&mac)[0] = (*this)[5];

	return mac;
}



void
caddress_ll::set_mac(uint64_t mac)
{
	(*this)[5] = ((uint8_t*)&mac)[0];
	(*this)[4] = ((uint8_t*)&mac)[1];
	(*this)[3] = ((uint8_t*)&mac)[2];
	(*this)[2] = ((uint8_t*)&mac)[3];
	(*this)[1] = ((uint8_t*)&mac)[4];
	(*this)[0] = ((uint8_t*)&mac)[5];
}



void
caddress_in4::str2addr(
		const std::string& addr)
{
	struct in_addr in4addr;
	memset((uint8_t*)&in4addr, 0, sizeof(in4addr));
	if (inet_pton(AF_INET, addr.c_str(), (void*)&in4addr) < 1) {
		throw eSysCall("inet_pton()");
	}
	memcpy(cmemory::somem(), &(in4addr.s_addr), INET4_ADDR_LEN);
}



std::string
caddress_in4::addr2str() const
{
	char buf[INET_ADDRSTRLEN];
	memset(buf, 0, sizeof(buf));
	struct in_addr in4addr;
	memcpy((uint8_t*)&(in4addr.s_addr), cmemory::somem(), INET4_ADDR_LEN);
	if (inet_ntop(AF_INET, (const void*)&in4addr, buf, sizeof(buf)) == NULL) {
		throw eSysCall("inet_ntop()");
	}
	return std::string(buf);
}



void
caddress_in6::str2addr(
		const std::string& addr)
{
	struct in6_addr in6addr;
	memset((uint8_t*)&in6addr, 0, sizeof(in6addr));
	if (inet_pton(AF_INET6, addr.c_str(), (void*)&in6addr) < 1) {
		throw eSysCall("inet_pton()");
	}
	memcpy(cmemory::somem(), &(in6addr.s6_addr), INET6_ADDR_LEN);
}



std::string
caddress_in6::addr2str() const
{
	char buf[INET6_ADDRSTRLEN];
	memset(buf, 0, sizeof(buf));
	struct in6_addr in6addr;
	memcpy((uint8_t*)&(in6addr.s6_addr), cmemory::somem(), INET6_ADDR_LEN);
	if (inet_ntop(AF_INET6, (const void*)&in6addr, buf, sizeof(buf)) == NULL) {
		throw eSysCall("inet_ntop()");
	}
	return std::string(buf);
}



