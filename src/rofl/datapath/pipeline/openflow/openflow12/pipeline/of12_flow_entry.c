#include "of12_flow_entry.h"

#include "../../../platform/memory.h"
#include "../of12_async_events_hooks.h"

#include <assert.h>
#include "../of12_switch.h"
#include "of12_pipeline.h"
#include "of12_flow_table.h"
#include "of12_action.h"
#include "of12_group_table.h"

#include "../../../util/logging.h"

/*
* Intializer and destructor
*/

of12_flow_entry_t* of12_init_flow_entry(of12_flow_entry_t* prev, of12_flow_entry_t* next, bool notify_removal){

	of12_flow_entry_t* entry = (of12_flow_entry_t*)platform_malloc_shared(sizeof(of12_flow_entry_t));
	
	if(!entry)
		return NULL;

	memset(entry,0,sizeof(of12_flow_entry_t));	
	
	if(NULL == (entry->rwlock = platform_rwlock_init(NULL))){
		platform_free_shared(entry);
		assert(0);
		return NULL; 
	}
	
	//Init linked list
	entry->prev = prev;
	entry->next = next;
	
	__of12_init_instruction_group(&entry->inst_grp);

	//init stats
	__of12_init_flow_stats(entry);

	//Flags
	entry->notify_removal = notify_removal;
	
	return entry;	

}


//This function is meant to only be used internally
rofl_result_t __of12_destroy_flow_entry_with_reason(of12_flow_entry_t* entry, of12_flow_remove_reason_t reason){
	
	of12_match_t* match = entry->matchs;

	//wait for any thread which is still using the entry (processing a packet)
	platform_rwlock_wrlock(entry->rwlock);
	
	//destroying timers, if any
	__of12_destroy_timer_entries(entry);

	//Notify flow removed
	if(entry->notify_removal && (reason != OF12_FLOW_REMOVE_NO_REASON ) ){
		//Safety checks
		if(entry->table && entry->table->pipeline && entry->table->pipeline->sw)
			platform_of12_notify_flow_removed(entry->table->pipeline->sw, reason, entry);	
			
	}	

	//destroy stats
	__of12_destroy_flow_stats(entry);

	//Destroy matches recursively
	while(match){
		of12_match_t* next = match->next;
		//TODO: maybe check result of destroy and print traces...
		of12_destroy_match(match);
		match = next; 
	}

	//Destroy instructions
	__of12_destroy_instruction_group(&entry->inst_grp);
	
	platform_rwlock_destroy(entry->rwlock);
	
	//Destroy entry itself
	platform_free_shared(entry);	
	
	return ROFL_SUCCESS;
}

//This is the interface to be used when deleting entries used as
//a message or not inserted in a table 
rofl_result_t of12_destroy_flow_entry(of12_flow_entry_t* entry){
	return __of12_destroy_flow_entry_with_reason(entry, OF12_FLOW_REMOVE_NO_REASON);	
}

//Adds one or more to the entry
rofl_result_t of12_add_match_to_entry(of12_flow_entry_t* entry, of12_match_t* match){

	unsigned int new_matches;

	if(!match)
		return ROFL_FAILURE;

	if(entry->matchs){
		__of12_add_match(entry->matchs, match);		
	}else{
		entry->matchs = match;

		//Make sure is correctly formed
		match->prev = NULL;

		//Set the number of matches
		entry->num_of_matches=0;
	}

	//Determine number of new matches.
	for(new_matches=0;match;match=match->next,new_matches++);

	entry->num_of_matches+=new_matches;

	return ROFL_SUCCESS;
}

rofl_result_t __of12_update_flow_entry(of12_flow_entry_t* entry_to_update, of12_flow_entry_t* mod, bool reset_counts){


	//Lock entry
	platform_rwlock_wrlock(entry_to_update->rwlock);

	//Copy instructions
	__of12_update_instructions(&entry_to_update->inst_grp, &mod->inst_grp);

	//Reset counts
	if(reset_counts)
		__of12_stats_flow_reset_counts(entry_to_update);

	//Unlock
	platform_rwlock_wrunlock(entry_to_update->rwlock);

	// let the platform do the necessary updates
	platform_of12_modify_entry_hook(entry_to_update, mod, reset_counts);

	//Destroy the mod entry
	__of12_destroy_flow_entry_with_reason(mod, OF12_FLOW_REMOVE_NO_REASON);

	return ROFL_SUCCESS;
}
/**
* Checks whether two entries overlap overlapping. This is potentially an expensive call.
* Try to avoid using it, if the matching algorithm can guess via other (more efficient) ways...
*/
bool __of12_flow_entry_check_overlap(of12_flow_entry_t*const original, of12_flow_entry_t*const entry, bool check_priority, bool check_cookie, uint32_t out_port, uint32_t out_group){

	of12_match_t* it_orig, *it_entry;
	
	//Check cookie first
	if(check_cookie && entry->cookie){
		if( (entry->cookie&entry->cookie_mask) == (original->cookie&entry->cookie_mask) )
			return false;
	}

	//Check priority
	if(check_priority && (entry->priority != original->priority))
		return false;

	//Check if matchs are contained. This is expensive.. //FIXME: move this to of12_match
	for( it_entry = entry->matchs; it_entry; it_entry = it_entry->next ){
		for( it_orig = original->matchs; it_orig; it_orig = it_orig->next ){

			//Skip if different types
			if( it_entry->type != it_orig->type)
				continue;	
			
			if( !__of12_is_submatch( it_entry, it_orig ) && !__of12_is_submatch( it_orig, it_entry ) )
				return false;
		}
	}


	//Check out group actions
	if( out_group != OF12_GROUP_ANY && ( 
			!__of12_write_actions_has(entry->inst_grp.instructions[OF12_IT_WRITE_ACTIONS].write_actions, OF12_AT_GROUP, out_group) &&
			!__of12_apply_actions_has(entry->inst_grp.instructions[OF12_IT_APPLY_ACTIONS].apply_actions, OF12_AT_GROUP, out_group)
			)
	)
		return false;


	//Check out port actions
	if( out_port != OF12_PORT_ANY && ( 
			!__of12_write_actions_has(entry->inst_grp.instructions[OF12_IT_WRITE_ACTIONS].write_actions, OF12_AT_OUTPUT, out_port) &&
			!__of12_apply_actions_has(entry->inst_grp.instructions[OF12_IT_APPLY_ACTIONS].apply_actions, OF12_AT_OUTPUT, out_port)
			)
	)
		return false;

	return true;
}

/**
* Checks whether an entry is contained in the other. This is potentially an expensive call.
* Try to avoid using it, if the matching algorithm can guess via other (more efficient) ways...
*/
bool __of12_flow_entry_check_contained(of12_flow_entry_t*const original, of12_flow_entry_t*const subentry, bool check_priority, bool check_cookie, uint32_t out_port, uint32_t out_group){

	of12_match_t* it_orig, *it_subentry;
	
	//Check cookie first
	if(check_cookie && subentry->cookie_mask){
		if( (subentry->cookie&subentry->cookie_mask) == (original->cookie&subentry->cookie_mask) )
			return false;
	}

	//Check priority
	if(check_priority && (original->priority != subentry->priority))
		return false;

	//Check if matchs are contained. This is expensive.. //FIXME: move this to of12_match
	for( it_subentry = subentry->matchs; it_subentry; it_subentry = it_subentry->next ){
		for( it_orig = original->matchs; it_orig; it_orig = it_orig->next ){
	
			//Skip if different types
			if( it_subentry->type != it_orig->type)
				continue;	
			
			if( !__of12_is_submatch( it_subentry, it_orig ) )
				return false;
		}
	}

	//Check out group actions
	if( out_group != OF12_GROUP_ANY && !(__of12_instruction_has(&subentry->inst_grp,OF12_AT_GROUP,out_group)) )
		return false;


	//Check out port actions
	if( out_port != OF12_PORT_ANY && !(__of12_instruction_has(&subentry->inst_grp,OF12_AT_OUTPUT,out_port)) )
		return false;


	return true;
}
/**
* Checks if entry is identical to another one
*/
bool __of12_flow_entry_check_equal(of12_flow_entry_t*const original, of12_flow_entry_t*const entry, uint32_t out_port, uint32_t out_group){

	of12_match_t* it_original, *it_entry;
	
	//Check cookie first
	if(entry->cookie_mask){
		if( (entry->cookie&entry->cookie_mask) == (original->cookie&entry->cookie_mask) )
			return false;
	}

	//Check priority
	if(entry->priority != original->priority)
		return false;

	//Fast Check #matchs
	if(original->num_of_matches != entry->num_of_matches) 
		return false;

	//Matches in-depth check //FIXME: move this to of12_match
	for(it_original = original->matchs, it_entry = entry->matchs; it_entry != NULL; it_original = it_original->next, it_entry = it_entry->next){	
		if(!__of12_equal_matches(it_original,it_entry))
			return false;
	}

	//Check out group actions
	if( out_group != OF12_GROUP_ANY && ( 
			!__of12_write_actions_has(original->inst_grp.instructions[OF12_IT_WRITE_ACTIONS].write_actions, OF12_AT_GROUP, out_group) &&
			!__of12_apply_actions_has(original->inst_grp.instructions[OF12_IT_APPLY_ACTIONS].apply_actions, OF12_AT_GROUP, out_group)
			)
	)
		return false;


	//Check out port actions
	if( out_port != OF12_PORT_ANY && ( 
			!__of12_write_actions_has(original->inst_grp.instructions[OF12_IT_WRITE_ACTIONS].write_actions, OF12_AT_OUTPUT, out_port) &&
			!__of12_apply_actions_has(original->inst_grp.instructions[OF12_IT_APPLY_ACTIONS].apply_actions, OF12_AT_OUTPUT, out_port)
			)
	)
		return false;


	return true;
}

void of12_dump_flow_entry(of12_flow_entry_t* entry){
	ROFL_PIPELINE_INFO_NO_PREFIX("Entry (%p), #hits %u prior. %u",entry,entry->num_of_matches, entry->priority);
	//print matches(all)
	ROFL_PIPELINE_INFO_NO_PREFIX(" Matches:{");
	of12_dump_matches(entry->matchs);
	ROFL_PIPELINE_INFO_NO_PREFIX("}\n\t\t");
	__of12_dump_instructions(entry->inst_grp);
	ROFL_PIPELINE_INFO_NO_PREFIX("\n");
}

/**
 * check if the entry is valid for insertion
 */
rofl_result_t __of12_validate_flow_entry(of12_group_table_t *gt, of12_flow_entry_t* entry){
	if(__of12_validate_instructions(gt,&entry->inst_grp)!=ROFL_SUCCESS)
		return ROFL_FAILURE;
	
	return ROFL_SUCCESS;
}

