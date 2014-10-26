#include "of1x_action.h"
#include "../../../common/datapacket.h"
#include "../../../common/protocol_constants.h"

#include <stdio.h>
#include <assert.h>

#include "../../../common/packet_matches.h" //TODO: evaluate if this is the best approach to update of1x_matches after actions
#include "../../../physical_switch.h"
#include "../../../util/logging.h"
#include "../../../platform/likely.h"
#include "../../../platform/memory.h"
#include "../of1x_async_events_hooks.h"
#include "of1x_flow_table.h"
#include "of1x_utils.h"

//Flood port
extern switch_port_t* flood_meta_port;

/* Actions init and destroyed */
of1x_packet_action_t* of1x_init_packet_action(of1x_packet_action_type_t type, wrap_uint_t field, uint16_t output_send_len){

	of1x_packet_action_t* action;

	if( unlikely(type==OF1X_AT_NO_ACTION) )
		return NULL;

	action = platform_malloc_shared(sizeof(of1x_packet_action_t));

	if( unlikely(action==NULL) )
		return NULL;
	
	//Set type
	action->type = type;

	//Set min max 
	action->ver_req.min_ver = OF1X_MIN_VERSION;
	action->ver_req.max_ver = OF1X_MAX_VERSION;

	//Make valgrind happy
	UINT128__T_HI(action->__field.u128) = UINT128__T_LO(action->__field.u128) = 0x0ULL;
	
	/*
	* Setting the field (for set_field actions) and fast validation flags
	*/
	switch(type){
		//16 byte
		case OF1X_AT_SET_FIELD_IPV6_ND_TARGET:
		case OF1X_AT_SET_FIELD_IPV6_SRC:
		case OF1X_AT_SET_FIELD_IPV6_DST:{
			uint128__t tmp = field.u128;
			HTONB128(tmp);
			action->__field.u128 = tmp;
			action->ver_req.min_ver = OF_VERSION_12;
		}break;

		//8 byte
		case OF1X_AT_SET_FIELD_TUNNEL_ID:
			action->__field.u64 = field.u64&OF1X_8_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_13;
			break;

		//6 byte values
		case OF1X_AT_SET_FIELD_IPV6_ND_SLL:
			field.u64 = HTONB64(OF1X_MAC_ALIGN(field.u64));
			action->__field.u64 = field.u64&OF1X_6_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_TLL:
			field.u64 = HTONB64(OF1X_MAC_ALIGN(field.u64));
			action->__field.u64 = field.u64&OF1X_6_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_ETH_DST:
			field.u64 = HTONB64(OF1X_MAC_ALIGN(field.u64));
			action->__field.u64 = field.u64&OF1X_6_BYTE_MASK;
			break;
		case OF1X_AT_SET_FIELD_ETH_SRC:
			field.u64 = HTONB64(OF1X_MAC_ALIGN(field.u64));
			action->__field.u64 = field.u64&OF1X_6_BYTE_MASK;
			break;
		case OF1X_AT_SET_FIELD_ARP_SHA:
			field.u64 = HTONB64(OF1X_MAC_ALIGN(field.u64));
			action->__field.u64 = field.u64&OF1X_6_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_ARP_THA:
			field.u64 = HTONB64(OF1X_MAC_ALIGN(field.u64));
			action->__field.u64 = field.u64&OF1X_6_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		/* Extensions */
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_1:
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_2:
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_3:
			field.u64 = HTONB64(OF1X_MAC_ALIGN(field.u64));
			action->__field.u64 = field.u64&OF1X_6_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		/* Extensions end */
	
		//4 byte values
		case OF1X_AT_SET_FIELD_NW_DST:
			action->ver_req.min_ver = OF_VERSION_10;
			action->ver_req.max_ver = OF_VERSION_10;
		case OF1X_AT_SET_FIELD_IPV4_DST:
			field.u32 = HTONB32(field.u32);
			action->__field.u32 = field.u32&OF1X_4_BYTE_MASK;
			break;
		case OF1X_AT_SET_FIELD_NW_SRC:
			action->ver_req.min_ver = OF_VERSION_10;
			action->ver_req.max_ver = OF_VERSION_10;
		case OF1X_AT_SET_FIELD_IPV4_SRC:
			field.u32 = HTONB32(field.u32);
			action->__field.u32 = field.u32&OF1X_4_BYTE_MASK;
			break;
		case OF1X_AT_SET_FIELD_ARP_SPA:
			field.u32 = HTONB32(field.u32);
			action->__field.u32 = field.u32&OF1X_4_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_10;
			break;
		case OF1X_AT_SET_FIELD_ARP_TPA:
			field.u32 = HTONB32(field.u32);
			action->__field.u32 = field.u32&OF1X_4_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_10;
			break;
		case OF1X_AT_OUTPUT:
			action->send_len = output_send_len;
			action->__field.u32 = field.u32&OF1X_4_BYTE_MASK;
			break;
		/* Extensions */
		case OF1X_AT_SET_FIELD_GTP_TEID:
			field.u32 = HTONB32(field.u32);
			action->__field.u32 = field.u32&OF1X_4_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_GRE_KEY:
			field.u32 = HTONB32(field.u32);
			action->__field.u32 = field.u32&OF1X_4_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		/* Extensions end */

		//3 byte
		case OF1X_AT_SET_FIELD_PBB_ISID:
			//TODO Align value
			action->__field.u32 = field.u32&OF1X_3_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_13;
			break;
	
		//20 bit values
		case OF1X_AT_SET_FIELD_IPV6_FLABEL:
			field.u32 = HTONB32(OF1X_IP6_FLABEL_ALIGN(field.u32));
			action->__field.u32 = field.u32&OF1X_20_BITS_IPV6_FLABEL_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_MPLS_LABEL:
			field.u32 = HTONB32(OF1X_MPLS_LABEL_ALIGN(field.u32));
			action->__field.u32 = field.u32&OF1X_20_BITS_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;

		//2 byte values
		case OF1X_AT_SET_FIELD_ETH_TYPE:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			break;
		case OF1X_AT_SET_FIELD_ARP_OPCODE:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK; // TODO: lower 8bits of opcode only?
			action->ver_req.min_ver = OF_VERSION_10;
			break;
		case OF1X_AT_SET_FIELD_TP_SRC:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_10;
			action->ver_req.max_ver = OF_VERSION_10;
			break;
		case OF1X_AT_SET_FIELD_TP_DST:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_10;
			action->ver_req.max_ver = OF_VERSION_10;
			break;
		case OF1X_AT_SET_FIELD_TCP_SRC:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_TCP_DST:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_UDP_SRC:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_UDP_DST:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_SCTP_SRC:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_SCTP_DST:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		/* Extensions */
		case OF1X_AT_SET_FIELD_PPPOE_SID:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_PPP_PROT:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_CAPWAP_FLAGS:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_WLAN_FC:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_GRE_VERSION:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_FIELD_GRE_PROT_TYPE:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		/* Extensions end */
		/*case OF1X_AT_POP_VLAN: TODO: CHECK THIS*/
		case OF1X_AT_POP_MPLS:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_POP_PPPOE:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_POP_PBB:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_13;
			break;
		/* Extensions */
		case OF1X_AT_POP_GRE:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_POP_WLAN:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_POP_GTP:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_POP_CAPWAP:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_PUSH_GTP:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_PUSH_CAPWAP:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_PUSH_WLAN:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_PUSH_GRE:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		/* Extensions end */
		case OF1X_AT_PUSH_PPPOE:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_PUSH_MPLS:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_PUSH_VLAN:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_PUSH_PBB:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_2_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_13;
			break;


		//12 bit values
		case OF1X_AT_SET_FIELD_VLAN_VID:
			field.u16 = HTONB16(field.u16);
			action->__field.u16 = field.u16&OF1X_12_BITS_MASK;
			break;


		//9 bit value
		case OF1X_AT_SET_FIELD_IPV6_EXTHDR:
			//TODO align to pipeline -- currently not implemented
			action->__field.u16 = field.u16&OF1X_9_BITS_MASK;
			action->ver_req.min_ver = OF_VERSION_13;
			break;

		//1 byte values
		case OF1X_AT_SET_FIELD_NW_PROTO:
			action->ver_req.min_ver = OF_VERSION_10;
			action->ver_req.max_ver = OF_VERSION_10;
			action->__field.u8 = field.u8&OF1X_1_BYTE_MASK;
			break;
		case OF1X_AT_SET_FIELD_ICMPV6_TYPE:
		case OF1X_AT_SET_FIELD_ICMPV6_CODE:
		case OF1X_AT_SET_FIELD_PPPOE_CODE:
		case OF1X_AT_SET_FIELD_PPPOE_TYPE:
		case OF1X_AT_SET_MPLS_TTL:
		case OF1X_AT_SET_NW_TTL:
		case OF1X_AT_SET_FIELD_IP_PROTO:
		case OF1X_AT_SET_FIELD_ICMPV4_TYPE:
		case OF1X_AT_SET_FIELD_ICMPV4_CODE:
			action->__field.u8 = field.u8&OF1X_1_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_10;
			break;
		/* Extensions */
		case OF1X_AT_SET_FIELD_GTP_MSG_TYPE:
		case OF1X_AT_SET_FIELD_CAPWAP_WBID:
		case OF1X_AT_SET_FIELD_CAPWAP_RID:
		case OF1X_AT_SET_FIELD_WLAN_TYPE:
		case OF1X_AT_SET_FIELD_WLAN_SUBTYPE:
		case OF1X_AT_SET_FIELD_WLAN_DIRECTION:
		/* Extensions end */
			action->__field.u8 = field.u8&OF1X_1_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;

		//6 bit values
		case OF1X_AT_SET_FIELD_IP_DSCP:
			field.u8 = OF1X_IP_DSCP_ALIGN(field.u8);
			action->__field.u8 = field.u8&OF1X_6MSBITS_MASK;
			break;

		//3 bit values
		case OF1X_AT_SET_FIELD_VLAN_PCP:
			field.u8 = OF1X_VLAN_PCP_ALIGN(field.u8);
			action->__field.u8 = field.u8&OF1X_3MSBITS_MASK;
			break;

		case OF1X_AT_SET_FIELD_MPLS_TC:
			field.u8 = OF1X_MPLS_TC_ALIGN(field.u8);
			action->__field.u8 = field.u8&OF1X_BITS_12AND3_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;

		//2 bit values
		case OF1X_AT_SET_FIELD_IP_ECN:
			action->__field.u8 = field.u8&OF1X_2LSBITS_MASK;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		case OF1X_AT_SET_QUEUE:
			action->__field.u32 =  field.u32&OF1X_4_BYTE_MASK;
			action->ver_req.min_ver = OF_VERSION_10;
			break;
		case OF1X_AT_GROUP:
			action->__field.u32 =  field.u32&OF1X_4_BYTE_MASK; //id of the group
			action->ver_req.min_ver = OF_VERSION_12;
			//action->group = of1x_group_search(sw->pipeline->groups, action->__field); // pointer to the group //FIXME evaluate if this can be done here or not
			break;

		//1 bit values
		case OF1X_AT_SET_FIELD_MPLS_BOS:
			action->__field.u8 =  field.u8&OF1X_BIT0_MASK; //id of the group
			action->ver_req.min_ver = OF_VERSION_13;
			break;

		//No value
		case OF1X_AT_POP_VLAN:
			action->__field.u64 = 0x0; // action strip vlan
			break;

		case OF1X_AT_COPY_TTL_IN:
		case OF1X_AT_COPY_TTL_OUT:
		case OF1X_AT_DEC_NW_TTL:
		case OF1X_AT_DEC_MPLS_TTL:
		case OF1X_AT_EXPERIMENTER:
			action->__field.u64 = 0x0;
			action->ver_req.min_ver = OF_VERSION_12;
			break;
		
		//Shall never happen
		case OF1X_AT_NO_ACTION: 
			assert(0);
			action->__field.u64 = 0x0;
			break;
	}
	
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

	if( unlikely(action_group==NULL) )
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
	bitmap128_clean(&action_group->bitmap);
	action_group->has_output_table = false;

	return action_group;
}

void of1x_destroy_action_group(of1x_action_group_t* group){

	of1x_packet_action_t* it,*next;

	if( unlikely(group==NULL) )
		return;

	for(it=group->head;it;it=next){
		next = it->next; 
		of1x_destroy_packet_action(it);
	}
	platform_free_shared(group);	
}

/* Addition of an action to an action group */
void of1x_push_packet_action_to_group(of1x_action_group_t* group, of1x_packet_action_t* action){

	if( unlikely(action==NULL) || action->type >= OF1X_AT_NUMBER ){
		assert(0);
		return;
	}

	//Set has_output_table for fast validation
	if(action->type == OF1X_AT_OUTPUT && action->__field.u32 == OF1X_PORT_TABLE){
		//We only accept an output action to PORT_TABLE
		if(unlikely(group->has_output_table == true)){
			ROFL_PIPELINE_ERR("Trying to add an action OUTPUT to port TABLE to action group(%p) when there is already one. Ignoring...", group);
			assert(0);
			return;
		}
		group->has_output_table = true; 
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

	//This cannot be done here, because the group might NOT exist yet; the sum will happen 
	//during insertion validation (for both action and WRITE_ACTIONS instruction
	//if(action->type == OF1X_AT_OUTPUT)
	//	group->num_of_output_actions++;
	
	//if(action->type == OF1X_AT_GROUP)
	//	group->num_of_output_actions+=action->group->num_of_output_actions;

	//Update fast validation flags (required versions)
	if(group->ver_req.min_ver < action->ver_req.min_ver)
		group->ver_req.min_ver = action->ver_req.min_ver;
	if(group->ver_req.max_ver > action->ver_req.max_ver)
		group->ver_req.max_ver = action->ver_req.max_ver;
	bitmap128_set(&group->bitmap, action->type);

}

of1x_write_actions_t* of1x_init_write_actions(){

	int i;
	of1x_write_actions_t* write_actions = platform_malloc_shared(sizeof(of1x_write_actions_t)); 

	if( unlikely(write_actions==NULL) )
		return NULL;

	//Clean actions bitmap
	bitmap128_clean(&write_actions->bitmap);

	for(i=0;i<OF1X_AT_NUMBER;i++)
		write_actions->actions[i].type = (of1x_packet_action_type_t)i;	
	
	//num of actions and output actions
	write_actions->num_of_actions = 0;
	write_actions->num_of_output_actions = 0;

	//Fast validation, set min max 
	write_actions->ver_req.min_ver = OF1X_MIN_VERSION;
	write_actions->ver_req.max_ver = OF1X_MAX_VERSION;

	return write_actions;
}

void __of1x_destroy_write_actions(of1x_write_actions_t* write_actions){
	platform_free_shared(write_actions);	
}

rofl_result_t of1x_set_packet_action_on_write_actions(of1x_write_actions_t* write_actions, of1x_packet_action_t* action){

	if( unlikely(write_actions==NULL) || action->type >= OF1X_AT_NUMBER ){
		assert(0);
		return ROFL_FAILURE;
	}
	
	//Ignore write action output OF1X_PORT_TABLE
	if(action->type == OF1X_AT_OUTPUT && action->__field.u32 == OF1X_PORT_TABLE)
		return ROFL_FAILURE;

	//Update field
	write_actions->actions[action->type].__field = action->__field;
	write_actions->actions[action->type].send_len = action->send_len;

	if( !bitmap128_is_bit_set(&write_actions->bitmap, action->type) ){
		write_actions->num_of_actions++;
		bitmap128_set(&write_actions->bitmap, action->type);
	}

	//This cannot be done here, because the group might NOT exist yet; the sum will happen 
	//during insertion validation (for both action and WRITE_ACTIONS instruction
	//if (action->type == OF1X_AT_OUTPUT)
	//	write_actions->num_of_output_actions++;
	//if(action->type == OF1X_AT_GROUP)
	//	write_actions->num_of_output_actions+=action->group->num_of_output_actions;
	
	//Update fast validation flags (required versions)
	if(write_actions->ver_req.min_ver < action->ver_req.min_ver)
		write_actions->ver_req.min_ver = action->ver_req.min_ver;
	if(write_actions->ver_req.max_ver > action->ver_req.max_ver)
		write_actions->ver_req.max_ver = action->ver_req.max_ver;
		
	return ROFL_SUCCESS;
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

//Checking functions
/*TODO specific funcions for 128 bits. So far only used for OUTPUT and GROUP actions, so not really necessary*/
bool __of1x_write_actions_has(of1x_write_actions_t* write_actions, of1x_packet_action_type_t type, uint64_t value){
	if( unlikely(write_actions==NULL))
		return false;	

	return bitmap128_is_bit_set(&write_actions->bitmap, type);
}
/*TODO specific funcions for 128 bits. So far only used for OUTPUT and GROUP actions, so not really necessary*/
bool __of1x_apply_actions_has(const of1x_action_group_t* apply_actions_group, of1x_packet_action_type_t type, uint64_t value){

	of1x_packet_action_t *it;

	if( unlikely(apply_actions_group==NULL) )
		return false;	

	for(it=apply_actions_group->head; it; it=it->next){
		
		if(type != OF1X_AT_GROUP){
			//Filter types where field cannot be 0
			return (it->type == type) &&  (value != 0x0 && it->__field.u64 == value);
		}else{
			//Groups or anything else
			return (it->type == type) &&  (it->__field.u64 == value);
		}
	}
	return false;	
}

//Copy (cloning) methods
of1x_packet_action_t* __of1x_copy_packet_action(of1x_packet_action_t* action){

	of1x_packet_action_t* copy;

	copy = platform_malloc_shared(sizeof(of1x_packet_action_t));

	if( unlikely(copy==NULL) )
		return NULL;

	*copy = *action;

	return copy;
}

of1x_action_group_t* __of1x_copy_action_group(of1x_action_group_t* origin){

	of1x_action_group_t* copy;
	of1x_packet_action_t* it;	

	if( unlikely(origin==NULL) )
		return NULL;

	copy = platform_malloc_shared(sizeof(of1x_action_group_t));


	if( unlikely(copy==NULL) )
		return NULL;

	copy->head = copy->tail = NULL;
	copy->num_of_actions = origin->num_of_actions;
	copy->num_of_output_actions = origin->num_of_output_actions;

	//Copy al apply actions
	for(it=origin->head;it;it=it->next){
		of1x_packet_action_t* act;
		
		act = __of1x_copy_packet_action(it);

		if(unlikely(act == NULL)){
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

	if( unlikely(origin==NULL) )
		return NULL;

	copy = platform_malloc_shared(sizeof(of1x_write_actions_t)); 

	if( unlikely(copy==NULL) )
		return NULL;
	
	//Copy Values
	*copy = *origin;
	
	return copy;
}

/* Dumping */
static void __of1x_dump_packet_action(of1x_packet_action_t* action, bool raw_nbo){

	ROFL_PIPELINE_INFO_NO_PREFIX("<");
	switch(action->type){
		case OF1X_AT_NO_ACTION: /*TODO: print some error traces? */
			break;

		case OF1X_AT_COPY_TTL_IN: ROFL_PIPELINE_INFO_NO_PREFIX("TTL_IN");
			break;

		case OF1X_AT_POP_VLAN:ROFL_PIPELINE_INFO_NO_PREFIX("POP_VLAN");
			break;
		case OF1X_AT_POP_MPLS:ROFL_PIPELINE_INFO_NO_PREFIX("POP_MPLS");
			break;
		case OF1X_AT_POP_PPPOE:ROFL_PIPELINE_INFO_NO_PREFIX("POP_PPPOE");
			break;

		case OF1X_AT_PUSH_PPPOE:ROFL_PIPELINE_INFO_NO_PREFIX("PUSH_PPPOE");
			break;
		case OF1X_AT_PUSH_MPLS:ROFL_PIPELINE_INFO_NO_PREFIX("PUSH_MPLS");
			break;
		case OF1X_AT_PUSH_VLAN:ROFL_PIPELINE_INFO_NO_PREFIX("PUSH_VLAN");
			break;

		case OF1X_AT_COPY_TTL_OUT:ROFL_PIPELINE_INFO_NO_PREFIX("COPY_TTL_OUT");
			break;

		case OF1X_AT_DEC_NW_TTL:ROFL_PIPELINE_INFO_NO_PREFIX("DEC_NW_TTL");
			break;
		case OF1X_AT_DEC_MPLS_TTL:ROFL_PIPELINE_INFO_NO_PREFIX("DEC_MPLS_TTL");
			break;

		case OF1X_AT_SET_MPLS_TTL:ROFL_PIPELINE_INFO_NO_PREFIX("SET_MPLS_TTL: %u", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_NW_TTL:ROFL_PIPELINE_INFO_NO_PREFIX("SET_NW_TTL: %u", __of1x_get_packet_action_field8(action, raw_nbo));
			break;

		case OF1X_AT_SET_QUEUE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_QUEUE: %u", __of1x_get_packet_action_field32(action, raw_nbo));
			break;

		case OF1X_AT_SET_FIELD_ETH_DST: ROFL_PIPELINE_INFO_NO_PREFIX("SET_ETH_DST: 0x%"PRIx64, __of1x_get_packet_action_field64(action, raw_nbo)); 
			break;
		case OF1X_AT_SET_FIELD_ETH_SRC: ROFL_PIPELINE_INFO_NO_PREFIX("SET_ETH_SRC: 0x%"PRIx64, __of1x_get_packet_action_field64(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_ETH_TYPE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_ETH_TYPE: 0x%x", __of1x_get_packet_action_field16(action, raw_nbo));
			break;

		case OF1X_AT_SET_FIELD_VLAN_VID:ROFL_PIPELINE_INFO_NO_PREFIX("SET_VLAN_VID: 0x%x", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_VLAN_PCP:ROFL_PIPELINE_INFO_NO_PREFIX("SET_VLAN_PCP: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;

		case OF1X_AT_SET_FIELD_ARP_OPCODE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_ARP_OPCODE: 0x%x", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_ARP_SHA: ROFL_PIPELINE_INFO_NO_PREFIX("SET_ARP_SHA: 0x%"PRIx64, __of1x_get_packet_action_field64(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_ARP_SPA: ROFL_PIPELINE_INFO_NO_PREFIX("SET_ARP_SPA: 0x%x", __of1x_get_packet_action_field32(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_ARP_THA: ROFL_PIPELINE_INFO_NO_PREFIX("SET_ARP_THA: 0x%"PRIx64, __of1x_get_packet_action_field64(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_ARP_TPA: ROFL_PIPELINE_INFO_NO_PREFIX("SET_ARP_TPA: 0x%x", __of1x_get_packet_action_field32(action, raw_nbo));
			break;

		/* OF1.0 only */
		case OF1X_AT_SET_FIELD_NW_PROTO: ROFL_PIPELINE_INFO_NO_PREFIX("SET_NW_PROTO: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_NW_SRC: ROFL_PIPELINE_INFO_NO_PREFIX("SET_NW_SRC: 0x%x", __of1x_get_packet_action_field32(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_NW_DST: ROFL_PIPELINE_INFO_NO_PREFIX("SET_NW_DST: 0x%x", __of1x_get_packet_action_field32(action, raw_nbo));
			break;
		/* OF1.0 only */

		case OF1X_AT_SET_FIELD_IP_DSCP: ROFL_PIPELINE_INFO_NO_PREFIX("SET_IP_DSCP: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_IP_ECN:  ROFL_PIPELINE_INFO_NO_PREFIX("SET_IP_ECN: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;

		case OF1X_AT_SET_FIELD_IP_PROTO:ROFL_PIPELINE_INFO_NO_PREFIX("SET_IP_PROTO: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;

		case OF1X_AT_SET_FIELD_IPV4_SRC:ROFL_PIPELINE_INFO_NO_PREFIX("SET_IPV4_SRC: 0x%x", __of1x_get_packet_action_field32(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_IPV4_DST:ROFL_PIPELINE_INFO_NO_PREFIX("SET_IPV4_DST: 0x%x", __of1x_get_packet_action_field32(action, raw_nbo));
			break;

		/* OF1.0 only */
		case OF1X_AT_SET_FIELD_TP_SRC: ROFL_PIPELINE_INFO_NO_PREFIX("SET_TP_SRC: %u", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_TP_DST: ROFL_PIPELINE_INFO_NO_PREFIX("SET_TP_DST: %u", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		/* OF1.0 only */
		case OF1X_AT_SET_FIELD_TCP_SRC: ROFL_PIPELINE_INFO_NO_PREFIX("SET_TCP_SRC: %u", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_TCP_DST: ROFL_PIPELINE_INFO_NO_PREFIX("SET_TCP_DST: %u", __of1x_get_packet_action_field16(action, raw_nbo));
			break;

		case OF1X_AT_SET_FIELD_UDP_SRC: ROFL_PIPELINE_INFO_NO_PREFIX("SET_UDP_SRC: %u", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_UDP_DST:  ROFL_PIPELINE_INFO_NO_PREFIX("SET_UDP_DST: %u", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_SCTP_SRC: ROFL_PIPELINE_INFO_NO_PREFIX("SET_SCTP_SRC: %u", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_SCTP_DST:  ROFL_PIPELINE_INFO_NO_PREFIX("SET_SCTP_DST: %u", __of1x_get_packet_action_field16(action, raw_nbo));
			break;

		case OF1X_AT_SET_FIELD_ICMPV4_TYPE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_ICMPV4_TYPE: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_ICMPV4_CODE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_ICMPV4_CODE: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;

		case OF1X_AT_SET_FIELD_MPLS_LABEL:ROFL_PIPELINE_INFO_NO_PREFIX("SET_MPLS_LABEL: 0x%x", __of1x_get_packet_action_field32(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_MPLS_TC:ROFL_PIPELINE_INFO_NO_PREFIX("SET_MPLS_TC: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_MPLS_BOS:ROFL_PIPELINE_INFO_NO_PREFIX("SET_MPLS_BOS: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;

		case OF1X_AT_SET_FIELD_PPPOE_CODE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_PPPOE_CODE: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_PPPOE_TYPE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_PPPOE_TYPE: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_PPPOE_SID:ROFL_PIPELINE_INFO_NO_PREFIX("SET_PPPOE_SID: 0x%x", __of1x_get_packet_action_field16(action, raw_nbo));
			break;

		case OF1X_AT_SET_FIELD_PPP_PROT:ROFL_PIPELINE_INFO_NO_PREFIX("SET_PPP_PROT: 0x%x", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
			
		case OF1X_AT_SET_FIELD_IPV6_SRC:
			{
				uint128__t addr = __of1x_get_packet_action_field128(action, raw_nbo);
				(void)addr;
				COND_NTOHB128(raw_nbo,addr);
				ROFL_PIPELINE_INFO_NO_PREFIX("SET_IPV6_SRC: 0x%lx %lx",UINT128__T_HI(addr),UINT128__T_LO(addr));
			}
			break;
		case OF1X_AT_SET_FIELD_IPV6_DST:
			{
				uint128__t addr = __of1x_get_packet_action_field128(action, raw_nbo);
				(void)addr;
				COND_NTOHB128(raw_nbo,addr);
				ROFL_PIPELINE_INFO_NO_PREFIX("SET_IPV6_DST: 0x%lx %lx",UINT128__T_HI(addr),UINT128__T_LO(addr));
			}
			break;
		case OF1X_AT_SET_FIELD_IPV6_FLABEL:ROFL_PIPELINE_INFO_NO_PREFIX("SET_IPV6_FLABEL: 0x%u", __of1x_get_packet_action_field32(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_TARGET:
			{
				uint128__t addr = __of1x_get_packet_action_field128(action, raw_nbo);
				(void)addr;
				COND_NTOHB128(raw_nbo,addr);
				ROFL_PIPELINE_INFO_NO_PREFIX("SET_IPV6_ND_TARGET: 0x%lx %lx",UINT128__T_HI(addr),UINT128__T_LO(addr));
			}
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_SLL:ROFL_PIPELINE_INFO_NO_PREFIX("SET_IPV6_ND_SLL: 0x%x", __of1x_get_packet_action_field64(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_TLL:ROFL_PIPELINE_INFO_NO_PREFIX("SET_IPV6_ND_TLL: 0x%x", __of1x_get_packet_action_field64(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_IPV6_EXTHDR:ROFL_PIPELINE_INFO_NO_PREFIX("SET_IPV6_EXTHDR: 0x%x", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
			
		case OF1X_AT_SET_FIELD_ICMPV6_TYPE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_ICMPV6_TYPE: 0x%u", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_ICMPV6_CODE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_ICMPV6_CODE: 0x%u", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_POP_PBB:ROFL_PIPELINE_INFO_NO_PREFIX("POP_PBB");
			break;
		case OF1X_AT_PUSH_PBB:ROFL_PIPELINE_INFO_NO_PREFIX("PUSH_PBB");
			break;
		case OF1X_AT_SET_FIELD_PBB_ISID:ROFL_PIPELINE_INFO_NO_PREFIX("SET_PBB_ISID: 0x%u", __of1x_get_packet_action_field64(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_TUNNEL_ID:ROFL_PIPELINE_INFO_NO_PREFIX("SET_TUNNEL_ID: 0x%u", __of1x_get_packet_action_field64(action, raw_nbo));
			break;
		

		case OF1X_AT_SET_FIELD_GTP_MSG_TYPE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_GTP_MSG_TYPE: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_GTP_TEID:ROFL_PIPELINE_INFO_NO_PREFIX("SET_GTP_TEID: 0x%x", __of1x_get_packet_action_field32(action, raw_nbo));
			break;
		case OF1X_AT_POP_GTP:ROFL_PIPELINE_INFO_NO_PREFIX("POP_GTP");
			break;
		case OF1X_AT_PUSH_GTP:ROFL_PIPELINE_INFO_NO_PREFIX("PUSH_GTP");
			break;

		case OF1X_AT_SET_FIELD_CAPWAP_WBID:ROFL_PIPELINE_INFO_NO_PREFIX("SET_CAPWAP_WBID: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_CAPWAP_RID:ROFL_PIPELINE_INFO_NO_PREFIX("SET_CAPWAP_RID: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_CAPWAP_FLAGS:ROFL_PIPELINE_INFO_NO_PREFIX("SET_CAPWAP_FLAGS: 0x%x", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		case OF1X_AT_POP_CAPWAP:ROFL_PIPELINE_INFO_NO_PREFIX("POP_CAPWAP");
			break;
		case OF1X_AT_PUSH_CAPWAP:ROFL_PIPELINE_INFO_NO_PREFIX("PUSH_CAPWAP");
			break;

		case OF1X_AT_SET_FIELD_WLAN_FC:ROFL_PIPELINE_INFO_NO_PREFIX("SET_WLAN_FC: 0x%x", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_WLAN_TYPE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_WLAN_TYPE: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_WLAN_SUBTYPE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_WLAN_SUBTYPE: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_WLAN_DIRECTION:ROFL_PIPELINE_INFO_NO_PREFIX("SET_WLAN_DIRECTION: 0x%x", __of1x_get_packet_action_field8(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_1:ROFL_PIPELINE_INFO_NO_PREFIX("SET_WLAN_ADDRESS_1: 0x%x", __of1x_get_packet_action_field64(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_2:ROFL_PIPELINE_INFO_NO_PREFIX("SET_WLAN_ADDRESS_2: 0x%x", __of1x_get_packet_action_field64(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_WLAN_ADDRESS_3:ROFL_PIPELINE_INFO_NO_PREFIX("SET_WLAN_ADDRESS_3: 0x%x", __of1x_get_packet_action_field64(action, raw_nbo));
			break;
		case OF1X_AT_POP_WLAN:ROFL_PIPELINE_INFO_NO_PREFIX("POP_WLAN");
			break;
		case OF1X_AT_PUSH_WLAN:ROFL_PIPELINE_INFO_NO_PREFIX("PUSH_WLAN");
			break;

		case OF1X_AT_SET_FIELD_GRE_VERSION:ROFL_PIPELINE_INFO_NO_PREFIX("SET_GRE_VERSION: 0x%x", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_GRE_PROT_TYPE:ROFL_PIPELINE_INFO_NO_PREFIX("SET_GRE_PROT_TYPE: 0x%x", __of1x_get_packet_action_field16(action, raw_nbo));
			break;
		case OF1X_AT_SET_FIELD_GRE_KEY:ROFL_PIPELINE_INFO_NO_PREFIX("SET_GRE_KEY: 0x%x", __of1x_get_packet_action_field32(action, raw_nbo));
			break;
		case OF1X_AT_POP_GRE:ROFL_PIPELINE_INFO_NO_PREFIX("POP_GRE");
			break;
		case OF1X_AT_PUSH_GRE:ROFL_PIPELINE_INFO_NO_PREFIX("PUSH_GRE");
			break;

		case OF1X_AT_GROUP:ROFL_PIPELINE_INFO_NO_PREFIX("GROUP:%u", __of1x_get_packet_action_field32(action, raw_nbo));
			break;

		case OF1X_AT_EXPERIMENTER:ROFL_PIPELINE_INFO_NO_PREFIX("EXPERIMENTER");
			break;

		case OF1X_AT_OUTPUT:
				ROFL_PIPELINE_INFO_NO_PREFIX("OUTPUT port: ");
				switch(__of1x_get_packet_action_field32(action, raw_nbo)){
	
					case OF1X_PORT_FLOOD:
						ROFL_PIPELINE_INFO_NO_PREFIX("FLOOD");
						break;
					case OF1X_PORT_NORMAL:
						ROFL_PIPELINE_INFO_NO_PREFIX("NORMAL");
						break;	
					case OF1X_PORT_CONTROLLER:
						ROFL_PIPELINE_INFO_NO_PREFIX("CONTROLLER");
						break;	
					case OF1X_PORT_ALL:
						ROFL_PIPELINE_INFO_NO_PREFIX("ALL");
						break;	
					case OF1X_PORT_TABLE:
						ROFL_PIPELINE_INFO_NO_PREFIX("TABLE");
						break;	
					case OF1X_PORT_IN_PORT:
						ROFL_PIPELINE_INFO_NO_PREFIX("IN-PORT");
						break;	
					default:
						ROFL_PIPELINE_INFO_NO_PREFIX("%u", __of1x_get_packet_action_field32(action, raw_nbo));
						break;
				}
			break;
	}
	ROFL_PIPELINE_INFO_NO_PREFIX(">,");

}

void __of1x_dump_write_actions(of1x_write_actions_t* write_actions, bool raw_nbo){

	int i,j;

	for(i=0,j=0;i<write_actions->num_of_actions && j < OF1X_AT_NUMBER;j++){
		if( bitmap128_is_bit_set(&write_actions->bitmap, j) ){
			//Process action
			__of1x_dump_packet_action(&write_actions->actions[j], raw_nbo);
			i++;		
		}
	}
}
	
void __of1x_dump_action_group(of1x_action_group_t* action_group, bool raw_nbo){

	of1x_packet_action_t* action;

	if( unlikely(action_group==NULL) )
		return;
	for(action=action_group->head;action;action=action->next){
		__of1x_dump_packet_action(action, raw_nbo);
	}
}

rofl_result_t __of1x_validate_action_group(bitmap128_t* supported, of1x_action_group_t *ag, of1x_group_table_t *gt, bool is_pkt_out_al){
	of1x_packet_action_t *pa_it;

	if(unlikely(ag == NULL))
		return ROFL_FAILURE;

	//Check supported bitmap (if defined). Won't be defined for packet_outs
	if( supported && !bitmap128_check_mask(&ag->bitmap, supported) )
		return ROFL_FAILURE;

	for(pa_it=ag->head; pa_it; pa_it=pa_it->next){
		if(pa_it->type == OF1X_AT_OUTPUT)
			ag->num_of_output_actions++;
		else if(pa_it->type == OF1X_AT_GROUP && gt){
			if((pa_it->group=__of1x_group_search(gt,pa_it->__field.u64))==NULL)
				return ROFL_FAILURE;
			else{	
				//If there is a group, FORCE cloning of the packet; state between
				//group num of actions and entry "num_of_output_actions cache"
				ag->num_of_output_actions+=2;
			}
		}
	}

	//Only pkt_out action lists can have output to table
	if(!is_pkt_out_al && ag->has_output_table)
		return ROFL_FAILURE;
	
	return ROFL_SUCCESS;
}

rofl_result_t __of1x_validate_write_actions(bitmap128_t* supported, of1x_write_actions_t *wa, of1x_group_table_t *gt){

	if(unlikely(wa == NULL))
		return ROFL_FAILURE;

	if(wa->num_of_actions == 0)
		return ROFL_SUCCESS;

	if( !bitmap128_check_mask(&wa->bitmap, supported) )
		return ROFL_FAILURE;
	
	if( bitmap128_is_bit_set(&wa->bitmap, OF1X_AT_OUTPUT) )
		wa->num_of_output_actions++;
	
	if(gt && bitmap128_is_bit_set(&wa->bitmap, OF1X_AT_GROUP)){
		if((wa->actions[OF1X_AT_GROUP].group=__of1x_group_search(gt,wa->actions[OF1X_AT_GROUP].__field.u64))==NULL )
			return ROFL_FAILURE;
		else{
			//If there is a group, FORCE cloning of the packet; state between
			//group num of actions and entry "num_of_output_actions cache"
			wa->num_of_output_actions+=2;
		}
	}
	
	return ROFL_SUCCESS;
}
