/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __PACKET_MATCHES_H__
#define __PACKET_MATCHES_H__

#include <inttypes.h> 
#include <string.h> 
#include "rofl.h"
#include "ternary_fields.h"
#include "../openflow/openflow1x/pipeline/of1x_utils.h"

/**
* @file packet_matches.h
* @author Marc Sune<marc.sune (at) bisdn.de>
* 
* @brief Utils to dump the "packet header values"
*
* @warning The pipeline uses internally Network Byte Order (NBO) for storing the values. In addition
* some of the values, for performance reasons, present a special alignment (byte-wise and/or intra-byte). For
* this reason packet_matches_get_XXX_value(pkt_matches) inline functions are defined to ease the access to these values. It is
* strongly discouraged to access directly the values outside from rofl-pipeline. Please refer to @ref endianness
* for more information about the special alignment within the pipeline. 
*/
struct datapacket;

/* 
* Packet matching values
*/
typedef struct packet_matches{

	//Packet size
	uint32_t __pkt_size_bytes;	/* Packet size in bytes */

	//Ports
	uint32_t __port_in;		/* Switch input port. */
	uint32_t __phy_port_in;		/* Switch physical input port. */
	
	//Associated metadata
	uint64_t __metadata;		/* Metadata passed between tables. */
 
	//802
	uint64_t __eth_dst;		/* Ethernet destination address. */
	uint64_t __eth_src;		/* Ethernet source address. */
	uint16_t __eth_type;		/* Ethernet frame type (WARNING: inner payload). */
	
	//802.1q VLAN outermost tag
	bool __has_vlan;		/* VLAN flag */
	uint16_t __vlan_vid;		/* VLAN id. */
	uint8_t __vlan_pcp;		/* VLAN PCP. */

	//ARP
	uint16_t __arp_opcode;		/* ARP opcode */
	uint64_t __arp_sha;		/* ARP source hardware address */
	uint32_t __arp_spa;		/* ARP source protocol address */
	uint64_t __arp_tha;		/* ARP target hardware address */
	uint32_t __arp_tpa;		/* ARP target protocol address */

	//IP
	uint8_t __ip_proto;		/* IP protocol. */
	uint8_t __ip_dscp;		/* IP DSCP (6 bits in ToS field). */
	uint8_t __ip_ecn;			/* IP ECN (2 bits in ToS field). */
	
	//IPv4
	uint32_t __ipv4_src;		/* IPv4 source address. */
	uint32_t __ipv4_dst;		/* IPv4 destination address. */

	//TCP
	uint16_t __tcp_src;		/* TCP source port. */
	uint16_t __tcp_dst;		/* TCP destination port. */

	//UDP
	uint16_t __udp_src;		/* UDP source port. */
	uint16_t __udp_dst;		/* UDP destination port. */

	//SCTP
	uint16_t __sctp_src;		/* SCTP source port. */
	uint16_t __sctp_dst;		/* SCTP destination port. */


	//ICMPv4
	uint8_t __icmpv4_type;		/* ICMP type. */
	uint8_t __icmpv4_code;		/* ICMP code. */

	//MPLS-outermost label 
	uint32_t __mpls_label;		/* MPLS label. */
	uint8_t __mpls_tc;		/* MPLS TC. */
	bool __mpls_bos;		/* MPLS BoS. */


	//IPv6
	uint128__t __ipv6_src;		/* IPv6 source address */
	uint128__t __ipv6_dst;		/* IPv6 source address */
	uint64_t __ipv6_flabel;		/* IPv6 flow label */
	uint128__t __ipv6_nd_target;	/* IPv6 Neighbor discovery protocol target */
	uint64_t __ipv6_nd_sll;		/* IPv6 Neighbor discovery protocol source link level */
	uint64_t __ipv6_nd_tll;		/* IPv6 Neighbor discovery protocol target link level */
	uint16_t __ipv6_exthdr;		/* IPv6 extension pseudo header */
	
	//ICMPv6 
	uint8_t __icmpv6_code;		/* ICMPv6 type */
	uint8_t __icmpv6_type;		/* ICMPv6 code */

	//PBB
	uint32_t __pbb_isid;		/* PBB_ISID code */
	
	//Tunnel id
	uint64_t __tunnel_id;		/* Tunnel id*/

	/*
	* Extensions
	*/

	//PPPoE related extensions
	uint8_t __pppoe_code;		/* PPPoE code */
	uint8_t __pppoe_type;		/* PPPoE type */
	uint16_t __pppoe_sid;		/* PPPoE session id */
	
	//PPP related extensions
	uint16_t __ppp_proto;		/* PPPoE session id */
	
	//GTP related extensions
	uint8_t __gtp_msg_type;		/* GTP message type */
	uint32_t __gtp_teid;		/* GTP teid */

}packet_matches_t;


//C++ extern C
ROFL_BEGIN_DECLS

/**
 * @brief Dump the values of packet (header values)  
 * @ingroup core_of1x
 * @param raw_nbo Show values in the pipeline internal byte order (NBO). Warning: some values are intentionally unaligned. 
 */
void dump_packet_matches(struct datapacket *const pkt, bool raw_nbo);

/**
 * @brief Fill in packet_matches (for PKT_INs)
 * @ingroup core_of1x
 */
void fill_packet_matches(struct datapacket *const pkt, packet_matches_t* pkt_matches);


/**
* @brief Get the packet match PACKET_LENGTH value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint32_t packet_matches_get_size_bytes(packet_matches_t *const pkt_matches){
	return pkt_matches->__pkt_size_bytes;
};
//Phy
/**
* @brief Get the packet match PORT_IN value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint32_t packet_matches_get_port_in_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__port_in;
};
/**
* @brief Get the packet match PHY_PORT_IN value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint32_t packet_matches_get_phy_port_in_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__phy_port_in;
};


//METADATA
/**
* @brief Get the packet match METADATA value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint64_t packet_matches_get_metadata_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__metadata;
};

//ETHERNET
/**
* @brief Get the packet match ETH_DST value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint64_t packet_matches_get_eth_dst_value(packet_matches_t *const pkt_matches){
	return OF1X_MAC_VALUE(NTOHB64(pkt_matches->__eth_dst));
};
/**
* @brief Get the packet match ETH_SRC value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint64_t packet_matches_get_eth_src_value(packet_matches_t *const pkt_matches){
	return OF1X_MAC_VALUE(NTOHB64(pkt_matches->__eth_src));
};
/**
* @brief Get the packet match ETH_TYPE value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint16_t packet_matches_get_eth_type_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__eth_type);
};

//8021.q
/**
* @brief Get flag has vlan 
* @ingroup core_of1x 
*/
static inline
bool packet_matches_has_vlan(packet_matches_t *const pkt_matches){
	return pkt_matches->__has_vlan;
};
/**
* @brief Get the packet match VLAN_VID value in HOST BYTE ORDER according to 1.2 semantics (13th bit is a flag) 
* @ingroup core_of1x 
*/
static inline
uint16_t packet_matches_get_vlan_vid_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__vlan_vid);
};

/**
* @brief Get the packet match VLAN_PCP value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint8_t packet_matches_get_vlan_pcp_value(packet_matches_t *const pkt_matches){
	return OF1X_VLAN_PCP_VALUE(pkt_matches->__vlan_pcp);
};

//MPLS
/**
* @brief Get the packet match MPLS_LABEL value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint32_t packet_matches_get_mpls_label_value(packet_matches_t *const pkt_matches){
	return OF1X_MPLS_LABEL_VALUE(NTOHB32(pkt_matches->__mpls_label));
};
/**
* @brief Get the packet match MPLS_TC value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint8_t packet_matches_get_mpls_tc_value(packet_matches_t *const pkt_matches){
	return OF1X_MPLS_TC_VALUE(pkt_matches->__mpls_tc);
};
/**
* @brief Get the packet match MPLS_BOS value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
bool packet_matches_get_mpls_bos_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__mpls_bos;
};

//ARP
/**
* @brief Get the packet match ARP_OPCODE value in HOST BYTE ORDER
* @ingroup core_of1x
*/
static inline
uint16_t packet_matches_get_arp_opcode_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__arp_opcode);
};
/**
* @brief Get the packet match ARP_THA value in HOST BYTE ORDER
* @ingroup core_of1x
*/
static inline
uint64_t packet_matches_get_arp_tha_value(packet_matches_t *const pkt_matches){
	return OF1X_MAC_VALUE(NTOHB64(pkt_matches->__arp_tha));
};
/**
* @brief Get the packet match ARP_SHA value in HOST BYTE ORDER
* @ingroup core_of1x
*/
static inline
uint64_t packet_matches_get_arp_sha_value(packet_matches_t *const pkt_matches){
	return OF1X_MAC_VALUE(NTOHB64(pkt_matches->__arp_sha));
};
/**
* @brief Get the packet match ARP_TPA value in HOST BYTE ORDER
* @ingroup core_of1x
*/
static inline
uint32_t packet_matches_get_arp_tpa_value(packet_matches_t *const pkt_matches){
	return NTOHB32(pkt_matches->__arp_tpa);
};
/**
* @brief Get the packet match ARP_SPA value in HOST BYTE ORDER
* @ingroup core_of1x
*/
static inline
uint32_t packet_matches_get_arp_spa_value(packet_matches_t *const pkt_matches){
	return NTOHB32(pkt_matches->__arp_spa);
};


//IP
/**
* @brief Get the packet match IP_PROTO value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint8_t packet_matches_get_ip_proto_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__ip_proto;
};
/**
* @brief Get the packet match IPC_ECN value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint8_t packet_matches_get_ip_ecn_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__ip_ecn;
};
/**
* @brief Get the packet match IP_DSCP value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint8_t packet_matches_get_ip_dscp_value(packet_matches_t *const pkt_matches){
	return OF1X_IP_DSCP_VALUE(pkt_matches->__ip_dscp);
};

//IPv4
/**
* @brief Get the packet match IP4_SRC value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint32_t packet_matches_get_ipv4_src_value(packet_matches_t *const pkt_matches){
	return NTOHB32(pkt_matches->__ipv4_src);
};
/**
* @brief Get the packet match IP4_DST value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint32_t packet_matches_get_ipv4_dst_value(packet_matches_t *const pkt_matches){
	return NTOHB32(pkt_matches->__ipv4_dst);
};

//TCP
/**
* @brief Get the packet match TCP_SRC value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint16_t packet_matches_get_tcp_src_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__tcp_src);
};
/**
* @brief Get the packet match TCP_DST value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint16_t packet_matches_get_tcp_dst_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__tcp_dst);
};

//UDP
/**
* @brief Get the packet match UDP_SRC value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint16_t packet_matches_get_udp_src_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__udp_src);
};
/**
* @brief Get the packet match UDP_DST value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint16_t packet_matches_get_udp_dst_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__udp_dst);
};

//SCTP
/**
* @brief Get the packet match SCTP_SRC value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint16_t packet_matches_get_sctp_src_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__sctp_src);
};
/**
* @brief Get the packet match SCTP_DST value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint16_t packet_matches_get_sctp_dst_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__sctp_dst);
};

//ICMPv4
/**
* @brief Get the packet match ICMPv4_TYPE value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint8_t packet_matches_get_icmpv4_type_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__icmpv4_type;
};
/**
* @brief Get the packet match ICMPv4_CODE value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint8_t packet_matches_get_icmpv4_code_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__icmpv4_code;
};

//IPv6
/**
 * @brief Get the packet match IP6_SRC value in HOST BYTE ORDER
 * @ingroup core_of1x
 */
static inline
uint128__t packet_matches_get_ipv6_src_value(packet_matches_t *const pkt_matches){
	uint128__t tmp =  pkt_matches->__ipv6_src;
	NTOHB128(tmp);
	return tmp;
};
/**
 * @brief Get the packet match IP6_DST value in HOST BYTE ORDER
 * @ingroup core_of1x
 */
static inline
uint128__t packet_matches_get_ipv6_dst_value(packet_matches_t *const pkt_matches){
	uint128__t tmp =  pkt_matches->__ipv6_dst;
	NTOHB128(tmp);
	return tmp;
};
/**
 * @brief Get the packet match IP6_FLABEL value in HOST BYTE ORDER
 * @ingroup core_of1x
 */
static inline
uint64_t packet_matches_get_ipv6_flabel_value(packet_matches_t *const pkt_matches){
	return OF1X_IP6_FLABEL_VALUE(NTOHB64(pkt_matches->__ipv6_flabel));
};
/**
 * @brief Get the packet match IP6_ND_TARGET value in HOST BYTE ORDER
 * @ingroup core_of1x
 */
static inline
uint128__t packet_matches_get_ipv6_nd_target_value(packet_matches_t *const pkt_matches){
	uint128__t tmp = pkt_matches->__ipv6_nd_target;
	NTOHB128(tmp);
	return tmp;
};
/**
 * @brief Get the packet match IP6_ND_SLL value in HOST BYTE ORDER
 * @ingroup core_of1x
 */
static inline
uint64_t packet_matches_get_ipv6_nd_sll_value(packet_matches_t *const pkt_matches){
	return OF1X_MAC_VALUE(NTOHB64(pkt_matches->__ipv6_nd_sll));
};
/**
 * @brief Get the packet match IP6_ND_TLL value in HOST BYTE ORDER
 * @ingroup core_of1x
 */
static inline
uint64_t packet_matches_get_ipv6_nd_tll_value(packet_matches_t *const pkt_matches){
	return OF1X_MAC_VALUE(NTOHB64(pkt_matches->__ipv6_nd_tll));
};
/**
 * @brief Get the packet match IP6_EXTHDR value in HOST BYTE ORDER
 * @ingroup core_of1x
 */
static inline
uint16_t packet_matches_get_ipv6_exthdr_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__ipv6_exthdr); //TODO align?
};

//ICMPv6
/**
 * @brief Get the packet match ICMPV6_TYPE value in HOST BYTE ORDER
 * @ingroup core_of1x
 */
static inline
uint8_t packet_matches_get_icmpv6_type_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__icmpv6_type;
};
/**
 * @brief Get the packet match ICMPV6_CODE value in HOST BYTE ORDER
 * @ingroup core_of1x
 */
static inline
uint8_t packet_matches_get_icmpv6_code_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__icmpv6_code;
};

/*
 * @brief Get the packet match PBB_ISD value in HOST BYTE ORDER
 * @ingroup core_of1x
 */
static inline
uint32_t packet_matches_get_pbb_isid_value(packet_matches_t *const pkt_matches){
	return NTOHB32(pkt_matches->__pbb_isid); //TODO align?
};

/**
 * @brief Get the packet match TUNNEL_ID value in HOST BYTE ORDER
 * @ingroup core_of1x
 */
static inline
uint64_t packet_matches_get_tunnel_id_value(packet_matches_t *const pkt_matches){
	return NTOHB64(pkt_matches->__tunnel_id);
};


//PPPoE
/**
* @brief Get the packet match PPPOE_CODE value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint8_t packet_matches_get_pppoe_code_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__pppoe_code;
};
/**
* @brief Get the packet match PPPOE_TYPE value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint8_t packet_matches_get_pppoe_type_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__pppoe_type;
};
/**
* @brief Get the packet match PPPOE_SESSION value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint16_t packet_matches_get_pppoe_sid_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__pppoe_sid);
};

//PPP
/**
* @brief Get the packet match PPP_PROTO value in HOST BYTE ORDER 
* @ingroup core_of1x 
*/
static inline
uint16_t packet_matches_get_ppp_proto_value(packet_matches_t *const pkt_matches){
	return NTOHB16(pkt_matches->__ppp_proto);
};

//GTP
/**
* @brief Get the packet match PPP_PROTO value in HOST BYTE ORDER
* @ingroup core_of1x
*/
static inline
uint8_t packet_matches_get_gtp_msg_type_value(packet_matches_t *const pkt_matches){
	return pkt_matches->__gtp_msg_type;
};
/**
* @brief Get the packet match PPP_PROTO value in HOST BYTE ORDER
* @ingroup core_of1x
*/
static inline
uint32_t packet_matches_get_gtp_teid_value(packet_matches_t *const pkt_matches){
	return NTOHB32(pkt_matches->__gtp_teid);
};

//C++ extern C
ROFL_END_DECLS

#endif //PACKET_MATCHES
