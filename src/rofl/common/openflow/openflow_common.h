/*
 * openflow.h
 *
 *  Created on: 02.03.2013
 *      Author: andi
 */

#ifndef OPENFLOW_COMMON_H_
#define OPENFLOW_COMMON_H_ 1

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#ifdef SWIG
#define OFP_ASSERT(EXPR)        /* SWIG can't handle OFP10_ASSERT. */
#elif !defined(__cplusplus)
/* Build-time assertion for use in a declaration context. */
#define OFP_ASSERT(EXPR)                                                \
        extern int (*build_assert(void))[ sizeof(struct {               \
                    unsigned int build_assert_failed : (EXPR) ? 1 : -1; })]
#else /* __cplusplus */
#define OFP_ASSERT(_EXPR) typedef int build_assert_failed[(_EXPR) ? 1 : -1]
#endif /* __cplusplus */

#ifndef SWIG
#define OFP_PACKED __attribute__((packed))
#else
#define OFP_PACKED              /* SWIG doesn't understand __attribute. */
#endif

#define OFP_MAX_TABLE_NAME_LEN 32
#define OFP_MAX_PORT_NAME_LEN  16

#define OFP_TCP_PORT  6633
#define OFP_SSL_PORT  6633

#define OFP_VERSION_UNKNOWN 0

#define OFP_ETH_ALEN 6          /* Bytes in an Ethernet address. */

namespace rofl {
namespace openflow {

	/* Header on all OpenFlow packets. */
	struct ofp_header {
		uint8_t version;    /* OFP10_VERSION. */
		uint8_t type;       /* One of the OFP10T_ constants. */
		uint16_t length;    /* Length including this ofp10_header. */
		uint32_t xid;       /* Transaction id associated with this packet.
							   Replies use the same id as was in the request
							   to facilitate pairing. */
	};
	OFP_ASSERT(sizeof(struct ofp_header) == 8);


	/* OFPT_ERROR: Error message (datapath -> controller). */
	struct ofp_error_msg {
		struct ofp_header header;

		uint16_t type;
		uint16_t code;
		uint8_t data[0];          /* Variable-length data.  Interpreted based
									 on the type and code.  No padding. */
	};
	OFP_ASSERT(sizeof(struct ofp_error_msg) == 12);







	/* Action header that is common to all actions.  The length includes the
	 * header and any padding used to make the action 64-bit aligned.
	 * NB: The length of an action *must* always be a multiple of eight. */
	struct ofp_action_header {
		uint16_t type;                  /* One of OFPAT_*. */
		uint16_t len;                   /* Length of action, including this
										   header.  This is the length of action,
										   including any padding to make it
										   64-bit aligned. */
		uint8_t pad[4];
	};
	OFP_ASSERT(sizeof(struct ofp_action_header) == 8);


	struct ofp_oxm_hdr {
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
	struct ofp_oxm_ofb_uint8_t {
		struct ofp_oxm_hdr hdr;		/* oxm header */
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
	struct ofp_oxm_ofb_uint16_t {
		struct ofp_oxm_hdr hdr;		/* oxm header */
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
	struct ofp_oxm_ofb_uint32_t {
		struct ofp_oxm_hdr hdr;		/* oxm header */
		uint32_t dword;				/* network byte order */
		uint32_t mask;				/* only valid, when oxm_hasmask=1 */
	};


	// OXM_OF_IPV6_ND_SLL
	// OXM_OF_IPV6_ND_TLL
	struct ofp_oxm_ofb_uint48_t {
		struct ofp_oxm_hdr hdr;		/* oxm header */
		uint8_t value[6];
		uint8_t mask[6];			/* only valid, when oxm_hasmask=1 */
	};

	// OXM_OF_METADATA (mask)
	struct ofp_oxm_ofb_uint64_t {
		struct ofp_oxm_hdr hdr;		/* oxm header */
		uint8_t word[8];
		uint8_t mask[8];
	#if 0
		uint64_t qword;				/* network byte order */
		uint64_t mask;				/* only valid, when oxm_hasmask=1 */
	#endif
	};


	// OXM_OF_ETH_DST (mask)
	// OXM_OF_ETH_SRC (mask)
	struct ofp_oxm_ofb_maddr {
		struct ofp_oxm_hdr hdr;		/* oxm header */
		uint8_t addr[OFP_ETH_ALEN];
		uint8_t mask[OFP_ETH_ALEN]; /* only valid, when oxm_hasmask=1 */
	};


	// OXM_OF_IPV6_SRC (mask)
	// OXM_OF_IPV6_DST (mask)
	// OXM_OF_IPV6_ND_TARGET
	struct ofp_oxm_ofb_ipv6_addr {
		struct ofp_oxm_hdr hdr;		/* oxm header */
		uint8_t addr[16];
		uint8_t mask[16];			/* only valid, when oxm_hasmask=1 */
	};


	/* OXM Class IDs.
	 * The high order bit differentiate reserved classes from member classes.
	 * Classes 0x0000 to 0x7FFF are member classes, allocated by ONF.
	 * Classes 0x8000 to 0xFFFE are reserved classes, reserved for standardisation.
	 */
	enum ofp_oxm_class {
		OFPXMC_NXM_0			= 0x0000, 	/* Backward compatibility with NXM */
		OFPXMC_NXM_1			= 0x0001,	/* Backward compatibility with NXM */
		OFPXMC_OPENFLOW_BASIC	= 0x8000,	/* Basic class for OpenFlow */
		OFPXMC_EXPERIMENTER		= 0xFFFF,	/* Experimenter class */
	};


	/* OXM Flow match field types for OpenFlow basic class. */
	enum oxm_ofb_match_fields {
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
		/* max value */
		OFPXMT_OFB_MAX,
	};


	/* The VLAN id is 12-bits, so we can use the entire 16 bits to indicate
	* special conditions.
	*/
	enum ofp_vlan_id {
		OFPVID_PRESENT = 0x1000, /* Bit that indicate that a VLAN id is set */
		OFPVID_NONE = 0x0000, /* No VLAN id was set. */
	};





	/* Fields to match against flows */
	struct ofp_match {
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
	OFP_ASSERT(sizeof(struct ofp_match) == 8);


	/* The match type indicates the match structure (set of fields that compose the
	* match) in use. The match type is placed in the type field at the beginning
	* of all match structures. The "OpenFlow Extensible Match" type corresponds
	* to OXM TLV format described below and must be supported by all OpenFlow
	* switches. Extensions that define other match types may be published on the
	* ONF wiki. Support for extensions is optional.
	*/
	enum ofp_match_type {
		OFPMT_STANDARD = 0, /* Deprecated. */
		OFPMT_OXM = 1, 		/* OpenFlow Extensible Match */
	};


}; // end of namespace openflow
}; // end of namespace rofl


#endif /* OPENFLOW_COMMON_H_ */
