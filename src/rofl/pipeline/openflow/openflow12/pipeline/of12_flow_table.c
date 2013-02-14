#include "of12_flow_table.h"

#include <stdio.h>

/* 
* Openflow table operations
*
* Assumptions:
* - A Table MUST always have entries ordered by Nº of OF hits first, and for the priority field in second place
* - A Table contains no more than OF12_MAX_NUMBER_OF_TABLE_ENTRIES
* -  
*/

/*
* Table mgmt 
*/ 

/* Initalizer. Table struct has been allocated by pipeline initializer. */
unsigned int of12_init_table(of12_flow_table_t* table, const unsigned int table_index, const of12_flow_table_config_t config, const enum matching_algorithm_available algorithm){

	//Initializing mutexes
	if(NULL == (table->mutex = platform_mutex_init(NULL)))
		return EXIT_FAILURE;
	if(NULL == (table->rwlock = platform_rwlock_init(NULL)))
		return EXIT_FAILURE;
	
	table->number = table_index;
	table->entries = NULL;
	table->num_of_entries = 0;
	table->default_action = config;
	
	//Setting up the matching algorithm	
	load_matching_algorithm(algorithm, &table->maf);

	//Auxiliary matching algorithm structs 
	table->matching_aux[0] = NULL; 
	table->matching_aux[1] = NULL;

	//Initializing timers. NOTE does that need to be done here or somewhere else?
#if OF12_TIMER_STATIC_ALLOCATION_SLOTS
	of12_timer_group_static_init(table);
#else
	table->timers = NULL;
#endif
	
	if(table->maf.init_hook)
		return table->maf.init_hook(table);
	
	return EXIT_SUCCESS;
}

/* Destructor. Table object is freed by pipeline destructor */
unsigned int of12_destroy_table(of12_flow_table_t* table){
	
	of12_flow_entry_t* entry;
	
	platform_mutex_lock(table->mutex);
	platform_rwlock_wrlock(table->rwlock);
	
	entry = table->entries; 
	while(entry){
		of12_flow_entry_t* next = entry->next;
		//TODO: maybe check result of destroy and print traces...	
		of12_destroy_flow_entry(entry);		
		entry = next; 
	}

	if(table->maf.destroy_hook)
		table->maf.destroy_hook(table);

	platform_mutex_destroy(table->mutex);
	platform_rwlock_destroy(table->rwlock);
	
	//NOTE missing destruction of timers
	
	//Do NOT free table, since it was allocated in a single buffer in pipeline.c	
	return EXIT_SUCCESS;
}

/*
*    TODO:
*    - Use hashs for entries and a hashmap ? 
*    - Add checkings entry (overlap, non overlap)
*    - Add timers
*/

/* 
* Adds flow_entry to the main table. This function is NOT thread safe, and mutual exclusion should be 
* acquired BEFORE this function being called, using table->mutex var. 
*/
unsigned int of12_add_flow_entry_table_imp(of12_flow_table_t *const table, of12_flow_entry_t *const entry){
	of12_flow_entry_t *it, *prev;
	
	if(table->num_of_entries == OF12_MAX_NUMBER_OF_TABLE_ENTRIES)
	{
		return EXIT_FAILURE; 
	}

	if(!table->entries){
		//No rule yet
		entry->prev = NULL;
		entry->next = NULL;
		table->entries = entry;
		//Point entry table to us
		entry->table = table;
	
		table->num_of_entries++;
		return EXIT_SUCCESS;
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
			return EXIT_SUCCESS;
		}
	}
	
	//Append at the end of the table
	entry->next = NULL;
	
	platform_rwlock_wrlock(table->rwlock);
	prev->next = entry;
	table->num_of_entries++;
	
	//Unlock mutexes
	platform_rwlock_wrunlock(table->rwlock);
	return EXIT_SUCCESS;
}

/*
* Removal of specific entry
* Warning pointer to the entry MUST be a valid pointer. Some rudimentary checking are made, such checking linked list correct state, but no further checkings are done
* In particular
*
*/
static of12_flow_entry_t* of12_remove_flow_entry_table_specific_imp(of12_flow_table_t *const table, of12_flow_entry_t *const specific_entry){
	
	if(table->num_of_entries == 0) 
		return NULL; 

	//Safety checks
	if(specific_entry->table != table)
		return NULL; 
	if(specific_entry->prev && specific_entry->prev->next != specific_entry)
		return NULL; 
	if(specific_entry->next && specific_entry->next->prev != specific_entry)
		return NULL; 

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

	//WE DONT'T DESTROY the rule, only deatach it from the table. Destroying is up to the driver
	return specific_entry;

}
/*
* 
* ENTRY removal for non-specific entries. It will remove the FIRST matching entry. This function assumes that match order of table_entry and entry are THE SAME. If not 
* the result is undefined.
*
* This function shall NOT be used if there is some prior knowledge by the lookup algorithm before (specially a pointer to the entry), as it is inherently VERY innefficient
*/

of12_flow_entry_t* of12_remove_flow_entry_table_non_specific_imp(of12_flow_table_t *const table, of12_flow_entry_t *const entry, const enum of12_flow_removal_strictness_t strict){
	int already_matched;
	of12_flow_entry_t *it;
	of12_match_t *match_it, *table_entry_match_it;

	if(table->num_of_entries == 0) 
		return NULL; 

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
	
	return NULL;
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

of12_flow_entry_t* of12_remove_flow_entry_table_imp(of12_flow_table_t *const table, of12_flow_entry_t *const entry, of12_flow_entry_t *const specific_entry, const enum of12_flow_removal_strictness_t strict){

	if( (entry&&specific_entry) || ( !entry && !specific_entry) )
		return NULL; 
	if(entry)
		return of12_remove_flow_entry_table_non_specific_imp(table, entry, strict);
	else
		return of12_remove_flow_entry_table_specific_imp(table, specific_entry);
}
/* 
* Interfaces for generic add/remove flow entry 
* Specific matchings may point them to their own routines, but they MUST always call
* of12_[whatever]_flow_entry_table_imp in order to update the main tables
*/
inline unsigned int of12_add_flow_entry_table(of12_flow_table_t *const table, of12_flow_entry_t *const entry){
	return table->maf.add_flow_entry_hook(table,entry);
}
inline unsigned int of12_remove_flow_entry_table(of12_flow_table_t *const table, of12_flow_entry_t *const entry, of12_flow_entry_t *const specific_entry, const enum of12_flow_removal_strictness_t strict, of12_mutex_acquisition_required_t mutex_acquired ){
	return table->maf.remove_flow_entry_hook(table,entry,specific_entry,strict, mutex_acquired);
}

/* Main process_packet_through */
inline of12_flow_entry_t* of12_find_best_match_table(of12_flow_table_t *const table, of12_packet_matches_t *const pkt){
	return table->maf.find_best_match_hook(table,pkt);
}	


/* Dump methods */
void of12_dump_table(of12_flow_table_t* table){
	of12_flow_entry_t* entry;
	int i;	

	fprintf(stderr,"\nDumping table # %u (%p). Default action: %u. # of entries: %d\n", table->number, table, table->default_action,table->num_of_entries);	
	if(!table->entries){
		fprintf(stderr,"\t[*] No entries\n");
		return;	
	}
	for(entry=table->entries, i=0;entry!=NULL;entry=entry->next,i++){
		fprintf(stderr,"\t[%d] ",i);
		of12_dump_flow_entry(entry);
	}
	
	fprintf(stderr,"\t[*] No more entries...\n");
	
	if(table->maf.dump_hook)
		table->maf.dump_hook(table);
}
