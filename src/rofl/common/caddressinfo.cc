/*
 * caddressinfo.cc
 *
 *  Created on: 12.06.2014
 *      Author: andreas
 */

#include "rofl/common/caddressinfo.h"

using namespace rofl;


caddress_info::caddress_info() :
		ai_flags(0),
		preferred_family(0),
		preferred_socktype(0),
		preferred_protocol(0),
		domain(0),
		socktype(0),
		protocol(0) {};


caddress_info::caddress_info(
		const std::string& host,
		const std::string& service,
		int ai_flags,
		int preferred_family,
		int preferred_socktype,
		int preferred_protocol) :
				ai_flags(ai_flags),
				preferred_family(preferred_family),
				preferred_socktype(preferred_socktype),
				preferred_protocol(preferred_protocol),
				domain(0),
				socktype(0),
				protocol(0)
{
	resolve(host, service, ai_flags, preferred_family, preferred_socktype, preferred_protocol);
}



void
caddress_info::resolve(
                std::string const& host,
                std::string const& service,
                int ai_flags,
                int preferred_family,
                int preferred_socktype,
                int preferred_protocol)
{
        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));

        hints.ai_flags          = ai_flags;
        hints.ai_family         = preferred_family;
        hints.ai_socktype       = preferred_socktype;
        hints.ai_protocol       = preferred_protocol;

        struct addrinfo *res = (struct addrinfo*)0;

        int rc = getaddrinfo((host.empty()) ? NULL : host.c_str(), (service.empty()) ? NULL : service.c_str(), &hints, &res);
        if (rc != 0) {
                switch (rc) {
                case EAI_ADDRFAMILY:
                case EAI_AGAIN:
                case EAI_BADFLAGS:
                case EAI_FAIL:
                case EAI_FAMILY:
                case EAI_MEMORY:
                case EAI_NODATA:
                case EAI_NONAME:
                case EAI_SERVICE:
                case EAI_SOCKTYPE: {
                        rofl::logging::error << "[rofl][caddress] name resolution failed: " << std::string(gai_strerror(rc)) << std::endl;
                } break;
                case EAI_SYSTEM:
                default: {
                        rofl::logging::error << "[rofl][caddress] name resolution failed: unknown error occured " << std::endl;
                };
                }
                throw eSysCall("getaddrinfo() "+std::string(gai_strerror(rc)));
        }

        // for now: we take the first entry returned from getaddrinfo

        domain          = res->ai_family;
        protocol        = res->ai_protocol;
        socktype        = res->ai_socktype;
        switch (domain) {
        case AF_INET: {
        	saddr = csockaddr(AF_INET, "0.0.0.0", 0);
        	saddr.unpack(res->ai_addr, res->ai_addrlen);
        } break;
        case AF_INET6: {
        	saddr = csockaddr(AF_INET6, "0000:0000:0000:0000:0000:0000:0000:0000", 0);
        	saddr.unpack(res->ai_addr, res->ai_addrlen);
        } break;
        }

        freeaddrinfo(res);
}


