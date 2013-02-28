#include "of12_packet_matches.h"

#include "../../../platform/platform_hooks.h"

/*
* Updates/Initializes packet matches based on platform information about the pkt
*/
void of12_update_packet_matches(datapacket_t *const pkt){
		
	of12_packet_matches_t* matches = (of12_packet_matches_t*)pkt->matches;

	//Pkt size
	matches->pkt_size_bytes = platform_get_packet_size_bytes(pkt);
	
	//Ports
	matches->port_in = platform_get_packet_port_in(pkt);
	matches->phy_port_in = platform_get_packet_phy_port_in(pkt);	

	//Associated metadata TODO
	//matches->metadata = platform_get_packet_metadata(pkt);
 
	//802
	matches->eth_dst = platform_get_packet_eth_dst(pkt);
	matches->eth_src = platform_get_packet_eth_src(pkt);
	matches->eth_type = platform_get_packet_eth_type(pkt);
	
	//802.1q VLAN outermost tag
	matches->vlan_vid = platform_get_packet_vlan_vid(pkt);
	matches->vlan_pcp = platform_get_packet_vlan_pcp(pkt);

	//IPv4
	matches->ip_proto = platform_get_packet_ip_proto(pkt);
	matches->ipv4_src = platform_get_packet_ipv4_src(pkt);
	matches->ipv4_dst = platform_get_packet_ipv4_dst(pkt);

	//TCP
	matches->tcp_dst = platform_get_packet_tcp_dst(pkt);
	matches->tcp_src = platform_get_packet_tcp_src(pkt);

	//UDP
	matches->udp_dst = platform_get_packet_udp_dst(pkt);
	matches->udp_src = platform_get_packet_udp_src(pkt);

	//ICMPv4
	matches->icmpv4_type = platform_get_packet_icmpv4_type(pkt);
	matches->icmpv4_code = platform_get_packet_icmpv4_code(pkt);

	//MPLS-outermost label 
	matches->mpls_label = platform_get_packet_mpls_label(pkt);
	matches->mpls_tc = platform_get_packet_mpls_tc(pkt);

	//PPPoE related extensions
	matches->pppoe_code = platform_get_packet_pppoe_code(pkt);
	matches->pppoe_type = platform_get_packet_pppoe_type(pkt);
	matches->pppoe_sid = platform_get_packet_pppoe_sid(pkt);

	//PPP related extensions
	matches->ppp_proto = platform_get_packet_ppp_proto(pkt);

}

/*
* Sets up pkt->matches and call update to initialize packet matches
*/
void of12_init_packet_matches(datapacket_t *const pkt, of12_packet_matches_t* pkt_matches){
	
	pkt->matches = (of_packet_matches_t*)pkt_matches; 
	of12_update_packet_matches(pkt);
}
