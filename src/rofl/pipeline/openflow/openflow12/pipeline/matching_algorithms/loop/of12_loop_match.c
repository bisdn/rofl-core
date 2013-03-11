#include "of12_loop_match.h"

#include <stdlib.h>
#include <assert.h>
#include "../../../../../util/rofl_pipeline_utils.h"
#include "../../of12_flow_table.h"
#include "../../of12_flow_entry.h"
#include "../../of12_match.h"
#include "../../of12_group_table.h"
#include "../../../../../platform/lock.h"

#define LOOP_NO_MATCH 0
#define LOOP_MATCH 1
#define LOOP_DESCRIPTION "The loop algorithm searches the list of entries by its priority order. On the worst case the performance is o(N) with the number of entries"

/**
* This matching algorithm is the most simple and straightforward
* implementation according to the spec. Very useful for testing
* and debugging; e.g. tracking problems in other components of
* the datapath.
*
* Lots of improvements could be done in terms of performance. 
* No optimizations are applied to this one. 
*
* If you want more performance: just create a new matching
* algorithm!
*/

/**
* Looks for an overlapping entry from the entry pointer by start_entry. This is an EXPENSIVE call
*/
static of12_flow_entry_t* of12_flow_table_loop_check_overlapping(of12_flow_entry_t *const start_entry, of12_flow_entry_t* entry, bool check_cookie, uint32_t out_port, uint32_t out_group){

	of12_flow_entry_t* it; //Just for code clarity

	//Empty table
	if(!start_entry)
		return NULL;

	for(it=start_entry; it != NULL; it=it->next){
		if( of12_flow_entry_check_overlap(it, entry, true, check_cookie, out_port, out_group) )
			return it;
	}	
	return NULL;
}

/**
* Looks for a previously added entry from the entry pointer by start_entry. This is an EXPENSIVE call
*/
static of12_flow_entry_t* of12_flow_table_loop_check_identical(of12_flow_entry_t *const start_entry, of12_flow_entry_t* entry, uint32_t out_port, uint32_t out_group){

	of12_flow_entry_t* it; //Just for code clarity

	//Empty table
	if(!start_entry)
		return NULL;

	for(it=start_entry; it != NULL; it=it->next){
		if( of12_flow_entry_check_equal(it, entry, out_port, out_group) )
			return it;
	}	
	return NULL;
}


/*
*
* Removal of specific entry
* Warning pointer to the entry MUST be a valid pointer. Some rudimentary checking are made, such checking linked list correct state,
* and table pointer, but no further checkings are done (including lookup in the table linked list)
*
*/
static rofl_result_t of12_remove_flow_entry_table_specific_imp(of12_flow_table_t *const table, of12_flow_entry_t *const specific_entry){
	
	if(table->num_of_entries == 0) 
		return ROFL_FAILURE; 

	//Safety checks
	if(specific_entry->table != table)
		return ROFL_FAILURE; 
	if(specific_entry->prev && specific_entry->prev->next != specific_entry)
		return ROFL_FAILURE; 
	if(specific_entry->next && specific_entry->next->prev != specific_entry)
		return ROFL_FAILURE; 

	//Prevent readers to jump in
	platform_rwlock_wrlock(table->rwlock);
	
	if(!specific_entry->prev){
		//First table entry
		if(specific_entry->next)
			specific_entry->next->prev = NULL;
		table->entries = specific_entry->next;	

	}else{
		specific_entry->prev->next = specific_entry->next;
		if(specific_entry->next)
			specific_entry->next->prev = specific_entry->prev;
	}
	table->num_of_entries--;
	
	//Green light to readers and other writers			
	platform_rwlock_wrunlock(table->rwlock);

	//Destroy entry
	return of12_destroy_flow_entry(specific_entry);
}

/* 
* Adds flow_entry to the main table. This function is NOT thread safe, and mutual exclusion should be 
* acquired BEFORE this function being called, using table->mutex var. 
*/
static rofl_of12_fm_result_t of12_add_flow_entry_table_imp(of12_flow_table_t *const table, of12_flow_entry_t *const entry, bool check_overlap, bool reset_counts){
	of12_flow_entry_t *it, *prev, *existing=NULL;
	
	if(table->num_of_entries == OF12_MAX_NUMBER_OF_TABLE_ENTRIES){
		return ROFL_OF12_FM_FAILURE; 
	}

	if(!table->entries){
		//No rule yet
		entry->prev = NULL;
		entry->next = NULL;
		table->entries = entry;
		//Point entry table to us
		entry->table = table;

		//Set Timers //XXX take this out of here!
		of12_add_timer(table,entry);
	
		table->num_of_entries++;
		return ROFL_OF12_FM_SUCCESS;
	}

	//Check overlapping
	if(check_overlap && of12_flow_table_loop_check_overlapping(table->entries, entry, false, OF12_PORT_ANY, OF12_GROUP_ANY)) //Why spec is saying not to match cookie only in flow_mod add??
		return ROFL_OF12_FM_OVERLAP;

	//Look for existing entries (only if check_overlap is false)
	if(!check_overlap)
		existing = of12_flow_table_loop_check_identical(table->entries, entry, OF12_PORT_ANY, OF12_GROUP_ANY);

	if(existing){
		//There was already an entry. Update it..
		if(!reset_counts){
			entry->stats.packet_count = existing->stats.packet_count; 
			entry->stats.byte_count = existing->stats.byte_count; 
			entry->stats.initial_time = existing->stats.initial_time; 
		}
		
		//Delete old entry
		if(of12_remove_flow_entry_table_specific_imp(table,existing) != ROFL_SUCCESS)
			return ROFL_OF12_FM_FAILURE;

		//Let it add normally...
	}
	
	//Look for appropiate position in the table
	for(it=table->entries,prev=NULL; it!=NULL;prev=it,it=it->next){
		if(it->num_of_matches<= entry->num_of_matches || ( it->num_of_matches<= entry->num_of_matches && it->priority<entry->priority ) ){
			//Insert
			if(prev == NULL){
				//Set current entry
				entry->prev = NULL; 
				entry->next = it; 

				//Prevent readers to jump in
				platform_rwlock_wrlock(table->rwlock);
					
				//Place in the head
				it->prev = entry;
				table->entries = entry;	
			}else{
				//Set current entry
				entry->prev = prev;
				entry->next = prev->next;
				
				//Prevent readers to jump in
				platform_rwlock_wrlock(table->rwlock);
				
				//Add to n+1 if not tail
				if(prev->next)
					prev->next->prev = entry;
				//Fix prev
				prev->next = entry;
				
			}
			table->num_of_entries++;
	
			//Point entry table to us
			entry->table = table;
	
			//Set Timers //XXX take this out of here!
			of12_add_timer(table,entry);
	
			//Unlock mutexes
			platform_rwlock_wrunlock(table->rwlock);
			return ROFL_OF12_FM_SUCCESS;
		}
	}
	
	if(!table->entries){
		//There are no entries in the table
		entry->next = entry->prev = NULL;	
	}else{
		//Last item
		entry->next = NULL;
		entry->prev = prev;
	}

	//Point entry table to us
	entry->table = table;

	//Prevent readers to jump in
	platform_rwlock_wrlock(table->rwlock);

	if(!table->entries){
		//No entries
		table->entries = entry;
	}else{
		//Last
		prev->next = entry;
	}
	table->num_of_entries++;

	
	//Unlock mutexes
	platform_rwlock_wrunlock(table->rwlock);
	return ROFL_OF12_FM_SUCCESS;
}

/*
* 
* ENTRY removal for non-specific entries. It will remove the FIRST matching entry. This function assumes that match order of table_entry and entry are THE SAME. If not 
* the result is undefined.
*
* This function shall NOT be used if there is some prior knowledge by the lookup algorithm before (specially a pointer to the entry), as it is inherently VERY innefficient
*/

static rofl_result_t of12_remove_flow_entry_table_non_specific_imp(of12_flow_table_t *const table, of12_flow_entry_t *const entry, const enum of12_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group){

	int deleted=0; 
	of12_flow_entry_t *it, *it_next;

	if(table->num_of_entries == 0) 
		return ROFL_SUCCESS; //according to spec 

	//Loop over all the table entries	
	for(it=table->entries; it; it=it_next){
		
		//Save next item
		it_next = it->next;
		
		if( strict == STRICT ){
			//Strict make sure they are equal
			if( of12_flow_entry_check_equal(it, entry, out_port, out_group ) ){
				
				if(of12_remove_flow_entry_table_specific_imp(table, it) != ROFL_SUCCESS){
					assert(0); //This should never happen
					return ROFL_FAILURE;
				}
				deleted++;
				break;
			}
		}else{
			if( of12_flow_entry_check_contained(it, entry, strict, true, out_port, out_group) ){
				
				if(of12_remove_flow_entry_table_specific_imp(table, it) != ROFL_SUCCESS){
					assert(0); //This should never happen
					return ROFL_FAILURE;
				}
				deleted++;
			}
		}
	}

	//Even if no deletions are performed return SUCCESS
	//if(deleted == 0)	
	//	return ROFL_FAILURE; 
	
	return ROFL_SUCCESS;
}


/* 
* Removes flow_entry to the main table. This function is NOT thread safe, and mutual exclusion should be 
* acquired BEFORE this function being called, using table->mutex var.
*
* specific_entry: the exact pointer (as per stored in the table) of the entry
* entry: entry containing the exact same matches (including masks) as the entry to be deleted but instance may be different 
*
* specific_entry and entry set both to non-NULL values is strictly forbbiden
*
* In both cases table entry is deatached from the table but NOT DESTROYED. The duty of destroying entry is for the matching alg. 
* 
*/

static inline rofl_result_t of12_remove_flow_entry_table_imp(of12_flow_table_t *const table, of12_flow_entry_t *const entry, of12_flow_entry_t *const specific_entry, uint32_t out_port, uint32_t out_group, const enum of12_flow_removal_strictness strict){

	if( (entry&&specific_entry) || ( !entry && !specific_entry) )
		return ROFL_FAILURE;
 
	if(entry)
		return of12_remove_flow_entry_table_non_specific_imp(table, entry, strict, out_port, out_group);
	else
		return of12_remove_flow_entry_table_specific_imp(table, specific_entry);
}

/* Conveniently wraps call with mutex.  */
 rofl_of12_fm_result_t of12_add_flow_entry_loop(of12_flow_table_t *const table, of12_flow_entry_t *const entry, bool check_overlap, bool reset_counts){

	rofl_of12_fm_result_t return_value;

	//Allow single add/remove operation over the table
	platform_mutex_lock(table->mutex);
	
	return_value = of12_add_flow_entry_table_imp(table, entry, check_overlap, reset_counts);

	//Green light to other threads
	platform_mutex_unlock(table->mutex);

	return return_value;
}

rofl_result_t of12_modify_flow_entry_loop(of12_flow_table_t *const table, of12_flow_entry_t *const entry, const enum of12_flow_removal_strictness strict, bool reset_counts){

	int moded=0; 
	of12_flow_entry_t *it;

	if(table->num_of_entries == 0) 
		return ROFL_FAILURE; 

	//Allow single add/remove operation over the table
	platform_mutex_lock(table->mutex);
	
	//Loop over all the table entries	
	for(it=table->entries; it; it=it->next){

		if( strict == STRICT ){
			//Strict make sure they are equal
			if( of12_flow_entry_check_equal(it, entry, OF12_PORT_ANY, OF12_GROUP_ANY) ){
				of12_update_flow_entry(it, entry, reset_counts);
				moded++;
				break;
			}
		}else{
			if( of12_flow_entry_check_contained(it, entry, strict, true, OF12_PORT_ANY, OF12_GROUP_ANY) ){
				of12_update_flow_entry(it, entry, reset_counts);
				moded++;
			}
		}
	}

	platform_mutex_unlock(table->mutex);

	if(moded == 0)	
		return ROFL_FAILURE; 
	
	return ROFL_SUCCESS;
}

rofl_result_t of12_remove_flow_entry_loop(of12_flow_table_t *const table , of12_flow_entry_t *const entry, of12_flow_entry_t *const specific_entry, const enum of12_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group, of12_mutex_acquisition_required_t mutex_acquired){

	rofl_result_t result;

	//Allow single add/remove operation over the table
	if(!mutex_acquired){
		platform_mutex_lock(table->mutex);
	}
	
	result = of12_remove_flow_entry_table_imp(table, entry, specific_entry, out_port, out_group, strict);

	//Green light to other threads
	if(!mutex_acquired){
		platform_mutex_unlock(table->mutex);
	}

	return result;
}
	
/* FLOW entry lookup entry point */ 
of12_flow_entry_t* of12_find_best_match_loop(of12_flow_table_t *const table, of12_packet_matches_t *const pkt){
	of12_flow_entry_t *entry;

	//Prevent writers to change structure during matching
	platform_rwlock_rdlock(table->rwlock);
	
	//Table is sorted out by nº of hits and priority N. First full match => best_match 
	for(entry = table->entries;entry!=NULL;entry = entry->next){
		unsigned int matched = LOOP_NO_MATCH;
		
		of12_match_t* it = entry->matchs; 

		for(;;){
			if(!of12_check_match(pkt, it))
				break;
			if (it->next == NULL){
				/* Last match, then rule has matched */
				matched = LOOP_MATCH; 
				break;
			}
			it = it->next;
		}

		if(matched){
			//Lock writers to modify the entry while packet processing. WARNING!!!! this must be released by the pipeline, once packet is processed!
			platform_rwlock_rdlock(entry->rwlock);

			//Green light for writers
			platform_rwlock_rdunlock(table->rwlock);
			return entry;
		}
	}
	
	//No match
	//Green light for writers
	platform_rwlock_rdunlock(table->rwlock);
	return NULL; 
}

void
load_matching_algorithm_loop(struct matching_algorithm_functions *f)
{
	if (NULL != f) {
		f->add_flow_entry_hook = of12_add_flow_entry_loop;
		f->modify_flow_entry_hook = of12_modify_flow_entry_loop;
		f->remove_flow_entry_hook = of12_remove_flow_entry_loop;
		f->find_best_match_hook = of12_find_best_match_loop;
		f->dump_hook = NULL;
		f->init_hook = NULL;
		f->destroy_hook = NULL;
		strncpy(f->description, LOOP_DESCRIPTION, OF12_MATCHING_ALGORITHMS_MAX_DESCRIPTION_LENGTH);
	}
}


