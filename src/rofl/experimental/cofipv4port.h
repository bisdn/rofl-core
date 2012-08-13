/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFIPV4PORT_H
#define COFIPV4PORT_H 1

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <arpa/inet.h>
#include <endian.h>
#include "rofl/common/openflow/openflow_rofl.h"

#ifndef be32toh
#include "rofl/common/endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "rofl/platform/unix/csyslog.h"
#include "rofl/common/caddress.h"
#include "rofl/common/cvastring.h"



class cofipv4port :
	public csyslog
{
public: // data structures

		uint32_t portno;
		caddress addr; // IPv4 address of this interface
		caddress mask;
		caddress peer; // 0.0.0.0, if no direct peer for point-to-point
		uint32_t flags;

public:
		/**
		 *
		 */
		cofipv4port(
				uint32_t portno = 0,
				caddress const& addr = caddress(AF_INET, "0.0.0.0"),
				caddress const& mask = caddress(AF_INET, "255.255.255.255"),
				caddress const& peer = caddress(AF_INET, "0.0.0.0"),
				uint32_t flags = 0);

		/**
		 *
		 */
		cofipv4port(
				uint32_t portno,
				uint32_t addr,
				uint32_t mask,
				uint32_t peer,
				uint32_t flags);

		/**
		 *
		 */
		virtual
		~cofipv4port();

		/**
		 *
		 */
		const char*
		c_str();

		/**
		 *
		 */
		void
		pack(
				struct ofp_rofl_ipv4_port *port);

		/**
		 *
		 */
		void
		unpack(
				struct ofp_rofl_ipv4_port *port);


private: // data structures

		std::string info;

};
#endif

