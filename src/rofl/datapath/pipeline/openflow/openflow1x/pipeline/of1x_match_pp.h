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
   		case OF1X_MATCH_MPLS_LABEL:{ 
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_MPLS_UNICAST || *ptr_ether_type == ETH_TYPE_MPLS_MULTICAST )) return false;
					return __utern_compare32(it->__tern, platform_packet_get_mpls_label(pkt));
		}
   		case OF1X_MATCH_MPLS_TC:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_MPLS_UNICAST || *ptr_ether_type == ETH_TYPE_MPLS_MULTICAST )) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_mpls_tc(pkt));
		}
   		case OF1X_MATCH_MPLS_BOS:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_MPLS_UNICAST || *ptr_ether_type == ETH_TYPE_MPLS_MULTICAST )) return false;
					uint8_t bos = platform_packet_get_mpls_bos(pkt);
					return __utern_compare8(it->__tern, &bos);
		}
	
		//ARP
   		case OF1X_MATCH_ARP_OP:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_ARP)) return false;
   					return __utern_compare16(it->__tern, platform_packet_get_arp_opcode(pkt));
		}
   		case OF1X_MATCH_ARP_SHA:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_ARP)) return false;
   					return __utern_compare64(it->__tern, platform_packet_get_arp_sha(pkt));
		}
   		case OF1X_MATCH_ARP_SPA:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_ARP)) return false;
					return __utern_compare32(it->__tern, platform_packet_get_arp_spa(pkt));
		}
   		case OF1X_MATCH_ARP_THA:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_ARP)) return false;
   					return __utern_compare64(it->__tern, platform_packet_get_arp_tha(pkt));
		}
   		case OF1X_MATCH_ARP_TPA:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_ARP)) return false;
					return __utern_compare32(it->__tern, platform_packet_get_arp_tpa(pkt));
		}

		//NW (OF1.0 only)
   		case OF1X_MATCH_NW_PROTO:{
#ifdef ROFL_EXPERIMENTAL
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					uint16_t *ptr_ppp_proto = platform_packet_get_ppp_proto(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4 || *ptr_ether_type == ETH_TYPE_IPV6 || *ptr_ether_type == ETH_TYPE_ARP || (*ptr_ether_type == ETH_TYPE_PPPOE_SESSION && ptr_ppp_proto && (*ptr_ppp_proto == PPP_PROTO_IP4 || *ptr_ppp_proto == PPP_PROTO_IP6) ))) return false;
					if(*ptr_ether_type == ETH_TYPE_ARP){
						uint8_t *low_byte = ((uint8_t*)(platform_packet_get_arp_opcode(pkt)));
						return __utern_compare8(it->__tern, ++low_byte);
					}
					else 
						return __utern_compare8(it->__tern, platform_packet_get_ip_proto(pkt));
#else
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4 || *ptr_ether_type == ETH_TYPE_IPV6 || *ptr_ether_type == ETH_TYPE_ARP )) return false;
					if(*ptr_ether_type == ETH_TYPE_ARP){
						uint8_t *low_byte = ((uint8_t*)(platform_packet_get_arp_opcode(pkt)));
						return __utern_compare8(it->__tern, ++low_byte);
					}
					else
						return __utern_compare8(it->__tern, platform_packet_get_ip_proto(pkt));
#endif
		}
   		case OF1X_MATCH_NW_SRC:{
#ifdef ROFL_EXPERIMENTAL
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					uint16_t *ptr_ppp_proto = platform_packet_get_ppp_proto(pkt);
					if( ptr_ether_type && (*ptr_ether_type == ETH_TYPE_IPV4 || (*ptr_ether_type == ETH_TYPE_PPPOE_SESSION && ptr_ppp_proto && *ptr_ppp_proto == PPP_PROTO_IP4 ))) 
						return __utern_compare32(it->__tern, platform_packet_get_ipv4_src(pkt)); 
					if(ptr_ether_type && *ptr_ether_type == ETH_TYPE_ARP)
						return __utern_compare32(it->__tern, platform_packet_get_arp_spa(pkt)); 
					return false;
#else
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( ptr_ether_type && (*ptr_ether_type == ETH_TYPE_IPV4))
						return __utern_compare32(it->__tern, platform_packet_get_ipv4_src(pkt));
					if(ptr_ether_type && *ptr_ether_type == ETH_TYPE_ARP)
						return __utern_compare32(it->__tern, platform_packet_get_arp_spa(pkt));
					return false;
#endif
		}
   		case OF1X_MATCH_NW_DST:{
#ifdef ROFL_EXPERIMENTAL
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					uint16_t *ptr_ppp_proto = platform_packet_get_ppp_proto(pkt);
					if( ptr_ether_type && (*ptr_ether_type == ETH_TYPE_IPV4 ||(*ptr_ether_type == ETH_TYPE_PPPOE_SESSION && ptr_ppp_proto && *ptr_ppp_proto == PPP_PROTO_IP4 )))  
						return __utern_compare32(it->__tern, platform_packet_get_ipv4_dst(pkt));
					if( ptr_ether_type && *ptr_ether_type == ETH_TYPE_ARP)
						return __utern_compare32(it->__tern, platform_packet_get_arp_tpa(pkt)); 
					return false;
#else
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( ptr_ether_type && (*ptr_ether_type == ETH_TYPE_IPV4))
						return __utern_compare32(it->__tern, platform_packet_get_ipv4_dst(pkt));
					if( ptr_ether_type && *ptr_ether_type == ETH_TYPE_ARP)
						return __utern_compare32(it->__tern, platform_packet_get_arp_tpa(pkt));
					return false;
#endif
		}
		
		//IP
   		case OF1X_MATCH_IP_PROTO:{
#ifdef ROFL_EXPERIMENTAL
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					uint16_t *ptr_ppp_proto = platform_packet_get_ppp_proto(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4 || *ptr_ether_type == ETH_TYPE_IPV6 || (*ptr_ether_type == ETH_TYPE_PPPOE_SESSION && ptr_ppp_proto && (*ptr_ppp_proto == PPP_PROTO_IP4 || *ptr_ppp_proto == PPP_PROTO_IP6) ))) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_ip_proto(pkt));
#else
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4 || *ptr_ether_type == ETH_TYPE_IPV6)) return false;
					return __utern_compare8(it->__tern, platform_packet_get_ip_proto(pkt));
#endif
		}
		case OF1X_MATCH_IP_ECN:{
#ifdef ROFL_EXPERIMENTAL
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					uint16_t *ptr_ppp_proto = platform_packet_get_ppp_proto(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4 || *ptr_ether_type == ETH_TYPE_IPV6 || (*ptr_ether_type == ETH_TYPE_PPPOE_SESSION && ptr_ppp_proto && *ptr_ppp_proto == PPP_PROTO_IP4 ))) return false; //NOTE PPP_PROTO_IP6
					{
						uint8_t ecn = platform_packet_get_ip_ecn(pkt);
						return __utern_compare8(it->__tern, &ecn);
					}
#else
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4 || *ptr_ether_type == ETH_TYPE_IPV6)) return false; //NOTE PPP_PROTO_IP6
					{
						uint8_t ecn = platform_packet_get_ip_ecn(pkt);
						return __utern_compare8(it->__tern, &ecn);
					}
#endif
		}
		case OF1X_MATCH_IP_DSCP:{
#ifdef ROFL_EXPERIMENTAL
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					uint16_t *ptr_ppp_proto = platform_packet_get_ppp_proto(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4 || *ptr_ether_type == ETH_TYPE_IPV6 || (*ptr_ether_type == ETH_TYPE_PPPOE_SESSION && ptr_ppp_proto && *ptr_ppp_proto == PPP_PROTO_IP4 ))) return false; //NOTE PPP_PROTO_IP6
					{
						uint8_t dscp = platform_packet_get_ip_dscp(pkt);
						return __utern_compare8(it->__tern, &dscp);
					}
#else
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4 || *ptr_ether_type == ETH_TYPE_IPV6)) return false; //NOTE PPP_PROTO_IP6
					{
						uint8_t dscp = platform_packet_get_ip_dscp(pkt);
						return __utern_compare8(it->__tern, &dscp);
					}
#endif
		}
		
		//IPv4
   		case OF1X_MATCH_IPV4_SRC:{
#ifdef ROFL_EXPERIMENTAL
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					uint16_t *ptr_ppp_proto = platform_packet_get_ppp_proto(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4 || (*ptr_ether_type == ETH_TYPE_PPPOE_SESSION && ptr_ppp_proto && *ptr_ppp_proto == PPP_PROTO_IP4 ))) return false; 
					return __utern_compare32(it->__tern, platform_packet_get_ipv4_src(pkt));
#else
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4)) return false;
					return __utern_compare32(it->__tern, platform_packet_get_ipv4_src(pkt));
#endif
		}
   		case OF1X_MATCH_IPV4_DST:{
#ifdef ROFL_EXPERIMENTAL
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					uint16_t *ptr_ppp_proto = platform_packet_get_ppp_proto(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4 ||(*ptr_ether_type == ETH_TYPE_PPPOE_SESSION && ptr_ppp_proto && *ptr_ppp_proto == PPP_PROTO_IP4 ))) return false;  
					return __utern_compare32(it->__tern, platform_packet_get_ipv4_dst(pkt));
#else
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV4)) return false;
					return __utern_compare32(it->__tern, platform_packet_get_ipv4_dst(pkt));
#endif
		}
	
		//TCP
   		case OF1X_MATCH_TCP_SRC:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_TCP)) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_tcp_src(pkt));
		}
   		case OF1X_MATCH_TCP_DST:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_TCP)) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_tcp_dst(pkt));
		}
	
		//UDP
   		case OF1X_MATCH_UDP_SRC:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_UDP)) return false; 	
					return __utern_compare16(it->__tern, platform_packet_get_udp_src(pkt));
		}
   		case OF1X_MATCH_UDP_DST:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_UDP)) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_udp_dst(pkt));
		}
		//SCTP
   		case OF1X_MATCH_SCTP_SRC:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_SCTP)) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_sctp_src(pkt));
		}
   		case OF1X_MATCH_SCTP_DST:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_SCTP)) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_sctp_dst(pkt));
		}
	
		//TP (OF1.0 only)
   		case OF1X_MATCH_TP_SRC:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if(ptr_ip_proto && (*ptr_ip_proto == IP_PROTO_TCP))
						return __utern_compare16(it->__tern, platform_packet_get_tcp_src(pkt));
   					if(ptr_ip_proto && (*ptr_ip_proto == IP_PROTO_UDP))
						return __utern_compare16(it->__tern, platform_packet_get_udp_src(pkt));
					if(ptr_ip_proto && (*ptr_ip_proto == IP_PROTO_ICMPV4)){
						uint8_t two_byte[2] = {0,*platform_packet_get_icmpv4_type(pkt)};
						return __utern_compare16(it->__tern, (uint16_t*)&two_byte);
					}
					return false;
		}
   		case OF1X_MATCH_TP_DST:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if(ptr_ip_proto && (*ptr_ip_proto == IP_PROTO_TCP))
						return __utern_compare16(it->__tern, platform_packet_get_tcp_dst(pkt));
   					if(ptr_ip_proto && (*ptr_ip_proto == IP_PROTO_UDP))
						return __utern_compare16(it->__tern, platform_packet_get_udp_dst(pkt));
					if(ptr_ip_proto && (*ptr_ip_proto == IP_PROTO_ICMPV4)){
						uint8_t two_byte[2] = {0,*platform_packet_get_icmpv4_code(pkt)};
						return __utern_compare16(it->__tern, (uint16_t*)&two_byte);
					}
					return false;
		}
		
		//ICMPv4
		case OF1X_MATCH_ICMPV4_TYPE:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_ICMPV4)) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_icmpv4_type(pkt));
		}
   		case OF1X_MATCH_ICMPV4_CODE:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_ICMPV4)) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_icmpv4_code(pkt));
		}
  		
		//IPv6
		case OF1X_MATCH_IPV6_SRC:{
#ifdef ROFL_EXPERIMENTAL
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					uint16_t *ptr_ppp_proto = platform_packet_get_ppp_proto(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV6 || (*ptr_ether_type == ETH_TYPE_PPPOE_SESSION && ptr_ppp_proto && *ptr_ppp_proto == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare128(it->__tern, platform_packet_get_ipv6_src(pkt));
#else
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV6)) return false;
					return __utern_compare128(it->__tern, platform_packet_get_ipv6_src(pkt));
#endif
		}
		case OF1X_MATCH_IPV6_DST:{
#ifdef ROFL_EXPERIMENTAL
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					uint16_t *ptr_ppp_proto = platform_packet_get_ppp_proto(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV6 || (*ptr_ether_type == ETH_TYPE_PPPOE_SESSION && ptr_ppp_proto && *ptr_ppp_proto == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare128(it->__tern, platform_packet_get_ipv6_dst(pkt));
#else
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV6)) return false;
					return __utern_compare128(it->__tern, platform_packet_get_ipv6_dst(pkt));
#endif
		}
		case OF1X_MATCH_IPV6_FLABEL:{
#ifdef ROFL_EXPERIMENTAL
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					uint16_t *ptr_ppp_proto = platform_packet_get_ppp_proto(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV6 || (*ptr_ether_type == ETH_TYPE_PPPOE_SESSION && ptr_ppp_proto && *ptr_ppp_proto == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare32(it->__tern, platform_packet_get_ipv6_flabel(pkt));
#else
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_IPV6)) return false;
					return __utern_compare32(it->__tern, platform_packet_get_ipv6_flabel(pkt));
#endif
		}
		case OF1X_MATCH_IPV6_ND_TARGET:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_ICMPV6)) return false; 
					return __utern_compare128(it->__tern, platform_packet_get_ipv6_nd_target(pkt));
		}
		case OF1X_MATCH_IPV6_ND_SLL:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_ICMPV6 && platform_packet_get_ipv6_nd_sll(pkt))) return false; //NOTE OPTION SLL active
					return __utern_compare64(it->__tern, platform_packet_get_ipv6_nd_sll(pkt));
		}
		case OF1X_MATCH_IPV6_ND_TLL:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_ICMPV6 && platform_packet_get_ipv6_nd_tll(pkt))) return false; //NOTE OPTION TLL active
					return __utern_compare64(it->__tern, platform_packet_get_ipv6_nd_tll(pkt));
		}
		case OF1X_MATCH_IPV6_EXTHDR: //TODO not yet implemented.
			return false;
			break;
					
		//ICMPv6
		case OF1X_MATCH_ICMPV6_TYPE:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_ICMPV6)) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_icmpv6_type(pkt));
		}
		case OF1X_MATCH_ICMPV6_CODE:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_ICMPV6 )) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_icmpv6_code(pkt));
		}
			
		//PBB
   		case OF1X_MATCH_PBB_ISID:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_PBB)) return false;
					return __utern_compare32(it->__tern, platform_packet_get_pbb_isid(pkt));
		}
	 	//TUNNEL id
   		case OF1X_MATCH_TUNNEL_ID: return __utern_compare64(it->__tern, platform_packet_get_tunnel_id(pkt));

#ifdef ROFL_EXPERIMENTAL
		//PPPoE related extensions
   		case OF1X_MATCH_PPPOE_CODE:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_PPPOE_DISCOVERY || *ptr_ether_type == ETH_TYPE_PPPOE_SESSION )) return false;  
					return __utern_compare8(it->__tern, platform_packet_get_pppoe_code(pkt));
		}
   		case OF1X_MATCH_PPPOE_TYPE:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_PPPOE_DISCOVERY || *ptr_ether_type == ETH_TYPE_PPPOE_SESSION )) return false; 
					return __utern_compare8(it->__tern, platform_packet_get_pppoe_type(pkt));
		}
   		case OF1X_MATCH_PPPOE_SID:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_PPPOE_DISCOVERY || *ptr_ether_type == ETH_TYPE_PPPOE_SESSION )) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_pppoe_sid(pkt));
		}

		//PPP 
   		case OF1X_MATCH_PPP_PROT:{
					uint16_t *ptr_ether_type = platform_packet_get_eth_type(pkt);
					if( !ptr_ether_type || !(*ptr_ether_type == ETH_TYPE_PPPOE_SESSION )) return false; 
					return __utern_compare16(it->__tern, platform_packet_get_ppp_proto(pkt));
		}

		//GTP
   		case OF1X_MATCH_GTP_MSG_TYPE:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					uint16_t *ptr_udp_dst = platform_packet_get_udp_dst(pkt);
					if (!ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_UDP || (ptr_udp_dst && *ptr_udp_dst == UDP_DST_PORT_GTPU))) return false;
   					return __utern_compare8(it->__tern, platform_packet_get_gtp_msg_type(pkt));
		}
   		case OF1X_MATCH_GTP_TEID:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					uint16_t *ptr_udp_dst = platform_packet_get_udp_dst(pkt);
					if ( !ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_UDP || (ptr_udp_dst && *ptr_udp_dst == UDP_DST_PORT_GTPU))) return false;
   					return __utern_compare32(it->__tern, platform_packet_get_gtp_teid(pkt));
		}

   		//CAPWAP
   		case OF1X_MATCH_CAPWAP_WBID:{
			uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
			uint16_t *ptr_udp_dst = platform_packet_get_udp_dst(pkt);
			// TODO: for CAPWAP-control or CAPWAP-data or both?
			if (!ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_UDP || (ptr_udp_dst && *ptr_udp_dst == UDP_DST_PORT_CAPWAPC))) return false;
				return __utern_compare8(it->__tern, platform_packet_get_capwap_wbid(pkt));
		}
   		case OF1X_MATCH_CAPWAP_RID:{
			uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
			uint16_t *ptr_udp_dst = platform_packet_get_udp_dst(pkt);
			// TODO: for CAPWAP-control or CAPWAP-data or both?
			if (!ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_UDP || (ptr_udp_dst && *ptr_udp_dst == UDP_DST_PORT_CAPWAPC))) return false;
				return __utern_compare8(it->__tern, platform_packet_get_capwap_rid(pkt));
		}
   		case OF1X_MATCH_CAPWAP_FLAGS:{
			uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
			uint16_t *ptr_udp_dst = platform_packet_get_udp_dst(pkt);
			// TODO: for CAPWAP-control or CAPWAP-data or both?
			if (!ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_UDP || (ptr_udp_dst && *ptr_udp_dst == UDP_DST_PORT_CAPWAPC))) return false;
				return __utern_compare16(it->__tern, platform_packet_get_capwap_flags(pkt));
   		}
   		//WLAN
   		case OF1X_MATCH_WLAN_FC:{
   			// TODO: check prerequisites for WLAN frame
			return __utern_compare16(it->__tern, platform_packet_get_wlan_fc(pkt));
		}
   		case OF1X_MATCH_WLAN_TYPE:{
   			// TODO: check prerequisites for WLAN frame
			return __utern_compare8(it->__tern, platform_packet_get_wlan_type(pkt));
		}
   		case OF1X_MATCH_WLAN_SUBTYPE:{
   			// TODO: check prerequisites for WLAN frame
			return __utern_compare8(it->__tern, platform_packet_get_wlan_subtype(pkt));
		}
   		case OF1X_MATCH_WLAN_DIRECTION:{
   			// TODO: check prerequisites for WLAN frame
			return __utern_compare8(it->__tern, platform_packet_get_wlan_direction(pkt));
		}
   		case OF1X_MATCH_WLAN_ADDRESS_1:{
   			// TODO: check prerequisites for WLAN frame
			return __utern_compare64(it->__tern, platform_packet_get_wlan_address_1(pkt));
		}
   		case OF1X_MATCH_WLAN_ADDRESS_2:{
   			// TODO: check prerequisites for WLAN frame
			return __utern_compare64(it->__tern, platform_packet_get_wlan_address_2(pkt));
		}
   		case OF1X_MATCH_WLAN_ADDRESS_3:{
   			// TODO: check prerequisites for WLAN frame
			return __utern_compare64(it->__tern, platform_packet_get_wlan_address_3(pkt));
		}

		//GRE
   		case OF1X_MATCH_GRE_VERSION:{
					uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
					if (!ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_GRE)) return false;
   					return __utern_compare8(it->__tern, platform_packet_get_gre_version(pkt));
		}
   		case OF1X_MATCH_GRE_PROT_TYPE:{
			uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
			if (!ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_GRE)) return false;
   					return __utern_compare16(it->__tern, platform_packet_get_gre_prot_type(pkt));
		}
   		case OF1X_MATCH_GRE_KEY:{
			uint8_t *ptr_ip_proto = platform_packet_get_ip_proto(pkt);
			if (!ptr_ip_proto || !(*ptr_ip_proto == IP_PROTO_GRE)) return false;
   					return __utern_compare32(it->__tern, platform_packet_get_gre_key(pkt));
		}
#else
   		case OF1X_MATCH_PPPOE_CODE:
   		case OF1X_MATCH_PPPOE_TYPE:
   		case OF1X_MATCH_PPPOE_SID:
   		case OF1X_MATCH_PPP_PROT:
   		case OF1X_MATCH_GTP_MSG_TYPE:
   		case OF1X_MATCH_GTP_TEID:
   		case OF1X_MATCH_CAPWAP_WBID:
   		case OF1X_MATCH_CAPWAP_RID:
   		case OF1X_MATCH_CAPWAP_FLAGS:
   		case OF1X_MATCH_WLAN_FC:
   		case OF1X_MATCH_WLAN_TYPE:
   		case OF1X_MATCH_WLAN_SUBTYPE:
   		case OF1X_MATCH_WLAN_DIRECTION:
   		case OF1X_MATCH_WLAN_ADDRESS_1:
   		case OF1X_MATCH_WLAN_ADDRESS_2:
   		case OF1X_MATCH_WLAN_ADDRESS_3:
   		case OF1X_MATCH_GRE_VERSION:
   		case OF1X_MATCH_GRE_PROT_TYPE:
   		case OF1X_MATCH_GRE_KEY:
   			break;

#endif
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
