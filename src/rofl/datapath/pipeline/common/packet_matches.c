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
	if(pkt->__port_in)
		ROFL_PIPELINE_INFO_NO_PREFIX("PORT_IN:%u, ",pkt->__port_in);
	if(pkt->__phy_port_in)
		ROFL_PIPELINE_INFO_NO_PREFIX("PHY_PORT_IN:%u, ",pkt->__phy_port_in);
	
	//Metadata
	if(pkt->__metadata)
		ROFL_PIPELINE_INFO_NO_PREFIX("METADATA: 0x%" PRIx64 ", ",pkt->__metadata);
	
	//802	
	if(pkt->__eth_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("ETH_SRC:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->__eth_src));
	if(pkt->__eth_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("ETH_DST:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->__eth_dst));
	if(pkt->__eth_type)
		ROFL_PIPELINE_INFO_NO_PREFIX("ETH_TYPE:0x%x, ", COND_NTOHB16(nbo,pkt->__eth_type));
	//802.1q
	if(pkt->__has_vlan)
		ROFL_PIPELINE_INFO_NO_PREFIX("VLAN_VID:%u, ", COND_NTOHB16(nbo,pkt->__vlan_vid));
	if(pkt->__has_vlan)
		ROFL_PIPELINE_INFO_NO_PREFIX("VLAN_PCP:%u, ",pkt->__vlan_pcp);
	//ARP
	if(pkt->__eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_OPCODE:0x%x, ", COND_NTOHB16(nbo,pkt->__arp_opcode));
	if(pkt->__eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_SHA:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->__arp_sha));
	if(pkt->__eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_SPA:0x%x, ", COND_NTOHB32(nbo,pkt->__arp_spa));
	if(pkt->__eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_THA:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->__arp_tha));
	if(pkt->__eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_TPA:0x%x, ", COND_NTOHB32(nbo,pkt->__arp_tpa));
	//IP/IPv4
	if((pkt->__eth_type == ETH_TYPE_IPV4 || pkt->__eth_type == ETH_TYPE_IPV6) && pkt->__ip_proto)
		ROFL_PIPELINE_INFO_NO_PREFIX("IP_PROTO:%u, ",pkt->__ip_proto);

	if((pkt->__eth_type == ETH_TYPE_IPV4 || pkt->__eth_type == ETH_TYPE_IPV6) && pkt->__ip_ecn)
		ROFL_PIPELINE_INFO_NO_PREFIX("IP_ECN:0x%x, ",pkt->__ip_ecn);
	
	if((pkt->__eth_type == ETH_TYPE_IPV4 || pkt->__eth_type == ETH_TYPE_IPV6) && pkt->__ip_dscp)
		ROFL_PIPELINE_INFO_NO_PREFIX("IP_DSCP:0x%x, ",pkt->__ip_dscp);
	
	if(pkt->__ipv4_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV4_SRC:0x%x, ", COND_NTOHB32(nbo,pkt->__ipv4_src));
	if(pkt->__ipv4_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV4_DST:0x%x, ", COND_NTOHB32(nbo,pkt->__ipv4_dst));
	//TCP
	if(pkt->__tcp_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("TCP_SRC:%u, ", COND_NTOHB16(nbo,pkt->__tcp_src));
	if(pkt->__tcp_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("TCP_DST:%u, ", COND_NTOHB16(nbo,pkt->__tcp_dst));
	//UDP
	if(pkt->__udp_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("UDP_SRC:%u, ", COND_NTOHB16(nbo,pkt->__udp_src));
	if(pkt->__udp_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("UDP_DST:%u, ", COND_NTOHB16(nbo,pkt->__udp_dst));

	//SCTP
	if(pkt->__sctp_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("SCTP_SRC:%u, ", COND_NTOHB16(nbo,pkt->__sctp_src));
	if(pkt->__sctp_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("SCTP_DST:%u, ", COND_NTOHB16(nbo,pkt->__sctp_dst));

	//ICMPV4
	if(pkt->__ip_proto == IP_PROTO_ICMPV4)
		ROFL_PIPELINE_INFO_NO_PREFIX("ICMPV4_TYPE:%u, ICMPV4_CODE:%u, ",pkt->__icmpv4_type,pkt->__icmpv4_code);
	
	//IPv6
	if( UINT128__T_LO(pkt->__ipv6_src) || UINT128__T_HI(pkt->__ipv6_src) )
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_SRC:0x%lx:%lx, ",UINT128__T_HI(pkt->__ipv6_src),UINT128__T_LO(pkt->__ipv6_src));
	if( UINT128__T_LO(pkt->__ipv6_dst) || UINT128__T_HI(pkt->__ipv6_dst) )
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_DST:0x%lx:%lx, ",UINT128__T_HI(pkt->__ipv6_dst),UINT128__T_LO(pkt->__ipv6_dst));
	if(pkt->__eth_type == ETH_TYPE_IPV6)
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_FLABEL:0x%lu, ", COND_NTOHB64(nbo,pkt->__ipv6_flabel));
	if(pkt->__ip_proto == IP_PROTO_ICMPV6)
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_ND_TARGET:0x%lx:%lx, ",UINT128__T_HI(pkt->__ipv6_nd_target),UINT128__T_LO(pkt->__ipv6_nd_target));
	if(pkt->__ip_proto == IP_PROTO_ICMPV6) //NOTE && pkt->__icmpv6_type ==?
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_ND_SLL:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->__ipv6_nd_sll));
	if(pkt->__ip_proto == IP_PROTO_ICMPV6) //NOTE && pkt->__icmpv6_type ==?
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_ND_TLL:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->__ipv6_nd_tll));
	/*TODO IPV6 exthdr*/
	/*nd_target nd_sll nd_tll exthdr*/
	
	//ICMPv6
	if(pkt->__ip_proto == IP_PROTO_ICMPV6)
		ROFL_PIPELINE_INFO_NO_PREFIX("ICMPV6_TYPE:%lu, ICMPV6_CODE:%lu, ",pkt->__icmpv6_type,pkt->__icmpv6_code);
	
	//MPLS	
   	if(pkt->__eth_type == ETH_TYPE_MPLS_UNICAST || pkt->__eth_type == ETH_TYPE_MPLS_MULTICAST )
		ROFL_PIPELINE_INFO_NO_PREFIX("MPLS_LABEL:0x%x, MPLS_TC:0x%x, MPLS_BOS:%u", COND_NTOHB32(nbo,pkt->__mpls_label), pkt->__mpls_tc, pkt->__mpls_bos);
	//PPPoE
	if(pkt->__eth_type == ETH_TYPE_PPPOE_DISCOVERY || pkt->__eth_type == ETH_TYPE_PPPOE_SESSION ){
		ROFL_PIPELINE_INFO_NO_PREFIX("PPPOE_CODE:0x%x, PPPOE_TYPE:0x%x, PPPOE_SID:0x%x, ",pkt->__pppoe_code, pkt->__pppoe_type,COND_NTOHB16(nbo, pkt->__pppoe_sid));
		//PPP
		if(pkt->__eth_type == ETH_TYPE_PPPOE_SESSION)
			ROFL_PIPELINE_INFO_NO_PREFIX("PPP_PROTO:0x%x, ",pkt->__ppp_proto);
				
	}

	//PBB
	if(pkt->__pbb_isid)
		ROFL_PIPELINE_INFO_NO_PREFIX("PBB_ISID:%u,", COND_NTOHB32(nbo,pkt->__pbb_isid));
	//Tunnel id
	if(pkt->__tunnel_id)
		ROFL_PIPELINE_INFO_NO_PREFIX("TUNNEL ID:0x%"PRIx64", ", COND_NTOHB64(nbo,pkt->__tunnel_id));
	
	//GTP
	if(pkt->__ip_proto == IP_PROTO_UDP && pkt->__udp_dst == UDP_DST_PORT_GTPU){
		ROFL_PIPELINE_INFO_NO_PREFIX("GTP_MSG_TYPE:%u, GTP_TEID:0x%x, ",pkt->__gtp_msg_type,  COND_NTOHB32(nbo, pkt->__gtp_teid));
	}

	ROFL_PIPELINE_INFO_NO_PREFIX("]\n");	

	//Add more here...	
}

