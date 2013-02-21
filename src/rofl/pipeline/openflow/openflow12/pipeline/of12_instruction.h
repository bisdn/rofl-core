#ifndef __OF12_INSTRUCTION_H__
#define __OF12_INSTRUCTION_H__

#include <inttypes.h> 
#include "../../../util/rofl_pipeline_utils.h"
#include "of12_action.h"

/* Instruction type. From OF12: enum ofp_12_instruction_type */
typedef enum {
    OF12_IT_NO_INSTRUCTION	= 0,		/* Setup the metadata field for use later in pipeline */
    OF12_IT_GOTO_TABLE		= 1,		/* Setup the next table in the lookup pipeline */
    OF12_IT_WRITE_METADATA	= 2,		/* Setup the metadata field for use later in pipeline */
    OF12_IT_WRITE_ACTIONS	= 3,		/* Write the action(s) onto the datapath action set */
    OF12_IT_APPLY_ACTIONS	= 4,		/* Applies the action(s) immediately */
    OF12_IT_CLEAR_ACTIONS	= 5,		/* Clears all actions from the datapath action set */
    OF12_IT_EXPERIMENTER	= 0xFFFF,		/* Experimenter instruction */
}of12_instruction_type_t;

/* Instruction abstraction data structure */
struct of12_instruction{
	//Type and value(for set fields and push)
	of12_instruction_type_t type;

	//Action group for APPLY 
	of12_action_group_t* apply_actions;

	//WRITE actions 	
	of12_write_actions_t* write_actions;

	//GO-TO-TABLE
	unsigned int go_to_table;	
};
typedef struct of12_instruction of12_instruction_t;

/* Instruction group, using a double-linked-list */ 
typedef struct{
	//Number of actions in the list
	unsigned int num_of_instructions;

	of12_instruction_t instructions[OF12_IT_GOTO_TABLE]; //Latest must ALWAYS be go to table
}of12_instruction_group_t;

//Fwd declaration
struct of_switch;

/*
*
* Function prototypes
*
*/

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

//Instruction group
void of12_init_instruction_group(of12_instruction_group_t* group);
void of12_destroy_instruction_group(of12_instruction_group_t* group);

//Add/remove instructions to/from group
void of12_add_instruction_to_group(of12_instruction_group_t* group, of12_instruction_type_t type, of12_action_group_t* apply_actions, of12_write_actions_t* write_actions, unsigned int go_to_table);
void of12_remove_instruction_from_the_group(of12_instruction_group_t* group, of12_instruction_type_t type);

unsigned int of12_process_instructions(const struct of_switch* sw, const unsigned int table_id, datapacket_t *const pkt, const of12_instruction_group_t* instructions);

//Dump
void of12_dump_instructions(of12_instruction_group_t group);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_INSTRUCTION
