#include "of12_action.h"

#include "of12_packet_matches.h" //TODO: evaluate if this is the best approach to update of12_matches after actions
#include "../../../platform/platform_hooks.h"
#include "../../../platform/memory.h"
#include <stdio.h>


//Byte masks
#define OF12_AT_6_BYTE_MASK 0x0000FFFFFFFFFFFF
#define OF12_AT_4_BYTE_MASK 0x00000000FFFFFFFF
#define OF12_AT_2_BYTE_MASK 0x000000000000FFFF
#define OF12_AT_1_BYTE_MASK 0x00000000000000FF

//Non-multiple of byte masks
#define OF12_AT_20_BITS_MASK 0x00000000000FFFFF
#define OF12_AT_13_BITS_MASK 0x00000000000FFFFF
#define OF12_AT_6_BITS_MASK 0x00000000000FFFFF
#define OF12_AT_3_BITS_MASK 0x00000000000FFFFF
#define OF12_AT_2_BITS_MASK 0x00000000000FFFFF

/* Actions init and destroyed */
of12_packet_action_t* of12_init_packet_action(of12_packet_action_type_t type, uint64_t field, of12_packet_action_t* prev, of12_packet_action_t* next){

	of12_packet_action_t* action;

	if(!type)
		return NULL;
	action = cutil_malloc_shared(sizeof(of12_packet_action_t));

	if(!action)
		return NULL;
	
	//Set type
	action->type = type;

	/*
	* Set field
	*/
	switch(type){
		//8 byte values (TODO: METADATA)
		//6 byte values
		case OF12_AT_SET_FIELD_ETH_DST:
		case OF12_AT_SET_FIELD_ETH_SRC:
			action->field = field&OF12_AT_6_BYTE_MASK;
			break;
	
		//4 byte values
		case OF12_AT_SET_FIELD_IPV4_DST:
		case OF12_AT_SET_FIELD_IPV4_SRC:
		case OF12_AT_OUTPUT:
			action->field = field&OF12_AT_4_BYTE_MASK;
			break;
		//20 bit values
		case OF12_AT_SET_FIELD_MPLS_LABEL:
			action->field = field&OF12_AT_20_BITS_MASK;
			break;
		//2 byte values
		case OF12_AT_SET_FIELD_ETH_TYPE:
		case OF12_AT_SET_FIELD_TCP_SRC:
		case OF12_AT_SET_FIELD_TCP_DST:
		case OF12_AT_SET_FIELD_UDP_SRC:
		case OF12_AT_SET_FIELD_UDP_DST:
		case OF12_AT_SET_FIELD_PPPOE_SID:
		case OF12_AT_SET_FIELD_PPP_PROT:
			action->field = field&OF12_AT_2_BYTE_MASK;
			break;
		//13 bit values
		case OF12_AT_SET_FIELD_VLAN_VID:
			action->field = field&OF12_AT_13_BITS_MASK;
			break;
		//1 byte values
		case OF12_AT_SET_FIELD_PPPOE_CODE:
		case OF12_AT_SET_FIELD_PPPOE_TYPE:
		case OF12_AT_SET_MPLS_TTL:
		case OF12_AT_SET_NW_TTL:
		case OF12_AT_SET_FIELD_IP_PROTO:
		case OF12_AT_SET_FIELD_ICMPV4_TYPE:
		case OF12_AT_SET_FIELD_ICMPV4_CODE:
			action->field = field&OF12_AT_1_BYTE_MASK;
			break;
		//6 bit values
		case OF12_AT_SET_FIELD_IP_DSCP:
			action->field = field&OF12_AT_6_BITS_MASK;
			break;
		//3 bit values
		case OF12_AT_SET_FIELD_VLAN_PCP:
		case OF12_AT_SET_FIELD_MPLS_TC:
			action->field = field&OF12_AT_3_BITS_MASK;
			break;
		//2 bit values
		case OF12_AT_SET_FIELD_IP_ECN:
			action->field = field&OF12_AT_2_BITS_MASK;
			break;
		//case OF12_AT_SET_QUEUE: TODO
		default:
			field = 0;
			break;
	}
	
	//Set list pointers
	action->next = next;
	action->prev = prev;
	
	return action;
}

void of12_destroy_packet_action(of12_packet_action_t* action){

	//FIXME destroy port group
	cutil_free_shared(action);
}

/* Action group init and destroy */
of12_action_group_t* of12_init_action_group(of12_packet_action_t* actions){

	unsigned int number_of_actions=0;
	of12_action_group_t* action_group;
	
	action_group = cutil_malloc_shared(sizeof(of12_action_group_t));

	if(!action_group)
		return NULL;
	
	if(actions){
		action_group->head = actions;
	
		for(;actions;actions=actions->next, number_of_actions++){
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
	
	return action_group;
}

void of12_destroy_action_group(of12_action_group_t* group){

	of12_packet_action_t* it,*next;

	if(!group)
		return;

	for(it=group->head;it;it=next){
		next = it->next; 
		of12_destroy_packet_action(it);
	}
	cutil_free_shared(group);	
}

/* Addition of an action to an action group */
void of12_push_packet_action_to_group(of12_action_group_t* group, of12_packet_action_t* action){

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

}

/* Write actions init */
void of12_init_packet_write_actions(datapacket_t *const pkt, of12_write_actions_t* write_actions){
	pkt->write_actions = (of_write_actions_t*)write_actions; 
	memset(write_actions, 0, sizeof(of12_write_actions_t));
}

of12_write_actions_t* of12_init_write_actions(){

	of12_write_actions_t* write_actions = cutil_malloc_shared(sizeof(of12_write_actions_t)); 

	if(!write_actions)
		return NULL;

	memset(write_actions, 0, sizeof(of12_write_actions_t));
	return write_actions;
}
void of12_destroy_write_actions(of12_write_actions_t* write_actions){
	cutil_free_shared(write_actions);	
}

void of12_set_packet_action_on_write_actions(of12_write_actions_t* write_actions, of12_packet_action_t* action){
	if( write_actions && action )	
		write_actions->write_actions[action->type] = *action;	
}

//Update of write actions
void of12_update_packet_write_actions(datapacket_t* pkt, const of12_write_actions_t* entry_write_actions) {

	unsigned int i;
	of12_write_actions_t* packet_write_actions;

	//Recover write actions from datapacket
	packet_write_actions = (of12_write_actions_t*)pkt->write_actions;
	
	if(!entry_write_actions)
		return;

	//Loop over entry write actions and update packet_write_actions
	for(i=0;i<OF12_AT_NUMBER;i++){
		if(entry_write_actions->write_actions[i].type)
			packet_write_actions->write_actions[i] = entry_write_actions->write_actions[i];
	}

}

//Clear actions
void of12_clear_write_actions(of12_write_actions_t* write_actions){
	memset(write_actions, 0, sizeof(of12_write_actions_t));
}

/* Contains switch with all the different action functions */
inline void of12_process_packet_action(datapacket_t* pkt, of12_packet_action_t* action){

	switch(action->type){
		case OF12_AT_NO_ACTION: /*TODO: print some error traces? */
			break;
		case OF12_AT_COPY_TTL_IN: platform_copy_ttl_in(pkt);
			break;
		case OF12_AT_POP_VLAN: platform_pop_vlan(pkt);
			break;
		case OF12_AT_POP_MPLS: platform_pop_mpls(pkt);
			break;
		case OF12_AT_POP_PPPOE: platform_pop_pppoe(pkt);
			break;
		case OF12_AT_POP_PPP: platform_pop_ppp(pkt);
			break;
		case OF12_AT_PUSH_PPPOE: platform_push_pppoe(pkt, action->field);
			break;
		case OF12_AT_PUSH_MPLS: platform_push_mpls(pkt, action->field);
			break;
		case OF12_AT_PUSH_VLAN: platform_push_vlan(pkt, action->field);
			break;
		case OF12_AT_COPY_TTL_OUT: platform_copy_ttl_out(pkt);
			break;
		case OF12_AT_DEC_NW_TTL: platform_dec_nw_ttl(pkt);
			break;
		case OF12_AT_DEC_MPLS_TTL: platform_dec_mpls_ttl(pkt);
			break;
		case OF12_AT_SET_MPLS_TTL: platform_set_mpls_ttl(pkt, action->field);
			break;
		case OF12_AT_SET_NW_TTL: platform_set_nw_ttl(pkt, action->field);
			break;
		case OF12_AT_SET_QUEUE: platform_set_queue(pkt, action->field);
			break;
		//TODO 
		//case OF12_AT_SET_FIELD_METADATA: platform_set_metadata(pkt, action->field);
		//	break;
		case OF12_AT_SET_FIELD_ETH_DST: platform_set_eth_dst(pkt, action->field); 
			break;
		case OF12_AT_SET_FIELD_ETH_SRC: platform_set_eth_src(pkt, action->field); 
			break;
		case OF12_AT_SET_FIELD_ETH_TYPE: platform_set_eth_type(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_VLAN_VID: platform_set_vlan_vid(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_VLAN_PCP: platform_set_vlan_pcp(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_IP_DSCP:  platform_set_ip_dscp(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_IP_ECN:   platform_set_ip_ecn(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_IP_PROTO: platform_set_ip_proto(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_IPV4_SRC: platform_set_ipv4_src(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_IPV4_DST: platform_set_ipv4_dst(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_TCP_SRC:  platform_set_tcp_src(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_TCP_DST:  platform_set_tcp_dst(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_UDP_SRC:  platform_set_udp_src(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_UDP_DST:   platform_set_udp_dst(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_ICMPV4_TYPE: platform_set_icmpv4_type(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_ICMPV4_CODE: platform_set_icmpv4_code(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_MPLS_LABEL: platform_set_mpls_label(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_MPLS_TC: platform_set_mpls_tc(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_PPPOE_CODE: platform_set_pppoe_code(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_PPPOE_TYPE: platform_set_pppoe_type(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_PPPOE_SID: platform_set_pppoe_sid(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_PPP_PROT: platform_set_ppp_proto(pkt, action->field);
			break;
		case OF12_AT_GROUP: //FIXME: implement
			break;
		case OF12_AT_EXPERIMENTER: //FIXME: implement
			break;
		case OF12_AT_OUTPUT: platform_output_packet(pkt, action->field);
			break;
	}
}

void of12_process_apply_actions(datapacket_t* pkt, const of12_action_group_t* apply_actions_group){

	of12_packet_action_t* it;

	for(it=apply_actions_group->head;it;it=it->next){
		of12_process_packet_action(pkt, it);
	}	
	of12_update_packet_matches(pkt); //TODO: evaluate wether it can be updated directly on of12_process_packet action without calling again platform methods, and evaluate the performance impact
}

/*
*
* The of12_process_write_actions is meant to encapsulate the processing of the write actions
*
*/
void of12_process_write_actions(datapacket_t* pkt){

	unsigned int i;
	of12_write_actions_t* packet_write_actions;

	//Recover write actions from datapacket
	packet_write_actions = (of12_write_actions_t*)pkt->write_actions;
	
	for(i=0;i<OF12_AT_NUMBER;i++){
		if(packet_write_actions->write_actions[i].type){
			of12_process_packet_action(pkt, &packet_write_actions->write_actions[i]);
		}
	}
}


/* Dumping */
static void of12_dump_packet_action(of12_packet_action_t action){

	fprintf(stderr,"<");
	switch(action.type){
		case OF12_AT_NO_ACTION: /*TODO: print some error traces? */
			break;
		case OF12_AT_COPY_TTL_IN: fprintf(stderr, "TTL_IN");
			break;
		case OF12_AT_POP_VLAN:fprintf(stderr, "POP_VLAN");
			break;
		case OF12_AT_POP_MPLS:fprintf(stderr, "POP_MPLS");
			break;
		case OF12_AT_POP_PPPOE:fprintf(stderr, "POP_PPPOE");
			break;
		case OF12_AT_POP_PPP:fprintf(stderr, "POP_PPP");
			break;
		case OF12_AT_PUSH_PPPOE:fprintf(stderr, "PUSH_PPPOE");
			break;
		case OF12_AT_PUSH_MPLS:fprintf(stderr, "PUSH_MPLS");
			break;
		case OF12_AT_PUSH_VLAN:fprintf(stderr, "PUSH_VLAN");
			break;
		case OF12_AT_COPY_TTL_OUT:fprintf(stderr, "COPY_TTL_OUT");
			break;
		case OF12_AT_DEC_NW_TTL:fprintf(stderr, "DEC_NW_TTL");
			break;
		case OF12_AT_DEC_MPLS_TTL:fprintf(stderr, "DEC_MPLS_TTL");
			break;
		case OF12_AT_SET_MPLS_TTL:fprintf(stderr, "SET_MPLS_TTL%llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_NW_TTL:fprintf(stderr, "SET_NW_TTL: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_QUEUE:fprintf(stderr, "SET_QUEUE:%llx",(long long unsigned int)action.field);
			break;
		//TODO 
		//case OF12_AT_SET_FIELD_METADATA:
		//	break;
		case OF12_AT_SET_FIELD_ETH_DST: fprintf(stderr, "SET_ETH_DST: %llx",(long long unsigned int)action.field); 
			break;
		case OF12_AT_SET_FIELD_ETH_SRC: fprintf(stderr, "SET_ETH_SRC: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_ETH_TYPE:fprintf(stderr, "SET_ETH_TYPE: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_VLAN_VID:fprintf(stderr, "SET_VLAN_VID: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_VLAN_PCP:fprintf(stderr, "SET_VLAN_PCP: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_IP_DSCP: fprintf(stderr, "SET_IP_DSCP: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_IP_ECN:  fprintf(stderr, "SET_IP_ECN: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_IP_PROTO:fprintf(stderr, "SET_IP_PROTO: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_IPV4_SRC:fprintf(stderr, "SET_IPV4_SRC: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_IPV4_DST:fprintf(stderr, "SET_IPV4_DST: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_TCP_SRC: fprintf(stderr, "SET_TCP_SRC: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_TCP_DST: fprintf(stderr, "SET_TCP_DST: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_UDP_SRC: fprintf(stderr, "SET_UDP_SRC: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_UDP_DST:  fprintf(stderr, "SET_UDP_DST: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_ICMPV4_TYPE:fprintf(stderr, "SET_ICMPV4_TYPE: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_ICMPV4_CODE:fprintf(stderr, "SET_ICMPV4_CODE: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_MPLS_LABEL:fprintf(stderr, "SET_MPLS_LABEL: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_MPLS_TC:fprintf(stderr, "SET_MPLS_TC: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_PPPOE_CODE:fprintf(stderr, "SET_PPPOE_CODE: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_PPPOE_TYPE:fprintf(stderr, "SET_PPPOE_TYPE: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_PPPOE_SID:fprintf(stderr, "SET_PPPOE_SID: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_SET_FIELD_PPP_PROT:fprintf(stderr, "SET_PPP_PROT: %llx",(long long unsigned int)action.field);
			break;
		case OF12_AT_GROUP:fprintf(stderr, "GROUP");
			break;
		case OF12_AT_EXPERIMENTER:fprintf(stderr, "EXPERIMENTER");
			break;
		case OF12_AT_OUTPUT:fprintf(stderr, "OUTPUT: %llx",(long long unsigned int)action.field);
			break;
	}
	fprintf(stderr,">,");

}

void of12_dump_write_actions(of12_write_actions_t* write_actions_group){
	unsigned int i=0;
	
	if(!write_actions_group)
		return;

	for(i=0;i<OF12_AT_NUMBER;i++){
		if(write_actions_group->write_actions[i].type)
			of12_dump_packet_action(write_actions_group->write_actions[i]);
	}
}
	
void of12_dump_action_group(of12_action_group_t* action_group){

	of12_packet_action_t* action;


	if(!action_group)
		return;
	for(action=action_group->head;action;action=action->next){
		of12_dump_packet_action(*action);
	}
}

