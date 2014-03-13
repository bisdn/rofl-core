#ifndef __OF1X_MATCH_H__
#define __OF1X_MATCH_H__

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "rofl.h"
#include "../../../common/ternary_fields.h"
#include "../../../common/packet_matches.h"
#include "of1x_utils.h"

//Fwd declarations
union of_packet_matches;

#define OF1X_VLAN_PRESENT_MASK 0x1000
#define OF1X_VLAN_ID_MASK 0x0FFF

/* Defines possible matchings. This is EXPLICITELY copied from openflow.h, to simplify names, avoid collisions and add extensions */
typedef enum{
	/* phy */
	OF1X_MATCH_IN_PORT,		/* Switch input port. */		//required
	OF1X_MATCH_IN_PHY_PORT,		/* Switch physical input port. */
	
	/* metadata */
	OF1X_MATCH_METADATA,		/* Metadata passed between tables. */

	/* eth */
	OF1X_MATCH_ETH_DST,		/* Ethernet destination address. */	//required
	OF1X_MATCH_ETH_SRC,		/* Ethernet source address. */		//required
	OF1X_MATCH_ETH_TYPE,		/* Ethernet frame type. */		//required
	OF1X_MATCH_VLAN_VID,		/* VLAN id. */
	OF1X_MATCH_VLAN_PCP,		/* VLAN priority. */

	/* mpls */
	OF1X_MATCH_MPLS_LABEL,		/* MPLS label. */
	OF1X_MATCH_MPLS_TC,		/* MPLS TC. */
	OF1X_MATCH_MPLS_BOS,		/* MPLS BoS flag. */

	/* arp */
	OF1X_MATCH_ARP_OP,		/* ARP opcode. */
	OF1X_MATCH_ARP_SPA,		/* ARP source IPv4 address. */
	OF1X_MATCH_ARP_TPA,		/* ARP target IPv4 address. */
	OF1X_MATCH_ARP_SHA,		/* ARP source hardware address. */
	OF1X_MATCH_ARP_THA,		/* ARP target hardware address. */

	/* network layer */
	OF1X_MATCH_NW_PROTO,		/* Network layer Ip proto/arp code. OF10 ONLY */	//required
	OF1X_MATCH_NW_SRC,		/* Network layer source address. OF10 ONLY */		//required
	OF1X_MATCH_NW_DST,		/* Network layer destination address. OF10 ONLY */	//required
	
	/* ipv4 */
	OF1X_MATCH_IP_DSCP,		/* IP DSCP (6 bits in ToS field). */
	OF1X_MATCH_IP_ECN,		/* IP ECN (2 bits in ToS field). */
	OF1X_MATCH_IP_PROTO,		/* IP protocol. */			//required
	OF1X_MATCH_IPV4_SRC,		/* IPv4 source address. */		//required
	OF1X_MATCH_IPV4_DST,		/* IPv4 destination address. */		//required

	/* ipv6 */
	OF1X_MATCH_IPV6_SRC,		/* IPv6 source address. */		//required
	OF1X_MATCH_IPV6_DST,		/* IPv6 destination address. */		//required
	OF1X_MATCH_IPV6_FLABEL,		/* IPv6 Flow Label */
	OF1X_MATCH_ICMPV6_TYPE,		/* ICMPv6 type. */
	OF1X_MATCH_ICMPV6_CODE,		/* ICMPv6 code. */
	OF1X_MATCH_IPV6_ND_TARGET,	/* Target address for ND. */
	OF1X_MATCH_IPV6_ND_SLL,		/* Source link-layer for ND. */
	OF1X_MATCH_IPV6_ND_TLL,		/* Target link-layer for ND. */
	OF1X_MATCH_IPV6_EXTHDR,		/* Extension header */

	/* transport */
	OF1X_MATCH_TP_SRC,		/* TCP/UDP source port. OF10 ONLY */	//required
	OF1X_MATCH_TP_DST,		/* TCP/UDP dest port. OF10 ONLY */	//required
	OF1X_MATCH_TCP_SRC,		/* TCP source port. */			//required
	OF1X_MATCH_TCP_DST,		/* TCP destination port. */		//required
	OF1X_MATCH_UDP_SRC,	        /* UDP source port. */			//required
	OF1X_MATCH_UDP_DST,		/* UDP destination port. */		//required
	OF1X_MATCH_SCTP_SRC,		/* SCTP source port. */
	OF1X_MATCH_SCTP_DST,		/* SCTP destination port. */
	OF1X_MATCH_ICMPV4_TYPE,		/* ICMP type. */
	OF1X_MATCH_ICMPV4_CODE,		/* ICMP code. */

	/* other */
	OF1X_MATCH_PBB_ISID,
	OF1X_MATCH_TUNNEL_ID,

	/********************************/
	/**** Extensions out of spec ****/
	/********************************/

	/* PPP/PPPoE related extensions */
	OF1X_MATCH_PPPOE_CODE,		/* PPPoE code */
	OF1X_MATCH_PPPOE_TYPE,		/* PPPoE type */
	OF1X_MATCH_PPPOE_SID,		/* PPPoE session id */
	OF1X_MATCH_PPP_PROT,		/* PPP protocol */
	
	/* GTP related extensions */
	OF1X_MATCH_GTP_MSG_TYPE,	/* GTP message type */
	OF1X_MATCH_GTP_TEID,		/* GTP teid */

	/* max value */
	OF1X_MATCH_MAX,
}of1x_match_type_t;

typedef struct of1x_match{
	
	//Type
	of1x_match_type_t type;

	//Ternary value
	utern_t* value;
	
	//Previous entry
	struct of1x_match* prev;
	
	//Next entry
	struct of1x_match* next;
	
	/* Fast validation flags */
	//Bitmap of required OF versions
	of1x_ver_req_t ver_req; 
	
	//OF1.0 only
	bool has_wildcard;
}of1x_match_t;


/* Match group, using a double-linked-list */
typedef struct of1x_match_group{
	//Double linked list
	of1x_match_t* head;
	of1x_match_t* tail;

	//Num of matches
	unsigned int num_elements;

	/* Fast validation flags */
	//Required OF versions
	of1x_ver_req_t ver_req;
 
	//OF1.0 only
	bool has_wildcard;
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


//METADATA
/**
* @brief Create an METADATA match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_metadata_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value, uint64_t mask);

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
* @brief Create an VLAN_VID match according to 1.2 semantics (13th bit is a flag) 
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
/**
* @brief Create an MPLS_BOS match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_mpls_bos_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);

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

//NW
/**
* @brief Create an NW_PROTO match, OF1.0 ONLY!  
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_nw_proto_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);
/**
* @brief Create an NW_SRC match (IP/ARP), OF1.0 ONLY! 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_nw_src_match(of1x_match_t* prev, of1x_match_t* next, uint32_t value, uint32_t mask);
/**
* @brief Create an NW_DST match (IP/ARP), OF1.0 ONLY! 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_nw_dst_match(of1x_match_t* prev, of1x_match_t* next, uint32_t value, uint32_t mask);

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

//SCTP
/**
* @brief Create an SCTP_SRC match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_sctp_src_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);
/**
* @brief Create an SCTP_DST match 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_sctp_dst_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);

//TP
/**
* @brief Create an TP_SRC match (TCP/UDP), OF1.0 ONLY! 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_tp_src_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);
/**
* @brief Create an TP_DST match (TCP/UDP), OF1.0 ONLY! 
* @ingroup core_of1x 
*/
of1x_match_t* of1x_init_tp_dst_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value);


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
of1x_match_t* of1x_init_ip6_exthdr_match(of1x_match_t* prev, of1x_match_t* next, uint16_t value, uint16_t mask);

//ICMPv6
/**
 * @brief Create an ICMPV6_TYPE match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_icmpv6_type_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);
/**
 * @brief Create an ICMPV6_CODE match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_icmpv6_code_match(of1x_match_t* prev, of1x_match_t* next, uint8_t value);

/*
 * @brief Create an PBB_ISD match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_pbb_isid_match(of1x_match_t* prev, of1x_match_t* next, uint32_t value, uint32_t mask);

/**
 * @brief Create an TUNNEL_ID match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_tunnel_id_match(of1x_match_t* prev, of1x_match_t* next, uint64_t value, uint64_t mask);

//TODO
//Add more here...

/** 
* @brief Destroys whichever match previously created using of1x_init_match_*() 
* @ingroup core_of1x 
*/
void of1x_destroy_match(of1x_match_t* match);

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
bool __of1x_check_match(const packet_matches_t* pkt, of1x_match_t* it);

/*
* Dumping
*/
void __of1x_dump_matches(of1x_match_t* matches, bool sbo);

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_MATCH
