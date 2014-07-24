#ifndef __OF1X_MATCH_H__
#define __OF1X_MATCH_H__

#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "rofl.h"
#include "../../../common/ternary_fields.h"
#include "../../../common/packet_matches.h"
#include "of1x_utils.h"

/**
* @file of1x_match.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief OpenFlow v1.0, 1.2 and 1.3.2 matches
* 
* Note regarding endianness:
* Conforming the convention that the pipeline works in Network Byte Order
* the matches need to to be initialized in NBO (Big Endian).
* This applies to the values comming from the packet (eth_src, eth_dst, ...) 
* and NOT to the ones that are external to it:
*  - port_in
*  - port_phy_in
*  - metadata
* 
* There is an special alignment for non complete values as
*  - mac addresses ( 6 bytes)
*  - vlan vid      (12 bits )
*  - mpls label    (20 bits )
*  - pbb isid      ( 3 bytes)
* More information on these alignments can be found in the
* pipeline general documentation
*/

//Fwd declarations
union of_packet_matches;

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

	/* CAPWAP related extensions */
	OF1X_MATCH_CAPWAP_WBID,		/* CAPWAP WBID */
	OF1X_MATCH_CAPWAP_RID,		/* CAPWAP RID */
	OF1X_MATCH_CAPWAP_FLAGS,	/* CAPWAP FLAGS */

	/* WLAN related extensions */
	OF1X_MATCH_WLAN_FC,			/* WLAN fc */
	OF1X_MATCH_WLAN_TYPE,		/* WLAN type */
	OF1X_MATCH_WLAN_SUBTYPE,	/* WLAN subtype */
	OF1X_MATCH_WLAN_DIRECTION,	/* WLAN direction */
	OF1X_MATCH_WLAN_ADDRESS_1,	/* WLAN address 1 */
	OF1X_MATCH_WLAN_ADDRESS_2,	/* WLAN address 2 */
	OF1X_MATCH_WLAN_ADDRESS_3,	/* WLAN address 3 */

	/* max value */
	OF1X_MATCH_MAX,
}of1x_match_type_t;

//Specific flags for vlans
enum of1x_vlan_present{
	OF1X_MATCH_VLAN_NONE=0,
	OF1X_MATCH_VLAN_ANY=1,
	OF1X_MATCH_VLAN_SPECIFIC=2,
};

typedef struct of1x_match{
	
	//Type
	of1x_match_type_t type;

	//Ternary value
	utern_t* __tern;
	
	//Previous entry
	struct of1x_match* prev;
	
	//Next entry
	struct of1x_match* next;
	
	/* Fast validation flags */
	//Bitmap of required OF versions
	of1x_ver_req_t ver_req;
	
	//VLAN only (blame OF spec)
	enum of1x_vlan_present vlan_present;
	
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
	
	//bitmaps of matches and wilcards
	bitmap128_t match_bm;
	bitmap128_t wildcard_bm; 
	bitmap128_t of10_wildcard_bm; //OF1.0 only
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
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_port_in_match(uint32_t value);
/**
* @brief Create an PHY_PORT_IN match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_port_in_phy_match(uint32_t value);


//METADATA
/**
* @brief Create an METADATA match 
* @ingroup core_of1x 
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_metadata_match(uint64_t value, uint64_t mask);

//ETHERNET
/**
* @brief Create an ETH_DST match 
* @ingroup core_of1x 
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_eth_dst_match(uint64_t value, uint64_t mask);
/**
* @brief Create an ETH_SRC match 
* @ingroup core_of1x 
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_eth_src_match(uint64_t value, uint64_t mask);
/**
* @brief Create an ETH_TYPE match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_eth_type_match(uint16_t value);

//8021.q
/**
* @brief Create an VLAN_VID match according to 1.2 semantics (13th bit is a flag) 
* @ingroup core_of1x 
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_vlan_vid_match(uint16_t value, uint16_t mask, enum of1x_vlan_present vlan_present);

/**
* @brief Create an VLAN_PCP match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_vlan_pcp_match(uint8_t value);

//MPLS
/**
* @brief Create an MPLS_LABEL match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_mpls_label_match(uint32_t value);
/**
* @brief Create an MPLS_TC match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_mpls_tc_match(uint8_t value);
/**
* @brief Create an MPLS_BOS match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_mpls_bos_match(uint8_t value);

//ARP
/**
* @brief Create an ARP_OPCODE match
* @ingroup core_of1x
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_arp_opcode_match(uint16_t value);
/**
* @brief Create an ARP_THA match
* @ingroup core_of1x
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_arp_tha_match(uint64_t value, uint64_t mask);
/**
* @brief Create an ARP_SHA match
* @ingroup core_of1x
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_arp_sha_match(uint64_t value, uint64_t mask);
/**
* @brief Create an ARP_TPA match
* @ingroup core_of1x
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_arp_tpa_match(uint32_t value, uint32_t mask);
/**
* @brief Create an ARP_SPA match
* @ingroup core_of1x
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_arp_spa_match(uint32_t value, uint32_t mask);

//PPPoE
/**
* @brief Create an PPPOE_CODE match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_pppoe_code_match(uint8_t value);
/**
* @brief Create an PPPOE_TYPE match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_pppoe_type_match(uint8_t value);
/**
* @brief Create an PPPOE_SESSION match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_pppoe_session_match(uint16_t value);

//PPP
/**
* @brief Create an PPP_PROTO match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_ppp_prot_match(uint16_t value);

//GTP
/**
* @brief Create an PPP_PROTO match
* @ingroup core_of1x
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_gtp_msg_type_match(uint8_t value);
/**
* @brief Create an PPP_PROTO match
* @ingroup core_of1x
* @warning parameters value and mask be in Host Byte Order
*/
of1x_match_t* of1x_init_gtp_teid_match(uint32_t value, uint32_t mask);

//CAPWAP
/**
* @brief Create a CAPWAP_WBID match
* @ingroup core_of1x
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_capwap_wbid_match(uint8_t value, uint8_t mask);
/**
* @brief Create a CAPWAP_RID match
* @ingroup core_of1x
* @warning parameters value and mask be in Host Byte Order
*/
of1x_match_t* of1x_init_capwap_rid_match(uint8_t value, uint8_t mask);
/**
* @brief Create a CAPWAP_FLAGS match
* @ingroup core_of1x
* @warning parameters value and mask be in Host Byte Order
*/
of1x_match_t* of1x_init_capwap_flags_match(uint16_t value, uint16_t mask);

//WLAN
/**
* @brief Create a WLAN_FC match
* @ingroup core_of1x
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_wlan_fc_match(uint16_t value, uint16_t mask);
/**
* @brief Create a WLAN_TYPE match
* @ingroup core_of1x
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_wlan_type_match(uint8_t value, uint8_t mask);
/**
* @brief Create a WLAN_SUBTYPE match
* @ingroup core_of1x
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_wlan_subtype_match(uint8_t value, uint8_t mask);
/**
* @brief Create a WLAN_DIRECTION match
* @ingroup core_of1x
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_wlan_direction_match(uint8_t value, uint8_t mask);
/**
* @brief Create an WLAN_ADDRESS_1 match
* @ingroup core_of1x
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_wlan_address_1_match(uint64_t value, uint64_t mask);
/**
* @brief Create an WLAN_ADDRESS_2 match
* @ingroup core_of1x
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_wlan_address_2_match(uint64_t value, uint64_t mask);
/**
* @brief Create an WLAN_ADDRESS_3 match
* @ingroup core_of1x
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_wlan_address_3_match(uint64_t value, uint64_t mask);

//NW
/**
* @brief Create an NW_PROTO match, OF1.0 ONLY!  
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_nw_proto_match(uint8_t value);
/**
* @brief Create an NW_SRC match (IP/ARP), OF1.0 ONLY! 
* @ingroup core_of1x 
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_nw_src_match(uint32_t value, uint32_t mask);
/**
* @brief Create an NW_DST match (IP/ARP), OF1.0 ONLY! 
* @ingroup core_of1x 
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_nw_dst_match(uint32_t value, uint32_t mask);

//IP
/**
* @brief Create an IP_PROTO match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_ip_proto_match(uint8_t value);
/**
* @brief Create an IPC_ECN match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_ip_ecn_match(uint8_t value);
/**
* @brief Create an IP_DSCP match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_ip_dscp_match(uint8_t value);

//IPv4
/**
* @brief Create an IP4_SRC match 
* @ingroup core_of1x 
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_ip4_src_match(uint32_t value, uint32_t mask);
/**
* @brief Create an IP4_DST match 
* @ingroup core_of1x 
* @warning parameters value and mask must be in Host Byte Order
*/
of1x_match_t* of1x_init_ip4_dst_match(uint32_t value, uint32_t mask);

//TCP
/**
* @brief Create an TCP_SRC match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_tcp_src_match(uint16_t value);
/**
* @brief Create an TCP_DST match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_tcp_dst_match(uint16_t value);

//UDP
/**
* @brief Create an UDP_SRC match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_udp_src_match(uint16_t value);
/**
* @brief Create an UDP_DST match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_udp_dst_match(uint16_t value);

//SCTP
/**
* @brief Create an SCTP_SRC match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_sctp_src_match(uint16_t value);
/**
* @brief Create an SCTP_DST match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_sctp_dst_match(uint16_t value);

//TP
/**
* @brief Create an TP_SRC match (TCP/UDP), OF1.0 ONLY! 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_tp_src_match(uint16_t value);
/**
* @brief Create an TP_DST match (TCP/UDP), OF1.0 ONLY! 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_tp_dst_match(uint16_t value);


//ICMPv4
/**
* @brief Create an ICMPv4_TYPE match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_icmpv4_type_match(uint8_t value);
/**
* @brief Create an ICMPv4_CODE match 
* @ingroup core_of1x 
* @warning parameter value must be in Host Byte Order
*/
of1x_match_t* of1x_init_icmpv4_code_match(uint8_t value);

//IPv6
/**
 * @brief Create an IP6_SRC match
 * @ingroup core_of1x
 * @warning parameters value and mask be in Host Byte Order
 */
of1x_match_t* of1x_init_ip6_src_match(uint128__t value, uint128__t mask);
/**
 * @brief Create an IP6_DST match
 * @ingroup core_of1x
 * @warning parameters value and mask must be in Host Byte Order
 */
of1x_match_t* of1x_init_ip6_dst_match(uint128__t value, uint128__t mask);
/**
 * @brief Create an IP6_FLABEL match
 * @ingroup core_of1x
 * @warning parameter value must be in Host Byte Order
 */
of1x_match_t* of1x_init_ip6_flabel_match(uint32_t value, uint32_t mask);
/**
 * @brief Create an IP6_ND_TARGET match
 * @ingroup core_of1x
 * @warning parameter value must be in Host Byte Order
 */
of1x_match_t* of1x_init_ip6_nd_target_match(uint128__t value);
/**
 * @brief Create an IP6_ND_SLL match
 * @ingroup core_of1x
 * @warning parameter value must be in Host Byte Order
 */
of1x_match_t* of1x_init_ip6_nd_sll_match(uint64_t value);
/**
 * @brief Create an IP6_ND_TLL match
 * @ingroup core_of1x
 * @warning parameter value must be in Host Byte Order
 */
of1x_match_t* of1x_init_ip6_nd_tll_match(uint64_t value);
/**
 * @brief Create an IP6_EXTHDR match
 * @ingroup core_of1x
 * @warning parameters value and mask must be in Host Byte Order
 */
of1x_match_t* of1x_init_ip6_exthdr_match(uint16_t value, uint16_t mask);

//ICMPv6
/**
 * @brief Create an ICMPV6_TYPE match
 * @ingroup core_of1x
 * @warning parameter value must be in Host Byte Order
 */
of1x_match_t* of1x_init_icmpv6_type_match(uint8_t value);
/**
 * @brief Create an ICMPV6_CODE match
 * @ingroup core_of1x
 * @warning parameter value must be in Host Byte Order
 */
of1x_match_t* of1x_init_icmpv6_code_match(uint8_t value);

/*
 * @brief Create an PBB_ISD match
 * @ingroup core_of1x
 */
of1x_match_t* of1x_init_pbb_isid_match(uint32_t value, uint32_t mask);

/**
 * @brief Create an TUNNEL_ID match
 * @ingroup core_of1x
 * @warning parameters value and mask must be in Host Byte Order
 */
of1x_match_t* of1x_init_tunnel_id_match(uint64_t value, uint64_t mask);

//TODO
//Add more here...

/** 
* @brief Destroys whichever match previously created using of1x_init_match_*() 
* @ingroup core_of1x 
*/
void of1x_destroy_match(of1x_match_t* match);

//
//Getters for values 
//

//8 bit
static inline 
uint8_t __of1x_get_match_val8(const of1x_match_t* match, bool get_mask, bool raw_nbo){

	wrap_uint_t* wrap;
	
	if(get_mask)
		wrap = &match->__tern->mask; 
	else
		wrap = &match->__tern->value; 

	if(raw_nbo)
		return wrap->u8;

	switch(match->type){
		case OF1X_MATCH_VLAN_PCP:
			return OF1X_VLAN_PCP_VALUE(wrap->u8);
		case OF1X_MATCH_MPLS_TC:
			return OF1X_MPLS_TC_VALUE(wrap->u8);
		case OF1X_MATCH_MPLS_BOS:
		case OF1X_MATCH_NW_PROTO:
		case OF1X_MATCH_IP_PROTO:
		case OF1X_MATCH_IP_ECN:
		case OF1X_MATCH_ICMPV4_TYPE:
		case OF1X_MATCH_ICMPV4_CODE:
		case OF1X_MATCH_ICMPV6_TYPE:
		case OF1X_MATCH_ICMPV6_CODE:
		case OF1X_MATCH_PPPOE_TYPE:
		case OF1X_MATCH_PPPOE_CODE:
		case OF1X_MATCH_GTP_MSG_TYPE:
			return wrap->u8;
		case OF1X_MATCH_IP_DSCP:
			return OF1X_IP_DSCP_VALUE(wrap->u8);
		default:{
			//ROFL_PIPELINE_ERR("%s: Match type %u not found\n",__func__,match->type);
			assert(0);
			return 0x0;
		}
	}
}

/**
* @ingroup core_of1x 
* Retrieve the match value for 8 bit values (or less) in HOST BYTE ORDER
*
* @retval The value in host byte order 
*/
static inline 
uint8_t of1x_get_match_value8(const of1x_match_t* match){
	return __of1x_get_match_val8(match, false, false);
}
	

//16 bit
static inline 
uint16_t __of1x_get_match_val16(const of1x_match_t* match, bool get_mask, bool raw_nbo){

	wrap_uint_t* wrap;
	
	if(get_mask)
		wrap = &match->__tern->mask; 
	else
		wrap = &match->__tern->value; 


	if(raw_nbo)
		return wrap->u16;	

	switch(match->type){
		case OF1X_MATCH_ETH_TYPE:
		case OF1X_MATCH_VLAN_VID:
		case OF1X_MATCH_ARP_OP:
		case OF1X_MATCH_TCP_SRC:
		case OF1X_MATCH_TCP_DST:
		case OF1X_MATCH_UDP_SRC:
		case OF1X_MATCH_UDP_DST:
		case OF1X_MATCH_SCTP_SRC:
		case OF1X_MATCH_SCTP_DST:
		case OF1X_MATCH_TP_SRC:
		case OF1X_MATCH_TP_DST:
		case OF1X_MATCH_PPPOE_SID:
		case OF1X_MATCH_PPP_PROT:
			return NTOHB16(wrap->u16);
		case OF1X_MATCH_IPV6_EXTHDR:
		default:{
			//ROFL_PIPELINE_ERR("%s: Match type %u not found\n",__func__,match->type);
			assert(0);
			return 0x0;
		}
	}
}
/**
* @ingroup core_of1x 
* Retrieve the match value for 16 bit values (or less) in HOST BYTE ORDER
*
* @retval The value in host byte order 
*/
static inline 
uint16_t of1x_get_match_value16(const of1x_match_t* match){
	return __of1x_get_match_val16(match, false, false);
}

//32 bit
static inline 
uint32_t __of1x_get_match_val32(const of1x_match_t* match, bool get_mask, bool raw_nbo){
	
	wrap_uint_t* wrap;
	
	if(get_mask)
		wrap = &match->__tern->mask; 
	else
		wrap = &match->__tern->value; 


	if(raw_nbo)
		return wrap->u32;	

	switch(match->type){
		case OF1X_MATCH_IN_PORT:
		case OF1X_MATCH_IN_PHY_PORT:
			return wrap->u32;
		case OF1X_MATCH_MPLS_LABEL:
			return OF1X_MPLS_LABEL_VALUE(NTOHB32(wrap->u32));
		case OF1X_MATCH_ARP_TPA:
		case OF1X_MATCH_ARP_SPA:
		case OF1X_MATCH_NW_SRC:
		case OF1X_MATCH_NW_DST:
		case OF1X_MATCH_IPV4_SRC:
		case OF1X_MATCH_IPV4_DST:
		case OF1X_MATCH_GTP_TEID:
		case OF1X_MATCH_PBB_ISID:
			return NTOHB32(wrap->u32);
		case OF1X_MATCH_IPV6_FLABEL:
			return OF1X_IP6_FLABEL_VALUE(NTOHB32(wrap->u32));
		default:{
			//ROFL_PIPELINE_ERR("%s: Match type %u not found\n",__func__,match->type);
			assert(0);
			return 0x0;
		}
	}
	
}
/**
* @ingroup core_of1x 
* Retrieve the match value for 32 bit values (or less) in HOST BYTE ORDER
*
* @retval The value in host byte order 
*/

static inline 
uint32_t of1x_get_match_value32(const of1x_match_t* match){
	return __of1x_get_match_val32(match, false, false);
}	

//64 bit
static inline 
uint64_t __of1x_get_match_val64(const of1x_match_t* match, bool get_mask, bool raw_nbo){
	
	wrap_uint_t* wrap;
	
	if(get_mask)
		wrap = &match->__tern->mask; 
	else
		wrap = &match->__tern->value; 


	if(raw_nbo)
		return wrap->u64;	

	switch(match->type){
		case OF1X_MATCH_METADATA:
		case OF1X_MATCH_TUNNEL_ID:
			return wrap->u64;
			break;
		case OF1X_MATCH_ETH_DST:
		case OF1X_MATCH_ETH_SRC:
		case OF1X_MATCH_ARP_THA:
		case OF1X_MATCH_ARP_SHA:
		case OF1X_MATCH_IPV6_ND_SLL:
		case OF1X_MATCH_IPV6_ND_TLL:
			return OF1X_MAC_VALUE(NTOHB64(wrap->u64));
			break;
		default:{
			//ROFL_PIPELINE_ERR("%s: Match type %u not found\n",__func__,match->type);
			assert(0);
			return 0x0;
		}
	}
}
/**
* @ingroup core_of1x 
* Retrieve the match value for 64 bit values (or less) in HOST BYTE ORDER
*
* @retval The value in host byte order 
*/
static inline 
uint64_t of1x_get_match_value64(const of1x_match_t* match){
	return __of1x_get_match_val64(match, false, false);
}

//128 bit
static inline 
uint128__t __of1x_get_match_val128(const of1x_match_t* match, bool get_mask, bool raw_nbo){
	uint128__t tmp;
	wrap_uint_t* wrap;
	
	if(get_mask)
		wrap = &match->__tern->mask; 
	else
		wrap = &match->__tern->value; 

	if(raw_nbo)
		return wrap->u128;

	switch(match->type){
		case OF1X_MATCH_IPV6_SRC:
		case OF1X_MATCH_IPV6_DST:
		case OF1X_MATCH_IPV6_ND_TARGET:
			tmp = wrap->u128;
			NTOHB128(tmp);
			return tmp;
		default:{
			//ROFL_PIPELINE_ERR("%s: Match type %u not found\n",__func__,match->type);
			assert(0);
			return tmp;
		}
	}
}

/**
* @ingroup core_of1x 
* Retrieve the match value for 128 bit values (or less) in HOST BYTE ORDER
*
* @retval The value in host byte order 
*/
static inline 
uint128__t of1x_get_match_value128(const of1x_match_t* match){
	return __of1x_get_match_val128(match,  false, false);
}

//
//Getters for masks
//

/**
* @ingroup core_of1x 
* Retrieve the match mask value for 8 bit values (or less) in HOST BYTE ORDER
*
* @retval The value of the mask in host byte order 
*/
static inline 
uint8_t of1x_get_match_mask8(const of1x_match_t* match){
	return __of1x_get_match_val8(match, true, false);
}

/**
* @ingroup core_of1x 
* Retrieve the match mask value for 16 bit values (or less) in HOST BYTE ORDER
*
* @retval The value of the mask in host byte order 
*/
static inline 
uint16_t of1x_get_match_mask16(const of1x_match_t* match){
	return __of1x_get_match_val16(match, true, false);
}

/**
* @ingroup core_of1x 
* Retrieve the match mask value for 32 bit values (or less) in HOST BYTE ORDER
*
* @retval The value of the mask in host byte order 
*/
static inline 
uint32_t of1x_get_match_mask32(const of1x_match_t* match){
	return __of1x_get_match_val32(match, true, false);
}

/**
* @ingroup core_of1x 
* Retrieve the match mask value for 64 bit values (or less) in HOST BYTE ORDER
*
* @retval The value of the mask in host byte order 
*/
static inline 
uint64_t of1x_get_match_mask64(const of1x_match_t* match){
	return __of1x_get_match_val64(match, true, false);
}

/**
* @ingroup core_of1x 
* Retrieve the match mask value for 128 bit values (or less) in HOST BYTE ORDER
*
* @retval The value of the mask in host byte order 
*/
static inline 
uint128__t of1x_get_match_mask128(const of1x_match_t* match){
	return __of1x_get_match_val128(match, true, false);
}

//
// End of getters
//

/* match group */
void __of1x_init_match_group(of1x_match_group_t* group);
void __of1x_destroy_match_group(of1x_match_group_t* group);
void __of1x_match_group_push_back(of1x_match_group_t* group, of1x_match_t* match);

/* Push match at the end of the match */
rofl_result_t __of1x_add_match(of1x_match_t* root_match, of1x_match_t* add_match);

/* 
* Whole (linked list) Match copy
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


/*
* OF1.0 specific behaviour for wildcard
*/
static inline bool __of10_is_wildcard(of1x_match_group_t* matches){
	return !bitmap128_is_empty(&matches->of10_wildcard_bm);
}

/*
* Dumping
*/
void __of1x_dump_matches(of1x_match_t* matches, bool raw_nbo);

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_MATCH
