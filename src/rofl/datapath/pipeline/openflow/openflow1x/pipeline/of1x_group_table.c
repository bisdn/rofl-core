/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * A group table is a list of group entries, each one of them contain:
 * - Group ID
 * - Group type
 * - Counters
 * - Action bucket list
 * 
 * What is going on in this file?
 * groups may be added , deleted & modifyed
 * if a table has an action that says so, the actions of the bucket will be applied
 */
#include "of1x_group_table.h"
#include "of1x_pipeline.h"
#include "../../../platform/lock.h"
#include "../../../platform/memory.h"
#include "../../../platform/likely.h"
#include "../../../util/logging.h"
#include <stdio.h>

static void __of1x_destroy_group(of1x_group_table_t *gt, of1x_group_t *ge);
bool __of1x_bucket_list_has_weights(of1x_bucket_list_t *bl);

of1x_group_table_t* of1x_init_group_table(){
	of1x_group_table_t *gt;
	gt = (of1x_group_table_t *) platform_malloc_shared(sizeof(of1x_group_table_t));
	
	if( unlikely(gt==NULL) ){
		return NULL;
	}
	
	gt->num_of_entries = 0;
	gt->head = NULL;
	gt->tail = NULL;
	
	gt->rwlock = platform_rwlock_init(NULL);
	bitmap128_clean(&gt->config.supported_actions);

	//We initialize with the support of all possible actions. This can be customized via the init hook
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_COPY_TTL_IN
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_POP_VLAN);		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_POP_MPLS);		
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_POP_GTP);		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_POP_PPPOE);		
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_POP_PBB);		
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_PUSH_PBB);		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_PUSH_PPPOE);		
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_PUSH_GTP);		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_PUSH_MPLS);		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_PUSH_VLAN);		
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_COPY_TTL_OUT);			
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_DEC_NW_TTL);		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_DEC_MPLS_TTL);		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_MPLS_TTL);		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_NW_TTL);		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_QUEUE);		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ETH_DST);  		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ETH_SRC);  		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ETH_TYPE); 		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_MPLS_LABEL);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_MPLS_TC);  	   	
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_MPLS_BOS);  	   	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_VLAN_VID); 		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_VLAN_PCP); 		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ARP_OPCODE);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ARP_SHA);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ARP_SPA);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ARP_THA);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ARP_TPA);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_NW_PROTO);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_NW_SRC);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_NW_DST);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IP_DSCP);  		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IP_ECN);   		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IP_PROTO); 		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IPV4_SRC); 		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IPV4_DST); 		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IPV6_SRC);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IPV6_DST);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IPV6_FLABEL);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IPV6_ND_TARGET);
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IPV6_ND_SLL);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IPV6_ND_TLL);	
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_IPV6_EXTHDR);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_TCP_SRC);  		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_TCP_DST);  		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_UDP_SRC);  		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_UDP_DST);  		
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_SCTP_SRC);  		
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_SCTP_DST);  		
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_TP_SRC);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_TP_DST);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ICMPV4_TYPE);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ICMPV4_CODE);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ICMPV6_TYPE);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_ICMPV6_CODE);	
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_PBB_ISID);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_TUNNEL_ID);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_PPPOE_CODE);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_PPPOE_TYPE);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_PPPOE_SID);	   	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_PPP_PROT); 	   	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_GTP_MSG_TYPE);	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_SET_FIELD_GTP_TEID);	
	//bitmap128_set(&gt->config.supported_actions, OF1X_AT_GROUP); //WE DON'T SUPPORT INDIRECT GROUP reference	
	
	bitmap128_set(&gt->config.supported_actions, OF1X_AT_OUTPUT);

	return gt;
}

void of1x_destroy_group_table(of1x_group_table_t* gt){
	of1x_group_t *iterator=NULL, *next=NULL;
	//check if there are existing entries and deleting them
	
	platform_rwlock_wrlock(gt->rwlock);
	
	for(iterator=gt->head; iterator!=NULL; iterator=next){
		next=iterator->next;
		__of1x_destroy_group(gt,iterator);
	}
	
	platform_rwlock_destroy(gt->rwlock);
	
	
	platform_free_shared(gt);
}

/**
 * Copies the structure of the group table.
 */
rofl_result_t of1x_fetch_group_table(of1x_pipeline_t *pipeline, of1x_group_table_t* group_table){
	platform_rwlock_rdlock(pipeline->groups->rwlock);
	
	*group_table = *(pipeline->groups);
	
	platform_rwlock_rdunlock(pipeline->groups->rwlock);
	return ROFL_SUCCESS;
}

static
rofl_of1x_gm_result_t __of1x_validate_group(of1x_group_table_t* gt, of1x_action_group_t* actions){

	//we dont allow OF1X_AT_GROUP
	//and neither OF1X_AT_OUTPUT in the case of OF1X_PORT_TABLE
	of1x_packet_action_t *it;
	
	for(it=actions->head; it; it=it->next){
		if(it->type == OF1X_AT_GROUP)
			return ROFL_OF1X_GM_CHAIN;
		if(it->type == OF1X_AT_OUTPUT && it->__field.u64 == OF1X_PORT_TABLE)
			return ROFL_OF1X_GM_INVAL;
	}
		
	//verify apply actions
	if(__of1x_validate_action_group(&gt->config.supported_actions, actions, gt) != ROFL_SUCCESS)
		return ROFL_OF1X_GM_INVAL;
	
	return ROFL_OF1X_GM_OK;
}


/**
 * Searches in the table for an entry with a specific id
 * returns pointer if found or NULL if not
 */
of1x_group_t* __of1x_group_search(of1x_group_table_t *gt, uint32_t id){
	of1x_group_t *iterator=NULL, *next=NULL;
	
	for(iterator=gt->head; iterator!=NULL; iterator=next){
		next=iterator->next;
		if(iterator->id == id)
			return iterator;
	}
	
	return NULL;
}

rofl_of1x_gm_result_t __of1x_check_group_parameters(of1x_group_table_t *gt, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t *buckets){
	of1x_bucket_t* bu_it;
	rofl_of1x_gm_result_t ret_val;
    
	if(id == OF1X_GROUP_ALL || id == OF1X_GROUP_ANY || id > OF1X_GROUP_MAX)
		return ROFL_OF1X_GM_INVAL;
    
	//Validate action set
	for(bu_it=buckets->head;bu_it!=NULL;bu_it=bu_it->next){
		if((ret_val=__of1x_validate_group(gt, bu_it->actions))!=ROFL_OF1X_GM_OK)
			return ret_val;
	}
	
	//Group types not supported
	if (type == OF1X_GROUP_TYPE_SELECT || type == OF1X_GROUP_TYPE_FF){
		ROFL_PIPELINE_DEBUG("Warning; group type %u NOT supported\n", type);
		return ROFL_OF1X_GM_INVAL;
	}
	
	
	if(type == OF1X_GROUP_TYPE_INDIRECT && buckets->num_of_buckets>1)
		return ROFL_OF1X_GM_INVAL;
	if( (type == OF1X_GROUP_TYPE_ALL || type == OF1X_GROUP_TYPE_INDIRECT) && __of1x_bucket_list_has_weights(buckets))
		return ROFL_OF1X_GM_INVAL;
	if (type == OF1X_GROUP_TYPE_SELECT && __of1x_bucket_list_has_weights(buckets) == false)
		return ROFL_OF1X_GM_INVAL;
	
	return ROFL_OF1X_GM_OK;
}

static
rofl_of1x_gm_result_t __of1x_init_group(of1x_group_table_t *gt, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t *buckets){
							//uint32_t weigth, uint32_t group, uint32_t port, of1x_action_group_t **actions){
	rofl_of1x_gm_result_t ret_val;
	of1x_group_t* ge=NULL;
	
	ge = (of1x_group_t *) platform_malloc_shared(sizeof(of1x_group_t));
	if ( unlikely(ge==NULL) ){
		return ROFL_OF1X_GM_OGRUPS;
	}
	
	if((ret_val=__of1x_check_group_parameters(gt,type,id,buckets))!=ROFL_OF1X_GM_OK){
		platform_free_shared(ge);		
	        return ret_val;
	}
	
	ge->bc_list = buckets;
	ge->id = id;
	ge->type = type;
	ge->group_table = gt;
	ge->rwlock = platform_rwlock_init(NULL);
	__of1x_init_group_stats(&ge->stats);
	
	// Count the number of output actions existing inside the group. WARNING For select type groups the count depends on the bucket used!
	ge->num_of_output_actions = 0;
	of1x_bucket_t *bc;
	for( bc=buckets->head; bc !=NULL; bc=bc->next){
		ge->num_of_output_actions += bc->actions->num_of_output_actions;
	}
	
	//insert in the end
	if (gt->head == NULL && gt->tail == NULL){
		gt->head = ge;
		gt->tail = ge;
		ge->prev = NULL;
	} else {
		gt->tail->next = ge;
		ge->prev = gt->tail;	
	}
	ge->next = NULL;
	gt->tail = ge;
	gt->num_of_entries++;
	
	return ROFL_OF1X_GM_OK;
}

rofl_of1x_gm_result_t of1x_group_add(of1x_group_table_t *gt, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t **buckets){
	rofl_of1x_gm_result_t ret_val;
	platform_rwlock_wrlock(gt->rwlock);
	
	//check wether onither entry with this ID already exists
	if(__of1x_group_search(gt,id)!=NULL){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_OF1X_GM_EXISTS;
	}
	
	ret_val = __of1x_init_group(gt,type,id,*buckets);
	if (ret_val!=ROFL_OF1X_GM_OK){
		platform_rwlock_wrunlock(gt->rwlock);
		return ret_val;
	}
	
	platform_rwlock_wrunlock(gt->rwlock);

	//Was successful set the pointer to NULL
	//so that is not further used outside the pipeline
	*buckets = NULL;	

	return ROFL_OF1X_GM_OK;
}

static
void __of1x_destroy_group(of1x_group_table_t *gt, of1x_group_t *ge){
	
	platform_rwlock_wrlock(ge->rwlock);
	
	//destroy buckets & actions inside
	of1x_destroy_bucket_list(ge->bc_list);
	
	__of1x_destroy_group_stats(&ge->stats);
	
	platform_rwlock_destroy(ge->rwlock);

	//free
	platform_free_shared(ge);
}

static
rofl_result_t __of1x_extract_group(of1x_group_table_t *gt, of1x_group_t *ge){
	
	//take write lock of the table
	platform_rwlock_wrlock(gt->rwlock);
	//check if the group is still in the table
	if( unlikely(ge->group_table==NULL) ){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_FAILURE;
	}
	ge->group_table = NULL;
	
	//detach
	if(ge->next)
	ge->next->prev = ge->prev;
	if(ge->prev)
	ge->prev->next = ge->next;
	
	//check head and tail
	if (gt->head == ge)
		gt->head = ge->next;
	if (gt->tail == ge)
		gt->tail = ge->prev;
	
	gt->num_of_entries--;
	//leave write lock of the table
	platform_rwlock_wrunlock(gt->rwlock);
	return ROFL_SUCCESS;
}

rofl_of1x_gm_result_t of1x_group_delete(of1x_pipeline_t *pipeline, of1x_group_table_t *gt, uint32_t id){
	int i;
	of1x_flow_entry_t* entry;
	of1x_group_t *ge, *next;
	
	//TODO if the group value is OFP1X_GROUP_ALL, delete all groups 
	if(id == OF1X_GROUP_ALL){
		for(ge = gt->head; ge; ge=next){
			next = ge->next;
			//extract the group without destroying it (only the first thread that comes gets it)
			if(__of1x_extract_group(gt, ge)==ROFL_FAILURE)
				return ROFL_OF1X_GM_OK; //if it is not found no need to throw an error
			
			//loop for all the tables and erase entries that point to the group
			for(i=0; i<pipeline->num_of_tables; i++){
				while((entry=of1x_matching_algorithms[pipeline->tables[i].matching_algorithm].find_entry_using_group_hook(&pipeline->tables[i],ge->id))!=NULL){
					__of1x_remove_specific_flow_entry_table(pipeline,i,entry, OF1X_FLOW_REMOVE_GROUP_DELETE, MUTEX_NOT_ACQUIRED);
				}
			}
			//destroy the group
			__of1x_destroy_group(gt,ge);
		}
		return ROFL_OF1X_GM_OK;
	}
	
	//search the table for the group
	if((ge=__of1x_group_search(gt,id))==NULL);
		return ROFL_OF1X_GM_OK; //if it is not found no need to throw an error
	
	//extract the group without destroying it (only the first thread that comes gets it)
	if(__of1x_extract_group(gt, ge)==ROFL_FAILURE)
		return ROFL_OF1X_GM_OK; //if it is not found no need to throw an error
	
	//loop for all the tables and erase entries that point to the group
	for(i=0; i<pipeline->num_of_tables; i++){
		while((entry=of1x_matching_algorithms[pipeline->tables[i].matching_algorithm].find_entry_using_group_hook(&pipeline->tables[i],ge->id))!=NULL){
			__of1x_remove_specific_flow_entry_table(pipeline,i,entry, OF1X_FLOW_REMOVE_GROUP_DELETE, MUTEX_NOT_ACQUIRED);
		}
	}
	
	//destroy the group
	__of1x_destroy_group(gt,ge);
	
	return ROFL_OF1X_GM_OK;
}

/**
 * Function that searches a group
 * and modifies the action buckets inside
 * @param actions is a null ended array with the action groups for each bucket
 */
rofl_of1x_gm_result_t of1x_group_modify(of1x_group_table_t *gt, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t **buckets){
	rofl_of1x_gm_result_t ret_val;
	
	if((ret_val=__of1x_check_group_parameters(gt,type,id,*buckets))!=ROFL_OF1X_GM_OK)
		return ret_val;
	
	of1x_group_t *ge = __of1x_group_search(gt,id);
	if (ge == NULL){
		return ROFL_OF1X_GM_UNKGRP;
	}
	
	platform_rwlock_wrlock(ge->rwlock);
	
	of1x_destroy_bucket_list(ge->bc_list);
	ge->bc_list = *buckets;
	ge->id = id;
	ge->type = type;
	ge->group_table = gt;

	platform_rwlock_wrunlock(ge->rwlock);
	
	//Was successful set the pointer to NULL
	//so that is not further used outside the pipeline
	*buckets = NULL;	

	return ROFL_SUCCESS;
}

of1x_bucket_list_t* of1x_init_bucket_list(void){
	of1x_bucket_list_t *bl = platform_malloc_shared(sizeof(of1x_bucket_list_t));
	if ( unlikely(bl==NULL) )
		return NULL;
	
	bl->num_of_buckets=0;
	bl->head = NULL;
	bl->tail = NULL;
	return bl;
}

rofl_result_t of1x_insert_bucket_in_list(of1x_bucket_list_t *bu_list,of1x_bucket_t *bucket){
	
	if(bu_list->head==NULL && bu_list->tail==NULL){
		bu_list->head = bucket;
		bu_list->tail = bucket;
	}
	else{
		bu_list->tail->next = bucket;
		bu_list->tail = bucket;
	}
	bu_list->num_of_buckets++;
	return ROFL_SUCCESS;
}

of1x_bucket_t* of1x_init_bucket(uint16_t weight, uint32_t port, uint32_t group, of1x_action_group_t* actions){
	
	of1x_bucket_t *bk = platform_malloc_shared(sizeof(of1x_bucket_t));
	if ( unlikely(bk==NULL) )
		return NULL;
	
	bk->next= NULL;
	bk->weight= weight;
	bk->port= port;
	bk->group= group;
	bk->actions = actions;// actions must be already initialized
	__of1x_init_bucket_stats(&bk->stats);
	
	return bk;
}

void of1x_destroy_bucket_list(of1x_bucket_list_t *bc_list){
	of1x_bucket_t *bk_it, *next;
	
	for(bk_it=bc_list->head;bk_it!=NULL;bk_it=next){
		next = bk_it->next;
		//NOTE were are the action groups created and deleted?
		of1x_destroy_action_group(bk_it->actions);
		__of1x_destroy_buckets_stats(&bk_it->stats);
		platform_free_shared(bk_it);
	}
	platform_free_shared(bc_list);
}

bool __of1x_bucket_list_has_weights(of1x_bucket_list_t *bl){
	of1x_bucket_t *bu_it;
	for(bu_it = bl->head; bu_it!=NULL; bu_it=bu_it->next){
		if(bu_it->weight!=0)
			return true;
	}
	return false;
}

void of1x_dump_bucket(of1x_bucket_t *bc, bool raw_nbo){
	ROFL_PIPELINE_DEBUG_NO_PREFIX("Weight %u, port %u, actions: ", bc->weight, bc->port);
	
	//NOTE stats?
	
	__of1x_dump_action_group(bc->actions, raw_nbo);
	ROFL_PIPELINE_DEBUG_NO_PREFIX("\n");
}

void of1x_dump_group(of1x_group_t* group, bool raw_nbo){
	of1x_bucket_t *bc_it;
	unsigned int i;
	
	ROFL_PIPELINE_DEBUG_NO_PREFIX("Id %u, ", group->id);
	switch(group->type){
		case OF1X_GROUP_TYPE_ALL:
			ROFL_PIPELINE_DEBUG_NO_PREFIX("GROUP_TYPE_ALL, ");
			break;
		case OF1X_GROUP_TYPE_SELECT:
			ROFL_PIPELINE_DEBUG_NO_PREFIX("GROUP_TYPE_SELECT, ");
			break;
		case OF1X_GROUP_TYPE_INDIRECT:
			ROFL_PIPELINE_DEBUG_NO_PREFIX("GROUP_TYPE_INDIRECT, ");
			break;
		case OF1X_GROUP_TYPE_FF:
			ROFL_PIPELINE_DEBUG_NO_PREFIX("GROUP_TYPE_FF, ");
			break;
		default:
			ROFL_PIPELINE_DEBUG_NO_PREFIX("UNEXPECTED GROUP_TYPE (%u), ", group->type);
			break;
	}
	ROFL_PIPELINE_DEBUG_NO_PREFIX("# of buckets %u\n", group->bc_list->num_of_buckets);
	
	//NOTE stats
	

	for(bc_it=group->bc_list->head, i=0; bc_it; bc_it=bc_it->next, i++){
		ROFL_PIPELINE_DEBUG("\t\t[%u] Bucket (%p). ", i, bc_it);
		of1x_dump_bucket(bc_it, raw_nbo);
	}
	ROFL_PIPELINE_DEBUG("\n");
}

void of1x_dump_group_table(of1x_group_table_t *gt, bool raw_nbo){
	of1x_group_t* it;
	unsigned int i;
	
	ROFL_PIPELINE_DEBUG("Dumping group table. # of group entries: %u. \n",gt->num_of_entries);
	if (gt->num_of_entries > 0){
		for(it=gt->head, i=0; it; it=it->next, i++){
			ROFL_PIPELINE_DEBUG("\t[%u] Group (%p). ", i, it);
			of1x_dump_group(it, raw_nbo);
		}
	}
	else{
		ROFL_PIPELINE_DEBUG("\t[*] No entries\n");
		ROFL_PIPELINE_DEBUG("\n");
	}
}
