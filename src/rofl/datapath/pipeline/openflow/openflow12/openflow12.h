/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Copyright (c) 2008 The Board of Trustees of The Leland Stanford
 * Junior University
 *
 * We are making the OpenFlow specification and associated documentation
 * (Software) available for public use and benefit with the expectation
 * that others will use, modify and enhance the Software and contribute
 * those enhancements back to the community. However, since we would
 * like to make the Software available for broadest use, with as few
 * restrictions as possible permission is hereby granted, free of
 * charge, to any person obtaining a copy of this Software to deal in
 * the Software under the copyrights without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * The name and trademarks of copyright holder(s) may NOT be used in
 * advertising or publicity pertaining to the Software or any
 * derivatives without specific, written prior permission.
 */

/* OpenFlow: protocol between controller and datapath. */

#ifndef OPENFLOW_OPENFLOW12_H
#define OPENFLOW_OPENFLOW12_H 1

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#ifdef SWIG
#define OF12P_ASSERT(EXPR)        /* SWIG can't handle OF12P_ASSERT. */
#elif !defined(__cplusplus)
/* Build-time assertion for use in a declaration context. */
#define OF12P_ASSERT(EXPR)                                                \
        extern int (*build_assert(void))[ sizeof(struct {               \
                    unsigned int build_assert_failed : (EXPR) ? 1 : -1; })]
#else /* __cplusplus */
#define OF12P_ASSERT(_EXPR) typedef int build_assert_failed[(_EXPR) ? 1 : -1]
#endif /* __cplusplus */

#ifndef SWIG
#define OF12P_PACKED __attribute__((packed))
#else
#define OF12P_PACKED              /* SWIG doesn't understand __attribute. */
#endif


/* Version number:
 * Non-experimental versions released: 0x01
 * Experimental versions released: 0x81 -- 0x99
 */
/* The most significant bit being set in the version field indicates an
 * experimental OpenFlow version.
 */
#define OF12P_VERSION   0x03

#define OF12P_MAX_TABLE_NAME_LEN 32
#define OF12P_MAX_PORT_NAME_LEN  16

#define OF12P_TCP_PORT  6633
#define OF12P_SSL_PORT  6633

#define OF12P_ETH_ALEN 6          /* Bytes in an Ethernet address. */

/* Port numbering. Ports are numbered starting from 1. */
enum of12p_port_no {
    /* Maximum number of physical switch ports. */
    OF12PP_MAX        = 0xffffff00,

    /* Fake output "ports". */
    OF12PP_IN_PORT    = 0xfffffff8,  /* Send the packet out the input port.  This
                                      virtual port must be explicitly used
                                      in order to send back out of the input
                                      port. */
    OF12PP_TABLE      = 0xfffffff9,  /* Submit the packet to the first flow table
                                      NB: This destination port can only be
                                      used in packet-out messages. */
    OF12PP_NORMAL     = 0xfffffffa,  /* Process with normal L2/L3 switching. */
    OF12PP_FLOOD      = 0xfffffffb,  /* All physical ports in VLAN, except input
                                      port and those blocked or link down. */
    OF12PP_ALL        = 0xfffffffc,  /* All physical ports except input port. */
    OF12PP_CONTROLLER = 0xfffffffd,  /* Send to controller. */
    OF12PP_LOCAL      = 0xfffffffe,  /* Local openflow "port". */
    OF12PP_ANY        = 0xffffffff   /* Wildcard port used only for flow mod
                                      (delete) and flow stats requests. Selects
                                      all flows regardless of output port
                                      (including flows with no output port). */
};

enum of12p_type {
    /* Immutable messages. */
    OF12PT_HELLO 					= 0,    /* Symmetric message */
    OF12PT_ERROR 					= 1,	/* Symmetric message */
    OF12PT_ECHO_REQUEST 			= 2,	/* Symmetric message */
    OF12PT_ECHO_REPLY				= 3,    /* Symmetric message */
    OF12PT_EXPERIMENTER			= 4,    /* Symmetric message */

    /* Switch configuration messages. */
    OF12PT_FEATURES_REQUEST		= 5,    /* Controller/switch message */
    OF12PT_FEATURES_REPLY			= 6,    /* Controller/switch message */
    OF12PT_GET_CONFIG_REQUEST		= 7,    /* Controller/switch message */
    OF12PT_GET_CONFIG_REPLY		= 8,    /* Controller/switch message */
    OF12PT_SET_CONFIG				= 9,    /* Controller/switch message */

    /* Asynchronous messages. */
    OF12PT_PACKET_IN				= 10,   /* Async message */
    OF12PT_FLOW_REMOVED			= 11,   /* Async message */
    OF12PT_PORT_STATUS			= 12,   /* Async message */

    /* Controller command messages. */
    OF12PT_PACKET_OUT				= 13,   /* Controller/switch message */
    OF12PT_FLOW_MOD				= 14,   /* Controller/switch message */
    OF12PT_GROUP_MOD				= 15,   /* Controller/switch message */
    OF12PT_PORT_MOD				= 16,   /* Controller/switch message */
    OF12PT_TABLE_MOD				= 17,   /* Controller/switch message */

    /* Statistics messages. */
    OF12PT_STATS_REQUEST			= 18,   /* Controller/switch message */
    OF12PT_STATS_REPLY			= 19,   /* Controller/switch message */

    /* Barrier messages. */
    OF12PT_BARRIER_REQUEST		= 20,   /* Controller/switch message */
    OF12PT_BARRIER_REPLY			= 21,   /* Controller/switch message */

    /* Queue Configuration messages. */
    OF12PT_QUEUE_GET_CONFIG_REQUEST	= 22,  /* Controller/switch message */
    OF12PT_QUEUE_GET_CONFIG_REPLY		= 23,  /* Controller/switch message */

    /* Controller role change request messages. */
    OF12PT_ROLE_REQUEST    		= 24, /* Controller/switch message */
    OF12PT_ROLE_REPLY				= 25, /* Controller/switch message */

};

/* Header on all OpenFlow packets. */
struct of12p_header {
    uint8_t version;    /* OF12P_VERSION. */
    uint8_t type;       /* One of the OF12PT_ constants. */
    uint16_t length;    /* Length including this of12p_header. */
    uint32_t xid;       /* Transaction id associated with this packet.
                           Replies use the same id as was in the request
                           to facilitate pairing. */
};
OF12P_ASSERT(sizeof(struct of12p_header) == 8);

/* OF12PT_HELLO.  This message has an empty body, but implementations must
 * ignore any data included in the body, to allow for future extensions. */
struct of12p_hello {
    struct of12p_header header;
};

#define OF12P_DEFAULT_MISS_SEND_LEN   128

enum of12p_config_flags {
    /* Handling of IP fragments. */
    OF12PC_FRAG_NORMAL   = 0,       /* No special handling for fragments. */
    OF12PC_FRAG_DROP     = 1 << 0,  /* Drop fragments. */
    OF12PC_FRAG_REASM    = 1 << 1,  /* Reassemble (only if OF12PC_IP_REASM set). */
    OF12PC_FRAG_MASK     = 3,

    /* TTL processing - applicable for IP and MPLS packets */
    OF12PC_INVALID_TTL_TO_CONTROLLER = 1 << 2, /* Send packets with invalid TTL
                                                ie. 0 or 1 to controller */
};

/* Switch configuration. */
struct of12p_switch_config {
    struct of12p_header header;
    uint16_t flags;             /* OF12PC_* flags. */
    uint16_t miss_send_len;     /* Max bytes of new flow that datapath should
                                   send to the controller. */
};
OF12P_ASSERT(sizeof(struct of12p_switch_config) == 12);

/* Flags to indicate behavior of the flow table for unmatched packets.
   These flags are used in of12p_table_stats messages to describe the current
   configuration and in of12p_table_mod messages to configure table behavior. */
enum of12p_table_config {
    OF12PTC_TABLE_MISS_CONTROLLER = 0,      /* Send to controller. */
    OF12PTC_TABLE_MISS_CONTINUE   = 1 << 0, /* Continue to the next table in the
                                             pipeline (OpenFlow 1.0
                                             behavior). */
    OF12PTC_TABLE_MISS_DROP       = 1 << 1, /* Drop the packet. */
    OF12PTC_TABLE_MISS_MASK       = 3
};

/* Table numbering. Tables can use any number up to OF12PT_MAX. */
enum of12p_table {
	/* Last usable table number. */
	OF12PTT_MAX = 0xfe,
	/* Fake tables. */
	OF12PTT_ALL = 0xff
	/* Wildcard table used for table config,
	   flow stats and flow deletes. */
};


/* Configure/Modify behavior of a flow table */
struct of12p_table_mod {
    struct of12p_header header;
    uint8_t table_id;       /* ID of the table, 0xFF indicates all tables */
    uint8_t pad[3];         /* Pad to 32 bits */
    uint32_t config;        /* Bitmap of OF12PTC_* flags */
};
OF12P_ASSERT(sizeof(struct of12p_table_mod) == 16);

/* Capabilities supported by the datapath. */
enum of12p_capabilities {
    OF12PC_FLOW_STATS     = 1 << 0,  /* Flow statistics. */
    OF12PC_TABLE_STATS    = 1 << 1,  /* Table statistics. */
    OF12PC_PORT_STATS     = 1 << 2,  /* Port statistics. */
    OF12PC_GROUP_STATS    = 1 << 3,  /* Group statistics. */
    OF12PC_IP_REASM       = 1 << 5,  /* Can reassemble IP fragments. */
    OF12PC_QUEUE_STATS    = 1 << 6,  /* Queue statistics. */
    OF12PC_ARP_MATCH_IP   = 1 << 7   /* Match IP addresses in ARP pkts. */
};

/* Flags to indicate behavior of the physical port.  These flags are
 * used in of12p_port to describe the current configuration.  They are
 * used in the of12p_port_mod message to configure the port's behavior.
 */
enum of12p_port_config {
    OF12PPC_PORT_DOWN    = 1 << 0,  /* Port is administratively down. */

    OF12PPC_NO_RECV      = 1 << 2,  /* Drop all packets received by port. */
    OF12PPC_NO_FWD       = 1 << 5,  /* Drop packets forwarded to port. */
    OF12PPC_NO_PACKET_IN = 1 << 6   /* Do not send packet-in msgs for port. */
};

/* Current state of the physical port.  These are not configurable from
 * the controller.
 */
enum of12p_port_state {
    OF12PPS_LINK_DOWN    = 1 << 0,  /* No physical link present. */
    OF12PPS_BLOCKED      = 1 << 1,  /* Port is blocked */
    OF12PPS_LIVE         = 1 << 2,  /* Live for Fast Failover Group. */
};

/* Features of ports available in a datapath. */
enum of12p_port_features {
    OF12PPF_10MB_HD    = 1 << 0,  /* 10 Mb half-duplex rate support. */
    OF12PPF_10MB_FD    = 1 << 1,  /* 10 Mb full-duplex rate support. */
    OF12PPF_100MB_HD   = 1 << 2,  /* 100 Mb half-duplex rate support. */
    OF12PPF_100MB_FD   = 1 << 3,  /* 100 Mb full-duplex rate support. */
    OF12PPF_1GB_HD     = 1 << 4,  /* 1 Gb half-duplex rate support. */
    OF12PPF_1GB_FD     = 1 << 5,  /* 1 Gb full-duplex rate support. */
    OF12PPF_10GB_FD    = 1 << 6,  /* 10 Gb full-duplex rate support. */
    OF12PPF_40GB_FD    = 1 << 7,  /* 40 Gb full-duplex rate support. */
    OF12PPF_100GB_FD   = 1 << 8,  /* 100 Gb full-duplex rate support. */
    OF12PPF_1TB_FD     = 1 << 9,  /* 1 Tb full-duplex rate support. */
    OF12PPF_OTHER      = 1 << 10, /* Other rate, not in the list. */

    OF12PPF_COPPER     = 1 << 11, /* Copper medium. */
    OF12PPF_FIBER      = 1 << 12, /* Fiber medium. */
    OF12PPF_AUTONEG    = 1 << 13, /* Auto-negotiation. */
    OF12PPF_PAUSE      = 1 << 14, /* Pause. */
    OF12PPF_PAUSE_ASYM = 1 << 15  /* Asymmetric pause. */
};

/* Description of a port */
struct of12p_port {
    uint32_t port_no;
    uint8_t pad[4];
    uint8_t hw_addr[OF12P_ETH_ALEN];
    uint8_t pad2[2];                  /* Align to 64 bits. */
    char name[OF12P_MAX_PORT_NAME_LEN]; /* Null-terminated */

    uint32_t config;        /* Bitmap of OF12PPC_* flags. */
    uint32_t state;         /* Bitmap of OF12PPS_* flags. */

    /* Bitmaps of OF12PPF_* that describe features.  All bits zeroed if
     * unsupported or unavailable. */
    uint32_t curr;          /* Current features. */
    uint32_t advertised;    /* Features being advertised by the port. */
    uint32_t supported;     /* Features supported by the port. */
    uint32_t peer;          /* Features advertised by peer. */

    uint32_t curr_speed;    /* Current port bitrate in kbps. */
    uint32_t max_speed;     /* Max port bitrate in kbps */
};
OF12P_ASSERT(sizeof(struct of12p_port) == 64);

/* Switch features. */
struct of12p_switch_features {
    struct of12p_header header;
    uint64_t datapath_id;   /* Datapath unique ID.  The lower 48-bits are for
                               a MAC address, while the upper 16-bits are
                               implementer-defined. */

    uint32_t n_buffers;     /* Max packets buffered at once. */

    uint8_t n_tables;       /* Number of tables supported by datapath. */
    uint8_t pad[3];         /* Align to 64-bits. */

    /* Features. */
    uint32_t capabilities;  /* Bitmap of support "of12p_capabilities". */
    uint32_t reserved;

    /* Port info.*/
    struct of12p_port ports[0];  /* Port definitions.  The number of ports
                                  is inferred from the length field in
                                  the header. */
};
OF12P_ASSERT(sizeof(struct of12p_switch_features) == 32);

/* What changed about the physical port */
enum of12p_port_reason {
    OF12PPR_ADD,              /* The port was added. */
    OF12PPR_DELETE,           /* The port was removed. */
    OF12PPR_MODIFY            /* Some attribute of the port has changed. */
};

/* A physical port has changed in the datapath */
struct of12p_port_status {
    struct of12p_header header;
    uint8_t reason;          /* One of OF12PPR_*. */
    uint8_t pad[7];          /* Align to 64-bits. */
    struct of12p_port desc;
};
OF12P_ASSERT(sizeof(struct of12p_port_status) == 80);

/* Modify behavior of the physical port */
struct of12p_port_mod {
    struct of12p_header header;
    uint32_t port_no;
    uint8_t pad[4];
    uint8_t hw_addr[OF12P_ETH_ALEN]; /* The hardware address is not
                                      configurable.  This is used to
                                      sanity-check the request, so it must
                                      be the same as returned in an
                                      of12p_port struct. */
    uint8_t pad2[2];        /* Pad to 64 bits. */
    uint32_t config;        /* Bitmap of OF12PPC_* flags. */
    uint32_t mask;          /* Bitmap of OF12PPC_* flags to be changed. */

    uint32_t advertise;     /* Bitmap of OF12PPF_*.  Zero all bits to prevent
                               any action taking place. */
    uint8_t pad3[4];        /* Pad to 64 bits. */
};
OF12P_ASSERT(sizeof(struct of12p_port_mod) == 40);

/* Why is this packet being sent to the controller? */
enum of12p_packet_in_reason {
    OF12PR_NO_MATCH = 0,      /* No matching flow. */
    OF12PR_ACTION = 1,        /* Action explicitly output to controller. */
    OF12PR_INVALID_TTL = 2	/* Packet has invalid TTL */
};




/* Fields to match against flows */
struct of12p_match {
	uint16_t type;			/* One of OF12PMT_* */
	uint16_t length;		/* Length of of12p_match (excluding padding) */
	/* Followed by:
	 * - Exactly (length - 4) (possibly 0) bytes containing OXM TLVs, then
	 * - Exactly ((length + 7)/8*8 - length) (between 0 and 7) bytes of
	 * all-zero bytes
	 * In summary, of12p_match is padded as needed, to make its overall size
	 * a multiple of 8, to preserve alignement in structures using it.
	 */
	uint8_t oxm_fields[4];
	/* OXMs start here - Make compiler happy */
};
OF12P_ASSERT(sizeof(struct of12p_match) == 8);


/* The match type indicates the match structure (set of fields that compose the
* match) in use. The match type is placed in the type field at the beginning
* of all match structures. The "OpenFlow Extensible Match" type corresponds
* to OXM TLV format described below and must be supported by all OpenFlow
* switches. Extensions that define other match types may be published on the
* ONF wiki. Support for extensions is optional.
*/
enum of12p_match_type {
	OF12PMT_STANDARD = 0, /* Deprecated. */
	OF12PMT_OXM = 1, 		/* OpenFlow Extensible Match */
};


struct of12p_oxm_hdr {
	uint16_t oxm_class;		/* oxm_class */
	uint8_t  oxm_field;		/* includes has_mask bit! */
	uint8_t  oxm_length;	/* oxm_length */
};




// OXM_OF_VLAN_PCP 		/* 3 bits */
// OXM_OF_IP_DSCP 		/* 6 bits */
// OXM_OF_IP_ECN		/* 2 bits */
// OXM_OF_IP_PROTO		/* 8 bits */
// OXM_OF_ICMPV4_TYPE
// OXM_OF_ICMPV4_CODE
// OXM_OF_ICMPV6_TYPE
// OXM_OF_ICMPV6_CODE
// OXM_OF_MPLS_TC		/* 3 bits */
struct of12p_oxm_ofb_uint8_t {
	struct of12p_oxm_hdr hdr;		/* oxm header */
	uint8_t byte;
	uint8_t mask;
};


// OXM_OF_ETH_TYPE
// OXM_OF_VLAN_VID (mask)
// OXM_OF_TCP_SRC
// OXM_OF_TCP_DST
// OXM_OF_UDP_SRC
// OXM_OF_UDP_DST
// OXM_OF_SCTP_SRC
// OXM_OF_SCTP_DST
// OXM_OF_ARP_OP
struct of12p_oxm_ofb_uint16_t {
	struct of12p_oxm_hdr hdr;		/* oxm header */
	uint16_t word;				/* network byte order */
	uint16_t mask;
};


// OXM_OF_IN_PORT
// OXM_OF_IN_PHY_PORT
// OXM_OF_IPV4_SRC (mask)
// OXM_OF_IPV4_DST (mask)
// OXM_OF_ARP_SPA (mask)
// OXM_OF_ARP_THA (mask)
// OXM_OF_IPV6_FLABEL (mask)
// OXM_OF_MPLS_LABEL
struct of12p_oxm_ofb_uint32_t {
	struct of12p_oxm_hdr hdr;		/* oxm header */
	uint32_t dword;				/* network byte order */
	uint32_t mask;				/* only valid, when oxm_hasmask=1 */
};


// OXM_OF_IPV6_ND_SLL
// OXM_OF_IPV6_ND_TLL
struct of12p_oxm_ofb_uint48_t {
	struct of12p_oxm_hdr hdr;		/* oxm header */
	uint8_t value[6];
	uint8_t mask[6];			/* only valid, when oxm_hasmask=1 */
};

// OXM_OF_METADATA (mask)
struct of12p_oxm_ofb_uint64_t {
	struct of12p_oxm_hdr hdr;		/* oxm header */
	uint8_t word[8];
	uint8_t mask[8];
#if 0
	uint64_t qword;				/* network byte order */
	uint64_t mask;				/* only valid, when oxm_hasmask=1 */
#endif
};


// OXM_OF_ETH_DST (mask)
// OXM_OF_ETH_SRC (mask)
struct of12p_oxm_ofb_maddr {
	struct of12p_oxm_hdr hdr;		/* oxm header */
	uint8_t addr[OF12P_ETH_ALEN];
	uint8_t mask[OF12P_ETH_ALEN]; /* only valid, when oxm_hasmask=1 */
};


// OXM_OF_IPV6_SRC (mask)
// OXM_OF_IPV6_DST (mask)
// OXM_OF_IPV6_ND_TARGET
struct of12p_oxm_ofb_ipv6_addr {
	struct of12p_oxm_hdr hdr;		/* oxm header */
	uint8_t addr[16];
	uint8_t mask[16];			/* only valid, when oxm_hasmask=1 */
};


/* OXM Class IDs.
 * The high order bit differentiate reserved classes from member classes.
 * Classes 0x0000 to 0x7FFF are member classes, allocated by ONF.
 * Classes 0x8000 to 0xFFFE are reserved classes, reserved for standardisation.
 */
enum of12p_oxm_class {
	OF12PXMC_NXM_0			= 0x0000, 	/* Backward compatibility with NXM */
	OF12PXMC_NXM_1			= 0x0001,	/* Backward compatibility with NXM */
	OF12PXMC_OPENFLOW_BASIC	= 0x8000,	/* Basic class for OpenFlow */
	OF12PXMC_EXPERIMENTER		= 0xFFFF,	/* Experimenter class */
};


/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_ofb_match_fields {
	OF12PXMT_OFB_IN_PORT = 0,			/* Switch input port. */				// required
	OF12PXMT_OFB_IN_PHY_PORT = 1,		/* Switch physical input port. */
	OF12PXMT_OFB_METADATA = 2,		/* Metadata passed between tables. */
	OF12PXMT_OFB_ETH_DST = 3,			/* Ethernet destination address. */		// required
	OF12PXMT_OFB_ETH_SRC = 4,			/* Ethernet source address. */			// required
	OF12PXMT_OFB_ETH_TYPE = 5,		/* Ethernet frame type. */				// required
	OF12PXMT_OFB_VLAN_VID = 6,		/* VLAN id. */
	OF12PXMT_OFB_VLAN_PCP = 7,		/* VLAN priority. */
	OF12PXMT_OFB_IP_DSCP = 8,			/* IP DSCP (6 bits in ToS field). */
	OF12PXMT_OFB_IP_ECN = 9,			/* IP ECN (2 bits in ToS field). */
	OF12PXMT_OFB_IP_PROTO = 10,		/* IP protocol. */						// required
	OF12PXMT_OFB_IPV4_SRC = 11,		/* IPv4 source address. */				// required
	OF12PXMT_OFB_IPV4_DST = 12,		/* IPv4 destination address. */			// required
	OF12PXMT_OFB_TCP_SRC = 13,		/* TCP source port. */					// required
	OF12PXMT_OFB_TCP_DST = 14,		/* TCP destination port. */				// required
	OF12PXMT_OFB_UDP_SRC = 15,		/* UDP source port. */					// required
	OF12PXMT_OFB_UDP_DST = 16,		/* UDP destination port. */				// required
	OF12PXMT_OFB_SCTP_SRC = 17,		/* SCTP source port. */
	OF12PXMT_OFB_SCTP_DST = 18,		/* SCTP destination port. */
	OF12PXMT_OFB_ICMPV4_TYPE = 19,	/* ICMP type. */
	OF12PXMT_OFB_ICMPV4_CODE = 20,	/* ICMP code. */
	OF12PXMT_OFB_ARP_OP = 21,			/* ARP opcode. */
	OF12PXMT_OFB_ARP_SPA = 22,		/* ARP source IPv4 address. */
	OF12PXMT_OFB_ARP_TPA = 23,		/* ARP target IPv4 address. */
	OF12PXMT_OFB_ARP_SHA = 24,		/* ARP source hardware address. */
	OF12PXMT_OFB_ARP_THA = 25,		/* ARP target hardware address. */
	OF12PXMT_OFB_IPV6_SRC = 26,		/* IPv6 source address. */				// required
	OF12PXMT_OFB_IPV6_DST = 27,		/* IPv6 destination address. */			// required
	OF12PXMT_OFB_IPV6_FLABEL = 28,	/* IPv6 Flow Label */
	OF12PXMT_OFB_ICMPV6_TYPE = 29,	/* ICMPv6 type. */
	OF12PXMT_OFB_ICMPV6_CODE = 30,	/* ICMPv6 code. */
	OF12PXMT_OFB_IPV6_ND_TARGET = 31,	/* Target address for ND. */
	OF12PXMT_OFB_IPV6_ND_SLL = 32,	/* Source link-layer for ND. */
	OF12PXMT_OFB_IPV6_ND_TLL = 33,	/* Target link-layer for ND. */
	OF12PXMT_OFB_MPLS_LABEL = 34,		/* MPLS label. */
	OF12PXMT_OFB_MPLS_TC = 35,		/* MPLS TC. */
	/* PPP/PPPoE related extensions */
	OF12PXMT_OFB_PPPOE_CODE = 36,		/* PPPoE code */
	OF12PXMT_OFB_PPPOE_TYPE = 37,		/* PPPoE type */
	OF12PXMT_OFB_PPPOE_SID = 38,		/* PPPoE session id */
	OF12PXMT_OFB_PPP_PROT = 39,		/* PPP protocol */
};


/* Header for OXM experimenter match fields. */
struct of12p_oxm_experimenter_header {
	uint32_t oxm_header;			/* oxm_class = OF12PXMC_EXPERIMENTER */
	uint32_t experimenter;			/* Experimenter ID which takes the same
									   form as in struct of12p_experimenter_header. */
};
OF12P_ASSERT(sizeof(struct of12p_oxm_experimenter_header) == 8);



#if 0
/* Packet received on port (datapath -> controller). */
struct of12p_packet_in {
    struct of12p_header header;
    uint32_t buffer_id;     /* ID assigned by datapath. */
    uint32_t in_port;       /* Port on which frame was received. */
    uint32_t in_phy_port;   /* Physical Port on which frame was received. */
    uint16_t total_len;     /* Full length of frame. */
    uint8_t reason;         /* Reason packet is being sent (one of OF12PR_*) */
    uint8_t table_id;       /* ID of the table that was looked up */
    uint8_t data[0];        /* Ethernet frame, halfway through 32-bit word,
                               so the IP header is 32-bit aligned.  The
                               amount of data is inferred from the length
                               field in the header.  Because of padding,
                               offsetof(struct of12p_packet_in, data) ==
                               sizeof(struct of12p_packet_in) - 2. */
};
OF12P_ASSERT(sizeof(struct of12p_packet_in) == 24);
#endif

/* Packet received on port (datapath -> controller). */
struct of12p_packet_in {
	struct of12p_header header;
	uint32_t buffer_id;			/* ID assigned by datapath. */
	uint16_t total_len;			/* Full length of frame. */
	uint8_t reason;				/* Reason packet is being sent (one of OF12PR_*) */
	uint8_t table_id;			/* ID of the table that was looked up */
	struct of12p_match match; 	/* Packet metadata. Variable size. */
	/* Followed by:
	 * - Exactly 2 all-zero padding bytes, then
	 * - An Ethernet frame whose length is inferred from header.length.
	 * The padding bytes preceding the Ethernet frame ensure that the IP
	 * header (if any) following the Ethernet header is 32-bit aligned.
	 */
	//uint8_t pad[2];			/* Align to 64 bit + 16 bit */
	//uint8_t data[0];			/* Ethernet frame */
};
OF12P_ASSERT(sizeof(struct of12p_packet_in) == 24);

#define OF12P_NO_BUFFER	0xffffffff

enum of12p_action_type {
	OF12PAT_OUTPUT 		= 0, 	/* Output to switch port. */
	OF12PAT_COPY_TTL_OUT 	= 11, 	/* Copy TTL "outwards" -- from next-to-outermost to outermost */
	OF12PAT_COPY_TTL_IN 	= 12, 	/* Copy TTL "inwards" -- from outermost to next-to-outermost */
	OF12PAT_SET_MPLS_TTL 	= 15, 	/* MPLS TTL */
	OF12PAT_DEC_MPLS_TTL 	= 16, 	/* Decrement MPLS TTL */
	OF12PAT_PUSH_VLAN 	= 17, 	/* Push a new VLAN tag */
	OF12PAT_POP_VLAN 		= 18, 	/* Pop the outer VLAN tag */
	OF12PAT_PUSH_MPLS 	= 19, 	/* Push a new MPLS tag */
	OF12PAT_POP_MPLS 		= 20, 	/* Pop the outer MPLS tag */
	OF12PAT_SET_QUEUE 	= 21, 	/* Set queue id when outputting to a port */
	OF12PAT_GROUP 		= 22, 	/* Apply group. */
	OF12PAT_SET_NW_TTL 	= 23, 	/* IP TTL. */
	OF12PAT_DEC_NW_TTL 	= 24, 	/* Decrement IP TTL. */
	OF12PAT_SET_FIELD 	= 25, 	/* Set a header field using OXM TLV format. */
	OF12PAT_PUSH_PPPOE 	= 26,	/* Push a new PPPoE tag */
	OF12PAT_POP_PPPOE 	= 27,	/* Pop the PPPoE tag */
	OF12PAT_PUSH_PPP 		= 28,	/* Push a new PPP tag */
	OF12PAT_POP_PPP 		= 29,	/* Pop the PPP tag */
	OF12PAT_EXPERIMENTER	= 0xffff
};


/* Action structure for OF12PAT_OUTPUT, which sends packets out 'port'.
 * When the 'port' is the OF12PP_CONTROLLER, 'max_len' indicates the max
 * number of bytes to send.  A 'max_len' of zero means no bytes of the
 * packet should be sent.*/
struct of12p_action_output {
    uint16_t type;                  /* OF12PAT_OUTPUT. */
    uint16_t len;                   /* Length is 16. */
    uint32_t port;                  /* Output port. */
    uint16_t max_len;               /* Max length to send to controller. */
    uint8_t pad[6];                 /* Pad to 64 bits. */
};
OF12P_ASSERT(sizeof(struct of12p_action_output) == 16);

enum of12p_controller_max_len {
	OF12PCML_MAX = 0xffe5, /* maximum max_len value which can be used to request a specific byte length. */
	OF12PCML_NO_BUFFER = 0xffff, /* indicates that no buffering should be
	applied and the whole packet is to be
	sent to the controller. */
};


/* Action structure for OF12PAT_SET_FIELD. */
struct of12p_action_set_field {
	uint16_t type; 	/* OF12PAT_SET_FIELD. */
	uint16_t len; 	/* Length is padded to 64 bits. */
					/* Followed by:
					 * - Exactly oxm_len bytes containing a single OXM TLV, then
					 * - Exactly ((oxm_len + 4) + 7)/8*8 - (oxm_len + 4) (between 0 and 7)
					 *   bytes of all-zero bytes
					 */
	uint8_t field[4]; /* OXM TLV - Make compiler happy */
};
OF12P_ASSERT(sizeof(struct of12p_action_set_field) == 8);


/* Action structure for OF12PAT_SET_MPLS_TTL. */
struct of12p_action_mpls_ttl {
    uint16_t type;                  /* OF12PAT_SET_MPLS_TTL. */
    uint16_t len;                   /* Length is 8. */
    uint8_t mpls_ttl;               /* MPLS TTL */
    uint8_t pad[3];
};
OF12P_ASSERT(sizeof(struct of12p_action_mpls_ttl) == 8);

/* Action structure for OF12PAT_PUSH_VLAN/MPLS. */
struct of12p_action_push {
    uint16_t type;                  /* OF12PAT_PUSH_VLAN/MPLS. */
    uint16_t len;                   /* Length is 8. */
    uint16_t ethertype;             /* Ethertype */
    uint8_t pad[2];
};
OF12P_ASSERT(sizeof(struct of12p_action_push) == 8);

/* Action structure for OF12PAT_POP_MPLS. */
struct of12p_action_pop_mpls {
    uint16_t type;                  /* OF12PAT_POP_MPLS. */
    uint16_t len;                   /* Length is 8. */
    uint16_t ethertype;             /* Ethertype */
    uint8_t pad[2];
};
OF12P_ASSERT(sizeof(struct of12p_action_pop_mpls) == 8);


/* Action structure for OF12PAT_POP_PPPOE. */
struct of12p_action_pop_pppoe {
    uint16_t type;                  /* OF12PAT_POP_PPPOE. */
    uint16_t len;                   /* Length is 8. */
    uint16_t ethertype;             /* Ethertype */
    uint8_t pad[2];
};
OF12P_ASSERT(sizeof(struct of12p_action_pop_pppoe) == 8);



/* Action structure for OF12PAT_GROUP. */
struct of12p_action_group {
    uint16_t type;                  /* OF12PAT_GROUP. */
    uint16_t len;                   /* Length is 8. */
    uint32_t group_id;              /* Group identifier. */
};
OF12P_ASSERT(sizeof(struct of12p_action_group) == 8);

/* Action structure for OF12PAT_SET_NW_TTL. */
struct of12p_action_nw_ttl {
    uint16_t type;                  /* OF12PAT_SET_NW_TTL. */
    uint16_t len;                   /* Length is 8. */
    uint8_t nw_ttl;                 /* IP TTL */
    uint8_t pad[3];
};
OF12P_ASSERT(sizeof(struct of12p_action_nw_ttl) == 8);

/* Action header for OF12PAT_EXPERIMENTER.
 * The rest of the body is experimenter-defined. */
struct of12p_action_experimenter_header {
    uint16_t type;                  /* OF12PAT_EXPERIMENTER. */
    uint16_t len;                   /* Length is a multiple of 8. */
    uint32_t experimenter;          /* Experimenter ID which takes the same
                                       form as in struct
                                       of12p_experimenter_header. */
};
OF12P_ASSERT(sizeof(struct of12p_action_experimenter_header) == 8);

/* Action header that is common to all actions.  The length includes the
 * header and any padding used to make the action 64-bit aligned.
 * NB: The length of an action *must* always be a multiple of eight. */
struct of12p_action_header {
    uint16_t type;                  /* One of OF12PAT_*. */
    uint16_t len;                   /* Length of action, including this
                                       header.  This is the length of action,
                                       including any padding to make it
                                       64-bit aligned. */
    uint8_t pad[4];
};
OF12P_ASSERT(sizeof(struct of12p_action_header) == 8);





/* Send packet (controller -> datapath). */
struct of12p_packet_out {
    struct of12p_header header;
    uint32_t buffer_id;           /* ID assigned by datapath (-1 if none). */
    uint32_t in_port;             /* Packet's input port or OF12PP_CONTROLLER. */
    uint16_t actions_len;         /* Size of action array in bytes. */
    uint8_t pad[6];
    struct of12p_action_header actions[0]; /* Action list. */
    /* uint8_t data[0]; */        /* Packet data.  The length is inferred
                                     from the length field in the header.
                                     (Only meaningful if buffer_id == -1.) */
};
OF12P_ASSERT(sizeof(struct of12p_packet_out) == 24);

enum of12p_flow_mod_command {
    OF12PFC_ADD,              /* New flow. */
    OF12PFC_MODIFY,           /* Modify all matching flows. */
    OF12PFC_MODIFY_STRICT,    /* Modify entry strictly matching wildcards and
                               priority. */
    OF12PFC_DELETE,           /* Delete all matching flows. */
    OF12PFC_DELETE_STRICT     /* Delete entry strictly matching wildcards and
                               priority. */
};

/* Group commands */
enum of12p_group_mod_command {
    OF12PGC_ADD,              /* New group. */
    OF12PGC_MODIFY,           /* Modify all matching groups. */
    OF12PGC_DELETE,           /* Delete all matching groups. */
};

/* Flow wildcards. */
enum of12p_flow_wildcards {
    OF12PFW_IN_PORT     = 1 << 0,  /* Switch input port. */
    OF12PFW_DL_VLAN     = 1 << 1,  /* VLAN id. */
    OF12PFW_DL_VLAN_PCP = 1 << 2,  /* VLAN priority. */
    OF12PFW_DL_TYPE     = 1 << 3,  /* Ethernet frame type. */
    OF12PFW_NW_TOS      = 1 << 4,  /* IP ToS (DSCP field, 6 bits). */
    OF12PFW_NW_PROTO    = 1 << 5,  /* IP protocol. */
    OF12PFW_TP_SRC      = 1 << 6,  /* TCP/UDP/SCTP source port. */
    OF12PFW_TP_DST      = 1 << 7,  /* TCP/UDP/SCTP destination port. */
    OF12PFW_MPLS_LABEL  = 1 << 8,  /* MPLS label. */
    OF12PFW_MPLS_TC     = 1 << 9,  /* MPLS TC. */
#ifdef ORAN
    OF12PFW_PPPOE_CODE  = 1 << 10, /* PPPoE code */
    OF12PFW_PPPOE_TYPE  = 1 << 11, /* PPPoE type */
    OF12PFW_PPPOE_SESS  = 1 << 12, /* PPPoE session */
    OF12PFW_PPP_CODE	  = 1 << 13, /* PPP code */
#endif

    /* Wildcard all fields. */
    OF12PFW_ALL           = ((1 << 14) - 1)
    //OF12PFW_ALL           = ((1 << 10) - 1)
};

/* The wildcards for ICMP type and code fields use the transport source
 * and destination port fields, respectively. */
#define OF12PFW_ICMP_TYPE OF12PFW_TP_SRC
#define OF12PFW_ICMP_CODE OF12PFW_TP_DST

/* Values below this cutoff are 802.3 packets and the two bytes
 * following MAC addresses are used as a frame length.  Otherwise, the
 * two bytes are used as the Ethernet type.
 */
#define OF12P_DL_TYPE_ETH2_CUTOFF   0x0600

/* Value of dl_type to indicate that the frame does not include an
 * Ethernet type.
 */
#define OF12P_DL_TYPE_NOT_ETH_TYPE  0x05ff

/*
 * OpenFlow 1.2 [start]
 */

/* The VLAN id is 12-bits, so we can use the entire 16 bits to indicate
* special conditions.
*/
enum of12p_vlan_id {
	OF12PVID_PRESENT = 0x1000, /* Bit that indicate that a VLAN id is set */
	OF12PVID_NONE = 0x0000, /* No VLAN id was set. */
};





/*
 * OpenFlow 1.2 [end]
 */




/* The match fields for ICMP type and code use the transport source and
 * destination port fields, respectively. */
#define icmp_type tp_src
#define icmp_code tp_dst

/* Value used in "idle_timeout" and "hard_timeout" to indicate that the entry
 * is permanent. */
#define OF12P_FLOW_PERMANENT 0

/* By default, choose a priority in the middle. */
#define OF12P_DEFAULT_PRIORITY 0x8000

enum of12p_instruction_type {
    OF12PIT_GOTO_TABLE = 1,       /* Setup the next table in the lookup
                                   pipeline */
    OF12PIT_WRITE_METADATA = 2,   /* Setup the metadata field for use later in
                                   pipeline */
    OF12PIT_WRITE_ACTIONS = 3,    /* Write the action(s) onto the datapath action
                                   set */
    OF12PIT_APPLY_ACTIONS = 4,    /* Applies the action(s) immediately */
    OF12PIT_CLEAR_ACTIONS = 5,    /* Clears all actions from the datapath
                                   action set */

    OF12PIT_EXPERIMENTER = 0xFFFF  /* Experimenter instruction */
};

/* Generic of12p_instruction structure */
struct of12p_instruction {
    uint16_t type;                /* Instruction type */
    uint16_t len;                 /* Length of this struct in bytes. */
    uint8_t pad[4];               /* Align to 64-bits */
};
OF12P_ASSERT(sizeof(struct of12p_instruction) == 8);

/* Instruction structure for OF12PIT_GOTO_TABLE */
struct of12p_instruction_goto_table {
    uint16_t type;                /* OF12PIT_GOTO_TABLE */
    uint16_t len;                 /* Length of this struct in bytes. */
    uint8_t table_id;             /* Set next table in the lookup pipeline */
    uint8_t pad[3];               /* Pad to 64 bits. */
};
OF12P_ASSERT(sizeof(struct of12p_instruction_goto_table) == 8);

/* Instruction structure for OF12PIT_WRITE_METADATA */
struct of12p_instruction_write_metadata {
    uint16_t type;                /* OF12PIT_WRITE_METADATA */
    uint16_t len;                 /* Length of this struct in bytes. */
    uint8_t pad[4];               /* Align to 64-bits */
    uint64_t metadata;            /* Metadata value to write */
    uint64_t metadata_mask;       /* Metadata write bitmask */
};
OF12P_ASSERT(sizeof(struct of12p_instruction_write_metadata) == 24);

/* Instruction structure for OF12PIT_WRITE/APPLY/CLEAR_ACTIONS */
struct of12p_instruction_actions {
    uint16_t type;              /* One of OF12PIT_*_ACTIONS */
    uint16_t len;               /* Length of this struct in bytes. */
    uint8_t pad[4];             /* Align to 64-bits */
    struct of12p_action_header actions[0];  /* Actions associated with
                                             OF12PIT_WRITE_ACTIONS and
                                             OF12PIT_APPLY_ACTIONS */
};
OF12P_ASSERT(sizeof(struct of12p_instruction_actions) == 8);

/* Instruction structure for experimental instructions */
struct of12p_instruction_experimenter {
    uint16_t type;		/* OF12PIT_EXPERIMENTER */
    uint16_t len;               /* Length of this struct in bytes */
    uint32_t experimenter;      /* Experimenter ID:
                                 * - MSB 0: low-order bytes are IEEE OUI.
                                 * - MSB != 0: defined by OpenFlow
                                 *   consortium. */
    /* Experimenter-defined arbitrary additional data. */
};
OF12P_ASSERT(sizeof(struct of12p_instruction_experimenter) == 8);

enum of12p_flow_mod_flags {
    OF12PFF_SEND_FLOW_REM = 1 << 0,  /* Send flow removed message when flow
                                    * expires or is deleted. */
    OF12PFF_CHECK_OVERLAP = 1 << 1,  /* Check for overlapping entries first. */
    OF12PFF_RESET_COUNTS  = 1 << 2   /* Reset flow packet and byte counts. */
};

/* Flow setup and teardown (controller -> datapath). */
struct of12p_flow_mod {
    struct of12p_header header;
    uint64_t cookie;             /* Opaque controller-issued identifier. */
    uint64_t cookie_mask;        /* Mask used to restrict the cookie bits
                                    that must match when the command is
                                    OF12PFC_MODIFY* or OF12PFC_DELETE*. A value
                                    of 0 indicates no restriction. */

    /* Flow actions. */
    uint8_t table_id;             /* ID of the table to put the flow in */
    uint8_t command;              /* One of OF12PFC_*. */
    uint16_t idle_timeout;        /* Idle time before discarding (seconds). */
    uint16_t hard_timeout;        /* Max time before discarding (seconds). */
    uint16_t priority;            /* Priority level of flow entry. */
    uint32_t buffer_id;           /* Buffered packet to apply to (or -1).
                                     Not meaningful for OF12PFC_DELETE*. */
    uint32_t out_port;            /* For OF12PFC_DELETE* commands, require
                                     matching entries to include this as an
                                     output port.  A value of OF12PP_ANY
                                     indicates no restriction. */
    uint32_t out_group;           /* For OF12PFC_DELETE* commands, require
                                     matching entries to include this as an
                                     output group.  A value of OF12PG_ANY
                                     indicates no restriction. */
    uint16_t flags;               /* One of OF12PFF_*. */
    uint8_t pad[2];
    struct of12p_match match;       /* Fields to match */
    //struct of12p_instruction instructions[0]; /* Instruction set */
};
OF12P_ASSERT(sizeof(struct of12p_flow_mod) == 56);

/* Group numbering. Groups can use any number up to OF12PG_MAX. */
enum of12p_group {
    /* Last usable group number. */
    OF12PG_MAX        = 0xffffff00,

    /* Fake groups. */
    OF12PG_ALL        = 0xfffffffc,  /* Represents all groups for group delete
                                      commands. */
    OF12PG_ANY        = 0xffffffff   /* Wildcard group used only for flow stats
                                      requests. Selects all flows regardless of
                                      group (including flows with no group).
                                      */
};

/* Bucket for use in groups. */
struct of12p_bucket {
    uint16_t len;                   /* Length the bucket in bytes, including
                                       this header and any padding to make it
                                       64-bit aligned. */
    uint16_t weight;                /* Relative weight of bucket.  Only
                                       defined for select groups. */
    uint32_t watch_port;            /* Port whose state affects whether this
                                       bucket is live.  Only required for fast
                                       failover groups. */
    uint32_t watch_group;           /* Group whose state affects whether this
                                       bucket is live.  Only required for fast
                                       failover groups. */
    uint8_t pad[4];
    struct of12p_action_header actions[0]; /* The action length is inferred
                                           from the length field in the
                                           header. */
};
OF12P_ASSERT(sizeof(struct of12p_bucket) == 16);

/* Group setup and teardown (controller -> datapath). */
struct of12p_group_mod {
    struct of12p_header header;
    uint16_t command;             /* One of OF12PGC_*. */
    uint8_t type;                 /* One of OF12PGT_*. */
    uint8_t pad;                  /* Pad to 64 bits. */
    uint32_t group_id;            /* Group identifier. */
    struct of12p_bucket buckets[0]; /* The bucket length is inferred from the
                                     length field in the header. */
};
OF12P_ASSERT(sizeof(struct of12p_group_mod) == 16);

/* Group types.  Values in the range [128, 255] are reserved for experimental
 * use. */
enum of12p_group_type {
    OF12PGT_ALL 		= 0,	/* All (multicast/broadcast) group.  */
    OF12PGT_SELECT 	= 1,   	/* Select group. */
    OF12PGT_INDIRECT 	= 2, 	/* Indirect group. */
    OF12PGT_FF 		= 3,	/* Fast failover group. */
};

/* Why was this flow removed? */
enum of12p_flow_removed_reason {
    OF12PRR_IDLE_TIMEOUT,         /* Flow idle time exceeded idle_timeout. */
    OF12PRR_HARD_TIMEOUT,         /* Time exceeded hard_timeout. */
    OF12PRR_DELETE,               /* Evicted by a DELETE flow mod. */
    OF12PRR_GROUP_DELETE          /* Group was removed. */
};

/* Flow removed (datapath -> controller). */ // adjusted to OF1.2
struct of12p_flow_removed {
    struct of12p_header header;
    uint64_t cookie;          /* Opaque controller-issued identifier. */

    uint16_t priority;        /* Priority level of flow entry. */
    uint8_t reason;           /* One of OF12PRR_*. */
    uint8_t table_id;         /* ID of the table */

    uint32_t duration_sec;    /* Time flow was alive in seconds. */
    uint32_t duration_nsec;   /* Time flow was alive in nanoseconds beyond
                                 duration_sec. */
    uint16_t idle_timeout;    /* Idle timeout from original flow mod. */
    uint16_t hard_timeout;    /* Idle timeout from original flow mod. */
    uint64_t packet_count;
    uint64_t byte_count;
    struct of12p_match match;   /* Description of fields. */
};
OF12P_ASSERT(sizeof(struct of12p_flow_removed) == 56);

/* Values for ’type’ in of12p_error_message. These values are immutable: they
* will not change in future versions of the protocol (although new values may
* be added). */
enum of12p_error_type {
	OF12PET_HELLO_FAILED = 0, /* Hello protocol failed. */
	OF12PET_BAD_REQUEST = 1, /* Request was not understood. */
	OF12PET_BAD_ACTION = 2, /* Error in action description. */
	OF12PET_BAD_INSTRUCTION = 3, /* Error in instruction list. */
	OF12PET_BAD_MATCH = 4, /* Error in match. */
	OF12PET_FLOW_MOD_FAILED = 5, /* Problem modifying flow entry. */
	OF12PET_GROUP_MOD_FAILED = 6, /* Problem modifying group entry. */
	OF12PET_PORT_MOD_FAILED = 7, /* Port mod request failed. */
	OF12PET_TABLE_MOD_FAILED = 8, /* Table mod request failed. */
	OF12PET_QUEUE_OP_FAILED = 9, /* Queue operation failed. */
	OF12PET_SWITCH_CONFIG_FAILED = 10, /* Switch config request failed. */
	OF12PET_ROLE_REQUEST_FAILED = 11, /* Controller Role request failed. */
	OF12PET_EXPERIMENTER = 0xffff /* Experimenter error messages. */
};

/* of12p_error_msg 'code' values for OF12PET_HELLO_FAILED.  'data' contains an
 * ASCII text string that may give failure details. */
enum of12p_hello_failed_code {
    OF12PHFC_INCOMPATIBLE,        /* No compatible version. */
    OF12PHFC_EPERM                /* Permissions error. */
};

/* of12p_error_msg ’code’ values for OF12PET_BAD_REQUEST. ’data’ contains at least
* the first 64 bytes of the failed request. */
enum of12p_bad_request_code {
	OF12PBRC_BAD_VERSION = 0, /* of12p_header.version not supported. */
	OF12PBRC_BAD_TYPE = 1, /* of12p_header.type not supported. */
	OF12PBRC_BAD_STAT = 2, /* of12p_stats_request.type not supported. */
	OF12PBRC_BAD_EXPERIMENTER = 3, /* Experimenter id not supported
	 	 	 	 	 	 	 	 * (in of12p_experimenter_header or
	 	 	 	 	 	 	 	 * of12p_stats_request or of12p_stats_reply). */
	OF12PBRC_BAD_EXP_TYPE = 4, /* Experimenter type not supported. */
	OF12PBRC_EPERM = 5, /* Permissions error. */
	OF12PBRC_BAD_LEN = 6, /* Wrong request length for type. */
	OF12PBRC_BUFFER_EMPTY = 7, /* Specified buffer has already been used. */
	OF12PBRC_BUFFER_UNKNOWN = 8, /* Specified buffer does not exist. */
	OF12PBRC_BAD_TABLE_ID = 9, /* Specified table-id invalid or does not
	 	 	 	 	 	 	  * exist. */
	OF12PBRC_IS_SLAVE = 10, /* Denied because controller is slave. */
	OF12PBRC_BAD_PORT = 11, /* Invalid port. */
	OF12PBRC_BAD_PACKET = 12, /* Invalid packet in packet-out. */
};


/* of12p_error_msg 'code' values for OF12PET_BAD_ACTION.  'data' contains at least
 * the first 64 bytes of the failed request. */
enum of12p_bad_action_code {
    OF12PBAC_BAD_TYPE,           /* Unknown action type. */
    OF12PBAC_BAD_LEN,            /* Length problem in actions. */
    OF12PBAC_BAD_EXPERIMENTER,   /* Unknown experimenter id specified. */
    OF12PBAC_BAD_EXPERIMENTER_TYPE, /* Unknown action type for experimenter id. */
    OF12PBAC_BAD_OUT_PORT,       /* Problem validating output port. */
    OF12PBAC_BAD_ARGUMENT,       /* Bad action argument. */
    OF12PBAC_EPERM,              /* Permissions error. */
    OF12PBAC_TOO_MANY,           /* Can't handle this many actions. */
    OF12PBAC_BAD_QUEUE,          /* Problem validating output queue. */
    OF12PBAC_BAD_OUT_GROUP,      /* Invalid group id in forward action. */
    OF12PBAC_MATCH_INCONSISTENT, /* Action can't apply for this match. */
    OF12PBAC_UNSUPPORTED_ORDER,  /* Action order is unsupported for the action
				  list in an Apply-Actions instruction */
    OF12PBAC_BAD_TAG,            /* Actions uses an unsupported
                                  tag/encap. */
};

/* of12p_error_msg 'code' values for OF12PET_BAD_INSTRUCTION.  'data' contains at least
 * the first 64 bytes of the failed request. */
enum of12p_bad_instruction_code {
    OF12PBIC_UNKNOWN_INST,       /* Unknown instruction. */
    OF12PBIC_UNSUP_INST,         /* Switch or table does not support the
                                  instruction. */
    OF12PBIC_BAD_TABLE_ID,       /* Invalid Table-ID specified. */
    OF12PBIC_UNSUP_METADATA,     /* Metadata value unsupported by datapath. */
    OF12PBIC_UNSUP_METADATA_MASK,/* Metadata mask value unsupported by
                                  datapath. */
    OF12PBIC_UNSUP_EXP_INST,     /* Specific experimenter instruction
                                  unsupported. */
};

/* of12p_error_msg 'code' values for OF12PET_BAD_MATCH.  'data' contains at least
 * the first 64 bytes of the failed request. */
enum of12p_bad_match_code {
    OF12PBMC_BAD_TYPE,            /* Unsupported match type specified by the
                                   match */
    OF12PBMC_BAD_LEN,             /* Length problem in match. */
    OF12PBMC_BAD_TAG,             /* Match uses an unsupported tag/encap. */
    OF12PBMC_BAD_DL_ADDR_MASK,    /* Unsupported datalink addr mask - switch does
                                   not support arbitrary datalink address
                                   mask. */
    OF12PBMC_BAD_NW_ADDR_MASK,    /* Unsupported network addr mask - switch does
                                   not support arbitrary network address
                                   mask. */
    OF12PBMC_BAD_WILDCARDS,       /* Unsupported wildcard specified in the
                                   match. */
    OF12PBMC_BAD_FIELD,		/* Unsupported field in the match. */
    OF12PBMC_BAD_VALUE,		/* Unsupported value in a match field. */
};

/* of12p_error_msg 'code' values for OF12PET_FLOW_MOD_FAILED.  'data' contains
 * at least the first 64 bytes of the failed request. */
enum of12p_flow_mod_failed_code {
    OF12PFMFC_UNKNOWN,            /* Unspecified error. */
    OF12PFMFC_TABLE_FULL,         /* Flow not added because table was full. */
    OF12PFMFC_BAD_TABLE_ID,       /* Table does not exist */
    OF12PFMFC_OVERLAP,            /* Attempted to add overlapping flow with
                                   CHECK_OVERLAP flag set. */
    OF12PFMFC_EPERM,              /* Permissions error. */
    OF12PFMFC_BAD_TIMEOUT,        /* Flow not added because of unsupported
                                   idle/hard timeout. */
    OF12PFMFC_BAD_COMMAND,        /* Unsupported or unknown command. */
};

/* of12p_error_msg 'code' values for OF12PET_GROUP_MOD_FAILED.  'data' contains
 * at least the first 64 bytes of the failed request. */
enum of12p_group_mod_failed_code {
    OF12PGMFC_GROUP_EXISTS,             /* Group not added because a group ADD
                                       * attempted to replace an
                                       * already-present group. */
    OF12PGMFC_INVALID_GROUP,            /* Group not added because Group specified
                                       * is invalid. */
    OF12PGMFC_WEIGHT_UNSUPPORTED,       /* Switch does not support unequal load
                                       * sharing with select groups. */
    OF12PGMFC_OUT_OF_GROUPS,            /* The group table is full. */
    OF12PGMFC_OUT_OF_BUCKETS,           /* The maximum number of action buckets
                                       * for a group has been exceeded. */
    OF12PGMFC_CHAINING_UNSUPPORTED,     /* Switch does not support groups that
                                       * forward to groups. */
    OF12PGMFC_WATCH_UNSUPPORTED,        /* This group cannot watch the
                                         watch_port or watch_group specified. */
    OF12PGMFC_LOOP,                     /* Group entry would cause a loop. */
    OF12PGMFC_UNKNOWN_GROUP,            /* Group not modified because a group
                                         MODIFY attempted to modify a
                                         non-existent group. */
};

/* of12p_error_msg 'code' values for OF12PET_PORT_MOD_FAILED.  'data' contains
 * at least the first 64 bytes of the failed request. */
enum of12p_port_mod_failed_code {
    OF12PPMFC_BAD_PORT,            /* Specified port number does not exist. */
    OF12PPMFC_BAD_HW_ADDR,         /* Specified hardware address does not
                                  * match the port number. */
    OF12PPMFC_BAD_CONFIG,          /* Specified config is invalid. */
    OF12PPMFC_BAD_ADVERTISE        /* Specified advertise is invalid. */
};

/* of12p_error_msg 'code' values for OF12PET_TABLE_MOD_FAILED.  'data' contains
 * at least the first 64 bytes of the failed request. */
enum of12p_table_mod_failed_code {
    OF12PTMFC_BAD_TABLE,           /* Specified table does not exist. */
    OF12PTMFC_BAD_CONFIG           /* Specified config is invalid. */
};

/* of12p_error msg 'code' values for OF12PET_QUEUE_OP_FAILED. 'data' contains
 * at least the first 64 bytes of the failed request */
enum of12p_queue_op_failed_code {
    OF12PQOFC_BAD_PORT,           /* Invalid port (or port does not exist). */
    OF12PQOFC_BAD_QUEUE,          /* Queue does not exist. */
    OF12PQOFC_EPERM               /* Permissions error. */
};

/* of12p_error_msg 'code' values for OF12PET_SWITCH_CONFIG_FAILED. 'data' contains
 * at least the first 64 bytes of the failed request. */
enum of12p_switch_config_failed_code {
    OF12PSCFC_BAD_FLAGS,           /* Specified flags is invalid. */
    OF12PSCFC_BAD_LEN              /* Specified len is invalid. */
};

/* of12p_error_msg ’code’ values for OF12PET_ROLE_REQUEST_FAILED. ’data’ contains
* at least the first 64 bytes of the failed request. */
enum of12p_role_request_failed_code {
	OF12PRRFC_STALE = 0, /* Stale Message: old generation_id. */
	OF12PRRFC_UNSUP = 1, /* Controller role change unsupported. */
	OF12PRRFC_BAD_ROLE = 2, /* Invalid role. */
};

/* OF12PT_ERROR: Error message (datapath -> controller). */
struct of12p_error_msg {
    struct of12p_header header;

    uint16_t type;
    uint16_t code;
    uint8_t data[0];          /* Variable-length data.  Interpreted based
                                 on the type and code.  No padding. */
};
OF12P_ASSERT(sizeof(struct of12p_error_msg) == 12);

enum of12p_stats_types {
    /* Description of this OpenFlow switch.
     * The request body is empty.
     * The reply body is struct of12p_desc_stats. */
    OF12PST_DESC = 0,

    /* Individual flow statistics.
     * The request body is struct of12p_flow_stats_request.
     * The reply body is an array of struct of12p_flow_stats. */
    OF12PST_FLOW = 1,

    /* Aggregate flow statistics.
     * The request body is struct of12p_aggregate_stats_request.
     * The reply body is struct of12p_aggregate_stats_reply. */
    OF12PST_AGGREGATE = 2,

    /* Flow table statistics.
     * The request body is empty.
     * The reply body is an array of struct of12p_table_stats. */
    OF12PST_TABLE = 3,

    /* Port statistics.
     * The request body is struct of12p_port_stats_request.
     * The reply body is an array of struct of12p_port_stats. */
    OF12PST_PORT = 4,

    /* Queue statistics for a port
     * The request body defines the port
     * The reply body is an array of struct of12p_queue_stats */
    OF12PST_QUEUE = 5,

    /* Group counter statistics.
     * The request body is empty.
     * The reply is struct of12p_group_stats. */
    OF12PST_GROUP = 6,

    /* Group description statistics.
     * The request body is empty.
     * The reply body is struct of12p_group_desc_stats. */
    OF12PST_GROUP_DESC = 7,

    /* Group features.
    * The request body is empty.
    * The reply body is struct of12p_group_features_stats. */
    OF12PST_GROUP_FEATURES = 8,

    /* Experimenter extension.
     * The request and reply bodies begin with a 32-bit experimenter ID,
     * which takes the same form as in "struct of12p_experimenter_header".
     * The request and reply bodies are otherwise experimenter-defined. */
    OF12PST_EXPERIMENTER = 0xffff
};

struct of12p_stats_request {
    struct of12p_header header;
    uint16_t type;              /* One of the OF12PST_* constants. */
    uint16_t flags;             /* OF12PSF_REQ_* flags (none yet defined). */
    uint8_t pad[4];
    uint8_t body[0];            /* Body of the request. */
};
OF12P_ASSERT(sizeof(struct of12p_stats_request) == 16);

enum of12p_stats_reply_flags {
    OF12PSF_REPLY_MORE  = 1 << 0  /* More replies to follow. */
};

struct of12p_stats_reply {
    struct of12p_header header;
    uint16_t type;              /* One of the OF12PST_* constants. */
    uint16_t flags;             /* OF12PSF_REPLY_* flags. */
    uint8_t pad[4];
    uint8_t body[0];            /* Body of the reply. */
};
OF12P_ASSERT(sizeof(struct of12p_stats_reply) == 16);

#define DESC_STR_LEN   256
#define SERIAL_NUM_LEN 32
/* Body of reply to OF12PST_DESC request.  Each entry is a NULL-terminated
 * ASCII string. */
struct of12p_desc_stats {
    char mfr_desc[DESC_STR_LEN];       /* Manufacturer description. */
    char hw_desc[DESC_STR_LEN];        /* Hardware description. */
    char sw_desc[DESC_STR_LEN];        /* Software description. */
    char serial_num[SERIAL_NUM_LEN];   /* Serial number. */
    char dp_desc[DESC_STR_LEN];        /* Human readable description of datapath. */
};
OF12P_ASSERT(sizeof(struct of12p_desc_stats) == 1056);

/* Body for of12p_stats_request of type OF12PST_FLOW. */
struct of12p_flow_stats_request {
    uint8_t table_id;         /* ID of table to read (from of12p_table_stats),
                                 0xff for all tables. */
    uint8_t pad[3];           /* Align to 64 bits. */
    uint32_t out_port;        /* Require matching entries to include this
                                 as an output port.  A value of OF12PP_ANY
                                 indicates no restriction. */
    uint32_t out_group;       /* Require matching entries to include this
                                 as an output group.  A value of OF12PG_ANY
                                 indicates no restriction. */
    uint8_t pad2[4];          /* Align to 64 bits. */
    uint64_t cookie;          /* Require matching entries to contain this
                                 cookie value */
    uint64_t cookie_mask;     /* Mask used to restrict the cookie bits that
                                 must match. A value of 0 indicates
                                 no restriction. */
    struct of12p_match match;   /* Fields to match. */
};
OF12P_ASSERT(sizeof(struct of12p_flow_stats_request) == 40);

/* Body of reply to OF12PST_FLOW request. */
struct of12p_flow_stats {
    uint16_t length;          /* Length of this entry. */
    uint8_t table_id;         /* ID of table flow came from. */
    uint8_t pad;
    uint32_t duration_sec;    /* Time flow has been alive in seconds. */
    uint32_t duration_nsec;   /* Time flow has been alive in nanoseconds beyond
                                 duration_sec. */
    uint16_t priority;        /* Priority of the entry. Only meaningful
                                 when this is not an exact-match entry. */
    uint16_t idle_timeout;    /* Number of seconds idle before expiration. */
    uint16_t hard_timeout;    /* Number of seconds before expiration. */
    uint8_t pad2[6];          /* Align to 64-bits. */
    uint64_t cookie;          /* Opaque controller-issued identifier. */
    uint64_t packet_count;    /* Number of packets in flow. */
    uint64_t byte_count;      /* Number of bytes in flow. */
    struct of12p_match match;   /* Description of fields. */
    //struct of12p_instruction instructions[0]; /* Instruction set. */
};
OF12P_ASSERT(sizeof(struct of12p_flow_stats) == 56);

/* Body for of12p_stats_request of type OF12PST_AGGREGATE. */
struct of12p_aggregate_stats_request {
    uint8_t table_id;         /* ID of table to read (from of12p_table_stats)
                                 0xff for all tables. */
    uint8_t pad[3];           /* Align to 64 bits. */
    uint32_t out_port;        /* Require matching entries to include this
                                 as an output port.  A value of OF12PP_ANY
                                 indicates no restriction. */
    uint32_t out_group;       /* Require matching entries to include this
                                 as an output group.  A value of OF12PG_ANY
                                 indicates no restriction. */
    uint8_t pad2[4];          /* Align to 64 bits. */
    uint64_t cookie;          /* Require matching entries to contain this
                                 cookie value */
    uint64_t cookie_mask;     /* Mask used to restrict the cookie bits that
                                 must match. A value of 0 indicates
                                 no restriction. */
    struct of12p_match match;   /* Fields to match. */
};
OF12P_ASSERT(sizeof(struct of12p_aggregate_stats_request) == 40);

/* Body of reply to OF12PST_AGGREGATE request. */
struct of12p_aggregate_stats_reply {
    uint64_t packet_count;    /* Number of packets in flows. */
    uint64_t byte_count;      /* Number of bytes in flows. */
    uint32_t flow_count;      /* Number of flows. */
    uint8_t pad[4];           /* Align to 64 bits. */
};
OF12P_ASSERT(sizeof(struct of12p_aggregate_stats_reply) == 24);

/* Flow match fields. */
enum of12p_flow_match_fields {
    OF12PFMF_IN_PORT     = 1 << 0,  /* Switch input port. */
    OF12PFMF_DL_VLAN     = 1 << 1,  /* VLAN id. */
    OF12PFMF_DL_VLAN_PCP = 1 << 2,  /* VLAN priority. */
    OF12PFMF_DL_TYPE     = 1 << 3,  /* Ethernet frame type. */
    OF12PFMF_NW_TOS      = 1 << 4,  /* IP ToS (DSCP field, 6 bits). */
    OF12PFMF_NW_PROTO    = 1 << 5,  /* IP protocol. */
    OF12PFMF_TP_SRC      = 1 << 6,  /* TCP/UDP/SCTP source port. */
    OF12PFMF_TP_DST      = 1 << 7,  /* TCP/UDP/SCTP destination port. */
    OF12PFMF_MPLS_LABEL  = 1 << 8,  /* MPLS label. */
    OF12PFMF_MPLS_TC     = 1 << 9,  /* MPLS TC. */
    OF12PFMF_TYPE        = 1 << 10, /* Match type. */
    OF12PFMF_DL_SRC      = 1 << 11, /* Ethernet source address. */
    OF12PFMF_DL_DST      = 1 << 12, /* Ethernet destination address. */
    OF12PFMF_NW_SRC      = 1 << 13, /* IP source address. */
    OF12PFMF_NW_DST      = 1 << 14, /* IP destination address. */
    OF12PFMF_METADATA    = 1 << 15, /* Metadata passed between tables. */
#ifdef ORAN
    OF12PFMF_PPPOE_CODE  = 1 << 16, /* PPPoE code */
    OF12PFMF_PPPOE_TYPE  = 1 << 17, /* PPPoE type */
    OF12PFMF_PPPOE_SESS  = 1 << 18, /* PPPoE session */
    OF12PFMF_PPP_PROT	   = 1 << 19, /* PPP code */
#endif
};

/* Body of reply to OF12PST_TABLE request. */
struct of12p_table_stats {
    uint8_t table_id;        /* Identifier of table.  Lower numbered tables
                                are consulted first. */
    uint8_t pad[7];          /* Align to 64-bits. */
    char name[OF12P_MAX_TABLE_NAME_LEN];
    uint64_t match;			 /* Bitmap of (1 << OF12PXMT_*) that indicate the
								fields the table can match on. */
    uint64_t wildcards;      /* Bitmap of (1 << OF12PXMT_*) wildcards that are
								supported by the table. */
    uint32_t write_actions;  /* Bitmap of OF12PAT_* that are supported
                                by the table with OF12PIT_WRITE_ACTIONS. */
    uint32_t apply_actions;  /* Bitmap of OF12PAT_* that are supported
                                by the table with OF12PIT_APPLY_ACTIONS. */
    uint64_t write_setfields;/* Bitmap of (1 << OF12PXMT_*) header fields that
								can be set with OF12PIT_WRITE_ACTIONS. */
    uint64_t apply_setfields;/* Bitmap of (1 << OF12PXMT_*) header fields that
								can be set with OF12PIT_APPLY_ACTIONS. */
    uint64_t metadata_match; /* Bits of metadata table can match. */
    uint64_t metadata_write; /* Bits of metadata table can write. */
    uint32_t instructions;	 /* Bitmap of OF12PIT_* values supported. */
    uint32_t config;         /* Bitmap of OF12PTC_* values */
    uint32_t max_entries;    /* Max number of entries supported. */
    uint32_t active_count;   /* Number of active entries. */
    uint64_t lookup_count;   /* Number of packets looked up in table. */
    uint64_t matched_count;  /* Number of packets that hit table. */
};
OF12P_ASSERT(sizeof(struct of12p_table_stats) == 128);

/* Body for of12p_stats_request of type OF12PST_PORT. */
struct of12p_port_stats_request {
    uint32_t port_no;        /* OF12PST_PORT message must request statistics
                              * either for a single port (specified in
                              * port_no) or for all ports (if port_no ==
                              * OF12PP_ANY). */
    uint8_t pad[4];
};
OF12P_ASSERT(sizeof(struct of12p_port_stats_request) == 8);

/* Body of reply to OF12PST_PORT request. If a counter is unsupported, set
 * the field to all ones. */
struct of12p_port_stats {
    uint32_t port_no;
    uint8_t pad[4];          /* Align to 64-bits. */
    uint64_t rx_packets;     /* Number of received packets. */
    uint64_t tx_packets;     /* Number of transmitted packets. */
    uint64_t rx_bytes;       /* Number of received bytes. */
    uint64_t tx_bytes;       /* Number of transmitted bytes. */
    uint64_t rx_dropped;     /* Number of packets dropped by RX. */
    uint64_t tx_dropped;     /* Number of packets dropped by TX. */
    uint64_t rx_errors;      /* Number of receive errors.  This is a super-set
                                of more specific receive errors and should be
                                greater than or equal to the sum of all
                                rx_*_err values. */
    uint64_t tx_errors;      /* Number of transmit errors.  This is a super-set
                                of more specific transmit errors and should be
                                greater than or equal to the sum of all
                                tx_*_err values (none currently defined.) */
    uint64_t rx_frame_err;   /* Number of frame alignment errors. */
    uint64_t rx_over_err;    /* Number of packets with RX overrun. */
    uint64_t rx_crc_err;     /* Number of CRC errors. */
    uint64_t collisions;     /* Number of collisions. */
};
OF12P_ASSERT(sizeof(struct of12p_port_stats) == 104);

/* Body of OF12PST_GROUP request. */
struct of12p_group_stats_request {
    uint32_t group_id;       /* All groups if OF12PG_ALL. */
    uint8_t pad[4];          /* Align to 64 bits. */
};
OF12P_ASSERT(sizeof(struct of12p_group_stats_request) == 8);

/* Used in group stats replies. */
struct of12p_bucket_counter {
    uint64_t packet_count;   /* Number of packets processed by bucket. */
    uint64_t byte_count;     /* Number of bytes processed by bucket. */
};
OF12P_ASSERT(sizeof(struct of12p_bucket_counter) == 16);

/* Body of reply to OF12PST_GROUP request. */
struct of12p_group_stats {
    uint16_t length;         /* Length of this entry. */
    uint8_t pad[2];          /* Align to 64 bits. */
    uint32_t group_id;       /* Group identifier. */
    uint32_t ref_count;      /* Number of flows or groups that directly forward
                                to this group. */
    uint8_t pad2[4];         /* Align to 64 bits. */
    uint64_t packet_count;   /* Number of packets processed by group. */
    uint64_t byte_count;     /* Number of bytes processed by group. */
    struct of12p_bucket_counter bucket_stats[0];
};
OF12P_ASSERT(sizeof(struct of12p_group_stats) == 32);

/* Body of reply to OF12PST_GROUP_DESC request. */
struct of12p_group_desc_stats {
    uint16_t length;              /* Length of this entry. */
    uint8_t type;                 /* One of OF12PGT_*. */
    uint8_t pad;                  /* Pad to 64 bits. */
    uint32_t group_id;            /* Group identifier. */
    struct of12p_bucket buckets[0];
};
OF12P_ASSERT(sizeof(struct of12p_group_desc_stats) == 8);

/* Body of reply to OF12PST_GROUP_FEATURES request. Group features. */
struct of12p_group_features_stats {
	uint32_t types;				/* Bitmap of OF12PGT_* values supported. */
	uint32_t capabilities;		/* Bitmap of OF12PGFC_* capability supported. */
	uint32_t max_groups[4];		/* Maximum number of groups for each type. */
	uint32_t actions[4];		/* Bitmaps of OF12PAT_* that are supported. */
};
OF12P_ASSERT(sizeof(struct of12p_group_features_stats) == 40);

/* Group configuration flags */
enum of12p_group_capabilities {
	OF12PGFC_SELECT_WEIGHT 	= 1 << 0,	/* Support weight for select groups */
	OF12PGFC_SELECT_LIVENESS 	= 1 << 1,	/* Support liveness for select groups */
	OF12PGFC_CHAINING 		= 1 << 2,	/* Support chaining groups */
	OF12PGFC_CHAINING_CHECKS 	= 1 << 3,	/* Check chaining for loops and delete */
};


/* Experimenter extension. */
struct of12p_experimenter_header {
    struct of12p_header header;   /* Type OF12PT_EXPERIMENTER. */
    uint32_t experimenter;      /* Experimenter ID:
                                 * - MSB 0: low-order bytes are IEEE OUI.
                                 * - MSB != 0: defined by OpenFlow
                                 *   consortium. */
    uint32_t exp_type;			/* Experimenter defined. */
    /* Experimenter-defined arbitrary additional data. */
};
OF12P_ASSERT(sizeof(struct of12p_experimenter_header) == 16);

/* All ones is used to indicate all queues in a port (for stats retrieval). */
#define OF12PQ_ALL      0xffffffff

/* Min rate > 1000 means not configured. */
#define OF12PQ_MIN_RATE_UNCFG      0xffff

enum of12p_queue_properties {
    OF12PQT_NONE = 0,       /* No property defined for queue (default). */
    OF12PQT_MIN_RATE,       /* Minimum datarate guaranteed. */
                          /* Other types should be added here
                           * (i.e. max rate, precedence, etc). */
};

/* Common description for a queue. */
struct of12p_queue_prop_header {
    uint16_t property;    /* One of OF12PQT_. */
    uint16_t len;         /* Length of property, including this header. */
    uint8_t pad[4];       /* 64-bit alignemnt. */
};
OF12P_ASSERT(sizeof(struct of12p_queue_prop_header) == 8);

/* Min-Rate queue property description. */
struct of12p_queue_prop_min_rate {
    struct of12p_queue_prop_header prop_header; /* prop: OF12PQT_MIN, len: 16. */
    uint16_t rate;        /* In 1/10 of a percent; >1000 -> disabled. */
    uint8_t pad[6];       /* 64-bit alignment */
};
OF12P_ASSERT(sizeof(struct of12p_queue_prop_min_rate) == 16);

/* Full description for a queue. */
struct of12p_packet_queue {
    uint32_t queue_id;     /* id for the specific queue. */
    uint16_t len;          /* Length in bytes of this queue desc. */
    uint8_t pad[2];        /* 64-bit alignment. */
    struct of12p_queue_prop_header properties[0]; /* List of properties. */
};
OF12P_ASSERT(sizeof(struct of12p_packet_queue) == 8);

/* Query for port queue configuration. */
struct of12p_queue_get_config_request {
    struct of12p_header header;
    uint32_t port;         /* Port to be queried. Should refer
                              to a valid physical port (i.e. < OF12PP_MAX) */
    uint8_t pad[4];
};
OF12P_ASSERT(sizeof(struct of12p_queue_get_config_request) == 16);

/* Queue configuration for a given port. */
struct of12p_queue_get_config_reply {
    struct of12p_header header;
    uint32_t port;
    uint8_t pad[4];
    struct of12p_packet_queue queues[0]; /* List of configured queues. */
};
OF12P_ASSERT(sizeof(struct of12p_queue_get_config_reply) == 16);

/* OF12PAT_SET_QUEUE action struct: send packets to given queue on port. */
struct of12p_action_set_queue {
    uint16_t type;            /* OF12PAT_SET_QUEUE. */
    uint16_t len;             /* Len is 8. */
    uint32_t queue_id;        /* Queue id for the packets. */
};
OF12P_ASSERT(sizeof(struct of12p_action_set_queue) == 8);

struct of12p_queue_stats_request {
    uint32_t port_no;        /* All ports if OF12PP_ANY. */
    uint32_t queue_id;       /* All queues if OF12PQ_ALL. */
};
OF12P_ASSERT(sizeof(struct of12p_queue_stats_request) == 8);

struct of12p_queue_stats {
    uint32_t port_no;
    uint32_t queue_id;       /* Queue i.d */
    uint64_t tx_bytes;       /* Number of transmitted bytes. */
    uint64_t tx_packets;     /* Number of transmitted packets. */
    uint64_t tx_errors;      /* Number of packets dropped due to overrun. */
};
OF12P_ASSERT(sizeof(struct of12p_queue_stats) == 32);



/* Role request and reply message. */
struct of12p_role_request {
	struct of12p_header header; 	/* Type OF12PT_ROLE_REQUEST/OF12PT_ROLE_REPLY. */
	uint32_t role;				/* One of NX_ROLE_*. */
	uint8_t pad[4];				/* Align to 64 bits. */
	uint64_t generation_id;		/* Master Election Generation Id */
};
OF12P_ASSERT(sizeof(struct of12p_role_request) == 24);

/* Controller roles. */
enum of12p_controller_role {
	OF12PCR_ROLE_NOCHANGE = 0,	/* Don’t change current role. */
	OF12PCR_ROLE_EQUAL = 1,		/* Default role, full access. */
	OF12PCR_ROLE_MASTER = 2,		/* Full access, at most one master. */
	OF12PCR_ROLE_SLAVE = 3,		/* Read-only access. */
};


#endif /* openflow/openflow.h */
