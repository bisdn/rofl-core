#include "of12_flow_entry.h"

#include "../../../platform/memory.h"

#include <stdio.h>

/*
* Intializer and destructor
*/

//of12_flow_entry_t* of12_init_flow_entry(const uint16_t priority, of12_match_group_t* match_group, of12_flow_entry_t* prev, of12_flow_entry_t* next){

of12_flow_entry_t* of12_init_flow_entry(of12_flow_entry_t* prev, of12_flow_entry_t* next){

	of12_flow_entry_t* entry = (of12_flow_entry_t*)cutil_malloc_shared(sizeof(of12_flow_entry_t));
	
	if(!entry)
		return NULL;

	memset(entry,0,sizeof(of12_flow_entry_t));	
	
	if(NULL == (entry->rwlock = platform_rwlock_init(NULL))){
		cutil_free_shared(entry);
		return NULL; 
	}

	entry->prev = prev;
	entry->next = next;
	
#if 0
	if(match_group){

		if(of12_add_match_to_entry(entry,matchs)!=ROFL_SUCCESS){
			cutil_free_shared(entry);
			return NULL;
		}
	}
#endif
	
	of12_init_instruction_group(&entry->instructions);

	//init stats
	of12_stats_flow_init(entry);
	
	return entry;	

}

rofl_result_t of12_destroy_flow_entry(of12_flow_entry_t* entry){
	
	of12_match_t* match = entry->matchs;

	//wait for any thread which is still using the entry (processing a packet)
	platform_rwlock_wrlock(entry->rwlock);
	
	//destroying timers, if any
	of12_destroy_timer_entries(entry);

	//Destroy matches recursively
	while(match){
		of12_match_t* next = match->next;
		//TODO: maybe check result of destroy and print traces...
		of12_destroy_match(match);
		match = next; 
	}

	//Destroy instructions
	of12_destroy_instruction_group(&entry->instructions);
	
	platform_rwlock_destroy(entry->rwlock);
	
	//Destroy entry itself
	cutil_free_shared(entry);	
	
	return ROFL_SUCCESS;
}

//Adds one or more to the entry
rofl_result_t of12_add_match_to_entry(of12_flow_entry_t* entry, of12_match_t* match){
	unsigned int i=0;

	if(!match)
		return ROFL_FAILURE;
	if(entry->matchs){
		of12_add_match(entry->matchs, match);		

		//Set number of matches
		for(;match;match=match->next,i++); //TODO: this could also be done in of12_add_match...

		entry->num_of_matches+=i;
	}else{
		entry->matchs = match;

		//Make sure is correctly formed
		match->prev = NULL;

		for(;match;match=match->next,i++); //TODO: this could also be done in of12_add_match...

		//Set the number of matches
		entry->num_of_matches=i;
	}
	return ROFL_SUCCESS;
}

//Check overlapping
bool of12_flow_entry_check_overlap(of12_flow_entry_t*const original, of12_flow_entry_t*const entry){

//	of12_match_t* it_original, *it_entry;
	
	//Check cookie first
	if(entry->cookie){
		if( (entry->cookie&entry->cookie_mask) == (original->cookie&entry->cookie_mask) )
			return false;
	}

	//Check priority
	if(entry->priority != original->priority)
		return false;

	//Check if matchs are contained
	//WARNING function assumes exact same order!
	/*for(it_new=;;){
		
	}*/

	return true;
}

//Check if entry is identical
bool of12_flow_entry_check_equal(of12_flow_entry_t*const original, of12_flow_entry_t*const entry){

	of12_match_t* it_original, *it_entry;
	
	//Check cookie first
	if(entry->cookie){
		if( (entry->cookie&entry->cookie_mask) == (original->cookie&entry->cookie_mask) )
			return false;
	}

	//Check priority
	if(entry->priority != original->priority)
		return false;

	//Fast Check #matchs
	if(original->num_of_matches != entry->num_of_matches) 
		return false;

	//In-depth check of apply and write actions
	for(it_original = original->matchs, it_entry = entry->matchs; it_entry != NULL; it_original = it_original->next, it_entry = it_entry->next){	
		if(!of12_equal_matches(it_original,it_entry))
			return false;
	}
	
	return true;
}

void of12_dump_flow_entry(of12_flow_entry_t* entry){
	fprintf(stderr,"Entry (%p), #hits %u prior. %u",entry,entry->num_of_matches, entry->priority);
	//print matches(all)
	fprintf(stderr," Matches:{");
	of12_dump_matches(entry->matchs);
	fprintf(stderr,"}\n\t\t");
	of12_dump_instructions(entry->instructions);
	fprintf(stderr,"\n");
}
