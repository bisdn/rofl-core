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

#ifndef OPENFLOW_OPENFLOW10_H
#define OPENFLOW_OPENFLOW10_H 1

#include "openflow_common.h"

/* Version number:
 * Non-experimental versions released: 0x01
 * Experimental versions released: 0x81 -- 0x99
 */
/* The most significant bit being set in the version field indicates an
 * experimental OpenFlow version.
 */
#define OFP10_VERSION   0x01


/* Port numbering.  Physical ports are numbered starting from 1. */
enum ofp10_port {
    /* Maximum number of physical switch ports. */
    OFP10P_MAX = 0xff00,

    /* Fake output "ports". */
    OFP10P_IN_PORT    = 0xfff8,  /* Send the packet out the input port.  This
                                  virtual port must be explicitly used
                                  in order to send back out of the input
                                  port. */
    OFP10P_TABLE      = 0xfff9,  /* Perform actions in flow table.
                                  NB: This can only be the destination
                                  port for packet-out messages. */
    OFP10P_NORMAL     = 0xfffa,  /* Process with normal L2/L3 switching. */
    OFP10P_FLOOD      = 0xfffb,  /* All physical ports except input port and
                                  those disabled by STP. */
    OFP10P_ALL        = 0xfffc,  /* All physical ports except input port. */
    OFP10P_CONTROLLER = 0xfffd,  /* Send to controller. */
    OFP10P_LOCAL      = 0xfffe,  /* Local openflow "port". */
    OFP10P_NONE       = 0xffff   /* Not associated with a physical port. */
};


/* OFP10T_HELLO.  This message has an empty body, but implementations must
 * ignore any data included in the body, to allow for future extensions. */
struct ofp10_hello {
    struct ofp_header header;
};

#define OFP10_DEFAULT_MISS_SEND_LEN   128

enum ofp10_config_flags {
    /* Handling of IP fragments. */
    OFP10C_FRAG_NORMAL   = 0,  /* No special handling for fragments. */
    OFP10C_FRAG_DROP     = 1,  /* Drop fragments. */
    OFP10C_FRAG_REASM    = 2,  /* Reassemble (only if OFP10C_IP_REASM set). */
    OFP10C_FRAG_MASK     = 3
};

/* Switch configuration. */
struct ofp10_switch_config {
    struct ofp_header header;
    uint16_t flags;             /* OFP10C_* flags. */
    uint16_t miss_send_len;     /* Max bytes of new flow that datapath should
                                   send to the controller. */
};
OFP_ASSERT(sizeof(struct ofp10_switch_config) == 12);

/* Capabilities supported by the datapath. */
enum ofp10_capabilities {
    OFP10C_FLOW_STATS     = 1 << 0,  /* Flow statistics. */
    OFP10C_TABLE_STATS    = 1 << 1,  /* Table statistics. */
    OFP10C_PORT_STATS     = 1 << 2,  /* Port statistics. */
    OFP10C_STP            = 1 << 3,  /* 802.1d spanning tree. */
    OFP10C_RESERVED       = 1 << 4,  /* Reserved, must be zero. */
    OFP10C_IP_REASM       = 1 << 5,  /* Can reassemble IP fragments. */
    OFP10C_QUEUE_STATS    = 1 << 6,  /* Queue statistics. */
    OFP10C_ARP_MATCH_IP   = 1 << 7   /* Match IP addresses in ARP pkts. */
};

/* Flags to indicate behavior of the physical port.  These flags are
 * used in ofp10_phy_port to describe the current configuration.  They are
 * used in the ofp10_port_mod message to configure the port's behavior.
 */
enum ofp10_port_config {
    OFP10PC_PORT_DOWN    = 1 << 0,  /* Port is administratively down. */

    OFP10PC_NO_STP       = 1 << 1,  /* Disable 802.1D spanning tree on port. */
    OFP10PC_NO_RECV      = 1 << 2,  /* Drop all packets except 802.1D spanning
                                     tree packets. */
    OFP10PC_NO_RECV_STP  = 1 << 3,  /* Drop received 802.1D STP packets. */
    OFP10PC_NO_FLOOD     = 1 << 4,  /* Do not include this port when flooding. */
    OFP10PC_NO_FWD       = 1 << 5,  /* Drop packets forwarded to port. */
    OFP10PC_NO_PACKET_IN = 1 << 6   /* Do not send packet-in msgs for port. */
};

/* Current state of the physical port.  These are not configurable from
 * the controller.
 */
enum ofp10_port_state {
    OFP10PS_LINK_DOWN   = 1 << 0, /* No physical link present. */

    /* The OFP10PS_STP_* bits have no effect on switch operation.  The
     * controller must adjust OFP10PC_NO_RECV, OFP10PC_NO_FWD, and
     * OFP10PC_NO_PACKET_IN appropriately to fully implement an 802.1D spanning
     * tree. */
    OFP10PS_STP_LISTEN  = 0 << 8, /* Not learning or relaying frames. */
    OFP10PS_STP_LEARN   = 1 << 8, /* Learning but not relaying frames. */
    OFP10PS_STP_FORWARD = 2 << 8, /* Learning and relaying frames. */
    OFP10PS_STP_BLOCK   = 3 << 8, /* Not part of spanning tree. */
    OFP10PS_STP_MASK    = 3 << 8  /* Bit mask for OFP10PS_STP_* values. */
};

/* Features of physical ports available in a datapath. */
enum ofp10_port_features {
    OFP10PF_10MB_HD    = 1 << 0,  /* 10 Mb half-duplex rate support. */
    OFP10PF_10MB_FD    = 1 << 1,  /* 10 Mb full-duplex rate support. */
    OFP10PF_100MB_HD   = 1 << 2,  /* 100 Mb half-duplex rate support. */
    OFP10PF_100MB_FD   = 1 << 3,  /* 100 Mb full-duplex rate support. */
    OFP10PF_1GB_HD     = 1 << 4,  /* 1 Gb half-duplex rate support. */
    OFP10PF_1GB_FD     = 1 << 5,  /* 1 Gb full-duplex rate support. */
    OFP10PF_10GB_FD    = 1 << 6,  /* 10 Gb full-duplex rate support. */
    OFP10PF_COPPER     = 1 << 7,  /* Copper medium. */
    OFP10PF_FIBER      = 1 << 8,  /* Fiber medium. */
    OFP10PF_AUTONEG    = 1 << 9,  /* Auto-negotiation. */
    OFP10PF_PAUSE      = 1 << 10, /* Pause. */
    OFP10PF_PAUSE_ASYM = 1 << 11  /* Asymmetric pause. */
};

/* Description of a physical port */
struct ofp10_phy_port {
    uint16_t port_no;
    uint8_t hw_addr[OFP_ETH_ALEN];
    char name[OFP_MAX_PORT_NAME_LEN]; /* Null-terminated */

    uint32_t config;        /* Bitmap of OFP10PC_* flags. */
    uint32_t state;         /* Bitmap of OFP10PS_* flags. */

    /* Bitmaps of OFP10PF_* that describe features.  All bits zeroed if
     * unsupported or unavailable. */
    uint32_t curr;          /* Current features. */
    uint32_t advertised;    /* Features being advertised by the port. */
    uint32_t supported;     /* Features supported by the port. */
    uint32_t peer;          /* Features advertised by peer. */
};
OFP_ASSERT(sizeof(struct ofp10_phy_port) == 48);

/* Switch features. */
struct ofp10_switch_features {
    struct ofp_header header;
    uint64_t datapath_id;   /* Datapath unique ID.  The lower 48-bits are for
                               a MAC address, while the upper 16-bits are
                               implementer-defined. */

    uint32_t n_buffers;     /* Max packets buffered at once. */

    uint8_t n_tables;       /* Number of tables supported by datapath. */
    uint8_t pad[3];         /* Align to 64-bits. */

    /* Features. */
    uint32_t capabilities;  /* Bitmap of support "ofp10_capabilities". */
    uint32_t actions;       /* Bitmap of supported "ofp10_action_type"s. */

    /* Port info.*/
    struct ofp10_phy_port ports[0];  /* Port definitions.  The number of ports
                                      is inferred from the length field in
                                      the header. */
};
OFP_ASSERT(sizeof(struct ofp10_switch_features) == 32);

/* What changed about the physical port */
enum ofp10_port_reason {
    OFP10PR_ADD,              /* The port was added. */
    OFP10PR_DELETE,           /* The port was removed. */
    OFP10PR_MODIFY            /* Some attribute of the port has changed. */
};

/* A physical port has changed in the datapath */
struct ofp10_port_status {
    struct ofp_header header;
    uint8_t reason;          /* One of OFP10PR_*. */
    uint8_t pad[7];          /* Align to 64-bits. */
    struct ofp10_phy_port desc;
};
OFP_ASSERT(sizeof(struct ofp10_port_status) == 64);

/* Modify behavior of the physical port */
struct ofp10_port_mod {
    struct ofp_header header;
    uint16_t port_no;
    uint8_t hw_addr[OFP_ETH_ALEN]; /* The hardware address is not
                                      configurable.  This is used to
                                      sanity-check the request, so it must
                                      be the same as returned in an
                                      ofp10_phy_port struct. */

    uint32_t config;        /* Bitmap of OFP10PC_* flags. */
    uint32_t mask;          /* Bitmap of OFP10PC_* flags to be changed. */

    uint32_t advertise;     /* Bitmap of "ofp10_port_features"s.  Zero all
                               bits to prevent any action taking place. */
    uint8_t pad[4];         /* Pad to 64-bits. */
};
OFP_ASSERT(sizeof(struct ofp10_port_mod) == 32);

/* Why is this packet being sent to the controller? */
enum ofp10_packet_in_reason {
    OFP10R_NO_MATCH,          /* No matching flow. */
    OFP10R_ACTION             /* Action explicitly output to controller. */
};

/* Packet received on port (datapath -> controller). */
struct ofp10_packet_in {
    struct ofp_header header;
    uint32_t buffer_id;     /* ID assigned by datapath. */
    uint16_t total_len;     /* Full length of frame. */
    uint16_t in_port;       /* Port on which frame was received. */
    uint8_t reason;         /* Reason packet is being sent (one of OFP10R_*) */
    uint8_t pad;
    uint8_t data[0];        /* Ethernet frame, halfway through 32-bit word,
                               so the IP header is 32-bit aligned.  The
                               amount of data is inferred from the length
                               field in the header.  Because of padding,
                               offsetof(struct ofp10_packet_in, data) ==
                               sizeof(struct ofp10_packet_in) - 2. */
};
OFP_ASSERT(sizeof(struct ofp10_packet_in) == 20);

enum ofp10_action_type {
    OFP10AT_OUTPUT,           /* Output to switch port. */
    OFP10AT_SET_VLAN_VID,     /* Set the 802.1q VLAN id. */
    OFP10AT_SET_VLAN_PCP,     /* Set the 802.1q priority. */
    OFP10AT_STRIP_VLAN,       /* Strip the 802.1q header. */
    OFP10AT_SET_DL_SRC,       /* Ethernet source address. */
    OFP10AT_SET_DL_DST,       /* Ethernet destination address. */
    OFP10AT_SET_NW_SRC,       /* IP source address. */
    OFP10AT_SET_NW_DST,       /* IP destination address. */
    OFP10AT_SET_NW_TOS,       /* IP ToS (DSCP field, 6 bits). */
    OFP10AT_SET_TP_SRC,       /* TCP/UDP source port. */
    OFP10AT_SET_TP_DST,       /* TCP/UDP destination port. */
    OFP10AT_ENQUEUE,          /* Output to queue.  */
    OFP10AT_VENDOR = 0xffff
};

/* Action structure for OFP10AT_OUTPUT, which sends packets out 'port'.
 * When the 'port' is the OFP10P_CONTROLLER, 'max_len' indicates the max
 * number of bytes to send.  A 'max_len' of zero means no bytes of the
 * packet should be sent.*/
struct ofp10_action_output {
    uint16_t type;                  /* OFP10AT_OUTPUT. */
    uint16_t len;                   /* Length is 8. */
    uint16_t port;                  /* Output port. */
    uint16_t max_len;               /* Max length to send to controller. */
};
OFP_ASSERT(sizeof(struct ofp10_action_output) == 8);

/* The VLAN id is 12 bits, so we can use the entire 16 bits to indicate
 * special conditions.  All ones is used to match that no VLAN id was
 * set. */
#define OFP10_VLAN_NONE      0xffff

/* Action structure for OFP10AT_SET_VLAN_VID. */
struct ofp10_action_vlan_vid {
    uint16_t type;                  /* OFP10AT_SET_VLAN_VID. */
    uint16_t len;                   /* Length is 8. */
    uint16_t vlan_vid;              /* VLAN id. */
    uint8_t pad[2];
};
OFP_ASSERT(sizeof(struct ofp10_action_vlan_vid) == 8);

/* Action structure for OFP10AT_SET_VLAN_PCP. */
struct ofp10_action_vlan_pcp {
    uint16_t type;                  /* OFP10AT_SET_VLAN_PCP. */
    uint16_t len;                   /* Length is 8. */
    uint8_t vlan_pcp;               /* VLAN priority. */
    uint8_t pad[3];
};
OFP_ASSERT(sizeof(struct ofp10_action_vlan_pcp) == 8);

/* Action structure for OFP10AT_SET_DL_SRC/DST. */
struct ofp10_action_dl_addr {
    uint16_t type;                  /* OFP10AT_SET_DL_SRC/DST. */
    uint16_t len;                   /* Length is 16. */
    uint8_t dl_addr[OFP_ETH_ALEN];  /* Ethernet address. */
    uint8_t pad[6];
};
OFP_ASSERT(sizeof(struct ofp10_action_dl_addr) == 16);

/* Action structure for OFP10AT_SET_NW_SRC/DST. */
struct ofp10_action_nw_addr {
    uint16_t type;                  /* OFP10AT_SET_TW_SRC/DST. */
    uint16_t len;                   /* Length is 8. */
    uint32_t nw_addr;               /* IP address. */
};
OFP_ASSERT(sizeof(struct ofp10_action_nw_addr) == 8);

/* Action structure for OFP10AT_SET_TP_SRC/DST. */
struct ofp10_action_tp_port {
    uint16_t type;                  /* OFP10AT_SET_TP_SRC/DST. */
    uint16_t len;                   /* Length is 8. */
    uint16_t tp_port;               /* TCP/UDP port. */
    uint8_t pad[2];
};
OFP_ASSERT(sizeof(struct ofp10_action_tp_port) == 8);

/* Action structure for OFP10AT_SET_NW_TOS. */
struct ofp10_action_nw_tos {
    uint16_t type;                  /* OFP10AT_SET_TW_SRC/DST. */
    uint16_t len;                   /* Length is 8. */
    uint8_t nw_tos;                 /* IP ToS (DSCP field, 6 bits). */
    uint8_t pad[3];
};
OFP_ASSERT(sizeof(struct ofp10_action_nw_tos) == 8);

/* Action header for OFP10AT_VENDOR. The rest of the body is vendor-defined. */
struct ofp10_action_vendor_header {
    uint16_t type;                  /* OFP10AT_VENDOR. */
    uint16_t len;                   /* Length is a multiple of 8. */
    uint32_t vendor;                /* Vendor ID, which takes the same form
                                       as in "struct ofp10_vendor_header". */
};
OFP_ASSERT(sizeof(struct ofp10_action_vendor_header) == 8);

/* Action header that is common to all actions.  The length includes the
 * header and any padding used to make the action 64-bit aligned.
 * NB: The length of an action *must* always be a multiple of eight. */
struct ofp10_action_header {
    uint16_t type;                  /* One of OFP10AT_*. */
    uint16_t len;                   /* Length of action, including this
                                       header.  This is the length of action,
                                       including any padding to make it
                                       64-bit aligned. */
    uint8_t pad[4];
};
OFP_ASSERT(sizeof(struct ofp10_action_header) == 8);

/* Send packet (controller -> datapath). */
struct ofp10_packet_out {
    struct ofp_header header;
    uint32_t buffer_id;           /* ID assigned by datapath (-1 if none). */
    uint16_t in_port;             /* Packet's input port (OFP10P_NONE if none). */
    uint16_t actions_len;         /* Size of action array in bytes. */
    struct ofp10_action_header actions[0]; /* Actions. */
    /* uint8_t data[0]; */        /* Packet data.  The length is inferred
                                     from the length field in the header.
                                     (Only meaningful if buffer_id == -1.) */
};
OFP_ASSERT(sizeof(struct ofp10_packet_out) == 16);

enum ofp10_flow_mod_command {
    OFP10FC_ADD,              /* New flow. */
    OFP10FC_MODIFY,           /* Modify all matching flows. */
    OFP10FC_MODIFY_STRICT,    /* Modify entry strictly matching wildcards */
    OFP10FC_DELETE,           /* Delete all matching flows. */
    OFP10FC_DELETE_STRICT    /* Strictly match wildcards and priority. */
};

/* Flow wildcards. */
enum ofp10_flow_wildcards {
    OFP10FW_IN_PORT  = 1 << 0,  /* Switch input port. */
    OFP10FW_DL_VLAN  = 1 << 1,  /* VLAN id. */
    OFP10FW_DL_SRC   = 1 << 2,  /* Ethernet source address. */
    OFP10FW_DL_DST   = 1 << 3,  /* Ethernet destination address. */
    OFP10FW_DL_TYPE  = 1 << 4,  /* Ethernet frame type. */
    OFP10FW_NW_PROTO = 1 << 5,  /* IP protocol. */
    OFP10FW_TP_SRC   = 1 << 6,  /* TCP/UDP source port. */
    OFP10FW_TP_DST   = 1 << 7,  /* TCP/UDP destination port. */

    /* IP source address wildcard bit count.  0 is exact match, 1 ignores the
     * LSB, 2 ignores the 2 least-significant bits, ..., 32 and higher wildcard
     * the entire field.  This is the *opposite* of the usual convention where
     * e.g. /24 indicates that 8 bits (not 24 bits) are wildcarded. */
    OFP10FW_NW_SRC_SHIFT = 8,
    OFP10FW_NW_SRC_BITS = 6,
    OFP10FW_NW_SRC_MASK = ((1 << OFP10FW_NW_SRC_BITS) - 1) << OFP10FW_NW_SRC_SHIFT,
    OFP10FW_NW_SRC_ALL = 32 << OFP10FW_NW_SRC_SHIFT,

    /* IP destination address wildcard bit count.  Same format as source. */
    OFP10FW_NW_DST_SHIFT = 14,
    OFP10FW_NW_DST_BITS = 6,
    OFP10FW_NW_DST_MASK = ((1 << OFP10FW_NW_DST_BITS) - 1) << OFP10FW_NW_DST_SHIFT,
    OFP10FW_NW_DST_ALL = 32 << OFP10FW_NW_DST_SHIFT,

    OFP10FW_DL_VLAN_PCP = 1 << 20,  /* VLAN priority. */
    OFP10FW_NW_TOS = 1 << 21,  /* IP ToS (DSCP field, 6 bits). */

    /* Wildcard all fields. */
    OFP10FW_ALL = ((1 << 22) - 1)
};

/* The wildcards for ICMP type and code fields use the transport source
 * and destination port fields, respectively. */
#define OFP10FW_ICMP_TYPE OFP10FW_TP_SRC
#define OFP10FW_ICMP_CODE OFP10FW_TP_DST

/* Values below this cutoff are 802.3 packets and the two bytes
 * following MAC addresses are used as a frame length.  Otherwise, the
 * two bytes are used as the Ethernet type.
 */
#define OFP10_DL_TYPE_ETH2_CUTOFF   0x0600

/* Value of dl_type to indicate that the frame does not include an
 * Ethernet type.
 */
#define OFP10_DL_TYPE_NOT_ETH_TYPE  0x05ff

/* The VLAN id is 12-bits, so we can use the entire 16 bits to indicate
 * special conditions.  All ones indicates that no VLAN id was set.
 */
#define OFP10_VLAN_NONE      0xffff

/* Fields to match against flows */
struct ofp10_match {
    uint32_t wildcards;        /* Wildcard fields. */
    uint16_t in_port;          /* Input switch port. */
    uint8_t dl_src[OFP_ETH_ALEN]; /* Ethernet source address. */
    uint8_t dl_dst[OFP_ETH_ALEN]; /* Ethernet destination address. */
    uint16_t dl_vlan;          /* Input VLAN id. */
    uint8_t dl_vlan_pcp;       /* Input VLAN priority. */
    uint8_t pad1[1];           /* Align to 64-bits */
    uint16_t dl_type;          /* Ethernet frame type. */
    uint8_t nw_tos;            /* IP ToS (actually DSCP field, 6 bits). */
    uint8_t nw_proto;          /* IP protocol or lower 8 bits of
                                * ARP opcode. */
    uint8_t pad2[2];           /* Align to 64-bits */
    uint32_t nw_src;           /* IP source address. */
    uint32_t nw_dst;           /* IP destination address. */
    uint16_t tp_src;           /* TCP/UDP source port. */
    uint16_t tp_dst;           /* TCP/UDP destination port. */
};
OFP_ASSERT(sizeof(struct ofp10_match) == 40);

/* The match fields for ICMP type and code use the transport source and
 * destination port fields, respectively. */
#define icmp_type tp_src
#define icmp_code tp_dst

/* Value used in "idle_timeout" and "hard_timeout" to indicate that the entry
 * is permanent. */
#define OFP10_FLOW_PERMANENT 0

/* By default, choose a priority in the middle. */
#define OFP10_DEFAULT_PRIORITY 0x8000

enum ofp10_flow_mod_flags {
    OFP10FF_SEND_FLOW_REM = 1 << 0,  /* Send flow removed message when flow
                                    * expires or is deleted. */
    OFP10FF_CHECK_OVERLAP = 1 << 1,  /* Check for overlapping entries first. */
    OFP10FF_EMERG         = 1 << 2   /* Remark this is for emergency. */
};

/* Flow setup and teardown (controller -> datapath). */
struct ofp10_flow_mod {
    struct ofp_header header;
    struct ofp10_match match;      /* Fields to match */
    uint64_t cookie;             /* Opaque controller-issued identifier. */

    /* Flow actions. */
    uint16_t command;             /* One of OFP10FC_*. */
    uint16_t idle_timeout;        /* Idle time before discarding (seconds). */
    uint16_t hard_timeout;        /* Max time before discarding (seconds). */
    uint16_t priority;            /* Priority level of flow entry. */
    uint32_t buffer_id;           /* Buffered packet to apply to (or -1).
                                     Not meaningful for OFP10FC_DELETE*. */
    uint16_t out_port;            /* For OFP10FC_DELETE* commands, require
                                     matching entries to include this as an
                                     output port.  A value of OFP10P_NONE
                                     indicates no restriction. */
    uint16_t flags;               /* One of OFP10FF_*. */
    struct ofp10_action_header actions[0]; /* The action length is inferred
                                            from the length field in the
                                            header. */
};
OFP_ASSERT(sizeof(struct ofp10_flow_mod) == 72);

/* Why was this flow removed? */
enum ofp10_flow_removed_reason {
    OFP10RR_IDLE_TIMEOUT,         /* Flow idle time exceeded idle_timeout. */
    OFP10RR_HARD_TIMEOUT,         /* Time exceeded hard_timeout. */
    OFP10RR_DELETE                /* Evicted by a DELETE flow mod. */
};

/* Flow removed (datapath -> controller). */
struct ofp10_flow_removed {
    struct ofp_header header;
    struct ofp10_match match;   /* Description of fields. */
    uint64_t cookie;          /* Opaque controller-issued identifier. */

    uint16_t priority;        /* Priority level of flow entry. */
    uint8_t reason;           /* One of OFP10RR_*. */
    uint8_t pad[1];           /* Align to 32-bits. */

    uint32_t duration_sec;    /* Time flow was alive in seconds. */
    uint32_t duration_nsec;   /* Time flow was alive in nanoseconds beyond
                                 duration_sec. */
    uint16_t idle_timeout;    /* Idle timeout from original flow mod. */
    uint8_t pad2[2];          /* Align to 64-bits. */
    uint64_t packet_count;
    uint64_t byte_count;
};
OFP_ASSERT(sizeof(struct ofp10_flow_removed) == 88);

/* Values for 'type' in ofp10_error_message.  These values are immutable: they
 * will not change in future versions of the protocol (although new values may
 * be added). */
enum ofp10_error_type {
    OFP10ET_HELLO_FAILED,         /* Hello protocol failed. */
    OFP10ET_BAD_REQUEST,          /* Request was not understood. */
    OFP10ET_BAD_ACTION,           /* Error in action description. */
    OFP10ET_FLOW_MOD_FAILED,      /* Problem modifying flow entry. */
    OFP10ET_PORT_MOD_FAILED,      /* Port mod request failed. */
    OFP10ET_QUEUE_OP_FAILED       /* Queue operation failed. */
};

/* ofp10_error_msg 'code' values for OFP10ET_HELLO_FAILED.  'data' contains an
 * ASCII text string that may give failure details. */
enum ofp10_hello_failed_code {
    OFP10HFC_INCOMPATIBLE,        /* No compatible version. */
    OFP10HFC_EPERM                /* Permissions error. */
};

/* ofp10_error_msg 'code' values for OFP10ET_BAD_REQUEST.  'data' contains at least
 * the first 64 bytes of the failed request. */
enum ofp10_bad_request_code {
    OFP10BRC_BAD_VERSION,         /* ofp_header.version not supported. */
    OFP10BRC_BAD_TYPE,            /* ofp_header.type not supported. */
    OFP10BRC_BAD_STAT,            /* ofp10_stats_request.type not supported. */
    OFP10BRC_BAD_VENDOR,          /* Vendor not supported (in ofp10_vendor_header
                                 * or ofp10_stats_request or ofp10_stats_reply). */
    OFP10BRC_BAD_SUBTYPE,         /* Vendor subtype not supported. */
    OFP10BRC_EPERM,               /* Permissions error. */
    OFP10BRC_BAD_LEN,             /* Wrong request length for type. */
    OFP10BRC_BUFFER_EMPTY,        /* Specified buffer has already been used. */
    OFP10BRC_BUFFER_UNKNOWN       /* Specified buffer does not exist. */
};

/* ofp10_error_msg 'code' values for OFP10ET_BAD_ACTION.  'data' contains at least
 * the first 64 bytes of the failed request. */
enum ofp10_bad_action_code {
    OFP10BAC_BAD_TYPE,           /* Unknown action type. */
    OFP10BAC_BAD_LEN,            /* Length problem in actions. */
    OFP10BAC_BAD_VENDOR,         /* Unknown vendor id specified. */
    OFP10BAC_BAD_VENDOR_TYPE,    /* Unknown action type for vendor id. */
    OFP10BAC_BAD_OUT_PORT,       /* Problem validating output action. */
    OFP10BAC_BAD_ARGUMENT,       /* Bad action argument. */
    OFP10BAC_EPERM,              /* Permissions error. */
    OFP10BAC_TOO_MANY,           /* Can't handle this many actions. */
    OFP10BAC_BAD_QUEUE           /* Problem validating output queue. */
};

/* ofp10_error_msg 'code' values for OFP10ET_FLOW_MOD_FAILED.  'data' contains
 * at least the first 64 bytes of the failed request. */
enum ofp10_flow_mod_failed_code {
    OFP10FMFC_ALL_TABLES_FULL,    /* Flow not added because of full tables. */
    OFP10FMFC_OVERLAP,            /* Attempted to add overlapping flow with
                                 * CHECK_OVERLAP flag set. */
    OFP10FMFC_EPERM,              /* Permissions error. */
    OFP10FMFC_BAD_EMERG_TIMEOUT,  /* Flow not added because of non-zero idle/hard
                                 * timeout. */
    OFP10FMFC_BAD_COMMAND,        /* Unknown command. */
    OFP10FMFC_UNSUPPORTED         /* Unsupported action list - cannot process in
                                 * the order specified. */
};

/* ofp10_error_msg 'code' values for OFP10ET_PORT_MOD_FAILED.  'data' contains
 * at least the first 64 bytes of the failed request. */
enum ofp10_port_mod_failed_code {
    OFP10PMFC_BAD_PORT,            /* Specified port does not exist. */
    OFP10PMFC_BAD_HW_ADDR,         /* Specified hardware address is wrong. */
};

/* ofp10_error msg 'code' values for OFP10ET_QUEUE_OP_FAILED. 'data' contains
 * at least the first 64 bytes of the failed request */
enum ofp10_queue_op_failed_code {
    OFP10QOFC_BAD_PORT,           /* Invalid port (or port does not exist). */
    OFP10QOFC_BAD_QUEUE,          /* Queue does not exist. */
    OFP10QOFC_EPERM               /* Permissions error. */
};

/* OFP10T_ERROR: Error message (datapath -> controller). */
struct ofp10_error_msg {
    struct ofp_header header;

    uint16_t type;
    uint16_t code;
    uint8_t data[0];          /* Variable-length data.  Interpreted based
                                 on the type and code. */
};
OFP_ASSERT(sizeof(struct ofp10_error_msg) == 12);

enum ofp10_stats_types {
    /* Description of this OpenFlow switch.
     * The request body is empty.
     * The reply body is struct ofp10_desc_stats. */
    OFP10ST_DESC,

    /* Individual flow statistics.
     * The request body is struct ofp10_flow_stats_request.
     * The reply body is an array of struct ofp10_flow_stats. */
    OFP10ST_FLOW,

    /* Aggregate flow statistics.
     * The request body is struct ofp10_aggregate_stats_request.
     * The reply body is struct ofp10_aggregate_stats_reply. */
    OFP10ST_AGGREGATE,

    /* Flow table statistics.
     * The request body is empty.
     * The reply body is an array of struct ofp10_table_stats. */
    OFP10ST_TABLE,

    /* Physical port statistics.
     * The request body is struct ofp10_port_stats_request.
     * The reply body is an array of struct ofp10_port_stats. */
    OFP10ST_PORT,

    /* Queue statistics for a port
     * The request body defines the port
     * The reply body is an array of struct ofp10_queue_stats */
    OFP10ST_QUEUE,

    /* Vendor extension.
     * The request and reply bodies begin with a 32-bit vendor ID, which takes
     * the same form as in "struct ofp10_vendor_header".  The request and reply
     * bodies are otherwise vendor-defined. */
    OFP10ST_VENDOR = 0xffff
};

struct ofp10_stats_request {
    struct ofp_header header;
    uint16_t type;              /* One of the OFP10ST_* constants. */
    uint16_t flags;             /* OFP10SF_REQ_* flags (none yet defined). */
    uint8_t body[0];            /* Body of the request. */
};
OFP_ASSERT(sizeof(struct ofp10_stats_request) == 12);

enum ofp10_stats_reply_flags {
    OFP10SF_REPLY_MORE  = 1 << 0  /* More replies to follow. */
};

struct ofp10_stats_reply {
    struct ofp_header header;
    uint16_t type;              /* One of the OFP10ST_* constants. */
    uint16_t flags;             /* OFP10SF_REPLY_* flags. */
    uint8_t body[0];            /* Body of the reply. */
};
OFP_ASSERT(sizeof(struct ofp10_stats_reply) == 12);

#define DESC_STR_LEN   256
#define SERIAL_NUM_LEN 32
/* Body of reply to OFP10ST_DESC request.  Each entry is a NULL-terminated
 * ASCII string. */
struct ofp10_desc_stats {
    char mfr_desc[DESC_STR_LEN];       /* Manufacturer description. */
    char hw_desc[DESC_STR_LEN];        /* Hardware description. */
    char sw_desc[DESC_STR_LEN];        /* Software description. */
    char serial_num[SERIAL_NUM_LEN];   /* Serial number. */
    char dp_desc[DESC_STR_LEN];        /* Human readable description of datapath. */
};
OFP_ASSERT(sizeof(struct ofp10_desc_stats) == 1056);

/* Body for ofp10_stats_request of type OFP10ST_FLOW. */
struct ofp10_flow_stats_request {
    struct ofp10_match match;   /* Fields to match. */
    uint8_t table_id;         /* ID of table to read (from ofp10_table_stats),
                                 0xff for all tables or 0xfe for emergency. */
    uint8_t pad;              /* Align to 32 bits. */
    uint16_t out_port;        /* Require matching entries to include this
                                 as an output port.  A value of OFP10P_NONE
                                 indicates no restriction. */
};
OFP_ASSERT(sizeof(struct ofp10_flow_stats_request) == 44);

/* Body of reply to OFP10ST_FLOW request. */
struct ofp10_flow_stats {
    uint16_t length;          /* Length of this entry. */
    uint8_t table_id;         /* ID of table flow came from. */
    uint8_t pad;
    struct ofp10_match match;   /* Description of fields. */
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
    struct ofp10_action_header actions[0]; /* Actions. */
};
OFP_ASSERT(sizeof(struct ofp10_flow_stats) == 88);

/* Body for ofp10_stats_request of type OFP10ST_AGGREGATE. */
struct ofp10_aggregate_stats_request {
    struct ofp10_match match;   /* Fields to match. */
    uint8_t table_id;         /* ID of table to read (from ofp10_table_stats)
                                 0xff for all tables or 0xfe for emergency. */
    uint8_t pad;              /* Align to 32 bits. */
    uint16_t out_port;        /* Require matching entries to include this
                                 as an output port.  A value of OFP10P_NONE
                                 indicates no restriction. */
};
OFP_ASSERT(sizeof(struct ofp10_aggregate_stats_request) == 44);

/* Body of reply to OFP10ST_AGGREGATE request. */
struct ofp10_aggregate_stats_reply {
    uint64_t packet_count;    /* Number of packets in flows. */
    uint64_t byte_count;      /* Number of bytes in flows. */
    uint32_t flow_count;      /* Number of flows. */
    uint8_t pad[4];           /* Align to 64 bits. */
};
OFP_ASSERT(sizeof(struct ofp10_aggregate_stats_reply) == 24);

/* Body of reply to OFP10ST_TABLE request. */
struct ofp10_table_stats {
    uint8_t table_id;        /* Identifier of table.  Lower numbered tables
                                are consulted first. */
    uint8_t pad[3];          /* Align to 32-bits. */
    char name[OFP_MAX_TABLE_NAME_LEN];
    uint32_t wildcards;      /* Bitmap of OFP10FW_* wildcards that are
                                supported by the table. */
    uint32_t max_entries;    /* Max number of entries supported. */
    uint32_t active_count;   /* Number of active entries. */
    uint64_t lookup_count;   /* Number of packets looked up in table. */
    uint64_t matched_count;  /* Number of packets that hit table. */
};
OFP_ASSERT(sizeof(struct ofp10_table_stats) == 64);

/* Body for ofp10_stats_request of type OFP10ST_PORT. */
struct ofp10_port_stats_request {
    uint16_t port_no;        /* OFP10ST_PORT message must request statistics
                              * either for a single port (specified in
                              * port_no) or for all ports (if port_no ==
                              * OFP10P_NONE). */
    uint8_t pad[6];
};
OFP_ASSERT(sizeof(struct ofp10_port_stats_request) == 8);

/* Body of reply to OFP10ST_PORT request. If a counter is unsupported, set
 * the field to all ones. */
struct ofp10_port_stats {
    uint16_t port_no;
    uint8_t pad[6];          /* Align to 64-bits. */
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
OFP_ASSERT(sizeof(struct ofp10_port_stats) == 104);

/* Vendor extension. */
struct ofp10_vendor_header {
    struct ofp_header header;   /* Type OFP10T_VENDOR. */
    uint32_t vendor;            /* Vendor ID:
                                 * - MSB 0: low-order bytes are IEEE OUI.
                                 * - MSB != 0: defined by OpenFlow
                                 *   consortium. */
    /* Vendor-defined arbitrary additional data. */
};
OFP_ASSERT(sizeof(struct ofp10_vendor_header) == 12);

/* All ones is used to indicate all queues in a port (for stats retrieval). */
#define OFP10Q_ALL      0xffffffff

/* Min rate > 1000 means not configured. */
#define OFP10Q_MIN_RATE_UNCFG      0xffff

enum ofp10_queue_properties {
    OFP10QT_NONE = 0,       /* No property defined for queue (default). */
    OFP10QT_MIN_RATE,       /* Minimum datarate guaranteed. */
                          /* Other types should be added here
                           * (i.e. max rate, precedence, etc). */
};

/* Common description for a queue. */
struct ofp10_queue_prop_header {
    uint16_t property;    /* One of OFP10QT_. */
    uint16_t len;         /* Length of property, including this header. */
    uint8_t pad[4];       /* 64-bit alignemnt. */
};
OFP_ASSERT(sizeof(struct ofp10_queue_prop_header) == 8);

/* Min-Rate queue property description. */
struct ofp10_queue_prop_min_rate {
    struct ofp10_queue_prop_header prop_header; /* prop: OFP10QT_MIN, len: 16. */
    uint16_t rate;        /* In 1/10 of a percent; >1000 -> disabled. */
    uint8_t pad[6];       /* 64-bit alignment */
};
OFP_ASSERT(sizeof(struct ofp10_queue_prop_min_rate) == 16);

/* Full description for a queue. */
struct ofp10_packet_queue {
    uint32_t queue_id;     /* id for the specific queue. */
    uint16_t len;          /* Length in bytes of this queue desc. */
    uint8_t pad[2];        /* 64-bit alignment. */
    struct ofp10_queue_prop_header properties[0]; /* List of properties. */
};
OFP_ASSERT(sizeof(struct ofp10_packet_queue) == 8);

/* Query for port queue configuration. */
struct ofp10_queue_get_config_request {
    struct ofp_header header;
    uint16_t port;         /* Port to be queried. Should refer
                              to a valid physical port (i.e. < OFP10P_MAX) */
    uint8_t pad[2];        /* 32-bit alignment. */
};
OFP_ASSERT(sizeof(struct ofp10_queue_get_config_request) == 12);

/* Queue configuration for a given port. */
struct ofp10_queue_get_config_reply {
    struct ofp_header header;
    uint16_t port;
    uint8_t pad[6];
    struct ofp10_packet_queue queues[0]; /* List of configured queues. */
};
OFP_ASSERT(sizeof(struct ofp10_queue_get_config_reply) == 16);

/* OFP10AT_ENQUEUE action struct: send packets to given queue on port. */
struct ofp10_action_enqueue {
    uint16_t type;            /* OFP10AT_ENQUEUE. */
    uint16_t len;             /* Len is 16. */
    uint16_t port;            /* Port that queue belongs. Should
                                 refer to a valid physical port
                                 (i.e. < OFP10P_MAX) or OFP10P_IN_PORT. */
    uint8_t pad[6];           /* Pad for 64-bit alignment. */
    uint32_t queue_id;        /* Where to enqueue the packets. */
};
OFP_ASSERT(sizeof(struct ofp10_action_enqueue) == 16);

struct ofp10_queue_stats_request {
    uint16_t port_no;        /* All ports if OFP10T_ALL. */
    uint8_t pad[2];          /* Align to 32-bits. */
    uint32_t queue_id;       /* All queues if OFP10Q_ALL. */
};
OFP_ASSERT(sizeof(struct ofp10_queue_stats_request) == 8);

struct ofp10_queue_stats {
    uint16_t port_no;
    uint8_t pad[2];          /* Align to 32-bits. */
    uint32_t queue_id;       /* Queue i.d */
    uint64_t tx_bytes;       /* Number of transmitted bytes. */
    uint64_t tx_packets;     /* Number of transmitted packets. */
    uint64_t tx_errors;      /* Number of packets dropped due to overrun. */
};
OFP_ASSERT(sizeof(struct ofp10_queue_stats) == 32);

#endif /* openflow/openflow.h */
