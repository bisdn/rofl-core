#include "of1x_loop_ma.h"

#include <stdlib.h>
#include <assert.h>
#include "../../of1x_pipeline.h"
#include "../../of1x_flow_table.h"
#include "../../of1x_flow_entry.h"
#include "../../of1x_match.h"
#include "../../of1x_group_table.h"
#include "../../of1x_instruction.h"
#include "../../../of1x_async_events_hooks.h"
#include "../../../../../platform/lock.h"
#include "../../../../../platform/likely.h"
#include "../../../../../platform/memory.h"
#include "../matching_algorithms.h"
#include "../../../../../util/logging.h"

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
static of1x_flow_entry_t* of1x_flow_table_loop_check_overlapping(of1x_flow_entry_t *const start_entry, of1x_flow_entry_t* entry, bool check_cookie, uint32_t out_port, uint32_t out_group){

	of1x_flow_entry_t* it; //Just for code clarity

	//Empty table
	if( !start_entry )
		return NULL;

	for(it=start_entry; it != NULL; it=it->next){
		if( __of1x_flow_entry_check_overlap(it, entry, true, check_cookie, out_port, out_group) )
			return it;
	}	
	return NULL;
}

/**
* Looks for a previously added entry from the entry pointer by start_entry. This is an EXPENSIVE call
*/
static of1x_flow_entry_t* of1x_flow_table_loop_check_identical(of1x_flow_entry_t *const start_entry, of1x_flow_entry_t* entry, uint32_t out_port, uint32_t out_group, bool check_cookie){

	of1x_flow_entry_t* it; //Just for code clarity

	//Empty table
	if(!start_entry)
		return NULL;

	for(it=start_entry; it != NULL; it=it->next){
		if( __of1x_flow_entry_check_equal(it, entry, out_port, out_group, check_cookie) )
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
static rofl_result_t of1x_remove_flow_entry_table_specific_imp(of1x_flow_table_t *const table, of1x_flow_entry_t *const specific_entry, of1x_flow_remove_reason_t reason){
	
	if( unlikely(table->num_of_entries == 0) ) 
		return ROFL_FAILURE; 

	//Safety checks
	if(unlikely(specific_entry->table != table))
		return ROFL_FAILURE; 
	if(specific_entry->prev && unlikely(specific_entry->prev->next != specific_entry))
		return ROFL_FAILURE; 
	if(specific_entry->next && unlikely(specific_entry->next->prev != specific_entry))
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
	platform_of1x_remove_entry_hook(specific_entry);

	//Destroy entry
	return __of1x_destroy_flow_entry_with_reason(specific_entry, reason);
}

/* 
* Adds flow_entry to the main table. This function is NOT thread safe, and mutual exclusion should be 
* acquired BEFORE this function being called, using table->mutex var. 
*/
static rofl_of1x_fm_result_t of1x_add_flow_entry_table_imp(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, bool check_overlap, bool reset_counts){
	of1x_flow_entry_t *it, *prev, *existing=NULL;
	
	if(unlikely(table->num_of_entries == OF1X_MAX_NUMBER_OF_TABLE_ENTRIES)){
		return ROFL_OF1X_FM_FAILURE; 
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
		plaftorm_of1x_add_entry_hook(entry);

		return ROFL_OF1X_FM_SUCCESS;
	}

	//Check overlapping
	if(check_overlap && of1x_flow_table_loop_check_overlapping(table->entries, entry, false, OF1X_PORT_ANY, OF1X_GROUP_ANY)) //Why spec is saying not to match cookie only in flow_mod add??
		return ROFL_OF1X_FM_OVERLAP;

	//Look for existing entries (only if check_overlap is false)
	if(!check_overlap)
		existing = of1x_flow_table_loop_check_identical(table->entries, entry, OF1X_PORT_ANY, OF1X_GROUP_ANY, false); //According to spec do NOT check cookie

	if(existing){
		ROFL_PIPELINE_DEBUG("[flowmod-add(%p)] Existing entry(%p) will be replaced by (%p)\n", entry, existing, entry);
		
		//There was already an entry. Update it..
		if(!reset_counts){
			ROFL_PIPELINE_DEBUG("[flowmod-add(%p)] Getting counters from (%p)\n", entry, existing);
			entry->stats.packet_count = existing->stats.packet_count; 
			entry->stats.byte_count = existing->stats.byte_count; 
			entry->stats.initial_time = existing->stats.initial_time; 
		}
		
		//Let it add normally...
	}
	
	//Look for appropiate position in the table
	for(it=table->entries,prev=NULL; it!=NULL;prev=it,it=it->next){
//		if(it->num_of_matches<= entry->num_of_matches || ( it->num_of_matches<= entry->num_of_matches && it->priority<entry->priority ) ){ //HITS|PRIORITY
		if(it->priority < entry->priority || (it->priority == entry->priority && it->matches.num_elements <= entry->matches.num_elements) ){ //PRIORITY|HITS
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
			//Unlock mutexes
			platform_rwlock_wrunlock(table->rwlock);

			//Increment the number of entries in the table (safe since we have the mutex acquired)
			table->num_of_entries++;
	
			//Point entry table to us
			entry->table = table;

			//Delete old entry
			if(existing){
				ROFL_PIPELINE_DEBUG("[flowmod-add(%p)] Removing old entry (%p)\n", entry, existing);
				if(of1x_remove_flow_entry_table_specific_imp(table,existing, OF1X_FLOW_REMOVE_NO_REASON) != ROFL_SUCCESS){
					assert(0);
				}
			}

			// let the platform do the necessary add operations
			plaftorm_of1x_add_entry_hook(entry);

			return ROFL_OF1X_FM_SUCCESS;
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
	
	//Unlock mutexes
	platform_rwlock_wrunlock(table->rwlock);
	
	//Increment the number of entries in the table (safe since we have the mutex acquired)
	table->num_of_entries++;

	//Delete old entry
	if(existing){
		ROFL_PIPELINE_DEBUG("[flowmod-add(%p)] Removing old entry (%p)\n", entry, existing);
		if(unlikely(of1x_remove_flow_entry_table_specific_imp(table,existing, OF1X_FLOW_REMOVE_NO_REASON) != ROFL_SUCCESS)){
			assert(0);
		}
	}

	// let the platform do the necessary add operations
	plaftorm_of1x_add_entry_hook(entry);

	return ROFL_OF1X_FM_SUCCESS;
}

/*
* 
* ENTRY removal for non-specific entries. It will remove the FIRST matching entry. This function assumes that match order of table_entry and entry are THE SAME. If not 
* the result is undefined.
*
* This function shall NOT be used if there is some prior knowledge by the lookup algorithm before (specially a pointer to the entry), as it is inherently VERY innefficient
*/

static void of1x_remove_flow_entry_table_trace( of1x_flow_entry_t *const entry, of1x_flow_entry_t *const it, of1x_flow_remove_reason_t reason){
	switch(reason){	
		case OF1X_FLOW_REMOVE_DELETE:
			ROFL_PIPELINE_DEBUG("[flowmod-remove(%p)] Existing entry (%p) will be removed\n", entry, it);
			break;
		case OF1X_FLOW_REMOVE_IDLE_TIMEOUT:
			ROFL_PIPELINE_DEBUG("[flowmod-remove] Removing entry(%p) due to IDLE timeout\n", it);
			break;
		case OF1X_FLOW_REMOVE_HARD_TIMEOUT:
			ROFL_PIPELINE_DEBUG("[flowmod-remove] Removing entry(%p) due to HARD timeout\n", it);
			break;
		case OF1X_FLOW_REMOVE_GROUP_DELETE:
			ROFL_PIPELINE_DEBUG("[flowmod-remove] Removing entry(%p) due to GROUP delete\n", it);
			break;
		default:
			break;	
	}

}

static rofl_result_t of1x_remove_flow_entry_table_non_specific_imp(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, const enum of1x_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group, of1x_flow_remove_reason_t reason){

	int deleted=0; 
	of1x_flow_entry_t *it, *it_next;
	of_version_t ver = table->pipeline->sw->of_ver;

	if(table->num_of_entries == 0) 
		return ROFL_SUCCESS; //according to spec 

	//Loop over all the table entries	
	for(it=table->entries; it; it=it_next){
		
		//Save next item
		it_next = it->next;
		
		if( strict == STRICT ){
			//Strict make sure they are equal
			if( __of1x_flow_entry_check_equal(it, entry, out_port, out_group, true && (ver != OF_VERSION_10)) ){

#ifdef DEBUG
				of1x_remove_flow_entry_table_trace(entry, it, reason);
#endif
				if(of1x_remove_flow_entry_table_specific_imp(table, it, reason) != ROFL_SUCCESS){
					assert(0); //This should never happen
					return ROFL_FAILURE;
				}
				deleted++;
				break;
			}
		}else{
			if( __of1x_flow_entry_check_contained(it, entry, strict, true && (ver != OF_VERSION_10), out_port, out_group,false) ){
				
#ifdef DEBUG
				of1x_remove_flow_entry_table_trace(entry, it, reason);
#endif
				if(of1x_remove_flow_entry_table_specific_imp(table, it, reason) != ROFL_SUCCESS){
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

static inline rofl_result_t of1x_remove_flow_entry_table_imp(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, of1x_flow_entry_t *const specific_entry, uint32_t out_port, uint32_t out_group, of1x_flow_remove_reason_t reason, const enum of1x_flow_removal_strictness strict){

	if( unlikely( (entry&&specific_entry) ) || unlikely( (!entry && !specific_entry) ) )
		return ROFL_FAILURE;
 
	if(entry)
		return of1x_remove_flow_entry_table_non_specific_imp(table, entry, strict, out_port, out_group, reason);
	else
		return of1x_remove_flow_entry_table_specific_imp(table, specific_entry, reason);
}

/* Conveniently wraps call with mutex.  */
rofl_of1x_fm_result_t of1x_add_flow_entry_loop(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, bool check_overlap, bool reset_counts){

	rofl_of1x_fm_result_t return_value;

	//Allow single add/remove operation over the table
	platform_mutex_lock(table->mutex);
	
	return_value = of1x_add_flow_entry_table_imp(table, entry, check_overlap, reset_counts);

	//Green light to other threads
	platform_mutex_unlock(table->mutex);

	return return_value;
}

rofl_result_t of1x_modify_flow_entry_loop(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, const enum of1x_flow_removal_strictness strict, bool reset_counts){

	int moded=0; 
	of1x_flow_entry_t *it;

	//Allow single add/remove operation over the table
	platform_mutex_lock(table->mutex);
	
	//Loop over all the table entries	
	for(it=table->entries; it; it=it->next){

		if( strict == STRICT ){
			//Strict make sure they are equal
			if( __of1x_flow_entry_check_equal(it, entry, OF1X_PORT_ANY, OF1X_GROUP_ANY, true) ){
				ROFL_PIPELINE_DEBUG("[flowmod-modify(%p)] Existing entry (%p) will be updated with (%p)\n", entry, it, entry);
				if(__of1x_update_flow_entry(it, entry, reset_counts) != ROFL_SUCCESS)
					return ROFL_FAILURE;
				moded++;
				break;
			}
		}else{
			if( __of1x_flow_entry_check_contained(it, entry, strict, true, OF1X_PORT_ANY, OF1X_GROUP_ANY,false) ){
				ROFL_PIPELINE_DEBUG("[flowmod-modify(%p)] Existing entry (%p) will be updated with (%p)\n", entry, it, entry);
				if(__of1x_update_flow_entry(it, entry, reset_counts) != ROFL_SUCCESS)
					return ROFL_FAILURE;
				moded++;
			}
		}
	}

	platform_mutex_unlock(table->mutex);

	//According to spec
	if(moded == 0){	
		return of1x_add_flow_entry_loop(table, entry, false, reset_counts);
	}

	ROFL_PIPELINE_DEBUG("[flowmod-modify(%p)] Deleting modifying flowmod \n", entry);
	
	//Delete the original flowmod (modify one)
	of1x_destroy_flow_entry(entry);	

	return ROFL_SUCCESS;
}

rofl_result_t of1x_remove_flow_entry_loop(of1x_flow_table_t *const table , of1x_flow_entry_t *const entry, of1x_flow_entry_t *const specific_entry, const enum of1x_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group, of1x_flow_remove_reason_t reason, of1x_mutex_acquisition_required_t mutex_acquired){

	rofl_result_t result;

	//Allow single add/remove operation over the table
	if(!mutex_acquired){
		platform_mutex_lock(table->mutex);
	}
	
	result = of1x_remove_flow_entry_table_imp(table, entry, specific_entry, out_port, out_group,reason, strict);

	//Green light to other threads
	if(!mutex_acquired){
		platform_mutex_unlock(table->mutex);
	}

	return result;
}

/*
*
* Statistics
*
*/
rofl_result_t of1x_get_flow_stats_loop(struct of1x_flow_table *const table,
		uint64_t cookie,
		uint64_t cookie_mask,
		uint32_t out_port, 
		uint32_t out_group,
		of1x_match_group_t *const matches,
		of1x_stats_flow_msg_t* msg){

	of1x_flow_entry_t* entry, flow_stats_entry;
	of1x_stats_single_flow_msg_t* flow_stats;
	bool check_cookie = (table->pipeline->sw->of_ver != OF_VERSION_10);

	if( unlikely(msg==NULL) || unlikely(table==NULL) )
		return ROFL_FAILURE;

	//Create a flow_stats_entry
	platform_memset(&flow_stats_entry,0,sizeof(of1x_flow_entry_t));
	flow_stats_entry.matches = *matches;
	flow_stats_entry.cookie = cookie;
	flow_stats_entry.cookie_mask = cookie_mask;
	check_cookie = ( table->pipeline->sw->of_ver != OF_VERSION_10 ); //Ignore cookie in OF1.0
	
	//Mark table as being read
	platform_rwlock_rdlock(table->rwlock);


	//Loop over the table and calculate stats
	for(entry = table->entries; entry!=NULL; entry = entry->next){
	
		//Check if is contained 
		if(__of1x_flow_entry_check_contained(&flow_stats_entry, entry, false, check_cookie, out_port, out_group, true)){

			// update statistics from platform
			platform_of1x_update_stats_hook(entry);

			//Create a new single flow entry and fillin 
			flow_stats = __of1x_init_stats_single_flow_msg(entry);
			
			if(!flow_stats)
				return ROFL_FAILURE;	
	
			//Push this stat to the msg
			__of1x_push_single_flow_stats_to_msg(msg, flow_stats);	
		}
	
	}

	//Release the table
	platform_rwlock_rdunlock(table->rwlock);

	return ROFL_SUCCESS;
}

rofl_result_t of1x_get_flow_aggregate_stats_loop(struct of1x_flow_table *const table,
		uint64_t cookie,
		uint64_t cookie_mask,
		uint32_t out_port, 
		uint32_t out_group,
		of1x_match_group_t *const matches,
		of1x_stats_flow_aggregate_msg_t* msg){

	bool check_cookie;
	of1x_flow_entry_t* entry, flow_stats_entry;

	if( unlikely(msg==NULL) || unlikely(table==NULL) )
		return ROFL_FAILURE;

	//Flow stats entry for easy comparison
	platform_memset(&flow_stats_entry,0,sizeof(of1x_flow_entry_t));
	flow_stats_entry.matches = *matches;
	flow_stats_entry.cookie = cookie;
	flow_stats_entry.cookie_mask = cookie_mask;
	check_cookie = ( table->pipeline->sw->of_ver != OF_VERSION_10 ); //Ignore cookie in OF1.0

	//Mark table as being read
	platform_rwlock_rdlock(table->rwlock);

	//Loop over the table and calculate stats
	for(entry = table->entries; entry!=NULL; entry = entry->next){
	
		//Check if is contained 
		if(__of1x_flow_entry_check_contained(&flow_stats_entry, entry, false, check_cookie, out_port, out_group,true)){
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
of1x_flow_entry_t* of1x_find_entry_using_group_loop(of1x_flow_table_t *const table, const unsigned int group_id){

	of1x_match_t* it;
	of1x_flow_entry_t *entry;

	//Prevent writers to change structure during matching
	platform_rwlock_rdlock(table->rwlock);
	
	//Find an entry that refers to the group with group_id
	for(entry = table->entries;entry!=NULL;entry = entry->next){
		
		bool has_group = false;	
		
		for( it=entry->matches.head; it; it=it->next ){
			if(__of1x_instructions_contain_group(entry, group_id)){
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

rofl_result_t of1x_destroy_loop(struct of1x_flow_table *const table){

	of1x_flow_entry_t *entry, *next;

	//Destroy all entries
	for(entry = table->entries; entry; entry = next){
		next = entry->next;
		__of1x_destroy_flow_entry_with_reason(entry, OF1X_FLOW_REMOVE_NO_REASON);
	}

	table->entries = NULL;

	return ROFL_SUCCESS;
}

//Define the matching algorithm struct
OF1X_REGISTER_MATCHING_ALGORITHM(loop) = {
	//Init and destroy hooks
	.init_hook = NULL,
	.destroy_hook = of1x_destroy_loop,

	//Flow mods
	.add_flow_entry_hook = of1x_add_flow_entry_loop,
	.modify_flow_entry_hook = of1x_modify_flow_entry_loop,
	.remove_flow_entry_hook = of1x_remove_flow_entry_loop,

#if 0
	//Find best match
	.find_best_match_hook = of1x_find_best_match_loop_ma,
#endif
	//Stats
	.get_flow_stats_hook = of1x_get_flow_stats_loop,
	.get_flow_aggregate_stats_hook = of1x_get_flow_aggregate_stats_loop,

	//Find group related entries	
	.find_entry_using_group_hook = of1x_find_entry_using_group_loop,

	//Dumping	
	.dump_hook = NULL,
	.description = LOOP_DESCRIPTION,
};


//Register matching algorithm
//WARNING! Name (first parameter) must be the folder name!!!
//OF1X_REGISTER_MATCHING_ALGORITHM(loop, of1x_loop);
