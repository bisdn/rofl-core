#include <assert.h>
#include "of12_match.h"

#include "../../../common/datapacket.h"
#include "../../../platform/memory.h"
#include "../../../util/logging.h"

/*
* Initializers 
*/

//Phy
inline of12_match_t* of12_init_port_in_match(of12_match_t* prev, of12_match_t* next, uint32_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IN_PORT; 
	match->value = __init_utern32(value,0xFFFFFFFF); //No wildcard
	match->prev = prev;
	match->next = next;
	return match;
}

inline of12_match_t* of12_init_port_in_phy_match(of12_match_t* prev, of12_match_t* next, uint32_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IN_PHY_PORT; 
	match->value = __init_utern32(value,0xFFFFFFFF); //No wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

/* TODO: add metadata */

//ETHERNET
inline of12_match_t* of12_init_eth_dst_match(of12_match_t* prev, of12_match_t* next, uint64_t value, uint64_t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ETH_DST; 
	match->value = __init_utern64(value&UINT64_C(0x0000FFFFFFFFFFFF), mask&UINT64_C(0x0000FFFFFFFFFFFF)); //Enforce mask bits are always 00 for the first bits

	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_eth_src_match(of12_match_t* prev, of12_match_t* next, uint64_t value, uint64_t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ETH_SRC; 
	match->value = __init_utern64(value&UINT64_C(0x0000FFFFFFFFFFFF), mask&UINT64_C(0x0000FFFFFFFFFFFF)); //Enforce mask bits are always 00 for the first bits
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_eth_type_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ETH_TYPE; 
	match->value = __init_utern16(value,0xFFFF); //No wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

//8021.q
inline of12_match_t* of12_init_vlan_vid_match(of12_match_t* prev, of12_match_t* next, uint16_t value, uint16_t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_VLAN_VID; 
	match->value = __init_utern16(value&0x1FFF,mask&0x1FFF); //Ensure only 13 bit value
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_vlan_pcp_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_VLAN_PCP; 
	match->value = __init_utern8(value&0x07,0x07); //Ensure only 3 bit value, no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

//MPLS
inline of12_match_t* of12_init_mpls_label_match(of12_match_t* prev, of12_match_t* next, uint32_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_MPLS_LABEL; 
	match->value = __init_utern32(value&0x000FFFFF,0x000FFFFF); //no wildcard?? wtf! 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_mpls_tc_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_MPLS_TC; 
	match->value = __init_utern8(value&0x07,0x07); //Ensure only 3 bit value, no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

//ARP
inline of12_match_t* of12_init_arp_opcode_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ARP_OP;
	match->value = __init_utern16(value,0xFFFF); //No wildcard
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_arp_tha_match(of12_match_t* prev, of12_match_t* next, uint64_t value, uint64_t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ARP_THA;
	match->value = __init_utern64(value&UINT64_C(0x0000FFFFFFFFFFFF), mask&UINT64_C(0x0000FFFFFFFFFFFF)); //Enforce mask bits are always 00 for the first bits

	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_arp_sha_match(of12_match_t* prev, of12_match_t* next, uint64_t value, uint64_t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ARP_SHA;
	match->value = __init_utern64(value&UINT64_C(0x0000FFFFFFFFFFFF), mask&UINT64_C(0x0000FFFFFFFFFFFF)); //Enforce mask bits are always 00 for the first bits
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_arp_tpa_match(of12_match_t* prev, of12_match_t* next, uint32_t value, uint32_t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ARP_TPA;
	match->value = __init_utern32(value,mask);
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_arp_spa_match(of12_match_t* prev, of12_match_t* next, uint32_t value, uint32_t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ARP_SPA;
	match->value = __init_utern32(value,mask);
	match->prev = prev;
	match->next = next;
	return match;
}

//PPPoE
inline of12_match_t* of12_init_pppoe_code_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_PPPOE_CODE; 
	match->value = __init_utern8(value,0xFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_pppoe_type_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_PPPOE_TYPE; 
	match->value = __init_utern8(value&0x0F,0x0F); //Ensure only 4 bit value, no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_pppoe_session_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_PPPOE_SID; 
	match->value = __init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
//PPP
inline of12_match_t* of12_init_ppp_prot_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_PPP_PROT; 
	match->value = __init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
//IPv4
inline of12_match_t* of12_init_ip4_src_match(of12_match_t* prev, of12_match_t* next, uint32_t value, uint32_t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IPV4_SRC;
	match->value = __init_utern32(value,mask); 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip4_dst_match(of12_match_t* prev, of12_match_t* next, uint32_t value, uint32_t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IPV4_DST;
	match->value = __init_utern32(value,mask); 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip_proto_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IP_PROTO; 
	match->value = __init_utern8(value,0xFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip_dscp_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IP_DSCP; 
	match->value = __init_utern8(value,0x3F); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

inline of12_match_t* of12_init_ip_ecn_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IP_ECN; 
	match->value = __init_utern8(value,0x03); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

//IPv6
inline of12_match_t* of12_init_ip6_src_match(of12_match_t* prev, of12_match_t* next, uint128__t value, uint128__t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IPV6_SRC;
	match->value = __init_utern128(value,mask); 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip6_dst_match(of12_match_t* prev, of12_match_t* next, uint128__t value, uint128__t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IPV6_DST;
	match->value = __init_utern128(value,mask); 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip6_flabel_match(of12_match_t* prev, of12_match_t* next, uint64_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IPV6_FLABEL;
	match->value = __init_utern64(value&0x00000000000FFFFF,0x00000000000FFFFF); // ensure 20 bits. No wildcard
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip6_nd_target_match(of12_match_t* prev, of12_match_t* next, uint128__t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
    uint128__t mask = {{0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
						0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff}};
	match->type = OF12_MATCH_IPV6_ND_TARGET;
	match->value = __init_utern128(value,mask); //No wildcard
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip6_nd_sll_match(of12_match_t* prev, of12_match_t* next, uint64_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IPV6_ND_SLL;
	match->value = __init_utern64(value&0x0000ffffffffffff,0x0000ffffffffffff); //ensure 48 bits. No wildcard
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip6_nd_tll_match(of12_match_t* prev, of12_match_t* next, uint64_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IPV6_ND_TLL;
	match->value = __init_utern64(value&0x0000ffffffffffff,0x0000ffffffffffff); //ensure 48 bits. No wildcard
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip6_exthdr_match(of12_match_t* prev, of12_match_t* next, uint64_t value, uint64_t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IPV6_EXTHDR;
	match->value = __init_utern64(value&0x00000000000001ff,mask);  //ensure 9 bits, with Wildcard
	match->prev = prev;
	match->next = next;
	return match;
}

//ICMPV6
inline of12_match_t* of12_init_icmpv6_type_match(of12_match_t* prev, of12_match_t* next, uint64_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ICMPV6_TYPE;
	match->value = __init_utern64(value&0x00000000000000FF,0x00000000000000FF); //ensure 8 bits. No wildcard
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_icmpv6_code_match(of12_match_t* prev, of12_match_t* next, uint64_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ICMPV6_CODE;
	match->value = __init_utern64(value&0x00000000000000FF,0x00000000000000FF); //ensure 8 bits. No wildcard
	match->prev = prev;
	match->next = next;
	return match;
}

//TCP
inline of12_match_t* of12_init_tcp_src_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_TCP_SRC;
	match->value = __init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_tcp_dst_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_TCP_DST;
	match->value = __init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
//UDP
inline of12_match_t* of12_init_udp_src_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_UDP_SRC;
	match->value = __init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_udp_dst_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_UDP_DST;
	match->value = __init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
//ICMPv4
inline of12_match_t* of12_init_icmpv4_type_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ICMPV4_TYPE; 
	match->value = __init_utern8(value,0xFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_icmpv4_code_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ICMPV4_CODE; 
	match->value = __init_utern8(value,0xFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

//GTP
inline of12_match_t* of12_init_gtp_msg_type_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_GTP_MSG_TYPE;
	match->value = __init_utern8(value,0xFF); //no wildcard
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_gtp_teid_match(of12_match_t* prev, of12_match_t* next, uint32_t value, uint32_t mask){
	of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_GTP_TEID;
	match->value = __init_utern32(value, mask);
	match->prev = prev;
	match->next = next;
	return match;
}

//Add more here...

//TODO
#if 0
/* Instruction groups init and destroy */
of12_match_group_t* of12_new_match_group(){

	of12_match_group_t *group = (of12_match_group_t*)platform_malloc_shared(sizeof(of12_match_group_t));

	if(!group)
		return NULL; 

	of12_init_match_group(group);

	return group;
}
#endif



/* Instruction groups init and destroy */
void __of12_init_match_group(of12_match_group_t* group){

	memset(group,0,sizeof(of12_match_group_t));
}


void __of12_destroy_match_group(of12_match_group_t* group){
	of12_match_t *match;

	if (!group->head)
		return;

	match = group->head;

	while (match)
	{
		of12_match_t *next = match->next;
		of12_destroy_match(match);
		match = next;
	}

	group->head = NULL; 
	group->tail = NULL; 
}



void __of12_match_group_push_back(of12_match_group_t* group, of12_match_t* match){

	if (!group)
		return;

	match->next = match->prev = NULL; 

	if(!group->head){
		group->head = group->tail = match;
	}
	else{
		match->prev = group->tail;
		group->tail->next = match;
		group->tail = match;
	}
}



/* Push match at the end of the match */
rofl_result_t __of12_add_match(of12_match_t* root_match, of12_match_t* add_match){
	of12_match_t* it;
	
	it = root_match;

	if(!it)
		return ROFL_FAILURE;
	
	while(it->next)
		it = it->next;
	
	//Last match in the list
	it->next = add_match;
	add_match->prev = it; 
	
	return ROFL_SUCCESS;
}
/*
* Copy match to heap. Leaves next and prev pointers to NULL
*/
inline of12_match_t* __of12_copy_match(of12_match_t* match){
	switch(match->type){

		case OF12_MATCH_IN_PORT: return of12_init_port_in_match(NULL, NULL, match->value->value.u32);
		case OF12_MATCH_IN_PHY_PORT: return of12_init_port_in_phy_match(NULL, NULL, match->value->value.u32);

	  	case OF12_MATCH_METADATA: //TODO FIXME
					return NULL; 
   
		case OF12_MATCH_ETH_DST:  return of12_init_eth_dst_match(NULL,NULL,match->value->value.u64,match->value->mask.u64); 
   		case OF12_MATCH_ETH_SRC:  return  of12_init_eth_src_match(NULL,NULL,match->value->value.u64,match->value->mask.u64);
   		case OF12_MATCH_ETH_TYPE: return of12_init_eth_type_match(NULL,NULL,match->value->value.u16);

   		case OF12_MATCH_VLAN_VID: return of12_init_vlan_vid_match(NULL,NULL,match->value->value.u16,match->value->mask.u16); 
   		case OF12_MATCH_VLAN_PCP: return of12_init_vlan_pcp_match(NULL,NULL,match->value->value.u8); 

   		case OF12_MATCH_MPLS_LABEL: return of12_init_mpls_label_match(NULL,NULL,match->value->value.u32); 
   		case OF12_MATCH_MPLS_TC: return of12_init_mpls_tc_match(NULL,NULL,match->value->value.u8); 

   		case OF12_MATCH_ARP_OP: return of12_init_arp_opcode_match(NULL,NULL,match->value->value.u16);
   		case OF12_MATCH_ARP_SHA: return of12_init_arp_sha_match(NULL,NULL,match->value->value.u64,match->value->mask.u64);
   		case OF12_MATCH_ARP_SPA: return of12_init_arp_spa_match(NULL,NULL,match->value->value.u32,match->value->mask.u32);
   		case OF12_MATCH_ARP_THA: return of12_init_arp_tha_match(NULL,NULL,match->value->value.u64,match->value->mask.u64);
   		case OF12_MATCH_ARP_TPA: return of12_init_arp_tpa_match(NULL,NULL,match->value->value.u32,match->value->mask.u32);

		case OF12_MATCH_IP_PROTO: return of12_init_ip_proto_match(NULL,NULL,match->value->value.u8); 
   		case OF12_MATCH_IP_ECN: return of12_init_ip_ecn_match(NULL,NULL,match->value->value.u8); 
   		case OF12_MATCH_IP_DSCP: return of12_init_ip_dscp_match(NULL,NULL,match->value->value.u8);

   		case OF12_MATCH_IPV4_SRC: return of12_init_ip4_src_match(NULL,NULL,match->value->value.u32,match->value->mask.u32); 
   		case OF12_MATCH_IPV4_DST: return of12_init_ip4_dst_match(NULL,NULL,match->value->value.u32,match->value->mask.u32); 

   		case OF12_MATCH_TCP_SRC: return of12_init_tcp_src_match(NULL,NULL,match->value->value.u16); 
   		case OF12_MATCH_TCP_DST: return of12_init_tcp_dst_match(NULL,NULL,match->value->value.u16); 

   		case OF12_MATCH_UDP_SRC: return of12_init_udp_src_match(NULL,NULL,match->value->value.u16); 
   		case OF12_MATCH_UDP_DST: return of12_init_udp_dst_match(NULL,NULL,match->value->value.u16); 

		case OF12_MATCH_ICMPV4_TYPE: return of12_init_icmpv4_type_match(NULL,NULL,match->value->value.u8); 
   		case OF12_MATCH_ICMPV4_CODE: return of12_init_icmpv4_code_match(NULL,NULL,match->value->value.u8); 
  		
		case OF12_MATCH_IPV6_SRC: return of12_init_ip6_src_match(NULL,NULL,match->value->value.u128, match->value->mask.u128);
		case OF12_MATCH_IPV6_DST: return of12_init_ip6_dst_match(NULL,NULL,match->value->value.u128, match->value->mask.u128);
		case OF12_MATCH_IPV6_FLABEL: return of12_init_ip6_flabel_match(NULL,NULL,match->value->value.u64);
		case OF12_MATCH_IPV6_ND_TARGET: return of12_init_ip6_nd_target_match(NULL,NULL,match->value->value.u128);
		case OF12_MATCH_IPV6_ND_SLL: return of12_init_ip6_nd_sll_match(NULL,NULL,match->value->value.u64);
		case OF12_MATCH_IPV6_ND_TLL: return of12_init_ip6_nd_tll_match(NULL,NULL,match->value->value.u64);
		case OF12_MATCH_IPV6_EXTHDR: return of12_init_ip6_exthdr_match(NULL,NULL,match->value->value.u64, match->value->mask.u64);
		
		case OF12_MATCH_ICMPV6_TYPE: return of12_init_icmpv6_type_match(NULL,NULL,match->value->value.u64);
		case OF12_MATCH_ICMPV6_CODE: return of12_init_icmpv6_code_match(NULL,NULL,match->value->value.u64);
		
		/* PPP/PPPoE related extensions */
   		case OF12_MATCH_PPPOE_CODE: return of12_init_pppoe_code_match(NULL,NULL,match->value->value.u8); 
   		case OF12_MATCH_PPPOE_TYPE: return of12_init_pppoe_type_match(NULL,NULL,match->value->value.u8); 
   		case OF12_MATCH_PPPOE_SID: return of12_init_pppoe_session_match(NULL,NULL,match->value->value.u16); 
   		case OF12_MATCH_PPP_PROT: return of12_init_ppp_prot_match(NULL,NULL,match->value->value.u16); 

   		/* GTP related extensions */
   		case OF12_MATCH_GTP_MSG_TYPE: return of12_init_gtp_msg_type_match(NULL,NULL,match->value->value.u8);
   		case OF12_MATCH_GTP_TEID: return of12_init_gtp_teid_match(NULL,NULL,match->value->value.u32,match->value->mask.u32);

		/* Add more here ...*/
		default:
			//Should NEVER reach this point
			assert(0);
			return NULL; 
	}	
	
}

/* 
* Whole (linked list) Match copy -> this should be deprecated in favour of the match group
*/
of12_match_t* __of12_copy_matches(of12_match_t* matches){

	of12_match_t* prev, *curr, *it, *copy;
	
	if(!matches)
		return NULL;
	
	for(prev=NULL,copy=NULL, it=matches; it; it = it->next){

		curr = __of12_copy_match(it);

		if(!curr){
			//FIXME: attempt to delete previous
			return NULL;
		}	

		//Set initial match
		if(!copy)
			copy = curr;

		if(prev)
			prev->next = curr;

		curr->prev = prev;	
		prev = curr;
	}

	return copy;	
}



/*
* Try to find the largest common value among match1 and match2, being ALWAYS match2 with a more strict mask 
*/
inline of12_match_t* __of12_get_alike_match(of12_match_t* match1, of12_match_t* match2){
	utern_t* common_tern = NULL;	

	if( match1->type != match2->type )
		return NULL;	

	common_tern = __utern_get_alike(*match1->value,*match2->value);

	if(common_tern){
		of12_match_t* match = (of12_match_t*)platform_malloc_shared(sizeof(of12_match_t));
		match->value = common_tern;
		match->type = match1->type;
		match->next = NULL;
		match->prev = NULL;
		return match;
	}
	return NULL;
}
/*
* Common destructor
*/
void of12_destroy_match(of12_match_t* match){
	__destroy_utern(match->value);
	platform_free_shared(match);
}

/*
*
* Matching routines...
* 
*/

//Compare matches
inline bool __of12_equal_matches(of12_match_t* match1, of12_match_t* match2){

	if( match1->type != match2->type )
		return false; 

	return __utern_equals(match1->value,match2->value);
}

//Finds out if sub_match is a submatch of match
inline bool __of12_is_submatch(of12_match_t* sub_match, of12_match_t* match){

	if( match->type != sub_match->type )
		return false; 
	
	return __utern_is_contained(sub_match->value,match->value);
}
/*
*
* CHECK fields against packet
*
*/
inline bool __of12_check_match(const of12_packet_matches_t* pkt, of12_match_t* it){
	if(!it)
		return false;
	
	switch(it->type){
		//Phy
		case OF12_MATCH_IN_PORT: return __utern_compare32(it->value,pkt->port_in);
		case OF12_MATCH_IN_PHY_PORT: if(!pkt->port_in) return false; //According to spec
					return __utern_compare32(it->value,pkt->phy_port_in);
		//Metadata
	  	case OF12_MATCH_METADATA: //TODO FIXME
					return false;
		//802
   		case OF12_MATCH_ETH_DST:  return __utern_compare64(it->value,pkt->eth_dst);
   		case OF12_MATCH_ETH_SRC:  return __utern_compare64(it->value,pkt->eth_src);
   		case OF12_MATCH_ETH_TYPE: return __utern_compare16(it->value,pkt->eth_type);
		
		//802.1q
   		case OF12_MATCH_VLAN_VID: return __utern_compare16(it->value,pkt->vlan_vid);
   		case OF12_MATCH_VLAN_PCP: if(!pkt->vlan_vid) return false;
					return __utern_compare8(it->value,pkt->vlan_pcp);
		//ARP
   		case OF12_MATCH_ARP_OP: if(!(pkt->eth_type == OF12_ETH_TYPE_ARP)) return false;
   					return __utern_compare16(it->value,pkt->arp_opcode);
   		case OF12_MATCH_ARP_SHA: if(!(pkt->eth_type == OF12_ETH_TYPE_ARP)) return false;
   					return __utern_compare64(it->value,pkt->arp_sha);
   		case OF12_MATCH_ARP_SPA: if(!(pkt->eth_type == OF12_ETH_TYPE_ARP)) return false;
					return __utern_compare32(it->value, pkt->arp_spa);
   		case OF12_MATCH_ARP_THA: if(!(pkt->eth_type == OF12_ETH_TYPE_ARP)) return false;
   					return __utern_compare64(it->value,pkt->arp_tha);
   		case OF12_MATCH_ARP_TPA: if(!(pkt->eth_type == OF12_ETH_TYPE_ARP)) return false;
					return __utern_compare32(it->value, pkt->arp_tpa);
		//MPLS
   		case OF12_MATCH_MPLS_LABEL: if(!(pkt->eth_type == OF12_ETH_TYPE_MPLS_UNICAST || pkt->eth_type == OF12_ETH_TYPE_MPLS_MULTICAST )) return false;
					return __utern_compare32(it->value,pkt->mpls_label);
   		case OF12_MATCH_MPLS_TC: if(!(pkt->eth_type == OF12_ETH_TYPE_MPLS_UNICAST || pkt->eth_type == OF12_ETH_TYPE_MPLS_MULTICAST )) return false; 
					return __utern_compare8(it->value,pkt->mpls_tc);
		//IP
   		case OF12_MATCH_IP_PROTO: if(!(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 || (pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && (pkt->ppp_proto == OF12_PPP_PROTO_IP4 || pkt->ppp_proto == OF12_PPP_PROTO_IP6) ))) return false; 
					return __utern_compare8(it->value,pkt->ip_proto);
		case OF12_MATCH_IP_ECN: if(!(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 || (pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP4 ))) return false; //NOTE OF12_PPP_PROTO_IP6
					return __utern_compare8(it->value,pkt->ip_ecn);
	
		case OF12_MATCH_IP_DSCP: if(!(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 || (pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP4 ))) return false; //NOTE OF12_PPP_PROTO_IP6
					return __utern_compare8(it->value,pkt->ip_dscp);
		
		//IPv4
   		case OF12_MATCH_IPV4_SRC: if(!(pkt->eth_type == OF12_ETH_TYPE_IPV4 || (pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP4 ))) return false; 
					return __utern_compare32(it->value, pkt->ipv4_src);
   		case OF12_MATCH_IPV4_DST:if(!(pkt->eth_type == OF12_ETH_TYPE_IPV4 ||(pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP4 ))) return false;  
					return __utern_compare32(it->value, pkt->ipv4_dst);
	
		//TCP
   		case OF12_MATCH_TCP_SRC: if(!(pkt->ip_proto == OF12_IP_PROTO_TCP)) return false; 
					return __utern_compare16(it->value,pkt->tcp_src);
   		case OF12_MATCH_TCP_DST: if(!(pkt->ip_proto == OF12_IP_PROTO_TCP)) return false; 
					return __utern_compare16(it->value,pkt->tcp_dst);
	
		//UDP
   		case OF12_MATCH_UDP_SRC: if(!(pkt->ip_proto == OF12_IP_PROTO_UDP)) return false; 	
					return __utern_compare16(it->value,pkt->udp_src);
   		case OF12_MATCH_UDP_DST: if(!(pkt->ip_proto == OF12_IP_PROTO_UDP)) return false; 
					return __utern_compare16(it->value,pkt->udp_dst);
		
		//ICMPv4
		case OF12_MATCH_ICMPV4_TYPE: if(!(pkt->ip_proto == OF12_IP_PROTO_ICMPV4)) return false; 
					return __utern_compare8(it->value,pkt->icmpv4_type);
   		case OF12_MATCH_ICMPV4_CODE: if(!(pkt->ip_proto == OF12_IP_PROTO_ICMPV4)) return false; 
					return __utern_compare8(it->value,pkt->icmpv4_code);
  		
		//IPv6
		case OF12_MATCH_IPV6_SRC: if(!(pkt->eth_type == OF12_ETH_TYPE_IPV6 || (pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP6 ))) return false; 
					return __utern_compare128(it->value, pkt->ipv6_src);
		case OF12_MATCH_IPV6_DST: if(!(pkt->eth_type == OF12_ETH_TYPE_IPV6 || (pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP6 ))) return false; 
					return __utern_compare128(it->value, pkt->ipv6_dst);
		case OF12_MATCH_IPV6_FLABEL: if(!(pkt->eth_type == OF12_ETH_TYPE_IPV6 || (pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP6 ))) return false; 
					return __utern_compare64(it->value, pkt->ipv6_flabel);
		case OF12_MATCH_IPV6_ND_TARGET: if(!(pkt->ip_proto == OF12_IP_PROTO_ICMPV6)) return false; 
					return __utern_compare128(it->value,pkt->ipv6_nd_target);
		case OF12_MATCH_IPV6_ND_SLL: if(!(pkt->ip_proto == OF12_IP_PROTO_ICMPV6 && pkt->ipv6_nd_sll)) return false; //NOTE OPTION SLL active
					return __utern_compare64(it->value, pkt->ipv6_nd_sll);
		case OF12_MATCH_IPV6_ND_TLL: if(!(pkt->ip_proto == OF12_IP_PROTO_ICMPV6 && pkt->ipv6_nd_tll)) return false; //NOTE OPTION TLL active
					return __utern_compare64(it->value, pkt->ipv6_nd_tll);
		case OF12_MATCH_IPV6_EXTHDR: //TODO not yet implemented.
			return false;
			break;
					
		//ICMPv6
		case OF12_MATCH_ICMPV6_TYPE: if(!(pkt->ip_proto == OF12_IP_PROTO_ICMPV6)) return false; 
					return __utern_compare64(it->value, pkt->icmpv6_type);
		case OF12_MATCH_ICMPV6_CODE: if(!(pkt->ip_proto == OF12_IP_PROTO_ICMPV6 )) return false; 
					return __utern_compare64(it->value, pkt->icmpv6_code);
			
		//PPPoE related extensions
   		case OF12_MATCH_PPPOE_CODE: if(!(pkt->eth_type == OF12_ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION )) return false;  
						return __utern_compare8(it->value,pkt->pppoe_code);
   		case OF12_MATCH_PPPOE_TYPE: if(!(pkt->eth_type == OF12_ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare8(it->value,pkt->pppoe_type);
   		case OF12_MATCH_PPPOE_SID: if(!(pkt->eth_type == OF12_ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare16(it->value,pkt->pppoe_sid);

		//PPP 
   		case OF12_MATCH_PPP_PROT: if(!(pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION )) return false; 
						return __utern_compare16(it->value,pkt->ppp_proto);

		//GTP
   		case OF12_MATCH_GTP_MSG_TYPE: if (!(pkt->ip_proto == OF12_IP_PROTO_UDP || pkt->udp_dst == OF12_UDP_DST_PORT_GTPU)) return false;
   						return __utern_compare8(it->value,pkt->gtp_msg_type);
   		case OF12_MATCH_GTP_TEID: if (!(pkt->ip_proto == OF12_IP_PROTO_UDP || pkt->udp_dst == OF12_UDP_DST_PORT_GTPU)) return false;
   						return __utern_compare32(it->value,pkt->gtp_teid);

		// Add more here ...
		default:
			//Should NEVER reach this point; TODO, add trace for debugging?
			assert(0);
			return false;
	}
}


/* 
* DEBUG/INFO dumping routines 
*/

//Dump packet matches
void of12_dump_packet_matches(of_packet_matches_t *const pkt_matches){

	of12_packet_matches_t *const pkt = &pkt_matches->of12;

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
	//TODO:Metadata	
	//if(pkt->metadata)
	//	ROFL_PIPELINE_DEBUG_NO_PREFIX("METADATA:%u, ",pkt->metadata);
	//802	
	if(pkt->eth_src)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ETH_SRC:0x%llx, ",(long long unsigned)pkt->eth_src);
	if(pkt->eth_dst)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ETH_DST:0x%llx, ",(long long unsigned)pkt->eth_dst);
	if(pkt->eth_type)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ETH_TYPE:0x%x, ",pkt->eth_type);
	//802.1q
	if(pkt->vlan_vid)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("VLAN_VID:%u, ",pkt->vlan_vid);
	if(pkt->vlan_pcp)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("VLAN_PCP:%u, ",pkt->vlan_pcp);
	//ARP
	if(pkt->eth_type == OF12_ETH_TYPE_ARP)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ARP_OPCODE:0x%x, ",pkt->arp_opcode);
	if(pkt->eth_type == OF12_ETH_TYPE_ARP)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ARP_SHA:0x%llx, ",(long long unsigned)pkt->arp_sha);
	if(pkt->eth_type == OF12_ETH_TYPE_ARP)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ARP_SPA:0x%x, ",pkt->arp_spa);
	if(pkt->eth_type == OF12_ETH_TYPE_ARP)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ARP_THA:0x%llx, ",(long long unsigned)pkt->arp_tha);
	if(pkt->eth_type == OF12_ETH_TYPE_ARP)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ARP_TPA:0x%x, ",pkt->arp_tpa);
	//IP/IPv4
	if(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 )
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IP_PROTO:%u, ",pkt->ip_proto);

	if(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 )
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IP_ECN:0x%x, ",pkt->ip_ecn);
	
	if(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 )
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
	//ICMPV4
	if(pkt->ip_proto == OF12_IP_PROTO_ICMPV4)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ICMPV4_TYPE:%u, ICMPV4_CODE:%u, ",pkt->icmpv4_type,pkt->icmpv4_code);
	
	//IPv6
	if( UINT128__T_LO(pkt->ipv6_src) || UINT128__T_HI(pkt->ipv6_src) )
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_SRC:0x%lx:%lx, ",UINT128__T_HI(pkt->ipv6_src),UINT128__T_LO(pkt->ipv6_src));
	if( UINT128__T_LO(pkt->ipv6_dst) || UINT128__T_HI(pkt->ipv6_dst) )
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_DST:0x%lx:%lx, ",UINT128__T_HI(pkt->ipv6_dst),UINT128__T_LO(pkt->ipv6_dst));
	if(pkt->eth_type == OF12_ETH_TYPE_IPV6)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_FLABEL:0x%lu, ",pkt->ipv6_flabel);
	if(pkt->ip_proto == OF12_IP_PROTO_ICMPV6)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_ND_TARGET:0x%lx:%lx, ",UINT128__T_HI(pkt->ipv6_nd_target),UINT128__T_LO(pkt->ipv6_nd_target));
	if(pkt->ip_proto == OF12_IP_PROTO_ICMPV6) //NOTE && pkt->icmpv6_type ==?
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_ND_SLL:0x%llx, ",pkt->ipv6_nd_sll);
	if(pkt->ip_proto == OF12_IP_PROTO_ICMPV6) //NOTE && pkt->icmpv6_type ==?
		ROFL_PIPELINE_DEBUG_NO_PREFIX("IPV6_ND_TLL:0x%llx, ",pkt->ipv6_nd_tll);
	/*TODO IPV6 exthdr*/
	/*nd_target nd_sll nd_tll exthdr*/
	
	//ICMPv6
	if(pkt->ip_proto == OF12_IP_PROTO_ICMPV6)
		ROFL_PIPELINE_DEBUG_NO_PREFIX("ICMPV6_TYPE:%lu, ICMPV6_CODE:%lu, ",pkt->icmpv6_type,pkt->icmpv6_code);
	
	//MPLS	
   	if(pkt->eth_type == OF12_ETH_TYPE_MPLS_UNICAST || pkt->eth_type == OF12_ETH_TYPE_MPLS_MULTICAST )
		ROFL_PIPELINE_DEBUG_NO_PREFIX("MPLS_LABEL:0x%x, MPLS_TC:0x%x, ",pkt->mpls_label, pkt->mpls_tc);
	//PPPoE
	if(pkt->eth_type == OF12_ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION ){
		ROFL_PIPELINE_DEBUG_NO_PREFIX("PPPOE_CODE:0x%x, PPPOE_TYPE:0x%x, PPPOE_SID:0x%x, ",pkt->pppoe_code, pkt->pppoe_type,pkt->pppoe_sid);
		//PPP
		if(pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION)
			ROFL_PIPELINE_DEBUG_NO_PREFIX("PPP_PROTO:0x%x, ",pkt->ppp_proto);
				
	}
	//GTP
	if(pkt->ip_proto == OF12_IP_PROTO_UDP && pkt->udp_dst == OF12_UDP_DST_PORT_GTPU){
		ROFL_PIPELINE_DEBUG_NO_PREFIX("GTP_MSG_TYPE:%u, GTP_TEID:0x%x, ",pkt->gtp_msg_type, pkt->gtp_teid);
	}

	ROFL_PIPELINE_DEBUG_NO_PREFIX("]");	
	//Add more here...	
}

//Matches without mask (in matches that do not support)
void of12_dump_matches(of12_match_t* matches){
	of12_match_t* it;
	for(it=matches;it;it=it->next){
		switch(it->type){
			case OF12_MATCH_IN_PORT: ROFL_PIPELINE_DEBUG_NO_PREFIX("[PORT_IN:%u], ",it->value->value.u32); 
				break;
			case OF12_MATCH_IN_PHY_PORT: ROFL_PIPELINE_DEBUG_NO_PREFIX("[PHY_PORT_IN:%u], ",it->value->value.u32);
				break; 

			case OF12_MATCH_METADATA: //TODO FIXME
						break;

			case OF12_MATCH_ETH_DST: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ETH_DST:0x%llx|0x%llx],  ",(long long unsigned)it->value->value.u64,(long long unsigned)it->value->mask.u64);
				break; 
			case OF12_MATCH_ETH_SRC:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[ETH_SRC:0x%llx|0x%llx], ",(long long unsigned)it->value->value.u64,(long long unsigned)it->value->mask.u64);
				break; 
			case OF12_MATCH_ETH_TYPE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[ETH_TYPE:0x%x], ",it->value->value.u16);
				break; 

			case OF12_MATCH_VLAN_VID:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[VLAN_ID:%u|0x%x], ",it->value->value.u16,it->value->mask.u16);
				break; 
			case OF12_MATCH_VLAN_PCP:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[VLAN_PCP:%u], ",it->value->value.u8);
				break; 

			case OF12_MATCH_MPLS_LABEL:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[MPLS_LABEL:0x%x], ",it->value->value.u32);
				break; 
			case OF12_MATCH_MPLS_TC:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[MPLS_TC:0x%x], ",it->value->value.u8);
				break; 

			case OF12_MATCH_ARP_OP: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ARP_OPCODE:0x%x], ",it->value->value.u16);
				break;
			case OF12_MATCH_ARP_SHA: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ARP_SHA:0x%llx|0x%llx],  ",(long long unsigned)it->value->value.u64,(long long unsigned)it->value->mask.u64);
				break;
			case OF12_MATCH_ARP_SPA: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ARP_SPA:0x%x|0x%x], ",it->value->value.u32,it->value->mask.u32);
				break;
			case OF12_MATCH_ARP_THA: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ARP_THA:0x%llx|0x%llx],  ",(long long unsigned)it->value->value.u64,(long long unsigned)it->value->mask.u64);
				break;
			case OF12_MATCH_ARP_TPA: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ARP_TPA:0x%x|0x%x], ",it->value->value.u32,it->value->mask.u32);
				break;

			case OF12_MATCH_IP_PROTO:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IP_PROTO:%u], ",it->value->value.u8);
				break; 
			case OF12_MATCH_IP_ECN:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IP_ECN:0x%x], ",it->value->value.u8);
				break; 
			case OF12_MATCH_IP_DSCP:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IP_DSCP:0x%x], ",it->value->value.u8);
				break; 

			case OF12_MATCH_IPV4_SRC:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IP4_SRC:0x%x|0x%x], ",it->value->value.u32,it->value->mask.u32);
				break; 
			case OF12_MATCH_IPV4_DST:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IP4_DST:0x%x|0x%x], ",it->value->value.u32,it->value->mask.u32);
				break; 

			case OF12_MATCH_TCP_SRC:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[TCP_SRC:%u], ",it->value->value.u16);
				break; 
			case OF12_MATCH_TCP_DST:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[TCP_DST:%u], ",it->value->value.u16);
				break; 

			case OF12_MATCH_UDP_SRC:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[UDP_SRC:%u], ",it->value->value.u16);
				break; 
			case OF12_MATCH_UDP_DST:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[UDP_DST:%u], ",it->value->value.u16);
				break; 

			case OF12_MATCH_ICMPV4_TYPE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[ICMPV4_TYPE:%u], ",it->value->value.u8);
				break; 
			case OF12_MATCH_ICMPV4_CODE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[ICMPV4_CODE:%u], ",it->value->value.u8);
				break; 
			//IPv6
			case OF12_MATCH_IPV6_SRC: ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_SRC:%lu %lu], ",UINT128__T_HI(it->value->value.u128),UINT128__T_LO(it->value->value.u128));
				break;
			case OF12_MATCH_IPV6_DST: ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_DST:%u], ",UINT128__T_HI(it->value->value.u128),UINT128__T_LO(it->value->value.u128));
				break;
			case OF12_MATCH_IPV6_FLABEL: ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_FLABEL:%u], ",it->value->value.u64);
				break;
			case OF12_MATCH_IPV6_ND_TARGET: ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_ND_TARGET:%lu %lu], ",UINT128__T_HI(it->value->value.u128),UINT128__T_LO(it->value->value.u128));
				break;
			case OF12_MATCH_IPV6_ND_SLL: ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_ND_SLL:%u], ",it->value->value.u64);
				break;
			case OF12_MATCH_IPV6_ND_TLL: ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_ND_TLL:%u], ",it->value->value.u64);
				break;
			case OF12_MATCH_IPV6_EXTHDR: ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_EXTHDR:%u], ",it->value->value.u64);
				break;
			//ICMPv6
			case OF12_MATCH_ICMPV6_TYPE: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ICMPV6_TYPE:%u], ",it->value->value.u64);
				break;
			case OF12_MATCH_ICMPV6_CODE: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ICMPV6_CODE:%u], ",it->value->value.u64);
				break;
			
			/* PPP/PPPoE related extensions */
			case OF12_MATCH_PPPOE_CODE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[PPPOE_CODE:%u], ",it->value->value.u8);
				break; 
			case OF12_MATCH_PPPOE_TYPE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[PPPOE_TYPE:%u], ",it->value->value.u8);
				break; 
			case OF12_MATCH_PPPOE_SID:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[PPPOE_SID:%u], ",it->value->value.u16);
				break; 
			case OF12_MATCH_PPP_PROT:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[PPP_PROT:%u] ",it->value->value.u16);
				break; 

			/* GTP related extensions */
			case OF12_MATCH_GTP_MSG_TYPE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[GTP_MSG_TYPE:%u], ",it->value->value);
				break;
			case OF12_MATCH_GTP_TEID: ROFL_PIPELINE_DEBUG_NO_PREFIX("[GTP_TEID:0x%x], ",it->value->value);
				break;

			/* Add more here ...*/
			default:
				ROFL_PIPELINE_DEBUG_NO_PREFIX("[UNKOWN!],");
				//Should NEVER reach this point
				
		}
	}	
}

//Matches with mask (including matches that do not support)
void of12_full_dump_matches(of12_match_t* matches){
	of12_match_t* it;
	for(it=matches;it;it=it->next){
		switch(it->type){
			case OF12_MATCH_IN_PORT: ROFL_PIPELINE_DEBUG_NO_PREFIX("[PORT_IN:%u|0x%x], ",it->value->value.u32,it->value->mask.u32); 
				break;
			case OF12_MATCH_IN_PHY_PORT: ROFL_PIPELINE_DEBUG_NO_PREFIX("[PHY_PORT_IN:%u|0x%x], ",it->value->value.u32,it->value->mask.u32);
				break; 

			case OF12_MATCH_METADATA: //TODO FIXME
						break;

			case OF12_MATCH_ETH_DST: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ETH_DST:0x%llx|0x%llx],  ",(long long unsigned)it->value->value.u64,(long long unsigned)it->value->mask.u64);
				break; 
			case OF12_MATCH_ETH_SRC:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[ETH_SRC:0x%llx|0x%llx], ",(long long unsigned)it->value->value.u64,(long long unsigned)it->value->mask.u64);
				break; 
			case OF12_MATCH_ETH_TYPE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[ETH_TYPE:0x%x|0x%x], ",it->value->value.u16,it->value->mask.u16);
				break; 

			case OF12_MATCH_VLAN_VID:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[VLAN_ID:%u|0x%x], ",it->value->value.u16,it->value->mask.u16);
				break; 
			case OF12_MATCH_VLAN_PCP:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[VLAN_PCP:%u|0x%x], ",it->value->mask.u8,it->value->mask.u8);
				break; 

			case OF12_MATCH_MPLS_LABEL:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[MPLS_LABEL:%u|0x%x], ",it->value->value.u32,it->value->mask.u32);
				break; 
			case OF12_MATCH_MPLS_TC:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[MPLS_TC:%u|0x%x], ",it->value->value.u8,it->value->mask.u8);
				break; 

			case OF12_MATCH_ARP_OP: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ARP_OPCODE:0x%x|0x%x], ",it->value->value.u16,it->value->mask.u16);
				break;
			case OF12_MATCH_ARP_SHA: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ARP_SHA:0x%llx|0x%llx], ",(long long unsigned)it->value->value.u64,(long long unsigned)it->value->mask.u64);
				break;
			case OF12_MATCH_ARP_SPA: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ARP_SPA:0x%x|0x%x], ",it->value->value.u32,it->value->mask.u32);
				break;
			case OF12_MATCH_ARP_THA: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ARP_THA:0x%llx|0x%llx], ",(long long unsigned)it->value->value.u64,(long long unsigned)it->value->mask.u64);
				break;
			case OF12_MATCH_ARP_TPA: ROFL_PIPELINE_DEBUG_NO_PREFIX("[ARP_TPA:0x%x|0x%x], ",it->value->value.u32,it->value->mask.u32);
				break;

			case OF12_MATCH_IP_ECN:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IP_ECN:0x%x|0x%x], ",it->value->value.u8,it->value->mask.u8);
				break; 
			case OF12_MATCH_IP_DSCP:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IP_DSCP:0x%x|0x%x], ",it->value->value.u8,it->value->mask.u8);
				break; 
			case OF12_MATCH_IP_PROTO:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IP_PROTO:%u|0x%x], ",it->value->value.u8,it->value->mask.u8);
				break; 

			case OF12_MATCH_IPV4_SRC:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IP4_SRC:0x%x|0x%x], ",it->value->value.u32,it->value->mask.u32);
				break; 
			case OF12_MATCH_IPV4_DST:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IP4_DST:0x%x|0x%x], ",it->value->value.u32,it->value->mask.u32);
				break; 

			case OF12_MATCH_TCP_SRC:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[TCP_SRC:%u|0x%x], ",it->value->value.u16,it->value->mask.u16);
				break; 
			case OF12_MATCH_TCP_DST:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[TCP_DST:%u|0x%x], ",it->value->value.u16,it->value->mask.u16);
				break; 

			case OF12_MATCH_UDP_SRC:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[UDP_SRC:%u|0x%x], ",it->value->value.u16,it->value->mask.u16);
				break; 
			case OF12_MATCH_UDP_DST:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[UDP_DST:%u|0x%x], ",it->value->value.u16,it->value->mask.u16);
				break; 

			case OF12_MATCH_ICMPV4_TYPE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[ICMPV4_TYPE:%u|0x%x], ",it->value->value.u8,it->value->mask.u8);
				break; 
			case OF12_MATCH_ICMPV4_CODE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[ICMPV4_CODE:%u|0x%x], ",it->value->value.u8,it->value->mask.u8);
				break; 
			
			//IPv6
			case OF12_MATCH_IPV6_SRC: ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_SRC:0x%lx:%lx|0x%lx:%lx], ",UINT128__T_HI(it->value->value.u128),UINT128__T_LO(it->value->value.u128),UINT128__T_HI(it->value->mask.u128),UINT128__T_LO(it->value->mask.u128));
				break;
			case OF12_MATCH_IPV6_DST: ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_DST:0x%lx:%lx|0x%lx:%lx], ",UINT128__T_HI(it->value->value.u128),UINT128__T_LO(it->value->value.u128),UINT128__T_HI(it->value->mask.u128),UINT128__T_LO(it->value->mask.u128));
				break;
			case OF12_MATCH_IPV6_FLABEL:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_FLABEL:%lu|0x%lx], ",it->value->value.u64,it->value->mask.u64);
				break; 
			case OF12_MATCH_IPV6_ND_TARGET: ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_ND_TARGET:0x%lx:%lx|0x%lx:%lx], ",UINT128__T_HI(it->value->value.u128),UINT128__T_LO(it->value->value.u128),UINT128__T_HI(it->value->mask.u128),UINT128__T_LO(it->value->mask.u128));
				break;
			case OF12_MATCH_IPV6_ND_SLL:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_ND_SLL:%lu|0x%lx], ",it->value->value.u64,it->value->mask.u64);
				break; 
			case OF12_MATCH_IPV6_ND_TLL:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_ND_TLL:%lu|0x%lx], ",it->value->value.u64,it->value->mask.u64);
				break; 
			case OF12_MATCH_IPV6_EXTHDR:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[IPV6_EXTHDR:%lu|0x%lx], ",it->value->value.u64,it->value->mask.u64);
				break; 
			//ICMPv6
			case OF12_MATCH_ICMPV6_TYPE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[ICMPV6_TYPE:%lu|0x%lx], ",it->value->value.u64,it->value->mask.u64);
				break; 
			case OF12_MATCH_ICMPV6_CODE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[ICMPV6_CODE:%lu|0x%lx], ",it->value->value.u64,it->value->mask.u64);
				break; 
				
			/* PPP/PPPoE related extensions */
			case OF12_MATCH_PPPOE_CODE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[PPPOE_CODE:%u|0x%x], ",it->value->value.u8,it->value->mask.u8);
				break; 
			case OF12_MATCH_PPPOE_TYPE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[PPPOE_TYPE:%u|0x%x], ",it->value->value.u8,it->value->mask.u8);
				break; 
			case OF12_MATCH_PPPOE_SID:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[PPPOE_SID:%u|0x%x], ",it->value->value.u16,it->value->mask.u16);
				break; 

			case OF12_MATCH_PPP_PROT:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[PPP_PROT:%u|0x%x] ",it->value->value.u16,it->value->mask.u16);
				break; 

			/* GTP related extensions */
			case OF12_MATCH_GTP_MSG_TYPE:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[GTP_MSG_TYPE:%u|0x%x], ",it->value->value.u8,it->value->mask.u8);
				break;
			case OF12_MATCH_GTP_TEID:  ROFL_PIPELINE_DEBUG_NO_PREFIX("[GTP_TEID:0x%x|0x%x], ",it->value->value.u32,it->value->mask.u32);
				break;

			/* Add more here ...*/
			default:
				ROFL_PIPELINE_DEBUG_NO_PREFIX("[UNKOWN!],");
				//Should NEVER reach this point
				
		}
	}	
}

