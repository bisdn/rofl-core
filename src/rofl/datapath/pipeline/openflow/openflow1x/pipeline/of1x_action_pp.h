/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_ACTION_PP_H__
#define __OF1X_ACTION_PP_H__

#include <inttypes.h> 
#include <string.h> 
#include <stdbool.h>
#include <assert.h>
#include "rofl.h"
#include "../../../util/pp_guard.h" //Never forget to include the guard
#include "of1x_action.h"
#include "of1x_group_table.h"
#include "of1x_flow_table.h"
#include "of1x_utils.h"
#include "../../../physical_switch.h"
#include "../../../util/logging.h"

#include "../../../common/protocol_constants.h"
#include "../../../common/ternary_fields.h"
#include "../../../common/bitmap.h"

#include "../../../platform/lock.h"
#include "../../../platform/likely.h"
#include "../../../platform/memory.h"
#include "../../../platform/packet.h"

#include "../of1x_async_events_hooks.h"

/**
* @file of1x_action_pp.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief OpenFlow v1.0, 1.2 and 1.3.2 action packet processing routines
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
*
*/

//C++ extern C
ROFL_BEGIN_DECLS

//Initialize (clear) pkt write actions
static inline void __of1x_init_packet_write_actions(of1x_write_actions_t* pkt_write_actions){
	bitmap128_clean(&pkt_write_actions->bitmap);
	pkt_write_actions->num_of_actions = 0;
}

//Update pkt write actions
static inline void __of1x_update_packet_write_actions(of1x_write_actions_t* packet_write_actions, const of1x_write_actions_t* entry_write_actions){
	
	unsigned int i,j;

	for(i=0,j=0;i<entry_write_actions->num_of_actions && j < OF1X_AT_NUMBER;j++){
		if(!bitmap128_is_bit_set(&entry_write_actions->bitmap,j))
			continue;
		packet_write_actions->actions[j].__field = entry_write_actions->actions[j].__field;
		packet_write_actions->actions[j].group = entry_write_actions->actions[j].group;
		packet_write_actions->actions[j].type = entry_write_actions->actions[j].type;
		
		if(!bitmap128_is_bit_set(&packet_write_actions->bitmap,j)){
			packet_write_actions->num_of_actions++;
			bitmap128_set(&packet_write_actions->bitmap,j);
		}
		i++;
	}
}

//Clear packet write actions
static inline void __of1x_clear_write_actions(of1x_write_actions_t* pkt_write_actions){
	bitmap128_clean(&pkt_write_actions->bitmap);
	pkt_write_actions->num_of_actions = 0;
}

//fwd decl
static inline void __of1x_process_apply_actions(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t* pkt, const of1x_action_group_t* apply_actions_group, bool replicate_pkts);

//Process all actions from a group
static inline void __of1x_process_group_actions(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t *pkt,uint64_t field, of1x_group_t *group, bool replicate_pkts){
	datapacket_t* pkt_replica;
	of1x_bucket_t *it_bk;
	
	platform_rwlock_rdlock(group->rwlock);
	
	//process the actions in the buckets depending on the type
	switch(group->type){
		case OF1X_GROUP_TYPE_ALL:
			//executes all buckets
			for (it_bk = group->bc_list->head; it_bk!=NULL;it_bk = it_bk->next){

				//If there are no output actions, skip bucket 
				if(it_bk->actions->num_of_output_actions == 0)
					continue;

				//Clone the packet according to spec before applying the bucket
				//action list
				pkt_replica = platform_packet_replicate(pkt);
				if(unlikely(pkt_replica == NULL)){
					assert(0);
					break;
				} 
				
				//Process all actions in the bucket
				__of1x_process_apply_actions(sw,table_id, pkt_replica, it_bk->actions, it_bk->actions->num_of_output_actions > 1); //No replica
				__of1x_stats_bucket_update(&it_bk->stats, platform_packet_get_size_bytes(pkt));
				
				if(it_bk->actions->num_of_output_actions > 1)
					platform_packet_drop(pkt_replica);
			}
			break;
		case OF1X_GROUP_TYPE_SELECT:
			//NOT SUPPORTED	
			assert(0);  //Should NEVER be installed
			break;
		case OF1X_GROUP_TYPE_INDIRECT:
			//executes the "one bucket defined"
			__of1x_process_apply_actions(sw,table_id,pkt,group->bc_list->head->actions, replicate_pkts);
			__of1x_stats_bucket_update(&group->bc_list->head->stats, platform_packet_get_size_bytes(pkt));
			break;
		case OF1X_GROUP_TYPE_FF:
			//NOT SUPPORTED
			assert(0);  //Should NEVER be installed
			break;
		default:
			assert(0);  //Should NEVER be reached 
			break;
	}
	__of1x_stats_group_update(&group->stats, platform_packet_get_size_bytes(pkt));
	platform_rwlock_rdunlock(group->rwlock);
}

/* Contains switch with all the different action functions */
static inline void __of1x_process_packet_action(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t* pkt, of1x_packet_action_t* action, bool replicate_pkts){

	uint32_t port_id;

	switch(action->type){
		case OF1X_AT_NO_ACTION: assert(0);
			break;

		case OF1X_AT_COPY_TTL_IN: platform_packet_copy_ttl_in(pkt);
			break;

		//POP
		case OF1X_AT_POP_VLAN: 
			//Call platform
			platform_packet_pop_vlan(pkt);
			break;
		case OF1X_AT_POP_MPLS: 
			//Call platform
			platform_packet_pop_mpls(pkt, action->__field.u16);
			break;
	
		//PUSH
		case OF1X_AT_PUSH_MPLS:
			//Call platform
			platform_packet_push_mpls(pkt, action->__field.u16);
			break;
		case OF1X_AT_PUSH_VLAN:
			//Call platform
			platform_packet_push_vlan(pkt, action->__field.u16);
			break;

		//TTL
		case OF1X_AT_COPY_TTL_OUT:
			//Call platform
			platform_packet_copy_ttl_out(pkt);
			break;
		case OF1X_AT_DEC_NW_TTL:
			//Call platform
			platform_packet_dec_nw_ttl(pkt);
			break;
		case OF1X_AT_DEC_MPLS_TTL:
			//Call platform
			platform_packet_dec_mpls_ttl(pkt);
			break;
		case OF1X_AT_SET_MPLS_TTL:
			//Call platform
			platform_packet_set_mpls_ttl(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_NW_TTL:
			//Call platform
			platform_packet_set_nw_ttl(pkt, action->__field.u8);
			break;

		//QUEUE
		case OF1X_AT_SET_QUEUE:
			//Call platform
			platform_packet_set_queue(pkt, action->__field.u32);
			break;

		//802
		case OF1X_AT_SET_FIELD_ETH_DST: 
			//Call platform
			platform_packet_set_eth_dst(pkt, action->__field.u64);
			break;
		case OF1X_AT_SET_FIELD_ETH_SRC: 
			//Call platform
			platform_packet_set_eth_src(pkt, action->__field.u64); 
			break;
		case OF1X_AT_SET_FIELD_ETH_TYPE: 
			//Call platform
			platform_packet_set_eth_type(pkt, action->__field.u16);
			break;

		//802.1q
		case OF1X_AT_SET_FIELD_VLAN_VID: 
			//For 1.0 we must first push it if we don't have. wtf...
			if(sw->of_ver == OF_VERSION_10 && !platform_packet_has_vlan(pkt)){
				//Push VLAN
				platform_packet_push_vlan(pkt, ETH_TYPE_8021Q);
				platform_packet_set_vlan_pcp(pkt, 0x0);
			}
			//Call platform
			platform_packet_set_vlan_vid(pkt, action->__field.u16);
			break;
		case OF1X_AT_SET_FIELD_VLAN_PCP: 
			//For 1.0 we must first push it if we don't have. wtf...
			if(sw->of_ver == OF_VERSION_10 && !platform_packet_has_vlan(pkt)){
				//Push VLAN
				platform_packet_push_vlan(pkt, ETH_TYPE_8021Q);
				platform_packet_set_vlan_vid(pkt, 0x0);
			}
			//Call platform
			platform_packet_set_vlan_pcp(pkt, action->__field.u8);
			break;

		//ARP
		case OF1X_AT_SET_FIELD_ARP_OPCODE:
			//Call plattform
			platform_packet_set_arp_opcode(pkt, action->__field.u16);
			break;
		case OF1X_AT_SET_FIELD_ARP_SHA:
			//Call platform
			platform_packet_set_arp_sha(pkt, action->__field.u64);
			break;
		case OF1X_AT_SET_FIELD_ARP_SPA:
			//Call platform
			platform_packet_set_arp_spa(pkt, action->__field.u32);
			break;
		case OF1X_AT_SET_FIELD_ARP_THA:
			//Call platform
			platform_packet_set_arp_tha(pkt, action->__field.u64);
			break;
		case OF1X_AT_SET_FIELD_ARP_TPA:
			//Call platform
			platform_packet_set_arp_tpa(pkt, action->__field.u32);
			break;

		//NW
		case OF1X_AT_SET_FIELD_NW_PROTO:
			if((*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV4)){
				//Call platform
				platform_packet_set_ip_proto(pkt, action->__field.u8);
			}else if((*platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP)){
				//Call plattform
				platform_packet_set_arp_opcode(pkt, action->__field.u8);
			}

			break;
		case OF1X_AT_SET_FIELD_NW_SRC:
			if((*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV4)){
				//Call platform
				platform_packet_set_ipv4_src(pkt, action->__field.u32);
			}else if((*platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP)){
				//Call platform
				platform_packet_set_arp_spa(pkt, action->__field.u32);
			}
			break;
		case OF1X_AT_SET_FIELD_NW_DST:
			if((*platform_packet_get_eth_type(pkt) == ETH_TYPE_IPV4)){
				//Call platform
				platform_packet_set_ipv4_dst(pkt, action->__field.u32);
			}else if((*platform_packet_get_eth_type(pkt) == ETH_TYPE_ARP)){
				//Call platform
				platform_packet_set_arp_tpa(pkt, action->__field.u32);
			}
			break;

		//IP
		case OF1X_AT_SET_FIELD_IP_DSCP:
			//Call platform
			platform_packet_set_ip_dscp(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_IP_ECN:
			//Call platform
			platform_packet_set_ip_ecn(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_IP_PROTO:
			//Call platform
			platform_packet_set_ip_proto(pkt, action->__field.u8);
			break;

		//IPv4
		case OF1X_AT_SET_FIELD_IPV4_SRC:
			//Call platform
			platform_packet_set_ipv4_src(pkt, action->__field.u32);
			break;
		case OF1X_AT_SET_FIELD_IPV4_DST:
			//Call platform
			platform_packet_set_ipv4_dst(pkt, action->__field.u32);
			break;

		//TP
		case OF1X_AT_SET_FIELD_TP_SRC:  
			if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_TCP)){
				//Call platform
				platform_packet_set_tcp_src(pkt, action->__field.u16);
			}else if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_UDP)){
				//Call platform
				platform_packet_set_udp_src(pkt, action->__field.u16);
			}else if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_ICMPV4)){
				//Call platform
				platform_packet_set_icmpv4_type(pkt, action->__field.u8);
			}
			break;
		case OF1X_AT_SET_FIELD_TP_DST:
			if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_TCP)){
				//Call platform
				platform_packet_set_tcp_dst(pkt, action->__field.u16);
			}else if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_UDP)){
				//Call platform
				platform_packet_set_udp_dst(pkt, action->__field.u16);
			}else if((*platform_packet_get_ip_proto(pkt) == IP_PROTO_ICMPV4)){
				//Call platform
				platform_packet_set_icmpv4_code(pkt, action->__field.u8);
			}
			break;

		//TCP
		case OF1X_AT_SET_FIELD_TCP_SRC:  
			//Call platform
			platform_packet_set_tcp_src(pkt, action->__field.u16);
			break;
		case OF1X_AT_SET_FIELD_TCP_DST:
			//Call platform
			platform_packet_set_tcp_dst(pkt, action->__field.u16);
			break;

		//UDP
		case OF1X_AT_SET_FIELD_UDP_SRC:
			//Call platform
			platform_packet_set_udp_src(pkt, action->__field.u16);
			break;
		case OF1X_AT_SET_FIELD_UDP_DST:
			//Call platform
			platform_packet_set_udp_dst(pkt, action->__field.u16);
			break;
		//SCTP
		case OF1X_AT_SET_FIELD_SCTP_SRC:
			//Call platform
			platform_packet_set_sctp_src(pkt, action->__field.u16);
			break;
		case OF1X_AT_SET_FIELD_SCTP_DST:
			//Call platform
			platform_packet_set_sctp_dst(pkt, action->__field.u16);
			break;


		//ICMPv4
		case OF1X_AT_SET_FIELD_ICMPV4_TYPE:
			//Call platform
			platform_packet_set_icmpv4_type(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_ICMPV4_CODE:
			//Call platform
			platform_packet_set_icmpv4_code(pkt, action->__field.u8);
			break;

		//MPLS
		case OF1X_AT_SET_FIELD_MPLS_LABEL:
			//Call platform
			platform_packet_set_mpls_label(pkt, action->__field.u32);
			break;
		case OF1X_AT_SET_FIELD_MPLS_TC:
			//Call platform
			platform_packet_set_mpls_tc(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_MPLS_BOS:
			//Call platform
			platform_packet_set_mpls_bos(pkt, action->__field.u8);
			break;


		//IPv6
		case OF1X_AT_SET_FIELD_IPV6_SRC:
			//Call platform
			platform_packet_set_ipv6_src(pkt, action->__field.u128);
			break;
		case OF1X_AT_SET_FIELD_IPV6_DST:
			//Call platform
			platform_packet_set_ipv6_dst(pkt, action->__field.u128);
			break;
		case OF1X_AT_SET_FIELD_IPV6_FLABEL:
			//Call platform
			platform_packet_set_ipv6_flabel(pkt, action->__field.u32);
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_TARGET:
			//Call platform
			platform_packet_set_ipv6_nd_target(pkt, action->__field.u128);
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_SLL:
			//Call platform
			platform_packet_set_ipv6_nd_sll(pkt, action->__field.u64);
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_TLL:
			//Call platform
			platform_packet_set_ipv6_nd_tll(pkt, action->__field.u64);
			break;
		case OF1X_AT_SET_FIELD_IPV6_EXTHDR:
			//Call platform
			platform_packet_set_ipv6_exthdr(pkt, action->__field.u16);
			break;
		//ICMPv6
		case OF1X_AT_SET_FIELD_ICMPV6_TYPE:
			//Call platform
			platform_packet_set_icmpv6_type(pkt, action->__field.u8);
			break;
			
		case OF1X_AT_SET_FIELD_ICMPV6_CODE:
			//Call platform
			platform_packet_set_icmpv6_code(pkt, action->__field.u8);
			break;

#ifdef EXPERIMENTAL

		case OF1X_AT_POP_PPPOE:
			//Call platform
			platform_packet_pop_pppoe(pkt, action->__field.u16);
			break;
		case OF1X_AT_PUSH_PPPOE:
			//Call platform
			platform_packet_push_pppoe(pkt, action->__field.u16);
			break;

		//PPPoE
		case OF1X_AT_SET_FIELD_PPPOE_CODE:
			//Call platform
			platform_packet_set_pppoe_code(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_PPPOE_TYPE:
			//Call platform
			platform_packet_set_pppoe_type(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_PPPOE_SID:
			//Call platform
			platform_packet_set_pppoe_sid(pkt, action->__field.u16);
			break;

		//PPP
		case OF1X_AT_SET_FIELD_PPP_PROT:
			//Call platform
			platform_packet_set_ppp_proto(pkt, action->__field.u16);
			break;

		//GTP
		case OF1X_AT_SET_FIELD_GTP_MSG_TYPE:
			//Call platform
			platform_packet_set_gtp_msg_type(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_GTP_TEID:
			//Call platform
			platform_packet_set_gtp_teid(pkt, action->__field.u32);
			break;
		case OF1X_AT_POP_GTP: 
			//Call platform
			platform_packet_pop_gtp(pkt);
			break;
		case OF1X_AT_PUSH_GTP: 
			//Call platform
			platform_packet_push_gtp(pkt);
			break;

		//CAPWAP
		case OF1X_AT_SET_FIELD_CAPWAP_WBID:
			//Call platform
			platform_packet_set_capwap_wbid(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_CAPWAP_RID:
			//Call platform
			platform_packet_set_capwap_rid(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_CAPWAP_FLAGS:
			//Call platform
			platform_packet_set_capwap_flags(pkt, action->__field.u16);
			break;
		case OF1X_AT_POP_CAPWAP:
			//Call platform
			platform_packet_pop_capwap(pkt);
			break;
		case OF1X_AT_PUSH_CAPWAP:
			//Call platform
			platform_packet_push_capwap(pkt);
			break;

		//IEEE80211 WLAN
		case OF1X_AT_SET_FIELD_WLAN_FC:
			//Call platform
			platform_packet_set_wlan_fc(pkt, action->__field.u16);
			break;
		case OF1X_AT_SET_FIELD_WLAN_TYPE:
			//Call platform
			platform_packet_set_wlan_type(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_WLAN_SUBTYPE:
			//Call platform
			platform_packet_set_wlan_subtype(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_WLAN_DIRECTION:
			//Call platform
			platform_packet_set_wlan_direction(pkt, action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_1:
			//Call platform
			platform_packet_set_wlan_address_1(pkt, action->__field.u64);
			break;
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_2:
			//Call platform
			platform_packet_set_wlan_address_2(pkt, action->__field.u64);
			break;
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_3:
			//Call platform
			platform_packet_set_wlan_address_3(pkt, action->__field.u64);
			break;
		case OF1X_AT_POP_WLAN:
			//Call platform
			platform_packet_pop_wlan(pkt);
			break;
		case OF1X_AT_PUSH_WLAN:
			//Call platform
			platform_packet_push_wlan(pkt);
			break;

#else
		case OF1X_AT_POP_PPPOE:
		case OF1X_AT_PUSH_PPPOE:
		case OF1X_AT_SET_FIELD_PPPOE_CODE:
		case OF1X_AT_SET_FIELD_PPPOE_TYPE:
		case OF1X_AT_SET_FIELD_PPPOE_SID:
		case OF1X_AT_SET_FIELD_PPP_PROT:
		case OF1X_AT_SET_FIELD_GTP_MSG_TYPE:
		case OF1X_AT_SET_FIELD_GTP_TEID:
		case OF1X_AT_POP_GTP:
		case OF1X_AT_PUSH_GTP:
		case OF1X_AT_SET_FIELD_CAPWAP_WBID:
		case OF1X_AT_SET_FIELD_CAPWAP_RID:
		case OF1X_AT_SET_FIELD_CAPWAP_FLAGS:
		case OF1X_AT_POP_CAPWAP:
		case OF1X_AT_PUSH_CAPWAP:
		case OF1X_AT_SET_FIELD_WLAN_FC:
		case OF1X_AT_SET_FIELD_WLAN_TYPE:
		case OF1X_AT_SET_FIELD_WLAN_SUBTYPE:
		case OF1X_AT_SET_FIELD_WLAN_DIRECTION:
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_1:
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_2:
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_3:
		case OF1X_AT_POP_WLAN:
		case OF1X_AT_PUSH_WLAN:
			break;
#endif
		//PBB
		case OF1X_AT_SET_FIELD_PBB_ISID:
			//Call platform
			platform_packet_set_pbb_isid(pkt, action->__field.u32);
			break;
		case OF1X_AT_POP_PBB: 
			//Call platform
			platform_packet_pop_pbb(pkt);
			break;
		case OF1X_AT_PUSH_PBB: 
			//Call platform
			platform_packet_push_pbb(pkt, action->__field.u16);
			break;

		//TUNNEL ID
		case OF1X_AT_SET_FIELD_TUNNEL_ID:
			//Call platform
			platform_packet_set_tunnel_id(pkt, action->__field.u64);
			break;

		case OF1X_AT_GROUP:
			__of1x_process_group_actions(sw, table_id, pkt, action->__field.u32, action->group, replicate_pkts);
			break;

		case OF1X_AT_EXPERIMENTER: //FIXME: implement
			break;

		case OF1X_AT_OUTPUT: 

			//Store in automatic
			port_id = action->__field.u32;
	
			if( port_id < OF1X_PORT_MAX ||
				port_id == OF1X_PORT_IN_PORT ||
				port_id == OF1X_PORT_ALL ||
				port_id == OF1X_PORT_FLOOD ||
				port_id == OF1X_PORT_NORMAL ||
				port_id == OF1X_PORT_CONTROLLER){

				//Pointer for the packet to be sent
				datapacket_t* pkt_to_send;			
	
				//Duplicate the packet only if necessary
				if(replicate_pkts){
					pkt_to_send = platform_packet_replicate(pkt);
				
					//check for wrong copy
					if(unlikely(pkt_to_send == NULL)){
						ROFL_PIPELINE_INFO("Packet[%p] could NOT be cloned during OUTPUT action\n", pkt);
						return;
					}
					ROFL_PIPELINE_INFO("Packet[%p] was cloned into [%p] during OUTPUT action\n", pkt, pkt_to_send);
					
				}else
					pkt_to_send = pkt;

				//Perform output
				if( port_id < LOGICAL_SWITCH_MAX_LOG_PORTS && unlikely(NULL != sw->logical_ports[port_id].port) ){

					//Single port output
					//According to the spec a packet cannot be sent to the incoming port
					//unless IN_PORT meta port is used
					if(unlikely(port_id == *platform_packet_get_port_in(pkt))){
						ROFL_PIPELINE_DEBUG("Packet[%p] dropped. Attempting to output to the incoming port %u\n", pkt_to_send, port_id);
						platform_packet_drop(pkt_to_send);
					}else{
						ROFL_PIPELINE_INFO("Packet[%p] outputting to port num. %u\n", pkt_to_send, port_id);
						platform_packet_output(pkt_to_send, sw->logical_ports[port_id].port);
					}

				}else if(port_id == OF1X_PORT_FLOOD){
					//Flood
					ROFL_PIPELINE_INFO("Packet[%p] outputting to FLOOD\n", pkt_to_send);
					platform_packet_output(pkt_to_send, flood_meta_port);
				}else if(port_id == OF1X_PORT_CONTROLLER ||
					port_id == OF1X_PORT_NORMAL){
					//Controller
					ROFL_PIPELINE_INFO("Packet[%p] outputting to CONTROLLER\n", pkt_to_send);
					platform_of1x_packet_in(sw, table_id, pkt_to_send, action->send_len, OF1X_PKT_IN_ACTION);
				}else if(port_id == OF1X_PORT_ALL){
					//All
					ROFL_PIPELINE_INFO("Packet[%p] outputting to ALL_PORT\n", pkt_to_send);
					platform_packet_output(pkt_to_send, all_meta_port);
				}else if(port_id == OF1X_PORT_IN_PORT){
					//in port
					ROFL_PIPELINE_INFO("Packet[%p] outputting to IN_PORT\n", pkt_to_send);
					platform_packet_output(pkt_to_send, in_port_meta_port);
				}else{

					//This condition can never happen, unless port number has been somehow corrupted??
					ROFL_PIPELINE_INFO("Packet[%p] WARNING output to UNKNOWN port %u\n", pkt_to_send, port_id);
					//assert(0);
					if(pkt != pkt_to_send) //Drop replica, if any
						platform_packet_drop(pkt_to_send);
				}
			}
			break;
	}
}



//Process apply actions
static inline void __of1x_process_apply_actions(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t* pkt, const of1x_action_group_t* apply_actions_group, bool replicate_pkts){

	of1x_packet_action_t* it;

	for(it=apply_actions_group->head;it;it=it->next){
		__of1x_process_packet_action(sw, table_id, pkt, it, replicate_pkts);
	}	
}

/*
* The of1x_process_write_actions is meant to encapsulate the processing of the write actions
*
*/
static inline void __of1x_process_write_actions(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t* pkt, bool replicate_pkts){

	unsigned int i,j;

	of1x_write_actions_t* write_actions = &pkt->write_actions.of1x;	

	for(i=0,j=0;(i<write_actions->num_of_actions) && (j < OF1X_AT_NUMBER);j++){
		if( bitmap128_is_bit_set(&write_actions->bitmap,j) ){
			//Process action
			__of1x_process_packet_action(sw, table_id, pkt, &write_actions->actions[j], replicate_pkts);	
			i++;		
		}
	}
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_ACTION_PP
