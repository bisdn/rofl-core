/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_INSTRUCTION_PP_H__
#define __OF1X_INSTRUCTION_PP_H__

#include <inttypes.h> 
#include <stdbool.h>
#include "rofl.h"
#include "../../../util/pp_guard.h" //Never forget to include the guard
#include "of1x_instruction.h"
#include "of1x_action_pp.h"

/**
* @file of1x_instruction_pp.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief OpenFlow v1.X instructions packet processing routines 
*/

//C++ extern C
ROFL_BEGIN_DECLS

static inline bool  __of1x_process_instructions_must_replicate(const of1x_instruction_group_t* inst_grp){

	bool has_goto = inst_grp->instructions[OF1X_IT_GOTO_TABLE].type == OF1X_IT_GOTO_TABLE;
	unsigned int n_out = inst_grp->num_of_outputs; 

	return  ( (n_out == 1) && (has_goto) ) || ( n_out > 1); 
}

/* Process instructions */
static inline unsigned int __of1x_process_instructions(const unsigned int tid, const struct of1x_switch* sw, const unsigned int table_id, datapacket_t *const pkt, const of1x_instruction_group_t* instructions){

	of1x_instruction_t* inst = (of1x_instruction_t*)&instructions->instructions[OF1X_IT_APPLY_ACTIONS]; 

	/**
	* Unrolled instructions loop
	*/

	//Check all instructions in order
	if(inst->type == OF1X_IT_APPLY_ACTIONS)
		__of1x_process_apply_actions(tid, sw, table_id, pkt, inst->apply_actions, __of1x_process_instructions_must_replicate(instructions) ); 

	inst++; //increase
	
	if(inst->type == OF1X_IT_CLEAR_ACTIONS)
		__of1x_clear_write_actions(&pkt->write_actions.of1x);

	inst++; //increase
	
	if(inst->type == OF1X_IT_WRITE_ACTIONS)
		__of1x_update_packet_write_actions(&pkt->write_actions.of1x, inst->write_actions);
	inst++; //increase
	
	if(inst->type == OF1X_IT_WRITE_METADATA){
		pkt->__metadata = (pkt->__metadata & ~inst->write_metadata.metadata_mask) |
				(inst->write_metadata.metadata & inst->write_metadata.metadata_mask);
	}
	inst++; //increase

	if(inst->type == OF1X_IT_EXPERIMENTER){
		//TODO:
	}
	
	inst++; //increase

	if(inst->type == OF1X_IT_METER){
		//TODO:
	}
	
	inst++; //increase

	if(inst->type == OF1X_IT_GOTO_TABLE){
		return inst->go_to_table;
	}

	return 0; //NO go-to-table
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_INSTRUCTION_PP
