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

static rofl_result_t of12_init_group_bucket(of12_group_t *ge, uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t *actions);
static void of12_destroy_group_bucket_all(of12_group_t *ge);
static void of12_destroy_group(of12_group_table_t *gt, of12_group_t *ge);
static rofl_result_t of12_validate_group(of12_action_group_t* actions);

of12_group_table_t* of12_init_group_table(){
	of12_group_table_t *gt;
	gt = (of12_group_table_t *) cutil_malloc_shared(sizeof(of12_group_table_t));
	
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
		of12_destroy_group(gt,iterator);
	}
	
	platform_rwlock_destroy(gt->rwlock);
	
	
	cutil_free_shared(gt);
}

/**
 * Searches in the table for an entry with a specific id
 * returns pointer if found or NULL if not
 */
of12_group_t *of12_group_search(of12_group_table_t *gt, uint32_t id){
	of12_group_t *iterator=NULL, *next=NULL;
	
	for(iterator=gt->head; iterator!=NULL; iterator=next){
		next=iterator->next;
		if(iterator->id == id)
			return iterator;
	}
	
	return NULL;
}

static
rofl_result_t of12_init_group(of12_group_table_t *gt, of12_group_type_t type, uint32_t id,
							uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t **actions){
	
	of12_group_t* ge=NULL;
	int i;
	
	ge = (of12_group_t *) cutil_malloc_shared(sizeof(of12_group_t));
	if (ge == NULL){
		return ROFL_FAILURE;
	}
	
	//validate action set
	for(i=0;actions[i]!=NULL;i++){
		if(of12_validate_group(actions[i])==ROFL_FAILURE)
			return ROFL_FAILURE;
	}
	
	ge->bl_head = ge->bl_tail = NULL;
	ge->id = id;
	ge->type = type;
	ge->group_table = gt;
	ge->rwlock = platform_rwlock_init(NULL);
	
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
	
	for(i=0;actions[i]!=NULL;i++){
		if(of12_init_group_bucket(ge,weigth,group,port, actions[i])!=ROFL_SUCCESS)
			return ROFL_FAILURE;
	}
	
	return ROFL_SUCCESS;
}

rofl_result_t of12_group_add(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, 
							 uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t **actions){
	
	platform_rwlock_wrlock(gt->rwlock);
	
	//check wether onither entry with this ID already exists
	if(of12_group_search(gt,id)!=NULL){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_FAILURE;
	}
	
	if (of12_init_group(gt,type,id, weigth, group, port, actions)!=ROFL_SUCCESS){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_FAILURE;
	}
	platform_rwlock_wrunlock(gt->rwlock);
	return ROFL_SUCCESS;
}

static
void of12_destroy_group(of12_group_table_t *gt, of12_group_t *ge){
	
	platform_rwlock_wrlock(ge->rwlock);
	
	//destroy buckets & actions inside
	of12_destroy_group_bucket_all(ge);
	
	platform_rwlock_destroy(ge->rwlock);

	//free
	cutil_free_shared(ge);
}

static
rofl_result_t of12_extract_group(of12_group_table_t *gt, of12_group_t *ge){
	
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

rofl_result_t of12_group_delete(of12_group_table_t *gt, uint32_t id, of12_pipeline_t *pipeline){
	int i;
	of12_flow_entry_t* entry;
	of12_group_t *ge;
	
	//search the table for the group
	if((ge=of12_group_search(gt,id))==NULL);
		return ROFL_SUCCESS; //if it is not found no need to throw an error
	
	//extract the group without destroying it (only the first thread that comes gets it)
	if(of12_extract_group(gt, ge)==ROFL_FAILURE)
		return ROFL_SUCCESS; //if it is not found no need to throw an error
	
	//loop for all the tables and erase entries that point to the group
	for(i=0; i<pipeline->num_of_tables; i++){
		while((entry=pipeline->tables[i].maf.find_entry_using_group_hook(&pipeline->tables[i],ge->id))!=NULL){
			of12_remove_specific_flow_entry_table(pipeline,i,entry,MUTEX_NOT_ACQUIRED);
		}
	}
	
	//destroy the group
	of12_destroy_group(gt,ge);
	
	return ROFL_SUCCESS;
}

/**
 * Function that searches a group
 * and modifies the action buckets inside
 * @param actions is a null ended array with the action groups for each bucket
 */
rofl_result_t of12_group_modify(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, 
								uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t **actions){
	int i;

	of12_group_t *ge = of12_group_search(gt,id);
	if (ge == NULL){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_FAILURE;
	}
	
	platform_rwlock_wrlock(ge->rwlock);
	
	of12_destroy_group_bucket_all(ge);
	
	for(i=0;actions[i]!=NULL;i++){
		if(of12_init_group_bucket(ge,weigth,group,port,actions[i])==ROFL_FAILURE){
			platform_rwlock_wrunlock(ge->rwlock);
			return ROFL_FAILURE;
		}
	}
	platform_rwlock_wrunlock(ge->rwlock);
	
	return ROFL_SUCCESS;
}
static
rofl_result_t of12_init_group_bucket(of12_group_t *ge, uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t *actions){
	
	of12_group_bucket_t *bk = cutil_malloc_shared(sizeof(of12_group_bucket_t));
	if (bk == NULL)
		return ROFL_FAILURE;
	
	bk->next= NULL;
	bk->weigth= weigth;
	bk->group=group;
	bk->port=port;
	bk->actions = actions;//of12_init_action_group(actions);
	
	//insert the bucket
	if (ge->bl_head == NULL && 	ge->bl_tail == NULL){
		ge->bl_head = ge->bl_tail = bk;
	}else if(ge->type == OF12_GROUP_TYPE_INDIRECT){
		//fprintf(stderr,"<%s:%d> This Group type is defined with only one bucket");
		return ROFL_FAILURE;
	}else{
		ge->bl_tail->next = bk;
		ge->bl_tail = bk;
	}
	
	return ROFL_SUCCESS;
}

static
void of12_destroy_group_bucket_all(of12_group_t *ge){
	of12_group_bucket_t *bk_it, *next;
	
	for(bk_it=ge->bl_head;bk_it!=NULL;bk_it=next){
		next = bk_it->next;
		//NOTE  of12_destroy_action_group(bk_it->actions);
		//were are the action groups created and deleted?
		cutil_free_shared(bk_it);
	}
}
static
rofl_result_t of12_validate_group(of12_action_group_t* actions){

	//we dont allow OF12_AT_GROUP
	//and neither OF12_AT_OUTPUT in the case of OF12_PORT_TABLE
	of12_packet_action_t *it;
	
	for(it=actions->head; it; it=it->next){
		if(it->type == OF12_AT_GROUP)
			return ROFL_FAILURE;
		if(it->type == OF12_AT_OUTPUT && it->field == OF12_PORT_TABLE)
			return ROFL_FAILURE;
	}
		
	//verify apply actions
	of12_validate_action_group(actions);
	return ROFL_FAILURE;
}
