/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <inttypes.h>
#include <rofl/datapath/pipeline/common/datapacket.h>
#include <rofl/datapath/pipeline/common/ternary_fields.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>
#include <rofl/datapath/pipeline/platform/packet.h>

void platform_packet_copy_ttl_in(datapacket_t* pkt){}
void platform_packet_pop_vlan(datapacket_t* pkt){}
void platform_packet_pop_mpls(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_pop_pppoe(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_pop_ppp(datapacket_t* pkt){}
void platform_packet_push_ppp(datapacket_t* pkt){}
void platform_packet_push_pppoe(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_push_mpls(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_push_vlan(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_copy_ttl_out(datapacket_t* pkt){}
void platform_packet_dec_nw_ttl(datapacket_t* pkt){}
void platform_packet_dec_mpls_ttl(datapacket_t* pkt){}
void platform_packet_set_mpls_ttl(datapacket_t* pkt, uint8_t new_ttl){}
void platform_packet_set_nw_ttl(datapacket_t* pkt, uint8_t new_ttl){}
void platform_packet_set_queue(datapacket_t* pkt, uint32_t queue){}
//TODO:
//void platform_packet_set_metadata(datapacket_t* pkt, uint64_t metadata){ }
void platform_packet_set_eth_dst(datapacket_t* pkt, uint64_t eth_dst){}
void platform_packet_set_eth_src(datapacket_t* pkt, uint64_t eth_src){}
void platform_packet_set_eth_type(datapacket_t* pkt, uint16_t eth_type){}
void platform_packet_set_vlan_vid(datapacket_t* pkt, uint16_t vlan_vid){}
void platform_packet_set_vlan_pcp(datapacket_t* pkt, uint8_t vlan_pcp){}
void platform_packet_set_arp_opcode(datapacket_t* pkt, uint16_t arp_opcode){}
void platform_packet_set_arp_sha(datapacket_t* pkt, uint64_t arp_sha){}
void platform_packet_set_arp_spa(datapacket_t* pkt, uint32_t arp_spa){}
void platform_packet_set_arp_tha(datapacket_t* pkt, uint64_t arp_tha){}
void platform_packet_set_arp_tpa(datapacket_t* pkt, uint32_t arp_tpa){}
void platform_packet_set_ip_dscp(datapacket_t* pkt, uint8_t ip_dscp){}
void platform_packet_set_ip_ecn(datapacket_t* pkt, uint8_t ip_ecn){}
void platform_packet_set_ip_proto(datapacket_t* pkt, uint8_t ip_proto){}
void platform_packet_set_ipv4_src(datapacket_t* pkt, uint32_t ip_src){}
void platform_packet_set_ipv4_dst(datapacket_t* pkt, uint32_t ip_dst){}
void platform_packet_set_tcp_src(datapacket_t* pkt, uint16_t tcp_src){}
void platform_packet_set_tcp_dst(datapacket_t* pkt, uint16_t tcp_dst){}
void platform_packet_set_udp_src(datapacket_t* pkt, uint16_t udp_src){}
void platform_packet_set_udp_dst(datapacket_t* pkt, uint16_t udp_dst){}
void platform_packet_set_sctp_src(datapacket_t* pkt, uint16_t sctp_src){}
void platform_packet_set_sctp_dst(datapacket_t* pkt, uint16_t sctp_dst){}



void platform_packet_set_icmpv4_type(datapacket_t* pkt, uint8_t type){}
void platform_packet_set_icmpv4_code(datapacket_t* pkt, uint8_t code){}
void platform_packet_set_mpls_label(datapacket_t* pkt, uint32_t label){}
void platform_packet_set_mpls_tc(datapacket_t* pkt, uint8_t tc){}
void platform_packet_set_mpls_bos(datapacket_t* pkt, bool bos){}
void platform_packet_set_pppoe_type(datapacket_t* pkt, uint8_t type){}
void platform_packet_set_pppoe_code(datapacket_t* pkt, uint8_t code){}
void platform_packet_set_pppoe_sid(datapacket_t* pkt, uint16_t sid){}
void platform_packet_set_ppp_proto(datapacket_t* pkt, uint16_t proto){}
void platform_packet_set_gtp_msg_type(datapacket_t* pkt, uint8_t msg_type){}
void platform_packet_set_gtp_teid(datapacket_t* pkt, uint32_t teid){}
void platform_packet_output(datapacket_t* pkt, switch_port_t* port){}
datapacket_t* platform_packet_replicate(datapacket_t* pkt){return NULL;}
void platform_packet_drop(datapacket_t* pkt){}
void platform_packet_set_ipv6_src(datapacket_t * pkt, uint128__t ipv6_src){}
void platform_packet_set_ipv6_dst(datapacket_t * pkt, uint128__t ipv6_dst){}
void platform_packet_set_ipv6_flabel(datapacket_t * pkt, uint64_t ipv6_flabel){}
void platform_packet_set_ipv6_nd_target(datapacket_t * pkt, uint128__t ipv6_nd_target){}
void platform_packet_set_ipv6_nd_sll(datapacket_t * pkt, uint64_t ipv6_nd_sll){}
void platform_packet_set_ipv6_nd_tll(datapacket_t * pkt, uint64_t ipv6_nd_tll){}
void platform_packet_set_ipv6_exthdr(datapacket_t * pkt, uint16_t ipv6_exthdr){}
void platform_packet_set_icmpv6_type(datapacket_t * pkt, uint8_t icmpv6_type){}
void platform_packet_set_icmpv6_code(datapacket_t * pkt, uint8_t icmpv6_code){}

void platform_packet_set_pbb_isid(datapacket_t*pkt, uint32_t pbb_isid){}
void platform_packet_pop_pbb(datapacket_t* pkt){}
void platform_packet_push_pbb(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_pop_gtp(datapacket_t* pkt){}
void platform_packet_push_gtp(datapacket_t* pkt){}



void platform_packet_set_tunnel_id(datapacket_t*pkt, uint64_t tunnel_id){}

uint128__t tmp_val = {{0}};

uint32_t platform_packet_get_size_bytes(datapacket_t * const pkt){
	return 0;
}
uint32_t* platform_packet_get_port_in(datapacket_t *const pkt){
	return (uint32_t*)&tmp_val;
}
uint32_t* platform_packet_get_phy_port_in(datapacket_t *const pkt){
	return (uint32_t*)&tmp_val;
}
uint64_t* platform_packet_get_eth_dst(datapacket_t *const pkt){
	return (uint64_t*)&tmp_val;
}
uint64_t* platform_packet_get_eth_src(datapacket_t *const pkt){
	return (uint64_t*)&tmp_val;
}
uint16_t* platform_packet_get_eth_type(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint16_t* platform_packet_get_vlan_vid(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint8_t* platform_packet_get_vlan_pcp(datapacket_t *const pkt){
	return (uint8_t*)&tmp_val;
}
uint16_t* platform_packet_get_arp_opcode(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint64_t* platform_packet_get_arp_sha(datapacket_t *const pkt){
	return (uint64_t*)&tmp_val;
}
uint32_t* platform_packet_get_arp_spa(datapacket_t *const pkt){
	return (uint32_t*)&tmp_val;
}
uint64_t* platform_packet_get_arp_tha(datapacket_t *const pkt){
	return (uint64_t*)&tmp_val;
}
uint32_t* platform_packet_get_arp_tpa(datapacket_t *const pkt){
	return (uint32_t*)&tmp_val;
}
uint8_t* platform_packet_get_ip_proto(datapacket_t *const pkt){
	return (uint8_t*)&tmp_val;
}
uint8_t platform_packet_get_ip_ecn(datapacket_t *const pkt){
	return 0x0;
}
uint8_t platform_packet_get_ip_dscp(datapacket_t *const pkt){
	return 0x0;
}
uint32_t* platform_packet_get_ipv4_src(datapacket_t *const pkt){
	return (uint32_t*)&tmp_val;
}
uint32_t* platform_packet_get_ipv4_dst(datapacket_t *const pkt){
	return (uint32_t*)&tmp_val;
}
uint16_t* platform_packet_get_tcp_dst(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint16_t* platform_packet_get_tcp_src(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint16_t* platform_packet_get_udp_dst(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint16_t* platform_packet_get_udp_src(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint16_t* platform_packet_get_sctp_dst(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint16_t* platform_packet_get_sctp_src(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint8_t* platform_packet_get_icmpv4_type(datapacket_t *const pkt){
	return (uint8_t*)&tmp_val;
}
uint8_t* platform_packet_get_icmpv4_code(datapacket_t *const pkt){
	return (uint8_t*)&tmp_val;
}
uint32_t* platform_packet_get_mpls_label(datapacket_t *const pkt){
	return (uint32_t*)&tmp_val;
}
uint8_t* platform_packet_get_mpls_tc(datapacket_t *const pkt){
	return (uint8_t*)&tmp_val;
}
bool platform_packet_get_mpls_bos(datapacket_t *const pkt){
	return false;
}
uint8_t* platform_packet_get_pppoe_code(datapacket_t *const pkt){
	return (uint8_t*)&tmp_val;
}
uint8_t* platform_packet_get_pppoe_type(datapacket_t *const pkt){
	return (uint8_t*)&tmp_val;
}
uint16_t* platform_packet_get_pppoe_sid(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint16_t* platform_packet_get_ppp_proto(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint128__t* platform_packet_get_ipv6_src(datapacket_t *const pkt){
	return &tmp_val;
}
uint128__t* platform_packet_get_ipv6_dst(datapacket_t *const pkt){
	return &tmp_val;
}
uint32_t* platform_packet_get_ipv6_flabel(datapacket_t *const pkt){
	return (uint32_t*)&tmp_val;
}
uint128__t* platform_packet_get_ipv6_nd_target(datapacket_t *const pkt){
	return &tmp_val;
}
uint64_t* platform_packet_get_ipv6_nd_sll(datapacket_t *const pkt){
	return (uint64_t*)&tmp_val;
}
uint64_t* platform_packet_get_ipv6_nd_tll(datapacket_t *const pkt){
	return (uint64_t*)&tmp_val;
}
uint16_t* platform_packet_get_ipv6_exthdr(datapacket_t *const pkt){
	return (uint16_t*)&tmp_val;
}
uint8_t* platform_packet_get_icmpv6_type(datapacket_t *const pkt){
	return (uint8_t*)&tmp_val;
}
uint8_t* platform_packet_get_icmpv6_code(datapacket_t *const pkt){
	return (uint8_t*)&tmp_val;
}
uint32_t* platform_packet_get_pbb_isid(datapacket_t *const pkt){
	return (uint32_t*)&tmp_val;
}
uint64_t* platform_packet_get_tunnel_id(datapacket_t *const pkt){
	return (uint64_t*)&tmp_val;
}
uint8_t* platform_packet_get_gtp_msg_type(datapacket_t *const pkt){
	return (uint8_t*)&tmp_val;
}
uint32_t* platform_packet_get_gtp_teid(datapacket_t *const pkt){
	return (uint32_t*)&tmp_val;
}
bool platform_packet_has_vlan(datapacket_t *const pkt){
       return false;
}
