#include <inttypes.h>
#include <rofl/datapath/pipeline/common/datapacket.h>

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
uint8_t platform_packet_get_gtp_msg_type(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_gtp_teid(datapacket_t *const pkt){
	return 0;
}
