#include "of12_instruction.h"

#include <stdio.h>

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
	
	if(group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions->write_actions[OF12_AT_OUTPUT].type != OF12_AT_NO_ACTION)
		num_of_outputs++;
	if(group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions->write_actions[OF12_AT_GROUP].type != OF12_AT_NO_ACTION)
		num_of_outputs++;

	//Assign flag
	group->has_multiple_outputs =  (num_of_outputs > 1); 
}


//Update instructions
rofl_result_t of12_update_instructions(of12_instruction_group_t* group, of12_instruction_group_t* new_group){
	

	//Apply Actions
	if(of12_update_apply_actions(group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_APPLY_ACTIONS)].apply_actions, new_group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_APPLY_ACTIONS)].apply_actions)!=ROFL_SUCCESS)
		return ROFL_FAILURE;	

	//Write actions	
	if(of12_update_write_actions(group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions, new_group->instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions) != ROFL_SUCCESS)
		return ROFL_FAILURE;	

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


void of12_dump_instructions(of12_instruction_group_t group){

	unsigned int i,has_write_actions=0, has_apply_actions=0;

	fprintf(stderr,"Inst->> ");

	for(i=0;i<OF12_IT_GOTO_TABLE;i++){

		//Check all instructions in order 
		switch(group.instructions[i].type){
			case OF12_IT_APPLY_ACTIONS:  
					fprintf(stderr," APPLY, ");
					has_apply_actions++;
					break;
    			case OF12_IT_CLEAR_ACTIONS: 
					fprintf(stderr," CLEAR, ");
					break;
			case OF12_IT_WRITE_ACTIONS: 
					fprintf(stderr," WRITE, ");
					has_write_actions++;
					break;
    			case OF12_IT_WRITE_METADATA: //TODO:
					fprintf(stderr," WRITE-META, ");
					break;
			case OF12_IT_EXPERIMENTER: //TODO:
					fprintf(stderr," EXP, ");
					break;
    			case OF12_IT_GOTO_TABLE:  
					fprintf(stderr," GOTO(%u), ",group.instructions[i].go_to_table);
					break;
				
			default: //Empty instruction 
				break;
		}
	}
	if( has_apply_actions ){
		fprintf(stderr,"\n\t\t\tAPP.ACTIONs:");
		of12_dump_action_group(group.instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_APPLY_ACTIONS)].apply_actions);
	}
	if( has_write_actions ){
		fprintf(stderr,"\n\t\t\tWR.ACTIONs:");
		of12_dump_write_actions(group.instructions[OF12_SAFE_IT_TYPE_INDEX(OF12_IT_WRITE_ACTIONS)].write_actions);
	}	
}
