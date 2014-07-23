/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <assert.h>
#include "of1x_match.h"

#include "../../../common/datapacket.h"
#include "../../../common/protocol_constants.h"
#include "../../../platform/memory.h"
#include "../../../platform/likely.h"
#include "../../../util/logging.h"

/*
* Initializers 
*/

#define OF1X_MIN_VERSION OF_VERSION_10
#define OF1X_MAX_VERSION OF_VERSION_13

//Phy
of1x_match_t* of1x_init_port_in_match(uint32_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_IN_PORT; 
	match->__tern = __init_utern32(value,OF1X_4_BYTE_MASK); //No wildcard

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

of1x_match_t* of1x_init_port_in_phy_match(uint32_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_IN_PHY_PORT; 
	match->__tern = __init_utern32(value,OF1X_4_BYTE_MASK); //No wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//METADATA
of1x_match_t* of1x_init_metadata_match(uint64_t value, uint64_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_METADATA; 
	match->__tern = __init_utern64(value, mask);

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_8_BYTE_MASK) != OF1X_8_BYTE_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//ETHERNET
of1x_match_t* of1x_init_eth_dst_match(uint64_t value, uint64_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB64(OF1X_MAC_ALIGN(value));
	mask = HTONB64(OF1X_MAC_ALIGN(mask));
	
	match->type = OF1X_MATCH_ETH_DST; 
	match->__tern = __init_utern64(value&OF1X_48_BITS_MASK, mask&OF1X_48_BITS_MASK); //Enforce mask bits are always 00 for the first bits


	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_48_BITS_MASK) != OF1X_48_BITS_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_eth_src_match(uint64_t value, uint64_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB64(OF1X_MAC_ALIGN(value));
	mask = HTONB64(OF1X_MAC_ALIGN(mask));

	match->type = OF1X_MATCH_ETH_SRC; 
	match->__tern = __init_utern64(value&OF1X_48_BITS_MASK, mask&OF1X_48_BITS_MASK); //Enforce mask bits are always 00 for the first bits
	
	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_48_BITS_MASK) != OF1X_48_BITS_MASK )
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_eth_type_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	value = HTONB16(value);

	match->type = OF1X_MATCH_ETH_TYPE; 
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //No wildcard 
	
	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards
	
	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//8021.q
of1x_match_t* of1x_init_vlan_vid_match(uint16_t value, uint16_t mask, enum of1x_vlan_present vlan_present){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);
	mask = HTONB16(mask);

	match->type = OF1X_MATCH_VLAN_VID; 
	//Setting values; note that value includes the flag HAS_VLAN in the 13th bit
	//The mask is set to be strictly 12 bits, so only matching the VLAN ID itself
	match->__tern = __init_utern16(value&OF1X_VLAN_ID_MASK,mask&OF1X_VLAN_ID_MASK);
	match->vlan_present = vlan_present;

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_VLAN_ID_MASK) != OF1X_VLAN_ID_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_vlan_pcp_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = OF1X_VLAN_PCP_ALIGN(value);

	match->type = OF1X_MATCH_VLAN_PCP; 
	match->__tern = __init_utern8(value&OF1X_3MSBITS_MASK,OF1X_3MSBITS_MASK); //Ensure only 3 bit value, no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards
	
	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//MPLS
of1x_match_t* of1x_init_mpls_label_match(uint32_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB32(OF1X_MPLS_LABEL_ALIGN(value));

	match->type = OF1X_MATCH_MPLS_LABEL; 
	match->__tern = __init_utern32(value&OF1X_20_BITS_MASK,OF1X_20_BITS_MASK); //no wildcard?? wtf! 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards
	
	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_mpls_tc_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = OF1X_MPLS_TC_ALIGN(value);

	match->type = OF1X_MATCH_MPLS_TC; 
	match->__tern = __init_utern8(value&OF1X_BITS_12AND3_MASK,OF1X_BITS_12AND3_MASK); //Ensure only 3 bit value, no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards
	
	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_mpls_bos_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
	
	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_MPLS_BOS; 
	match->__tern = __init_utern8(value&OF1X_BIT0_MASK,OF1X_BIT0_MASK); //Ensure only 1 bit value, no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_13;	//First supported in OF1.3
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards
	
	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//ARP
of1x_match_t* of1x_init_arp_opcode_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);

	match->type = OF1X_MATCH_ARP_OP;
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //No wildcard

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0 (1.0: lower 8bits of opcode)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_arp_tha_match(uint64_t value, uint64_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
	
	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB64(OF1X_MAC_ALIGN(value));
	mask = HTONB64(OF1X_MAC_ALIGN(mask));

	match->type = OF1X_MATCH_ARP_THA;
	match->__tern = __init_utern64(value&OF1X_48_BITS_MASK, mask&OF1X_48_BITS_MASK); //Enforce mask bits are always 00 for the first bits


	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_48_BITS_MASK) != OF1X_48_BITS_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_arp_sha_match(uint64_t value, uint64_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB64(OF1X_MAC_ALIGN(value));
	mask = HTONB64(OF1X_MAC_ALIGN(mask));

	match->type = OF1X_MATCH_ARP_SHA;
	match->__tern = __init_utern64(value&OF1X_48_BITS_MASK, mask&OF1X_48_BITS_MASK); //Enforce mask bits are always 00 for the first bits

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_48_BITS_MASK) != OF1X_48_BITS_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_arp_tpa_match(uint32_t value, uint32_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB32(value);
	mask = HTONB32(mask);

	match->type = OF1X_MATCH_ARP_TPA;
	match->__tern = __init_utern32(value,mask);

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( mask != OF1X_4_BYTE_MASK )
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_arp_spa_match(uint32_t value, uint32_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB32(value);
	mask = HTONB32(mask);

	match->type = OF1X_MATCH_ARP_SPA;
	match->__tern = __init_utern32(value,mask);

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( mask != OF1X_4_BYTE_MASK )
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//NW
of1x_match_t* of1x_init_nw_proto_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_NW_PROTO; 
	match->__tern = __init_utern8(value,OF1X_1_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF_VERSION_10; //Last supported in OF1.0
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_nw_src_match(uint32_t value, uint32_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB32(value);
	mask = HTONB32(mask);

	match->type = OF1X_MATCH_NW_SRC;
	match->__tern = __init_utern32(value,mask); 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF_VERSION_10; //Last supported in OF1.0
	if( (mask&OF1X_4_BYTE_MASK) != OF1X_4_BYTE_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_nw_dst_match(uint32_t value, uint32_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB32(value);
	mask = HTONB32(mask);

	match->type = OF1X_MATCH_NW_DST;
	match->__tern = __init_utern32(value,mask); 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF_VERSION_10; //Last supported in OF1.0
	if( (mask&OF1X_4_BYTE_MASK) != OF1X_4_BYTE_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//IPv4
of1x_match_t* of1x_init_ip4_src_match(uint32_t value, uint32_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB32(value);
	mask = HTONB32(mask);

	match->type = OF1X_MATCH_IPV4_SRC;
	match->__tern = __init_utern32(value,mask); 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_4_BYTE_MASK) != OF1X_4_BYTE_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_ip4_dst_match(uint32_t value, uint32_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB32(value);
	mask = HTONB32(mask);

	match->type = OF1X_MATCH_IPV4_DST;
	match->__tern = __init_utern32(value,mask); 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_4_BYTE_MASK) != OF1X_4_BYTE_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_ip_proto_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
	
	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_IP_PROTO; 
	match->__tern = __init_utern8(value,OF1X_1_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_ip_dscp_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
	
	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = OF1X_IP_DSCP_ALIGN(value);

	match->type = OF1X_MATCH_IP_DSCP; 
	match->__tern = __init_utern8(value&OF1X_6MSBITS_MASK,OF1X_6MSBITS_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0 (ToS)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

of1x_match_t* of1x_init_ip_ecn_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_IP_ECN; 
	match->__tern = __init_utern8(value&OF1X_2LSBITS_MASK,OF1X_2LSBITS_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//IPv6
of1x_match_t* of1x_init_ip6_src_match(uint128__t value, uint128__t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
	
	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	HTONB128(value);
	HTONB128(mask);

	uint128__t fixed_mask = {{0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff}};
	match->type = OF1X_MATCH_IPV6_SRC;
	match->__tern = __init_utern128(value,mask); 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if(memcmp(&fixed_mask,&mask, sizeof(mask)) != 0)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_ip6_dst_match(uint128__t value, uint128__t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	HTONB128(value);
	HTONB128(mask);

	uint128__t fixed_mask = {{0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff}};
	match->type = OF1X_MATCH_IPV6_DST;
	match->__tern = __init_utern128(value,mask); 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if(memcmp(&fixed_mask,&mask, sizeof(mask)) != 0)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_ip6_flabel_match(uint32_t value, uint32_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB32(OF1X_IP6_FLABEL_ALIGN(value));
	mask = HTONB32(OF1X_IP6_FLABEL_ALIGN(mask));

	match->type = OF1X_MATCH_IPV6_FLABEL;
	match->__tern = __init_utern32(value&OF1X_20_BITS_IPV6_FLABEL_MASK,mask&OF1X_20_BITS_IPV6_FLABEL_MASK); // ensure 20 bits. 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_ip6_nd_target_match(uint128__t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
	
	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	HTONB128(value);

	uint128__t mask = {{0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff}};
	
	match->type = OF1X_MATCH_IPV6_ND_TARGET;
	match->__tern = __init_utern128(value,mask); //No wildcard

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_ip6_nd_sll_match(uint64_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB64(OF1X_MAC_ALIGN(value));

	match->type = OF1X_MATCH_IPV6_ND_SLL;
	match->__tern = __init_utern64(value & OF1X_48_BITS_MASK, OF1X_48_BITS_MASK); //ensure 48 bits. No wildcard

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_ip6_nd_tll_match(uint64_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB64(OF1X_MAC_ALIGN(value));

	match->type = OF1X_MATCH_IPV6_ND_TLL;
	match->__tern = __init_utern64(value & OF1X_48_BITS_MASK, OF1X_48_BITS_MASK); //ensure 48 bits. No wildcard

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_ip6_exthdr_match(uint16_t value, uint16_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// TODO Align to pipeline convention (NBO, lower memory address) -- currently not implemented

	match->type = OF1X_MATCH_IPV6_EXTHDR;
	match->__tern = __init_utern16(value&OF1X_9_BITS_MASK, mask & OF1X_9_BITS_MASK );  //ensure 9 bits, with Wildcard

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_13;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_9_BITS_MASK) != OF1X_9_BITS_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//ICMPV6
of1x_match_t* of1x_init_icmpv6_type_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_ICMPV6_TYPE;
	match->__tern = __init_utern8(value,OF1X_1_BYTE_MASK);

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_icmpv6_code_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
	
	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_ICMPV6_CODE;
	match->__tern = __init_utern8(value,OF1X_1_BYTE_MASK);

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//TCP
of1x_match_t* of1x_init_tcp_src_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);

	match->type = OF1X_MATCH_TCP_SRC;
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards


	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_tcp_dst_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);

	match->type = OF1X_MATCH_TCP_DST;
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
//UDP
of1x_match_t* of1x_init_udp_src_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);

	match->type = OF1X_MATCH_UDP_SRC;
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_udp_dst_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);

	match->type = OF1X_MATCH_UDP_DST;
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//SCTP
of1x_match_t* of1x_init_sctp_src_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);

	match->type = OF1X_MATCH_SCTP_SRC;
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards


	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_sctp_dst_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
	
	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);

	match->type = OF1X_MATCH_SCTP_DST;
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//TP
of1x_match_t* of1x_init_tp_src_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);

	match->type = OF1X_MATCH_TP_SRC;
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF_VERSION_10;	//Last supported in OF1.0
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_tp_dst_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);

	match->type = OF1X_MATCH_TP_DST;
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_10;	//First supported in OF1.0
	match->ver_req.max_ver = OF_VERSION_10;	//Last supported in OF1.0
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
//ICMPv4
of1x_match_t* of1x_init_icmpv4_type_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_ICMPV4_TYPE; 
	match->__tern = __init_utern8(value,OF1X_1_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_icmpv4_code_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_ICMPV4_CODE; 
	match->__tern = __init_utern8(value,OF1X_1_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//PBB
of1x_match_t* of1x_init_pbb_isid_match(uint32_t value, uint32_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
	
	if(unlikely(match == NULL))
		return NULL;
	
	//TODO Align to pipeline convention (NBO, lower memory address) -- currently not implemented

	match->type = OF1X_MATCH_PBB_ISID;
	match->__tern = __init_utern32(value&OF1X_3_BYTE_MASK, mask&OF1X_3_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_13;	//First supported in OF1.3
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_3_BYTE_MASK) == OF1X_3_BYTE_MASK)
		match->has_wildcard = false;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//Tunnel Id
of1x_match_t* of1x_init_tunnel_id_match(uint64_t value, uint64_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	//TODO align?

	match->type = OF1X_MATCH_TUNNEL_ID; 
	match->__tern = __init_utern64(value, mask); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_13;	//First supported in OF1.3
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if(mask != OF1X_8_BYTE_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//Add more here...

/* Extensions */

//PPPoE
of1x_match_t* of1x_init_pppoe_code_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_PPPOE_CODE; 
	match->__tern = __init_utern8(value&OF1X_1_BYTE_MASK,OF1X_1_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_pppoe_type_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_PPPOE_TYPE; 
	match->__tern = __init_utern8(value&OF1X_4_BITS_MASK,OF1X_4_BITS_MASK); //Ensure only 4 bit value, no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_pppoe_session_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);

	match->type = OF1X_MATCH_PPPOE_SID; 
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting w
	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
//PPP
of1x_match_t* of1x_init_ppp_prot_match(uint16_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);

	match->type = OF1X_MATCH_PPP_PROT; 
	match->__tern = __init_utern16(value,OF1X_2_BYTE_MASK); //no wildcard 

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
//GTP
of1x_match_t* of1x_init_gtp_msg_type_match(uint8_t value){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
	
	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_GTP_MSG_TYPE;
	match->__tern = __init_utern8(value,OF1X_1_BYTE_MASK); //no wildcard

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	match->has_wildcard = false;		//Not accepting wildcards

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_gtp_teid_match(uint32_t value, uint32_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
	
	if(unlikely(match == NULL))
		return NULL;
	
	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB32(value);

	match->type = OF1X_MATCH_GTP_TEID;
	match->__tern = __init_utern32(value, mask);

	//Set fast validation flags	
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( mask != OF1X_4_BYTE_MASK )
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
//CAPWAP
of1x_match_t* of1x_init_capwap_wbid_match(uint8_t value, uint8_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_CAPWAP_WBID;
	match->__tern = __init_utern8(value, mask);

	//Set fast validation flags
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( mask != OF1X_1_BYTE_MASK )
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_capwap_rid_match(uint8_t value, uint8_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_CAPWAP_RID;
	match->__tern = __init_utern8(value, mask);

	//Set fast validation flags
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( mask != OF1X_1_BYTE_MASK )
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_capwap_flags_match(uint16_t value, uint16_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);
	mask = HTONB16(mask);

	match->type = OF1X_MATCH_CAPWAP_FLAGS;
	match->__tern = __init_utern16(value, mask);

	//Set fast validation flags
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( mask != OF1X_2_BYTE_MASK )
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
//WLAN
of1x_match_t* of1x_init_wlan_fc_match(uint16_t value, uint16_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB16(value);
	mask = HTONB16(mask);

	match->type = OF1X_MATCH_WLAN_FC;
	match->__tern = __init_utern16(value, mask);

	//Set fast validation flags
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( mask != OF1X_2_BYTE_MASK )
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_wlan_type_match(uint8_t value, uint8_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_WLAN_TYPE;
	match->__tern = __init_utern8(value, mask);

	//Set fast validation flags
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( mask != OF1X_1_BYTE_MASK )
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_wlan_subtype_match(uint8_t value, uint8_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_WLAN_SUBTYPE;
	match->__tern = __init_utern8(value, mask);

	//Set fast validation flags
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( mask != OF1X_1_BYTE_MASK )
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_wlan_direction_match(uint8_t value, uint8_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	match->type = OF1X_MATCH_WLAN_DIRECTION;
	match->__tern = __init_utern8(value, mask);

	//Set fast validation flags
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2 (extensions)
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( mask != OF1X_1_BYTE_MASK )
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_wlan_address_1_match(uint64_t value, uint64_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB64(OF1X_MAC_ALIGN(value));
	mask = HTONB64(OF1X_MAC_ALIGN(mask));

	match->type = OF1X_MATCH_WLAN_ADDRESS_1;
	match->__tern = __init_utern64(value&OF1X_48_BITS_MASK, mask&OF1X_48_BITS_MASK); //Enforce mask bits are always 00 for the first bits

	//Set fast validation flags
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_48_BITS_MASK) != OF1X_48_BITS_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_wlan_address_2_match(uint64_t value, uint64_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB64(OF1X_MAC_ALIGN(value));
	mask = HTONB64(OF1X_MAC_ALIGN(mask));

	match->type = OF1X_MATCH_WLAN_ADDRESS_2;
	match->__tern = __init_utern64(value&OF1X_48_BITS_MASK, mask&OF1X_48_BITS_MASK); //Enforce mask bits are always 00 for the first bits

	//Set fast validation flags
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_48_BITS_MASK) != OF1X_48_BITS_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}
of1x_match_t* of1x_init_wlan_address_3_match(uint64_t value, uint64_t mask){
	of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(unlikely(match == NULL))
		return NULL;

	// Align to pipeline convention (NBO, lower memory address)
	value = HTONB64(OF1X_MAC_ALIGN(value));
	mask = HTONB64(OF1X_MAC_ALIGN(mask));

	match->type = OF1X_MATCH_WLAN_ADDRESS_3;
	match->__tern = __init_utern64(value&OF1X_48_BITS_MASK, mask&OF1X_48_BITS_MASK); //Enforce mask bits are always 00 for the first bits

	//Set fast validation flags
	match->ver_req.min_ver = OF_VERSION_12;	//First supported in OF1.2
	match->ver_req.max_ver = OF1X_MAX_VERSION;		//No limitation on max
	if( (mask&OF1X_48_BITS_MASK) != OF1X_48_BITS_MASK)
		match->has_wildcard = true;
	else
		match->has_wildcard = false;

	//Initialize linked-list
	match->prev=match->next=NULL;

	return match;
}

//Add more here...

/* Instruction groups init and destroy */
void __of1x_init_match_group(of1x_match_group_t* group){

	platform_memset(group,0,sizeof(of1x_match_group_t));
	
	//Set min max 
	group->ver_req.min_ver = OF1X_MIN_VERSION;
	group->ver_req.max_ver = OF1X_MAX_VERSION;
	
	//OF1.0 full wildcard
	bitmap128_set(&group->of10_wildcard_bm, OF1X_MATCH_ETH_DST);
	bitmap128_set(&group->of10_wildcard_bm, OF1X_MATCH_ETH_SRC);
	bitmap128_set(&group->of10_wildcard_bm, OF1X_MATCH_NW_SRC);
	bitmap128_set(&group->of10_wildcard_bm, OF1X_MATCH_NW_DST);
}

void __of1x_destroy_match_group(of1x_match_group_t* group){
	of1x_match_t *match;

	if ( unlikely(group->head==NULL) )
		return;

	match = group->head;

	while (match){
		of1x_match_t *next = match->next;
		of1x_destroy_match(match);
		match = next;
	}

	group->head = NULL; 
	group->tail = NULL; 
}



void __of1x_match_group_push_back(of1x_match_group_t* group, of1x_match_t* match){

	if ( unlikely(group==NULL) || unlikely(match==NULL) )
		return;

	match->next = match->prev = NULL; 

	if(!group->head){
		group->head = match;
	}else{
		match->prev = group->tail;
		group->tail->next = match;
	}

	//Deduce new tail and update validation flags and num of elements
	do{
		//Update fast validation flags (required versions)
		if(group->ver_req.min_ver < match->ver_req.min_ver)
			group->ver_req.min_ver = match->ver_req.min_ver;
		if(group->ver_req.max_ver > match->ver_req.max_ver)
			group->ver_req.max_ver = match->ver_req.max_ver;

		//Update matches
		bitmap128_set(&group->match_bm, match->type);

		if(!match->has_wildcard)
			bitmap128_unset(&group->of10_wildcard_bm, match->type);
		else	
			bitmap128_set(&group->wildcard_bm, match->type);

		group->num_elements++;

		if(match->next == NULL)
			break;
		else	
			match = match->next;
	}while(1);
	
	//Add new tail
	group->tail = match;
	
}

/*
* Copy match to heap. Leaves next and prev pointers to NULL
*/
of1x_match_t* __of1x_copy_match(of1x_match_t* match){

	of1x_match_t* tmp = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));

	if(!tmp)
		return tmp;

	//Copy contents
	*tmp = *match;
	
	//Initialize linked-list to null
	tmp->prev=tmp->next=NULL;

	//Create a whatever type utern and copy from the orignal tern
	tmp->__tern = __init_utern8(0x0, 0x0);

	if(!tmp->__tern)
		return NULL;

	*tmp->__tern = *match->__tern;

	return tmp;
}

/* 
* Whole (linked list) Match copy -> this should be deprecated in favour of the match group
*/
of1x_match_t* __of1x_copy_matches(of1x_match_t* matches){

	of1x_match_t* prev, *curr, *it, *copy;
	
	if( unlikely(matches==NULL) )
		return NULL;
	
	for(prev=NULL,copy=NULL, it=matches; it; it = it->next){

		curr = __of1x_copy_match(it);

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
of1x_match_t* __of1x_get_alike_match(of1x_match_t* match1, of1x_match_t* match2){
	utern_t* common_tern = NULL;	

	if( match1->type != match2->type )
		return NULL;	

	common_tern = __utern_get_alike(*match1->__tern,*match2->__tern);

	if(common_tern){
		of1x_match_t* match = (of1x_match_t*)platform_malloc_shared(sizeof(of1x_match_t));
		match->__tern = common_tern;
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
void of1x_destroy_match(of1x_match_t* match){
	__destroy_utern(match->__tern);
	platform_free_shared(match);
}

/*
*
* Matching routines...
* 
*/

//Compare matches
bool __of1x_equal_matches(of1x_match_t* match1, of1x_match_t* match2){

	if( match1->type != match2->type )
		return false; 

	return __utern_equals(match1->__tern,match2->__tern);
}

//Finds out if sub_match is a submatch of match
bool __of1x_is_submatch(of1x_match_t* sub_match, of1x_match_t* match){

	if( match->type != sub_match->type )
		return false; 
	
	return __utern_is_contained(sub_match->__tern,match->__tern);
}

//Matches with mask (including matches that do not support)
void __of1x_dump_matches(of1x_match_t* matches, bool raw_nbo){
	of1x_match_t* it;
	for(it=matches;it;it=it->next){
		switch(it->type){
			case OF1X_MATCH_IN_PORT: ROFL_PIPELINE_INFO_NO_PREFIX("[PORT_IN:%u], ", __of1x_get_match_val32(it, false, raw_nbo)); 
				break;
			case OF1X_MATCH_IN_PHY_PORT: ROFL_PIPELINE_INFO_NO_PREFIX("[PHY_PORT_IN:%u], ", __of1x_get_match_val32(it, false, raw_nbo));
				break; 

			case OF1X_MATCH_METADATA: ROFL_PIPELINE_INFO_NO_PREFIX("[METADATA:0x%"PRIx64"|0x%"PRIx64"],  ", __of1x_get_match_val64(it, false, raw_nbo), __of1x_get_match_val64(it, true, raw_nbo)); 
				break;

			case OF1X_MATCH_ETH_DST: ROFL_PIPELINE_INFO_NO_PREFIX("[ETH_DST:0x%"PRIx64"|0x%"PRIx64"],  ",__of1x_get_match_val64(it, false, raw_nbo),__of1x_get_match_val64(it, true, raw_nbo));
				break; 
			case OF1X_MATCH_ETH_SRC:  ROFL_PIPELINE_INFO_NO_PREFIX("[ETH_SRC:0x%"PRIx64"|0x%"PRIx64"], ",__of1x_get_match_val64(it, false, raw_nbo),__of1x_get_match_val64(it, true, raw_nbo));
				break; 
			case OF1X_MATCH_ETH_TYPE:  ROFL_PIPELINE_INFO_NO_PREFIX("[ETH_TYPE:0x%x], ",__of1x_get_match_val16(it, false, raw_nbo));
				break; 

			case OF1X_MATCH_VLAN_VID:  	if(it->vlan_present == OF1X_MATCH_VLAN_NONE)
								ROFL_PIPELINE_INFO_NO_PREFIX("[NO_VLAN], ");
							else if(it->vlan_present == OF1X_MATCH_VLAN_ANY)
								ROFL_PIPELINE_INFO_NO_PREFIX("[ANY_VLAN], ");
							else
								ROFL_PIPELINE_INFO_NO_PREFIX("[VLAN_ID:%u|0x%x], ",__of1x_get_match_val16(it, false, raw_nbo),__of1x_get_match_val16(it, true, raw_nbo));
				break; 
			case OF1X_MATCH_VLAN_PCP:  ROFL_PIPELINE_INFO_NO_PREFIX("[VLAN_PCP:%u], ",__of1x_get_match_val8(it, false, raw_nbo));
				break; 

			case OF1X_MATCH_MPLS_LABEL:  ROFL_PIPELINE_INFO_NO_PREFIX("[MPLS_LABEL:0x%x], ",__of1x_get_match_val32(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_MPLS_TC:  ROFL_PIPELINE_INFO_NO_PREFIX("[MPLS_TC:0x%x], ",__of1x_get_match_val8(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_MPLS_BOS:  ROFL_PIPELINE_INFO_NO_PREFIX("[MPLS_BOS:0x%x], ",__of1x_get_match_val8(it, false, raw_nbo));
				break;

			case OF1X_MATCH_ARP_OP: ROFL_PIPELINE_INFO_NO_PREFIX("[ARP_OPCODE:0x%x], ",__of1x_get_match_val16(it, false, raw_nbo));
				break;
			case OF1X_MATCH_ARP_SHA: ROFL_PIPELINE_INFO_NO_PREFIX("[ARP_SHA:0x%"PRIx64"|0x%"PRIx64"], ",__of1x_get_match_val64(it, false, raw_nbo),__of1x_get_match_val64(it, true, raw_nbo));
				break;
			case OF1X_MATCH_ARP_SPA: ROFL_PIPELINE_INFO_NO_PREFIX("[ARP_SPA:0x%x|0x%x], ",__of1x_get_match_val32(it, false, raw_nbo),__of1x_get_match_val32(it, true, raw_nbo));
				break;
			case OF1X_MATCH_ARP_THA: ROFL_PIPELINE_INFO_NO_PREFIX("[ARP_THA:0x%"PRIx64"|0x%"PRIx64"], ",__of1x_get_match_val64(it, false, raw_nbo),__of1x_get_match_val64(it, true, raw_nbo));
				break;
			case OF1X_MATCH_ARP_TPA: ROFL_PIPELINE_INFO_NO_PREFIX("[ARP_TPA:0x%x|0x%x], ",__of1x_get_match_val32(it, false, raw_nbo),__of1x_get_match_val32(it, true, raw_nbo));
				break;

			case OF1X_MATCH_NW_PROTO:  ROFL_PIPELINE_INFO_NO_PREFIX("[NW_PROTO:%u|0x%x], ",__of1x_get_match_val8(it, false, raw_nbo),__of1x_get_match_val8(it, true, raw_nbo));
				break; 
			case OF1X_MATCH_NW_SRC:  ROFL_PIPELINE_INFO_NO_PREFIX("[NW_SRC:0x%x|0x%x], ",__of1x_get_match_val32(it, false, raw_nbo),__of1x_get_match_val32(it, true, raw_nbo));
				break; 
			case OF1X_MATCH_NW_DST:  ROFL_PIPELINE_INFO_NO_PREFIX("[NW_DST:0x%x|0x%x], ",__of1x_get_match_val32(it, false, raw_nbo),__of1x_get_match_val32(it, true, raw_nbo));
				break; 

			case OF1X_MATCH_IP_ECN:  ROFL_PIPELINE_INFO_NO_PREFIX("[IP_ECN:0x%x], ",__of1x_get_match_val8(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_IP_DSCP:  ROFL_PIPELINE_INFO_NO_PREFIX("[IP_DSCP:0x%x], ",__of1x_get_match_val8(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_IP_PROTO:  ROFL_PIPELINE_INFO_NO_PREFIX("[IP_PROTO:%u|0x%x], ",__of1x_get_match_val8(it, false, raw_nbo),__of1x_get_match_val8(it, true, raw_nbo));
				break; 

			case OF1X_MATCH_IPV4_SRC:  ROFL_PIPELINE_INFO_NO_PREFIX("[IP4_SRC:0x%x|0x%x], ",__of1x_get_match_val32(it, false, raw_nbo),__of1x_get_match_val32(it, true, raw_nbo));
				break; 
			case OF1X_MATCH_IPV4_DST:  ROFL_PIPELINE_INFO_NO_PREFIX("[IP4_DST:0x%x|0x%x], ",__of1x_get_match_val32(it, false, raw_nbo),__of1x_get_match_val32(it, true, raw_nbo));
				break; 

			case OF1X_MATCH_TCP_SRC:  ROFL_PIPELINE_INFO_NO_PREFIX("[TCP_SRC:%u], ",__of1x_get_match_val16(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_TCP_DST:  ROFL_PIPELINE_INFO_NO_PREFIX("[TCP_DST:%u], ",__of1x_get_match_val16(it, false, raw_nbo));
				break; 

			case OF1X_MATCH_UDP_SRC:  ROFL_PIPELINE_INFO_NO_PREFIX("[UDP_SRC:%u], ",__of1x_get_match_val16(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_UDP_DST:  ROFL_PIPELINE_INFO_NO_PREFIX("[UDP_DST:%u], ",__of1x_get_match_val16(it, false, raw_nbo));
				break; 

			case OF1X_MATCH_SCTP_SRC:  ROFL_PIPELINE_INFO_NO_PREFIX("[SCTP_SRC:%u], ",__of1x_get_match_val16(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_SCTP_DST:  ROFL_PIPELINE_INFO_NO_PREFIX("[SCTP_DST:%u], ",__of1x_get_match_val16(it, false, raw_nbo));
				break; 

			//OF1.0 only
			case OF1X_MATCH_TP_SRC:  ROFL_PIPELINE_INFO_NO_PREFIX("[TP_SRC:%u], ",__of1x_get_match_val16(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_TP_DST:  ROFL_PIPELINE_INFO_NO_PREFIX("[TP_DST:%u], ",__of1x_get_match_val16(it, false, raw_nbo));
				break; 


			case OF1X_MATCH_ICMPV4_TYPE:  ROFL_PIPELINE_INFO_NO_PREFIX("[ICMPV4_TYPE:%u], ",__of1x_get_match_val8(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_ICMPV4_CODE:  ROFL_PIPELINE_INFO_NO_PREFIX("[ICMPV4_CODE:%u], ",__of1x_get_match_val8(it, false, raw_nbo));
				break; 
			
			//IPv6
			case OF1X_MATCH_IPV6_SRC: 
				{
					uint128__t value = __of1x_get_match_val128(it, false, raw_nbo);	
					uint128__t mask = __of1x_get_match_val128(it, true, raw_nbo);
					(void)value;	
					(void)mask;	
					ROFL_PIPELINE_INFO_NO_PREFIX("[IPV6_SRC:0x%lx:%lx|0x%lx:%lx], ",UINT128__T_HI(value),UINT128__T_LO(value),UINT128__T_HI(mask),UINT128__T_LO(mask));
				}
				break;
			case OF1X_MATCH_IPV6_DST: 
				{
					uint128__t value = __of1x_get_match_val128(it, false, raw_nbo);	
					uint128__t mask = __of1x_get_match_val128(it, true, raw_nbo);
					(void)value;	
					(void)mask;	

					ROFL_PIPELINE_INFO_NO_PREFIX("[IPV6_DST:0x%lx:%lx|0x%lx:%lx], ",UINT128__T_HI(value),UINT128__T_LO(value),UINT128__T_HI(mask),UINT128__T_LO(mask));
				}
				break;
			case OF1X_MATCH_IPV6_FLABEL:  ROFL_PIPELINE_INFO_NO_PREFIX("[IPV6_FLABEL:%lu], ",__of1x_get_match_val32(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_IPV6_ND_TARGET: {
					uint128__t value = __of1x_get_match_val128(it, false, raw_nbo);	
					uint128__t mask = __of1x_get_match_val128(it, true, raw_nbo);
					(void)value;	
					(void)mask;	

					ROFL_PIPELINE_INFO_NO_PREFIX("[IPV6_ND_TARGET:0x%lx:%lx], ",UINT128__T_HI(value),UINT128__T_LO(mask));
				}
				break;
			case OF1X_MATCH_IPV6_ND_SLL:  ROFL_PIPELINE_INFO_NO_PREFIX("[IPV6_ND_SLL:0x%"PRIx64"], ",__of1x_get_match_val64(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_IPV6_ND_TLL:  ROFL_PIPELINE_INFO_NO_PREFIX("[IPV6_ND_TLL:0x%"PRIx64"], ",__of1x_get_match_val64(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_IPV6_EXTHDR:  ROFL_PIPELINE_INFO_NO_PREFIX("[IPV6_EXTHDR:%lu|0x%lx], ",__of1x_get_match_val16(it, false, raw_nbo),__of1x_get_match_val16(it, true, raw_nbo));
				break; 
			//ICMPv6
			case OF1X_MATCH_ICMPV6_TYPE:  ROFL_PIPELINE_INFO_NO_PREFIX("[ICMPV6_TYPE:%lu], ",__of1x_get_match_val8(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_ICMPV6_CODE:  ROFL_PIPELINE_INFO_NO_PREFIX("[ICMPV6_CODE:%lu], ",__of1x_get_match_val8(it, false, raw_nbo));
				break; 
					
			//PBB	
			case OF1X_MATCH_PBB_ISID: ROFL_PIPELINE_INFO_NO_PREFIX("[PBB_ISID:0x%x|0x%x], ",__of1x_get_match_val32(it, false, raw_nbo),__of1x_get_match_val32(it, true, raw_nbo));
				break;
			//TUNNEL ID
			case OF1X_MATCH_TUNNEL_ID: ROFL_PIPELINE_INFO_NO_PREFIX("[TUNNEL_ID:0x%"PRIx64"|0x%"PRIx64"], ",__of1x_get_match_val64(it, false, raw_nbo),__of1x_get_match_val64(it, true, raw_nbo));
				break;

			/* PPP/PPPoE related extensions */
			case OF1X_MATCH_PPPOE_CODE:  ROFL_PIPELINE_INFO_NO_PREFIX("[PPPOE_CODE:%u], ",__of1x_get_match_val8(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_PPPOE_TYPE:  ROFL_PIPELINE_INFO_NO_PREFIX("[PPPOE_TYPE:%u], ",__of1x_get_match_val8(it, false, raw_nbo));
				break; 
			case OF1X_MATCH_PPPOE_SID:  ROFL_PIPELINE_INFO_NO_PREFIX("[PPPOE_SID:%u], ",__of1x_get_match_val16(it, false, raw_nbo));
				break; 

			case OF1X_MATCH_PPP_PROT:  ROFL_PIPELINE_INFO_NO_PREFIX("[PPP_PROT:%u] ",__of1x_get_match_val16(it, false, raw_nbo));
				break; 

			/* GTP related extensions */
			case OF1X_MATCH_GTP_MSG_TYPE:  ROFL_PIPELINE_INFO_NO_PREFIX("[GTP_MSG_TYPE:%u], ",__of1x_get_match_val8(it, false, raw_nbo));
				break;
			case OF1X_MATCH_GTP_TEID:  ROFL_PIPELINE_INFO_NO_PREFIX("[GTP_TEID:0x%x|0x%x], ",__of1x_get_match_val32(it, false, raw_nbo),__of1x_get_match_val32(it, true, raw_nbo));
				break;

			/* CAPWAP related extensions */
			case OF1X_MATCH_CAPWAP_WBID:  ROFL_PIPELINE_INFO_NO_PREFIX("[CAPWAP_WBID:0x%x], ",__of1x_get_match_val8(it, false, raw_nbo));
				break;
			case OF1X_MATCH_CAPWAP_RID:  ROFL_PIPELINE_INFO_NO_PREFIX("[CAPWAP_RID:0x%x], ",__of1x_get_match_val8(it, false, raw_nbo));
				break;
			case OF1X_MATCH_CAPWAP_FLAGS:  ROFL_PIPELINE_INFO_NO_PREFIX("[CAPWAP_FLAGS:%u], ",__of1x_get_match_val16(it, false, raw_nbo));
				break;

			/* WLAN related extensions */
			case OF1X_MATCH_WLAN_FC:  ROFL_PIPELINE_INFO_NO_PREFIX("[WLAN_FC:0x%x], ",__of1x_get_match_val16(it, false, raw_nbo));
				break;
			case OF1X_MATCH_WLAN_TYPE:  ROFL_PIPELINE_INFO_NO_PREFIX("[WLAN_TYPE:0x%x], ",__of1x_get_match_val8(it, false, raw_nbo));
				break;
			case OF1X_MATCH_WLAN_SUBTYPE:  ROFL_PIPELINE_INFO_NO_PREFIX("[WLAN_SUBTYPE:0x%x], ",__of1x_get_match_val8(it, false, raw_nbo));
				break;
			case OF1X_MATCH_WLAN_DIRECTION:  ROFL_PIPELINE_INFO_NO_PREFIX("[WLAN_DIRECTION:0x%x], ",__of1x_get_match_val8(it, false, raw_nbo));
				break;
			case OF1X_MATCH_WLAN_ADDRESS_1:  ROFL_PIPELINE_INFO_NO_PREFIX("[WLAN_ADDRESS_1:0x%"PRIx64"], ",__of1x_get_match_val64(it, false, raw_nbo));
				break;
			case OF1X_MATCH_WLAN_ADDRESS_2:  ROFL_PIPELINE_INFO_NO_PREFIX("[WLAN_ADDRESS_2:0x%"PRIx64"], ",__of1x_get_match_val64(it, false, raw_nbo));
				break;
			case OF1X_MATCH_WLAN_ADDRESS_3:  ROFL_PIPELINE_INFO_NO_PREFIX("[WLAN_ADDRESS_3:0x%"PRIx64"], ",__of1x_get_match_val64(it, false, raw_nbo));
				break;

			case OF1X_MATCH_MAX: assert(0);
				break;

			//Add more here ...
			//Warning: NEVER add a default clause
		}
	}	
}

