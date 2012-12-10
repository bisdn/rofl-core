#include "of12_flow_entry.h"

#include "../../../platform/memory.h"

#include <stdio.h>

/*
* Intializer and destructor
*/

//of12_flow_entry_t* of12_init_flow_entry(const uint16_t priority, of12_match_group_t* match_group, of12_flow_entry_t* prev, of12_flow_entry_t* next){

of12_flow_entry_t*
of12_init_flow_entry(
		of12_flow_entry_t* prev,
		of12_flow_entry_t* next)
{
	of12_flow_entry_t* entry = (of12_flow_entry_t*)cutil_malloc_shared(sizeof(of12_flow_entry_t));
	
	if(!entry)
		return NULL;

	memset(entry,0,sizeof(of12_flow_entry_t));	
	
	if(0 == (entry->rwlock = platform_rwlock_init(NULL))){
		cutil_free_shared(entry);
		return NULL; 
	}

	entry->prev = prev;
	entry->next = next;
	
#if 0
	if(match_group){

		if(of12_add_match_to_entry(entry,matchs)!=EXIT_SUCCESS){
			cutil_free_shared(entry);
			return NULL;
		}
	}
#endif
	
	of12_init_instruction_group(&entry->instructions);
	
	return entry;	

}

unsigned int of12_destroy_flow_entry(of12_flow_entry_t* entry){
	
	of12_match_t* match = entry->matchs;

	//wait for any thread which is still using the entry (processing a packet)
	platform_rwlock_wrlock(&entry->rwlock);
	
	//Destroy matches recursively
	while(match){
		of12_match_t* next = match->next;
		//TODO: maybe check result of destroy and print traces...
		of12_destroy_match(match);
		match = next; 
	}

	//Destroy instructions
	of12_destroy_instruction_group(&entry->instructions);
	
	//TODO statistics counters

	//Destroy entry itself
	cutil_free_shared(entry);	
	
	return EXIT_SUCCESS;
}

//Adds one or more to the entry
unsigned int of12_add_match_to_entry(of12_flow_entry_t* entry, of12_match_t* match){
	unsigned int i=0;

	if(!match)
		return EXIT_FAILURE;
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
	return EXIT_SUCCESS;
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
