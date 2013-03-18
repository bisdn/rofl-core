#include "of12_match.h"

#include "../../../platform/memory.h"

#include <stdio.h>

/*
* Initializers 
*/

//Phy
inline of12_match_t* of12_init_port_in_match(of12_match_t* prev, of12_match_t* next, uint32_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IN_PORT; 
	match->value = init_utern32(value,0xFFFFFFFF); //No wildcard
	match->prev = prev;
	match->next = next;
	return match;
}

inline of12_match_t* of12_init_port_in_phy_match(of12_match_t* prev, of12_match_t* next, uint32_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IN_PHY_PORT; 
	match->value = init_utern32(value,0xFFFFFFFF); //No wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

/* TODO: add metadata */

//ETHERNET
inline of12_match_t* of12_init_eth_dst_match(of12_match_t* prev, of12_match_t* next, uint64_t value, uint64_t mask){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ETH_DST; 
	match->value = init_utern64(value&0x0000FFFFFFFFFFFF,mask&0x0000FFFFFFFFFFFF); //Enforce mask bits are always 00 for the first bits 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_eth_src_match(of12_match_t* prev, of12_match_t* next, uint64_t value, uint64_t mask){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ETH_SRC; 
	match->value = init_utern64(value&0x0000FFFFFFFFFFFF,mask&0x0000FFFFFFFFFFFF); //Enforce mask bits are always 00 for the first bits 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_eth_type_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ETH_TYPE; 
	match->value = init_utern16(value,0xFFFF); //No wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

//8021.q
inline of12_match_t* of12_init_vlan_vid_match(of12_match_t* prev, of12_match_t* next, uint16_t value, uint16_t mask){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_VLAN_VID; 
	match->value = init_utern16(value&0x1FFF,mask&0x1FFF); //Ensure only 13 bit value
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_vlan_pcp_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_VLAN_PCP; 
	match->value = init_utern8(value&0x07,0x07); //Ensure only 3 bit value, no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

//MPLS
inline of12_match_t* of12_init_mpls_label_match(of12_match_t* prev, of12_match_t* next, uint32_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_MPLS_LABEL; 
	match->value = init_utern32(value&0x000FFFFF,0x000FFFFF); //no wildcard?? wtf! 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_mpls_tc_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_MPLS_TC; 
	match->value = init_utern8(value&0x07,0x07); //Ensure only 3 bit value, no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
//PPPoE
inline of12_match_t* of12_init_pppoe_code_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_PPPOE_CODE; 
	match->value = init_utern8(value,0xFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_pppoe_type_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_PPPOE_TYPE; 
	match->value = init_utern8(value&0x0F,0x0F); //Ensure only 4 bit value, no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_pppoe_session_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_PPPOE_SID; 
	match->value = init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
//PPP
inline of12_match_t* of12_init_ppp_prot_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_PPP_PROT; 
	match->value = init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
//IPv4
inline of12_match_t* of12_init_ip4_src_match(of12_match_t* prev, of12_match_t* next, uint32_t value, uint32_t mask){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IPV4_SRC;
	match->value = init_utern32(value,mask); 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip4_dst_match(of12_match_t* prev, of12_match_t* next, uint32_t value, uint32_t mask){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IPV4_DST;
	match->value = init_utern32(value,mask); 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip_proto_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IP_PROTO; 
	match->value = init_utern8(value,0xFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_ip_dscp_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IP_DSCP; 
	match->value = init_utern8(value,0x3F); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

inline of12_match_t* of12_init_ip_ecn_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_IP_ECN; 
	match->value = init_utern8(value,0x03); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}


//TCP
inline of12_match_t* of12_init_tcp_src_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_TCP_SRC;
	match->value = init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_tcp_dst_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_TCP_DST;
	match->value = init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
//UDP
inline of12_match_t* of12_init_udp_src_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_UDP_SRC;
	match->value = init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_udp_dst_match(of12_match_t* prev, of12_match_t* next, uint16_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_UDP_DST;
	match->value = init_utern16(value,0xFFFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
//ICMPv4
inline of12_match_t* of12_init_icmpv4_type_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ICMPV4_TYPE; 
	match->value = init_utern8(value,0xFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}
inline of12_match_t* of12_init_icmpv4_code_match(of12_match_t* prev, of12_match_t* next, uint8_t value){
	of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
	match->type = OF12_MATCH_ICMPV4_CODE; 
	match->value = init_utern8(value,0xFF); //no wildcard 
	match->prev = prev;
	match->next = next;
	return match;
}

//IPv6 && ICMv6
//TODO

//Add more here...



/* Instruction groups init and destroy */
of12_match_group_t* of12_new_match_group(){

	of12_match_group_t *group = (of12_match_group_t*)cutil_malloc_shared(sizeof(of12_match_group_t));

	if(!group)
		return NULL; 

	of12_init_match_group(group);

	return group;
}




/* Instruction groups init and destroy */
void of12_init_match_group(of12_match_group_t* group){

	memset(group,0,sizeof(of12_match_group_t));
}


void of12_destroy_match_group(of12_match_group_t* group){
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



void of12_match_group_push_back(of12_match_group_t* group, of12_match_t* match){

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
rofl_result_t of12_add_match(of12_match_t* root_match, of12_match_t* add_match){
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
inline of12_match_t* of12_copy_match(of12_match_t* match){
	switch(match->type){

		case OF12_MATCH_IN_PORT: return of12_init_port_in_match(NULL, NULL, ((utern32_t*)match->value)->value);
		case OF12_MATCH_IN_PHY_PORT: return of12_init_port_in_phy_match(NULL, NULL, ((utern32_t*)match->value)->value);

	  	case OF12_MATCH_METADATA: //TODO FIXME
					return NULL; 
   
		case OF12_MATCH_ETH_DST:  return of12_init_eth_dst_match(NULL,NULL,((utern64_t*)match->value)->value,((utern64_t*)match->value)->mask); 
   		case OF12_MATCH_ETH_SRC:  return  of12_init_eth_src_match(NULL,NULL,((utern64_t*)match->value)->value,((utern64_t*)match->value)->mask);
   		case OF12_MATCH_ETH_TYPE: return of12_init_eth_type_match(NULL,NULL,((utern16_t*)match->value)->value);

   		case OF12_MATCH_VLAN_VID: return of12_init_vlan_vid_match(NULL,NULL,((utern16_t*)match->value)->value,((utern16_t*)match->value)->mask); 
   		case OF12_MATCH_VLAN_PCP: return of12_init_vlan_pcp_match(NULL,NULL,((utern8_t*)match->value)->value); 

   		case OF12_MATCH_MPLS_LABEL: return of12_init_mpls_label_match(NULL,NULL,((utern32_t*)match->value)->value); 
   		case OF12_MATCH_MPLS_TC: return of12_init_mpls_tc_match(NULL,NULL,((utern8_t*)match->value)->value); 

   		case OF12_MATCH_IP_PROTO: return of12_init_ip_proto_match(NULL,NULL,((utern8_t*)match->value)->value); 
   		case OF12_MATCH_IP_ECN: return of12_init_ip_ecn_match(NULL,NULL,((utern8_t*)match->value)->value); 
   		case OF12_MATCH_IP_DSCP: return of12_init_ip_dscp_match(NULL,NULL,((utern8_t*)match->value)->value);

   		case OF12_MATCH_IPV4_SRC: return of12_init_ip4_src_match(NULL,NULL,((utern32_t*)match->value)->value,((utern32_t*)match->value)->mask); 
   		case OF12_MATCH_IPV4_DST: return of12_init_ip4_dst_match(NULL,NULL,((utern32_t*)match->value)->value,((utern32_t*)match->value)->mask); 

   		case OF12_MATCH_TCP_SRC: return of12_init_tcp_src_match(NULL,NULL,((utern16_t*)match->value)->value); 
   		case OF12_MATCH_TCP_DST: return of12_init_tcp_dst_match(NULL,NULL,((utern16_t*)match->value)->value); 

   		case OF12_MATCH_UDP_SRC: return of12_init_udp_src_match(NULL,NULL,((utern16_t*)match->value)->value); 
   		case OF12_MATCH_UDP_DST: return of12_init_udp_dst_match(NULL,NULL,((utern16_t*)match->value)->value); 

    		case OF12_MATCH_ICMPV4_TYPE: return of12_init_icmpv4_type_match(NULL,NULL,((utern8_t*)match->value)->value); 
   		case OF12_MATCH_ICMPV4_CODE: return of12_init_icmpv4_code_match(NULL,NULL,((utern8_t*)match->value)->value); 
  		
		/* PPP/PPPoE related extensions */
   		case OF12_MATCH_PPPOE_CODE: return of12_init_pppoe_code_match(NULL,NULL,((utern8_t*)match->value)->value); 
   		case OF12_MATCH_PPPOE_TYPE: return of12_init_pppoe_type_match(NULL,NULL,((utern8_t*)match->value)->value); 
   		case OF12_MATCH_PPPOE_SID: return of12_init_pppoe_session_match(NULL,NULL,((utern16_t*)match->value)->value); 
   		case OF12_MATCH_PPP_PROT: return of12_init_ppp_prot_match(NULL,NULL,((utern16_t*)match->value)->value); 
		/* Add more here ...*/
		default:
			//Should NEVER reach this point
			return NULL; 
	}	
	
}

/* 
* Whole (linked list) Match copy -> this should be deprecated in favour of the match group
*/
of12_match_t* of12_copy_matches(of12_match_t* matches){

	of12_match_t* prev, *curr, *it, *copy;
	
	if(!matches)
		return NULL;
	
	for(prev=NULL,copy=NULL, it=matches; it; it = it->next){

		curr = of12_copy_match(it);

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
inline of12_match_t* of12_get_alike_match(of12_match_t* match1, of12_match_t* match2){
	utern_t* common_tern = NULL;	

	if( match1->type != match2->type )
		return NULL;	


	switch(match1->type){
		case OF12_MATCH_IN_PORT: common_tern = utern32_get_alike(*(utern32_t*)match1->value,*(utern32_t*)match2->value);
					break;
		case OF12_MATCH_IN_PHY_PORT: common_tern = utern32_get_alike(*(utern32_t*)match1->value,*(utern32_t*)match2->value);
					break;
	
	  	case OF12_MATCH_METADATA: //TODO FIXME
					return NULL; 

   		case OF12_MATCH_ETH_DST:  common_tern = utern64_get_alike(*(utern64_t*)match1->value,*(utern64_t*)match2->value);
					break;
   		case OF12_MATCH_ETH_SRC:  common_tern = utern64_get_alike(*(utern64_t*)match1->value,*(utern64_t*)match2->value);
					break;
   		case OF12_MATCH_ETH_TYPE: common_tern = utern16_get_alike(*(utern16_t*)match1->value,*(utern16_t*)match2->value);
					break;

   		case OF12_MATCH_VLAN_VID: common_tern = utern16_get_alike(*(utern16_t*)match1->value,*(utern16_t*)match2->value);
					break;
   		case OF12_MATCH_VLAN_PCP: common_tern = utern8_get_alike(*(utern8_t*)match1->value,*(utern8_t*)match2->value);
					break;

   		case OF12_MATCH_MPLS_LABEL: common_tern = utern32_get_alike(*(utern32_t*)match1->value,*(utern32_t*)match2->value);
					break;
   		case OF12_MATCH_MPLS_TC: common_tern = utern8_get_alike(*(utern8_t*)match1->value,*(utern8_t*)match2->value);
					break;

   		case OF12_MATCH_IP_PROTO: common_tern = utern8_get_alike(*(utern8_t*)match1->value,*(utern8_t*)match2->value);
					break;
		case OF12_MATCH_IP_ECN: common_tern = utern8_get_alike(*(utern8_t*)match1->value,*(utern8_t*)match2->value);
					break;
		case OF12_MATCH_IP_DSCP: common_tern = utern8_get_alike(*(utern8_t*)match1->value,*(utern8_t*)match2->value);
					break;

   		case OF12_MATCH_IPV4_SRC: common_tern = utern32_get_alike(*(utern32_t*)match1->value,*(utern32_t*)match2->value);
					break;
   		case OF12_MATCH_IPV4_DST: common_tern = utern32_get_alike(*(utern32_t*)match1->value,*(utern32_t*)match2->value);
					break;

   		case OF12_MATCH_TCP_SRC: common_tern = utern16_get_alike(*(utern16_t*)match1->value,*(utern16_t*)match2->value);
					break;
   		case OF12_MATCH_TCP_DST: common_tern = utern16_get_alike(*(utern16_t*)match1->value,*(utern16_t*)match2->value);
					break;

   		case OF12_MATCH_UDP_SRC: common_tern = utern16_get_alike(*(utern16_t*)match1->value,*(utern16_t*)match2->value);
					break;
   		case OF12_MATCH_UDP_DST: common_tern = utern16_get_alike(*(utern16_t*)match1->value,*(utern16_t*)match2->value);
					break;

    		case OF12_MATCH_ICMPV4_TYPE: common_tern = utern8_get_alike(*(utern8_t*)match1->value,*(utern8_t*)match2->value);
					break;
   		case OF12_MATCH_ICMPV4_CODE: common_tern = utern8_get_alike(*(utern8_t*)match1->value,*(utern8_t*)match2->value);
					break;
  		
		/* PPP/PPPoE related extensions */
   		case OF12_MATCH_PPPOE_CODE: common_tern = utern8_get_alike(*(utern8_t*)match1->value,*(utern8_t*)match2->value);
					break;
   		case OF12_MATCH_PPPOE_TYPE: common_tern = utern8_get_alike(*(utern8_t*)match1->value,*(utern8_t*)match2->value);
					break;
   		case OF12_MATCH_PPPOE_SID: common_tern = utern16_get_alike(*(utern16_t*)match1->value,*(utern16_t*)match2->value);
					break;

   		case OF12_MATCH_PPP_PROT: common_tern = utern16_get_alike(*(utern16_t*)match1->value,*(utern16_t*)match2->value);
					break;
		/* Add more here ...*/
		default:
			//Should NEVER reach this point
			return NULL; 
	}	
	if(common_tern){
		of12_match_t* match = (of12_match_t*)cutil_malloc_shared(sizeof(of12_match_t));
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
	destroy_utern(match->value);
	cutil_free_shared(match);
}

/*
*
* Matching routines...
* 
*/

//Compare matches
inline bool of12_equal_matches(of12_match_t* match1, of12_match_t* match2){

	if( match1->type != match2->type )
		return false; 

	switch(match1->type){
		case OF12_MATCH_IN_PORT: return utern_equals32((utern32_t*)match1->value,(utern32_t*)match2->value);
		case OF12_MATCH_IN_PHY_PORT: return utern_equals32((utern32_t*)match1->value,(utern32_t*)match2->value);

	  	case OF12_MATCH_METADATA: //TODO FIXME
					return false;

   		case OF12_MATCH_ETH_DST: return utern_equals64((utern64_t*)match1->value,(utern64_t*)match2->value);
   		case OF12_MATCH_ETH_SRC: return utern_equals64((utern64_t*)match1->value,(utern64_t*)match2->value);
   		case OF12_MATCH_ETH_TYPE: return utern_equals16((utern16_t*)match1->value,(utern16_t*)match2->value);

   		case OF12_MATCH_VLAN_VID: return utern_equals16((utern16_t*)match1->value,(utern16_t*)match2->value);
   		case OF12_MATCH_VLAN_PCP: return utern_equals8((utern8_t*)match1->value,(utern8_t*)match2->value);

   		case OF12_MATCH_MPLS_LABEL: return utern_equals32((utern32_t*)match1->value,(utern32_t*)match2->value);
   		case OF12_MATCH_MPLS_TC: return utern_equals8((utern8_t*)match1->value,(utern8_t*)match2->value);

   		case OF12_MATCH_IP_PROTO: return utern_equals8((utern8_t*)match1->value,(utern8_t*)match2->value);
   		case OF12_MATCH_IP_ECN: return utern_equals8((utern8_t*)match1->value,(utern8_t*)match2->value);
   		case OF12_MATCH_IP_DSCP: return utern_equals8((utern8_t*)match1->value,(utern8_t*)match2->value);

   		case OF12_MATCH_IPV4_SRC: return utern_equals32((utern32_t*)match1->value,(utern32_t*)match2->value);
   		case OF12_MATCH_IPV4_DST: return utern_equals32((utern32_t*)match1->value,(utern32_t*)match2->value);

   		case OF12_MATCH_TCP_SRC: return utern_equals16((utern16_t*)match1->value,(utern16_t*)match2->value);
   		case OF12_MATCH_TCP_DST: return utern_equals16((utern16_t*)match1->value,(utern16_t*)match2->value);

   		case OF12_MATCH_UDP_SRC: return utern_equals16((utern16_t*)match1->value,(utern16_t*)match2->value);
   		case OF12_MATCH_UDP_DST: return utern_equals16((utern16_t*)match1->value,(utern16_t*)match2->value);

    		case OF12_MATCH_ICMPV4_TYPE: return utern_equals8((utern8_t*)match1->value,(utern8_t*)match2->value);
   		case OF12_MATCH_ICMPV4_CODE: return utern_equals8((utern8_t*)match1->value,(utern8_t*)match2->value);
  		
		/* PPP/PPPoE related extensions */
   		case OF12_MATCH_PPPOE_CODE: return utern_equals8((utern8_t*)match1->value,(utern8_t*)match2->value);
   		case OF12_MATCH_PPPOE_TYPE: return utern_equals8((utern8_t*)match1->value,(utern8_t*)match2->value);
   		case OF12_MATCH_PPPOE_SID: return utern_equals16((utern16_t*)match1->value,(utern16_t*)match2->value);

   		case OF12_MATCH_PPP_PROT:  return utern_equals16((utern16_t*)match1->value,(utern16_t*)match2->value);
		/* Add more here ...*/
		default:
			//Should NEVER reach this point
			return false; 
	}	
}

//Finds out if sub_match is a submatch of match
inline bool of12_is_submatch(of12_match_t* sub_match, of12_match_t* match){

	if( match->type != sub_match->type )
		return false; 

	switch(match->type){
		case OF12_MATCH_IN_PORT: return utern_is_contained32((utern32_t*)sub_match->value,(utern32_t*)match->value);
		case OF12_MATCH_IN_PHY_PORT: return utern_is_contained32((utern32_t*)sub_match->value,(utern32_t*)match->value);

	  	case OF12_MATCH_METADATA: //TODO FIXME
					return false;
   		case OF12_MATCH_ETH_DST: return utern_is_contained64((utern64_t*)sub_match->value,(utern64_t*)match->value);
   		case OF12_MATCH_ETH_SRC: return utern_is_contained64((utern64_t*)sub_match->value,(utern64_t*)match->value);
   		case OF12_MATCH_ETH_TYPE: return utern_is_contained16((utern16_t*)sub_match->value,(utern16_t*)match->value);

   		case OF12_MATCH_VLAN_VID: return utern_is_contained16((utern16_t*)sub_match->value,(utern16_t*)match->value);
   		case OF12_MATCH_VLAN_PCP: return utern_is_contained8((utern8_t*)sub_match->value,(utern8_t*)match->value);

   		case OF12_MATCH_MPLS_LABEL: return utern_is_contained32((utern32_t*)sub_match->value,(utern32_t*)match->value);
   		case OF12_MATCH_MPLS_TC: return utern_is_contained8((utern8_t*)sub_match->value,(utern8_t*)match->value);

   		case OF12_MATCH_IP_PROTO: return utern_is_contained8((utern8_t*)sub_match->value,(utern8_t*)match->value);
   		case OF12_MATCH_IP_ECN: return utern_is_contained8((utern8_t*)sub_match->value,(utern8_t*)match->value);
   		case OF12_MATCH_IP_DSCP: return utern_is_contained8((utern8_t*)sub_match->value,(utern8_t*)match->value);

   		case OF12_MATCH_IPV4_SRC: return utern_is_contained32((utern32_t*)sub_match->value,(utern32_t*)match->value);
   		case OF12_MATCH_IPV4_DST: return utern_is_contained32((utern32_t*)sub_match->value,(utern32_t*)match->value);

   		case OF12_MATCH_TCP_SRC: return utern_is_contained16((utern16_t*)sub_match->value,(utern16_t*)match->value);
   		case OF12_MATCH_TCP_DST: return utern_is_contained16((utern16_t*)sub_match->value,(utern16_t*)match->value);

   		case OF12_MATCH_UDP_SRC: return utern_is_contained16((utern16_t*)sub_match->value,(utern16_t*)match->value);
   		case OF12_MATCH_UDP_DST: return utern_is_contained16((utern16_t*)sub_match->value,(utern16_t*)match->value);

    		case OF12_MATCH_ICMPV4_TYPE: return utern_is_contained8((utern8_t*)sub_match->value,(utern8_t*)match->value);
   		case OF12_MATCH_ICMPV4_CODE: return utern_is_contained8((utern8_t*)sub_match->value,(utern8_t*)match->value);
  		
		/* PPP/PPPoE related extensions */
   		case OF12_MATCH_PPPOE_CODE: return utern_is_contained8((utern8_t*)sub_match->value,(utern8_t*)match->value);
   		case OF12_MATCH_PPPOE_TYPE: return utern_is_contained8((utern8_t*)sub_match->value,(utern8_t*)match->value);
   		case OF12_MATCH_PPPOE_SID: return utern_is_contained16((utern16_t*)sub_match->value,(utern16_t*)match->value);

   		case OF12_MATCH_PPP_PROT:  return utern_is_contained16((utern16_t*)sub_match->value,(utern16_t*)match->value);
		/* Add more here ...*/
		default:
			//Should NEVER reach this point
			return false;
	}	
}
/*
*
* CHECK fields against packet
*
*/
inline bool of12_check_match(const of12_packet_matches_t* pkt, of12_match_t* it){
	if(!it)
		return false;
	
	switch(it->type){
		//Phy
		case OF12_MATCH_IN_PORT: return utern_compare32((utern32_t*)it->value,pkt->port_in);
		case OF12_MATCH_IN_PHY_PORT: if(!pkt->port_in) return false; //According to spec
					return utern_compare32((utern32_t*)it->value,pkt->phy_port_in);
		//Metadata
	  	case OF12_MATCH_METADATA: //TODO FIXME
					return false;
		//802
   		case OF12_MATCH_ETH_DST:  return utern_compare64((utern64_t*)it->value,pkt->eth_dst);
   		case OF12_MATCH_ETH_SRC:  return utern_compare64((utern64_t*)it->value,pkt->eth_src);
   		case OF12_MATCH_ETH_TYPE: return utern_compare16((utern16_t*)it->value,pkt->eth_type);
		
		//802.1q
   		case OF12_MATCH_VLAN_VID: return utern_compare16((utern16_t*)it->value,pkt->vlan_vid);
   		case OF12_MATCH_VLAN_PCP: if(!pkt->vlan_vid) return false;
					return utern_compare8((utern8_t*)it->value,pkt->vlan_pcp);
		//MPLS
   		case OF12_MATCH_MPLS_LABEL: if(!(pkt->eth_type == OF12_ETH_TYPE_MPLS_UNICAST || pkt->eth_type == OF12_ETH_TYPE_MPLS_MULTICAST )) return false;
					return utern_compare32((utern32_t*)it->value,pkt->mpls_label);
   		case OF12_MATCH_MPLS_TC: if(!(pkt->eth_type == OF12_ETH_TYPE_MPLS_UNICAST || pkt->eth_type == OF12_ETH_TYPE_MPLS_MULTICAST )) return false; 
					return utern_compare8((utern8_t*)it->value,pkt->mpls_tc);
		//IP
   		case OF12_MATCH_IP_PROTO: if(!(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 || (pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP4 ))) return false; 
					return utern_compare8((utern8_t*)it->value,pkt->ip_proto);
		case OF12_MATCH_IP_ECN: if(!(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 || (pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP4 ))) return false; 
					return utern_compare8((utern8_t*)it->value,pkt->ip_ecn);
	
		case OF12_MATCH_IP_DSCP: if(!(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 || (pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP4 ))) return false; 
					return utern_compare8((utern8_t*)it->value,pkt->ip_dscp);
		
		//IPv4
   		case OF12_MATCH_IPV4_SRC: if(!(pkt->eth_type == OF12_ETH_TYPE_IPV4 || (pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP4 ))) return false; 
					return utern_compare32((utern32_t*)it->value, pkt->ipv4_src);
   		case OF12_MATCH_IPV4_DST:if(!(pkt->eth_type == OF12_ETH_TYPE_IPV4 ||(pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION && pkt->ppp_proto == OF12_PPP_PROTO_IP4 ))) return false;  
					return utern_compare32((utern32_t*)it->value, pkt->ipv4_dst);
	
		//TCP
   		case OF12_MATCH_TCP_SRC: if(!(pkt->ip_proto == OF12_IP_PROTO_TCP)) return false; 
					return utern_compare16((utern16_t*)it->value,pkt->tcp_src);
   		case OF12_MATCH_TCP_DST: if(!(pkt->ip_proto == OF12_IP_PROTO_TCP)) return false; 
					return utern_compare16((utern16_t*)it->value,pkt->tcp_dst);
	
		//UDP
   		case OF12_MATCH_UDP_SRC: if(!(pkt->ip_proto == OF12_IP_PROTO_UDP)) return false; 	
					return utern_compare16((utern16_t*)it->value,pkt->udp_src);
   		case OF12_MATCH_UDP_DST: if(!(pkt->ip_proto == OF12_IP_PROTO_UDP)) return false; 
					return utern_compare16((utern16_t*)it->value,pkt->udp_dst);
		
		//ICMPv4
    		case OF12_MATCH_ICMPV4_TYPE: if(!(pkt->ip_proto == OF12_IP_PROTO_ICMPV4)) return false; 
					return utern_compare8((utern8_t*)it->value,pkt->icmpv4_type);
   		case OF12_MATCH_ICMPV4_CODE: if(!(pkt->ip_proto == OF12_IP_PROTO_ICMPV4)) return false; 
					return utern_compare8((utern8_t*)it->value,pkt->icmpv4_code);
  		
		//PPPoE related extensions
   		case OF12_MATCH_PPPOE_CODE: if(!(pkt->eth_type == OF12_ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION )) return false;  
						return utern_compare8((utern8_t*)it->value,pkt->pppoe_code);
   		case OF12_MATCH_PPPOE_TYPE: if(!(pkt->eth_type == OF12_ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION )) return false; 
						return utern_compare8((utern8_t*)it->value,pkt->pppoe_type);
   		case OF12_MATCH_PPPOE_SID: if(!(pkt->eth_type == OF12_ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION )) return false; 
						return utern_compare16((utern16_t*)it->value,pkt->pppoe_sid);

		//PPP 
   		case OF12_MATCH_PPP_PROT: if(!(pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION )) return false; 
						return utern_compare16((utern16_t*)it->value,pkt->ppp_proto);

		// Add more here ...
		default:
			//Should NEVER reach this point; TODO, add trace for debugging?
			return false;
	}
}


/* 
* DEBUG/INFO dumping routines 
*/

//Dump packet matches
void of12_dump_packet_matches(of12_packet_matches_t *const pkt){

	fprintf(stderr,"Packet matches [");	

	//Ports
	if(pkt->port_in)
		fprintf(stderr,"PORT_IN:%u, ",pkt->port_in);
	if(pkt->phy_port_in)
		fprintf(stderr,"PHY_PORT_IN:%u, ",pkt->phy_port_in);
	//Metadata	
	if(pkt->metadata)
		fprintf(stderr,"METADATA:%u, ",pkt->phy_port_in);
	//802	
	if(pkt->eth_src)
		fprintf(stderr,"ETH_SRC:0x%llx, ",(long long unsigned)pkt->eth_src);
	if(pkt->eth_dst)
		fprintf(stderr,"ETH_DST:0x%llx, ",(long long unsigned)pkt->eth_dst);
	if(pkt->eth_type)
		fprintf(stderr,"ETH_TYPE:0x%x, ",pkt->eth_type);
	//802.1q
	if(pkt->vlan_vid)
		fprintf(stderr,"VLAN_VID:%u, ",pkt->vlan_vid);
	if(pkt->vlan_pcp)
		fprintf(stderr,"VLAN_PCP:%u, ",pkt->vlan_pcp);
	//IP/IPv4
	if(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 )
		fprintf(stderr,"IP_PROTO:%u, ",pkt->ip_proto);

	if(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 )
		fprintf(stderr,"IP_ECN:0x%x, ",pkt->ip_ecn);
	
	if(pkt->eth_type == OF12_ETH_TYPE_IPV4 || pkt->eth_type == OF12_ETH_TYPE_IPV6 )
		fprintf(stderr,"IP_DSCP:0x%x, ",pkt->ip_dscp);
	
	if(pkt->ipv4_src)
		fprintf(stderr,"IPV4_SRC:0x%x, ",pkt->ipv4_src);
	if(pkt->ipv4_dst)
		fprintf(stderr,"IPV4_DST:0x%x, ",pkt->ipv4_dst);
	//TCP
	if(pkt->tcp_src)
		fprintf(stderr,"TCP_SRC:%u, ",pkt->tcp_src);
	if(pkt->tcp_dst)
		fprintf(stderr,"TCP_DST:%u, ",pkt->tcp_dst);
	//UDP
	if(pkt->udp_src)
		fprintf(stderr,"UDP_SRC:%u, ",pkt->udp_src);
	if(pkt->udp_dst)
		fprintf(stderr,"UDP_DST:%u, ",pkt->udp_dst);
	//ICMPV4
	if(pkt->ip_proto == OF12_IP_PROTO_ICMPV4)
		fprintf(stderr,"ICMPV4_TYPE:%u, ICMPV4_CODE:%u, ",pkt->icmpv4_type,pkt->icmpv4_code);
	//MPLS	
   	if(pkt->eth_type == OF12_ETH_TYPE_MPLS_UNICAST || pkt->eth_type == OF12_ETH_TYPE_MPLS_MULTICAST )
		fprintf(stderr,"MPLS_LABEL:0x%x, MPLS_TC:0x%x, ",pkt->mpls_label, pkt->mpls_tc);
	//PPPoE
	if(pkt->eth_type == OF12_ETH_TYPE_PPPOE_DISCOVERY || pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION ){
		fprintf(stderr,"PPPOE_CODE:0x%x, PPPOE_TYPE:0x%x, PPPOE_SID:0x%x, ",pkt->pppoe_code, pkt->pppoe_type,pkt->pppoe_sid);
		//PPP
		if(pkt->eth_type == OF12_ETH_TYPE_PPPOE_SESSION)
			fprintf(stderr,"PPP_PROTO:0x%x, ",pkt->ppp_proto);
				
	}
		 

	fprintf(stderr,"]");	
	//Add more here...	
}

//Matches without mask (in matches that do not support)
void of12_dump_matches(of12_match_t* matches){
	of12_match_t* it;
	for(it=matches;it;it=it->next){
		switch(it->type){
			case OF12_MATCH_IN_PORT: fprintf(stderr,"[PORT_IN:%u], ",((utern32_t*)it->value)->value); 
				break;
			case OF12_MATCH_IN_PHY_PORT: fprintf(stderr,"[PHY_PORT_IN:%u], ",((utern32_t*)it->value)->value);
				break; 

			case OF12_MATCH_METADATA: //TODO FIXME
						break;

			case OF12_MATCH_ETH_DST: fprintf(stderr,"[ETH_DST:0x%llx|0x%llx],  ",(long long unsigned)((utern64_t*)it->value)->value,(long long unsigned)((utern64_t*)it->value)->mask);
				break; 
			case OF12_MATCH_ETH_SRC:  fprintf(stderr,"[ETH_SRC:0x%llx|0x%llx], ",(long long unsigned)((utern64_t*)it->value)->value,(long long unsigned)((utern64_t*)it->value)->mask);
				break; 
			case OF12_MATCH_ETH_TYPE:  fprintf(stderr,"[ETH_TYPE:0x%x], ",((utern16_t*)it->value)->value);
				break; 

			case OF12_MATCH_VLAN_VID:  fprintf(stderr,"[VLAN_ID:%u|0x%x], ",((utern16_t*)it->value)->value,((utern16_t*)it->value)->mask);
				break; 
			case OF12_MATCH_VLAN_PCP:  fprintf(stderr,"[VLAN_PCP:%u], ",((utern8_t*)it->value)->value);
				break; 

			case OF12_MATCH_MPLS_LABEL:  fprintf(stderr,"[MPLS_LABEL:%u], ",((utern32_t*)it->value)->value);
				break; 
			case OF12_MATCH_MPLS_TC:  fprintf(stderr,"[MPLS_TC:0x%x], ",((utern8_t*)it->value)->value);
				break; 

			case OF12_MATCH_IP_PROTO:  fprintf(stderr,"[IP_PROTO:%u], ",((utern8_t*)it->value)->value);
				break; 
			case OF12_MATCH_IP_ECN:  fprintf(stderr,"[IP_ECN:%u], ",((utern8_t*)it->value)->value);
				break; 
			case OF12_MATCH_IP_DSCP:  fprintf(stderr,"[IP_DSCP:%u], ",((utern8_t*)it->value)->value);
				break; 

			case OF12_MATCH_IPV4_SRC:  fprintf(stderr,"[IP4_SRC:0x%x|0x%x], ",((utern32_t*)it->value)->value,((utern32_t*)it->value)->mask);
				break; 
			case OF12_MATCH_IPV4_DST:  fprintf(stderr,"[IP4_DST:0x%x|0x%x], ",((utern32_t*)it->value)->value,((utern32_t*)it->value)->mask);
				break; 

			case OF12_MATCH_TCP_SRC:  fprintf(stderr,"[TCP_SRC:%u], ",((utern16_t*)it->value)->value);
				break; 
			case OF12_MATCH_TCP_DST:  fprintf(stderr,"[TCP_DST:%u], ",((utern16_t*)it->value)->value);
				break; 

			case OF12_MATCH_UDP_SRC:  fprintf(stderr,"[UDP_SRC:%u], ",((utern16_t*)it->value)->value);
				break; 
			case OF12_MATCH_UDP_DST:  fprintf(stderr,"[UDP_DST:%u], ",((utern16_t*)it->value)->value);
				break; 

			case OF12_MATCH_ICMPV4_TYPE:  fprintf(stderr,"[ICMPV4_TYPE:%u], ",((utern8_t*)it->value)->value);
				break; 
			case OF12_MATCH_ICMPV4_CODE:  fprintf(stderr,"[ICMPV4_CODE:%u], ",((utern8_t*)it->value)->value);
				break; 
			
			/* PPP/PPPoE related extensions */
			case OF12_MATCH_PPPOE_CODE:  fprintf(stderr,"[PPPOE_CODE:%u], ",((utern8_t*)it->value)->value);
				break; 
			case OF12_MATCH_PPPOE_TYPE:  fprintf(stderr,"[PPPOE_TYPE:%u], ",((utern8_t*)it->value)->value);
				break; 
			case OF12_MATCH_PPPOE_SID:  fprintf(stderr,"[PPPOE_SID:%u], ",((utern16_t*)it->value)->value);
				break; 
			case OF12_MATCH_PPP_PROT:  fprintf(stderr,"[PPP_PROT:%u] ",((utern16_t*)it->value)->value);
				break; 
			/* Add more here ...*/
			default:
				fprintf(stderr,"[UNKOWN!],");
				//Should NEVER reach this point
				
		}
	}	
}

//Matches with mask (including matches that do not support)
void of12_full_dump_matches(of12_match_t* matches){
	of12_match_t* it;
	for(it=matches;it;it=it->next){
		switch(it->type){
			case OF12_MATCH_IN_PORT: fprintf(stderr,"[PORT_IN:%u|0x%x], ",((utern32_t*)it->value)->value,((utern32_t*)it->value)->mask); 
				break;
			case OF12_MATCH_IN_PHY_PORT: fprintf(stderr,"[PHY_PORT_IN:%u|0x%x], ",((utern32_t*)it->value)->value,((utern32_t*)it->value)->mask);
				break; 
			case OF12_MATCH_METADATA: //TODO FIXME
						break;
			case OF12_MATCH_ETH_DST: fprintf(stderr,"[ETH_DST:0x%llx|0x%llx],  ",(long long unsigned)((utern64_t*)it->value)->value,(long long unsigned)((utern64_t*)it->value)->mask);
				break; 
			case OF12_MATCH_ETH_SRC:  fprintf(stderr,"[ETH_SRC:0x%llx|0x%llx], ",(long long unsigned)((utern64_t*)it->value)->value,(long long unsigned)((utern64_t*)it->value)->mask);
				break; 
			case OF12_MATCH_ETH_TYPE:  fprintf(stderr,"[ETH_TYPE:0x%x|0x%x], ",((utern16_t*)it->value)->value,((utern16_t*)it->value)->mask);
				break; 
			case OF12_MATCH_VLAN_VID:  fprintf(stderr,"[VLAN_ID:%u|0x%x], ",((utern16_t*)it->value)->value,((utern16_t*)it->value)->mask);
				break; 
			case OF12_MATCH_VLAN_PCP:  fprintf(stderr,"[VLAN_PCP:%u|0x%x], ",((utern8_t*)it->value)->value,((utern8_t*)it->value)->mask);
				break; 
			case OF12_MATCH_MPLS_LABEL:  fprintf(stderr,"[MPLS_LABEL:%u|0x%x], ",((utern32_t*)it->value)->value,((utern32_t*)it->value)->mask);
				break; 
			case OF12_MATCH_MPLS_TC:  fprintf(stderr,"[MPLS_TC:%u|0x%x], ",((utern8_t*)it->value)->value,((utern8_t*)it->value)->mask);
				break; 
			case OF12_MATCH_IP_PROTO:  fprintf(stderr,"[IP_PROTO:%u|0x%x], ",((utern8_t*)it->value)->value,((utern8_t*)it->value)->mask);
				break; 
			case OF12_MATCH_IPV4_SRC:  fprintf(stderr,"[IP4_SRC:0x%x|0x%x], ",((utern32_t*)it->value)->value,((utern32_t*)it->value)->mask);
				break; 
			case OF12_MATCH_IPV4_DST:  fprintf(stderr,"[IP4_DST:0x%x|0x%x], ",((utern32_t*)it->value)->value,((utern32_t*)it->value)->mask);
				break; 
			case OF12_MATCH_TCP_SRC:  fprintf(stderr,"[TCP_SRC:%u|0x%x], ",((utern16_t*)it->value)->value,((utern16_t*)it->value)->mask);
				break; 
			case OF12_MATCH_TCP_DST:  fprintf(stderr,"[TCP_DST:%u|0x%x], ",((utern16_t*)it->value)->value,((utern16_t*)it->value)->mask);
				break; 
			case OF12_MATCH_UDP_SRC:  fprintf(stderr,"[UDP_SRC:%u|0x%x], ",((utern16_t*)it->value)->value,((utern16_t*)it->value)->mask);
				break; 
			case OF12_MATCH_UDP_DST:  fprintf(stderr,"[UDP_DST:%u|0x%x], ",((utern16_t*)it->value)->value,((utern16_t*)it->value)->mask);
				break; 
			case OF12_MATCH_ICMPV4_TYPE:  fprintf(stderr,"[ICMPV4_TYPE:%u|0x%x], ",((utern8_t*)it->value)->value,((utern8_t*)it->value)->mask);
				break; 
			case OF12_MATCH_ICMPV4_CODE:  fprintf(stderr,"[ICMPV4_CODE:%u|0x%x], ",((utern8_t*)it->value)->value,((utern8_t*)it->value)->mask);
				break; 
			
			/* PPP/PPPoE related extensions */
			case OF12_MATCH_PPPOE_CODE:  fprintf(stderr,"[PPPOE_CODE:%u|0x%x], ",((utern8_t*)it->value)->value,((utern8_t*)it->value)->mask);
				break; 
			case OF12_MATCH_PPPOE_TYPE:  fprintf(stderr,"[PPPOE_TYPE:%u|0x%x], ",((utern8_t*)it->value)->value,((utern8_t*)it->value)->mask);
				break; 
			case OF12_MATCH_PPPOE_SID:  fprintf(stderr,"[PPPOE_SID:%u|0x%x], ",((utern16_t*)it->value)->value,((utern16_t*)it->value)->mask);
				break; 
			case OF12_MATCH_PPP_PROT:  fprintf(stderr,"[PPP_PROT:%u|0x%x] ",((utern16_t*)it->value)->value,((utern16_t*)it->value)->mask);
				break; 
			/* Add more here ...*/
			default:
				fprintf(stderr,"[UNKOWN!],");
				//Should NEVER reach this point
				
		}
	}	
}

