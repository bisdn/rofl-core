/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF12_PACKET_MATCHES_H__
#define __OF12_PACKET_MATCHES_H__

#include <inttypes.h> 
#include <string.h> 
#include "rofl.h"
#include "../../../common/datapacket.h"
#include "../../../common/ternary_fields.h"

/**
* @author Marc Sune<marc.sune (at) bisdn.de>
*/

/* 
* Packet OF12 matching values. Matching structure expected by the pipeline for Openflow 1.2
*/
typedef struct{

	//Packet size
	uint32_t pkt_size_bytes;	/* Packet size in bytes */

	//Ports
	uint32_t port_in;		/* Switch input port. */
	uint32_t phy_port_in;		/* Switch physical input port. */
	
	//Associated metadata
	uint64_t metadata;		//TODO: what to do /* Metadata passed between tables. */
 
	//802
	uint64_t eth_dst;		/* Ethernet destination address. */
	uint64_t eth_src;		/* Ethernet source address. */
	uint16_t eth_type;		/* Ethernet frame type (WARNING: inner payload). */
	
	//802.1q VLAN outermost tag
	uint16_t vlan_vid;		/* VLAN id. */
	uint8_t vlan_pcp;		/* VLAN PCP. */

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

	//ICMPv4
	uint8_t icmpv4_type;		/* ICMP type. */
	uint8_t icmpv4_code;		/* ICMP code. */

	//MPLS-outermost label 
	uint32_t mpls_label;		/* MPLS label. */
	uint8_t mpls_tc;		/* MPLS TC. */

	//PPPoE related extensions
	uint8_t pppoe_code;		/* PPPoE code */
	uint8_t pppoe_type;		/* PPPoE type */
	uint16_t pppoe_sid;		/* PPPoE session id */
	
	//PPP related extensions
	uint16_t ppp_proto;		/* PPPoE session id */
	
	//IPv6
	uint128__t ipv6_src; /* IPv6 source address */
	uint128__t ipv6_dst; /* IPv6 source address */
	uint64_t ipv6_flabel; /* IPv6 flow label */
	uint128__t ipv6_nd_target; /* IPv6 Neighbor discovery protocol target */
	uint64_t ipv6_nd_sll; /* IPv6 Neighbor discovery protocol source link level */
	uint64_t ipv6_nd_tll; /* IPv6 Neighbor discovery protocol target link level */
	uint64_t ipv6_exthdr; /* IPv6 extension pseudo header */
	
	//ICMPv6 
	uint64_t icmpv6_code; /* ICMPv6 type */
	uint64_t icmpv6_type; /* ICMPv6 code */

}of12_packet_matches_t;


//C++ extern C
ROFL_BEGIN_DECLS

//Init packet matches
void __of12_init_packet_matches(datapacket_t *const pkt, of12_packet_matches_t* pkt_matches);

//Update packet matches after applying actions 
void __of12_update_packet_matches(datapacket_t *const pkt);



//C++ extern C
ROFL_END_DECLS

#endif //OF12_PACKET_MATCHES
