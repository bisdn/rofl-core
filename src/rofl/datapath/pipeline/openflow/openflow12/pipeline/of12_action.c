#include "of12_action.h"

#include <stdio.h>
#include <assert.h>

#include "of12_packet_matches.h" //TODO: evaluate if this is the best approach to update of12_matches after actions
#include "../../../physical_switch.h"
#include "../../../platform/packet.h"
#include "../../../platform/memory.h"
#include "../of12_endpoint_hooks.h"

//Flood port
extern switch_port_t* flood_meta_port;

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

static void of12_process_group_actions(const struct of12_switch* sw, const unsigned int table_id, datapacket_t *pkt,uint64_t field, of12_group_t* group);

/* Actions init and destroyed */
of12_packet_action_t* of12_init_packet_action(/*const struct of12_switch* sw, */of12_packet_action_type_t type, uint64_t field, of12_packet_action_t* prev, of12_packet_action_t* next){

	of12_packet_action_t* action;

	if(!type)
		return NULL;
	action = platform_malloc_shared(sizeof(of12_packet_action_t));

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
		/*case OF12_AT_POP_VLAN: TODO: CHECK THIS*/
		case OF12_AT_POP_MPLS: 
		case OF12_AT_POP_PPPOE: 
		case OF12_AT_PUSH_PPPOE:
		case OF12_AT_PUSH_MPLS: 
		case OF12_AT_PUSH_VLAN: 
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
		case OF12_AT_GROUP:
			action->field =  field&OF12_AT_4_BYTE_MASK; //id of the group
			//action->group = of12_group_search(sw->pipeline->groups, action->field); // pointer to the group //FIXME evaluate if this can be done here or not
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

	platform_free_shared(action);
}

/* Action group init and destroy */
of12_action_group_t* of12_init_action_group(of12_packet_action_t* actions){

	unsigned int number_of_actions=0, number_of_output_actions=0;
	of12_action_group_t* action_group;
	
	action_group = platform_malloc_shared(sizeof(of12_action_group_t));

	if(!action_group)
		return NULL;
	
	if(actions){
		action_group->head = actions;
	
		for(;actions;actions=actions->next, number_of_actions++){

			if(actions->type == OF12_AT_OUTPUT || actions->type == OF12_AT_GROUP)
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
	platform_free_shared(group);	
}

/* Addition of an action to an action group */
void of12_push_packet_action_to_group(of12_action_group_t* group, of12_packet_action_t* action){

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

	if(action->type == OF12_AT_OUTPUT || action->type == OF12_AT_GROUP)
		group->num_of_output_actions++;
	
}

/* Write actions init */
void of12_init_packet_write_actions(datapacket_t *const pkt, of12_write_actions_t* write_actions){
	pkt->write_actions = (of_write_actions_t*)write_actions; 
	memset(write_actions, 0, sizeof(of12_write_actions_t));
}

of12_write_actions_t* of12_init_write_actions(){

	of12_write_actions_t* write_actions = platform_malloc_shared(sizeof(of12_write_actions_t)); 

	if(!write_actions)
		return NULL;

	memset(write_actions, 0, sizeof(of12_write_actions_t));
	return write_actions;
}

void of12_destroy_write_actions(of12_write_actions_t* write_actions){
	platform_free_shared(write_actions);	
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
static inline void of12_process_packet_action(const struct of12_switch* sw, const unsigned int table_id, datapacket_t* pkt, of12_packet_action_t* action, bool replicate_pkts){

	switch(action->type){
		case OF12_AT_NO_ACTION: /*TODO: print some error traces? */
			break;

		case OF12_AT_COPY_TTL_IN: platform_packet_copy_ttl_in(pkt);
			break;

		case OF12_AT_POP_VLAN: platform_packet_pop_vlan(pkt);
			break;
		case OF12_AT_POP_MPLS: platform_packet_pop_mpls(pkt, action->field);
			break;

		case OF12_AT_POP_PPPOE: platform_packet_pop_pppoe(pkt, action->field);
			break;
		case OF12_AT_PUSH_PPPOE: platform_packet_push_pppoe(pkt, action->field);
			break;

		case OF12_AT_PUSH_MPLS: platform_packet_push_mpls(pkt, action->field);
			break;
		case OF12_AT_PUSH_VLAN: platform_packet_push_vlan(pkt, action->field);
			break;

		case OF12_AT_COPY_TTL_OUT: platform_packet_copy_ttl_out(pkt);
			break;

		case OF12_AT_DEC_NW_TTL: platform_packet_dec_nw_ttl(pkt);
			break;
		case OF12_AT_DEC_MPLS_TTL: platform_packet_dec_mpls_ttl(pkt);
			break;

		case OF12_AT_SET_MPLS_TTL: platform_packet_set_mpls_ttl(pkt, action->field);
			break;
		case OF12_AT_SET_NW_TTL: platform_packet_set_nw_ttl(pkt, action->field);
			break;

		case OF12_AT_SET_QUEUE: platform_packet_set_queue(pkt, action->field);
			break;

		//TODO 
		//case OF12_AT_SET_FIELD_METADATA: platform_packet_set_metadata(pkt, action->field);
		//	break;

		case OF12_AT_SET_FIELD_ETH_DST: platform_packet_set_eth_dst(pkt, action->field); 
			break;
		case OF12_AT_SET_FIELD_ETH_SRC: platform_packet_set_eth_src(pkt, action->field); 
			break;
		case OF12_AT_SET_FIELD_ETH_TYPE: platform_packet_set_eth_type(pkt, action->field);
			break;

		case OF12_AT_SET_FIELD_VLAN_VID: platform_packet_set_vlan_vid(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_VLAN_PCP: platform_packet_set_vlan_pcp(pkt, action->field);
			break;

		case OF12_AT_SET_FIELD_IP_DSCP:  platform_packet_set_ip_dscp(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_IP_ECN:   platform_packet_set_ip_ecn(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_IP_PROTO: platform_packet_set_ip_proto(pkt, action->field);
			break;

		case OF12_AT_SET_FIELD_IPV4_SRC: platform_packet_set_ipv4_src(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_IPV4_DST: platform_packet_set_ipv4_dst(pkt, action->field);
			break;

		case OF12_AT_SET_FIELD_TCP_SRC:  platform_packet_set_tcp_src(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_TCP_DST:  platform_packet_set_tcp_dst(pkt, action->field);
			break;

		case OF12_AT_SET_FIELD_UDP_SRC:  platform_packet_set_udp_src(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_UDP_DST:   platform_packet_set_udp_dst(pkt, action->field);
			break;

		case OF12_AT_SET_FIELD_ICMPV4_TYPE: platform_packet_set_icmpv4_type(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_ICMPV4_CODE: platform_packet_set_icmpv4_code(pkt, action->field);
			break;

		case OF12_AT_SET_FIELD_MPLS_LABEL: platform_packet_set_mpls_label(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_MPLS_TC: platform_packet_set_mpls_tc(pkt, action->field);
			break;

		case OF12_AT_SET_FIELD_PPPOE_CODE: platform_packet_set_pppoe_code(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_PPPOE_TYPE: platform_packet_set_pppoe_type(pkt, action->field);
			break;
		case OF12_AT_SET_FIELD_PPPOE_SID: platform_packet_set_pppoe_sid(pkt, action->field);
			break;

		case OF12_AT_SET_FIELD_PPP_PROT: platform_packet_set_ppp_proto(pkt, action->field);
			break;
		case OF12_AT_GROUP: of12_process_group_actions(sw, table_id, pkt, action->field, action->group);
			break;
		case OF12_AT_EXPERIMENTER: //FIXME: implement
			break;
		case OF12_AT_OUTPUT: 
			
			if(action->field == OF12_PORT_NORMAL){

				//Generate packet in
				platform_of12_packet_in(sw, table_id, pkt, OF12_PKT_IN_ACTION);

			}else if(action->field == OF12_PORT_TABLE || 
				 action->field == OF12_PORT_NORMAL ||
				 action->field == OF12_PORT_ANY || 
				 action->field == OF12_PORT_MAX ){	
				//Do nothing

			}else{
				//Pointer to the packet to send
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
				if(action->field == OF12_PORT_FLOOD || 
				   action->field == OF12_PORT_ALL){
					//Flood
					platform_packet_output(pkt_to_send, flood_meta_port);
				}else{
					//Single port output
					platform_packet_output(pkt_to_send, sw->logical_ports[action->field].port);
				}
				
			}
			break;
	}
}

void of12_process_apply_actions(const struct of12_switch* sw, const unsigned int table_id, datapacket_t* pkt, const of12_action_group_t* apply_actions_group, bool replicate_pkts){

	of12_packet_action_t* it;

	for(it=apply_actions_group->head;it;it=it->next){
		of12_process_packet_action(sw, table_id, pkt, it, replicate_pkts);
	}	
	of12_update_packet_matches(pkt); //TODO: evaluate wether it can be updated directly on of12_process_packet action without calling again platform methods, and evaluate the performance impact
}

/*
*
* The of12_process_write_actions is meant to encapsulate the processing of the write actions
*
*/
void of12_process_write_actions(const struct of12_switch* sw, const unsigned int table_id, datapacket_t* pkt, bool replicate_pkts){

	unsigned int i;
	of12_write_actions_t* packet_write_actions;

	//Recover write actions from datapacket
	packet_write_actions = (of12_write_actions_t*)pkt->write_actions;
	
	for(i=0;i<OF12_AT_NUMBER;i++){
		if(packet_write_actions->write_actions[i].type){
			of12_process_packet_action(sw, table_id, pkt, &packet_write_actions->write_actions[i], replicate_pkts);
		}
	}
}

//Update apply/write
rofl_result_t of12_update_apply_actions(of12_action_group_t** group, of12_action_group_t* new_group){

	of12_action_group_t* old_group = *group;

	//Transfer
	*group = new_group;

	//Release if necessary
	if(old_group)
		of12_destroy_action_group(old_group);	

	return ROFL_SUCCESS;
}

rofl_result_t of12_update_write_actions(of12_write_actions_t** group, of12_write_actions_t* new_group){

	of12_write_actions_t* old_group = *group;
	
	//Transfer
	*group = new_group;
	
	//Destroy old group	
	if(old_group)
		of12_destroy_write_actions(old_group);
	
	return ROFL_SUCCESS;
}

static
void of12_process_group_actions(const struct of12_switch* sw, const unsigned int table_id, datapacket_t *pkt,uint64_t field, of12_group_t *group){
	of12_bucket_t *it_bk;
	of12_packet_matches_t *matches = (of12_packet_matches_t *) pkt->matches;
	
	//process the actions in the buckets depending on the type
	switch(group->type){
		case OF12_GROUP_TYPE_ALL:
			//executes all buckets
			platform_rwlock_rdlock(group->rwlock);
			for (it_bk = group->bc_list->head; it_bk!=NULL;it_bk = it_bk->next){
				//process all actions in the bucket
				of12_process_apply_actions(sw,table_id,pkt,it_bk->actions, it_bk->actions->num_of_output_actions > 1);
				of12_stats_bucket_update(&it_bk->stats, matches->pkt_size_bytes);
			}
			platform_rwlock_rdunlock(group->rwlock);
			break;
		case OF12_GROUP_TYPE_SELECT:
			//optional
			//fprintf(stderr,"<%s:%d> Group type not implemented",__func__,__LINE__);
			break;
		case OF12_GROUP_TYPE_INDIRECT:
			//executes the "one bucket defined"
			platform_rwlock_rdlock(group->rwlock);
			of12_process_apply_actions(sw,table_id,pkt,group->bc_list->head->actions, group->bc_list->head->actions->num_of_output_actions > 1);
			of12_stats_bucket_update(&group->bc_list->head->stats, matches->pkt_size_bytes);
			platform_rwlock_rdunlock(group->rwlock);
			break;
		case OF12_GROUP_TYPE_FF:
			//optional
			//fprintf(stderr,"<%s:%d> Group type not implemented",__func__,__LINE__);
			break;
		default:
			break;
	}
	of12_stats_group_update(&group->stats, matches->pkt_size_bytes);
	
}
//Checking functions
bool of12_write_actions_has(of12_write_actions_t* write_actions, of12_packet_action_type_t type, uint64_t value){
	if(!write_actions)
		return false;	
	
	of12_packet_action_t action = write_actions->write_actions[type];

	return (action.type != OF12_AT_NO_ACTION) && (value != 0x0 && action.field == value ); 
}

bool of12_apply_actions_has(const of12_action_group_t* apply_actions_group, of12_packet_action_type_t type, uint64_t value){

	of12_packet_action_t *it;

	if(!apply_actions_group)
		return false;	


	for(it=apply_actions_group->head; it; it=it->next){
		if( (it->type == type) && (value != 0x0 && it->field == value) )
			return true;
	}
	return false;	
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

bool of12_validate_action_group(of12_action_group_t *ag){
	//TODO we need to validate the actions here!
	return true;
}

bool of12_validate_write_actions(of12_write_actions_t *wa){
	//TODO we need to validate the actions here!
	return true;
}
