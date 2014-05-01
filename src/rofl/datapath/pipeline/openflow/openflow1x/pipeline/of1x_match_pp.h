#ifndef __OF1X_MATCH_PP_H__
#define __OF1X_MATCH_PP_H__

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "rofl.h"
#include "../../../util/pp_guard.h" //Never forget to include the guard
#include "../../../common/packet_matches.h"
#include "../../../common/protocol_constants.h"
#include "../../../common/ternary_fields.h"
#include "../../../platform/likely.h"
#include "../../../platform/packet.h"
#include "of1x_match.h"
#include "of1x_utils.h"

/**
* @file of1x_match_pp.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief OpenFlow v1.0, 1.2 and 1.3.2 matches packet processing routines
*
* Note regarding endianness:
* Conforming the convention that the pipeline works in Network Byte Order
* the matches need to to be initialized in NBO (Big Endian).
* This applies to the values comming from the packet (eth_src, eth_dst, ...) 
* and NOT to the ones that are external to it:
*  - port_in
*  - port_phy_in
*  - metadata
* 
* There is an special alignment for non complete values as
*  - mac addresses ( 6 bytes)
*  - vlan vid      (12 bits )
*  - mpls label    (20 bits )
*  - pbb isid      ( 3 bytes)
* More information on these alignments can be found in the
* pipeline general documentation
*/

//C++ extern C
ROFL_BEGIN_DECLS

/*
* CHECK fields against packet
*
* @warning: it MUST BE != NULL
*/
static inline bool __of1x_check_match(datapacket_t *const pkt, of1x_match_t* it){

	switch(it->type){
		//Phy
		case OF1X_MATCH_IN_PORT: return __utern_compare32(it->__tern, platform_packet_get_port_in(pkt));
		case OF1X_MATCH_IN_PHY_PORT: if(!platform_packet_get_port_in(pkt)) return false; //According to spec
					return __utern_compare32(it->__tern, platform_packet_get_phy_port_in(pkt));
		//Metadata
	  	case OF1X_MATCH_METADATA: return __utern_compare64(it->__tern, &pkt->__metadata); 
		
		//802
   		case OF1X_MATCH_ETH_DST:  return __utern_compare64(it->__tern, platform_packet_get_eth_dst(pkt));
   		case OF1X_MATCH_ETH_SRC:  return __utern_compare64(it->__tern, platform_packet_get_eth_src(pkt));
   		case OF1X_MATCH_ETH_TYPE: return __utern_compare16(it->__tern, platform_packet_get_eth_type(pkt));
		
		//802.1q
   		case OF1X_MATCH_VLAN_VID: if( it->vlan_present == OF1X_MATCH_VLAN_SPECIFIC )
						return platform_packet_has_vlan(pkt) && __utern_compare16(it->__tern, platform_packet_get_vlan_vid(pkt));
					  else
						return platform_packet_has_vlan(pkt) == it->vlan_present;
   		case OF1X_MATCH_VLAN_PCP: return platform_packet_has_vlan(pkt) &&  __utern_compare8(it->__tern, platform_packet_get_vlan_pcp(pkt));

		//MPLS
   		case OF1X_MATCH_MPLS_LABEL: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_MPLS_UNICAST || *platform_packet_get_eth_type(pkt) == ETH_TYPE_MPLS_MULTICAST )) return false;
					return __utern_compare32(it->__tern, platform_packet_get_mpls_label(pkt));
   		case OF1X_MATCH_MPLS_TC: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_MPLS_UNICAST || *platform_packet_get_eth_type(pkt) == ETH_TYPE_MPLS_MULTICAST )) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_mpls_tc(pkt));
   		case OF1X_MATCH_MPLS_BOS: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_MPLS_UNICAST || *platform_packet_get_eth_type(pkt) == ETH_TYPE_MPLS_MULTICAST )) return false; 
					return __utern_compare8(it->__tern, (uint8_t*)platform_packet_get_mpls_bos(pkt));
	
		//ARP
   		case OF1X_MATCH_ARP_OP: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP)) return false;
   					return __utern_compare16(it->__tern, platform_packet_get_arp_opcode(pkt));
   		case OF1X_MATCH_ARP_SHA: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP)) return false;
   					return __utern_compare64(it->__tern, platform_packet_get_arp_sha(pkt));
   		case OF1X_MATCH_ARP_SPA: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP)) return false;
					return __utern_compare32(it->__tern, platform_packet_get_arp_spa(pkt));
   		case OF1X_MATCH_ARP_THA: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP)) return false;
   					return __utern_compare64(it->__tern, platform_packet_get_arp_tha(pkt));
   		case OF1X_MATCH_ARP_TPA: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP)) return false;
					return __utern_compare32(it->__tern, platform_packet_get_arp_tpa(pkt));

		//NW (OF1.0 only)
   		case OF1X_MATCH_NW_PROTO: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV4 || *platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV6 || *platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP || (*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION && (*platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP4 || *platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP6) ))) return false;
					if(*platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP){
						uint8_t *low_byte = ((uint8_t*)(platform_packet_get_arp_opcode(pkt)));
						return __utern_compare8(it->__tern, ++low_byte);
					}
					else 
						return __utern_compare8(it->__tern, platform_packet_get_ip_proto(pkt));
	
   		case OF1X_MATCH_NW_SRC:	if((*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV4 || (*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION && *platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP4 ))) 
						return __utern_compare32(it->__tern, platform_packet_get_ipv4_src(pkt)); 
					if(*platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP)
						return __utern_compare32(it->__tern, platform_packet_get_arp_spa(pkt)); 
					return false;
   		case OF1X_MATCH_NW_DST:	if((*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV4 ||(*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION && *platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP4 )))  
						return __utern_compare32(it->__tern, platform_packet_get_ipv4_dst(pkt));
					if(*platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP)
						return __utern_compare32(it->__tern, platform_packet_get_arp_tpa(pkt)); 
					return false;
		//IP
   		case OF1X_MATCH_IP_PROTO: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV4 || *platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV6 || (*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION && (*platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP4 || *platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP6) ))) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_ip_proto(pkt));
		case OF1X_MATCH_IP_ECN: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV4 || *platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV6 || (*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION && *platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP4 ))) return false; //NOTE PPP_PROTO_IP6
					return __utern_compare8(it->__tern, platform_packet_get_ip_ecn(pkt));
	
		case OF1X_MATCH_IP_DSCP: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV4 || *platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV6 || (*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION && *platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP4 ))) return false; //NOTE PPP_PROTO_IP6
					return __utern_compare8(it->__tern, platform_packet_get_ip_dscp(pkt));
		
		//IPv4
   		case OF1X_MATCH_IPV4_SRC: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV4 || (*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION && *platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP4 ))) return false; 
					return __utern_compare32(it->__tern, platform_packet_get_ipv4_src(pkt));
   		case OF1X_MATCH_IPV4_DST:if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV4 ||(*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION && *platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP4 ))) return false;  
					return __utern_compare32(it->__tern, platform_packet_get_ipv4_dst(pkt));
	
		//TCP
   		case OF1X_MATCH_TCP_SRC: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_TCP)) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_tcp_src(pkt));
   		case OF1X_MATCH_TCP_DST: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_TCP)) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_tcp_dst(pkt));
	
		//UDP
   		case OF1X_MATCH_UDP_SRC: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_UDP)) return false; 	
					return __utern_compare16(it->__tern, platform_packet_get_udp_src(pkt));
   		case OF1X_MATCH_UDP_DST: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_UDP)) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_udp_dst(pkt));
		//SCTP
   		case OF1X_MATCH_SCTP_SRC: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_SCTP)) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_tcp_src(pkt));
   		case OF1X_MATCH_SCTP_DST: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_SCTP)) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_tcp_dst(pkt));
	
		//TP (OF1.0 only)
   		case OF1X_MATCH_TP_SRC: if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_TCP))
						return __utern_compare16(it->__tern, platform_packet_get_tcp_src(pkt));
   					if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_UDP))
						return __utern_compare16(it->__tern, platform_packet_get_udp_src(pkt));
					if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_ICMPV4)){
						uint8_t two_byte[2] = {0,*platform_packet_get_icmpv4_type(pkt)};
						return __utern_compare16(it->__tern, (uint16_t*)&two_byte);
					}
					return false;

   		case OF1X_MATCH_TP_DST: if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_TCP))
						return __utern_compare16(it->__tern, platform_packet_get_tcp_dst(pkt));
   					if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_UDP))
						return __utern_compare16(it->__tern, platform_packet_get_udp_dst(pkt));
					if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_ICMPV4)){
						uint8_t two_byte[2] = {0,*platform_packet_get_icmpv4_code(pkt)};
						return __utern_compare16(it->__tern, (uint16_t*)&two_byte);
					}
					return false;
		
		//ICMPv4
		case OF1X_MATCH_ICMPV4_TYPE: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_ICMPV4)) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_icmpv4_type(pkt));
   		case OF1X_MATCH_ICMPV4_CODE: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_ICMPV4)) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_icmpv4_code(pkt));
  		
		//IPv6
		case OF1X_MATCH_IPV6_SRC: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV6 || (*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION && *platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare128(it->__tern, platform_packet_get_ipv6_src(pkt));
		case OF1X_MATCH_IPV6_DST: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV6 || (*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION && *platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare128(it->__tern, platform_packet_get_ipv6_dst(pkt));
		case OF1X_MATCH_IPV6_FLABEL: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV6 || (*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION && *platform_packet_get_ppp_proto(pkt) == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare64(it->__tern, platform_packet_get_ipv6_flabel(pkt));
		case OF1X_MATCH_IPV6_ND_TARGET: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_ICMPV6)) return false; 
					return __utern_compare128(it->__tern, platform_packet_get_ipv6_nd_target(pkt));
		case OF1X_MATCH_IPV6_ND_SLL: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_ICMPV6 && platform_packet_get_ipv6_nd_sll(pkt))) return false; //NOTE OPTION SLL active
					return __utern_compare64(it->__tern, platform_packet_get_ipv6_nd_sll(pkt));
		case OF1X_MATCH_IPV6_ND_TLL: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_ICMPV6 && platform_packet_get_ipv6_nd_tll(pkt))) return false; //NOTE OPTION TLL active
					return __utern_compare64(it->__tern, platform_packet_get_ipv6_nd_tll(pkt));
		case OF1X_MATCH_IPV6_EXTHDR: //TODO not yet implemented.
			return false;
			break;
					
		//ICMPv6
		case OF1X_MATCH_ICMPV6_TYPE: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_ICMPV6)) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_icmpv6_type(pkt));
		case OF1X_MATCH_ICMPV6_CODE: if(!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_ICMPV6 )) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_icmpv6_code(pkt));
			
		//PPPoE related extensions
   		case OF1X_MATCH_PPPOE_CODE: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_DISCOVERY || *platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION )) return false;  
						return __utern_compare8(it->__tern, platform_packet_get_pppoe_code(pkt));
   		case OF1X_MATCH_PPPOE_TYPE: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_DISCOVERY || *platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare8(it->__tern, platform_packet_get_pppoe_type(pkt));
   		case OF1X_MATCH_PPPOE_SID: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_DISCOVERY || *platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare16(it->__tern, platform_packet_get_pppoe_sid(pkt));

		//PPP 
   		case OF1X_MATCH_PPP_PROT: if(!(*platform_packet_get_eth_type(pkt) == ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare16(it->__tern, platform_packet_get_ppp_proto(pkt));
	
		//PBB
   		case OF1X_MATCH_PBB_ISID: if(*platform_packet_get_eth_type(pkt) == ETH_TYPE_PBB) return false;	
						return __utern_compare32(it->__tern, platform_packet_get_pbb_isid(pkt));
	 	//TUNNEL id
   		case OF1X_MATCH_TUNNEL_ID: return __utern_compare64(it->__tern, platform_packet_get_tunnel_id(pkt));
 
		//GTP
   		case OF1X_MATCH_GTP_MSG_TYPE: if (!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_UDP || *platform_packet_get_udp_dst(pkt) == UDP_DST_PORT_GTPU)) return false;
   						return __utern_compare8(it->__tern, platform_packet_get_gtp_msg_type(pkt));
   		case OF1X_MATCH_GTP_TEID: if (!(*platform_packet_get_ip_proto(pkt) == IP_PROTO_UDP || *platform_packet_get_udp_dst(pkt) == UDP_DST_PORT_GTPU)) return false;
   						return __utern_compare32(it->__tern, platform_packet_get_gtp_teid(pkt));
		case OF1X_MATCH_MAX:
				break;
		//Add more here ...
		//Warning: NEVER add a default clause
	}

	assert(0);	
	return false;
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_MATCH_PP
