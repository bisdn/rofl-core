#include "of1x_action.h"
#include "../../../common/datapacket.h"

#include <stdio.h>
#include <assert.h>

#include "of1x_packet_matches.h" //TODO: evaluate if this is the best approach to update of1x_matches after actions
#include "../../../physical_switch.h"
#include "../../../platform/packet.h"
#include "../../../util/logging.h"
#include "../../../platform/memory.h"
#include "../of1x_async_events_hooks.h"
#include "of1x_utils.h"

//Flood port
extern switch_port_t* flood_meta_port;

//fwd declarations
static void __of1x_process_group_actions(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t *pkt,uint64_t field, of1x_group_t* group, bool replicate_pkts);

/* Actions init and destroyed */
of1x_packet_action_t* of1x_init_packet_action(/*const struct of1x_switch* sw, */of1x_packet_action_type_t type, wrap_uint_t field, of1x_packet_action_t* prev, of1x_packet_action_t* next){

	of1x_packet_action_t* action;

	if(!type)
		return NULL;

	action = platform_malloc_shared(sizeof(of1x_packet_action_t));

	if(!action)
		return NULL;
	
	//Set type
	action->type = type;

	//Set min max 
	action->ver_req.min_ver = OF1X_MIN_VERSION;
	action->ver_req.max_ver = OF1X_MAX_VERSION;

	/*
	* Setting the field (for set_field actions) and fast validation flags
	*/
	switch(type){
		//16 byte
		case OF1X_AT_SET_FIELD_IPV6_ND_TARGET:
		case OF1X_AT_SET_FIELD_IPV6_SRC:
		case OF1X_AT_SET_FIELD_IPV6_DST:
			action->field.u128 = field.u128;
			action->ver_req.min_ver = OF_VERSION_12;
			break;

		//8 byte values (TODO: METADATA)
		case OF1X_AT_SET_FIELD_TUNNEL_ID:
			action->field.u64 = field.u64&OF1X_8_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_13;
			break;

		//6 byte values
		case OF1X_AT_SET_FIELD_IPV6_ND_SLL:
			action->field.u64 = field.u64&OF1X_6_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_TLL:
			action->field.u64 = field.u64&OF1X_6_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_ETH_DST:
			action->field.u64 = field.u64&OF1X_6_BYTE_MASK;
			break;
		case OF1X_AT_SET_FIELD_ETH_SRC:
			action->field.u64 = field.u64&OF1X_6_BYTE_MASK;
			break;
		case OF1X_AT_SET_FIELD_ARP_SHA:
			action->field.u64 = field.u64&OF1X_6_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_ARP_THA:
			action->field.u64 = field.u64&OF1X_6_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
	
		//4 byte values
		case OF1X_AT_SET_FIELD_IPV4_DST:
			action->field.u64 = field.u64&OF1X_4_BYTE_MASK;
			break;
		case OF1X_AT_SET_FIELD_IPV4_SRC:
			action->field.u64 = field.u64&OF1X_4_BYTE_MASK;
			break;
		case OF1X_AT_SET_FIELD_ARP_SPA:
			action->field.u64 = field.u64&OF1X_4_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_ARP_TPA:
			action->field.u64 = field.u64&OF1X_4_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_OUTPUT:
			action->field.u64 = field.u64&OF1X_4_BYTE_MASK;	// TODO: max_len when port_no == OFPP_CONTROLLER
			break;
		case OF1X_AT_SET_FIELD_GTP_TEID:
			action->field.u64 = field.u64&OF1X_4_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;

		//3 byte
		case OF1X_AT_SET_FIELD_PBB_ISID:
			action->field.u64 = field.u64&OF1X_3_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_13;
			break;
	
		//20 bit values
		case OF1X_AT_SET_FIELD_IPV6_FLABEL:
			action->field.u64 = field.u64&OF1X_20_BITS_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_MPLS_LABEL:
			action->field.u64 = field.u64&OF1X_20_BITS_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;

		//2 byte values
		case OF1X_AT_SET_FIELD_ETH_TYPE:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			break;
		case OF1X_AT_SET_FIELD_ARP_OPCODE:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_TP_SRC:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_10;
			action->ver_req.max_ver = OF_VERSION_10;
			break;
		case OF1X_AT_SET_FIELD_TP_DST:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_10;
			action->ver_req.max_ver = OF_VERSION_10;
			break;
		case OF1X_AT_SET_FIELD_TCP_SRC:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_TCP_DST:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_UDP_SRC:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_UDP_DST:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_SCTP_SRC:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_SCTP_DST:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_PPPOE_SID:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_PPP_PROT:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		/*case OF1X_AT_POP_VLAN: TODO: CHECK THIS*/
		case OF1X_AT_POP_MPLS: 
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_POP_PPPOE: 
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_POP_PBB: 
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_13;
			break;

		case OF1X_AT_PUSH_PPPOE:
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_PUSH_MPLS: 
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_PUSH_VLAN: 
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_PUSH_PBB: 
			action->field.u64 = field.u64&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_13;
			break;


		//13 bit values
		case OF1X_AT_SET_FIELD_VLAN_VID:
			action->field.u64 = field.u64&OF1X_13_BITS_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;


		//9 bit value
		case OF1X_AT_SET_FIELD_IPV6_EXTHDR:
			action->field.u64 = field.u64&OF1X_9_BITS_MASK;
			action->ver_req.min_ver = OF_VERSION_13;
			break;

		//1 byte values
		case OF1X_AT_SET_FIELD_ICMPV6_TYPE:
		case OF1X_AT_SET_FIELD_ICMPV6_CODE:
		case OF1X_AT_SET_FIELD_PPPOE_CODE:
		case OF1X_AT_SET_FIELD_PPPOE_TYPE:
		case OF1X_AT_SET_MPLS_TTL:
		case OF1X_AT_SET_NW_TTL:
		case OF1X_AT_SET_FIELD_IP_PROTO:
		case OF1X_AT_SET_FIELD_ICMPV4_TYPE:
		case OF1X_AT_SET_FIELD_ICMPV4_CODE:
		case OF1X_AT_SET_FIELD_GTP_MSG_TYPE:
		case OF1X_AT_SET_QUEUE:
			action->field.u64 = field.u64&OF1X_1_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;

		//6 bit values
		case OF1X_AT_SET_FIELD_IP_DSCP:
			action->field.u64 = field.u64&OF1X_6_BITS_MASK;
			break;

		//3 bit values
		case OF1X_AT_SET_FIELD_VLAN_PCP:
		case OF1X_AT_SET_FIELD_MPLS_TC:
			action->field.u64 = field.u64&OF1X_3_BITS_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;

		//2 bit values
		case OF1X_AT_SET_FIELD_IP_ECN:
			action->field.u64 = field.u64&OF1X_2_BITS_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_GROUP:
			action->field.u64 =  field.u64&OF1X_4_BYTE_MASK; //id of the group
			action->ver_req.min_ver = OF_VERSION_12;
			//action->group = of1x_group_search(sw->pipeline->groups, action->field); // pointer to the group //FIXME evaluate if this can be done here or not
			break;

		//1 bit values
		case OF1X_AT_SET_FIELD_MPLS_BOS:
			action->field.u64 =  field.u64&OF1X_1_BIT_MASK; //id of the group
			action->ver_req.min_ver = OF_VERSION_13;
			break;

		//No value
		case OF1X_AT_COPY_TTL_IN:
		case OF1X_AT_COPY_TTL_OUT:
		case OF1X_AT_DEC_NW_TTL:
		case OF1X_AT_DEC_MPLS_TTL:
		case OF1X_AT_POP_VLAN:
		case OF1X_AT_POP_GTP:
		case OF1X_AT_PUSH_GTP:
		case OF1X_AT_EXPERIMENTER:
			action->field.u64 = 0x0;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		
		//Shall never happen
		case OF1X_AT_NO_ACTION: 
			assert(0);
			action->field.u64 = 0x0;
			break;
	}
	
	//Set list pointers
	action->next = next;
	action->prev = prev;
	
	return action;
}

void of1x_destroy_packet_action(of1x_packet_action_t* action){

	platform_free_shared(action);
}

/* Action group init and destroy */
of1x_action_group_t* of1x_init_action_group(of1x_packet_action_t* actions){

	unsigned int number_of_actions=0, number_of_output_actions=0;
	of1x_action_group_t* action_group;
	
	action_group = platform_malloc_shared(sizeof(of1x_action_group_t));

	if(!action_group)
		return NULL;
	
	if(actions){
		action_group->head = actions;
	
		for(;actions;actions=actions->next, number_of_actions++){

			if(actions->type == OF1X_AT_OUTPUT /*|| actions->type == OF1X_AT_GROUP*/)
				number_of_output_actions++;

			if(!actions->next){
				action_group->tail = actions;
				break;
			}	
		}
	}else{
		action_group->head = NULL;
		action_group->tail = NULL;
	}

	action_group->num_of_actions = number_of_actions;
	action_group->num_of_output_actions = number_of_output_actions;

	//Fast validation, set min max 
	action_group->ver_req.min_ver = OF1X_MIN_VERSION;
	action_group->ver_req.max_ver = OF1X_MAX_VERSION;

	return action_group;
}

void of1x_destroy_action_group(of1x_action_group_t* group){

	of1x_packet_action_t* it,*next;

	if(!group)
		return;

	for(it=group->head;it;it=next){
		next = it->next; 
		of1x_destroy_packet_action(it);
	}
	platform_free_shared(group);	
}

/* Addition of an action to an action group */
void of1x_push_packet_action_to_group(of1x_action_group_t* group, of1x_packet_action_t* action){

	if(!action){
		assert(0);
		return;
	}

	if(!group->tail){
		group->head = action; 
		action->prev = NULL;
	}else{
		action->prev = group->tail;
		group->tail->next = action;
	}		

	group->tail = action;
	action->next = NULL;
	
	group->num_of_actions++;

	if(action->type == OF1X_AT_OUTPUT)
		group->num_of_output_actions++;
	
	if(action->type == OF1X_AT_GROUP)
		group->num_of_output_actions+=action->group->num_of_output_actions;

	//Update fast validation flags (required versions)
	if(group->ver_req.min_ver < action->ver_req.min_ver)
		group->ver_req.min_ver = action->ver_req.min_ver;
	if(group->ver_req.max_ver > action->ver_req.max_ver)
		group->ver_req.max_ver = action->ver_req.max_ver;

}

/* Write actions init */
void __of1x_init_packet_write_actions(datapacket_t *const pkt){
       memset(&pkt->write_actions.of1x, 0, sizeof(of1x_write_actions_t));
}

of1x_write_actions_t* of1x_init_write_actions(){

	of1x_write_actions_t* write_actions = platform_malloc_shared(sizeof(of1x_write_actions_t)); 

	if(!write_actions)
		return NULL;

	memset(write_actions, 0, sizeof(of1x_write_actions_t));

	//Fast validation, set min max 
	write_actions->ver_req.min_ver = OF1X_MIN_VERSION;
	write_actions->ver_req.max_ver = OF1X_MAX_VERSION;

	return write_actions;
}

void __of1x_destroy_write_actions(of1x_write_actions_t* write_actions){
	platform_free_shared(write_actions);	
}

void of1x_set_packet_action_on_write_actions(of1x_write_actions_t* write_actions, of1x_packet_action_t* action){
	if( write_actions && action )	
		write_actions->write_actions[action->type] = *action;
	
	if (action->type == OF1X_AT_OUTPUT)
		write_actions->num_of_output_actions++;

	if(action->type == OF1X_AT_GROUP)
		write_actions->num_of_output_actions+=action->group->num_of_output_actions;
	
	//Update fast validation flags (required versions)
	if(write_actions->ver_req.min_ver < action->ver_req.min_ver)
		write_actions->ver_req.min_ver = action->ver_req.min_ver;
	if(write_actions->ver_req.max_ver > action->ver_req.max_ver)
		write_actions->ver_req.max_ver = action->ver_req.max_ver;


}

//Update of write actions
void __of1x_update_packet_write_actions(datapacket_t* pkt, const of1x_write_actions_t* entry_write_actions) {

	unsigned int i;
	of1x_write_actions_t* packet_write_actions;

	//Recover write actions from datapacket
	packet_write_actions = &pkt->write_actions.of1x;
	
	if(!entry_write_actions)
		return;

	//Loop over entry write actions and update packet_write_actions
	for(i=0;i<OF1X_AT_NUMBER;i++){
		if(entry_write_actions->write_actions[i].type)
			packet_write_actions->write_actions[i] = entry_write_actions->write_actions[i];
	}

}

//Clear actions
void __of1x_clear_write_actions(datapacket_t* pkt){
	memset(&pkt->write_actions.of1x, 0, sizeof(of1x_write_actions_t));
}

/* Contains switch with all the different action functions */
static inline void __of1x_process_packet_action(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t* pkt, of1x_packet_action_t* action, bool replicate_pkts){

	of1x_packet_matches_t* pkt_matches = &pkt->matches.of1x;

	switch(action->type){
		case OF1X_AT_NO_ACTION: assert(0);
			break;

		case OF1X_AT_COPY_TTL_IN: platform_packet_copy_ttl_in(pkt);
			break;

		//POP
		case OF1X_AT_POP_VLAN: 
			//Call platform
			platform_packet_pop_vlan(pkt);
			//Update match
			pkt_matches->eth_type= platform_packet_get_eth_type(pkt); 
			pkt_matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt); 
			break;
		case OF1X_AT_POP_MPLS: 
			//Call platform
			platform_packet_pop_mpls(pkt, action->field.u64);
			//Update match
			pkt_matches->eth_type= platform_packet_get_eth_type(pkt); 
			pkt_matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt); 
			break;
		case OF1X_AT_POP_PPPOE: 
			//Call platform
			platform_packet_pop_pppoe(pkt, action->field.u64);
			//Update match
			pkt_matches->eth_type= platform_packet_get_eth_type(pkt); 
			pkt_matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt); 
			break;
	
		//PUSH
		case OF1X_AT_PUSH_PPPOE:
			//Call platform
			platform_packet_push_pppoe(pkt, action->field.u16);
			//Update match
			pkt_matches->eth_type= platform_packet_get_eth_type(pkt); 
			pkt_matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt); 
			break;
		case OF1X_AT_PUSH_MPLS:
			//Call platform
			platform_packet_push_mpls(pkt, action->field.u16);
			//Update match
			pkt_matches->eth_type= platform_packet_get_eth_type(pkt); 
			pkt_matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt); 
			break;
		case OF1X_AT_PUSH_VLAN:
			//Call platform
			platform_packet_push_vlan(pkt, action->field.u16);
			//Update match
			pkt_matches->eth_type= platform_packet_get_eth_type(pkt); 
			pkt_matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt); 
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
			platform_packet_set_mpls_ttl(pkt, action->field.u64);
			break;
		case OF1X_AT_SET_NW_TTL:
			//Call platform
			platform_packet_set_nw_ttl(pkt, action->field.u64);
			break;

		//QUEUE
		case OF1X_AT_SET_QUEUE:
			//Call platform
			platform_packet_set_queue(pkt, action->field.u64);
			break;

		//TODO 
		//case OF1X_AT_SET_FIELD_METADATA: platform_packet_set_metadata(pkt, action->field);
		//	break;

		//802
		case OF1X_AT_SET_FIELD_ETH_DST: 
			//Call platform
			platform_packet_set_eth_dst(pkt, action->field.u64);
			//Update match
			pkt_matches->eth_dst = action->field.u64; 
			break;
		case OF1X_AT_SET_FIELD_ETH_SRC: 
			//Call platform
			platform_packet_set_eth_src(pkt, action->field.u64); 
			//Update match
			pkt_matches->eth_src = action->field.u64; 
			break;
		case OF1X_AT_SET_FIELD_ETH_TYPE: 
			//Call platform
			platform_packet_set_eth_type(pkt, action->field.u64);
			//Update match
			pkt_matches->eth_type = action->field.u64;
			break;

		//802.1q
		case OF1X_AT_SET_FIELD_VLAN_VID: 
			//Call platform
			platform_packet_set_vlan_vid(pkt, action->field.u64);
			//Update match
			pkt_matches->vlan_vid = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_VLAN_PCP: 
			//Call platform
			platform_packet_set_vlan_pcp(pkt, action->field.u64);
			//Update match
			pkt_matches->vlan_pcp = action->field.u64;
			break;

		//ARP
		case OF1X_AT_SET_FIELD_ARP_OPCODE:
			//Call plattform
			platform_packet_set_arp_opcode(pkt, action->field.u16);
			//Update match
			pkt_matches->arp_opcode = action->field.u16;
			break;
		case OF1X_AT_SET_FIELD_ARP_SHA:
			//Call platform
			platform_packet_set_arp_sha(pkt, action->field.u64);
			//Update match
			pkt_matches->arp_sha = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_ARP_SPA:
			//Call platform
			platform_packet_set_arp_spa(pkt, action->field.u32);
			//Update match
			pkt_matches->arp_spa = action->field.u32;
			break;
		case OF1X_AT_SET_FIELD_ARP_THA:
			//Call platform
			platform_packet_set_arp_tha(pkt, action->field.u64);
			//Update match
			pkt_matches->arp_tha = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_ARP_TPA:
			//Call platform
			platform_packet_set_arp_tpa(pkt, action->field.u32);
			//Update match
			pkt_matches->arp_tpa = action->field.u32;
			break;

		//IP
		case OF1X_AT_SET_FIELD_IP_DSCP:
			//Call platform
			platform_packet_set_ip_dscp(pkt, action->field.u64);
			//Update match
			pkt_matches->ip_dscp = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_IP_ECN:
			//Call platform
			platform_packet_set_ip_ecn(pkt, action->field.u64);
			//Update match
			pkt_matches->ip_ecn = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_IP_PROTO:
			//Call platform
			platform_packet_set_ip_proto(pkt, action->field.u64);
			//Update match
			pkt_matches->ip_proto = action->field.u64;
			break;

		//IPv4
		case OF1X_AT_SET_FIELD_IPV4_SRC:
			//Call platform
			platform_packet_set_ipv4_src(pkt, action->field.u64);
			//Update match
			pkt_matches->ipv4_src = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_IPV4_DST:
			//Call platform
			platform_packet_set_ipv4_dst(pkt, action->field.u64);
			//Update match
			pkt_matches->ipv4_dst = action->field.u64;
			break;

		//TP
		case OF1X_AT_SET_FIELD_TP_SRC:  
			if((pkt_matches->ip_proto == OF1X_IP_PROTO_TCP)){
				//Call platform
				platform_packet_set_tcp_src(pkt, action->field.u64);
				//Update match
				pkt_matches->tcp_src = action->field.u64;
			}else if((pkt_matches->ip_proto == OF1X_IP_PROTO_UDP)){
				//Call platform
				platform_packet_set_udp_src(pkt, action->field.u64);
				//Update match
				pkt_matches->udp_src = action->field.u64;
			}
			break;
		case OF1X_AT_SET_FIELD_TP_DST:
			if((pkt_matches->ip_proto == OF1X_IP_PROTO_TCP)){
				//Call platform
				platform_packet_set_tcp_dst(pkt, action->field.u64);
				//Update match
				pkt_matches->tcp_dst = action->field.u64;
			}else if((pkt_matches->ip_proto == OF1X_IP_PROTO_UDP)){
				//Call platform
				platform_packet_set_udp_dst(pkt, action->field.u64);
				//Update match
				pkt_matches->udp_dst = action->field.u64;
			}
			break;

		//TCP
		case OF1X_AT_SET_FIELD_TCP_SRC:  
			//Call platform
			platform_packet_set_tcp_src(pkt, action->field.u64);
			//Update match
			pkt_matches->tcp_src = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_TCP_DST:
			//Call platform
			platform_packet_set_tcp_dst(pkt, action->field.u64);
			//Update match
			pkt_matches->tcp_dst = action->field.u64;
			break;

		//UDP
		case OF1X_AT_SET_FIELD_UDP_SRC:
			//Call platform
			platform_packet_set_udp_src(pkt, action->field.u64);
			//Update match
			pkt_matches->udp_src = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_UDP_DST:
			//Call platform
			platform_packet_set_udp_dst(pkt, action->field.u64);
			//Update match
			pkt_matches->udp_dst = action->field.u64;
			break;
		//SCTP
		case OF1X_AT_SET_FIELD_SCTP_SRC:
			//Call platform
			platform_packet_set_sctp_src(pkt, action->field.u64);
			//Update match
			pkt_matches->sctp_src = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_SCTP_DST:
			//Call platform
			platform_packet_set_sctp_dst(pkt, action->field.u64);
			//Update match
			pkt_matches->sctp_dst = action->field.u64;
			break;


		//ICMPv4
		case OF1X_AT_SET_FIELD_ICMPV4_TYPE:
			//Call platform
			platform_packet_set_icmpv4_type(pkt, action->field.u64);
			//Update match
			pkt_matches->icmpv4_type = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_ICMPV4_CODE:
			//Call platform
			platform_packet_set_icmpv4_code(pkt, action->field.u64);
			//Update match
			pkt_matches->icmpv4_code = action->field.u64;
			break;

		//MPLS
		case OF1X_AT_SET_FIELD_MPLS_LABEL:
			//Call platform
			platform_packet_set_mpls_label(pkt, action->field.u64);
			//Update match
			pkt_matches->mpls_label = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_MPLS_TC:
			//Call platform
			platform_packet_set_mpls_tc(pkt, action->field.u64);
			//Update match
			pkt_matches->mpls_tc = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_MPLS_BOS:
			//Call platform
			platform_packet_set_mpls_bos(pkt, action->field.u64);
			//Update match
			pkt_matches->mpls_bos = action->field.u64;
			break;


		//PPPoE
		case OF1X_AT_SET_FIELD_PPPOE_CODE:
			//Call platform
			platform_packet_set_pppoe_code(pkt, action->field.u64);
			//Update match
			pkt_matches->pppoe_code = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_PPPOE_TYPE:
			//Call platform
			platform_packet_set_pppoe_type(pkt, action->field.u64);
			//Update match
			pkt_matches->pppoe_type = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_PPPOE_SID:
			//Call platform
			platform_packet_set_pppoe_sid(pkt, action->field.u64);
			//Update match
			pkt_matches->pppoe_sid = action->field.u64;
			break;

		//PPP
		case OF1X_AT_SET_FIELD_PPP_PROT:
			//Call platform
			platform_packet_set_ppp_proto(pkt, action->field.u64);
			//Update match
			pkt_matches->ppp_proto = action->field.u64;
			break;
			
		//IPv6
		case OF1X_AT_SET_FIELD_IPV6_SRC:
			//Call platform
			platform_packet_set_ipv6_src(pkt, action->field.u128);
			//Update match
			pkt_matches->ipv6_src = action->field.u128;
			break;
		case OF1X_AT_SET_FIELD_IPV6_DST:
			//Call platform
			platform_packet_set_ipv6_dst(pkt, action->field.u128);
			//Update match
			pkt_matches->ipv6_dst = action->field.u128;
			break;
		case OF1X_AT_SET_FIELD_IPV6_FLABEL:
			//Call platform
			platform_packet_set_ipv6_flabel(pkt, action->field.u64);
			//Update match
			pkt_matches->ipv6_flabel = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_TARGET:
			//Call platform
			platform_packet_set_ipv6_nd_target(pkt, action->field.u128);
			//Update match
			pkt_matches->ipv6_nd_target = action->field.u128;
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_SLL:
			//Call platform
			platform_packet_set_ipv6_nd_sll(pkt, action->field.u64);
			//Update match
			pkt_matches->ipv6_nd_sll = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_TLL:
			//Call platform
			platform_packet_set_ipv6_nd_tll(pkt, action->field.u64);
			//Update match
			pkt_matches->ipv6_nd_tll = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_IPV6_EXTHDR:
			//Call platform
			platform_packet_set_ipv6_exthdr(pkt, action->field.u64);
			//Update match
			pkt_matches->ipv6_exthdr = action->field.u64;
			break;
		//ICMPv6
		case OF1X_AT_SET_FIELD_ICMPV6_TYPE:
			//Call platform
			platform_packet_set_icmpv6_type(pkt, action->field.u64);
			//Update match
			pkt_matches->icmpv6_type = action->field.u64;
			break;
			
		case OF1X_AT_SET_FIELD_ICMPV6_CODE:
			//Call platform
			platform_packet_set_icmpv6_code(pkt, action->field.u64);
			//Update match
			pkt_matches->icmpv6_code = action->field.u64;
			break;
		//GTP
		case OF1X_AT_SET_FIELD_GTP_MSG_TYPE:
			//Call platform
			platform_packet_set_gtp_msg_type(pkt, action->field.u64);
			//Update match
			pkt_matches->gtp_msg_type = action->field.u64;
			break;
		case OF1X_AT_SET_FIELD_GTP_TEID:
			//Call platform
			platform_packet_set_gtp_teid(pkt, action->field.u64);
			//Update match
			pkt_matches->gtp_teid = action->field.u64;
			break;
		case OF1X_AT_POP_GTP: 
			//Call platform
			platform_packet_pop_gtp(pkt);
			//Update match
			pkt_matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt); 
			break;
		case OF1X_AT_PUSH_GTP: 
			//Call platform
			platform_packet_push_gtp(pkt);
			//Update match
			pkt_matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt); 
			break;

		//PBB
		case OF1X_AT_SET_FIELD_PBB_ISID:
			//Call platform
			platform_packet_set_pbb_isid(pkt, action->field.u32);
			//Update match
			pkt_matches->pbb_isid = action->field.u32;
			break;
		case OF1X_AT_POP_PBB: 
			//Call platform
			platform_packet_pop_pbb(pkt, action->field.u16);
			//Update match
			pkt_matches->eth_type= platform_packet_get_eth_type(pkt); 
			pkt_matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt); 
			break;
		case OF1X_AT_PUSH_PBB: 
			//Call platform
			platform_packet_push_pbb(pkt, action->field.u16);
			//Update match
			pkt_matches->eth_type= platform_packet_get_eth_type(pkt); 
			pkt_matches->pkt_size_bytes = platform_packet_get_size_bytes(pkt); 
			break;

		//TUNNEL ID
		case OF1X_AT_SET_FIELD_TUNNEL_ID:
			//Call platform
			platform_packet_set_tunnel_id(pkt, action->field.u64);
			//Update match
			pkt_matches->tunnel_id = action->field.u64;
			break;

		case OF1X_AT_GROUP: __of1x_process_group_actions(sw, table_id, pkt, action->field.u64, action->group, replicate_pkts);
			break;

		case OF1X_AT_EXPERIMENTER: //FIXME: implement
			break;

		case OF1X_AT_OUTPUT: 
			
			if(action->field.u64 < OF1X_PORT_MAX || 
				 action->field.u64 == OF1X_PORT_ALL || 
				 action->field.u64 == OF1X_PORT_FLOOD || 
				 action->field.u64 == OF1X_PORT_NORMAL || 
				 action->field.u64 == OF1X_PORT_CONTROLLER){ 

				//Pointer for the packet to be sent
				datapacket_t* pkt_to_send;			
	
				//Duplicate the packet only if necessary
				if(replicate_pkts){
					pkt_to_send = platform_packet_replicate(pkt);
	
					//check for wrong copy
					if(!pkt_to_send)
						return;
				}else
					pkt_to_send = pkt;

				//Perform output
				if( action->field.u64 < LOGICAL_SWITCH_MAX_LOG_PORTS && NULL != sw->logical_ports[action->field.u64].port ){

					//Single port output
					platform_packet_output(pkt_to_send, sw->logical_ports[action->field.u64].port);

				}else if(action->field.u64 == OF1X_PORT_FLOOD || 
					action->field.u64 == OF1X_PORT_ALL){

					//Flood
					platform_packet_output(pkt_to_send, flood_meta_port);

				}else if(action->field.u64 == OF1X_PORT_CONTROLLER ||
					action->field.u64 == OF1X_PORT_NORMAL){

					//Controller
					platform_of1x_packet_in(sw, table_id, pkt_to_send, OF1X_PKT_IN_ACTION);

				}else{

					//This condition can never happen, unless port number has been somehow corrupted??
					assert(0);
					if(pkt != pkt_to_send)
						platform_packet_drop(pkt_to_send);
				}
			}
			break;
	}
}

void __of1x_process_apply_actions(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t* pkt, const of1x_action_group_t* apply_actions_group, bool replicate_pkts){

	of1x_packet_action_t* it;

	for(it=apply_actions_group->head;it;it=it->next){
		__of1x_process_packet_action(sw, table_id, pkt, it, replicate_pkts);
	}	
}

/*
*
* The of1x_process_write_actions is meant to encapsulate the processing of the write actions
*
*/
void __of1x_process_write_actions(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t* pkt, bool replicate_pkts){

	unsigned int i;
	of1x_write_actions_t* packet_write_actions;

	//Recover write actions from datapacket
	packet_write_actions = &pkt->write_actions.of1x;
	
	for(i=0;i<OF1X_AT_NUMBER;i++){
		if(packet_write_actions->write_actions[i].type){
			__of1x_process_packet_action(sw, table_id, pkt, &packet_write_actions->write_actions[i], replicate_pkts);
		}
	}
}

//Update apply/write
rofl_result_t __of1x_update_apply_actions(of1x_action_group_t** group, of1x_action_group_t* new_group){

	of1x_action_group_t* old_group = *group;

	//Transfer
	*group = new_group;

	//Release if necessary
	if(old_group)
		of1x_destroy_action_group(old_group);	

	return ROFL_SUCCESS;
}

rofl_result_t __of1x_update_write_actions(of1x_write_actions_t** group, of1x_write_actions_t* new_group){

	of1x_write_actions_t* old_group = *group;
	
	//Transfer
	*group = new_group;
	
	//Destroy old group	
	if(old_group)
		__of1x_destroy_write_actions(old_group);
	
	return ROFL_SUCCESS;
}

static
void __of1x_process_group_actions(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t *pkt,uint64_t field, of1x_group_t *group, bool replicate_pkts){
	of1x_bucket_t *it_bk;
	of1x_packet_matches_t *matches = &pkt->matches.of1x;
	
	//process the actions in the buckets depending on the type
	switch(group->type){
		case OF1X_GROUP_TYPE_ALL:
			//executes all buckets
			platform_rwlock_rdlock(group->rwlock);
			for (it_bk = group->bc_list->head; it_bk!=NULL;it_bk = it_bk->next){
				//process all actions in the bucket
				__of1x_process_apply_actions(sw,table_id,pkt,it_bk->actions, replicate_pkts);
				__of1x_stats_bucket_update(&it_bk->stats, matches->pkt_size_bytes);
			}
			platform_rwlock_rdunlock(group->rwlock);
			break;
		case OF1X_GROUP_TYPE_SELECT:
			//optional
			//ROFL_PIPELINE_DEBUG("<%s:%d> Group type not implemented",__func__,__LINE__);
			break;
		case OF1X_GROUP_TYPE_INDIRECT:
			//executes the "one bucket defined"
			platform_rwlock_rdlock(group->rwlock);
			__of1x_process_apply_actions(sw,table_id,pkt,group->bc_list->head->actions, replicate_pkts);
			__of1x_stats_bucket_update(&group->bc_list->head->stats, matches->pkt_size_bytes);
			platform_rwlock_rdunlock(group->rwlock);
			break;
		case OF1X_GROUP_TYPE_FF:
			//optional
			//ROFL_PIPELINE_DEBUG("<%s:%d> Group type not implemented",__func__,__LINE__);
			break;
		default:
			break;
	}
	__of1x_stats_group_update(&group->stats, matches->pkt_size_bytes);
	
}
//Checking functions
/*TODO specific funcions for 128 bits*/
bool __of1x_write_actions_has(of1x_write_actions_t* write_actions, of1x_packet_action_type_t type, uint64_t value){
	if(!write_actions)
		return false;	
	
	of1x_packet_action_t action = write_actions->write_actions[type];

	return (action.type != OF1X_AT_NO_ACTION) && (value != 0x0 && action.field.u64 == value ); 
}
/*TODO specific funcions for 128 bits*/
bool __of1x_apply_actions_has(const of1x_action_group_t* apply_actions_group, of1x_packet_action_type_t type, uint64_t value){

	of1x_packet_action_t *it;

	if(!apply_actions_group)
		return false;	


	for(it=apply_actions_group->head; it; it=it->next){
		if( (it->type == type) && (value != 0x0 && it->field.u64 == value) )
			return true;
	}
	return false;	
}

//Copy (cloning) methods
of1x_packet_action_t* __of1x_copy_packet_action(of1x_packet_action_t* action){

	of1x_packet_action_t* copy;

	copy = platform_malloc_shared(sizeof(of1x_packet_action_t));

	if(!copy)
		return NULL;

	*copy = *action;

	return copy;
}

of1x_action_group_t* __of1x_copy_action_group(of1x_action_group_t* origin){

	of1x_action_group_t* copy;
	of1x_packet_action_t* it;	

	if(!origin)
		return NULL;

	copy = platform_malloc_shared(sizeof(of1x_action_group_t));


	if(!copy)
		return NULL;

	copy->head = copy->tail = NULL;
	copy->num_of_actions = origin->num_of_actions;
	copy->num_of_output_actions = origin->num_of_output_actions;

	//Copy al apply actions
	for(it=origin->head;it;it=it->next){
		of1x_packet_action_t* act;
		
		act = __of1x_copy_packet_action(it);

		if(!act){
			of1x_destroy_action_group(copy);
			return NULL;
		}	


		//Insert in the double linked-list
		if(!copy->tail){
			copy->head = act; 
			act->prev = NULL;
		}else{
			act->prev = copy->tail;
			copy->tail->next = act;
		}				
		act->next = NULL;
		copy->tail = act;
	}

	return copy;
}

of1x_write_actions_t* __of1x_copy_write_actions(of1x_write_actions_t* origin){
	
	of1x_write_actions_t* copy; 

	if(!origin)
		return NULL;

	copy = platform_malloc_shared(sizeof(of1x_write_actions_t)); 

	if(!copy)
		return NULL;
	
	//Copy Values
	*copy = *origin;
	
	return copy;
}

#define HI(x) *(uint64_t*)&x.val[0]
#define LO(x) *(uint64_t*)&x.val[8]

/* Dumping */
static void __of1x_dump_packet_action(of1x_packet_action_t action){

	ROFL_PIPELINE_DEBUG_NO_PREFIX("<");
	switch(action.type){
		case OF1X_AT_NO_ACTION: /*TODO: print some error traces? */
			break;

		case OF1X_AT_COPY_TTL_IN: ROFL_PIPELINE_DEBUG_NO_PREFIX("TTL_IN");
			break;

		case OF1X_AT_POP_VLAN:ROFL_PIPELINE_DEBUG_NO_PREFIX("POP_VLAN");
			break;
		case OF1X_AT_POP_MPLS:ROFL_PIPELINE_DEBUG_NO_PREFIX("POP_MPLS");
			break;
		case OF1X_AT_POP_PPPOE:ROFL_PIPELINE_DEBUG_NO_PREFIX("POP_PPPOE");
			break;

		case OF1X_AT_PUSH_PPPOE:ROFL_PIPELINE_DEBUG_NO_PREFIX("PUSH_PPPOE");
			break;
		case OF1X_AT_PUSH_MPLS:ROFL_PIPELINE_DEBUG_NO_PREFIX("PUSH_MPLS");
			break;
		case OF1X_AT_PUSH_VLAN:ROFL_PIPELINE_DEBUG_NO_PREFIX("PUSH_VLAN");
			break;

		case OF1X_AT_COPY_TTL_OUT:ROFL_PIPELINE_DEBUG_NO_PREFIX("COPY_TTL_OUT");
			break;

		case OF1X_AT_DEC_NW_TTL:ROFL_PIPELINE_DEBUG_NO_PREFIX("DEC_NW_TTL");
			break;
		case OF1X_AT_DEC_MPLS_TTL:ROFL_PIPELINE_DEBUG_NO_PREFIX("DEC_MPLS_TTL");
			break;

		case OF1X_AT_SET_MPLS_TTL:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_MPLS_TTL: %u",action.field.u64);
			break;
		case OF1X_AT_SET_NW_TTL:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_NW_TTL: %u",action.field.u64);
			break;

		case OF1X_AT_SET_QUEUE:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_QUEUE: %u",action.field.u64);
			break;

		//TODO 
		//case OF1X_AT_SET_FIELD_METADATA:
		//	break;

		case OF1X_AT_SET_FIELD_ETH_DST: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ETH_DST: 0x%"PRIx64,action.field.u64); 
			break;
		case OF1X_AT_SET_FIELD_ETH_SRC: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ETH_SRC: 0x%"PRIx64,action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_ETH_TYPE:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ETH_TYPE: 0x%x",action.field.u64);
			break;

		case OF1X_AT_SET_FIELD_VLAN_VID:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_VLAN_VID: 0x%x",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_VLAN_PCP:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_VLAN_PCP: 0x%x",action.field.u64);
			break;

		case OF1X_AT_SET_FIELD_ARP_OPCODE:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ARP_OPCODE: 0x%x",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_ARP_SHA: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ARP_SHA: 0x%"PRIx64,action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_ARP_SPA: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ARP_SPA: 0x%x",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_ARP_THA: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ARP_THA: 0x%"PRIx64,action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_ARP_TPA: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ARP_TPA: 0x%x",action.field.u64);
			break;

		case OF1X_AT_SET_FIELD_IP_DSCP: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IP_DSCP: 0x%x",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_IP_ECN:  ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IP_ECN: 0x%x",action.field.u64);
			break;

		case OF1X_AT_SET_FIELD_IP_PROTO:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IP_PROTO: 0x%x",action.field.u64);
			break;

		case OF1X_AT_SET_FIELD_IPV4_SRC:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IPV4_SRC: 0x%x",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_IPV4_DST:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IPV4_DST: 0x%x",action.field.u64);
			break;

		case OF1X_AT_SET_FIELD_TP_SRC: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_TP_SRC: %u",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_TP_DST: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_TP_DST: %u",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_TCP_SRC: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_TCP_SRC: %u",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_TCP_DST: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_TCP_DST: %u",action.field.u64);
			break;

		case OF1X_AT_SET_FIELD_UDP_SRC: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_UDP_SRC: %u",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_UDP_DST:  ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_UDP_DST: %u",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_SCTP_SRC: ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_SCTP_SRC: %u",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_SCTP_DST:  ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_SCTP_DST: %u",action.field.u64);
			break;

		case OF1X_AT_SET_FIELD_ICMPV4_TYPE:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ICMPV4_TYPE: 0x%x",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_ICMPV4_CODE:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ICMPV4_CODE: 0x%x",action.field.u64);
			break;

		case OF1X_AT_SET_FIELD_MPLS_LABEL:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_MPLS_LABEL: 0x%x",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_MPLS_TC:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_MPLS_TC: 0x%x",action.field.u64);
		case OF1X_AT_SET_FIELD_MPLS_BOS:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_MPLS_BOS: 0x%x",action.field.u64);
			break;

		case OF1X_AT_SET_FIELD_PPPOE_CODE:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_PPPOE_CODE: 0x%x",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_PPPOE_TYPE:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_PPPOE_TYPE: 0x%x",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_PPPOE_SID:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_PPPOE_SID: 0x%x",action.field.u64);
			break;

		case OF1X_AT_SET_FIELD_PPP_PROT:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_PPP_PROT: 0x%x",action.field.u64);
			break;
			
		case OF1X_AT_SET_FIELD_IPV6_SRC:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IPV6_SRC: 0x%lx %lx",HI(action.field.u128),LO(action.field.u128));
			break;
		case OF1X_AT_SET_FIELD_IPV6_DST:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IPV6_DST: 0x%lx %lx",HI(action.field.u128),LO(action.field.u128));
			break;
		case OF1X_AT_SET_FIELD_IPV6_FLABEL:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IPV6_FLABEL: 0x%u",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_TARGET:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IPV6_ND_TARGET: 0x%lx %lx",HI(action.field.u128),LO(action.field.u128));
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_SLL:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IPV6_ND_SLL: 0x%x",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_TLL:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IPV6_ND_TLL: 0x%x",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_IPV6_EXTHDR:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_IPV6_EXTHDR: 0x%x",action.field.u64);
			break;
			
		case OF1X_AT_SET_FIELD_ICMPV6_TYPE:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ICMPV6_TYPE: 0x%u",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_ICMPV6_CODE:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_ICMPV6_CODE: 0x%u",action.field.u64);
		
		case OF1X_AT_POP_PBB:ROFL_PIPELINE_DEBUG_NO_PREFIX("POP_PBB");
			break;
		case OF1X_AT_PUSH_PBB:ROFL_PIPELINE_DEBUG_NO_PREFIX("PUSH_PBB");
			break;
		case OF1X_AT_SET_FIELD_PBB_ISID:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_PBB_ISID: 0x%u",action.field.u64);
			break;
		case OF1X_AT_SET_FIELD_TUNNEL_ID:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_TUNNEL_ID: 0x%u",action.field.u64);
			break;
		

		case OF1X_AT_SET_FIELD_GTP_MSG_TYPE:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_GTP_MSG_TYPE: 0x%x",action.field);
			break;
		case OF1X_AT_SET_FIELD_GTP_TEID:ROFL_PIPELINE_DEBUG_NO_PREFIX("SET_GTP_TEID: 0x%x",action.field);
			break;
		case OF1X_AT_POP_GTP:ROFL_PIPELINE_DEBUG_NO_PREFIX("POP_GTP");
			break;
		case OF1X_AT_PUSH_GTP:ROFL_PIPELINE_DEBUG_NO_PREFIX("PUSH_GTP");
			break;


		case OF1X_AT_GROUP:ROFL_PIPELINE_DEBUG_NO_PREFIX("GROUP");
			break;

		case OF1X_AT_EXPERIMENTER:ROFL_PIPELINE_DEBUG_NO_PREFIX("EXPERIMENTER");
			break;

		case OF1X_AT_OUTPUT:ROFL_PIPELINE_DEBUG_NO_PREFIX("OUTPUT port: %u",action.field.u64);
			break;
	}
	ROFL_PIPELINE_DEBUG_NO_PREFIX(">,");

}

void __of1x_dump_write_actions(of1x_write_actions_t* write_actions_group){
	unsigned int i=0;
	
	if(!write_actions_group)
		return;

	for(i=0;i<OF1X_AT_NUMBER;i++){
		if(write_actions_group->write_actions[i].type)
			__of1x_dump_packet_action(write_actions_group->write_actions[i]);
	}
}
	
void __of1x_dump_action_group(of1x_action_group_t* action_group){

	of1x_packet_action_t* action;


	if(!action_group)
		return;
	for(action=action_group->head;action;action=action->next){
		__of1x_dump_packet_action(*action);
	}
}

bool __of1x_validate_action_group(of1x_action_group_t *ag, of1x_group_table_t *gt){
	//TODO we need to validate ALL the actions here!
	of1x_packet_action_t *pa_it;

	if(ag){
		for(pa_it=ag->head; pa_it; pa_it=pa_it->next){
			if(pa_it->type == OF1X_AT_GROUP && gt){
				if((pa_it->group=__of1x_group_search(gt,pa_it->field.u64))==NULL)
					return ROFL_FAILURE;
				else{
					ag->num_of_output_actions+=pa_it->group->num_of_output_actions;
				}
			}
		}
	}
	
	return true;
}

bool __of1x_validate_write_actions(of1x_write_actions_t *wa, of1x_group_table_t *gt){
	//TODO we need to ALL validate the actions here!
	int i;
	of1x_packet_action_t *pa_it;
	
	for(i=0;i<OF1X_AT_NUMBER;i++){
		pa_it = &(wa->write_actions[i]);
		if(gt && pa_it && pa_it->type == OF1X_AT_GROUP){
			if((pa_it->group=__of1x_group_search(gt,pa_it->field.u64))==NULL )
				return ROFL_FAILURE;
			else{
				wa->num_of_output_actions+=pa_it->group->num_of_output_actions;
			}
		}
	}
	
	return true;
}
