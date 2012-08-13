/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef OPENFLOW_ROFL_H
#define OPENFLOW_ROFL_H 1

#ifdef __cplusplus
extern "C" {
#endif
	#include "openflow12.h"
	#include <inttypes.h>
#ifdef __cplusplus
}
#endif



// ROFL vendor extension identifier
#define VENDOR_EXT_ROFL 0x551bcccc	// shamelessly taken

// ROFL extension types
#define OFPRET_BASE			0xc000

enum ofp_rofl_ext_type {
	OFPRET_NSP_BASE = ((OFPRET_BASE << 16) | (0x0001)),
	OFPRET_NSP_GET_FSP_REQUEST,
	OFPRET_NSP_GET_FSP_REPLY,
	OFPRET_NSP_OPEN_REQUEST,
	OFPRET_NSP_OPEN_REPLY,
	OFPRET_NSP_CLOSE_REQUEST,
	OFPRET_NSP_CLOSE_REPLY,
	OFPRET_NSP_IOCTL_REQUEST,
	OFPRET_NSP_IOCTL_REPLY,
	OFPRET_PORT_OPEN,
	OFPRET_PORT_CLOSE,
	OFPRET_PORT_EXTD_CONFIG_REQUEST,
	OFPRET_PORT_EXTD_CONFIG_REPLY,
	OFPRET_EXTD_PORT_CONFIG,
	OFPRET_IPV4_PORT_STATUS,
};



/* common header ROFL vendor extensions
 */
struct ofp_vendor_ext_rofl {
	struct ofp_experimenter_header header;
										// common header including normal and vendor OpenFlow header
	uint32_t exttype;					// rofl extension type
	uint8_t payload[0];					// pointer to payload following header
};

// -------------------------------------------------------------------------------------------

/*
 * extended port configuration
 */

#if 0
enum ofp_rofl_extd_port_flags {
	ROFL_IFF_UP 			= (1 << 0),
	ROFL_IFF_BROADCAST 		= (1 << 1),
	ROFL_IFF_POINTOPOINT 	= (1 << 4),
};
#endif

// extended port description
struct ofp_rofl_ipv4_port {
	uint32_t portno;
	uint32_t ipaddr;
	uint32_t mask;
	uint32_t peer; // for pointopoint interfaces
	uint32_t flags;
};

// body for OFPRET_PORT_EXTD_CONFIG_REQUEST
struct ofp_rofl_port_extd_config_request {
	uint32_t type;			// one of OFPRET_*, here: OFPRET_PORT_EXTD_CONFIG_REQUEST
	uint32_t portno;		// we request extended port configuration for this portno (may be also OFPP_ALL)
};

// body for OFPRET_PORT_EXTD_CONFIG_REPLY
struct ofp_rofl_port_extd_config_reply {
	uint32_t type;			// one of OFPRET_*, here: OFPRET_PORT_EXTD_CONFIG_REPLY
	uint8_t pad[4];
	struct ofp_rofl_ipv4_port ports[0];		// returns list of extended rofl port descriptions
};

// body for OFPRET_EXTD_PORT_CONFIG
struct ofp_rofl_extd_port_config {
	uint32_t type;			// one of OFPRET_*, here: OFPRET_PORT_EXTD_CONFIG_REPLY
	uint8_t pad[4];
	struct ofp_rofl_ipv4_port ports[0];		// returns list of extended rofl port descriptions
};

// -------------------------------------------------------------------------------------------

/*
 * flowspace management
 */

enum ofp_rofl_ext_nsp_result {
	OFPRET_NSP_RESULT_OK,				// command successful
	OFPRET_NSP_RESULT_OVERLAP,			// namespace overlaps with already registered nsp,
										// match contains the overlapping namespace (open)
	OFPRET_NSP_RESULT_NOT_FOUND,		// namespace not found (close)
};

/** nsp_get_fsp reply (request is a common ofp_vendor_ext_rofl header)
 *
 */
struct ofp_vendor_ext_rofl_nsp_get_fsp {
	struct ofp_vendor_ext_rofl header;	// common headers
	uint8_t matches[0];					// list of ofp_match_* structures
};

/* nsp_open
 */
struct ofp_vendor_ext_rofl_nsp {
	struct ofp_vendor_ext_rofl header;	// common headers
//	uint64_t controller_id;				// controller id (survive socket down events? => ...)
	uint32_t result;					// result of associated request, set to 0 in request
	struct ofp_match match[0];			// matching structure
};

/* A controller entity should be enabled to register multiple nsp structs in parallel,
 * e.g. in order to register several MAC addresses as L2 transport endpoints.
 * - Shall we introduce a maximum number of nsp structs per controller?
 *
 */

// -------------------------------------------------------------------------------------------

/* processing port configuration structure
 *
 */
struct ofp_vendor_ext_rofl_port {
	struct ofp_vendor_ext_rofl header;	// common headers
	uint16_t sessid; 					// pppoe session id
	uint8_t port_name[OFP_MAX_PORT_NAME_LEN];
	uint8_t port_type[OFP_MAX_PORT_NAME_LEN];
	uint8_t umaddr[OFP_ETH_ALEN];		// user pppoe mac address
	uint8_t vmaddr[OFP_ETH_ALEN];		// vport pppoe mac address
	uint8_t cmaddr[OFP_ETH_ALEN];		// controller pppoe mac address
	uint8_t imaddr[OFP_ETH_ALEN];		// IP gateway mac address
	uint32_t uipaddr;					// user IP address
	uint32_t gipaddr;					// gateway IP address
	uint8_t data[0];					// data specific to command
};

#endif
