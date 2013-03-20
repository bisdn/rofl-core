#include "openflow_common.h"

#define OFP13_VERSION   0x04

/* Fields to match against flows */
struct ofp13_match {
	uint16_t type;			/* One of OFPMT_* */
	uint16_t length;		/* Length of ofp_match (excluding padding) */
	/* Followed by:
	 * - Exactly (length - 4) (possibly 0) bytes containing OXM TLVs, then
	 * - Exactly ((length + 7)/8*8 - length) (between 0 and 7) bytes of
	 * all-zero bytes
	 * In summary, ofp_match is padded as needed, to make its overall size
	 * a multiple of 8, to preserve alignement in structures using it.
	 */
	uint8_t oxm_fields[4];
	/* OXMs start here - Make compiler happy */
};
OFP_ASSERT(sizeof(struct ofp13_match) == 8);



// A.1 OpenFlow Header

/* unaltered since OpenFlow 1.2, new message types defined */



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
#if 0
	OFPQT_MIN_RATE = 1,				/* Minimum datarate guaranteed. */
#endif
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
#if 0
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
#endif
	OFPXMT_OFP_MPLS_BOS = 36,		/* MPLS BoS bit. */
	OFPXMT_OFB_PBB_ISID = 37,		/* PBB I-SID. */
	OFPXMT_OFB_TUNNEL_ID = 38,		/* Logical Port Metadata. */
	OFPXMT_OFB_IPV6_EXTHDR = 39,	/* IPv6 Extension Header pseudo-field */
#if 0
	/* PPP/PPPoE related extensions */
	OFPXMT_OFB_PPPOE_CODE = 40,		/* PPPoE code */
	OFPXMT_OFB_PPPOE_TYPE = 41,		/* PPPoE type */
	OFPXMT_OFB_PPPOE_SID = 42,		/* PPPoE session id */
	OFPXMT_OFB_PPP_PROT = 43,		/* PPP protocol */
	/* max value */
	OFPXMT_OFB_MAX,
#endif
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
#if 0
	OFPIT_GOTO_TABLE = 1,			/* Setup the next table in the lookup pipeline */
	OFPIT_WRITE_METADATA = 2,		/* Setup the metadata field for use later in pipeline */
	OFPIT_WRITE_ACTIONS = 3,		/* Write the action(s) onto the datapath action set */
	OFPIT_APPLY_ACTIONS = 4,		/* Applies the action(s) immediately */
	OFPIT_CLEAR_ACTIONS = 5,		/* Clears all actions from the datapath action set */
#endif
	OFPIT_METER = 6,				/* Apply meter (rate limiter) */
#if 0
	OFPIT_EXPERIMENTER = 0xFFFF, 	/* Experimenter instruction */
#endif
};


/* Instruction structure for OFPIT_METER */
struct ofp13_instruction_meter {
	uint16_t type;					/* OFPIT_METER */
	uint16_t len;					/* Length is 8. */
	uint32_t meter_id;				/* Meter instance. */
};
OFP_ASSERT(sizeof(struct ofp13_instruction_meter) == 8);


enum ofp13_action_type {
#if 0
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
#endif
	OFPAT_PUSH_PBB = 26, 			/* Push a new PBB service tag (I-TAG) */
	OFPAT_POP_PBB = 27, 			/* Pop the outer PBB service tag (I-TAG) */
	OFPAT_PUSH_PPPOE = 30,			/* Push a new PPPoE tag */
	OFPAT_POP_PPPOE = 31,			/* Pop the PPPoE tag */
#if 0
	OFPAT_EXPERIMENTER = 0xffff,
#endif
};




// A3.1 Handshake

/* Switch features. */
struct ofp13_switch_features {
	struct ofp_header header;
	uint64_t datapath_id;		/* Datapath unique ID. The lower 48-bits are for
								   a MAC address, while the upper 16-bits are
								   implementer-defined. */
	uint32_t n_buffers; 		/* Max packets buffered at once. */
	uint8_t n_tables; 			/* Number of tables supported by datapath. */
	uint8_t auxiliary_id; 		/* Identify auxiliary connections */
	uint8_t pad[2]; 			/* Align to 64-bits. */
	/* Features. */
	uint32_t capabilities;	/* Bitmap of support "ofp_capabilities". */
	uint32_t reserved;
};
OFP_ASSERT(sizeof(struct ofp13_switch_features) == 32);

/* Capabilities supported by the datapath. */
enum ofp13_capabilities {
#if 0
	OFPC_FLOW_STATS 	= 1 << 0, /* Flow statistics. */
	OFPC_TABLE_STATS	= 1 << 1, /* Table statistics. */
	OFPC_PORT_STATS		= 1 << 2, /* Port statistics. */
	OFPC_GROUP_STATS	= 1 << 3, /* Group statistics. */
	OFPC_IP_REASM		= 1 << 5, /* Can reassemble IP fragments. */
	OFPC_QUEUE_STATS	= 1 << 6, /* Queue statistics. */
	//OFPC_ARP_MATCH_IP removed from OF1.3 /* Match IP addresses in ARP pkts. */
#endif
	OFPC_PORT_BLOCKED	= 1 << 8, /* Switch will block looping ports. */
};



// A3.2 Switch Configuration

/* unaltered message types since OpenFlow 1.2, flag constants have change */

/* Switch configuration. */
struct ofp13_switch_config {
    struct ofp_header header;
    uint16_t flags;             /* OFPC_* flags. */
    uint16_t miss_send_len;     /* Max bytes of new flow that datapath should
                                   send to the controller. */
};
OFP_ASSERT(sizeof(struct ofp13_switch_config) == 12);

enum ofp13_config_flags {
#if 0
	/* Handling of IP fragments. */
	OFPC_FRAG_NORMAL 	= 0,		/* No special handling for fragments. */
	OFPC_FRAG_DROP		= 1 << 0, 	/* Drop fragments. */
	OFPC_FRAG_REASM		= 1 << 1, 	/* Reassemble (only if OFPC_IP_REASM set). */
	OFPC_FRAG_MASK		= 3,

	// OFPC_INVALID_TTL_TO_CONTROLLER was removed in OF1.3
#endif
};



// A3.3 Flow Table Configuration

/* unaltered message types since OpenFlow 1.2, config constants have changed */

/* Flags to configure the table. Reserved for future use. */
enum ofp13_table_config {
	OFPTC_DEPRECATED_MASK	= 3, 	/* Deprecated bits */
};

/* Configure/Modify behavior of a flow table */
struct ofp13_table_mod {
    struct ofp_header header;
    uint8_t table_id;       /* ID of the table, 0xFF indicates all tables */
    uint8_t pad[3];         /* Pad to 32 bits */
    uint32_t config;        /* Bitmap of OFPTC_* flags */
};
OFP_ASSERT(sizeof(struct ofp13_table_mod) == 16);


// A3.4 Modify State Messages

// A3.4.1 Modify Flow Entry Messages

/* unaltered since OpenFlow 1.2, flag constants have changed */

/* Flow setup and teardown (controller -> datapath). */
struct ofp13_flow_mod {
    struct ofp_header header;
    uint64_t cookie;             /* Opaque controller-issued identifier. */
    uint64_t cookie_mask;        /* Mask used to restrict the cookie bits
                                    that must match when the command is
                                    OFPFC_MODIFY* or OFPFC_DELETE*. A value
                                    of 0 indicates no restriction. */

    /* Flow actions. */
    uint8_t table_id;             /* ID of the table to put the flow in */
    uint8_t command;              /* One of OFPFC_*. */
    uint16_t idle_timeout;        /* Idle time before discarding (seconds). */
    uint16_t hard_timeout;        /* Max time before discarding (seconds). */
    uint16_t priority;            /* Priority level of flow entry. */
    uint32_t buffer_id;           /* Buffered packet to apply to (or -1).
                                     Not meaningful for OFPFC_DELETE*. */
    uint32_t out_port;            /* For OFPFC_DELETE* commands, require
                                     matching entries to include this as an
                                     output port.  A value of OFPP_ANY
                                     indicates no restriction. */
    uint32_t out_group;           /* For OFPFC_DELETE* commands, require
                                     matching entries to include this as an
                                     output group.  A value of OFPG_ANY
                                     indicates no restriction. */
    uint16_t flags;               /* One of OFPFF_*. */
    uint8_t pad[2];
    struct ofp13_match match;     /* Fields to match */
    //struct ofp_instruction instructions[0]; /* Instruction set */
};
OFP_ASSERT(sizeof(struct ofp13_flow_mod) == 56);

enum ofp13_flow_mod_flags {
#if 0
	OFPFF_SEND_FLOW_REM 	= 1 << 0,	/* Send flow removed message when flow expires or is deleted. */
	OFPFF_CHECK_OVERLAP		= 1 << 1, 	/* Check for overlapping entries first. */
	OFPFF_RESET_COUNTS		= 1 << 2,	/* Reset flow packet and byte counts. */
#endif
	OFPFF_NO_PKT_COUNTS		= 1 << 3,	/* Don’t keep track of packet count. */
	OFPFF_NO_BYT_COUNTS		= 1 << 4,	/* Don’t keep track of byte count. */
};


// A3.4.2 Modify Group Entry Message

/* unaltered since OpenFlow 1.2 */

/* Bucket for use in groups. */
struct ofp13_bucket {
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
    struct ofp_action_header actions[0]; /* The action length is inferred
                                           from the length field in the
                                           header. */
};
OFP_ASSERT(sizeof(struct ofp13_bucket) == 16);

/* Group setup and teardown (controller -> datapath). */
struct ofp13_group_mod {
    struct ofp_header header;
    uint16_t command;             /* One of OFPGC_*. */
    uint8_t type;                 /* One of OFPGT_*. */
    uint8_t pad;                  /* Pad to 64 bits. */
    uint32_t group_id;            /* Group identifier. */
    struct ofp13_bucket buckets[0]; /* The bucket length is inferred from the
                                     length field in the header. */
};
OFP_ASSERT(sizeof(struct ofp13_group_mod) == 16);

// A3.4.3 Port Modification Message

/* unaltered since OpenFlow 1.2 */

/* Modify behavior of the physical port */
struct ofp13_port_mod {
    struct ofp_header header;
    uint32_t port_no;
    uint8_t pad[4];
    uint8_t hw_addr[OFP_ETH_ALEN]; /* The hardware address is not
                                      configurable.  This is used to
                                      sanity-check the request, so it must
                                      be the same as returned in an
                                      ofp_port struct. */
    uint8_t pad2[2];        /* Pad to 64 bits. */
    uint32_t config;        /* Bitmap of OFPPC_* flags. */
    uint32_t mask;          /* Bitmap of OFPPC_* flags to be changed. */

    uint32_t advertise;     /* Bitmap of OFPPF_*.  Zero all bits to prevent
                               any action taking place. */
    uint8_t pad3[4];        /* Pad to 64 bits. */
};
OFP_ASSERT(sizeof(struct ofp13_port_mod) == 40);

// A3.4.4 Meter Modification Message


/* Meter numbering. Flow meters can use any number up to OFPM_MAX. */
enum ofp13_meter {
	/* Last usable meter. */
	OFPM_MAX 		= 0xffff0000,

	/* Virtual meters. */
	OFPM_SLOWPATH	= 0xfffffffd, 	/* Meter for slow datapath. */
	OFPM_CONTROLLER = 0xfffffffe, 	/* Meter for controller connection. */
	OFPM_ALL		= 0xffffffff, 	/* Represents all meters for stat requests commands. */
};

/* Meter commands */
enum ofp13_meter_mod_command {
	OFPMC_ADD,				/* New meter. */
	OFPMC_MODIFY,			/* Modify specified meter. */
	OFPMC_DELETE,			/* Delete specified meter. */
};

/* Meter configuration flags */
enum ofp13_meter_flags {
	OFPMF_KBPS		= 1 << 0,		/* Rate value in kb/s (kilo-bit per second). */
	OFPMF_PKTPS		= 1 << 1,		/* Rate value in packet/sec. */
	OFPMF_BURST		= 1 << 2,		/* Do burst size. */
	OFPMF_STATS		= 1 << 3,		/* Collect statistics. */
};

/* Common header for all meter bands */
struct ofp13_meter_band_header {
	uint16_t type;			/* One of OFPMBT_*. */
	uint16_t len;			/* Length in bytes of this band. */
	uint32_t rate;			/* Rate for this band. */
	uint32_t burst_size; 	/* Size of bursts. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_band_header) == 12);

/* Meter band types */
enum ofp13_meter_band_type {
	OFPMBT_DROP			= 1,
	OFPMBT_DSCP_REMARK	= 2,
	OFPMBT_EXPERIMENTER	= 0xFFFF,
};

/* OFPMBT_DROP band - drop packets */
struct ofp13_meter_band_drop {
	uint16_t type;			/* OFPMBT_DROP. */
	uint16_t len;			/* Length in bytes of this band. */
	uint32_t rate;			/* Rate for dropping packets. */
	uint32_t burst_size; 	/* Size of bursts. */
	uint8_t pad[4];
};
OFP_ASSERT(sizeof(struct ofp13_meter_band_drop) == 16);

/* OFPMBT_DSCP_REMARK band - Remark DSCP in the IP header */
struct ofp13_meter_band_dscp_remark {
	uint16_t type;			/* OFPMBT_DSCP_REMARK. */
	uint16_t len;			/* Length in bytes of this band. */
	uint32_t rate;			/* Rate for remarking packets. */
	uint32_t burst_size; 	/* Size of bursts. */
	uint8_t prec_level; 	/* Number of drop precedence level to add. */
	uint8_t pad[3];
};
OFP_ASSERT(sizeof(struct ofp13_meter_band_dscp_remark) == 16);

/* OFPMBT_EXPERIMENTER band - Write actions in action set */
struct ofp13_meter_band_experimenter {
	uint16_t type;			/* One of OFPMBT_*. */
	uint16_t len;			/* Length in bytes of this band. */
	uint32_t rate;			/* Rate for this band. */
	uint32_t burst_size;	/* Size of bursts. */
	uint32_t experimenter; 	/* Experimenter ID which takes the same form as in struct ofp_experimenter_header. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_band_experimenter) == 16);


/* Meter configuration. OFPT_METER_MOD. */
struct ofp13_meter_mod {
	struct ofp_header header;
	uint16_t command; 		/* One of OFPMC_*. */
	uint16_t flags;			/* One of OFPMF_*. */
	uint32_t meter_id;		/* Meter instance. */
	struct ofp13_meter_band_header bands[0];
	/* The bands length is inferred from the length field in the header. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_mod) == 16);






// A3.5 Multipart Messages

struct ofp13_multipart_request {
	struct ofp_header header;
	uint16_t type;					/* One of the OFPMP_* constants. */
	uint16_t flags;					/* OFPMPF_REQ_* flags. */
	uint8_t pad[4];
	uint8_t body[0];				/* Body of the request. */
};
OFP_ASSERT(sizeof(struct ofp13_multipart_request) == 16);

enum ofp13_multipart_request_flags {
	OFPMPF_REQ_MORE = 1 << 0 /* More requests to follow. */
};

struct ofp13_multipart_reply {
	struct ofp_header header;
	uint16_t type;					/* One of the OFPMP_* constants. */
	uint16_t flags;					/* OFPMPF_REPLY_* flags. */
	uint8_t pad[4];
	uint8_t body[0];				/* Body of the reply. */
};
OFP_ASSERT(sizeof(struct ofp13_multipart_reply) == 16);


enum ofp13_multipart_reply_flags {
	OFPMPF_REPLY_MORE = 1 << 0 /* More replies to follow. */
};


enum ofp13_multipart_types {
	/* Description of this OpenFlow switch.
	* The request body is empty.
	* The reply body is struct ofp_desc. */
	OFPMP_DESC = 0,

	/* Individual flow statistics.
	* The request body is struct ofp_flow_stats_request.
	* The reply body is an array of struct ofp_flow_stats. */
	OFPMP_FLOW = 1,

	/* Aggregate flow statistics.
	* The request body is struct ofp_aggregate_stats_request.
	* The reply body is struct ofp_aggregate_stats_reply. */
	OFPMP_AGGREGATE = 2,

	/* Flow table statistics.
	* The request body is empty.
	* The reply body is an array of struct ofp_table_stats. */
	OFPMP_TABLE = 3,

	/* Port statistics.
	* The request body is struct ofp_port_stats_request.
	* The reply body is an array of struct ofp_port_stats. */
	OFPMP_PORT_STATS = 4,

	/* Queue statistics for a port
	* The request body is struct ofp_queue_stats_request.
	* The reply body is an array of struct ofp_queue_stats */
	OFPMP_QUEUE = 5,

	/* Group counter statistics.
	* The request body is struct ofp_group_stats_request.
	* The reply is an array of struct ofp_group_stats. */
	OFPMP_GROUP = 6,

	/* Group description.
	* The request body is empty.
	* The reply body is an array of struct ofp_group_desc_stats. */
	OFPMP_GROUP_DESC = 7,

	/* Group features.
	* The request body is empty.
	* The reply body is struct ofp_group_features. */
	OFPMP_GROUP_FEATURES = 8,

	/* Meter statistics.
	* The request body is struct ofp_meter_multipart_requests.
	* The reply body is an array of struct ofp_meter_stats. */
	OFPMP_METER = 9,

	/* Meter configuration.
	* The request body is struct ofp_meter_multipart_requests.
	* The reply body is an array of struct ofp_meter_config. */
	OFPMP_METER_CONFIG = 10,

	/* Meter features.
	* The request body is empty.
	* The reply body is struct ofp_meter_features. */
	OFPMP_METER_FEATURES = 11,

	/* Table features.
	* The request body is either empty or contains an array of
	* struct ofp_table_features containing the controller’s
	* desired view of the switch. If the switch is unable to
	* set the specified view an error is returned.
	* The reply body is an array of struct ofp_table_features. */
	OFPMP_TABLE_FEATURES = 12,

	/* Port description.
	* The request body is empty.
	* The reply body is an array of struct ofp_port. */
	OFPMP_PORT_DESC = 13,

	/* Experimenter extension.
	* The request and reply bodies begin with
	* struct ofp_experimenter_multipart_header.
	* The request and reply bodies are otherwise experimenter-defined. */
	OFPMP_EXPERIMENTER = 0xffff
};

// A3.5.1 Description

/* Body of reply to OFPMP_DESC request. Each entry is a NULL-terminated
* ASCII string. */
struct ofp13_desc {
	char mfr_desc[DESC_STR_LEN];		/* Manufacturer description. */
	char hw_desc[DESC_STR_LEN];			/* Hardware description. */
	char sw_desc[DESC_STR_LEN];			/* Software description. */
	char serial_num[SERIAL_NUM_LEN];	/* Serial number. */
	char dp_desc[DESC_STR_LEN];			/* Human readable description of datapath. */
};
OFP_ASSERT(sizeof(struct ofp13_desc) == 1056);

// A3.5.2 Individual Flow Statistics

/* Body for ofp_multipart_request of type OFPMP_FLOW. */
struct ofp13_flow_stats_request {
	uint8_t table_id;					/* ID of table to read (from ofp_table_stats), OFPTT_ALL for all tables. */
	uint8_t pad[3];						/* Align to 32 bits. */
	uint32_t out_port;					/* Require matching entries to include this as an output port. A value of OFPP_ANY indicates no restriction. */
	uint32_t out_group;					/* Require matching entries to include this as an output group. A value of OFPG_ANY indicates no restriction. */
	uint8_t pad2[4];					/* Align to 64 bits. */
	uint64_t cookie;					/* Require matching entries to contain this cookie value */
	uint64_t cookie_mask;				/* Mask used to restrict the cookie bits that must match. A value of 0 indicates no restriction. */
	struct ofp13_match match;			/* Fields to match. Variable size. */
};
OFP_ASSERT(sizeof(struct ofp13_flow_stats_request) == 40);

/* Body of reply to OFPST_FLOW request. */
struct ofp13_flow_stats {
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
    struct ofp13_match match; /* Description of fields. */
    //struct ofp_instruction instructions[0]; /* Instruction set. */
};
OFP_ASSERT(sizeof(struct ofp13_flow_stats) == 56);

// A3.5.3 Aggregate Flow Statistics

/* Body for ofp_stats_request of type OFPST_AGGREGATE. */
struct ofp13_aggregate_stats_request {
    uint8_t table_id;         /* ID of table to read (from ofp_table_stats)
                                 0xff for all tables. */
    uint8_t pad[3];           /* Align to 64 bits. */
    uint32_t out_port;        /* Require matching entries to include this
                                 as an output port.  A value of OFPP_ANY
                                 indicates no restriction. */
    uint32_t out_group;       /* Require matching entries to include this
                                 as an output group.  A value of OFPG_ANY
                                 indicates no restriction. */
    uint8_t pad2[4];          /* Align to 64 bits. */
    uint64_t cookie;          /* Require matching entries to contain this
                                 cookie value */
    uint64_t cookie_mask;     /* Mask used to restrict the cookie bits that
                                 must match. A value of 0 indicates
                                 no restriction. */
    struct ofp13_match match; /* Fields to match. */
};
OFP_ASSERT(sizeof(struct ofp13_aggregate_stats_request) == 40);

/* Body of reply to OFPST_AGGREGATE request. */
struct ofp13_aggregate_stats_reply {
    uint64_t packet_count;    	/* Number of packets in flows. */
    uint64_t byte_count;      	/* Number of bytes in flows. */
    uint32_t flow_count;      	/* Number of flows. */
    uint8_t pad[4];           	/* Align to 64 bits. */
};
OFP_ASSERT(sizeof(struct ofp13_aggregate_stats_reply) == 24);


// A3.5.4 Table Statistics

/* Body of reply to OFPMP_TABLE request. */
struct ofp13_table_stats {
	uint8_t table_id;			/* Identifier of table. Lower numbered tables are consulted first. */
	uint8_t pad[3];				/* Align to 32-bits. */
	uint32_t active_count;		/* Number of active entries. */
	uint64_t lookup_count;		/* Number of packets looked up in table. */
	uint64_t matched_count; 	/* Number of packets that hit table. */
};
OFP_ASSERT(sizeof(struct ofp13_table_stats) == 24);

// A3.5.5 Table Features



/* Table Feature property types.
* Low order bit cleared indicates a property for a regular Flow Entry.
* Low order bit set indicates a property for the Table-Miss Flow Entry.
*/
enum ofp13_table_feature_prop_type {
	OFPTFPT_INSTRUCTIONS 			= 0,	/* Instructions property. */
	OFPTFPT_INSTRUCTIONS_MISS 		= 1, 	/* Instructions for table-miss. */
	OFPTFPT_NEXT_TABLES 			= 2, 	/* Next Table property. */
	OFPTFPT_NEXT_TABLES_MISS 		= 3, 	/* Next Table for table-miss. */
	OFPTFPT_WRITE_ACTIONS 			= 4, 	/* Write Actions property. */
	OFPTFPT_WRITE_ACTIONS_MISS 		= 5, 	/* Write Actions for table-miss. */
	OFPTFPT_APPLY_ACTIONS 			= 6, 	/* Apply Actions property. */
	OFPTFPT_APPLY_ACTIONS_MISS 		= 7, 	/* Apply Actions for table-miss. */
	OFPTFPT_MATCH 					= 8, 	/* Match property. */
	OFPTFPT_WILDCARDS 				= 10, 	/* Wildcards property. */
	OFPTFPT_WRITE_SETFIELD 			= 12, 	/* Write Set-Field property. */
	OFPTFPT_WRITE_SETFIELD_MISS 	= 13, 	/* Write Set-Field for table-miss. */
	OFPTFPT_APPLY_SETFIELD 			= 14, 	/* Apply Set-Field property. */
	OFPTFPT_APPLY_SETFIELD_MISS 	= 15, 	/* Apply Set-Field for table-miss. */
	OFPTFPT_EXPERIMENTER 			= 0xFFFE, 	/* Experimenter property. */
	OFPTFPT_EXPERIMENTER_MISS 		= 0xFFFF, 	/* Experimenter for table-miss. */
};

/* Common header for all Table Feature Properties */
struct ofp13_table_feature_prop_header {
	uint16_t type;				/* One of OFPTFPT_*. */
	uint16_t length; 			/* Length in bytes of this property. */
};
OFP_ASSERT(sizeof(struct ofp13_table_feature_prop_header) == 4);


/* Instructions property */
struct ofp13_table_feature_prop_instructions {
	uint16_t type;				/* One of OFPTFPT_INSTRUCTIONS, OFPTFPT_INSTRUCTIONS_MISS. */
	uint16_t length; 			/* Length in bytes of this property. */
	/* Followed by:
	 * - Exactly (length - 4) bytes containing the instruction ids, then
	 * - Exactly (length + 7)/8*8 - (length) (between 0 and 7)
	 *   bytes of all-zero bytes */
	struct ofp_instruction instruction_ids[0];	/* List of instructions */
};
OFP_ASSERT(sizeof(struct ofp13_table_feature_prop_instructions) == 4);


/* Next Tables property */
struct ofp13_table_feature_prop_next_tables {
	uint16_t type;				/* One of OFPTFPT_NEXT_TABLES, OFPTFPT_NEXT_TABLES_MISS. */
	uint16_t length; 			/* Length in bytes of this property. */
	/* Followed by:
	 * - Exactly (length - 4) bytes containing the table_ids, then
	 * - Exactly (length + 7)/8*8 - (length) (between 0 and 7)
	 *   bytes of all-zero bytes */
	uint8_t next_table_ids[0];
};
OFP_ASSERT(sizeof(struct ofp13_table_feature_prop_next_tables) == 4);



/* Actions property */
struct ofp13_table_feature_prop_actions {
	uint16_t type;				/* One of OFPTFPT_WRITE_ACTIONS, OFPTFPT_WRITE_ACTIONS_MISS, OFPTFPT_APPLY_ACTIONS, OFPTFPT_APPLY_ACTIONS_MISS. */
	uint16_t length; 			/* Length in bytes of this property. */
	/* Followed by:
	 * - Exactly (length - 4) bytes containing the action_ids, then
	 * - Exactly (length + 7)/8*8 - (length) (between 0 and 7)
	 *   bytes of all-zero bytes */
	struct ofp_action_header action_ids[0]; /* List of actions */
};
OFP_ASSERT(sizeof(struct ofp13_table_feature_prop_actions) == 4);


/* Match, Wildcard or Set-Field property */
struct ofp13_table_feature_prop_oxm {
	uint16_t type;				/* One of OFPTFPT_MATCH, OFPTFPT_WILDCARDS, OFPTFPT_WRITE_SETFIELD, OFPTFPT_WRITE_SETFIELD_MISS, OFPTFPT_APPLY_SETFIELD, OFPTFPT_APPLY_SETFIELD_MISS. */
	uint16_t length;			/* Length in bytes of this property. */
	/* Followed by:
	 * - Exactly (length - 4) bytes containing the oxm_ids, then
	 * - Exactly (length + 7)/8*8 - (length) (between 0 and 7)
	 *   bytes of all-zero bytes */
	uint32_t oxm_ids[0];		/* Array of OXM headers */
};
OFP_ASSERT(sizeof(struct ofp13_table_feature_prop_oxm) == 4);



/* Experimenter table feature property */
struct ofp13_table_feature_prop_experimenter {
	uint16_t type;				/* One of OFPTFPT_EXPERIMENTER, OFPTFPT_EXPERIMENTER_MISS. */
	uint16_t length; 			/* Length in bytes of this property. */
	uint32_t experimenter; 		/* Experimenter ID which takes the same form as in struct ofp_experimenter_header. */
	uint32_t exp_type; 			/* Experimenter defined. */
	/* Followed by:
	 * - Exactly (length - 12) bytes containing the experimenter data, then
	 * - Exactly (length + 7)/8*8 - (length) (between 0 and 7)
	 *   bytes of all-zero bytes */
	uint32_t experimenter_data[0];
};
OFP_ASSERT(sizeof(struct ofp13_table_feature_prop_experimenter) == 12);


/* Body for ofp_multipart_request of type OFPMP_TABLE_FEATURES./
 * Body of reply to OFPMP_TABLE_FEATURES request. */
struct ofp13_table_features {
	uint16_t length;			/* Length is padded to 64 bits. */
	uint8_t table_id;			/* Identifier of table. Lower numbered tables are consulted first. */
	uint8_t pad[5];				/* Align to 64-bits. */
	char name[OFP_MAX_TABLE_NAME_LEN];
	uint64_t metadata_match;	/* Bits of metadata table can match. */
	uint64_t metadata_write; 	/* Bits of metadata table can write. */
	uint32_t config;			/* Bitmap of OFPTC_* values */
	uint32_t max_entries;		/* Max number of entries supported. */
	/* Table Feature Property list */
	struct ofp13_table_feature_prop_header properties[0];
};
OFP_ASSERT(sizeof(struct ofp13_table_features) == 64);




// A3.5.6 Port Statistics

/* Body for ofp_multipart_request of type OFPMP_PORT. */
struct ofp13_port_stats_request {
	uint32_t port_no;
	/* OFPMP_PORT message must request statistics
	 * either for a single port (specified in
	 * port_no) or for all ports (if port_no ==
	 * OFPP_ANY). */
	uint8_t pad[4];
};
OFP_ASSERT(sizeof(struct ofp13_port_stats_request) == 8);


/* Body of reply to OFPMP_PORT request. If a counter is unsupported, set
* the field to all ones. */
struct ofp13_port_stats {
	uint32_t port_no;
	uint8_t pad[4];				/* Align to 64-bits. */
	uint64_t rx_packets;		/* Number of received packets. */
	uint64_t tx_packets;		/* Number of transmitted packets. */
	uint64_t rx_bytes;			/* Number of received bytes. */
	uint64_t tx_bytes;			/* Number of transmitted bytes. */
	uint64_t rx_dropped;		/* Number of packets dropped by RX. */
	uint64_t tx_dropped;		/* Number of packets dropped by TX. */
	uint64_t rx_errors;			/* Number of receive errors. This is a super-set
								   of more specific receive errors and should be
								   greater than or equal to the sum of all
								   rx_*_err values. */
	uint64_t tx_errors;			/* Number of transmit errors. This is a super-set
								   of more specific transmit errors and should be
								   greater than or equal to the sum of all
								   tx_*_err values (none currently defined.) */
	uint64_t rx_frame_err;		/* Number of frame alignment errors. */
	uint64_t rx_over_err;		/* Number of packets with RX overrun. */
	uint64_t rx_crc_err;		/* Number of CRC errors. */
	uint64_t collisions;		/* Number of collisions. */
	uint32_t duration_sec;		/* Time port has been alive in seconds. */
	uint32_t duration_nsec; 	/* Time port has been alive in nanoseconds beyond
								   duration_sec. */
};
OFP_ASSERT(sizeof(struct ofp13_port_stats) == 112);


// A3.5.7 Port Description


/* Description of a port */
struct ofp13_port {
    uint32_t port_no;
    uint8_t pad[4];
    uint8_t hw_addr[OFP_ETH_ALEN];
    uint8_t pad2[2];                  /* Align to 64 bits. */
    char name[OFP_MAX_PORT_NAME_LEN]; /* Null-terminated */

    uint32_t config;        /* Bitmap of OFPPC_* flags. */
    uint32_t state;         /* Bitmap of OFPPS_* flags. */

    /* Bitmaps of OFPPF_* that describe features.  All bits zeroed if
     * unsupported or unavailable. */
    uint32_t curr;          /* Current features. */
    uint32_t advertised;    /* Features being advertised by the port. */
    uint32_t supported;     /* Features supported by the port. */
    uint32_t peer;          /* Features advertised by peer. */

    uint32_t curr_speed;    /* Current port bitrate in kbps. */
    uint32_t max_speed;     /* Max port bitrate in kbps */
};
OFP_ASSERT(sizeof(struct ofp13_port) == 64);


// A3.5.8 Queue Statistics

struct ofp13_queue_stats_request {
	uint32_t port_no;		/* All ports if OFPP_ANY. */
	uint32_t queue_id;		/* All queues if OFPQ_ALL. */
};
OFP_ASSERT(sizeof(struct ofp13_queue_stats_request) == 8);


struct ofp13_queue_stats {
	uint32_t port_no;
	uint32_t queue_id;		/* Queue i.d */
	uint64_t tx_bytes;		/* Number of transmitted bytes. */
	uint64_t tx_packets;	/* Number of transmitted packets. */
	uint64_t tx_errors;		/* Number of packets dropped due to overrun. */
	uint32_t duration_sec;	/* Time queue has been alive in seconds. */
	uint32_t duration_nsec;	/* Time queue has been alive in nanoseconds beyond duration_sec. */
};
OFP_ASSERT(sizeof(struct ofp13_queue_stats) == 40);

// A3.5.9 Group Statistics

/* Body of OFPMP_GROUP request. */
struct ofp13_group_stats_request {
	uint32_t group_id;		/* All groups if OFPG_ALL. */
	uint8_t pad[4];			/* Align to 64 bits. */
};
OFP_ASSERT(sizeof(struct ofp13_group_stats_request) == 8);

/* Body of reply to OFPMP_GROUP request. */
struct ofp13_group_stats {
	uint16_t length; 		/* Length of this entry. */
	uint8_t pad[2]; 		/* Align to 64 bits. */
	uint32_t group_id; 		/* Group identifier. */
	uint32_t ref_count; 	/* Number of flows or groups that directly forward to this group. */
	uint8_t pad2[4]; 		/* Align to 64 bits. */
	uint64_t packet_count; 	/* Number of packets processed by group. */
	uint64_t byte_count; 	/* Number of bytes processed by group. */
	uint32_t duration_sec; 	/* Time group has been alive in seconds. */
	uint32_t duration_nsec; /* Time group has been alive in nanoseconds beyond duration_sec. */
	struct ofp12_bucket_counter bucket_stats[0];
};
OFP_ASSERT(sizeof(struct ofp13_group_stats) == 40);

/* Used in group stats replies. */
struct ofp13_bucket_counter {
	uint64_t packet_count;	/* Number of packets processed by bucket. */
	uint64_t byte_count;	/* Number of bytes processed by bucket. */
};
OFP_ASSERT(sizeof(struct ofp13_bucket_counter) == 16);

// A3.5.10 Group Description

/* Body of reply to OFPMP_GROUP_DESC request. */
struct ofp13_group_desc_stats {
	uint16_t length;  		/* Length of this entry. */
	uint8_t type;			/* One of OFPGT_*. */
	uint8_t pad;			/* Pad to 64 bits. */
	uint32_t group_id;		/* Group identifier. */
	struct ofp12_bucket buckets[0];
};
OFP_ASSERT(sizeof(struct ofp13_group_desc_stats) == 8);

// A3.5.11 Group Features

/* Body of reply to OFPMP_GROUP_FEATURES request. Group features. */
struct ofp13_group_features {
	uint32_t types;			/* Bitmap of OFPGT_* values supported. */
	uint32_t capabilities;	/* Bitmap of OFPGFC_* capability supported. */
	uint32_t max_groups[4];	/* Maximum number of groups for each type. */
	uint32_t actions[4];	/* Bitmaps of OFPAT_* that are supported. */
};
OFP_ASSERT(sizeof(struct ofp13_group_features) == 40);

/* Group configuration flags */
enum ofp13_group_capabilities {
#if 0
	OFPGFC_SELECT_WEIGHT 		= 1 << 0, 	/* Support weight for select groups */
	OFPGFC_SELECT_LIVENESS 		= 1 << 1, 	/* Support liveness for select groups */
	OFPGFC_CHAINING 			= 1 << 2, 	/* Support chaining groups */
	OFPGFC_CHAINING_CHECKS 		= 1 << 3, 	/* Check chaining for loops and delete */
#endif
};

// A3.5.12 Meter Statistics

/* Body of OFPMP_METER and OFPMP_METER_CONFIG requests. */
struct ofp13_meter_multipart_request {
	uint32_t meter_id;		/* Meter instance, or OFPM_ALL. */
	uint8_t pad[4];			/* Align to 64 bits. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_multipart_request) == 8);



/* Statistics for each meter band */
struct ofp13_meter_band_stats {
	uint64_t packet_band_count;	/* Number of packets in band. */
	uint64_t byte_band_count; 	/* Number of bytes in band. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_band_stats) == 16);

/* Body of reply to OFPMP_METER request. Meter statistics. */
struct ofp13_meter_stats {
	uint32_t meter_id;			/* Meter instance. */
	uint16_t len;				/* Length in bytes of this stats. */
	uint8_t	pad[6];
	uint32_t flow_count;		/* Number of flows bound to meter. */
	uint64_t packet_in_count; 	/* Number of packets in input. */
	uint64_t byte_in_count;		/* Number of bytes in input. */
	uint32_t duration_sec; 		/* Time meter has been alive in seconds. */
	uint32_t duration_nsec; 	/* Time meter has been alive in nanoseconds beyond duration_sec. */
	struct ofp13_meter_band_stats band_stats[0]; /* The band_stats length is inferred from the length field. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_stats) == 40);


// A3.5.13 Meter Configuration Statistics

/* Body of request => same as for Meter Statistics */

/* Body of reply to OFPMP_METER_CONFIG request. Meter configuration. */
struct ofp13_meter_config {
	uint16_t length;			/* Length of this entry. */
	uint16_t flags;				/* All OFPMC_* that apply. */
	uint32_t meter_id;			/* Meter instance. */
	struct ofp13_meter_band_header bands[0]; /* The bands length is inferred from the length field. */
};
OFP_ASSERT(sizeof(struct ofp13_meter_config) == 8);

// A3.5.14 Meter Features Statistics

/* Body of request is empty */

/* Body of reply to OFPMP_METER_FEATURES request. Meter features. */
struct ofp13_meter_features {
	uint32_t max_meter;			/* Maximum number of meters. */
	uint32_t band_types;		/* Bitmaps of OFPMBT_* values supported. */
	uint32_t capabilities; 		/* Bitmaps of "ofp_meter_flags". */
	uint8_t max_bands;			/* Maximum bands per meters */
	uint8_t	max_color;			/* Maximum color value */
	uint8_t pad[2];
};
OFP_ASSERT(sizeof(struct ofp13_meter_features) == 16);

// A3.5.15 Experimenter Multipart

/* Body for ofp_multipart_request/reply of type OFPMP_EXPERIMENTER. */
struct ofp13_experimenter_multipart_header {
	uint32_t experimenter;		/* Experimenter ID which takes the same form as in struct ofp_experimenter_header. */
	uint32_t exp_type;			/* Experimenter defined. */
	/* Experimenter-defined arbitrary additional data. */
};
OFP_ASSERT(sizeof(struct ofp13_experimenter_multipart_header) == 8);



// A3.6 Queue Configuration Messages

/* unaltered since OpenFlow 1.2 */

/* Query for port queue configuration. */
struct ofp13_queue_get_config_request {
    struct ofp_header header;
    uint32_t port;         /* Port to be queried. Should refer
                              to a valid physical port (i.e. < OFPP_MAX) */
    uint8_t pad[4];
};
OFP_ASSERT(sizeof(struct ofp13_queue_get_config_request) == 16);

/* Queue configuration for a given port. */
struct ofp13_queue_get_config_reply {
    struct ofp_header header;
    uint32_t port;
    uint8_t pad[4];
    struct ofp_packet_queue queues[0]; /* List of configured queues. */
};
OFP_ASSERT(sizeof(struct ofp13_queue_get_config_reply) == 16);

// A3.7 Packet-Out Message

/* unaltered since OpenFlow 1.2 */

/* Send packet (controller -> datapath). */
struct ofp13_packet_out {
    struct ofp_header header;
    uint32_t buffer_id;           /* ID assigned by datapath (-1 if none). */
    uint32_t in_port;             /* Packet's input port or OFPP_CONTROLLER. */
    uint16_t actions_len;         /* Size of action array in bytes. */
    uint8_t pad[6];
    struct ofp_action_header actions[0]; /* Action list. */
    /* uint8_t data[0]; */        /* Packet data.  The length is inferred
                                     from the length field in the header.
                                     (Only meaningful if buffer_id == -1.) */
};
OFP_ASSERT(sizeof(struct ofp13_packet_out) == 24);


// A3.8 Barrier Message

/* unaltered since OpenFlow 1.2 */


// A3.9 Role-Request Message

/* unaltered since OpenFlow 1.2 */


// A3.10 Set Asynchronous Configuration Message

/* Asynchronous message configuration. */
struct ofp13_async_config {
	struct ofp_header header; 		/* OFPT_GET_ASYNC_REPLY or OFPT_SET_ASYNC. */
	uint32_t packet_in_mask[2];		/* Bitmasks of OFPR_* values. */
	uint32_t port_status_mask[2]; 	/* Bitmasks of OFPPR_* values. */
	uint32_t flow_removed_mask[2];	/* Bitmasks of OFPRR_* values. */
};
OFP_ASSERT(sizeof(struct ofp13_async_config) == 32);



// A4.1 Packet-In Message

/* Packet received on port (datapath -> controller). */
struct ofp13_packet_in {
	struct ofp_header header;
	uint32_t buffer_id;				/* ID assigned by datapath. */
	uint16_t total_len;				/* Full length of frame. */
	uint8_t reason;					/* Reason packet is being sent (one of OFPR_*) */
	uint8_t table_id;				/* ID of the table that was looked up */
	uint64_t cookie;				/* Cookie of the flow entry that was looked up. */
	struct ofp13_match match; 		/* Packet metadata. Variable size. */
	/* Followed by:
	 * - Exactly 2 all-zero padding bytes, then
	 * - An Ethernet frame whose length is inferred from header.length.
	 *   The padding bytes preceding the Ethernet frame ensure that the IP
	 *   header (if any) following the Ethernet header is 32-bit aligned.
	 */
	//uint8_t pad[2];				/* Align to 64 bit + 16 bit */
	//uint8_t data[0];				/* Ethernet frame */
};
OFP_ASSERT(sizeof(struct ofp13_packet_in) == 32);



// A4.2 Flow Removed Message

/* unaltered since OpenFlow 1.2 */

/* Flow removed (datapath -> controller). */ // adjusted to OF1.2
struct ofp13_flow_removed {
    struct ofp_header header;
    uint64_t cookie;          /* Opaque controller-issued identifier. */

    uint16_t priority;        /* Priority level of flow entry. */
    uint8_t reason;           /* One of OFPRR_*. */
    uint8_t table_id;         /* ID of the table */

    uint32_t duration_sec;    /* Time flow was alive in seconds. */
    uint32_t duration_nsec;   /* Time flow was alive in nanoseconds beyond
                                 duration_sec. */
    uint16_t idle_timeout;    /* Idle timeout from original flow mod. */
    uint16_t hard_timeout;    /* Idle timeout from original flow mod. */
    uint64_t packet_count;
    uint64_t byte_count;
    struct ofp13_match match; /* Description of fields. */
};
OFP_ASSERT(sizeof(struct ofp13_flow_removed) == 56);


// A4.3 Port Status Message

/* unaltered since OpenFlow 1.2 */

/* A physical port has changed in the datapath */
struct ofp13_port_status {
    struct ofp_header header;
    uint8_t reason;          /* One of OFPPR_*. */
    uint8_t pad[7];          /* Align to 64-bits. */
    struct ofp13_port desc;
};
OFP_ASSERT(sizeof(struct ofp13_port_status) == 80);

// A4.4 Error Message

/* unaltered since OpenFlow 1.2, new error types */

/* Values for ’type’ in ofp_error_message. These values are immutable: they
* will not change in future versions of the protocol (although new values may
* be added). */
enum ofp13_error_type {
#if 0
	OFPET_HELLO_FAILED			= 0, 	/* Hello protocol failed. */
	OFPET_BAD_REQUEST			= 1, 	/* Request was not understood. */
	OFPET_BAD_ACTION			= 2, 	/* Error in action description. */
	OFPET_BAD_INSTRUCTION		= 3, 	/* Error in instruction list. */
	OFPET_BAD_MATCH				= 4, 	/* Error in match. */
	OFPET_FLOW_MOD_FAILED		= 5, 	/* Problem modifying flow entry. */
	OFPET_GROUP_MOD_FAILED		= 6, 	/* Problem modifying group entry. */
	OFPET_PORT_MOD_FAILED		= 7, 	/* Port mod request failed. */
	OFPET_TABLE_MOD_FAILED		= 8, 	/* Table mod request failed. */
	OFPET_QUEUE_OP_FAILED		= 9, 	/* Queue operation failed. */
	OFPET_SWITCH_CONFIG_FAILED 	= 10, 	/* Switch config request failed. */
	OFPET_ROLE_REQUEST_FAILED 	= 11, 	/* Controller Role request failed. */
#endif
	OFPET_METER_MOD_FAILED		= 12, 	/* Error in meter. */
	OFPET_TABLE_FEATURES_FAILED = 13, 	/* Setting table features failed. */
#if 0
	OFPET_EXPERIMENTER 			= 0xffff/* Experimenter error messages. */
#endif
};

/* ofp_error_msg ’code’ values for OFPET_HELLO_FAILED. ’data’ contains an
 * ASCII text string that may give failure details. */
enum ofp13_hello_failed_code {
#if 0
	OFPHFC_INCOMPATIBLE 		= 0,	/* No compatible version. */
	OFPHFC_EPERM 				= 1,  	/* Permissions error. */
#endif
};



// A5.1 Hello



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


/* OFPT_HELLO. This message includes zero or more hello elements having
* variable size. Unknown elements types must be ignored/skipped, to allow
* for future extensions. */
struct ofp13_hello {
	struct ofp_header header;
	/* Hello element list */
	struct ofp13_hello_elem_header elements[0];
};
OFP_ASSERT(sizeof(struct ofp13_hello) == 8);



// A5.2 Echo Request

/* unaltered since OpenFlow 1.2 */


// A5.3 Echo Reply

/* unaltered since OpenFlow 1.2 */


// A5.4 Experimenter

/* unaltered since OpenFlow 1.2 */


