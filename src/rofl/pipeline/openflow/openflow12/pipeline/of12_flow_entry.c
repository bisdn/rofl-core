#include "of12_flow_entry.h"

#include "../../../platform/memory.h"

#include <stdio.h>
#include <assert.h>

/*
* Intializer and destructor
*/

//of12_flow_entry_t* of12_init_flow_entry(const uint16_t priority, of12_match_group_t* match_group, of12_flow_entry_t* prev, of12_flow_entry_t* next){

of12_flow_entry_t* of12_init_flow_entry(of12_flow_entry_t* prev, of12_flow_entry_t* next, bool notify_removal){

	of12_flow_entry_t* entry = (of12_flow_entry_t*)cutil_malloc_shared(sizeof(of12_flow_entry_t));
	
	if(!entry)
		return NULL;

	memset(entry,0,sizeof(of12_flow_entry_t));	
	
	if(NULL == (entry->rwlock = platform_rwlock_init(NULL))){
		cutil_free_shared(entry);
		assert(0);
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

	//Flags
	entry->notify_removal = notify_removal;
	
	return entry;	

}

rofl_result_t of12_destroy_flow_entry(of12_flow_entry_t* entry){
	
	of12_match_t* match = entry->matchs;

	//wait for any thread which is still using the entry (processing a packet)
	platform_rwlock_wrlock(entry->rwlock);
	
	//destroying timers, if any
	of12_destroy_timer_entries(entry);


	//FIXME TODO XXX Implement flow_removed message

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

/**
* Checks whether two entries overlap overlapping. This is potentially an expensive call.
* Try to avoid using it, if the matching algorithm can guess via other (more efficient) ways...
*/
bool of12_flow_entry_check_overlap(of12_flow_entry_t*const original, of12_flow_entry_t*const entry, bool check_cookie){

	of12_match_t* it_orig, *it_entry;
	
	//Check cookie first
	if(check_cookie && entry->cookie){
		if( (entry->cookie&entry->cookie_mask) == (original->cookie&entry->cookie_mask) )
			return false;
	}

	//Check priority
	if(entry->priority != original->priority)
		return false;

	//Check if matchs are contained. This is expensive..
	for( it_entry = entry->matchs; it_entry; it_entry = it_entry->next ){
		for( it_orig = original->matchs; it_orig; it_orig = it_orig->next ){

			//Skip if different types
			if( it_entry->type != it_orig->type)
				continue;	
			
			if( of12_is_submatch( it_entry, it_orig ) || of12_is_submatch( it_orig, it_entry ) )
				return false;
		}
	}

	return true;
}

/**
* Checks whether an entry is contained in the other. This is potentially an expensive call.
* Try to avoid using it, if the matching algorithm can guess via other (more efficient) ways...
*/
bool of12_flow_entry_check_contained(of12_flow_entry_t*const original, of12_flow_entry_t*const subentry, bool check_cookie){

	of12_match_t* it_orig, *it_subentry;
	
	//Check cookie first
	if(check_cookie && subentry->cookie){
		if( (subentry->cookie&subentry->cookie_mask) == (original->cookie&subentry->cookie_mask) )
			return false;
	}

	//Check priority
	if(subentry->priority != original->priority)
		return false;

	//Check if matchs are contained. This is expensive..
	for( it_subentry = subentry->matchs; it_subentry; it_subentry = it_subentry->next ){
		for( it_orig = original->matchs; it_orig; it_orig = it_orig->next ){
	
			//Skip if different types
			if( it_subentry->type != it_orig->type)
				continue;	
			
			if( of12_is_submatch( it_subentry, it_orig ) )
				return false;
		}
	}

	return true;
}
/**
* Checks if entry is identical to another one
*/
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

	//Matches in-depth check
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
