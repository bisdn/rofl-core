#include "packet_matches.h"

#include "rofl.h"
#include "endianness.h"
#include "protocol_constants.h"
#include "../platform/packet.h"
#include "../util/logging.h"

//FIXME: unify packet matches alignment with MATCHEs and ACTIONs; there should be only one piece of code dealing with alignment

/* 
* DEBUG/INFO dumping routines 
*/
//Fill pkt matches structure
void fill_packet_matches(datapacket_t *const pkt, packet_matches_t* m){
	uint8_t*  ptr8;
	uint16_t* ptr16;
	uint32_t* ptr32;
	uint64_t* ptr64;
	uint128__t* ptr128;
	uint128__t zero_u128 = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
	
	//Packet size
	m->__pkt_size_bytes = platform_packet_get_size_bytes(pkt);	

	//Ports
	m->__port_in = ( (ptr32=platform_packet_get_port_in(pkt))==NULL ? 0 : *ptr32);
	m->__phy_port_in = ( (ptr32=platform_packet_get_phy_port_in(pkt))==NULL ? 0 : *ptr32);
	
	//Associated metadata
	m->__metadata = pkt->__metadata; 
 
	//802
	m->__eth_dst = ( (ptr64=platform_packet_get_eth_dst(pkt))==NULL ? 0 : *ptr64);
	m->__eth_src = ( (ptr64=platform_packet_get_eth_src(pkt))==NULL ? 0 : *ptr64);
	m->__eth_type = ( (ptr16=platform_packet_get_eth_type(pkt))==NULL ? 0 : *ptr16);
	
	//802.1q VLAN outermost tag
	m->__has_vlan = platform_packet_has_vlan(pkt);		
	m->__vlan_vid = ( (ptr16=platform_packet_get_vlan_vid(pkt))==NULL ? 0 : *ptr16);
	m->__vlan_pcp = ( (ptr8=platform_packet_get_vlan_pcp(pkt))==NULL ? 0 : *ptr8);

	//ARP
	m->__arp_opcode = ( (ptr16=platform_packet_get_arp_opcode(pkt))==NULL ? 0 : *ptr16);
	m->__arp_sha = ( (ptr64=platform_packet_get_arp_sha(pkt))==NULL ? 0 : *ptr64);
	m->__arp_spa = ( (ptr32=platform_packet_get_arp_spa(pkt))==NULL ? 0 : *ptr32);
	m->__arp_tha = ( (ptr64=platform_packet_get_arp_tha(pkt))==NULL ? 0 : *ptr64);
	m->__arp_tpa = ( (ptr32=platform_packet_get_arp_tpa(pkt))==NULL ? 0 : *ptr32);

	//IP
	m->__ip_proto = ( (ptr8=platform_packet_get_ip_proto(pkt))==NULL ? 0 : *ptr8);
	m->__ip_dscp = platform_packet_get_ip_dscp(pkt);		
	m->__ip_ecn = platform_packet_get_ip_ecn(pkt);			
	
	//IPv4
	m->__ipv4_src = ( (ptr32=platform_packet_get_ipv4_src(pkt))==NULL ? 0 : *ptr32);
	m->__ipv4_dst = ( (ptr32=platform_packet_get_ipv4_dst(pkt))==NULL ? 0 : *ptr32);

	//TCP
	m->__tcp_src = ( (ptr16=platform_packet_get_tcp_src(pkt))==NULL ? 0 : *ptr16);
	m->__tcp_dst = ( (ptr16=platform_packet_get_tcp_dst(pkt))==NULL ? 0 : *ptr16);

	//UDP
	m->__udp_src = ( (ptr16=platform_packet_get_udp_src(pkt))==NULL ? 0 : *ptr16);
	m->__udp_dst = ( (ptr16=platform_packet_get_udp_dst(pkt))==NULL ? 0 : *ptr16);

	//SCTP
	m->__sctp_src = ( (ptr16=platform_packet_get_sctp_src(pkt))==NULL ? 0 : *ptr16);
	m->__sctp_dst = ( (ptr16=platform_packet_get_sctp_dst(pkt))==NULL ? 0 : *ptr16);


	//ICMPv4
	m->__icmpv4_type = ( (ptr8=platform_packet_get_icmpv4_type(pkt))==NULL ? 0 : *ptr8);
	m->__icmpv4_code = ( (ptr8=platform_packet_get_icmpv4_code(pkt))==NULL ? 0 : *ptr8);

	//MPLS-outermost label 
	m->__mpls_label = ( (ptr32=platform_packet_get_mpls_label(pkt))==NULL ? 0 : *ptr32);
	m->__mpls_tc = ( (ptr8=platform_packet_get_mpls_tc(pkt))==NULL ? 0 : *ptr8);
	m->__mpls_bos = platform_packet_get_mpls_bos(pkt);		


	//IPv6
	m->__ipv6_src = ( (ptr128=platform_packet_get_ipv6_src(pkt))==NULL ? zero_u128 : *ptr128);
	m->__ipv6_dst = ( (ptr128=platform_packet_get_ipv6_dst(pkt))==NULL ? zero_u128 : *ptr128);
	m->__ipv6_flabel = ( (ptr32=platform_packet_get_ipv6_flabel(pkt))==NULL ? 0 : *ptr32);
	m->__ipv6_nd_target = ( (ptr128=platform_packet_get_ipv6_nd_target(pkt))==NULL ? zero_u128 : *ptr128);
	m->__ipv6_nd_sll = ( (ptr64=platform_packet_get_ipv6_nd_sll(pkt))==NULL ? 0 : *ptr64);
	m->__ipv6_nd_tll = ( (ptr64=platform_packet_get_ipv6_nd_tll(pkt))==NULL ? 0 : *ptr64);
	m->__ipv6_exthdr = ( (ptr16=platform_packet_get_ipv6_exthdr(pkt))==NULL ? 0 : *ptr16);
	
	//ICMPv6 
	m->__icmpv6_code = ( (ptr8=platform_packet_get_icmpv6_code(pkt))==NULL ? 0 : *ptr8);
	m->__icmpv6_type = ( (ptr8=platform_packet_get_icmpv6_type(pkt))==NULL ? 0 : *ptr8);

	//PBB
	m->__pbb_isid = ( (ptr32=platform_packet_get_pbb_isid(pkt))==NULL ? 0 : *ptr32);
	
	//Tunnel id
	m->__tunnel_id = ( (ptr64=platform_packet_get_tunnel_id(pkt))==NULL ? 0 : *ptr64);

	/*	
	* Extensions
	*/

	//PPPoE related extensions
	m->__pppoe_code = ( (ptr8=platform_packet_get_pppoe_code(pkt))==NULL ? 0 : *ptr8);
	m->__pppoe_type = ( (ptr8=platform_packet_get_pppoe_type(pkt))==NULL ? 0 : *ptr8);
	m->__pppoe_sid = ( (ptr16=platform_packet_get_pppoe_sid(pkt))==NULL ? 0 : *ptr16);
	
	//PPP related extensions
	m->__ppp_proto = ( (ptr16=platform_packet_get_ppp_proto(pkt))==NULL ? 0 : *ptr16);
	
	//GTP related extensions
	m->__gtp_msg_type = ( (ptr8=platform_packet_get_gtp_msg_type(pkt))==NULL ? 0 : *ptr8);
	m->__gtp_teid = ( (ptr32=platform_packet_get_gtp_teid(pkt))==NULL ? 0 : *ptr32);

	//CAPWAP related extensions
	m->__capwap_wbid = ( (ptr8=platform_packet_get_capwap_wbid(pkt))==NULL ? 0 : *ptr8);
	m->__capwap_rid = ( (ptr8=platform_packet_get_capwap_rid(pkt))==NULL ? 0 : *ptr8);
	m->__capwap_flags = ( (ptr16=platform_packet_get_capwap_flags(pkt))==NULL ? 0 : *ptr16);

	//WLAN related extensions
	m->__wlan_fc = ( (ptr16=platform_packet_get_wlan_fc(pkt))==NULL ? 0 : *ptr16);
	m->__wlan_type = ( (ptr8=platform_packet_get_wlan_type(pkt))==NULL ? 0 : *ptr8);
	m->__wlan_subtype = ( (ptr8=platform_packet_get_wlan_subtype(pkt))==NULL ? 0 : *ptr8);
	m->__wlan_direction = ( (ptr8=platform_packet_get_wlan_direction(pkt))==NULL ? 0 : *ptr8);
	m->__wlan_address_1 = ( (ptr64=platform_packet_get_wlan_address_1(pkt))==NULL ? 0 : *ptr64);
	m->__wlan_address_2 = ( (ptr64=platform_packet_get_wlan_address_2(pkt))==NULL ? 0 : *ptr64);
	m->__wlan_address_3 = ( (ptr64=platform_packet_get_wlan_address_3(pkt))==NULL ? 0 : *ptr64);
}


//Dump packet matches
void dump_packet_matches(datapacket_t *const pkt, bool raw_nbo){

	packet_matches_t matches;
	packet_matches_t* m = &matches;

	//Prefill packet matches	
	fill_packet_matches(pkt, m);
	
	ROFL_PIPELINE_INFO_NO_PREFIX("Packet matches [");	
	
	//Ports
	if(m->__port_in)
		ROFL_PIPELINE_INFO_NO_PREFIX("PORT_IN:%u, ",m->__port_in);
	if(m->__phy_port_in)
		ROFL_PIPELINE_INFO_NO_PREFIX("PHY_PORT_IN:%u, ",m->__phy_port_in);
	
	//Metadata
	if(m->__metadata)
		ROFL_PIPELINE_INFO_NO_PREFIX("METADATA: 0x%" PRIx64 ", ",m->__metadata);
	
	//802	
	if(m->__eth_src){
		uint64_t tmp = m->__eth_src;
		if(!raw_nbo)
			tmp = OF1X_MAC_VALUE(NTOHB64(tmp));
		ROFL_PIPELINE_INFO_NO_PREFIX("ETH_SRC:0x%"PRIx64", ", tmp);
	}
	if(m->__eth_dst){
		uint64_t tmp = m->__eth_dst;
		if(!raw_nbo)
			tmp = OF1X_MAC_VALUE(NTOHB64(tmp));
		ROFL_PIPELINE_INFO_NO_PREFIX("ETH_DST:0x%"PRIx64", ", tmp);
	}
	if(m->__eth_type)
		ROFL_PIPELINE_INFO_NO_PREFIX("ETH_TYPE:0x%x, ", COND_NTOHB16(raw_nbo,m->__eth_type));

	//802.1q
	if(m->__has_vlan)
		ROFL_PIPELINE_INFO_NO_PREFIX("VLAN_VID:%u, ", COND_NTOHB16(raw_nbo,m->__vlan_vid));
	if(m->__has_vlan){
		uint8_t tmp = m->__vlan_pcp;
		if(!raw_nbo)
			tmp = OF1X_VLAN_PCP_VALUE(tmp);
		ROFL_PIPELINE_INFO_NO_PREFIX("VLAN_PCP:%u, ", tmp);
	}
	//ARP
	if(m->__eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_OPCODE:0x%x, ", COND_NTOHB16(raw_nbo,m->__arp_opcode));
	if(m->__eth_type == ETH_TYPE_ARP){
		uint64_t tmp = m->__arp_sha;
		if(!raw_nbo)
			tmp = OF1X_MAC_VALUE(NTOHB64(tmp));
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_SHA:0x%"PRIx64", ", tmp);
	}
	if(m->__eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_SPA:0x%x, ", COND_NTOHB32(raw_nbo,m->__arp_spa));
	if(m->__eth_type == ETH_TYPE_ARP){
		uint64_t tmp = m->__arp_tha;
		if(!raw_nbo)
			tmp = OF1X_MAC_VALUE(NTOHB64(tmp));
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_THA:0x%"PRIx64", ", tmp); 
	}
	if(m->__eth_type == ETH_TYPE_ARP)
		ROFL_PIPELINE_INFO_NO_PREFIX("ARP_TPA:0x%x, ", COND_NTOHB32(raw_nbo,m->__arp_tpa));
	//IP/IPv4
	if((m->__eth_type == ETH_TYPE_IPV4 || m->__eth_type == ETH_TYPE_IPV6) && m->__ip_proto)
		ROFL_PIPELINE_INFO_NO_PREFIX("IP_PROTO:%u, ",m->__ip_proto);

	if((m->__eth_type == ETH_TYPE_IPV4 || m->__eth_type == ETH_TYPE_IPV6) && m->__ip_ecn)
		ROFL_PIPELINE_INFO_NO_PREFIX("IP_ECN:0x%x, ",m->__ip_ecn);
	
	if((m->__eth_type == ETH_TYPE_IPV4 || m->__eth_type == ETH_TYPE_IPV6) && m->__ip_dscp){
		uint8_t tmp = m->__ip_dscp;
		if(!raw_nbo)
			tmp = OF1X_IP_DSCP_VALUE(tmp); 
		
		ROFL_PIPELINE_INFO_NO_PREFIX("IP_DSCP:0x%x, ", tmp);
	}
	
	if(m->__ipv4_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV4_SRC:0x%x, ", COND_NTOHB32(raw_nbo,m->__ipv4_src));
	if(m->__ipv4_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV4_DST:0x%x, ", COND_NTOHB32(raw_nbo,m->__ipv4_dst));
	//TCP
	if(m->__tcp_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("TCP_SRC:%u, ", COND_NTOHB16(raw_nbo,m->__tcp_src));
	if(m->__tcp_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("TCP_DST:%u, ", COND_NTOHB16(raw_nbo,m->__tcp_dst));
	//UDP
	if(m->__udp_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("UDP_SRC:%u, ", COND_NTOHB16(raw_nbo,m->__udp_src));
	if(m->__udp_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("UDP_DST:%u, ", COND_NTOHB16(raw_nbo,m->__udp_dst));

	//SCTP
	if(m->__sctp_src)
		ROFL_PIPELINE_INFO_NO_PREFIX("SCTP_SRC:%u, ", COND_NTOHB16(raw_nbo,m->__sctp_src));
	if(m->__sctp_dst)
		ROFL_PIPELINE_INFO_NO_PREFIX("SCTP_DST:%u, ", COND_NTOHB16(raw_nbo,m->__sctp_dst));

	//ICMPV4
	if(m->__ip_proto == IP_PROTO_ICMPV4)
		ROFL_PIPELINE_INFO_NO_PREFIX("ICMPV4_TYPE:%u, ICMPV4_CODE:%u, ",m->__icmpv4_type,m->__icmpv4_code);
	
	//IPv6
	if( UINT128__T_LO(m->__ipv6_src) || UINT128__T_HI(m->__ipv6_src) ){
		uint128__t tmp = m->__ipv6_src;
		if(!raw_nbo)
			NTOHB128(tmp); 	
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_SRC:0x%lx:%lx, ",UINT128__T_HI(tmp),UINT128__T_LO(tmp));
	}
	if( UINT128__T_LO(m->__ipv6_dst) || UINT128__T_HI(m->__ipv6_dst) ){
		uint128__t tmp = m->__ipv6_dst;
		if(!raw_nbo)
			NTOHB128(tmp); 
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_DST:0x%lx:%lx, ",UINT128__T_HI(tmp),UINT128__T_LO(tmp));
	}
	if(m->__eth_type == ETH_TYPE_IPV6){
		uint32_t tmp = m->__ipv6_flabel;

		if(!raw_nbo)
			tmp = OF1X_IP6_FLABEL_VALUE(NTOHB32(tmp));		
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_FLABEL:0x%lu, ", tmp);
	}
	if(m->__ip_proto == IP_PROTO_ICMPV6){
		uint128__t tmp = m->__ipv6_nd_target; 
		if(!raw_nbo)
			NTOHB128(tmp); 	
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_ND_TARGET:0x%lx:%lx, ",UINT128__T_HI(tmp),UINT128__T_LO(tmp));
	}
	if(m->__ip_proto == IP_PROTO_ICMPV6){
		//NOTE && m->__icmpv6_type ==?
		uint64_t tmp = m->__ipv6_nd_sll;
		if(!raw_nbo)
			tmp = OF1X_MAC_VALUE(NTOHB64(tmp));

		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_ND_SLL:0x%"PRIx64", ", tmp);
	}
	if(m->__ip_proto == IP_PROTO_ICMPV6){
		//NOTE && m->__icmpv6_type ==?
		uint64_t tmp = m->__ipv6_nd_tll; 
		if(!raw_nbo)
			tmp = OF1X_MAC_VALUE(NTOHB64(tmp));
		ROFL_PIPELINE_INFO_NO_PREFIX("IPV6_ND_TLL:0x%"PRIx64", ", tmp);
	} 
	/*TODO IPV6 exthdr*/
	/*nd_target nd_sll nd_tll exthdr*/
	
	//ICMPv6
	if(m->__ip_proto == IP_PROTO_ICMPV6)
		ROFL_PIPELINE_INFO_NO_PREFIX("ICMPV6_TYPE:%lu, ICMPV6_CODE:%lu, ",m->__icmpv6_type,m->__icmpv6_code);
	
	//MPLS	
   	if(m->__eth_type == ETH_TYPE_MPLS_UNICAST || m->__eth_type == ETH_TYPE_MPLS_MULTICAST ){
		uint8_t tmp_tc = m->__mpls_tc;
		uint32_t tmp_label = m->__mpls_label; 
		if(!raw_nbo){
			tmp_tc = OF1X_MPLS_TC_VALUE(tmp_tc);
			tmp_label = OF1X_MPLS_LABEL_VALUE(NTOHB32(tmp_label));
		}
		ROFL_PIPELINE_INFO_NO_PREFIX("MPLS_LABEL:0x%x, MPLS_TC:0x%x, MPLS_BOS:%u", tmp_label, tmp_tc, m->__mpls_bos);
	}
	//PPPoE
	if(m->__eth_type == ETH_TYPE_PPPOE_DISCOVERY || m->__eth_type == ETH_TYPE_PPPOE_SESSION ){
		ROFL_PIPELINE_INFO_NO_PREFIX("PPPOE_CODE:0x%x, PPPOE_TYPE:0x%x, PPPOE_SID:0x%x, ",m->__pppoe_code, m->__pppoe_type,COND_NTOHB16(raw_nbo, m->__pppoe_sid));
		//PPP
		if(m->__eth_type == ETH_TYPE_PPPOE_SESSION)
			ROFL_PIPELINE_INFO_NO_PREFIX("PPP_PROTO:0x%x, ",m->__ppp_proto);
				
	}

	//PBB
	if(m->__pbb_isid)
		ROFL_PIPELINE_INFO_NO_PREFIX("PBB_ISID:%u,", COND_NTOHB32(raw_nbo,m->__pbb_isid));
	//Tunnel id
	if(m->__tunnel_id)
		ROFL_PIPELINE_INFO_NO_PREFIX("TUNNEL ID:0x%"PRIx64", ", COND_NTOHB64(raw_nbo,m->__tunnel_id));
	
	//GTP
	if(m->__ip_proto == IP_PROTO_UDP && m->__udp_dst == UDP_DST_PORT_GTPU){
		ROFL_PIPELINE_INFO_NO_PREFIX("GTP_MSG_TYPE:%u, GTP_TEID:0x%x, ",m->__gtp_msg_type,  COND_NTOHB32(raw_nbo, m->__gtp_teid));
	}
	
	ROFL_PIPELINE_INFO_NO_PREFIX("]\n");	

	//Add more here...	
}

