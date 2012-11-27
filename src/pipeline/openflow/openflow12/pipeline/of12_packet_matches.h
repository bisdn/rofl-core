#ifndef __OF12_PACKET_MATCHES_H__
#define __OF12_PACKET_MATCHES_H__

#include <inttypes.h> 
#include <string.h> 
#include "../../../util/rofl_pipeline_utils.h"
#include "../../../common/datapacket.h"


/* 
* Packet OF12 matching values. Matching structure expected by the pipeline for Openflow 1.2
*/
typedef struct{

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

	//IPv4
	//uint8_t ip_dscp;		/* IP DSCP (6 bits in ToS field). */
	//uint8_t ip_ecn;		/* IP ECN (2 bits in ToS field). */
	uint8_t ip_proto;		/* IP protocol. */
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

}of12_packet_matches_t;


//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

//Init packet matches
void of12_init_packet_matches(datapacket_t *const pkt, of12_packet_matches_t* pkt_matches);

//Update packet matches after applying actions 
void of12_update_packet_matches(datapacket_t *const pkt);

/*
*
* Prototypes of the functions that platform must support and provide an implementation.
*
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

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_PACKET_MATCHES
