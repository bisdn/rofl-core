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
*/
static inline bool __of1x_check_match(const packet_matches_t* pkt, of1x_match_t* it){

	if( unlikely(it==NULL ) )
		return false;
	
	switch(it->type){
		//Phy
		case OF1X_MATCH_IN_PORT: return __utern_compare32(it->value, &pkt->port_in);
		case OF1X_MATCH_IN_PHY_PORT: if(!pkt->port_in) return false; //According to spec
					return __utern_compare32(it->value, &pkt->phy_port_in);
		//Metadata
	  	case OF1X_MATCH_METADATA: return __utern_compare64(it->value, &pkt->metadata); 
		
		//802
   		case OF1X_MATCH_ETH_DST:  return __utern_compare64(it->value, &pkt->eth_dst);
   		case OF1X_MATCH_ETH_SRC:  return __utern_compare64(it->value, &pkt->eth_src);
   		case OF1X_MATCH_ETH_TYPE: return __utern_compare16(it->value, &pkt->eth_type);
		
		//802.1q
   		case OF1X_MATCH_VLAN_VID: 
					if( it->vlan_present != pkt->has_vlan )
						return false;
					return __utern_compare16(it->value, &pkt->vlan_vid);
   		case OF1X_MATCH_VLAN_PCP: if(!pkt->has_vlan) return false;
					return __utern_compare8(it->value, &pkt->vlan_pcp);

		//MPLS
   		case OF1X_MATCH_MPLS_LABEL: if(!(pkt->eth_type == ETH_TYPE_MPLS_UNICAST || pkt->eth_type == ETH_TYPE_MPLS_MULTICAST )) return false;
					return __utern_compare32(it->value, &pkt->mpls_label);
   		case OF1X_MATCH_MPLS_TC: if(!(pkt->eth_type == ETH_TYPE_MPLS_UNICAST || pkt->eth_type == ETH_TYPE_MPLS_MULTICAST )) return false; 
					return __utern_compare8(it->value, &pkt->mpls_tc);
   		case OF1X_MATCH_MPLS_BOS: if(!(pkt->eth_type == ETH_TYPE_MPLS_UNICAST || pkt->eth_type == ETH_TYPE_MPLS_MULTICAST )) return false; 
					return __utern_compare8(it->value, (uint8_t*)&pkt->mpls_bos);
	
		//ARP
   		case OF1X_MATCH_ARP_OP: if(!(pkt->eth_type == ETH_TYPE_ARP)) return false;
   					return __utern_compare16(it->value, &pkt->arp_opcode);
   		case OF1X_MATCH_ARP_SHA: if(!(pkt->eth_type == ETH_TYPE_ARP)) return false;
   					return __utern_compare64(it->value, &pkt->arp_sha);
   		case OF1X_MATCH_ARP_SPA: if(!(pkt->eth_type == ETH_TYPE_ARP)) return false;
					return __utern_compare32(it->value, &pkt->arp_spa);
   		case OF1X_MATCH_ARP_THA: if(!(pkt->eth_type == ETH_TYPE_ARP)) return false;
   					return __utern_compare64(it->value, &pkt->arp_tha);
   		case OF1X_MATCH_ARP_TPA: if(!(pkt->eth_type == ETH_TYPE_ARP)) return false;
					return __utern_compare32(it->value, &pkt->arp_tpa);

		//NW (OF1.0 only)
   		case OF1X_MATCH_NW_PROTO: if(!(pkt->eth_type == ETH_TYPE_IPV4 || pkt->eth_type == ETH_TYPE_IPV6 || pkt->eth_type == ETH_TYPE_ARP || (pkt->eth_type == ETH_TYPE_PPPOE_SESSION && (pkt->ppp_proto == PPP_PROTO_IP4 || pkt->ppp_proto == PPP_PROTO_IP6) ))) return false;
					if(pkt->eth_type == ETH_TYPE_ARP){
						uint8_t *low_byte = ((uint8_t*)&(pkt->arp_opcode));
						return __utern_compare8(it->value, ++low_byte);
					}
					else 
						return __utern_compare8(it->value, &pkt->ip_proto);
	
   		case OF1X_MATCH_NW_SRC:	if((pkt->eth_type == ETH_TYPE_IPV4 || (pkt->eth_type == ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == PPP_PROTO_IP4 ))) 
						return __utern_compare32(it->value, &pkt->ipv4_src); 
					if(pkt->eth_type == ETH_TYPE_ARP)
						return __utern_compare32(it->value, &pkt->arp_spa); 
					return false;
   		case OF1X_MATCH_NW_DST:	if((pkt->eth_type == ETH_TYPE_IPV4 ||(pkt->eth_type == ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == PPP_PROTO_IP4 )))  
						return __utern_compare32(it->value, &pkt->ipv4_dst);
					if(pkt->eth_type == ETH_TYPE_ARP)
						return __utern_compare32(it->value, &pkt->arp_tpa); 
					return false;
		//IP
   		case OF1X_MATCH_IP_PROTO: if(!(pkt->eth_type == ETH_TYPE_IPV4 || pkt->eth_type == ETH_TYPE_IPV6 || (pkt->eth_type == ETH_TYPE_PPPOE_SESSION && (pkt->ppp_proto == PPP_PROTO_IP4 || pkt->ppp_proto == PPP_PROTO_IP6) ))) return false; 
					return __utern_compare8(it->value, &pkt->ip_proto);
		case OF1X_MATCH_IP_ECN: if(!(pkt->eth_type == ETH_TYPE_IPV4 || pkt->eth_type == ETH_TYPE_IPV6 || (pkt->eth_type == ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == PPP_PROTO_IP4 ))) return false; //NOTE PPP_PROTO_IP6
					return __utern_compare8(it->value, &pkt->ip_ecn);
	
		case OF1X_MATCH_IP_DSCP: if(!(pkt->eth_type == ETH_TYPE_IPV4 || pkt->eth_type == ETH_TYPE_IPV6 || (pkt->eth_type == ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == PPP_PROTO_IP4 ))) return false; //NOTE PPP_PROTO_IP6
					return __utern_compare8(it->value, &pkt->ip_dscp);
		
		//IPv4
   		case OF1X_MATCH_IPV4_SRC: if(!(pkt->eth_type == ETH_TYPE_IPV4 || (pkt->eth_type == ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == PPP_PROTO_IP4 ))) return false; 
					return __utern_compare32(it->value, &pkt->ipv4_src);
   		case OF1X_MATCH_IPV4_DST:if(!(pkt->eth_type == ETH_TYPE_IPV4 ||(pkt->eth_type == ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == PPP_PROTO_IP4 ))) return false;  
					return __utern_compare32(it->value, &pkt->ipv4_dst);
	
		//TCP
   		case OF1X_MATCH_TCP_SRC: if(!(pkt->ip_proto == IP_PROTO_TCP)) return false; 
					return __utern_compare16(it->value, &pkt->tcp_src);
   		case OF1X_MATCH_TCP_DST: if(!(pkt->ip_proto == IP_PROTO_TCP)) return false; 
					return __utern_compare16(it->value, &pkt->tcp_dst);
	
		//UDP
   		case OF1X_MATCH_UDP_SRC: if(!(pkt->ip_proto == IP_PROTO_UDP)) return false; 	
					return __utern_compare16(it->value, &pkt->udp_src);
   		case OF1X_MATCH_UDP_DST: if(!(pkt->ip_proto == IP_PROTO_UDP)) return false; 
					return __utern_compare16(it->value, &pkt->udp_dst);
		//SCTP
   		case OF1X_MATCH_SCTP_SRC: if(!(pkt->ip_proto == IP_PROTO_SCTP)) return false; 
					return __utern_compare16(it->value, &pkt->tcp_src);
   		case OF1X_MATCH_SCTP_DST: if(!(pkt->ip_proto == IP_PROTO_SCTP)) return false; 
					return __utern_compare16(it->value, &pkt->tcp_dst);
	
		//TP (OF1.0 only)
   		case OF1X_MATCH_TP_SRC: if((pkt->ip_proto == IP_PROTO_TCP))
						return __utern_compare16(it->value, &pkt->tcp_src);
   					if((pkt->ip_proto == IP_PROTO_UDP))
						return __utern_compare16(it->value, &pkt->udp_src);
					if((pkt->ip_proto == IP_PROTO_ICMPV4)){
						uint8_t two_byte[2] = {0,pkt->icmpv4_type};
						return __utern_compare16(it->value, (uint16_t*)&two_byte);
					}
					return false;

   		case OF1X_MATCH_TP_DST: if((pkt->ip_proto == IP_PROTO_TCP))
						return __utern_compare16(it->value, &pkt->tcp_dst);
   					if((pkt->ip_proto == IP_PROTO_UDP))
						return __utern_compare16(it->value, &pkt->udp_dst);
					if((pkt->ip_proto == IP_PROTO_ICMPV4)){
						uint8_t two_byte[2] = {0,pkt->icmpv4_code};
						return __utern_compare16(it->value, (uint16_t*)&two_byte);
					}
					return false;
		
		//ICMPv4
		case OF1X_MATCH_ICMPV4_TYPE: if(!(pkt->ip_proto == IP_PROTO_ICMPV4)) return false; 
					return __utern_compare8(it->value, &pkt->icmpv4_type);
   		case OF1X_MATCH_ICMPV4_CODE: if(!(pkt->ip_proto == IP_PROTO_ICMPV4)) return false; 
					return __utern_compare8(it->value, &pkt->icmpv4_code);
  		
		//IPv6
		case OF1X_MATCH_IPV6_SRC: if(!(pkt->eth_type == ETH_TYPE_IPV6 || (pkt->eth_type == ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare128(it->value, &pkt->ipv6_src);
		case OF1X_MATCH_IPV6_DST: if(!(pkt->eth_type == ETH_TYPE_IPV6 || (pkt->eth_type == ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare128(it->value, &pkt->ipv6_dst);
		case OF1X_MATCH_IPV6_FLABEL: if(!(pkt->eth_type == ETH_TYPE_IPV6 || (pkt->eth_type == ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare64(it->value, &pkt->ipv6_flabel);
		case OF1X_MATCH_IPV6_ND_TARGET: if(!(pkt->ip_proto == IP_PROTO_ICMPV6)) return false; 
					return __utern_compare128(it->value, &pkt->ipv6_nd_target);
		case OF1X_MATCH_IPV6_ND_SLL: if(!(pkt->ip_proto == IP_PROTO_ICMPV6 && pkt->ipv6_nd_sll)) return false; //NOTE OPTION SLL active
					return __utern_compare64(it->value, &pkt->ipv6_nd_sll);
		case OF1X_MATCH_IPV6_ND_TLL: if(!(pkt->ip_proto == IP_PROTO_ICMPV6 && pkt->ipv6_nd_tll)) return false; //NOTE OPTION TLL active
					return __utern_compare64(it->value, &pkt->ipv6_nd_tll);
		case OF1X_MATCH_IPV6_EXTHDR: //TODO not yet implemented.
			return false;
			break;
					
		//ICMPv6
		case OF1X_MATCH_ICMPV6_TYPE: if(!(pkt->ip_proto == IP_PROTO_ICMPV6)) return false; 
					return __utern_compare8(it->value, &pkt->icmpv6_type);
		case OF1X_MATCH_ICMPV6_CODE: if(!(pkt->ip_proto == IP_PROTO_ICMPV6 )) return false; 
					return __utern_compare8(it->value, &pkt->icmpv6_code);
			
		//PPPoE related extensions
   		case OF1X_MATCH_PPPOE_CODE: if(!(pkt->eth_type == ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == ETH_TYPE_PPPOE_SESSION )) return false;  
						return __utern_compare8(it->value, &pkt->pppoe_code);
   		case OF1X_MATCH_PPPOE_TYPE: if(!(pkt->eth_type == ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare8(it->value, &pkt->pppoe_type);
   		case OF1X_MATCH_PPPOE_SID: if(!(pkt->eth_type == ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare16(it->value, &pkt->pppoe_sid);

		//PPP 
   		case OF1X_MATCH_PPP_PROT: if(!(pkt->eth_type == ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare16(it->value, &pkt->ppp_proto);
	
		//PBB
   		case OF1X_MATCH_PBB_ISID: if(pkt->eth_type == ETH_TYPE_PBB) return false;	
						return __utern_compare32(it->value, &pkt->pbb_isid);
	 	//TUNNEL id
   		case OF1X_MATCH_TUNNEL_ID: return __utern_compare64(it->value, &pkt->tunnel_id);
 
		//GTP
   		case OF1X_MATCH_GTP_MSG_TYPE: if (!(pkt->ip_proto == IP_PROTO_UDP || pkt->udp_dst == UDP_DST_PORT_GTPU)) return false;
   						return __utern_compare8(it->value, &pkt->gtp_msg_type);
   		case OF1X_MATCH_GTP_TEID: if (!(pkt->ip_proto == IP_PROTO_UDP || pkt->udp_dst == UDP_DST_PORT_GTPU)) return false;
   						return __utern_compare32(it->value, &pkt->gtp_teid);
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
