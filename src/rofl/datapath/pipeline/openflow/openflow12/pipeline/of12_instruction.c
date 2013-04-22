#include "of12_instruction.h"
#include "of12_flow_entry.h"
#include "of12_group_table.h"

#include "../../../util/logging.h"

/* Instructions init and destroyers */ 
static void of12_init_instruction(of12_instruction_t* inst, of12_instruction_type_t type, of12_action_group_t* apply_actions, of12_write_actions_t* write_actions, unsigned int go_to_table){

	if(!type)
		return;
	
	inst->type = type;
	inst->apply_actions = apply_actions;
	inst->write_actions = write_actions;

	if(type == OF12_IT_GOTO_TABLE)
		inst->go_to_table = go_to_table;
	
}



static void of12_destroy_instruction(of12_instruction_t* inst){
	//Check if empty	
	if(inst->type == OF12_IT_NO_INSTRUCTION)
		return;
	
	if(inst->apply_actions)
		of12_destroy_action_group(inst->apply_actions);

	if(inst->write_actions)
		of12_destroy_write_actions(inst->write_actions);
}

/* Instruction groups init and destroy */
void of12_init_instruction_group(of12_instruction_group_t* group){
	
	memset(group,0,sizeof(of12_instruction_group_t));	
}

void of12_destroy_instruction_group(of12_instruction_group_t* group){

	unsigned int i;	

	for(i=0;i<OF12_IT_GOTO_TABLE;i++)
		of12_destroy_instruction(&group->instructions[i]);
	
	group->num_of_instructions=0;
} 

#define OF12_SAFE_IT_TYPE_INDEX(m)\
	m-1

//Removal of instruction from the group.
void of12_remove_instruction_from_the_group(of12_instruction_group_t* group, of12_instruction_type_t type){
	
	of12_destroy_instruction(&group->instructions[OF12_SAFE_IT_TYPE_INDEX(type)]);
	group->num_of_instructions--;
}

//Addition of instruction to group
void of12_add_instruction_to_group(of12_instruction_group_t* group, of12_instruction_type_t type, of12_action_group_t* apply_actions, of12_write_actions_t* write_actions, unsigned int go_to_table){

	unsigned int num_of_outputs=0;
	
	if(group->instructions[OF12_SAFE_IT_TYPE_INDEX(type)].type != OF12_IT_NO_INSTRUCTION)
		of12_remove_instruction_from_the_group(group,type);
		
	of12_init_instruction(&group->instructions[OF12_SAFE_IT_TYPE_INDEX(type)], type, apply_actions, write_actions, go_to_table);
	group->num_of_instructions++;

	//Set flag for lazy copying
	if(group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_APPLY_ACTIONS)].apply_actions)
		num_of_outputs += group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_APPLY_ACTIONS)].apply_actions->num_of_output_actions;
	
	if(group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions && group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions->write_actions[OF12_AT_OUTPUT].type != OF12_AT_NO_ACTION)
		num_of_outputs++;
	//if(group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions && group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions->write_actions[OF12_AT_GROUP].type != OF12_AT_NO_ACTION)
		//num_of_outputs+=group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions->write_actions[OF12_AT_GROUP].group->num_of_output_actions;

	//Assign flag
	group->has_multiple_outputs =  (num_of_outputs > 1); 
}


//Update instructions
rofl_result_t of12_update_instructions(of12_instruction_group_t* group, of12_instruction_group_t* new_group){
	

	//Apply Actions
	if(of12_update_apply_actions(&group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_APPLY_ACTIONS)].apply_actions, new_group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_APPLY_ACTIONS)].apply_actions)!=ROFL_SUCCESS)
		return ROFL_FAILURE;	

	//Make sure apply actions inst is marked as NULL, so that is not released
	memset(&new_group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_APPLY_ACTIONS)],0,sizeof(of12_instruction_t));

	//Write actions	
	if(of12_update_write_actions(&group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions, new_group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions) != ROFL_SUCCESS)
		return ROFL_FAILURE;	

	//Make sure write actions inst is marked as NULL, so that is not freed 
	memset(&new_group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)],0,sizeof(of12_instruction_t));


	//Static ones
	//TODO: METADATA && EXPERIMENTER
	
	//Static stuff
	group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_CLEAR_ACTIONS)] = new_group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_CLEAR_ACTIONS)];	
	group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_GOTO_TABLE)] = new_group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_GOTO_TABLE)];
			

	//Static stuff
	group->num_of_instructions = new_group->num_of_instructions;
	group->has_multiple_outputs = new_group->has_multiple_outputs;
	
	return ROFL_SUCCESS;
}

/* Check whether instructions contain group */
bool of12_instructions_contain_group(of12_flow_entry_t *const entry, const unsigned int group_id){

	return of12_write_actions_has(entry->inst_grp.instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions,OF12_AT_GROUP,group_id)
		|| of12_apply_actions_has(entry->inst_grp.instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_APPLY_ACTIONS)].apply_actions,OF12_AT_GROUP,group_id);
}

/* Process instructions */
unsigned int of12_process_instructions(const struct of12_switch* sw, const unsigned int table_id, datapacket_t *const pkt, const of12_instruction_group_t* instructions){

	unsigned int i;

	for(i=0;i<OF12_IT_GOTO_TABLE;i++){
	
		//Check all instructions in order 
		switch(instructions->instructions[i].type){
			case OF12_IT_APPLY_ACTIONS: of12_process_apply_actions(sw, table_id, pkt,instructions->instructions[i].apply_actions, instructions->has_multiple_outputs); 
					break;
    			case OF12_IT_CLEAR_ACTIONS: of12_clear_write_actions((of12_write_actions_t*)pkt->write_actions);
					break;
			case OF12_IT_WRITE_ACTIONS: of12_update_packet_write_actions(pkt, instructions->instructions[i].write_actions);
					break;
    			case OF12_IT_WRITE_METADATA: //TODO:
					break;
			case OF12_IT_EXPERIMENTER: //TODO:
					break;
    			case OF12_IT_GOTO_TABLE: return instructions->instructions[i].go_to_table; 
					break;
				
			default: //Empty instruction 
				break;
		}
	}		

	return 0; //NO go-to-table
}

//Copy (clone) instructions: TODO evaluate if is necessary to check for errors
void of12_copy_instruction_group(of12_instruction_group_t* origin, of12_instruction_group_t* dest){
	
	unsigned int i;
	
	for(i=0;i<OF12_IT_GOTO_TABLE;i++){
		
		//Check all instructions in order 
		switch(origin->instructions[i].type){
    			
			case OF12_IT_CLEAR_ACTIONS: 
			case OF12_IT_EXPERIMENTER: 
    			case OF12_IT_WRITE_METADATA:
			case OF12_IT_GOTO_TABLE:  
    					dest->instructions[i] = origin->instructions[i];	
					break;
			
			case OF12_IT_APPLY_ACTIONS:  
    					dest->instructions[i] = origin->instructions[i];	
    					dest->instructions[i].apply_actions = of12_copy_action_group(origin->instructions[i].apply_actions);	
					break;
			case OF12_IT_WRITE_ACTIONS: 
    					dest->instructions[i] = origin->instructions[i];	
    					dest->instructions[i].write_actions = of12_copy_write_actions(origin->instructions[i].write_actions);	
					break;
			
			default: //Empty instruction 
    					dest->instructions[i] = origin->instructions[i];	
					break;	
		}
	}	
}



void of12_dump_instructions(of12_instruction_group_t group){

	unsigned int i,has_write_actions=0, has_apply_actions=0;

	ROFL_PIPELINE_INFO_NO_PREFIX("Inst->> ");

	for(i=0;i<OF12_IT_GOTO_TABLE;i++){

		//Check all instructions in order 
		switch(group.instructions[i].type){
			case OF12_IT_APPLY_ACTIONS:  
					ROFL_PIPELINE_INFO_NO_PREFIX(" APPLY, ");
					has_apply_actions++;
					break;
    			case OF12_IT_CLEAR_ACTIONS: 
					ROFL_PIPELINE_INFO_NO_PREFIX(" CLEAR, ");
					break;
			case OF12_IT_WRITE_ACTIONS: 
					ROFL_PIPELINE_INFO_NO_PREFIX(" WRITE, ");
					has_write_actions++;
					break;
    			case OF12_IT_WRITE_METADATA: //TODO:
					ROFL_PIPELINE_INFO_NO_PREFIX(" WRITE-META, ");
					break;
			case OF12_IT_EXPERIMENTER: //TODO:
					ROFL_PIPELINE_INFO_NO_PREFIX(" EXP, ");
					break;
    			case OF12_IT_GOTO_TABLE:  
					ROFL_PIPELINE_INFO_NO_PREFIX(" GOTO(%u), ",group.instructions[i].go_to_table);
					break;
				
			default: //Empty instruction 
				break;
		}
	}
	if( has_apply_actions ){
		ROFL_PIPELINE_INFO_NO_PREFIX("\n\t\t\tAPP.ACTIONs:");
		of12_dump_action_group(group.instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_APPLY_ACTIONS)].apply_actions);
	}
	if( has_write_actions ){
		ROFL_PIPELINE_INFO_NO_PREFIX("\n\t\t\tWR.ACTIONs:");
		of12_dump_write_actions(group.instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions);
	}	
}

bool of12_instruction_has(of12_instruction_group_t *inst_grp, of12_packet_action_type_t type, uint64_t value){
	///returns true if the action type with the specific value is in the set of instructions.
	
	return ( of12_write_actions_has(inst_grp->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions, type, value) ||
		of12_apply_actions_has(inst_grp->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_APPLY_ACTIONS)].apply_actions, type, value) );
}

rofl_result_t of12_validate_instructions(of12_group_table_t *gt, of12_instruction_group_t* inst_grp){
	int i, num_of_output_actions=0;
	
	//if there is a group action we should check that the group exists
	for(i=0;i<OF12_IT_GOTO_TABLE;i++){
		switch(inst_grp->instructions[i].type){
			case OF12_IT_NO_INSTRUCTION:
				continue;
				break;
				
			case OF12_IT_APPLY_ACTIONS:
				if(of12_validate_action_group(inst_grp->instructions[i].apply_actions, gt)!=true)
					return ROFL_FAILURE;
				num_of_output_actions+=inst_grp->instructions[i].apply_actions->num_of_output_actions;
				break;
				
			case OF12_IT_WRITE_ACTIONS:
				if(of12_validate_write_actions(inst_grp->instructions[i].write_actions, gt)!=true)
					return ROFL_FAILURE;
				num_of_output_actions+=inst_grp->instructions[i].write_actions->num_of_output_actions;
				break;
				
			default:
				continue;
				break;
		}
	}
	
	//update has multiple outputs flag
	inst_grp->has_multiple_outputs =  ( num_of_output_actions > 1);
	
	return ROFL_SUCCESS;
}
