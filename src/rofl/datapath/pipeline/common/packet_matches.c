#include "packet_matches.h"

#include "rofl.h"
#include "endianness.h"
#include "protocol_constants.h"
#include "../util/logging.h"

/* 
* DEBUG/INFO dumping routines 
*/

//Dump packet matches
void dump_packet_matches(packet_matches_t *const pkt, bool nbo){

	ROFL_PIPELINE_INFO_NO_PREFIX("Packet matches [");	

	if(!pkt){
		ROFL_PIPELINE_INFO_NO_PREFIX("]. No matches. Probably comming from a PACKET_OUT");	
		return;
	}
	
	//Ports
	if(pkt->port_in)
		ROFL_PIPELINE_INFO_NO_PREFIX("PORT_IN:%u, ",pkt->port_in);
	if(pkt->phy_port_in)
		ROFL_PIPELINE_INFO_NO_PREFIX("PHY_PORT_IN:%u, ",pkt->phy_port_in);
	
	//Metadata
	if(pkt->metadata)
		ROFL_PIPELINE_INFO_NO_PREFIX("METADATA: 0x%" PRIx64 ", ",pkt->metadata);
	
	//802	
	if(pkt->eth_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("ETH_SRC:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->eth_src));
	if(pkt->eth_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("ETH_DST:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->eth_dst));
	if(pkt->eth_type)
		ROFL_PIPELINE_INFO_NO_PREFIX("ETH_TYPE:0x%x, ", COND_NTOHB16(nbo,pkt->eth_type));
	//802.1q
	if(pkt->has_vlan)
		ROFL_PIPELINE_INFO_NO_PREFIX("VLAN_VID:%u, ", COND_NTOHB16(nbo,pkt->vlan_vid));
	if(pkt->has_vlan)
		ROFL_PIPELINE_INFO_NO_PREFIX("VLAN_PCP:%u, ",pkt->vlan_pcp);
	//ARP
	if(pkt->eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_OPCODE:0x%x, ", COND_NTOHB16(nbo,pkt->arp_opcode));
	if(pkt->eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_SHA:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->arp_sha));
	if(pkt->eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_SPA:0x%x, ", COND_NTOHB32(nbo,pkt->arp_spa));
	if(pkt->eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_THA:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->arp_tha));
	if(pkt->eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_TPA:0x%x, ", COND_NTOHB32(nbo,pkt->arp_tpa));
	//IP/IPv4
	if((pkt->eth_type == ETH_TYPE_IPV4 || pkt->eth_type == ETH_TYPE_IPV6) && pkt->ip_proto)
		ROFL_PIPELINE_INFO_NO_PREFIX("IP_PROTO:%u, ",pkt->ip_proto);

	if((pkt->eth_type == ETH_TYPE_IPV4 || pkt->eth_type == ETH_TYPE_IPV6) && pkt->ip_ecn)
		ROFL_PIPELINE_INFO_NO_PREFIX("IP_ECN:0x%x, ",pkt->ip_ecn);
	
	if((pkt->eth_type == ETH_TYPE_IPV4 || pkt->eth_type == ETH_TYPE_IPV6) && pkt->ip_dscp)
		ROFL_PIPELINE_INFO_NO_PREFIX("IP_DSCP:0x%x, ",pkt->ip_dscp);
	
	if(pkt->ipv4_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV4_SRC:0x%x, ", COND_NTOHB32(nbo,pkt->ipv4_src));
	if(pkt->ipv4_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV4_DST:0x%x, ", COND_NTOHB32(nbo,pkt->ipv4_dst));
	//TCP
	if(pkt->tcp_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("TCP_SRC:%u, ", COND_NTOHB16(nbo,pkt->tcp_src));
	if(pkt->tcp_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("TCP_DST:%u, ", COND_NTOHB16(nbo,pkt->tcp_dst));
	//UDP
	if(pkt->udp_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("UDP_SRC:%u, ", COND_NTOHB16(nbo,pkt->udp_src));
	if(pkt->udp_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("UDP_DST:%u, ", COND_NTOHB16(nbo,pkt->udp_dst));

	//SCTP
	if(pkt->sctp_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("SCTP_SRC:%u, ", COND_NTOHB16(nbo,pkt->sctp_src));
	if(pkt->sctp_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("SCTP_DST:%u, ", COND_NTOHB16(nbo,pkt->sctp_dst));

	//ICMPV4
	if(pkt->ip_proto == IP_PROTO_ICMPV4)
		ROFL_PIPELINE_INFO_NO_PREFIX("ICMPV4_TYPE:%u, ICMPV4_CODE:%u, ",pkt->icmpv4_type,pkt->icmpv4_code);
	
	//IPv6
	if( UINT128__T_LO(pkt->ipv6_src) || UINT128__T_HI(pkt->ipv6_src) )
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_SRC:0x%lx:%lx, ",UINT128__T_HI(pkt->ipv6_src),UINT128__T_LO(pkt->ipv6_src));
	if( UINT128__T_LO(pkt->ipv6_dst) || UINT128__T_HI(pkt->ipv6_dst) )
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_DST:0x%lx:%lx, ",UINT128__T_HI(pkt->ipv6_dst),UINT128__T_LO(pkt->ipv6_dst));
	if(pkt->eth_type == ETH_TYPE_IPV6)
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_FLABEL:0x%lu, ", COND_NTOHB64(nbo,pkt->ipv6_flabel));
	if(pkt->ip_proto == IP_PROTO_ICMPV6)
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_ND_TARGET:0x%lx:%lx, ",UINT128__T_HI(pkt->ipv6_nd_target),UINT128__T_LO(pkt->ipv6_nd_target));
	if(pkt->ip_proto == IP_PROTO_ICMPV6) //NOTE && pkt->icmpv6_type ==?
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_ND_SLL:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->ipv6_nd_sll));
	if(pkt->ip_proto == IP_PROTO_ICMPV6) //NOTE && pkt->icmpv6_type ==?
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_ND_TLL:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->ipv6_nd_tll));
	/*TODO IPV6 exthdr*/
	/*nd_target nd_sll nd_tll exthdr*/
	
	//ICMPv6
	if(pkt->ip_proto == IP_PROTO_ICMPV6)
		ROFL_PIPELINE_INFO_NO_PREFIX("ICMPV6_TYPE:%lu, ICMPV6_CODE:%lu, ",pkt->icmpv6_type,pkt->icmpv6_code);
	
	//MPLS	
   	if(pkt->eth_type == ETH_TYPE_MPLS_UNICAST || pkt->eth_type == ETH_TYPE_MPLS_MULTICAST )
		ROFL_PIPELINE_INFO_NO_PREFIX("MPLS_LABEL:0x%x, MPLS_TC:0x%x, MPLS_BOS:%u", COND_NTOHB32(nbo,pkt->mpls_label), pkt->mpls_tc, pkt->mpls_bos);
	//PPPoE
	if(pkt->eth_type == ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == ETH_TYPE_PPPOE_SESSION ){
		ROFL_PIPELINE_INFO_NO_PREFIX("PPPOE_CODE:0x%x, PPPOE_TYPE:0x%x, PPPOE_SID:0x%x, ",pkt->pppoe_code, pkt->pppoe_type,COND_NTOHB16(nbo, pkt->pppoe_sid));
		//PPP
		if(pkt->eth_type == ETH_TYPE_PPPOE_SESSION)
			ROFL_PIPELINE_INFO_NO_PREFIX("PPP_PROTO:0x%x, ",pkt->ppp_proto);
				
	}

	//PBB
	if(pkt->pbb_isid)
		ROFL_PIPELINE_INFO_NO_PREFIX("PBB_ISID:%u,", COND_NTOHB32(nbo,pkt->pbb_isid));
	//Tunnel id
	if(pkt->tunnel_id)
		ROFL_PIPELINE_INFO_NO_PREFIX("TUNNEL ID:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->tunnel_id));
	
	//GTP
	if(pkt->ip_proto == IP_PROTO_UDP && pkt->udp_dst == UDP_DST_PORT_GTPU){
		ROFL_PIPELINE_INFO_NO_PREFIX("GTP_MSG_TYPE:%u, GTP_TEID:0x%x, ",pkt->gtp_msg_type,  COND_NTOHB32(nbo, pkt->gtp_teid));
	}

	ROFL_PIPELINE_INFO_NO_PREFIX("]\n");	

	//Add more here...	
}

