/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_INSTRUCTION_H__
#define __OF1X_INSTRUCTION_H__

#include <inttypes.h> 
#include <stdbool.h>
#include "rofl.h"
#include "of1x_action.h"
#include "../../../common/datapacket.h"

/**
* @file of1x_instruction.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Openflow v1.X instructions 
*
* Instructions should never be used directly. Instructions
* are arleady part of the of1x_flow_entry.
* 
* How to add and remove instructions (pseudo-code):
* @code
*
* entry = of1x_init_flow_entry()
* action_group = of1x_init_action_group()
*
* //Push actions... (refer to of1x_action.h)
*
* //APPLY ACTIONS
* of1x_add_instruction_to_group(&entry->inst_grp, OF1X_IT_APPLY_ACTIONS, action_group,NULL,0)
*
* //WRITE ACTIONS
* write_actions = of1x_init_write_actions()
* //Add actions to write group... (refer to of1x_action.h)
*
* of1x_add_instruction_to_group(&entry->inst_grp, OF1X_IT_APPLY_ACTIONS, NULL, write_actions,0)
*
* //GOTO_TABLE
* of1x_add_instruction_to_group(&entry->inst_grp, OF1X_IT_GOTO_TABLE, NULL, NULL,10)
* 
*
* //To release resources
* of1x_destroy_flow_entry(entry)
* @endcode
*
*/



/**
* @ingroup core_of1x 
* Instruction type. From OF12: enum ofp_12_instruction_type 
*/
typedef enum {
    OF1X_IT_NO_INSTRUCTION	= 0,		/* Setup the metadata field for use later in pipeline */
    OF1X_IT_APPLY_ACTIONS	= 1,		/* Applies the action(s) immediately */
    OF1X_IT_CLEAR_ACTIONS	= 2,		/* Clears all actions from the datapath action set */
    OF1X_IT_WRITE_ACTIONS	= 3,		/* Write the action(s) onto the datapath action set */
    OF1X_IT_WRITE_METADATA	= 4,		/* Setup the metadata field for use later in pipeline */

    OF1X_IT_EXPERIMENTER	= 5,		/* Experimenter instruction */
    OF1X_IT_GOTO_TABLE		= 6,		/* Setup the next table in the lookup pipeline */
}of1x_instruction_type_t;

/* Instruction abstraction data structure */
typedef struct of1x_instruction{
	//Type and value(for set fields and push)
	of1x_instruction_type_t type;

	//Action group for APPLY_ACTIONS type only 
	of1x_action_group_t* apply_actions;

	//WRITE_ACTIONS type only actions 	
	of1x_write_actions_t* write_actions;

	//GO-TO-TABLE
	unsigned int go_to_table;	
}of1x_instruction_t;

/* Instruction group, using a double-linked-list */ 
typedef struct of1x_instruction_group{
	//Number of actions in the list
	unsigned int num_of_instructions;

	of1x_instruction_t instructions[OF1X_IT_GOTO_TABLE]; //Latest must ALWAYS be go to table
	
	//Flag indicating that there are multiple 
	//outputs in several instructions/in an apply 
	//actions group
	bool has_multiple_outputs;
	
}of1x_instruction_group_t;

//Fwd declaration
struct of1x_switch;
struct of1x_flow_entry;
struct of1x_group_table;

/*
*
* Function prototypes
*
*/

//C++ extern C
ROFL_BEGIN_DECLS

//Instruction group

void __of1x_init_instruction_group(of1x_instruction_group_t* group);
void __of1x_destroy_instruction_group(of1x_instruction_group_t* group);

//Add/remove instructions to/from group
/**
* @brief Adds an instruction of the group 
* @ingroup core_of1x 
* @param type Instruction type (OF1X_IT_XX) 
* @param apply_actions (For OF1X_IT_APPLY_ACTIONS only)Apply actions group, with actions PREVIOUSLY filled. The group instance cannot be further accessed or freed from outside the library 
* @param apply_actions (For OF1X_IT_WRITE_ACTIONS only) Write_actions group, with actions PREVIOUSLY filled. The group instance cannot be further accessed or freed from outside the library.
* @param go_to_table (For OF1X_IT_GO_TO_TABLE only) Index of the table to go. 
*/
void of1x_add_instruction_to_group(of1x_instruction_group_t* group, of1x_instruction_type_t type, of1x_action_group_t* apply_actions, of1x_write_actions_t* write_actions, unsigned int go_to_table);
/**
* @brief Remove an instruction of the group 
* @ingroup core_of1x 
* @param group Instruction group 
* @param type Instruction type (OF1X_IT_XX) 
*/
void of1x_remove_instruction_from_the_group(of1x_instruction_group_t* group, of1x_instruction_type_t type);

//Update instructions
rofl_result_t __of1x_update_instructions(of1x_instruction_group_t* group, of1x_instruction_group_t* new_group);

//Check whether instructions contain group
bool __of1x_instructions_contain_group(struct of1x_flow_entry *const entry, const unsigned int group_id);

//Copy (clone) instructions: TODO evaluate if is necessary to check for errors
void __of1x_copy_instruction_group(of1x_instruction_group_t* origin, of1x_instruction_group_t* dest);

unsigned int __of1x_process_instructions(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t *const pkt, const of1x_instruction_group_t* instructions);

bool __of1x_instruction_has(of1x_instruction_group_t *inst_grp, of1x_packet_action_type_t type, uint64_t value);

rofl_result_t __of1x_validate_instructions(struct of1x_group_table *gt, of1x_instruction_group_t* inst_grp);

//Dump
void __of1x_dump_instructions(of1x_instruction_group_t group);

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_INSTRUCTION
