/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CROFLEXP_H
#define CROFLEXP_H 1

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <inttypes.h>

#include <endian.h>
#ifndef be32toh
#include "endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "cmemory.h"
#include "cerror.h"
#include "cvastring.h"

class eRoflExpBase : public cerror {};
class eRoflExpInval : public eRoflExpBase {};

class croflexp :
	public cmemory
{
public:

	enum roflexp_exttype_t {
		ROFL_EXTTYPE_NONE = 0,
		ROFL_EXTTYPE_PORT_IPV4_STATUS = 1,
	};

	enum croflexp_ipv4_port_flags_t {
		ROFL_IFF_UP 			= (1 << 0),
		ROFL_IFF_BROADCAST 		= (1 << 1),
		ROFL_IFF_POINTOPOINT 	= (1 << 4),
	};

	struct ofp_vendor_ext_rofl_port_ipv4 {
		uint8_t reason;
		uint32_t portno;
		uint32_t addr;
		uint32_t mask;
		uint32_t peer;
		uint32_t flags;
		uint32_t mtu;
		uint8_t payload[0];
	};

// this must be set to largest ofp_vendor_ext_rofl_... structure defined
#define ROFL_EXP_MAX_SIZE (sizeof(struct ofp_vendor_ext_rofl_port_ipv4))

public:
	/**
	 *
	 */
	croflexp();

	/**
	 *
	 */
	virtual
	~croflexp();

	/**
	 *
	 */
	void
	pack(
			uint8_t *body, size_t bodylen) throw (eRoflExpInval);

	/**
	 *
	 */
	void
	unpack(
			uint8_t *body, size_t bodylen);

	/**
	 *
	 */
	const char*
	c_str();

	/**
	 *
	 */
	size_t
	size();


public: // data structures

	union {
		uint8_t									*rehu_body;
		struct ofp_vendor_ext_rofl_port_ipv4 	*rehu_port_ipv4;
	} reh_rehu;

#define rext_body						reh_rehu.rehu_body
#define rext_port_ipv4					reh_rehu.rehu_port_ipv4

private: // data structures

	std::string info;
};

#endif

