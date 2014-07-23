/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "of1x_instruction.h"
#include "of1x_pipeline.h"
#include "../of1x_switch.h"
#include "of1x_flow_entry.h"
#include "of1x_group_table.h"

#include <assert.h> 
#include "../../../util/logging.h"
#include "../../../platform/likely.h"
#include "../../../platform/memory.h"

/* Instructions init and destroyers */ 
static void __of1x_init_instruction(of1x_instruction_t* inst, of1x_instruction_type_t type, of1x_action_group_t* apply_actions, of1x_write_actions_t* write_actions, of1x_write_metadata_t* write_metadata, unsigned int go_to_table){

	if( unlikely(type==0) )
		return;
	
	inst->type = type;
	inst->apply_actions = apply_actions;
	inst->write_actions = write_actions;
	if(write_metadata)
		inst->write_metadata = *write_metadata;

	if(type == OF1X_IT_GOTO_TABLE)
		inst->go_to_table = go_to_table;
	
}

static void __of1x_destroy_instruction(of1x_instruction_t* inst){
	//Check if empty	
	if(inst->type == OF1X_IT_NO_INSTRUCTION)
		return;
	
	if(inst->apply_actions)
		of1x_destroy_action_group(inst->apply_actions);

	if(inst->write_actions)
		__of1x_destroy_write_actions(inst->write_actions);
}

/* Instruction groups init and destroy */
void __of1x_init_instruction_group(of1x_instruction_group_t* group){
	
	platform_memset(group,0,sizeof(of1x_instruction_group_t));	
}

void __of1x_destroy_instruction_group(of1x_instruction_group_t* group){

	unsigned int i;	

	for(i=0;i<OF1X_IT_MAX;i++)
		__of1x_destroy_instruction(&group->instructions[i]);
	
	group->num_of_instructions=0;
} 

//Removal of instruction from the group.
void of1x_remove_instruction_from_the_group(of1x_instruction_group_t* group, of1x_instruction_type_t type){
	
	__of1x_destroy_instruction(&group->instructions[type]);
	group->num_of_instructions--;
}

//Addition of instruction to group
void of1x_add_instruction_to_group(of1x_instruction_group_t* group, of1x_instruction_type_t type, of1x_action_group_t* apply_actions, of1x_write_actions_t* write_actions, of1x_write_metadata_t* write_metadata,  unsigned int go_to_table){

	if(group->instructions[type].type != OF1X_IT_NO_INSTRUCTION)
		of1x_remove_instruction_from_the_group(group,type);
		
	__of1x_init_instruction(&group->instructions[type], type, apply_actions, write_actions, write_metadata, go_to_table);
	group->num_of_instructions++;


	//Note: Num of actions and has_multiple_outputs are calculated during validation of the flow, since
	//they depend on GROUPS which may not exist at this point and we don't know in which LSI will the
	//flow be installed/modified/deleted
}


//Update instructions
rofl_result_t __of1x_update_instructions(of1x_instruction_group_t* group, of1x_instruction_group_t* new_group){
	

	//Apply Actions
	if(__of1x_update_apply_actions(&group->instructions[OF1X_IT_APPLY_ACTIONS].apply_actions, new_group->instructions[OF1X_IT_APPLY_ACTIONS].apply_actions)!=ROFL_SUCCESS)
		return ROFL_FAILURE;	

	//Make sure apply actions inst is marked as NULL, so that is not released
	platform_memset(&new_group->instructions[OF1X_IT_APPLY_ACTIONS],0,sizeof(of1x_instruction_t));

	//Write actions	
	if(__of1x_update_write_actions(&group->instructions[OF1X_IT_WRITE_ACTIONS].write_actions, new_group->instructions[OF1X_IT_WRITE_ACTIONS].write_actions) != ROFL_SUCCESS)
		return ROFL_FAILURE;	

	//Make sure write actions inst is marked as NULL, so that is not freed 
	platform_memset(&new_group->instructions[OF1X_IT_WRITE_ACTIONS],0,sizeof(of1x_instruction_t));


	//Static ones
	//TODO: METADATA && EXPERIMENTER
	
	//Static stuff
	group->instructions[OF1X_IT_CLEAR_ACTIONS] = new_group->instructions[OF1X_IT_CLEAR_ACTIONS];	
	group->instructions[OF1X_IT_GOTO_TABLE] = new_group->instructions[OF1X_IT_GOTO_TABLE];
			

	//Static stuff
	group->num_of_instructions = new_group->num_of_instructions;
	
	return ROFL_SUCCESS;
}

/* Check whether instructions contain group */
bool __of1x_instructions_contain_group(of1x_flow_entry_t *const entry, const unsigned int group_id){

	return __of1x_write_actions_has(entry->inst_grp.instructions[OF1X_IT_WRITE_ACTIONS].write_actions,OF1X_AT_GROUP,group_id)
		|| __of1x_apply_actions_has(entry->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions,OF1X_AT_GROUP,group_id);
}


//Copy (clone) instructions: TODO evaluate if is necessary to check for errors
void __of1x_copy_instruction_group(of1x_instruction_group_t* origin, of1x_instruction_group_t* dest){
	
	unsigned int i;
	
	for(i=0;i<OF1X_IT_MAX;i++){
		
		//Check all instructions in order 
		switch(origin->instructions[i].type){
    			
			case OF1X_IT_CLEAR_ACTIONS: 
			case OF1X_IT_EXPERIMENTER: 
    			case OF1X_IT_WRITE_METADATA:
			case OF1X_IT_GOTO_TABLE:  
    					dest->instructions[i] = origin->instructions[i];	
					break;
			
			case OF1X_IT_APPLY_ACTIONS:  
    					dest->instructions[i] = origin->instructions[i];	
    					dest->instructions[i].apply_actions = __of1x_copy_action_group(origin->instructions[i].apply_actions);	
					break;
			case OF1X_IT_WRITE_ACTIONS: 
    					dest->instructions[i] = origin->instructions[i];	
    					dest->instructions[i].write_actions = __of1x_copy_write_actions(origin->instructions[i].write_actions);	
					break;
			
			default: //Empty instruction 
    					dest->instructions[i] = origin->instructions[i];	
					break;	
		}
	}	
}



void __of1x_dump_instructions(of1x_instruction_group_t group, bool raw_nbo){

	unsigned int i,has_write_actions=0, has_apply_actions=0;

	ROFL_PIPELINE_INFO_NO_PREFIX("Inst->> ");

	for(i=0;i<OF1X_IT_MAX;i++){

		//Check all instructions in order 
		switch(group.instructions[i].type){
			case OF1X_IT_APPLY_ACTIONS:  
					ROFL_PIPELINE_INFO_NO_PREFIX(" APPLY, ");
					has_apply_actions++;
					break;
    			case OF1X_IT_CLEAR_ACTIONS: 
					ROFL_PIPELINE_INFO_NO_PREFIX(" CLEAR, ");
					break;
			case OF1X_IT_WRITE_ACTIONS: 
					ROFL_PIPELINE_INFO_NO_PREFIX(" WRITE, ");
					has_write_actions++;
					break;
    			case OF1X_IT_WRITE_METADATA:
					ROFL_PIPELINE_INFO_NO_PREFIX(" WRITE-META(0x%"PRIx64":0x%"PRIx64"), ", group.instructions[i].write_metadata.metadata, group.instructions[i].write_metadata.metadata_mask);
					break;
			case OF1X_IT_EXPERIMENTER: //TODO:
					ROFL_PIPELINE_INFO_NO_PREFIX(" EXP, ");
					break;
    			case OF1X_IT_GOTO_TABLE:  
					ROFL_PIPELINE_INFO_NO_PREFIX(" GOTO(%u), ",group.instructions[i].go_to_table);
					break;
    			
			case OF1X_IT_METER:  //TODO implement
				assert(0);
				break;
				
			case OF1X_IT_NO_INSTRUCTION: //Empty instruction
				break;
		}
	}
	if( has_apply_actions ){
		ROFL_PIPELINE_INFO_NO_PREFIX("\n\t\t\tAPP.ACTIONs:");
		__of1x_dump_action_group(group.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions, raw_nbo);
	}
	if( has_write_actions ){
		ROFL_PIPELINE_INFO_NO_PREFIX("\n\t\t\tWR.ACTIONs:");
		__of1x_dump_write_actions(group.instructions[OF1X_IT_WRITE_ACTIONS].write_actions, raw_nbo);
	}	
}

bool __of1x_instruction_has(of1x_instruction_group_t *inst_grp, of1x_packet_action_type_t type, uint64_t value){
	///returns true if the action type with the specific value is in the set of instructions.
	
	return ( __of1x_write_actions_has(inst_grp->instructions[OF1X_IT_WRITE_ACTIONS].write_actions, type, value) ||
		__of1x_apply_actions_has(inst_grp->instructions[OF1X_IT_APPLY_ACTIONS].apply_actions, type, value) );
}

rofl_result_t __of1x_validate_instructions(of1x_instruction_group_t* inst_grp, of1x_pipeline_t* pipeline, unsigned int table_id){
	int i, num_of_output_actions=0;
	of1x_group_table_t *gt = pipeline->groups;
	of_version_t version = pipeline->sw->of_ver;
	of1x_flow_table_t* table = &pipeline->tables[table_id];
	
	//if there is a group action we should check that the group exists
	for(i=0;i<OF1X_IT_MAX;i++){
		switch(inst_grp->instructions[i].type){
			case OF1X_IT_NO_INSTRUCTION:
				continue;
				break;
				
			case OF1X_IT_APPLY_ACTIONS:
				if(__of1x_validate_action_group(&table->config.apply_actions, inst_grp->instructions[i].apply_actions, gt) != ROFL_SUCCESS)
					return ROFL_FAILURE;
				num_of_output_actions+=inst_grp->instructions[i].apply_actions->num_of_output_actions;
				if( (version < inst_grp->instructions[i].apply_actions->ver_req.min_ver) ||
			        	(version > inst_grp->instructions[i].apply_actions->ver_req.max_ver) )
					return ROFL_FAILURE;
	
				break;
				
			case OF1X_IT_WRITE_ACTIONS:
				//Fast check WRITE actions supported from 1.2
				if( (version < OF_VERSION_12))	
					return ROFL_FAILURE;

				if(__of1x_validate_write_actions(&table->config.write_actions, inst_grp->instructions[i].write_actions, gt) != ROFL_SUCCESS)
					return ROFL_FAILURE;
		
				num_of_output_actions+=inst_grp->instructions[i].write_actions->num_of_output_actions;
				if( (version < inst_grp->instructions[i].write_actions->ver_req.min_ver) ||
			        	(version > inst_grp->instructions[i].write_actions->ver_req.max_ver) )
					return ROFL_FAILURE;
	
				break;
			
			case OF1X_IT_GOTO_TABLE:
				break;
			case OF1X_IT_WRITE_METADATA:
			case OF1X_IT_CLEAR_ACTIONS:
			case OF1X_IT_EXPERIMENTER:
				//Fast check WRITE actions supported from 1.2
				if( (version < OF_VERSION_12))	
					return ROFL_FAILURE;

				break;
			case OF1X_IT_METER:
				//Fast check WRITE actions supported from 1.3
				if( (version < OF_VERSION_13))	
					return ROFL_FAILURE;

				break;
				
		}
	}
	
	//update has multiple outputs flag
	inst_grp->num_of_outputs = num_of_output_actions;
	
	return ROFL_SUCCESS;
}
