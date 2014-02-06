#include "packet_matches.h"

#include "rofl.h"
#include "../platform/packet.h"
#include "../util/logging.h"

/*
* Updates/Initializes packet matches based on platform information about the pkt
*/
void __update_packet_matches(datapacket_t *const pkt){
		
	packet_matches_t* matches = &pkt->matches;

	//Pkt size
	matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt);
	
	//Ports
	matches->port_in = platform_packet_get_port_in(pkt);
	matches->phy_port_in = platform_packet_get_phy_port_in(pkt);	

	
	//802
	matches->eth_dst = platform_packet_get_eth_dst(pkt);
	matches->eth_src = platform_packet_get_eth_src(pkt);
	matches->eth_type = platform_packet_get_eth_type(pkt);
	
	//802.1q VLAN outermost tag
	matches->has_vlan = platform_packet_has_vlan(pkt);
	if(matches->has_vlan){
		matches->vlan_vid = platform_packet_get_vlan_vid(pkt);
		matches->vlan_pcp = platform_packet_get_vlan_pcp(pkt);
	}else{
		matches->vlan_vid = matches->vlan_pcp = 0x0;
	}

	matches->ip_proto = platform_packet_get_ip_proto(pkt);
	matches->ip_ecn = platform_packet_get_ip_ecn(pkt);
	matches->ip_dscp = platform_packet_get_ip_dscp(pkt);
	
	//ARP
	matches->arp_opcode = platform_packet_get_arp_opcode(pkt);
	matches->arp_sha = platform_packet_get_arp_sha(pkt);
	matches->arp_spa = platform_packet_get_arp_spa(pkt);
	matches->arp_tha = platform_packet_get_arp_tha(pkt);
	matches->arp_tpa = platform_packet_get_arp_tpa(pkt);

	//IPv4
	matches->ipv4_src = platform_packet_get_ipv4_src(pkt);
	matches->ipv4_dst = platform_packet_get_ipv4_dst(pkt);

	//TCP
	matches->tcp_dst = platform_packet_get_tcp_dst(pkt);
	matches->tcp_src = platform_packet_get_tcp_src(pkt);

	//UDP
	matches->udp_dst = platform_packet_get_udp_dst(pkt);
	matches->udp_src = platform_packet_get_udp_src(pkt);

	//SCTP
	matches->sctp_dst = platform_packet_get_sctp_dst(pkt);
	matches->sctp_src = platform_packet_get_sctp_src(pkt);


	//ICMPv4
	matches->icmpv4_type = platform_packet_get_icmpv4_type(pkt);
	matches->icmpv4_code = platform_packet_get_icmpv4_code(pkt);

	//MPLS-outermost label 
	matches->mpls_label = platform_packet_get_mpls_label(pkt);
	matches->mpls_tc = platform_packet_get_mpls_tc(pkt);
	matches->mpls_bos = platform_packet_get_mpls_bos(pkt);

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
	
	//PBB
	matches->pbb_isid = platform_packet_get_pbb_isid(pkt);
    
	//Tunnel id
	matches->tunnel_id = platform_packet_get_tunnel_id(pkt);

	//GTP related extensions
	matches->gtp_msg_type = platform_packet_get_gtp_msg_type(pkt);
	matches->gtp_teid = platform_packet_get_gtp_teid(pkt);
}

/*
* Sets up pkt->matches and call update to initialize packet matches
*/
void __init_packet_matches(datapacket_t *const pkt){
	
	packet_matches_t* matches = &pkt->matches;

	//Associated metadata
	matches->metadata = 0x0; 
 
	__update_packet_matches(pkt);
}


/* 
* DEBUG/INFO dumping routines 
*/

//Dump packet matches
void dump_packet_matches(packet_matches_t *const pkt){

	ROFL_PIPELINE_DEBUG_NO_PREFIX("Packet matches [");	

	if(!pkt){
		ROFL_PIPELINE_DEBUG_NO_PREFIX("]. No matches. Probably comming from a PACKET_OUT");	
		return;
	}
	
	//Ports
	if(pkt->port_in)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("PORT_IN:%u, ",pkt->port_in);
	if(pkt->phy_port_in)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("PHY_PORT_IN:%u, ",pkt->phy_port_in);
	
	//Metadata
	if(pkt->metadata)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("METADATA:" PRIu64 ", ",pkt->metadata);
	
	//802	
	if(pkt->eth_src)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ETH_SRC:0x%llx, ",(long long unsigned)pkt->eth_src);
	if(pkt->eth_dst)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ETH_DST:0x%llx, ",(long long unsigned)pkt->eth_dst);
	if(pkt->eth_type)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ETH_TYPE:0x%x, ",pkt->eth_type);
	//802.1q
	if(pkt->has_vlan)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("VLAN_VID:%u, ",pkt->vlan_vid);
	if(pkt->has_vlan)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("VLAN_PCP:%u, ",pkt->vlan_pcp);
	//ARP
	if(pkt->eth_type == OF1X_ETH_TYPE_ARP)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ARP_OPCODE:0x%x, ",pkt->arp_opcode);
	if(pkt->eth_type == OF1X_ETH_TYPE_ARP)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ARP_SHA:0x%llx, ",(long long unsigned)pkt->arp_sha);
	if(pkt->eth_type == OF1X_ETH_TYPE_ARP)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ARP_SPA:0x%x, ",pkt->arp_spa);
	if(pkt->eth_type == OF1X_ETH_TYPE_ARP)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ARP_THA:0x%llx, ",(long long unsigned)pkt->arp_tha);
	if(pkt->eth_type == OF1X_ETH_TYPE_ARP)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ARP_TPA:0x%x, ",pkt->arp_tpa);
	//IP/IPv4
	if((pkt->eth_type == OF1X_ETH_TYPE_IPV4 || pkt->eth_type == OF1X_ETH_TYPE_IPV6) && pkt->ip_proto)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IP_PROTO:%u, ",pkt->ip_proto);

	if((pkt->eth_type == OF1X_ETH_TYPE_IPV4 || pkt->eth_type == OF1X_ETH_TYPE_IPV6) && pkt->ip_ecn)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IP_ECN:0x%x, ",pkt->ip_ecn);
	
	if((pkt->eth_type == OF1X_ETH_TYPE_IPV4 || pkt->eth_type == OF1X_ETH_TYPE_IPV6) && pkt->ip_dscp)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IP_DSCP:0x%x, ",pkt->ip_dscp);
	
	if(pkt->ipv4_src)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV4_SRC:0x%x, ",pkt->ipv4_src);
	if(pkt->ipv4_dst)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV4_DST:0x%x, ",pkt->ipv4_dst);
	//TCP
	if(pkt->tcp_src)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("TCP_SRC:%u, ",pkt->tcp_src);
	if(pkt->tcp_dst)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("TCP_DST:%u, ",pkt->tcp_dst);
	//UDP
	if(pkt->udp_src)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("UDP_SRC:%u, ",pkt->udp_src);
	if(pkt->udp_dst)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("UDP_DST:%u, ",pkt->udp_dst);

	//SCTP
	if(pkt->sctp_src)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("SCTP_SRC:%u, ",pkt->sctp_src);
	if(pkt->sctp_dst)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("SCTP_DST:%u, ",pkt->sctp_dst);

	//ICMPV4
	if(pkt->ip_proto == OF1X_IP_PROTO_ICMPV4)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ICMPV4_TYPE:%u, ICMPV4_CODE:%u, ",pkt->icmpv4_type,pkt->icmpv4_code);
	
	//IPv6
	if( UINT128__T_LO(pkt->ipv6_src) || UINT128__T_HI(pkt->ipv6_src) )
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_SRC:0x%lx:%lx, ",UINT128__T_HI(pkt->ipv6_src),UINT128__T_LO(pkt->ipv6_src));
	if( UINT128__T_LO(pkt->ipv6_dst) || UINT128__T_HI(pkt->ipv6_dst) )
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_DST:0x%lx:%lx, ",UINT128__T_HI(pkt->ipv6_dst),UINT128__T_LO(pkt->ipv6_dst));
	if(pkt->eth_type == OF1X_ETH_TYPE_IPV6)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_FLABEL:0x%lu, ",pkt->ipv6_flabel);
	if(pkt->ip_proto == OF1X_IP_PROTO_ICMPV6)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_ND_TARGET:0x%lx:%lx, ",UINT128__T_HI(pkt->ipv6_nd_target),UINT128__T_LO(pkt->ipv6_nd_target));
	if(pkt->ip_proto == OF1X_IP_PROTO_ICMPV6) //NOTE && pkt->icmpv6_type ==?
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_ND_SLL:0x%llx, ",pkt->ipv6_nd_sll);
	if(pkt->ip_proto == OF1X_IP_PROTO_ICMPV6) //NOTE && pkt->icmpv6_type ==?
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_ND_TLL:0x%llx, ",pkt->ipv6_nd_tll);
	/*TODO IPV6 exthdr*/
	/*nd_target nd_sll nd_tll exthdr*/
	
	//ICMPv6
	if(pkt->ip_proto == OF1X_IP_PROTO_ICMPV6)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ICMPV6_TYPE:%lu, ICMPV6_CODE:%lu, ",pkt->icmpv6_type,pkt->icmpv6_code);
	
	//MPLS	
   	if(pkt->eth_type == OF1X_ETH_TYPE_MPLS_UNICAST || pkt->eth_type == OF1X_ETH_TYPE_MPLS_MULTICAST )
		ROFL_PIPELINE_DEBUG_NO_PREFIX("MPLS_LABEL:0x%x, MPLS_TC:0x%x, MPLS_BOS:%u",pkt->mpls_label, pkt->mpls_tc, pkt->mpls_bos);
	//PPPoE
	if(pkt->eth_type == OF1X_ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == OF1X_ETH_TYPE_PPPOE_SESSION ){
		ROFL_PIPELINE_DEBUG_NO_PREFIX("PPPOE_CODE:0x%x, PPPOE_TYPE:0x%x, PPPOE_SID:0x%x, ",pkt->pppoe_code, pkt->pppoe_type,pkt->pppoe_sid);
		//PPP
		if(pkt->eth_type == OF1X_ETH_TYPE_PPPOE_SESSION)
			ROFL_PIPELINE_DEBUG_NO_PREFIX("PPP_PROTO:0x%x, ",pkt->ppp_proto);
				
	}

	//PBB
	if(pkt->pbb_isid)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("PBB_ISID:%u,",pkt->pbb_isid);
	//Tunnel id
	if(pkt->tunnel_id)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("TUNNEL ID:0x%llx, ",(long long unsigned)pkt->tunnel_id);
	
	//GTP
	if(pkt->ip_proto == OF1X_IP_PROTO_UDP && pkt->udp_dst == OF1X_UDP_DST_PORT_GTPU){
		ROFL_PIPELINE_DEBUG_NO_PREFIX("GTP_MSG_TYPE:%u, GTP_TEID:0x%x, ",pkt->gtp_msg_type, pkt->gtp_teid);
	}

	ROFL_PIPELINE_DEBUG_NO_PREFIX("]\n");	

	//Add more here...	
}

