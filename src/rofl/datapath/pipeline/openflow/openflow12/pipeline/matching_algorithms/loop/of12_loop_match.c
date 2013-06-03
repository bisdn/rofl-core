#include "of12_loop_match.h"

#include <stdlib.h>
#include <assert.h>
#include "../../of12_pipeline.h"
#include "../../of12_flow_table.h"
#include "../../of12_flow_entry.h"
#include "../../of12_match.h"
#include "../../of12_group_table.h"
#include "../../of12_instruction.h"
#include "../../../of12_async_events_hooks.h"
#include "../../../../../platform/lock.h"
#include "../matching_algorithms.h"

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
		if( __of12_flow_entry_check_overlap(it, entry, true, check_cookie, out_port, out_group) )
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
		if( __of12_flow_entry_check_equal(it, entry, out_port, out_group) )
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
static rofl_result_t of12_remove_flow_entry_table_specific_imp(of12_flow_table_t *const table, of12_flow_entry_t *const specific_entry, of12_flow_remove_reason_t reason){
	
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

	// let the platform do the necessary cleanup
	platform_of12_remove_entry_hook(specific_entry);

	//Destroy entry
	return __of12_destroy_flow_entry_with_reason(specific_entry, reason);
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

		table->num_of_entries++;

		// let the platform do the necessary add operations
		plaftorm_of12_add_entry_hook(entry);

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
		if(of12_remove_flow_entry_table_specific_imp(table,existing, OF12_FLOW_REMOVE_NO_REASON) != ROFL_SUCCESS)
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

			//Unlock mutexes
			platform_rwlock_wrunlock(table->rwlock);

			// let the platform do the necessary add operations
			plaftorm_of12_add_entry_hook(entry);

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

	// let the platform do the necessary add operations
	plaftorm_of12_add_entry_hook(entry);

	return ROFL_OF12_FM_SUCCESS;
}

/*
* 
* ENTRY removal for non-specific entries. It will remove the FIRST matching entry. This function assumes that match order of table_entry and entry are THE SAME. If not 
* the result is undefined.
*
* This function shall NOT be used if there is some prior knowledge by the lookup algorithm before (specially a pointer to the entry), as it is inherently VERY innefficient
*/

static rofl_result_t of12_remove_flow_entry_table_non_specific_imp(of12_flow_table_t *const table, of12_flow_entry_t *const entry, const enum of12_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group, of12_flow_remove_reason_t reason){

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
			if( __of12_flow_entry_check_equal(it, entry, out_port, out_group ) ){
				
				if(of12_remove_flow_entry_table_specific_imp(table, it, reason) != ROFL_SUCCESS){
					assert(0); //This should never happen
					return ROFL_FAILURE;
				}
				deleted++;
				break;
			}
		}else{
			if( __of12_flow_entry_check_contained(it, entry, strict, true, out_port, out_group) ){
				
				if(of12_remove_flow_entry_table_specific_imp(table, it, reason) != ROFL_SUCCESS){
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

static inline rofl_result_t of12_remove_flow_entry_table_imp(of12_flow_table_t *const table, of12_flow_entry_t *const entry, of12_flow_entry_t *const specific_entry, uint32_t out_port, uint32_t out_group, of12_flow_remove_reason_t reason, const enum of12_flow_removal_strictness strict){

	if( (entry&&specific_entry) || ( !entry && !specific_entry) )
		return ROFL_FAILURE;
 
	if(entry)
		return of12_remove_flow_entry_table_non_specific_imp(table, entry, strict, out_port, out_group, reason);
	else
		return of12_remove_flow_entry_table_specific_imp(table, specific_entry, reason);
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
		return ROFL_SUCCESS; //Acording to spec 

	//Allow single add/remove operation over the table
	platform_mutex_lock(table->mutex);
	
	//Loop over all the table entries	
	for(it=table->entries; it; it=it->next){

		if( strict == STRICT ){
			//Strict make sure they are equal
			if( __of12_flow_entry_check_equal(it, entry, OF12_PORT_ANY, OF12_GROUP_ANY) ){
				if(__of12_update_flow_entry(it, entry, reset_counts) != ROFL_SUCCESS)
					return ROFL_FAILURE;
				moded++;
				break;
			}
		}else{
			if( __of12_flow_entry_check_contained(it, entry, strict, true, OF12_PORT_ANY, OF12_GROUP_ANY) ){
				if(__of12_update_flow_entry(it, entry, reset_counts) != ROFL_SUCCESS)
					return ROFL_FAILURE;
				moded++;
			}
		}
	}

	platform_mutex_unlock(table->mutex);

	//According to spec
	//if(moded == 0)	
	//	return ROFL_FAILURE; 
	
	return ROFL_SUCCESS;
}

rofl_result_t of12_remove_flow_entry_loop(of12_flow_table_t *const table , of12_flow_entry_t *const entry, of12_flow_entry_t *const specific_entry, const enum of12_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group, of12_flow_remove_reason_t reason, of12_mutex_acquisition_required_t mutex_acquired){

	rofl_result_t result;

	//Allow single add/remove operation over the table
	if(!mutex_acquired){
		platform_mutex_lock(table->mutex);
	}
	
	result = of12_remove_flow_entry_table_imp(table, entry, specific_entry, out_port, out_group,reason, strict);

	//Green light to other threads
	if(!mutex_acquired){
		platform_mutex_unlock(table->mutex);
	}

	return result;
}

	
/* FLOW entry lookup entry point */ 
of12_flow_entry_t* of12_find_best_match_loop(of12_flow_table_t *const table, of12_packet_matches_t *const pkt_matches){
	
	of12_match_t* it;
	of12_flow_entry_t *entry;

	//Prevent writers to change structure during matching
	platform_rwlock_rdlock(table->rwlock);
	
	//Table is sorted out by nº of hits and priority N. First full match => best_match 
	for(entry = table->entries;entry!=NULL;entry = entry->next){
		bool matched = true;
		
		for( it=entry->matchs ; it ; it=it->next ){
			if(!__of12_check_match(pkt_matches, it)){
				matched = false;
				break;
			}
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


/*
*
* Statistics
*
*/
rofl_result_t of12_get_flow_stats_loop(struct of12_flow_table *const table,
		uint64_t cookie,
		uint64_t cookie_mask,
		uint32_t out_port, 
		uint32_t out_group,
		of12_match_t *const matchs,
		of12_stats_flow_msg_t* msg){

	of12_flow_entry_t* entry, flow_stats_entry;
	of12_stats_single_flow_msg_t* flow_stats;

	if(!msg || !table)
		return ROFL_FAILURE;

	//Create a flow_stats_entry
	memset(&flow_stats_entry,0,sizeof(of12_flow_entry_t));
	flow_stats_entry.matchs = matchs;
	flow_stats_entry.cookie = cookie;
	flow_stats_entry.cookie_mask = cookie_mask;

	
	//Mark table as being read
	platform_rwlock_rdlock(table->rwlock);


	//Loop over the table and calculate stats
	for(entry = table->entries; entry!=NULL; entry = entry->next){
	
		//Check if is contained 
		if(__of12_flow_entry_check_contained(&flow_stats_entry, entry, false, true, out_port, out_group)){

			// update statistics from platform
			platform_of12_update_stats_hook(entry);

			//Create a new single flow entry and fillin 
			flow_stats = __of12_init_stats_single_flow_msg(entry);
			
			if(!flow_stats)
				return ROFL_FAILURE;	
	
			//Push this stat to the msg
			__of12_push_single_flow_stats_to_msg(msg, flow_stats);	
		}
	
	}

	//Release the table
	platform_rwlock_rdunlock(table->rwlock);

	return ROFL_SUCCESS;
}

rofl_result_t of12_get_flow_aggregate_stats_loop(struct of12_flow_table *const table,
		uint64_t cookie,
		uint64_t cookie_mask,
		uint32_t out_port, 
		uint32_t out_group,
		of12_match_t *const matchs,
		of12_stats_flow_aggregate_msg_t* msg){

	of12_flow_entry_t* entry, flow_stats_entry;

	if(!msg || !table)
		return ROFL_FAILURE;

	//Flow stats entry for easy comparison
	memset(&flow_stats_entry,0,sizeof(of12_flow_entry_t));
	flow_stats_entry.matchs = matchs;
	flow_stats_entry.cookie = cookie;
	flow_stats_entry.cookie_mask = cookie_mask;

	//Mark table as being read
	platform_rwlock_rdlock(table->rwlock);

	//Loop over the table and calculate stats
	for(entry = table->entries; entry!=NULL; entry = entry->next){
	
		//Check if is contained 
		if(__of12_flow_entry_check_contained(&flow_stats_entry, entry, false, true, out_port, out_group)){
			//Increment stats
			msg->packet_count += entry->stats.packet_count;
			msg->byte_count += entry->stats.byte_count;
			msg->flow_count++;
		}
	
	}
	
	//Release the table
	platform_rwlock_rdunlock(table->rwlock);
	
	return ROFL_SUCCESS;
}

/* Group related FLOW entry lookup */ 
of12_flow_entry_t* of12_find_entry_using_group_loop(of12_flow_table_t *const table, const unsigned int group_id){

	of12_match_t* it;
	of12_flow_entry_t *entry;

	//Prevent writers to change structure during matching
	platform_rwlock_rdlock(table->rwlock);
	
	//Find an entry that refers to the group with group_id
	for(entry = table->entries;entry!=NULL;entry = entry->next){
		
		bool has_group = false;	
		
		for( it=entry->matchs; it; it=it->next ){
			if(__of12_instructions_contain_group(entry, group_id)){
				has_group = true;
				break;
			}
		}

		if(has_group){
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

rofl_result_t of12_destroy_loop(struct of12_flow_table *const table){

	of12_flow_entry_t *entry, *next;

	//Destroy all entries
	for(entry = table->entries; entry; entry = next){
		next = entry->next;
		__of12_destroy_flow_entry_with_reason(entry, OF12_FLOW_REMOVE_NO_REASON);
	}

	table->entries = NULL;

	return ROFL_SUCCESS;
}

//Define the matching algorithm struct
OF12_REGISTER_MATCHING_ALGORITHM(loop) = {
	//Init and destroy hooks
	.init_hook = NULL,
	.destroy_hook = of12_destroy_loop,

	//Flow mods
	.add_flow_entry_hook = of12_add_flow_entry_loop,
	.modify_flow_entry_hook = of12_modify_flow_entry_loop,
	.remove_flow_entry_hook = of12_remove_flow_entry_loop,

	//Find best match
	.find_best_match_hook = of12_find_best_match_loop,

	//Stats
	.get_flow_stats_hook = of12_get_flow_stats_loop,
	.get_flow_aggregate_stats_hook = of12_get_flow_aggregate_stats_loop,

	//Find group related entries	
	.find_entry_using_group_hook = of12_find_entry_using_group_loop,

	//Dumping	
	.dump_hook = NULL,
	.description = LOOP_DESCRIPTION,
};


//Register matching algorithm
//WARNING! Name (first parameter) must be the folder name!!!
//OF12_REGISTER_MATCHING_ALGORITHM(loop, of12_loop);
