#include <inttypes.h>
#include <rofl/datapath/pipeline/common/datapacket.h>
#include <rofl/datapath/pipeline/common/ternary_fields.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>

void platform_packet_copy_ttl_in(datapacket_t* pkt){
}
void platform_packet_pop_vlan(datapacket_t* pkt){
}
void platform_packet_pop_mpls(datapacket_t* pkt){
}
void platform_packet_pop_pppoe(datapacket_t* pkt){
}
void platform_packet_pop_ppp(datapacket_t* pkt){
}
void platform_packet_push_ppp(datapacket_t* pkt){
}
void platform_packet_push_pppoe(datapacket_t* pkt){
}
void platform_packet_push_mpls(datapacket_t* pkt){
}
void platform_packet_push_vlan(datapacket_t* pkt){
}
void platform_packet_copy_ttl_out(datapacket_t* pkt){
}
void platform_packet_dec_nw_ttl(datapacket_t* pkt){
}
void platform_packet_dec_mpls_ttl(datapacket_t* pkt){
}
void platform_packet_set_mpls_ttl(datapacket_t* pkt, uint8_t new_ttl){
}
void platform_packet_set_nw_ttl(datapacket_t* pkt, uint8_t new_ttl){
}
void platform_packet_set_queue(datapacket_t* pkt, uint32_t queue){
}
//TODO:
//void platform_packet_set_metadata(datapacket_t* pkt, uint64_t metadata){ }
void platform_packet_set_eth_dst(datapacket_t* pkt, uint64_t eth_dst){
}
void platform_packet_set_eth_src(datapacket_t* pkt, uint64_t eth_src){
}
void platform_packet_set_eth_type(datapacket_t* pkt, uint16_t eth_type){
}
void platform_packet_set_vlan_vid(datapacket_t* pkt, uint16_t vlan_vid){
}
void platform_packet_set_vlan_pcp(datapacket_t* pkt, uint8_t vlan_pcp){
}
void platform_packet_set_ip_dscp(datapacket_t* pkt, uint8_t ip_dscp){
}
void platform_packet_set_ip_ecn(datapacket_t* pkt, uint8_t ip_ecn){
}
void platform_packet_set_ip_proto(datapacket_t* pkt, uint8_t ip_proto){
}
void platform_packet_set_ipv4_src(datapacket_t* pkt, uint32_t ip_src){
}
void platform_packet_set_ipv4_dst(datapacket_t* pkt, uint32_t ip_dst){
}
void platform_packet_set_tcp_src(datapacket_t* pkt, uint16_t tcp_src){
}
void platform_packet_set_tcp_dst(datapacket_t* pkt, uint16_t tcp_dst){
}
void platform_packet_set_udp_src(datapacket_t* pkt, uint16_t udp_src){
}
void platform_packet_set_udp_dst(datapacket_t* pkt, uint16_t udp_dst){
}
void platform_packet_set_icmpv4_type(datapacket_t* pkt, uint8_t type){
}
void platform_packet_set_icmpv4_code(datapacket_t* pkt, uint8_t code){
}
void platform_packet_set_mpls_label(datapacket_t* pkt, uint32_t label){
}
void platform_packet_set_mpls_tc(datapacket_t* pkt, uint8_t tc){
}
void platform_packet_set_pppoe_type(datapacket_t* pkt, uint8_t type){
}
void platform_packet_set_pppoe_code(datapacket_t* pkt, uint8_t code){
}
void platform_packet_set_pppoe_sid(datapacket_t* pkt, uint16_t sid){
}
void platform_packet_set_ppp_proto(datapacket_t* pkt, uint16_t proto){
}
void platform_packet_output(datapacket_t* pkt, uint32_t port_num){
}
void platform_packet_replicate(datapacket_t* pkt){
}
void platform_packet_drop(datapacket_t* pkt){
}
void platform_packet_set_ipv6_src(datapacket_t * pkt, uint128__t ipv6_src){
}
void platform_packet_set_ipv6_dst(datapacket_t * pkt, uint128__t ipv6_dst){
}
void platform_packet_set_ipv6_flabel(datapacket_t * pkt, uint64_t ipv6_flabel){
}
void platform_packet_set_ipv6_nd_target(datapacket_t * pkt, uint128__t ipv6_nd_target){
}
void platform_packet_set_ipv6_nd_sll(datapacket_t * pkt, uint64_t ipv6_nd_sll){
}
void platform_packet_set_ipv6_nd_tll(datapacket_t * pkt, uint64_t ipv6_nd_tll){
}
void platform_packet_set_ipv6_exthdr(datapacket_t * pkt, uint64_t ipv6_exthdr){
}
void platform_packet_set_icmpv6_type(datapacket_t * pkt, uint64_t icmpv6_type){
}
void platform_packet_set_icmpv6_code(datapacket_t * pkt, uint64_t icmpv6_code){
}