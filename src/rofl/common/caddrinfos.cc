/*
 * caddrinfos.cc
 *
 *  Created on: 12.06.2014
 *      Author: andreas
 */

#include "rofl/common/caddrinfos.h"

using namespace rofl;



void
caddrinfos::resolve()
{
	struct addrinfo ai_hints;
	memset(&ai_hints, 0, sizeof(struct addrinfo));
	ai_hints.ai_family 		= hints.get_ai_family();
	ai_hints.ai_socktype 	= hints.get_ai_socktype();
	ai_hints.ai_flags 		= hints.get_ai_flags();
	ai_hints.ai_protocol	= hints.get_ai_protocol();
	ai_hints.ai_addr 		= NULL;
	ai_hints.ai_addrlen		= 0;

	struct addrinfo *result, *rp = (struct addrinfo*)0;

	int rc = getaddrinfo((node.empty()) ? NULL : node.c_str(), (service.empty()) ? NULL : service.c_str(), &ai_hints, &result);
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
					LOGGING_ERROR << "[rofl][caddrinfos] name resolution failed: " << std::string(gai_strerror(rc)) << std::endl;
			} break;
			case EAI_SYSTEM:
			default: {
					LOGGING_ERROR << "[rofl][caddrinfos] name resolution failed: unknown error occured " << std::endl;
			};
			}
			throw eSysCall("getaddrinfo() "+std::string(gai_strerror(rc)));
	}


	clear();

	unsigned int index = 0;

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		add_addr_info(index++).unpack(rp, sizeof(*rp));
	}

	freeaddrinfo(result);
}



caddrinfo&
caddrinfos::add_addr_info(unsigned int index)
{
	if (ais.find(index) != ais.end()) {
		ais.erase(index);
	}
	return ais[index];
}



caddrinfo&
caddrinfos::set_addr_info(unsigned int index)
{
	return ais[index];
}



void
caddrinfos::drop_addr_info(unsigned int index)
{
	if (ais.find(index) == ais.end()) {
		return;
	}
	ais.erase(index);
}



const caddrinfo&
caddrinfos::get_addr_info(unsigned int index) const
{
	if (ais.find(index) == ais.end()) {
		throw eAddrInfoNotFound("caddrinfos::get_addr_info()");
	}
	return ais.at(index);
}



bool
caddrinfos::has_addr_info(unsigned int index) const
{
	return (not (ais.find(index) == ais.end()));
}




