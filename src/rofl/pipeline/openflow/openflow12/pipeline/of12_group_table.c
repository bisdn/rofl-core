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
#include "rofl.h"
#include "of12_group_table.h"
#include "../../../platform/memory.h"


of12_group_table_t* of12_group_table_init(){
	of12_group_table_t *gt;
	gt = (of12_group_table_t *) cutil_malloc_shared(sizeof(of12_group_table_t));
	
	if(gt==NULL){
		return NULL;
	}
	
	gt->num_of_entries = 0;
	gt->head = NULL;
	gt->tail = NULL;
	
	return gt;
}
/**
 * Searches in the table for an entry with a specific id
 * returns pointer if found or NULL if not
 */
of12_group_entry_t *of12_group_search(of12_group_table_t *gt, uint32_t id){
	of12_group_entry_t *iterator=NULL, *next=NULL;
	
	for(iterator=gt->head; iterator=!NULL; iterator=next)
	{
		next=iterator->next;
		if(iterator->id == id)
			return iterator;
	}
	
	return NULL;
}

rofl_result_t of12_group_add(of12_group_table_t *gt, enum of12p_group_type type, uint32_t id){
	of12_group_entry_t* ge=NULL, *iterator=NULL, *next=NULL;
	
	ge = (of12_group_entry_t *) cutil_malloc_shared(sizeof(of12_group_entry_t));
	if (ge == NULL){
		return ROFL_FAILURE;
	}
	
	//check wether onither entry with this ID already exists
	if(of12_group_search(gt,id)!=NULL){
			return ROFL_FAILURE;
	}
	
	//TODO validate action sets
	
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
		ge->prev = gt->tail->next;	
	}
	ge->next = NULL;
	gt->tail = ge;
	
	return ROFL_SUCCESS;
}

rofl_result_t of12_group_delete(of12_group_table_t *gt, uint32_t id){
	of12_group_entry_t *ge = of12_group_search(gt,id);
	if(ge==NULL){
		return ROFL_SUCCESS; //if it is not found no need to throw an error
	}
	
	//detach
	ge->next->prev = ge->prev;
	ge->prev->next = ge->next;
	
	//check head and tail
	if (gt->head == ge)
		gt->head = ge->next;
	if (gt->tail == ge)
		gt->tail = ge->prev;
	
	//free
	cutil_free_shared(ge);
	
	return ROFL_SUCCESS;
}

rofl_result_t of12_group_modify(of12_group_table_t *gt){
	//search && remove && add
}



