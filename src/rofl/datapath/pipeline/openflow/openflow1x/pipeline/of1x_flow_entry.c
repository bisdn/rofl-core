#include "of1x_flow_entry.h"

#include "../../../platform/likely.h"
#include "../../../platform/memory.h"
#include "../of1x_async_events_hooks.h"

#include <assert.h>
#include "../of1x_switch.h"
#include "of1x_pipeline.h"
#include "of1x_flow_table.h"
#include "of1x_action.h"
#include "of1x_group_table.h"

#include "../../../util/logging.h"


/*
* Intializer and destructor
*/

of1x_flow_entry_t* of1x_init_flow_entry(of1x_flow_entry_t* prev, of1x_flow_entry_t* next, bool notify_removal){

	of1x_flow_entry_t* entry = (of1x_flow_entry_t*)platform_malloc_shared(sizeof(of1x_flow_entry_t));
	
	if( unlikely(entry==NULL) )
		return NULL;

	memset(entry,0,sizeof(of1x_flow_entry_t));	
	
	entry->rwlock = platform_rwlock_init(NULL);
	if( unlikely(NULL==entry->rwlock) ){
		platform_free_shared(entry);
		assert(0);
		return NULL; 
	}
	
	//Init linked list
	entry->prev = prev;
	entry->next = next;

	//Init matches
	__of1x_init_match_group(&entry->matches);
	
	//Init instructions	
	__of1x_init_instruction_group(&entry->inst_grp);

	//init stats
	__of1x_init_flow_stats(entry);

	//Flags
	entry->notify_removal = notify_removal;
	
	return entry;	

}


//This function is meant to only be used internally
rofl_result_t __of1x_destroy_flow_entry_with_reason(of1x_flow_entry_t* entry, of1x_flow_remove_reason_t reason){
	
	//wait for any thread which is still using the entry (processing a packet)
	platform_rwlock_wrlock(entry->rwlock);
	
	//destroying timers, if any
	__of1x_destroy_timer_entries(entry);

	//Notify flow removed
	if(entry->notify_removal && (reason != OF1X_FLOW_REMOVE_NO_REASON ) ){
		//Safety checks
		if(entry->table && entry->table->pipeline && entry->table->pipeline->sw)
			platform_of1x_notify_flow_removed(entry->table->pipeline->sw, reason, entry);	
			
	}	

	//destroy stats
	__of1x_destroy_flow_stats(entry);

	//Destroy matches group 
	__of1x_destroy_match_group(&entry->matches);

	//Destroy instructions
	__of1x_destroy_instruction_group(&entry->inst_grp);
	
	platform_rwlock_destroy(entry->rwlock);
	
	//Destroy entry itself
	platform_free_shared(entry);	
	
	return ROFL_SUCCESS;
}

//This is the interface to be used when deleting entries used as
//a message or not inserted in a table 
rofl_result_t of1x_destroy_flow_entry(of1x_flow_entry_t* entry){
	return __of1x_destroy_flow_entry_with_reason(entry, OF1X_FLOW_REMOVE_NO_REASON);	
}

//Adds one or more to the entry
rofl_result_t of1x_add_match_to_entry(of1x_flow_entry_t* entry, of1x_match_t* match){

	__of1x_match_group_push_back(&entry->matches, match);
	
	return ROFL_SUCCESS;
#if 0
	unsigned int new_matches;

	if(!match)
		return ROFL_FAILURE;

	if(entry->matchs){
		__of1x_add_match(entry->matchs, match);		
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
#endif
}

rofl_result_t __of1x_update_flow_entry(of1x_flow_entry_t* entry_to_update, of1x_flow_entry_t* mod, bool reset_counts){


	// let the platform do the necessary updates
	platform_of1x_modify_entry_hook(entry_to_update, mod, reset_counts);

	//Lock entry
	platform_rwlock_wrlock(entry_to_update->rwlock);

	//Copy instructions
	__of1x_update_instructions(&entry_to_update->inst_grp, &mod->inst_grp);

	//Reset counts
	if(reset_counts){
		__of1x_stats_flow_reset_counts(entry_to_update);
		__of1x_reset_last_packet_count_idle_timeout(&entry_to_update->timer_info);
	}

	//Unlock
	platform_rwlock_wrunlock(entry_to_update->rwlock);

	return ROFL_SUCCESS;
}
/**
* Checks whether two entries overlap overlapping. This is potentially an expensive call.
* Try to avoid using it, if the matching algorithm can guess via other (more efficient) ways...
* out_port and out_grouap are ALWAYS checked against original flow_entry
*/
bool __of1x_flow_entry_check_overlap(of1x_flow_entry_t*const original, of1x_flow_entry_t*const entry, bool check_priority, bool check_cookie, uint32_t out_port, uint32_t out_group){

	of1x_match_t* it_orig, *it_entry;
	
	//Check cookie first
	if(check_cookie && entry->cookie_mask){
		if( (entry->cookie&entry->cookie_mask) != (original->cookie&entry->cookie_mask) )
			return false;
	}

	//Check priority
	if(check_priority && ((entry->priority&OF1X_2_BYTE_MASK) != (original->priority&OF1X_2_BYTE_MASK))) //In OF1.0 overlapping ignore wildcard flag 
		return false;

	//Check if matches are contained. This is expensive.. //FIXME: move this to of1x_match
	for( it_entry = entry->matches.head; it_entry; it_entry = it_entry->next ){
		for( it_orig = original->matches.head; it_orig; it_orig = it_orig->next ){

			//Skip if different types
			if( it_entry->type != it_orig->type)
				continue;	
			
			if( !__of1x_is_submatch( it_entry, it_orig ) && !__of1x_is_submatch( it_orig, it_entry ) )
				return false;
		}
	}


	//Check out group actions
	if( out_group != OF1X_GROUP_ANY && ( 
			!__of1x_write_actions_has(original->inst_grp.instructions[OF1X_IT_WRITE_ACTIONS].write_actions, OF1X_AT_GROUP, out_group) &&
			!__of1x_apply_actions_has(original->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions, OF1X_AT_GROUP, out_group)
			)
	)
		return false;


	//Check out port actions
	if( out_port != OF1X_PORT_ANY && ( 
			!__of1x_write_actions_has(original->inst_grp.instructions[OF1X_IT_WRITE_ACTIONS].write_actions, OF1X_AT_OUTPUT, out_port) &&
			!__of1x_apply_actions_has(original->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions, OF1X_AT_OUTPUT, out_port)
			)
	)
		return false;

	return true;
}

/**
* Checks whether an entry is contained in the other. This is potentially an expensive call.
* Try to avoid using it, if the matching algorithm can guess via other (more efficient) ways...
*/
bool __of1x_flow_entry_check_contained(of1x_flow_entry_t*const original, of1x_flow_entry_t*const subentry, bool check_priority, bool check_cookie, uint32_t out_port, uint32_t out_group, bool reverse_out_check){

	of1x_match_t* it_orig, *it_subentry;
	
	//Check cookie first
	if(check_cookie && subentry->cookie_mask){
		if( (subentry->cookie&subentry->cookie_mask) != (original->cookie&subentry->cookie_mask) )
			return false;
	}

	//Check priority
	if(check_priority && (original->priority != subentry->priority))
		return false;

	//Check if matches are contained. This is expensive.. //FIXME: move this to of1x_match
	for( it_subentry = subentry->matches.head; it_subentry; it_subentry = it_subentry->next ){
		for( it_orig = original->matches.head; it_orig; it_orig = it_orig->next ){
	
			//Skip if different types
			if( it_subentry->type != it_orig->type)
				continue;	
			
			if( !__of1x_is_submatch( it_subentry, it_orig ) )
				return false;
		}
	}

	//Check out group actions
	if( out_group != OF1X_GROUP_ANY){
		if(!reverse_out_check && !(__of1x_instruction_has(&original->inst_grp,OF1X_AT_GROUP,out_group)) )
			return false;
		else if(reverse_out_check && !(__of1x_instruction_has(&subentry->inst_grp,OF1X_AT_GROUP,out_group)) )
			return false;
	}

	//Check out port actions
	if( out_port != OF1X_PORT_ANY){
		if(!reverse_out_check && !(__of1x_instruction_has(&original->inst_grp,OF1X_AT_OUTPUT,out_port)) )
			return false;
		else if(reverse_out_check && !(__of1x_instruction_has(&subentry->inst_grp,OF1X_AT_OUTPUT,out_port)) )
			return false;
	}


	return true;
}
/**
* Checks if entry is identical to another one
* out_port and out_grouap are ALWAYS checked against original flow_entry
*/
bool __of1x_flow_entry_check_equal(of1x_flow_entry_t*const original, of1x_flow_entry_t*const entry, uint32_t out_port, uint32_t out_group, bool check_cookie){

	of1x_match_t* it_original, *it_entry;
	
	//Check cookie first
	if(check_cookie && entry->cookie_mask){
		if( (entry->cookie&entry->cookie_mask) != (original->cookie&entry->cookie_mask) )
			return false;
	}

	//Check priority
	if(entry->priority != original->priority)
		return false;

	//Fast Check #matches
	if(original->matches.num_elements != entry->matches.num_elements) 
		return false;

	//Matches in-depth check //FIXME: move this to of1x_match
	for(it_original = original->matches.head, it_entry = entry->matches.head; it_entry != NULL; it_original = it_original->next, it_entry = it_entry->next){	
		if(!__of1x_equal_matches(it_original,it_entry))
			return false;
	}

	//Check out group actions
	if( out_group != OF1X_GROUP_ANY && ( 
			!__of1x_write_actions_has(original->inst_grp.instructions[OF1X_IT_WRITE_ACTIONS].write_actions, OF1X_AT_GROUP, out_group) &&
			!__of1x_apply_actions_has(original->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions, OF1X_AT_GROUP, out_group)
			)
	)
		return false;


	//Check out port actions
	if( out_port != OF1X_PORT_ANY && ( 
			!__of1x_write_actions_has(original->inst_grp.instructions[OF1X_IT_WRITE_ACTIONS].write_actions, OF1X_AT_OUTPUT, out_port) &&
			!__of1x_apply_actions_has(original->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions, OF1X_AT_OUTPUT, out_port)
			)
	)
		return false;


	return true;
}

void of1x_dump_flow_entry(of1x_flow_entry_t* entry){
	ROFL_PIPELINE_INFO_NO_PREFIX("Entry (%p), prior. %u #hits %u ",entry, entry->priority, entry->matches.num_elements);
	//print matches(all)
	ROFL_PIPELINE_INFO_NO_PREFIX(" Matches:{");
	__of1x_dump_matches(entry->matches.head);
	ROFL_PIPELINE_INFO_NO_PREFIX("}\n\t\t");
	__of1x_dump_instructions(entry->inst_grp);
	ROFL_PIPELINE_INFO_NO_PREFIX("\n");
}

/**
 * Check if the entry(matches, actions and instructions is valid for insertion) 
 */
rofl_result_t __of1x_validate_flow_entry( of1x_flow_entry_t* entry, of1x_pipeline_t* pipeline){

	of_version_t version = pipeline->sw->of_ver;

	//Validate matches
	if( entry->matches.head)
		if( (version < entry->matches.ver_req.min_ver) ||
		(version > entry->matches.ver_req.max_ver) )
			return ROFL_FAILURE;
		
	//Validate instructions (and actions)
	if(__of1x_validate_instructions(&entry->inst_grp, pipeline)!=ROFL_SUCCESS)
		return ROFL_FAILURE;

	if(version == OF_VERSION_10 && entry->matches.head && !entry->matches.has_wildcard)
		entry->priority |= OF10_NON_WILDCARDED_PRIORITY_FLAG;
	return ROFL_SUCCESS;
}

