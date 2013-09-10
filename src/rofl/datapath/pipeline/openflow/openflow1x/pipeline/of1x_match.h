#ifndef __OF1X_MATCH_H__
#define __OF1X_MATCH_H__

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "rofl.h"
#include "../../../common/ternary_fields.h"
#include "of1x_packet_matches.h"

//Fwd declarations
union of_packet_matches;

//Useful ETH_TYPE values
#define OF1X_ETH_TYPE_MPLS_UNICAST 0x8847
#define OF1X_ETH_TYPE_MPLS_MULTICAST 0x8848
#define OF1X_ETH_TYPE_IPV4 0x0800
#define OF1X_ETH_TYPE_IPV6 0x86DD
#define OF1X_ETH_TYPE_PPPOE_DISCOVERY 0x8863
#define OF1X_ETH_TYPE_PPPOE_SESSION 0x8864
#define OF1X_ETH_TYPE_ARP 0x0806

//Useful IP_PROTO values
#define OF1X_IP_PROTO_TCP 6
#define OF1X_IP_PROTO_UDP 17
#define OF1X_IP_PROTO_ICMPV4 1
#define OF1X_IP_PROTO_ICMPV6 58

//PPP PROTO values
#define OF1X_PPP_PROTO_IP4 0x0021 
#define OF1X_PPP_PROTO_IP6 0x0057

//Useful UDP destination port values
#define OF1X_UDP_DST_PORT_GTPC 2123
#define OF1X_UDP_DST_PORT_GTPU 2152

/* Defines possible matchings. This is EXPLICITELY copied from openflow.h, to simplify names, avoid collisions and add extensions */
typedef enum{
	OF1X_MATCH_IN_PORT = 0,          /* Switch input port. */                // required
	OF1X_MATCH_IN_PHY_PORT = 1,     /* Switch physical input port. */
	OF1X_MATCH_METADATA = 2,        /* Metadata passed between tables. */

	/* mac */
	OF1X_MATCH_ETH_DST = 3,            /* Ethernet destination address. */        // required
	OF1X_MATCH_ETH_SRC = 4,            /* Ethernet source address. */            // required
	OF1X_MATCH_ETH_TYPE = 5,        /* Ethernet frame type. */                // required
	OF1X_MATCH_VLAN_VID = 6,        /* VLAN id. */
	OF1X_MATCH_VLAN_PCP = 7,        /* VLAN priority. */

	/* ipv4 */
	OF1X_MATCH_IP_DSCP = 8,            /* IP DSCP (6 bits in ToS field). */
	OF1X_MATCH_IP_ECN = 9,            /* IP ECN (2 bits in ToS field). */
	OF1X_MATCH_IP_PROTO = 10,        /* IP protocol. */                        // required
	OF1X_MATCH_IPV4_SRC = 11,        /* IPv4 source address. */                // required
	OF1X_MATCH_IPV4_DST = 12,        /* IPv4 destination address. */            // required

	/* transport */
	OF1X_MATCH_TCP_SRC = 13,        /* TCP source port. */                    // required
	OF1X_MATCH_TCP_DST = 14,        /* TCP destination port. */                // required
	OF1X_MATCH_UDP_SRC = 15,        /* UDP source port. */                    // required
	OF1X_MATCH_UDP_DST = 16,        /* UDP destination port. */                // required
	OF1X_MATCH_SCTP_SRC = 17,        /* SCTP source port. */
	OF1X_MATCH_SCTP_DST = 18,        /* SCTP destination port. */
	OF1X_MATCH_ICMPV4_TYPE = 19,    /* ICMP type. */
	OF1X_MATCH_ICMPV4_CODE = 20,    /* ICMP code. */

	/* arp */
	OF1X_MATCH_ARP_OP = 21,            /* ARP opcode. */
	OF1X_MATCH_ARP_SPA = 22,        /* ARP source IPv4 address. */
	OF1X_MATCH_ARP_TPA = 23,        /* ARP target IPv4 address. */
	OF1X_MATCH_ARP_SHA = 24,        /* ARP source hardware address. */
	OF1X_MATCH_ARP_THA = 25,        /* ARP target hardware address. */

	/* ipv6 */
	OF1X_MATCH_IPV6_SRC = 26,        /* IPv6 source address. */                // required
	OF1X_MATCH_IPV6_DST = 27,        /* IPv6 destination address. */            // required
	OF1X_MATCH_IPV6_FLABEL = 28,    /* IPv6 Flow Label */
	OF1X_MATCH_ICMPV6_TYPE = 29,    /* ICMPv6 type. */
	OF1X_MATCH_ICMPV6_CODE = 30,    /* ICMPv6 code. */
	OF1X_MATCH_IPV6_ND_TARGET = 31,    /* Target address for ND. */
	OF1X_MATCH_IPV6_ND_SLL = 32,    /* Source link-layer for ND. */
	OF1X_MATCH_IPV6_ND_TLL = 33,    /* Target link-layer for ND. */
	OF1X_MATCH_IPV6_EXTHDR = 34,

	/* mpls */
	OF1X_MATCH_MPLS_LABEL = 35,        /* MPLS label. */
	OF1X_MATCH_MPLS_TC = 36,        /* MPLS TC. */

	OF1X_MATCH_PBB_ISID = 37,
	OF1X_MATCH_TUNNEL_ID = 38,


	/********************************/
	/**** Extensions out of spec ****/
	/********************************/
	/* PPP/PPPoE related extensions */
	OF1X_MATCH_PPPOE_CODE = 39,        /* PPPoE code */
	OF1X_MATCH_PPPOE_TYPE = 40,        /* PPPoE type */
	OF1X_MATCH_PPPOE_SID = 41,        /* PPPoE session id */
	OF1X_MATCH_PPP_PROT = 42,        /* PPP protocol */
	
	/* GTP related extensions */
	OF1X_MATCH_GTP_MSG_TYPE = 43,	/* GTP message type */
	OF1X_MATCH_GTP_TEID = 44,		/* GTP teid */

	/* max value */
	OF1X_MATCH_MAX,
} of1x_match_type_t;


struct of1x_match{
	
	//Type
	of1x_match_type_t type;

	//Ternary value
	utern_t* value;
	
	//Previous entry
	struct of1x_match* prev;
	
	//Next entry
	struct of1x_match* next;
};
typedef struct of1x_match of1x_match_t;


/* Match group, using a double-linked-list */
typedef struct{
	//Double linked list
	of1x_match_t* head;
	of1x_match_t* tail;
}of1x_match_group_t;


/*
*
* Initializers per match 
*
*/

//C++ extern C
ROFL_BEGIN_DECLS

//Phy
/**
* @brief Create an PORT_IN match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_port_in_match(of1x_match_t* prev, of1x_match_t* next, uint32_t value);
/**
* @brief Create an PHY_PORT_IN match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_port_in_phy_match(of1x_match_t* prev, of1x_match_t* next, uint32_t value);

/* TODO: add metadata */

//ETHERNET
/**
* @brief Create an ETH_DST match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_eth_dst_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value, uint64_t mask);
/**
* @brief Create an ETH_SRC match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_eth_src_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value, uint64_t mask);
/**
* @brief Create an ETH_TYPE match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_eth_type_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);

//8021.q
/**
* @brief Create an VLAN_VID match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_vlan_vid_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value, uint16_t mask);
/**
* @brief Create an VLAN_PCP match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_vlan_pcp_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);

//MPLS
/**
* @brief Create an MPLS_LABEL match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_mpls_label_match(of1x_match_t* prev, of1x_match_t* next, uint32_t value);
/**
* @brief Create an MPLS_TC match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_mpls_tc_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);

//ARP
/**
* @brief Create an ARP_OPCODE match
* @ingroup core_of1x
*/
of1x_match_t* of1x_init_arp_opcode_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);
/**
* @brief Create an ARP_THA match
* @ingroup core_of1x
*/
of1x_match_t* of1x_init_arp_tha_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value, uint64_t mask);
/**
* @brief Create an ARP_SHA match
* @ingroup core_of1x
*/
of1x_match_t* of1x_init_arp_sha_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value, uint64_t mask);
/**
* @brief Create an ARP_TPA match
* @ingroup core_of1x
*/
of1x_match_t* of1x_init_arp_tpa_match(of1x_match_t* prev, of1x_match_t* next, uint32_t value, uint32_t mask);
/**
* @brief Create an ARP_SPA match
* @ingroup core_of1x
*/
of1x_match_t* of1x_init_arp_spa_match(of1x_match_t* prev, of1x_match_t* next, uint32_t value, uint32_t mask);

//PPPoE
/**
* @brief Create an PPPOE_CODE match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_pppoe_code_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);
/**
* @brief Create an PPPOE_TYPE match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_pppoe_type_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);
/**
* @brief Create an PPPOE_SESSION match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_pppoe_session_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);

//PPP
/**
* @brief Create an PPP_PROTO match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_ppp_prot_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);

//GTP
/**
* @brief Create an PPP_PROTO match
* @ingroup core_of1x
*/
of1x_match_t* of1x_init_gtp_msg_type_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);
/**
* @brief Create an PPP_PROTO match
* @ingroup core_of1x
*/
of1x_match_t* of1x_init_gtp_teid_match(of1x_match_t* prev, of1x_match_t* next, uint32_t value, uint32_t mask);


//IP
/**
* @brief Create an IP_PROTO match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_ip_proto_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);
/**
* @brief Create an IPC_ECN match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_ip_ecn_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);
/**
* @brief Create an IP_DSCP match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_ip_dscp_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);

//IPv4
/**
* @brief Create an IP4_SRC match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_ip4_src_match(of1x_match_t* prev, of1x_match_t* next, uint32_t value, uint32_t mask);
/**
* @brief Create an IP4_DST match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_ip4_dst_match(of1x_match_t* prev, of1x_match_t* next, uint32_t value, uint32_t mask);

//TCP
/**
* @brief Create an TCP_SRC match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_tcp_src_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);
/**
* @brief Create an TCP_DST match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_tcp_dst_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);

//UDP
/**
* @brief Create an UDP_SRC match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_udp_src_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);
/**
* @brief Create an UDP_DST match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_udp_dst_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);

//ICMPv4
/**
* @brief Create an ICMPv4_TYPE match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_icmpv4_type_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);
/**
* @brief Create an ICMPv4_CODE match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_icmpv4_code_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);

//IPv6
/**
 * @brief Create an IP6_SRC match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_ip6_src_match(of1x_match_t* prev, of1x_match_t* next, uint128__t value, uint128__t mask);
/**
 * @brief Create an IP6_DST match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_ip6_dst_match(of1x_match_t* prev, of1x_match_t* next, uint128__t value, uint128__t mask);
/**
 * @brief Create an IP6_FLABEL match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_ip6_flabel_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value);
/**
 * @brief Create an IP6_ND_TARGET match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_ip6_nd_target_match(of1x_match_t* prev, of1x_match_t* next, uint128__t value);
/**
 * @brief Create an IP6_ND_SLL match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_ip6_nd_sll_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value);
/**
 * @brief Create an IP6_ND_TLL match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_ip6_nd_tll_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value);
/**
 * @brief Create an IP6_EXTHDR match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_ip6_exthdr_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value, uint64_t mask);

//ICMv6
/**
 * @brief Create an ICMPV6_TYPE match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_icmpv6_type_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value);
/**
 * @brief Create an ICMPV6_CODE match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_icmpv6_code_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value);


//TODO
//Add more here...

/** 
* @brief Destroys whichever match previously created using of1x_init_match_*() 
* @ingroup core_of1x 
*/
void of1x_destroy_match(of1x_match_t* match);





#if 0
/* match group */
void of1x_init_match_group(of1x_match_group_t* group);
void of1x_destroy_match_group(of1x_match_group_t* group);
void of1x_match_group_push_match(of1x_match_group_t* group, of1x_match_t* match);
#endif

/* match group */
void __of1x_init_match_group(of1x_match_group_t* group);
void __of1x_destroy_match_group(of1x_match_group_t* group);
void __of1x_match_group_push_back(of1x_match_group_t* group, of1x_match_t* match);

/* Push match at the end of the match */
rofl_result_t __of1x_add_match(of1x_match_t* root_match, of1x_match_t* add_match);

/* 
* Single Match copy
* TODO: deprecate this in favour of match_group
*/
of1x_match_t* __of1x_copy_match(of1x_match_t* match);

/* 
* Whole (linked list) Match copy
* TODO: deprecate this in favour of match_group
*/
of1x_match_t* __of1x_copy_matches(of1x_match_t* matches);


/* 
* Get alike match 
*/ 
of1x_match_t* __of1x_get_alike_match(of1x_match_t* match1, of1x_match_t* match2);

/*
* Matching 
*/
bool __of1x_equal_matches(of1x_match_t* match1, of1x_match_t* match2);
bool __of1x_is_submatch(of1x_match_t* sub_match, of1x_match_t* match);
bool __of1x_check_match(const of1x_packet_matches_t* pkt, of1x_match_t* it);

/*
* Dumping
*/
void of1x_dump_packet_matches(union of_packet_matches *const pkt_matches);

void of1x_dump_matches(of1x_match_t* matches);
void of1x_full_dump_matches(of1x_match_t* matches);

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_MATCH
