#include "of12_packet_matches.h"

#include "../../../platform/packet.h"

/*
* Updates/Initializes packet matches based on platform information about the pkt
*/
void __of12_update_packet_matches(datapacket_t *const pkt){
		
	of12_packet_matches_t* matches = (of12_packet_matches_t*)pkt->matches;

	//Pkt size
	matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt);
	
	//Ports
	matches->port_in = platform_packet_get_port_in(pkt);
	matches->phy_port_in = platform_packet_get_phy_port_in(pkt);	

	//Associated metadata TODO
	//matches->metadata = platform_packet_get_metadata(pkt);
 
	//802
	matches->eth_dst = platform_packet_get_eth_dst(pkt);
	matches->eth_src = platform_packet_get_eth_src(pkt);
	matches->eth_type = platform_packet_get_eth_type(pkt);
	
	//802.1q VLAN outermost tag
	matches->vlan_vid = platform_packet_get_vlan_vid(pkt);
	matches->vlan_pcp = platform_packet_get_vlan_pcp(pkt);

	matches->ip_proto = platform_packet_get_ip_proto(pkt);
	matches->ip_ecn = platform_packet_get_ip_ecn(pkt);
	matches->ip_dscp = platform_packet_get_ip_dscp(pkt);
	
	//IPv4
	matches->ipv4_src = platform_packet_get_ipv4_src(pkt);
	matches->ipv4_dst = platform_packet_get_ipv4_dst(pkt);

	//TCP
	matches->tcp_dst = platform_packet_get_tcp_dst(pkt);
	matches->tcp_src = platform_packet_get_tcp_src(pkt);

	//UDP
	matches->udp_dst = platform_packet_get_udp_dst(pkt);
	matches->udp_src = platform_packet_get_udp_src(pkt);

	//ICMPv4
	matches->icmpv4_type = platform_packet_get_icmpv4_type(pkt);
	matches->icmpv4_code = platform_packet_get_icmpv4_code(pkt);

	//MPLS-outermost label 
	matches->mpls_label = platform_packet_get_mpls_label(pkt);
	matches->mpls_tc = platform_packet_get_mpls_tc(pkt);

	//PPPoE related extensions
	matches->pppoe_code = platform_packet_get_pppoe_code(pkt);
	matches->pppoe_type = platform_packet_get_pppoe_type(pkt);
	matches->pppoe_sid = platform_packet_get_pppoe_sid(pkt);

	//PPP related extensions
	matches->ppp_proto = platform_packet_get_ppp_proto(pkt);
    
	//IPv6 related extensions
	matches->ipv6_src = platform_packet_get_ipv6_src(pkt);
	matches->ipv6_dst = platform_packet_get_ipv6_dst(pkt);
	matches->ipv6_flabel = platform_packet_get_ipv6_flabel(pkt);
	matches->ipv6_nd_target = platform_packet_get_ipv6_nd_target(pkt);
	matches->ipv6_nd_sll = platform_packet_get_ipv6_nd_sll(pkt);
	matches->ipv6_nd_tll = platform_packet_get_ipv6_nd_tll(pkt);
	matches->ipv6_exthdr = platform_packet_get_ipv6_exthdr(pkt);
	
	//ICMPv6
	matches->icmpv6_type = platform_packet_get_icmpv6_type(pkt);
	matches->icmpv6_code = platform_packet_get_icmpv6_code(pkt);
    

}

/*
* Sets up pkt->matches and call update to initialize packet matches
*/
void __of12_init_packet_matches(datapacket_t *const pkt, of12_packet_matches_t* pkt_matches){
	
	pkt->matches = (of_packet_matches_t*)pkt_matches; 
	__of12_update_packet_matches(pkt);
}
