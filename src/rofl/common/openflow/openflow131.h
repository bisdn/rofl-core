#include "openflow12.h"

enum ofp13_type {
    /* Immutable messages. */
    OFPT_HELLO 					= 0,    /* Symmetric message */
    OFPT_ERROR 					= 1,	/* Symmetric message */
    OFPT_ECHO_REQUEST 			= 2,	/* Symmetric message */
    OFPT_ECHO_REPLY				= 3,    /* Symmetric message */
    OFPT_EXPERIMENTER			= 4,    /* Symmetric message */

    /* Switch configuration messages. */
    OFPT_FEATURES_REQUEST		= 5,    /* Controller/switch message */
    OFPT_FEATURES_REPLY			= 6,    /* Controller/switch message */
    OFPT_GET_CONFIG_REQUEST		= 7,    /* Controller/switch message */
    OFPT_GET_CONFIG_REPLY		= 8,    /* Controller/switch message */
    OFPT_SET_CONFIG				= 9,    /* Controller/switch message */

    /* Asynchronous messages. */
    OFPT_PACKET_IN				= 10,   /* Async message */
    OFPT_FLOW_REMOVED			= 11,   /* Async message */
    OFPT_PORT_STATUS			= 12,   /* Async message */

    /* Controller command messages. */
    OFPT_PACKET_OUT				= 13,   /* Controller/switch message */
    OFPT_FLOW_MOD				= 14,   /* Controller/switch message */
    OFPT_GROUP_MOD				= 15,   /* Controller/switch message */
    OFPT_PORT_MOD				= 16,   /* Controller/switch message */
    OFPT_TABLE_MOD				= 17,   /* Controller/switch message */

    /* Multipart messages. */
    OFPT_MULTIPART_REQUEST		= 18,   /* Controller/switch message */
    OFPT_MULTIPART_REPLY		= 19,   /* Controller/switch message */

    /* Barrier messages. */
    OFPT_BARRIER_REQUEST		= 20,   /* Controller/switch message */
    OFPT_BARRIER_REPLY			= 21,   /* Controller/switch message */

    /* Queue Configuration messages. */
    OFPT_QUEUE_GET_CONFIG_REQUEST	= 22,  /* Controller/switch message */
    OFPT_QUEUE_GET_CONFIG_REPLY		= 23,  /* Controller/switch message */

    /* Controller role change request messages. */
    OFPT_ROLE_REQUEST    		= 24, /* Controller/switch message */
    OFPT_ROLE_REPLY				= 25, /* Controller/switch message */

    /* Asynchronous message configuration. */
   	OFPT_GET_ASYNC_REQUEST		= 26, /* Controller/switch message */
    OFPT_GET_ASYNC_REPLY		= 27, /* Controller/switch message */
    OFPT_SET_ASYNC				= 28, /* Controller/switch message */

    /* Meters and rate limiters configuration messages. */
    OFPT_METER_MOD				= 29, /* Controller/switch message */
};


/* Full description for a queue. */
struct ofp13_packet_queue {
	uint32_t queue_id;	/* id for the specific queue. */
	uint32_t port;		/* Port this queue is attached to. */
	uint16_t len;		/* Length in bytes of this queue desc. */
	uint8_t pad[6];		/* 64-bit alignment. */
	struct ofp_queue_prop_header properties[0]; /* List of properties. */
};
OFP_ASSERT(sizeof(struct ofp13_packet_queue) == 16);


enum ofp13_queue_properties {
	OFPQT_MIN_RATE = 1,				/* Minimum datarate guaranteed. */
	OFPQT_MAX_RATE = 2,				/* Maximum datarate. */
	OFPQT_EXPERIMENTER = 0xffff,	/* Experimenter defined property. */
};


/* Min-Rate queue property description. */
struct ofp13_queue_prop_min_rate {
    struct ofp_queue_prop_header prop_header; /* prop: OFPQT_MIN, len: 16. */
    uint16_t rate;        			/* In 1/10 of a percent; >1000 -> disabled. */
    uint8_t pad[6];       			/* 64-bit alignment */
};
OFP_ASSERT(sizeof(struct ofp13_queue_prop_min_rate) == 16);


/* Max-Rate queue property description. */
struct ofp13_queue_prop_max_rate {
	struct ofp_queue_prop_header prop_header; /* prop: OFPQT_MAX, len: 16. */
	uint16_t rate;					/* In 1/10 of a percent; >1000 -> disabled. */
	uint8_t pad[6];					/* 64-bit alignment */
};
OFP_ASSERT(sizeof(struct ofp13_queue_prop_max_rate) == 16);


/* Experimenter queue property description. */
struct ofp13_queue_prop_experimenter {
	struct ofp_queue_prop_header prop_header; /* prop: OFPQT_EXPERIMENTER, len: 16. */
	uint32_t experimenter;			/* Experimenter ID which takes the same form as in struct ofp_experimenter_header. */
	uint8_t pad[4];					/* 64-bit alignment */
	uint8_t data[0];				/* Experimenter defined data. */
};
OFP_ASSERT(sizeof(struct ofp13_queue_prop_experimenter) == 16);


/* OXM Flow match field types for OpenFlow basic class. */
enum ofp13_oxm_ofb_match_fields {
	OFPXMT_OFB_IN_PORT = 0,			/* Switch input port. */				// required
	OFPXMT_OFB_IN_PHY_PORT = 1,		/* Switch physical input port. */
	OFPXMT_OFB_METADATA = 2,		/* Metadata passed between tables. */
	OFPXMT_OFB_ETH_DST = 3,			/* Ethernet destination address. */		// required
	OFPXMT_OFB_ETH_SRC = 4,			/* Ethernet source address. */			// required
	OFPXMT_OFB_ETH_TYPE = 5,		/* Ethernet frame type. */				// required
	OFPXMT_OFB_VLAN_VID = 6,		/* VLAN id. */
	OFPXMT_OFB_VLAN_PCP = 7,		/* VLAN priority. */
	OFPXMT_OFB_IP_DSCP = 8,			/* IP DSCP (6 bits in ToS field). */
	OFPXMT_OFB_IP_ECN = 9,			/* IP ECN (2 bits in ToS field). */
	OFPXMT_OFB_IP_PROTO = 10,		/* IP protocol. */						// required
	OFPXMT_OFB_IPV4_SRC = 11,		/* IPv4 source address. */				// required
	OFPXMT_OFB_IPV4_DST = 12,		/* IPv4 destination address. */			// required
	OFPXMT_OFB_TCP_SRC = 13,		/* TCP source port. */					// required
	OFPXMT_OFB_TCP_DST = 14,		/* TCP destination port. */				// required
	OFPXMT_OFB_UDP_SRC = 15,		/* UDP source port. */					// required
	OFPXMT_OFB_UDP_DST = 16,		/* UDP destination port. */				// required
	OFPXMT_OFB_SCTP_SRC = 17,		/* SCTP source port. */
	OFPXMT_OFB_SCTP_DST = 18,		/* SCTP destination port. */
	OFPXMT_OFB_ICMPV4_TYPE = 19,	/* ICMP type. */
	OFPXMT_OFB_ICMPV4_CODE = 20,	/* ICMP code. */
	OFPXMT_OFB_ARP_OP = 21,			/* ARP opcode. */
	OFPXMT_OFB_ARP_SPA = 22,		/* ARP source IPv4 address. */
	OFPXMT_OFB_ARP_TPA = 23,		/* ARP target IPv4 address. */
	OFPXMT_OFB_ARP_SHA = 24,		/* ARP source hardware address. */
	OFPXMT_OFB_ARP_THA = 25,		/* ARP target hardware address. */
	OFPXMT_OFB_IPV6_SRC = 26,		/* IPv6 source address. */				// required
	OFPXMT_OFB_IPV6_DST = 27,		/* IPv6 destination address. */			// required
	OFPXMT_OFB_IPV6_FLABEL = 28,	/* IPv6 Flow Label */
	OFPXMT_OFB_ICMPV6_TYPE = 29,	/* ICMPv6 type. */
	OFPXMT_OFB_ICMPV6_CODE = 30,	/* ICMPv6 code. */
	OFPXMT_OFB_IPV6_ND_TARGET = 31,	/* Target address for ND. */
	OFPXMT_OFB_IPV6_ND_SLL = 32,	/* Source link-layer for ND. */
	OFPXMT_OFB_IPV6_ND_TLL = 33,	/* Target link-layer for ND. */
	OFPXMT_OFB_MPLS_LABEL = 34,		/* MPLS label. */
	OFPXMT_OFB_MPLS_TC = 35,		/* MPLS TC. */
	OFPXMT_OFP_MPLS_BOS = 36,		/* MPLS BoS bit. */
	OFPXMT_OFB_PBB_ISID = 37,		/* PBB I-SID. */
	OFPXMT_OFB_TUNNEL_ID = 38,		/* Logical Port Metadata. */
	OFPXMT_OFB_IPV6_EXTHDR = 39,	/* IPv6 Extension Header pseudo-field */
	/* PPP/PPPoE related extensions */
	OFPXMT_OFB_PPPOE_CODE = 40,		/* PPPoE code */
	OFPXMT_OFB_PPPOE_TYPE = 41,		/* PPPoE type */
	OFPXMT_OFB_PPPOE_SID = 42,		/* PPPoE session id */
	OFPXMT_OFB_PPP_PROT = 43,		/* PPP protocol */
	/* max value */
	OFPXMT_OFB_MAX,
};


/* Bit definitions for IPv6 Extension Header pseudo-field. */
enum ofp13_ipv6exthdr_flags {
	OFPIEH_NONEXT = 1 << 0,			/* "No next header" encountered. */
	OFPIEH_ESP = 1 << 1,			/* Encrypted Sec Payload header present. */
	OFPIEH_AUTH = 1 << 2,			/* Authentication header present. */
	OFPIEH_DEST	= 1 << 3,			/* 1 or 2 dest headers present. */
	OFPIEH_FRAG	= 1 << 4,			/* Fragment header present. */
	OFPIEH_ROUTER = 1 << 5,			/* Router header present. */
	OFPIEH_HOP = 1 << 6,			/* Hop-by-hop header present. */
	OFPIEH_UNREP = 1 << 7,			/* Unexpected repeats encountered. */
	OFPIEH_UNSEQ = 1 << 8,			/* Unexpected sequencing encountered. */
};


enum ofp13_instruction_type {
	OFPIT_GOTO_TABLE = 1,			/* Setup the next table in the lookup pipeline */
	OFPIT_WRITE_METADATA = 2,		/* Setup the metadata field for use later in pipeline */
	OFPIT_WRITE_ACTIONS = 3,		/* Write the action(s) onto the datapath action set */
	OFPIT_APPLY_ACTIONS = 4,		/* Applies the action(s) immediately */
	OFPIT_CLEAR_ACTIONS = 5,		/* Clears all actions from the datapath action set */
	OFPIT_METER = 6,				/* Apply meter (rate limiter) */
	OFPIT_EXPERIMENTER = 0xFFFF, 	/* Experimenter instruction */
};


/* Instruction structure for OFPIT_METER */
struct ofp13_instruction_meter {
	uint16_t type;					/* OFPIT_METER */
	uint16_t len;					/* Length is 8. */
	uint32_t meter_id;				/* Meter instance. */
};
OFP_ASSERT(sizeof(struct ofp13_instruction_meter) == 8);


enum ofp13_action_type {
	OFPAT_OUTPUT = 0, 				/* Output to switch port. */
	OFPAT_COPY_TTL_OUT = 11, 		/* Copy TTL "outwards" -- from next-to-outermost to outermost */
	OFPAT_COPY_TTL_IN = 12, 		/* Copy TTL "inwards" -- from outermost to next-to-outermost */
	OFPAT_SET_MPLS_TTL = 15, 		/* MPLS TTL */
	OFPAT_DEC_MPLS_TTL = 16, 		/* Decrement MPLS TTL */
	OFPAT_PUSH_VLAN = 17, 			/* Push a new VLAN tag */
	OFPAT_POP_VLAN = 18, 			/* Pop the outer VLAN tag */
	OFPAT_PUSH_MPLS = 19, 			/* Push a new MPLS tag */
	OFPAT_POP_MPLS = 20, 			/* Pop the outer MPLS tag */
	OFPAT_SET_QUEUE = 21, 			/* Set queue id when outputting to a port */
	OFPAT_GROUP = 22, 				/* Apply group. */
	OFPAT_SET_NW_TTL = 23, 			/* IP TTL. */
	OFPAT_DEC_NW_TTL = 24, 			/* Decrement IP TTL. */
	OFPAT_SET_FIELD = 25, 			/* Set a header field using OXM TLV format. */
	OFPAT_PUSH_PBB = 26, 			/* Push a new PBB service tag (I-TAG) */
	OFPAT_POP_PBB = 27, 			/* Pop the outer PBB service tag (I-TAG) */
	OFPAT_PUSH_PPPOE = 30,			/* Push a new PPPoE tag */
	OFPAT_POP_PPPOE = 31,			/* Pop the PPPoE tag */
	OFPAT_EXPERIMENTER = 0xffff,
};




/* OFPT_HELLO. This message includes zero or more hello elements having
* variable size. Unknown elements types must be ignored/skipped, to allow
* for future extensions. */
struct ofp13_hello {
	struct ofp_header header;
	/* Hello element list */
	struct ofp_hello_elem_header elements[0];
};
OFP_ASSERT(sizeof(struct ofp13_hello) == 8);


/* Hello elements types.
*/
enum ofp13_hello_elem_type {
	OFPHET_VERSIONBITMAP	= 1,/* Bitmap of version supported. */
};


/* Common header for all Hello Elements */
struct ofp13_hello_elem_header {
	uint16_t type;		/* One of OFPHET_*. */
	uint16_t length; 	/* Length in bytes of this element. */
};
OFP_ASSERT(sizeof(struct ofp13_hello_elem_header) == 4);


/* Version bitmap Hello Element */
struct ofp13_hello_elem_versionbitmap {
	uint16_t type;		/* OFPHET_VERSIONBITMAP. */
	uint16_t length; 	/* Length in bytes of this element. */
	/* Followed by:
	 * - Exactly (length - 4) bytes containing the bitmaps, then
	 * - Exactly (length + 7)/8*8 - (length) (between 0 and 7)
	 *   bytes of all-zero bytes */
	uint32_t bitmaps[0];/* List of bitmaps - supported versions */
};
OFP_ASSERT(sizeof(struct ofp13_hello_elem_versionbitmap) == 4);

