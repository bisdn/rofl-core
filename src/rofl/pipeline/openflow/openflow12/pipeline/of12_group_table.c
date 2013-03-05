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
#include "../../../platform/memory.h"

static rofl_result_t of12_init_group_bucket(of12_group_entry_t *ge, uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t *actions);
static rofl_result_t of12_destroy_group_bucket_all(of12_group_entry_t *ge);
static rofl_result_t of12_destroy_group_entry(of12_group_table_t *gt, of12_group_entry_t *ge);

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
	of12_group_entry_t *iterator=NULL, *next=NULL;
	//check if there are existing entries and deleting them
	
	for(iterator=gt->head; iterator!=NULL; iterator=next)
	{
		next=iterator->next;
		of12_destroy_group_entry(gt,iterator);
	}
	///WARNING we need to make sure that no one is in so we can destroy the mutex!!!
	platform_rwlock_destroy(gt->rwlock);
	
	
	cutil_free_shared(gt);
}

/**
 * Searches in the table for an entry with a specific id
 * returns pointer if found or NULL if not
 */
static
of12_group_entry_t *of12_group_search(of12_group_table_t *gt, uint32_t id){
	of12_group_entry_t *iterator=NULL, *next=NULL;
	
	for(iterator=gt->head; iterator!=NULL; iterator=next)
	{
		next=iterator->next;
		if(iterator->id == id)
			return iterator;
	}
	
	return NULL;
}

static
rofl_result_t of12_init_group_entry(of12_group_table_t *gt, of12_group_type_t type, uint32_t id,
									uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t *actions){
	
	of12_group_entry_t* ge=NULL;
	
	//TODO we should recieve the list of actions that must be stored in the buckets
	
	ge = (of12_group_entry_t *) cutil_malloc_shared(sizeof(of12_group_entry_t));
	if (ge == NULL){
		return ROFL_FAILURE;
	}
	
	//TODO validate action sets here
	
	ge->bl_head = ge->bl_tail = NULL;
	ge->id = id;
	ge->type = type;
	
	//insert in the end
	if (gt->head == NULL && gt->tail == NULL){
		gt->head = ge;
		gt->tail = ge;
		ge->prev = NULL;
	}
	else{
		gt->tail->next = ge;
		ge->prev = gt->tail;	
	}
	ge->next = NULL;
	gt->tail = ge;
	gt->num_of_entries++;
	
	if(of12_init_group_bucket(ge,weigth,group,port, actions)!=ROFL_SUCCESS)
		return ROFL_FAILURE;
	
	return ROFL_SUCCESS;
}

rofl_result_t of12_group_add(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, 
							 uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t *actions){
	
	platform_rwlock_wrlock(gt->rwlock);
	
	//check wether onither entry with this ID already exists
	if(of12_group_search(gt,id)!=NULL){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_FAILURE;
	}
	
	if (of12_init_group_entry(gt,type,id, weigth, group, port, actions)!=ROFL_SUCCESS){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_FAILURE;
	}
	platform_rwlock_wrunlock(gt->rwlock);
	return ROFL_SUCCESS;
}

static
rofl_result_t of12_destroy_group_entry(of12_group_table_t *gt, of12_group_entry_t *ge){
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
	
	//destroy buckets & actions inside
	of12_destroy_group_bucket_all(ge);
	
	//free
	cutil_free_shared(ge);
	
	return ROFL_SUCCESS;
}

rofl_result_t of12_group_delete(of12_group_table_t *gt, uint32_t id){
	
	platform_rwlock_wrlock(gt->rwlock);
	
	of12_group_entry_t *ge = of12_group_search(gt,id);
	if(ge==NULL){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_SUCCESS; //if it is not found no need to throw an error
	}
	
	if(of12_destroy_group_entry(gt,ge)!=ROFL_SUCCESS){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_FAILURE;
	}
	platform_rwlock_wrunlock(gt->rwlock);
	return ROFL_SUCCESS;
}


rofl_result_t of12_group_modify(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, 
								uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t *actions){
	
	platform_rwlock_wrlock(gt->rwlock);
	
	//search && remove && add
	of12_group_entry_t *ge = of12_group_search(gt,id);
	if (ge == NULL){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_FAILURE;
	}
	
	if(of12_destroy_group_entry(gt,ge)!=ROFL_SUCCESS){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_FAILURE;
	}
	if(of12_init_group_entry(gt,type,id,weigth,group,port,actions)!=ROFL_SUCCESS){
		platform_rwlock_wrunlock(gt->rwlock);
		return ROFL_FAILURE;
	}
	platform_rwlock_wrunlock(gt->rwlock);
	return ROFL_SUCCESS;
}
static
rofl_result_t of12_init_group_bucket(of12_group_entry_t *ge, uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t *actions){
	of12_group_bucket_t *bk = cutil_malloc_shared(sizeof(of12_group_bucket_t));
	if (bk == NULL)
		return ROFL_FAILURE;
	
	bk->next= NULL;
	bk->weigth= weigth;
	bk->group=group;
	bk->port=port;
	bk->actions = actions;//of12_init_action_group(actions);
	
	//insert the bucket
	if (ge->bl_head == NULL && 	ge->bl_tail == NULL)
		ge->bl_head = ge->bl_tail = bk;
	else{
		ge->bl_tail->next = bk;
		ge->bl_tail = bk;
	}
	
	return ROFL_SUCCESS;
}

static
rofl_result_t of12_destroy_group_bucket_all(of12_group_entry_t *ge){
	of12_group_bucket_t *bk_it, *next;
	
	for(bk_it=ge->bl_head;bk_it!=NULL;bk_it=next){
		next = bk_it->next;
		///*of12_destroy_action_group(bk_it->actions);*/
		cutil_free_shared(bk_it);
	}
	
	return ROFL_SUCCESS;
}

