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
*
* @warning: it MUST BE != NULL
*/
static inline bool __of1x_check_match(const packet_matches_t* pkt_matches, of1x_match_t* it){

	switch(it->type){
		//Phy
		case OF1X_MATCH_IN_PORT: return __utern_compare32(it->__tern, &pkt_matches->__port_in);
		case OF1X_MATCH_IN_PHY_PORT: if(!pkt_matches->__port_in) return false; //According to spec
					return __utern_compare32(it->__tern, &pkt_matches->__phy_port_in);
		//Metadata
	  	case OF1X_MATCH_METADATA: return __utern_compare64(it->__tern, &pkt_matches->__metadata); 
		
		//802
   		case OF1X_MATCH_ETH_DST:  return __utern_compare64(it->__tern, &pkt_matches->__eth_dst);
   		case OF1X_MATCH_ETH_SRC:  return __utern_compare64(it->__tern, &pkt_matches->__eth_src);
   		case OF1X_MATCH_ETH_TYPE: return __utern_compare16(it->__tern, &pkt_matches->__eth_type);
		
		//802.1q
   		case OF1X_MATCH_VLAN_VID: if( it->vlan_present == OF1X_MATCH_VLAN_SPECIFIC )
						return pkt_matches->__has_vlan && __utern_compare16(it->__tern, &pkt_matches->__vlan_vid);
					  else
						return pkt_matches->__has_vlan == it->vlan_present;
   		case OF1X_MATCH_VLAN_PCP: return pkt_matches->__has_vlan &&  __utern_compare8(it->__tern, &pkt_matches->__vlan_pcp);

		//MPLS
   		case OF1X_MATCH_MPLS_LABEL: if(!(pkt_matches->__eth_type == ETH_TYPE_MPLS_UNICAST || pkt_matches->__eth_type == ETH_TYPE_MPLS_MULTICAST )) return false;
					return __utern_compare32(it->__tern, &pkt_matches->__mpls_label);
   		case OF1X_MATCH_MPLS_TC: if(!(pkt_matches->__eth_type == ETH_TYPE_MPLS_UNICAST || pkt_matches->__eth_type == ETH_TYPE_MPLS_MULTICAST )) return false; 
					return __utern_compare8(it->__tern, &pkt_matches->__mpls_tc);
   		case OF1X_MATCH_MPLS_BOS: if(!(pkt_matches->__eth_type == ETH_TYPE_MPLS_UNICAST || pkt_matches->__eth_type == ETH_TYPE_MPLS_MULTICAST )) return false; 
					return __utern_compare8(it->__tern, (uint8_t*)&pkt_matches->__mpls_bos);
	
		//ARP
   		case OF1X_MATCH_ARP_OP: if(!(pkt_matches->__eth_type == ETH_TYPE_ARP)) return false;
   					return __utern_compare16(it->__tern, &pkt_matches->__arp_opcode);
   		case OF1X_MATCH_ARP_SHA: if(!(pkt_matches->__eth_type == ETH_TYPE_ARP)) return false;
   					return __utern_compare64(it->__tern, &pkt_matches->__arp_sha);
   		case OF1X_MATCH_ARP_SPA: if(!(pkt_matches->__eth_type == ETH_TYPE_ARP)) return false;
					return __utern_compare32(it->__tern, &pkt_matches->__arp_spa);
   		case OF1X_MATCH_ARP_THA: if(!(pkt_matches->__eth_type == ETH_TYPE_ARP)) return false;
   					return __utern_compare64(it->__tern, &pkt_matches->__arp_tha);
   		case OF1X_MATCH_ARP_TPA: if(!(pkt_matches->__eth_type == ETH_TYPE_ARP)) return false;
					return __utern_compare32(it->__tern, &pkt_matches->__arp_tpa);

		//NW (OF1.0 only)
   		case OF1X_MATCH_NW_PROTO: if(!(pkt_matches->__eth_type == ETH_TYPE_IPV4 || pkt_matches->__eth_type == ETH_TYPE_IPV6 || pkt_matches->__eth_type == ETH_TYPE_ARP || (pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION && (pkt_matches->__ppp_proto == PPP_PROTO_IP4 || pkt_matches->__ppp_proto == PPP_PROTO_IP6) ))) return false;
					if(pkt_matches->__eth_type == ETH_TYPE_ARP){
						uint8_t *low_byte = ((uint8_t*)&(pkt_matches->__arp_opcode));
						return __utern_compare8(it->__tern, ++low_byte);
					}
					else 
						return __utern_compare8(it->__tern, &pkt_matches->__ip_proto);
	
   		case OF1X_MATCH_NW_SRC:	if((pkt_matches->__eth_type == ETH_TYPE_IPV4 || (pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION && pkt_matches->__ppp_proto == PPP_PROTO_IP4 ))) 
						return __utern_compare32(it->__tern, &pkt_matches->__ipv4_src); 
					if(pkt_matches->__eth_type == ETH_TYPE_ARP)
						return __utern_compare32(it->__tern, &pkt_matches->__arp_spa); 
					return false;
   		case OF1X_MATCH_NW_DST:	if((pkt_matches->__eth_type == ETH_TYPE_IPV4 ||(pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION && pkt_matches->__ppp_proto == PPP_PROTO_IP4 )))  
						return __utern_compare32(it->__tern, &pkt_matches->__ipv4_dst);
					if(pkt_matches->__eth_type == ETH_TYPE_ARP)
						return __utern_compare32(it->__tern, &pkt_matches->__arp_tpa); 
					return false;
		//IP
   		case OF1X_MATCH_IP_PROTO: if(!(pkt_matches->__eth_type == ETH_TYPE_IPV4 || pkt_matches->__eth_type == ETH_TYPE_IPV6 || (pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION && (pkt_matches->__ppp_proto == PPP_PROTO_IP4 || pkt_matches->__ppp_proto == PPP_PROTO_IP6) ))) return false; 
					return __utern_compare8(it->__tern, &pkt_matches->__ip_proto);
		case OF1X_MATCH_IP_ECN: if(!(pkt_matches->__eth_type == ETH_TYPE_IPV4 || pkt_matches->__eth_type == ETH_TYPE_IPV6 || (pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION && pkt_matches->__ppp_proto == PPP_PROTO_IP4 ))) return false; //NOTE PPP_PROTO_IP6
					return __utern_compare8(it->__tern, &pkt_matches->__ip_ecn);
	
		case OF1X_MATCH_IP_DSCP: if(!(pkt_matches->__eth_type == ETH_TYPE_IPV4 || pkt_matches->__eth_type == ETH_TYPE_IPV6 || (pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION && pkt_matches->__ppp_proto == PPP_PROTO_IP4 ))) return false; //NOTE PPP_PROTO_IP6
					return __utern_compare8(it->__tern, &pkt_matches->__ip_dscp);
		
		//IPv4
   		case OF1X_MATCH_IPV4_SRC: if(!(pkt_matches->__eth_type == ETH_TYPE_IPV4 || (pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION && pkt_matches->__ppp_proto == PPP_PROTO_IP4 ))) return false; 
					return __utern_compare32(it->__tern, &pkt_matches->__ipv4_src);
   		case OF1X_MATCH_IPV4_DST:if(!(pkt_matches->__eth_type == ETH_TYPE_IPV4 ||(pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION && pkt_matches->__ppp_proto == PPP_PROTO_IP4 ))) return false;  
					return __utern_compare32(it->__tern, &pkt_matches->__ipv4_dst);
	
		//TCP
   		case OF1X_MATCH_TCP_SRC: if(!(pkt_matches->__ip_proto == IP_PROTO_TCP)) return false; 
					return __utern_compare16(it->__tern, &pkt_matches->__tcp_src);
   		case OF1X_MATCH_TCP_DST: if(!(pkt_matches->__ip_proto == IP_PROTO_TCP)) return false; 
					return __utern_compare16(it->__tern, &pkt_matches->__tcp_dst);
	
		//UDP
   		case OF1X_MATCH_UDP_SRC: if(!(pkt_matches->__ip_proto == IP_PROTO_UDP)) return false; 	
					return __utern_compare16(it->__tern, &pkt_matches->__udp_src);
   		case OF1X_MATCH_UDP_DST: if(!(pkt_matches->__ip_proto == IP_PROTO_UDP)) return false; 
					return __utern_compare16(it->__tern, &pkt_matches->__udp_dst);
		//SCTP
   		case OF1X_MATCH_SCTP_SRC: if(!(pkt_matches->__ip_proto == IP_PROTO_SCTP)) return false; 
					return __utern_compare16(it->__tern, &pkt_matches->__tcp_src);
   		case OF1X_MATCH_SCTP_DST: if(!(pkt_matches->__ip_proto == IP_PROTO_SCTP)) return false; 
					return __utern_compare16(it->__tern, &pkt_matches->__tcp_dst);
	
		//TP (OF1.0 only)
   		case OF1X_MATCH_TP_SRC: if((pkt_matches->__ip_proto == IP_PROTO_TCP))
						return __utern_compare16(it->__tern, &pkt_matches->__tcp_src);
   					if((pkt_matches->__ip_proto == IP_PROTO_UDP))
						return __utern_compare16(it->__tern, &pkt_matches->__udp_src);
					if((pkt_matches->__ip_proto == IP_PROTO_ICMPV4)){
						uint8_t two_byte[2] = {0,pkt_matches->__icmpv4_type};
						return __utern_compare16(it->__tern, (uint16_t*)&two_byte);
					}
					return false;

   		case OF1X_MATCH_TP_DST: if((pkt_matches->__ip_proto == IP_PROTO_TCP))
						return __utern_compare16(it->__tern, &pkt_matches->__tcp_dst);
   					if((pkt_matches->__ip_proto == IP_PROTO_UDP))
						return __utern_compare16(it->__tern, &pkt_matches->__udp_dst);
					if((pkt_matches->__ip_proto == IP_PROTO_ICMPV4)){
						uint8_t two_byte[2] = {0,pkt_matches->__icmpv4_code};
						return __utern_compare16(it->__tern, (uint16_t*)&two_byte);
					}
					return false;
		
		//ICMPv4
		case OF1X_MATCH_ICMPV4_TYPE: if(!(pkt_matches->__ip_proto == IP_PROTO_ICMPV4)) return false; 
					return __utern_compare8(it->__tern, &pkt_matches->__icmpv4_type);
   		case OF1X_MATCH_ICMPV4_CODE: if(!(pkt_matches->__ip_proto == IP_PROTO_ICMPV4)) return false; 
					return __utern_compare8(it->__tern, &pkt_matches->__icmpv4_code);
  		
		//IPv6
		case OF1X_MATCH_IPV6_SRC: if(!(pkt_matches->__eth_type == ETH_TYPE_IPV6 || (pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION && pkt_matches->__ppp_proto == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare128(it->__tern, &pkt_matches->__ipv6_src);
		case OF1X_MATCH_IPV6_DST: if(!(pkt_matches->__eth_type == ETH_TYPE_IPV6 || (pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION && pkt_matches->__ppp_proto == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare128(it->__tern, &pkt_matches->__ipv6_dst);
		case OF1X_MATCH_IPV6_FLABEL: if(!(pkt_matches->__eth_type == ETH_TYPE_IPV6 || (pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION && pkt_matches->__ppp_proto == PPP_PROTO_IP6 ))) return false; 
					return __utern_compare64(it->__tern, &pkt_matches->__ipv6_flabel);
		case OF1X_MATCH_IPV6_ND_TARGET: if(!(pkt_matches->__ip_proto == IP_PROTO_ICMPV6)) return false; 
					return __utern_compare128(it->__tern, &pkt_matches->__ipv6_nd_target);
		case OF1X_MATCH_IPV6_ND_SLL: if(!(pkt_matches->__ip_proto == IP_PROTO_ICMPV6 && pkt_matches->__ipv6_nd_sll)) return false; //NOTE OPTION SLL active
					return __utern_compare64(it->__tern, &pkt_matches->__ipv6_nd_sll);
		case OF1X_MATCH_IPV6_ND_TLL: if(!(pkt_matches->__ip_proto == IP_PROTO_ICMPV6 && pkt_matches->__ipv6_nd_tll)) return false; //NOTE OPTION TLL active
					return __utern_compare64(it->__tern, &pkt_matches->__ipv6_nd_tll);
		case OF1X_MATCH_IPV6_EXTHDR: //TODO not yet implemented.
			return false;
			break;
					
		//ICMPv6
		case OF1X_MATCH_ICMPV6_TYPE: if(!(pkt_matches->__ip_proto == IP_PROTO_ICMPV6)) return false; 
					return __utern_compare8(it->__tern, &pkt_matches->__icmpv6_type);
		case OF1X_MATCH_ICMPV6_CODE: if(!(pkt_matches->__ip_proto == IP_PROTO_ICMPV6 )) return false; 
					return __utern_compare8(it->__tern, &pkt_matches->__icmpv6_code);
			
		//PPPoE related extensions
   		case OF1X_MATCH_PPPOE_CODE: if(!(pkt_matches->__eth_type == ETH_TYPE_PPPOE_DISCOVERY || pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION )) return false;  
						return __utern_compare8(it->__tern, &pkt_matches->__pppoe_code);
   		case OF1X_MATCH_PPPOE_TYPE: if(!(pkt_matches->__eth_type == ETH_TYPE_PPPOE_DISCOVERY || pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare8(it->__tern, &pkt_matches->__pppoe_type);
   		case OF1X_MATCH_PPPOE_SID: if(!(pkt_matches->__eth_type == ETH_TYPE_PPPOE_DISCOVERY || pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare16(it->__tern, &pkt_matches->__pppoe_sid);

		//PPP 
   		case OF1X_MATCH_PPP_PROT: if(!(pkt_matches->__eth_type == ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare16(it->__tern, &pkt_matches->__ppp_proto);
	
		//PBB
   		case OF1X_MATCH_PBB_ISID: if(pkt_matches->__eth_type == ETH_TYPE_PBB) return false;	
						return __utern_compare32(it->__tern, &pkt_matches->__pbb_isid);
	 	//TUNNEL id
   		case OF1X_MATCH_TUNNEL_ID: return __utern_compare64(it->__tern, &pkt_matches->__tunnel_id);
 
		//GTP
   		case OF1X_MATCH_GTP_MSG_TYPE: if (!(pkt_matches->__ip_proto == IP_PROTO_UDP || pkt_matches->__udp_dst == UDP_DST_PORT_GTPU)) return false;
   						return __utern_compare8(it->__tern, &pkt_matches->__gtp_msg_type);
   		case OF1X_MATCH_GTP_TEID: if (!(pkt_matches->__ip_proto == IP_PROTO_UDP || pkt_matches->__udp_dst == UDP_DST_PORT_GTPU)) return false;
   						return __utern_compare32(it->__tern, &pkt_matches->__gtp_teid);
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
