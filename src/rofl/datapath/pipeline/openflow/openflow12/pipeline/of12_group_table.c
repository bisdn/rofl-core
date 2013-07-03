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
#include "of12_group_table.h"
#include "of12_pipeline.h"
#include "../../../platform/memory.h"
#include <stdio.h>

static void __of12_destroy_group(of12_group_table_t *gt, of12_group_t *ge);
static rofl_of12_gm_result_t __of12_validate_group(of12_action_group_t* actions);
bool __of12_bucket_list_has_weights(of12_bucket_list_t *bl);

of12_group_table_t* of12_init_group_table(){
	of12_group_table_t *gt;
	gt = (of12_group_table_t *) platform_malloc_shared(sizeof(of12_group_table_t));
	
	if(gt==NULL){
		return NULL;
	}
	
	gt->num_of_entries = 0;
	gt->head = NULL;
	gt->tail = NULL;
	
	gt->rwlock = platform_rwlock_init(NULL);
	
	return gt;
}

void of12_destroy_group_table(of12_group_table_t* gt){
	of12_group_t *iterator=NULL, *next=NULL;
	//check if there are existing entries and deleting them
	
	platform_rwlock_wrlock(gt->rwlock);
	
	for(iterator=gt->head; iterator!=NULL; iterator=next){
		next=iterator->next;
		__of12_destroy_group(gt,iterator);
	}
	
	platform_rwlock_destroy(gt->rwlock);
	
	
	platform_free_shared(gt);
}

/**
 * Copies the structure of the group table.
 */
rofl_result_t of12_fetch_group_table(of12_pipeline_t *pipeline, of12_group_table_t* group_table){
	platform_rwlock_rdlock(pipeline->groups->rwlock);
	
	*group_table = *(pipeline->groups);
	
	platform_rwlock_rdunlock(pipeline->groups->rwlock);
	return ROFL_SUCCESS;
}

/**
 * Searches in the table for an entry with a specific id
 * returns pointer if found or NULL if not
 */
of12_group_t* __of12_group_search(of12_group_table_t *gt, uint32_t id){
	of12_group_t *iterator=NULL, *next=NULL;
	
	for(iterator=gt->head; iterator!=NULL; iterator=next){
		next=iterator->next;
		if(iterator->id == id)
			return iterator;
	}
	
	return NULL;
}

rofl_of12_gm_result_t __of12_check_group_parameters(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, of12_bucket_list_t *buckets){
	of12_bucket_t* bu_it;
	rofl_of12_gm_result_t ret_val;
	//TODO check type
    
	if(id == OF12_GROUP_ALL || id == OF12_GROUP_ANY || id > OF12_GROUP_MAX)
		return ROFL_OF12_GM_INVAL;
    
	//validate action set
	for(bu_it=buckets->head;bu_it!=NULL;bu_it=bu_it->next){
		if((ret_val=__of12_validate_group(bu_it->actions))!=ROFL_OF12_GM_OK)
			return ret_val;
	}
	
	//FIXME group types not ssupported
	if (type == OF12_GROUP_TYPE_SELECT || type == OF12_GROUP_TYPE_FF){
		fprintf(stderr,"<%s:%d> GROUP TYPE NOT IMPLEMENTED\n",__func__,__LINE__);
		return ROFL_OF12_GM_INVAL;
	}
	
	
	if(type == OF12_GROUP_TYPE_INDIRECT && buckets->num_of_buckets>1)
		return ROFL_OF12_GM_INVAL;
	if( (type == OF12_GROUP_TYPE_ALL || type == OF12_GROUP_TYPE_INDIRECT) && __of12_bucket_list_has_weights(buckets))
		return ROFL_OF12_GM_INVAL;
	if (type == OF12_GROUP_TYPE_SELECT && __of12_bucket_list_has_weights(buckets) == false)
		return ROFL_OF12_GM_INVAL;
	
	return ROFL_OF12_GM_OK;
}

static
rofl_of12_gm_result_t __of12_init_group(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, of12_bucket_list_t *buckets){
							//uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t **actions){
	rofl_of12_gm_result_t ret_val;
	of12_group_t* ge=NULL;
	
	ge = (of12_group_t *) platform_malloc_shared(sizeof(of12_group_t));
	if (ge == NULL){
		return ROFL_OF12_GM_OGRUPS;
	}
	
	if((ret_val=__of12_check_group_parameters(gt,type,id,buckets))!=ROFL_OF12_GM_OK)
        return ret_val;
	
	ge->bc_list = buckets;
	ge->id = id;
	ge->type = type;
	ge->group_table = gt;
	ge->rwlock = platform_rwlock_init(NULL);
	__of12_init_group_stats(&ge->stats);
	
	// Count the number of output actions existing inside the group. WARNING For select type groups the count depends on the bucket used!
	ge->num_of_output_actions = 0;
	of12_bucket_t *bc;
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
	
	return ROFL_OF12_GM_OK;
}

rofl_of12_gm_result_t of12_group_add(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, of12_bucket_list_t *buckets){
							 //uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t **actions){
	rofl_of12_gm_result_t ret_val;
	platform_rwlock_wrlock(gt->rwlock);
	
	//check wether onither entry with this ID already exists
	if(__of12_group_search(gt,id)!=NULL){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_OF12_GM_EXISTS;
	}
	
	ret_val = __of12_init_group(gt,type,id, buckets);
	if (ret_val!=ROFL_OF12_GM_OK){
		platform_rwlock_wrunlock(gt->rwlock);
		return ret_val;
	}
	
	platform_rwlock_wrunlock(gt->rwlock);
	return ROFL_OF12_GM_OK;
}

static
void __of12_destroy_group(of12_group_table_t *gt, of12_group_t *ge){
	
	platform_rwlock_wrlock(ge->rwlock);
	
	//destroy buckets & actions inside
	of12_destroy_bucket_list(ge->bc_list);
	
	__of12_destroy_group_stats(&ge->stats);
	
	platform_rwlock_destroy(ge->rwlock);

	//free
	platform_free_shared(ge);
}

static
rofl_result_t __of12_extract_group(of12_group_table_t *gt, of12_group_t *ge){
	
	//take write lock of the table
	platform_rwlock_wrlock(gt->rwlock);
	//check if the group is still in the table
	if(ge->group_table==NULL){
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

rofl_of12_gm_result_t of12_group_delete(of12_pipeline_t *pipeline, of12_group_table_t *gt, uint32_t id){
	int i;
	of12_flow_entry_t* entry;
	of12_group_t *ge, *next;
	
	//TODO if the group value is OFP12_GROUP_ALL, delete all groups 
	if(id == OF12_GROUP_ALL){
		for(ge = gt->head; ge; ge=next){
			next = ge->next;
			//extract the group without destroying it (only the first thread that comes gets it)
			if(__of12_extract_group(gt, ge)==ROFL_FAILURE)
				return ROFL_OF12_GM_OK; //if it is not found no need to throw an error
			
			//loop for all the tables and erase entries that point to the group
			for(i=0; i<pipeline->num_of_tables; i++){
				while((entry=of12_matching_algorithms[pipeline->tables[i].matching_algorithm].find_entry_using_group_hook(&pipeline->tables[i],ge->id))!=NULL){
					__of12_remove_specific_flow_entry_table(pipeline,i,entry, OF12_FLOW_REMOVE_GROUP_DELETE, MUTEX_NOT_ACQUIRED);
				}
			}
			//destroy the group
			__of12_destroy_group(gt,ge);
		}
		return ROFL_OF12_GM_OK;
	}
	
	//search the table for the group
	if((ge=__of12_group_search(gt,id))==NULL);
		return ROFL_OF12_GM_OK; //if it is not found no need to throw an error
	
	//extract the group without destroying it (only the first thread that comes gets it)
	if(__of12_extract_group(gt, ge)==ROFL_FAILURE)
		return ROFL_OF12_GM_OK; //if it is not found no need to throw an error
	
	//loop for all the tables and erase entries that point to the group
	for(i=0; i<pipeline->num_of_tables; i++){
		while((entry=of12_matching_algorithms[pipeline->tables[i].matching_algorithm].find_entry_using_group_hook(&pipeline->tables[i],ge->id))!=NULL){
			__of12_remove_specific_flow_entry_table(pipeline,i,entry, OF12_FLOW_REMOVE_GROUP_DELETE, MUTEX_NOT_ACQUIRED);
		}
	}
	
	//destroy the group
	__of12_destroy_group(gt,ge);
	
	return ROFL_OF12_GM_OK;
}

/**
 * Function that searches a group
 * and modifies the action buckets inside
 * @param actions is a null ended array with the action groups for each bucket
 */
rofl_of12_gm_result_t of12_group_modify(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, of12_bucket_list_t *buckets){
	rofl_of12_gm_result_t ret_val;
	
	if((ret_val=__of12_check_group_parameters(gt,type,id,buckets))!=ROFL_OF12_GM_OK)
		return ret_val;
	
	of12_group_t *ge = __of12_group_search(gt,id);
	if (ge == NULL){
		return ROFL_OF12_GM_UNKGRP;
	}
	
	platform_rwlock_wrlock(ge->rwlock);
	
	of12_destroy_bucket_list(ge->bc_list);
	ge->bc_list = buckets;
	ge->id = id;
	ge->type = type;
	ge->group_table = gt;
	/*for(i=0;buckets[i]!=NULL;i++){
		if(of12_init_group_bucket(ge,buckets[i])==ROFL_FAILURE){
			platform_rwlock_wrunlock(ge->rwlock);
			return ROFL_FAILURE;
		}
	}*/
	platform_rwlock_wrunlock(ge->rwlock);
	
	return ROFL_SUCCESS;
}

of12_bucket_list_t* of12_init_bucket_list(void){
	of12_bucket_list_t *bl = platform_malloc_shared(sizeof(of12_bucket_list_t));
	if (bl == NULL)
		return NULL;
	
	bl->num_of_buckets=0;
	bl->head = NULL;
	bl->tail = NULL;
	return bl;
}

rofl_result_t of12_insert_bucket_in_list(of12_bucket_list_t *bu_list,of12_bucket_t *bucket){
	
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

of12_bucket_t* of12_init_bucket(uint16_t weight, uint32_t port, uint32_t group, of12_action_group_t* actions){
	
	of12_bucket_t *bk = platform_malloc_shared(sizeof(of12_bucket_t));
	if (bk == NULL)
		return NULL;
	
	bk->next= NULL;
	bk->weight= weight;
	bk->port= port;
	bk->group= group;
	bk->actions = actions;// actions must be already initialized
	__of12_init_bucket_stats(&bk->stats);
	
	return bk;
}

void of12_destroy_bucket_list(of12_bucket_list_t *bc_list){
	of12_bucket_t *bk_it, *next;
	
	for(bk_it=bc_list->head;bk_it!=NULL;bk_it=next){
		next = bk_it->next;
		//NOTE were are the action groups created and deleted?
		of12_destroy_action_group(bk_it->actions);
		__of12_destroy_buckets_stats(&bk_it->stats);
		platform_free_shared(bk_it);
	}
	platform_free_shared(bc_list);
}

static
rofl_of12_gm_result_t __of12_validate_group(of12_action_group_t* actions){

	//we dont allow OF12_AT_GROUP
	//and neither OF12_AT_OUTPUT in the case of OF12_PORT_TABLE
	of12_packet_action_t *it;
	
	for(it=actions->head; it; it=it->next){
		if(it->type == OF12_AT_GROUP)
			return ROFL_OF12_GM_CHAIN;
		if(it->type == OF12_AT_OUTPUT && it->field == OF12_PORT_TABLE)
			return ROFL_OF12_GM_INVAL;
	}
		
	//verify apply actions
	if(__of12_validate_action_group(actions, NULL)==false)
		return ROFL_OF12_GM_INVAL;
	
	return ROFL_OF12_GM_OK;
}

bool __of12_bucket_list_has_weights(of12_bucket_list_t *bl){
	of12_bucket_t *bu_it;
	for(bu_it = bl->head; bu_it!=NULL; bu_it=bu_it->next){
		if(bu_it->weight!=0)
			return true;
	}
	return false;
}
