#include <inttypes.h>
#include <rofl/datapath/pipeline/common/datapacket.h>
#include <rofl/datapath/pipeline/common/ternary_fields.h>

uint32_t
platform_packet_get_size_bytes(datapacket_t * const pkt){
	return 0; 
}
uint32_t platform_packet_get_port_in(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_phy_port_in(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_eth_dst(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_eth_src(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_eth_type(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_vlan_vid(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_vlan_pcp(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_arp_opcode(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_arp_sha(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_arp_spa(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_arp_tha(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_arp_tpa(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_ip_proto(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_ip_ecn(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_ip_dscp(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_ipv4_src(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_ipv4_dst(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_tcp_dst(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_tcp_src(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_udp_dst(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_udp_src(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_sctp_dst(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_sctp_src(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_icmpv4_type(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_icmpv4_code(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_mpls_label(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_mpls_tc(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_mpls_bos(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_pppoe_code(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_pppoe_type(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_pppoe_sid(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_ppp_proto(datapacket_t *const pkt){
	return 0;
}
uint128__t platform_packet_get_ipv6_src(datapacket_t *const pkt){
	uint128__t ret;
	return ret;
}
uint128__t platform_packet_get_ipv6_dst(datapacket_t *const pkt){
	uint128__t ret;
	return ret;
}
uint64_t platform_packet_get_ipv6_flabel(datapacket_t *const pkt){
	return 0;
}
uint128__t platform_packet_get_ipv6_nd_target(datapacket_t *const pkt){
	uint128__t ret;
	return ret;
}
uint64_t platform_packet_get_ipv6_nd_sll(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_ipv6_nd_tll(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_ipv6_exthdr(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_icmpv6_type(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_icmpv6_code(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_pbb_isid(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_tunnel_id(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_gtp_msg_type(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_gtp_teid(datapacket_t *const pkt){
	return 0;
}
bool platform_packet_has_vlan(datapacket_t *const pkt){
       return false;
}
