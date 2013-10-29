/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_PACKET_MATCHES_H__
#define __OF1X_PACKET_MATCHES_H__

#include <inttypes.h> 
#include <string.h> 
#include "rofl.h"
#include "../../../common/ternary_fields.h"

/**
* @author Marc Sune<marc.sune (at) bisdn.de>
*/

//Fwd decl
struct datapacket;
union of_packet_matches;

/* 
* Packet OF12 matching values. Matching structure expected by the pipeline for OpenFlow 1.2
*/
typedef struct{

	//Packet size
	uint32_t pkt_size_bytes;	/* Packet size in bytes */

	//Ports
	uint32_t port_in;		/* Switch input port. */
	uint32_t phy_port_in;		/* Switch physical input port. */
	
	//Associated metadata
	uint64_t metadata;		/* Metadata passed between tables. */
 
	//802
	uint64_t eth_dst;		/* Ethernet destination address. */
	uint64_t eth_src;		/* Ethernet source address. */
	uint16_t eth_type;		/* Ethernet frame type (WARNING: inner payload). */
	
	//802.1q VLAN outermost tag
	bool has_vlan;			/* VLAN flag */
	uint16_t vlan_vid;		/* VLAN id. */
	uint8_t vlan_pcp;		/* VLAN PCP. */

	//ARP
	uint16_t arp_opcode;		/* ARP opcode */
	uint64_t arp_sha;		/* ARP source hardware address */
	uint32_t arp_spa;		/* ARP source protocol address */
	uint64_t arp_tha;		/* ARP target hardware address */
	uint32_t arp_tpa;		/* ARP target protocol address */

	//IP
	uint8_t ip_proto;		/* IP protocol. */
	uint8_t ip_dscp;		/* IP DSCP (6 bits in ToS field). */
	uint8_t ip_ecn;			/* IP ECN (2 bits in ToS field). */
	
	//IPv4
	uint32_t ipv4_src;		/* IPv4 source address. */
	uint32_t ipv4_dst;		/* IPv4 destination address. */

	//TCP
	uint16_t tcp_src;		/* TCP source port. */
	uint16_t tcp_dst;		/* TCP destination port. */

	//UDP
	uint16_t udp_src;		/* UDP source port. */
	uint16_t udp_dst;		/* UDP destination port. */

	//SCTP
	uint16_t sctp_src;		/* SCTP source port. */
	uint16_t sctp_dst;		/* SCTP destination port. */


	//ICMPv4
	uint8_t icmpv4_type;		/* ICMP type. */
	uint8_t icmpv4_code;		/* ICMP code. */

	//MPLS-outermost label 
	uint32_t mpls_label;		/* MPLS label. */
	uint8_t mpls_tc;		/* MPLS TC. */
	bool mpls_bos;			/* MPLS BoS. */


	//IPv6
	uint128__t ipv6_src;		/* IPv6 source address */
	uint128__t ipv6_dst;		/* IPv6 source address */
	uint64_t ipv6_flabel;		/* IPv6 flow label */
	uint128__t ipv6_nd_target;	/* IPv6 Neighbor discovery protocol target */
	uint64_t ipv6_nd_sll;		/* IPv6 Neighbor discovery protocol source link level */
	uint64_t ipv6_nd_tll;		/* IPv6 Neighbor discovery protocol target link level */
	uint16_t ipv6_exthdr;		/* IPv6 extension pseudo header */
	
	//ICMPv6 
	uint8_t icmpv6_code;		/* ICMPv6 type */
	uint8_t icmpv6_type;		/* ICMPv6 code */

	//PBB
	uint32_t pbb_isid;		/* PBB_ISID code */
	
	//Tunnel id
	uint64_t tunnel_id;		/* Tunnel id*/

	/*
	* Extensions
	*/

	//PPPoE related extensions
	uint8_t pppoe_code;		/* PPPoE code */
	uint8_t pppoe_type;		/* PPPoE type */
	uint16_t pppoe_sid;		/* PPPoE session id */
	
	//PPP related extensions
	uint16_t ppp_proto;		/* PPPoE session id */
	
	//GTP related extensions
	uint8_t gtp_msg_type;		/* GTP message type */
	uint32_t gtp_teid;		/* GTP teid */

	//CAPWAP related extensions
	uint8_t capwap_rid;		/* CAPWAP RID */
	uint16_t capwap_flags;		/* CAPWAP Flags */
	uint8_t capwap_wbid;		/* CAPWAP WBID */

	//IEEE80211 related extensions
	uint16_t ieee80211_fc;		/* IEEE802.11 FC */
	uint8_t ieee80211_type;		/* IEEE802.11 type */
	uint8_t ieee80211_subtype;	/* IEEE802.11 subtype */
	uint8_t ieee80211_direction;	/* IEEE802.11 direction */
	uint64_t ieee80211_address_1;	/* IEEE802.11 address 1 */
	uint64_t ieee80211_address_2;	/* IEEE802.11 address 2 */
	uint64_t ieee80211_address_3;	/* IEEE802.11 address 3 */
}of1x_packet_matches_t;


//C++ extern C
ROFL_BEGIN_DECLS

//Init packet matches
void __of1x_init_packet_matches(struct datapacket *const pkt);

//Update packet matches after applying actions 
void __of1x_update_packet_matches(struct datapacket *const pkt);

/**
 * @brief Dump the values of packet (header values)  
 * @ingroup core_of1x
 */
void of1x_dump_packet_matches(union of_packet_matches *const pkt_matches);



//C++ extern C
ROFL_END_DECLS

#endif //OF1X_PACKET_MATCHES
