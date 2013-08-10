/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PLATFORM_HOOKS
#define PLATFORM_HOOKS

#include <stdint.h>

#include "rofl.h"
#include "../common/datapacket.h"
#include "../common/ternary_fields.h"
#include "../switch_port.h"

/**
* @file packet.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Defines the interface for packet mangling. 
* The user of the library MUST provide an implementation for his/her platform.
*
* The packet mangling platform API is the set of calls used by library
* to perform operations over the packet. These operations may include
* modifications (set field in header, push/pop header) as well as output
* and drop actions.
*
* The user of the library does not need to care about internals of the pipeline
* like table statistics, pipeline statistics, timers... except for the port
* statistics, which he/she should use the appropiate statistics API (device API).
*
* The user MUST provide an implementation for his/her particular
* platform. 
*/

//C++ extern C
ROFL_BEGIN_DECLS

/*
* Actions over the packet
*/
/**
* @ingroup platform_packet
* Copy ttl in
*/
void platform_packet_copy_ttl_in(datapacket_t* pkt);

/* POP */
/**
* @ingroup platform_packet
* Pop vlan header
*/
void platform_packet_pop_vlan(datapacket_t* pkt);
/**
* @ingroup platform_packet
* Pop mpls header
*/
void platform_packet_pop_mpls(datapacket_t* pkt, uint16_t ether_type);
/**
* @ingroup platform_packet
* Pop pppoe header
*/
void platform_packet_pop_pppoe(datapacket_t* pkt, uint16_t ether_type);

/* PUSH */
void platform_packet_push_pppoe(datapacket_t* pkt, uint16_t ether_type);
void platform_packet_push_mpls(datapacket_t* pkt, uint16_t ether_type);
void platform_packet_push_vlan(datapacket_t* pkt, uint16_t ether_type);

/* Copy ttl out*/
void platform_packet_copy_ttl_out(datapacket_t* pkt);

/* Decrement ttl */
void platform_packet_dec_nw_ttl(datapacket_t* pkt);
void platform_packet_dec_mpls_ttl(datapacket_t* pkt);

/* Set field */
void platform_packet_set_mpls_ttl(datapacket_t* pkt, uint8_t new_ttl);
void platform_packet_set_nw_ttl(datapacket_t* pkt, uint8_t new_ttl);
void platform_packet_set_queue(datapacket_t* pkt, uint32_t queue);

//TODO:
//void platform_packet_set_metadata(datapacket_t* pkt, uint64_t metadata);

//Ethernet
void platform_packet_set_eth_dst(datapacket_t* pkt, uint64_t eth_dst);
void platform_packet_set_eth_src(datapacket_t* pkt, uint64_t eth_src);
void platform_packet_set_eth_type(datapacket_t* pkt, uint16_t eth_type);

//802.1q
void platform_packet_set_vlan_vid(datapacket_t* pkt, uint16_t vlan_vid);
void platform_packet_set_vlan_pcp(datapacket_t* pkt, uint8_t vlan_pcp);

//ARP
void platform_packet_set_arp_opcode(datapacket_t* pkt, uint16_t arp_opcode);
void platform_packet_set_arp_sha(datapacket_t* pkt, uint64_t arp_sha);
void platform_packet_set_arp_spa(datapacket_t* pkt, uint32_t arp_spa);
void platform_packet_set_arp_tha(datapacket_t* pkt, uint64_t arp_tha);
void platform_packet_set_arp_tpa(datapacket_t* pkt, uint32_t arp_tpa);

//IP, IPv4
void platform_packet_set_ip_dscp(datapacket_t* pkt, uint8_t ip_dscp);
void platform_packet_set_ip_ecn(datapacket_t* pkt, uint8_t ip_ecn);
void platform_packet_set_ip_proto(datapacket_t* pkt, uint8_t ip_proto);
void platform_packet_set_ipv4_src(datapacket_t* pkt, uint32_t ip_src);
void platform_packet_set_ipv4_dst(datapacket_t* pkt, uint32_t ip_dst);

//TCP
void platform_packet_set_tcp_src(datapacket_t* pkt, uint16_t tcp_src);
void platform_packet_set_tcp_dst(datapacket_t* pkt, uint16_t tcp_dst);

//UDP
void platform_packet_set_udp_src(datapacket_t* pkt, uint16_t udp_src);
void platform_packet_set_udp_dst(datapacket_t* pkt, uint16_t udp_dst);


//ICMPV4
void platform_packet_set_icmpv4_type(datapacket_t* pkt, uint8_t type);
void platform_packet_set_icmpv4_code(datapacket_t* pkt, uint8_t code);

//MPLS
void platform_packet_set_mpls_label(datapacket_t* pkt, uint32_t label);
void platform_packet_set_mpls_tc(datapacket_t* pkt, uint8_t tc);

//PPPOE
void platform_packet_set_pppoe_type(datapacket_t* pkt, uint8_t type);
void platform_packet_set_pppoe_code(datapacket_t* pkt, uint8_t code);
void platform_packet_set_pppoe_sid(datapacket_t* pkt, uint16_t sid);

//PPP
void platform_packet_set_ppp_proto(datapacket_t* pkt, uint16_t proto);

//IPv6
void platform_packet_set_ipv6_src(datapacket_t*pkt, uint128__t ipv6_src);
void platform_packet_set_ipv6_dst(datapacket_t*pkt, uint128__t ipv6_dst);
void platform_packet_set_ipv6_flabel(datapacket_t*pkt, uint64_t ipv6_flabel);
void platform_packet_set_ipv6_nd_target(datapacket_t*pkt, uint128__t ipv6_nd_target);
void platform_packet_set_ipv6_nd_sll(datapacket_t*pkt, uint64_t ipv6_nd_sll);
void platform_packet_set_ipv6_nd_tll(datapacket_t*pkt, uint64_t ipv6_nd_tll);
void platform_packet_set_ipv6_exthdr(datapacket_t*pkt, uint64_t ipv6_exthdr);

//ICMPv6
void platform_packet_set_icmpv6_type(datapacket_t*pkt, uint64_t icmpv6_type);
void platform_packet_set_icmpv6_code(datapacket_t*pkt, uint64_t icmpv6_code);

//GTP
void platform_packet_set_gtp_msg_type(datapacket_t* pkt, uint8_t msg_type);
void platform_packet_set_gtp_teid(datapacket_t* pkt, uint32_t teid);

/**
* @ingroup platform_packet
* Output packet to the port(s)
* The action HAS to implement the destruction/release of the pkt
* (including if this pkt is a replica).
*
* If a flooding output actions needs to be done, the function
* has itself to deal with packet replication.
*/
void platform_packet_output(datapacket_t* pkt, switch_port_t* port);

/**
* @ingroup platform_packet
* Creates a copy (in heap) of the datapacket_t structure including any
* platform specific state (->platform_state). The following behaviour
* is expected from this hook:
* 
* - All data fields and pointers of datapacket_t struct must be memseted to 0, except:
* - datapacket_t flag is_replica must be set to true
* - platform_state, if used, must be replicated (copied) otherwise NULL
*
*/
datapacket_t* platform_packet_replicate(datapacket_t* pkt);

//Packet drop
void platform_packet_drop(datapacket_t* pkt);

/*
* Packet value getters 
*/

//Packet size
uint32_t platform_packet_get_size_bytes(datapacket_t *const pkt);

//Ports
uint32_t platform_packet_get_port_in(datapacket_t *const pkt);
uint32_t platform_packet_get_phy_port_in(datapacket_t *const pkt);	

//Associated metadata TODO
//uint64_t platform_packet_get_metadata(datapacket_t *const pkt);

//802
uint64_t platform_packet_get_eth_dst(datapacket_t *const pkt);
uint64_t platform_packet_get_eth_src(datapacket_t *const pkt);
uint16_t platform_packet_get_eth_type(datapacket_t *const pkt);

//802.1q VLAN outermost tag
uint16_t platform_packet_get_vlan_vid(datapacket_t *const pkt);
uint8_t platform_packet_get_vlan_pcp(datapacket_t *const pkt);

//ARP
uint16_t platform_packet_get_arp_opcode(datapacket_t *const pkt);
uint64_t platform_packet_get_arp_sha(datapacket_t *const pkt);
uint32_t platform_packet_get_arp_spa(datapacket_t *const pkt);
uint64_t platform_packet_get_arp_tha(datapacket_t *const pkt);
uint32_t platform_packet_get_arp_tpa(datapacket_t *const pkt);

//IPv4
uint8_t platform_packet_get_ip_proto(datapacket_t *const pkt);
uint8_t platform_packet_get_ip_ecn(datapacket_t *const pkt);
uint8_t platform_packet_get_ip_dscp(datapacket_t *const pkt);
uint32_t platform_packet_get_ipv4_src(datapacket_t *const pkt);
uint32_t platform_packet_get_ipv4_dst(datapacket_t *const pkt);

//TCP
uint16_t platform_packet_get_tcp_dst(datapacket_t *const pkt);
uint16_t platform_packet_get_tcp_src(datapacket_t *const pkt);

//UDP
uint16_t platform_packet_get_udp_dst(datapacket_t *const pkt);
uint16_t platform_packet_get_udp_src(datapacket_t *const pkt);

//ICMPv4
uint8_t platform_packet_get_icmpv4_type(datapacket_t *const pkt);
uint8_t platform_packet_get_icmpv4_code(datapacket_t *const pkt);

//MPLS-outermost label 
uint32_t platform_packet_get_mpls_label(datapacket_t *const pkt);
uint8_t platform_packet_get_mpls_tc(datapacket_t *const pkt);

//PPPoE related extensions
uint8_t platform_packet_get_pppoe_code(datapacket_t *const pkt);
uint8_t platform_packet_get_pppoe_type(datapacket_t *const pkt);
uint16_t platform_packet_get_pppoe_sid(datapacket_t *const pkt);

//PPP related extensions
uint16_t platform_packet_get_ppp_proto(datapacket_t *const pkt);

//IPv6
uint128__t platform_packet_get_ipv6_src(datapacket_t *const pkt);
uint128__t platform_packet_get_ipv6_dst(datapacket_t *const pkt);
uint64_t platform_packet_get_ipv6_flabel(datapacket_t *const pkt);
uint128__t platform_packet_get_ipv6_nd_target(datapacket_t *const pkt);
uint64_t platform_packet_get_ipv6_nd_sll(datapacket_t *const pkt);
uint64_t platform_packet_get_ipv6_nd_tll(datapacket_t *const pkt);
uint64_t platform_packet_get_ipv6_exthdr(datapacket_t *const pkt);

//ICMPv6
uint64_t platform_packet_get_icmpv6_type(datapacket_t *const pkt);
uint64_t platform_packet_get_icmpv6_code(datapacket_t *const pkt);

//GTP related extensions
uint8_t platform_packet_get_gtp_msg_type(datapacket_t *const pkt);
uint32_t platform_packet_get_gtp_teid(datapacket_t *const pkt);

ROFL_END_DECLS

#endif //PLATFORM_HOOKS
