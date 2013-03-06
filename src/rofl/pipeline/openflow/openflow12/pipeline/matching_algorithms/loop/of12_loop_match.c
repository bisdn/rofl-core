#include "of12_loop_match.h"

#include <stdlib.h>
#include "../../../../../util/rofl_pipeline_utils.h"
#include "../../of12_flow_table.h"
#include "../../of12_flow_entry.h"
#include "../../of12_match.h"
#include "../../../../../platform/lock.h"

#define LOOP_NO_MATCH 0
#define LOOP_MATCH 1
#define LOOP_DESCRIPTION "The loop algorithm searches the list of entries by its priority order. On the worst case the performance is o(N) with the number of entries"

/*
*    TODO:
*    - Use hashs for entries and a hashmap ? 
*/

/**
* Looks for an overlapping entry from the entry pointer by start_entry
*/
static of12_flow_entry_t* of12_flow_table_loop_check_overlapping(of12_flow_entry_t *const start_entry, of12_flow_entry_t* entry){

	of12_flow_entry_t* it; //Just for code clarity

	//Empty table
	if(!start_entry)
		return NULL;

	for(it=start_entry; it->next != NULL; it=it->next){
		if( of12_flow_entry_check_overlap(it, entry) )
			return it;
	}	
	return NULL;
}

/**
* Looks for a previously added entry from the entry pointer by start_entry
*/
static of12_flow_entry_t* of12_flow_table_loop_check_identical(of12_flow_entry_t *const start_entry, of12_flow_entry_t* entry){

	of12_flow_entry_t* it; //Just for code clarity

	//Empty table
	if(!start_entry)
		return NULL;

	for(it=start_entry; it->next != NULL; it=it->next){
		if( of12_flow_entry_check_equal(it, entry) )
			return it;
	}	
	return NULL;
}


/*
*
* Removal of specific entry
* Warning pointer to the entry MUST be a valid pointer. Some rudimentary checking are made, such checking linked list correct state, but no further checkings are done
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
	
		table->num_of_entries++;
		return ROFL_OF12_FM_SUCCESS;
	}

	//Check overlapping
	if(check_overlap && of12_flow_table_loop_check_overlapping(table->entries, entry))
		return ROFL_OF12_FM_OVERLAP;

	//Look for existing entries (only if check_overlap is false)
	if(!check_overlap)
		existing = of12_flow_table_loop_check_identical(table->entries, entry);

	if(existing){
		//There was already an entry. Update it..
		if(!reset_counts)
			entry->stats = existing->stats; 

		//Delete old entry
		if(!of12_remove_flow_entry_table_specific_imp(table,existing))
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
			return ROFL_OF12_FM_SUCCESS;
		}
	}
	
	//Append at the end of the table
	entry->next = NULL;
	
	platform_rwlock_wrlock(table->rwlock);
	prev->next = entry;
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
	int already_matched;
	of12_flow_entry_t *it;
	of12_match_t *match_it, *table_entry_match_it;

	if(table->num_of_entries == 0) 
		return ROFL_FAILURE; 

	//Loop over all the table entries	
	for(it=table->entries; it; it=it->next){
		if(it->num_of_matches<entry->num_of_matches) //Fast skipping, if table iterator is more restrictive than entry
			continue;
		if(strict && (entry->priority != it->priority || entry->num_of_matches != it->num_of_matches)) //Fast skipping for strict
			continue;
	
		for(match_it = entry->matchs, already_matched=0; match_it; match_it = match_it->next){
			for(table_entry_match_it = it->matchs; table_entry_match_it; table_entry_match_it = table_entry_match_it->next){
				//Fast skip
				if(match_it->type != table_entry_match_it->type)
					continue;

				if(strict){
					//Check if the matches are exactly the same ones
					if(of12_equal_matches(match_it, table_entry_match_it)){
						already_matched++;
							
						//Check if the number of matches are the same, and entry has no more matches	
						if(already_matched == entry->num_of_matches) //No need to check #hits as entry->num_of_matches == it->num_of_matches
							//WE DONT'T DESTROY the rule, only deatach it from the table. Destroying is up to the matching algorithm 
							return of12_remove_flow_entry_table_specific_imp(table, it);	
						else
							break; //Next entry match, skip rest	
					}
				}else{
					if(!of12_is_submatch(match_it,table_entry_match_it)){
						//If not a subset. Signal to skip
						already_matched = -1;
					}
					break; //Skip rest of the matches
				}
			}
			if(already_matched < 0) //If not strict and there was a match out-of-scope, skip rest of matches
				break;
		}
		if(!strict && already_matched ==0){ //That means entry is within entry vect. space 
			//WE DONT'T DESTROY the rule, only deatach it from the table. Destroying is up to the matching algorithm 
			return of12_remove_flow_entry_table_specific_imp(table, it);
		}
	}
	
	return ROFL_FAILURE; 
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

/* Flow management routines. Wraps call with mutex.  */
 rofl_of12_fm_result_t of12_add_flow_entry_loop(of12_flow_table_t *const table, of12_flow_entry_t *const entry, bool check_overlap, bool reset_counts){

	rofl_of12_fm_result_t return_value;

	//Allow single add/remove operation over the table
	platform_mutex_lock(table->mutex);
	
	return_value = of12_add_flow_entry_table_imp(table, entry, check_overlap, reset_counts);

	//FIXME TODO
	//if(mutex_acquired!=MUTEX_ALREADY_ACQUIRED_BY_TIMER_EXPIRATION)
	//{
		//Add/update counters
	//Add/update timers NOTE check return value;
	of12_add_timer(table,entry);
	//}

	//Green light to other threads
	platform_mutex_unlock(table->mutex);

	return return_value;
}

rofl_result_t of12_modify_flow_entry_loop(of12_flow_table_t *const table, of12_flow_entry_t *const entry, const enum of12_flow_removal_strictness strict, bool reset_counts){

	//TODO: implement

	return ROFL_FAILURE;
}

rofl_result_t of12_remove_flow_entry_loop(of12_flow_table_t *const table , of12_flow_entry_t *const entry, of12_flow_entry_t *const specific_entry, const enum of12_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group, of12_mutex_acquisition_required_t mutex_acquired){

	rofl_result_t result;

	//Allow single add/remove operation over the table
	if(!mutex_acquired){
		platform_mutex_lock(table->mutex);
	}
	
	result = of12_remove_flow_entry_table_imp(table, entry, specific_entry, strict, out_port, out_group);

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


