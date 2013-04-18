/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF12_INSTRUCTION_H__
#define __OF12_INSTRUCTION_H__

#include <inttypes.h> 
#include <stdbool.h>
#include "rofl.h"
#include "of12_action.h"

/**
* @file of12_instruction.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Openflow v1.2 instructions 
*
* Instructions should never be used directly. Instructions
* are arleady part of the of12_flow_entry.
* 
* How to add and remove instructions (pseudo-code):
* @code
*
* entry = of12_init_flow_entry()
* action_group = of12_init_action_group()
*
* //Push actions... (refer to of12_action.h)
*
* //APPLY ACTIONS
* of12_add_instruction_to_group(&entry->inst_grp, OF12_IT_APPLY_ACTIONS, action_group,NULL,0)
*
* //WRITE ACTIONS
* write_actions = of12_init_write_actions()
* //Add actions to write group... (refer to of12_action.h)
*
* of12_add_instruction_to_group(&entry->inst_grp, OF12_IT_APPLY_ACTIONS, NULL, write_actions,0)
*
* //GOTO_TABLE
* of12_add_instruction_to_group(&entry->inst_grp, OF12_IT_GOTO_TABLE, NULL, NULL,10)
* 
*
* //To release resources
* of12_destroy_flow_entry(entry)
* @endcode
*
*/



/**
* @ingroup core_of12 
* Instruction type. From OF12: enum ofp_12_instruction_type 
*/
typedef enum {
    OF12_IT_NO_INSTRUCTION	= 0,		/* Setup the metadata field for use later in pipeline */
    OF12_IT_APPLY_ACTIONS	= 1,		/* Applies the action(s) immediately */
    OF12_IT_CLEAR_ACTIONS	= 2,		/* Clears all actions from the datapath action set */
    OF12_IT_WRITE_ACTIONS	= 3,		/* Write the action(s) onto the datapath action set */
    OF12_IT_WRITE_METADATA	= 4,		/* Setup the metadata field for use later in pipeline */

    OF12_IT_EXPERIMENTER	= 5,		/* Experimenter instruction */
    OF12_IT_GOTO_TABLE		= 6,		/* Setup the next table in the lookup pipeline */
}of12_instruction_type_t;

/* Instruction abstraction data structure */
typedef struct of12_instruction{
	//Type and value(for set fields and push)
	of12_instruction_type_t type;

	//Action group for APPLY_ACTIONS type only 
	of12_action_group_t* apply_actions;

	//WRITE_ACTIONS type only actions 	
	of12_write_actions_t* write_actions;

	//GO-TO-TABLE
	unsigned int go_to_table;	
}of12_instruction_t;

/* Instruction group, using a double-linked-list */ 
typedef struct of12_instruction_group{
	//Number of actions in the list
	unsigned int num_of_instructions;

	of12_instruction_t instructions[OF12_IT_GOTO_TABLE]; //Latest must ALWAYS be go to table
	
	//Flag indicating that there are multiple 
	//outputs in several instructions/in an apply 
	//actions group
	bool has_multiple_outputs;
	
}of12_instruction_group_t;

//Fwd declaration
struct of12_switch;
struct of12_flow_entry;

/*
*
* Function prototypes
*
*/

//C++ extern C
ROFL_BEGIN_DECLS

//Instruction group

void of12_init_instruction_group(of12_instruction_group_t* group);
void of12_destroy_instruction_group(of12_instruction_group_t* group);

//Add/remove instructions to/from group
/**
* @brief Adds an instruction of the group 
* @ingroup core_of12 
* @param type Instruction type (OF12_IT_XX) 
* @param apply_actions (For OF12_IT_APPLY_ACTIONS only)Apply actions group, with actions PREVIOUSLY filled. The group instance cannot be further accessed or freed from outside the library 
* @param apply_actions (For OF12_IT_WRITE_ACTIONS only) Write_actions group, with actions PREVIOUSLY filled. The group instance cannot be further accessed or freed from outside the library.
* @param go_to_table (For OF12_IT_GO_TO_TABLE only) Index of the table to go. 
*/
void of12_add_instruction_to_group(of12_instruction_group_t* group, of12_instruction_type_t type, of12_action_group_t* apply_actions, of12_write_actions_t* write_actions, unsigned int go_to_table);
/**
* @brief Remove an instruction of the group 
* @ingroup core_of12 
* @param group Instruction group 
* @param type Instruction type (OF12_IT_XX) 
*/
void of12_remove_instruction_from_the_group(of12_instruction_group_t* group, of12_instruction_type_t type);

//Update instructions
rofl_result_t of12_update_instructions(of12_instruction_group_t* group, of12_instruction_group_t* new_group);

//Check whether instructions contain group
bool of12_instructions_contain_group(struct of12_flow_entry *const entry, const unsigned int group_id);

//Copy (clone) instructions: TODO evaluate if is necessary to check for errors
void of12_copy_instruction_group(of12_instruction_group_t* origin, of12_instruction_group_t* dest);

unsigned int of12_process_instructions(const struct of12_switch* sw, const unsigned int table_id, datapacket_t *const pkt, const of12_instruction_group_t* instructions);


//Dump
void of12_dump_instructions(of12_instruction_group_t group);

//C++ extern C
ROFL_END_DECLS

#endif //OF12_INSTRUCTION
