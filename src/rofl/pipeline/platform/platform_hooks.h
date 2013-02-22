#ifndef PLATFORM_HOOKS
#define PLATFORM_HOOKS

#include <stdint.h>

#include "../util/rofl_pipeline_utils.h"
#include "../common/datapacket.h"

/*
*
* Prototypes of the functions that platform must support and provide an implementation.
*
*/

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/*
* Actions over the packet
*/

/* Copy ttl */
void platform_copy_ttl_in(datapacket_t* pkt);

/* POP */
void platform_pop_vlan(datapacket_t* pkt);
void platform_pop_mpls(datapacket_t* pkt, uint16_t ether_type);
void platform_pop_pppoe(datapacket_t* pkt, uint16_t ether_type);

/* PUSH */
void platform_push_pppoe(datapacket_t* pkt, uint16_t ether_type);
void platform_push_mpls(datapacket_t* pkt, uint16_t ether_type);
void platform_push_vlan(datapacket_t* pkt, uint16_t ether_type);

/* Copy ttl out*/
void platform_copy_ttl_out(datapacket_t* pkt);

/* Decrement ttl */
void platform_dec_nw_ttl(datapacket_t* pkt);
void platform_dec_mpls_ttl(datapacket_t* pkt);

/* Set field */
void platform_set_mpls_ttl(datapacket_t* pkt, uint8_t new_ttl);
void platform_set_nw_ttl(datapacket_t* pkt, uint8_t new_ttl);
void platform_set_queue(datapacket_t* pkt, uint32_t queue);

//TODO:
//void platform_set_metadata(datapacket_t* pkt, uint64_t metadata);

//Ethernet
void platform_set_eth_dst(datapacket_t* pkt, uint64_t eth_dst);
void platform_set_eth_src(datapacket_t* pkt, uint64_t eth_src);
void platform_set_eth_type(datapacket_t* pkt, uint16_t eth_type);

//802.1q
void platform_set_vlan_vid(datapacket_t* pkt, uint16_t vlan_vid);
void platform_set_vlan_pcp(datapacket_t* pkt, uint8_t vlan_pcp);

//IP, IPv4
void platform_set_ip_dscp(datapacket_t* pkt, uint8_t ip_dscp);
void platform_set_ip_ecn(datapacket_t* pkt, uint8_t ip_ecn);
void platform_set_ip_proto(datapacket_t* pkt, uint8_t ip_proto);
void platform_set_ipv4_src(datapacket_t* pkt, uint32_t ip_src);
void platform_set_ipv4_dst(datapacket_t* pkt, uint32_t ip_dst);

//TCP
void platform_set_tcp_src(datapacket_t* pkt, uint16_t tcp_src);
void platform_set_tcp_dst(datapacket_t* pkt, uint16_t tcp_dst);

//UDP
void platform_set_udp_src(datapacket_t* pkt, uint16_t udp_src);
void platform_set_udp_dst(datapacket_t* pkt, uint16_t udp_dst);


//ICMPV4
void platform_set_icmpv4_type(datapacket_t* pkt, uint8_t type);
void platform_set_icmpv4_code(datapacket_t* pkt, uint8_t code);

//MPLS
void platform_set_mpls_label(datapacket_t* pkt, uint32_t label);
void platform_set_mpls_tc(datapacket_t* pkt, uint8_t tc);

//PPPOE
void platform_set_pppoe_type(datapacket_t* pkt, uint8_t type);
void platform_set_pppoe_code(datapacket_t* pkt, uint8_t code);
void platform_set_pppoe_sid(datapacket_t* pkt, uint16_t sid);

//PPP
void platform_set_ppp_proto(datapacket_t* pkt, uint16_t proto);

/**
* Output packet to the port(s)
* The action HAS to implement the destruction/release of the pkt
* (including if the pkt is a replica).
*
* If a flooding output actions needs to be done, the function
* has itself to deal with packet replication.
*/
void platform_output_packet(datapacket_t* pkt, uint32_t port_num);

/**
* Creates a copy (in heap) of the datapacket_t structure including any
* platform specific state (->platform_state). The following behaviour
* is expected from this hook:
* 
* - All data fields and pointers of datapacket_t struct must be memseted to 0, except:
* - datapacket_t flag is_replica must be set to true
* - platform_state, if used, must be replicated (copied) otherwise NULL
*
*/
datapacket_t* platform_replicate_packet(datapacket_t* pkt);

//Packet drop
void platform_packet_drop(datapacket_t* pkt);

/*
* Packet value getters 
*/

//Ports
uint32_t platform_get_packet_port_in(datapacket_t *const pkt);
uint32_t platform_get_packet_phy_port_in(datapacket_t *const pkt);	

//Associated metadata TODO
//uint64_t platform_get_packet_metadata(datapacket_t *const pkt);

//802
uint64_t platform_get_packet_eth_dst(datapacket_t *const pkt);
uint64_t platform_get_packet_eth_src(datapacket_t *const pkt);
uint16_t platform_get_packet_eth_type(datapacket_t *const pkt);

//802.1q VLAN outermost tag
uint16_t platform_get_packet_vlan_vid(datapacket_t *const pkt);
uint8_t platform_get_packet_vlan_pcp(datapacket_t *const pkt);

//IPv4
uint8_t platform_get_packet_ip_proto(datapacket_t *const pkt);
uint32_t platform_get_packet_ipv4_src(datapacket_t *const pkt);
uint32_t platform_get_packet_ipv4_dst(datapacket_t *const pkt);

//TCP
uint16_t platform_get_packet_tcp_dst(datapacket_t *const pkt);
uint16_t platform_get_packet_tcp_src(datapacket_t *const pkt);

//UDP
uint16_t platform_get_packet_udp_dst(datapacket_t *const pkt);
uint16_t platform_get_packet_udp_src(datapacket_t *const pkt);

//ICMPv4
uint8_t platform_get_packet_icmpv4_type(datapacket_t *const pkt);
uint8_t platform_get_packet_icmpv4_code(datapacket_t *const pkt);

//MPLS-outermost label 
uint32_t platform_get_packet_mpls_label(datapacket_t *const pkt);
uint8_t platform_get_packet_mpls_tc(datapacket_t *const pkt);

//PPPoE related extensions
uint8_t platform_get_packet_pppoe_code(datapacket_t *const pkt);
uint8_t platform_get_packet_pppoe_type(datapacket_t *const pkt);
uint16_t platform_get_packet_pppoe_sid(datapacket_t *const pkt);

//PPP related extensions
uint16_t platform_get_packet_ppp_proto(datapacket_t *const pkt);

ROFL_PIPELINE_END_DECLS

#endif //PLATFORM_HOOKS
