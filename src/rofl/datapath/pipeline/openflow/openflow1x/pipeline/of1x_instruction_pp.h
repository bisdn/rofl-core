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
static inline unsigned int __of1x_process_instructions(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t *const pkt, const of1x_instruction_group_t* instructions){

	unsigned int i;

	for(i=0;i<OF1X_IT_MAX;i++){
	
		//Check all instructions in order 
		switch(instructions->instructions[i].type){
			case OF1X_IT_APPLY_ACTIONS: __of1x_process_apply_actions(sw, table_id, pkt,instructions->instructions[i].apply_actions, __of1x_process_instructions_must_replicate(instructions) ); 
					break;
    			case OF1X_IT_CLEAR_ACTIONS: __of1x_clear_write_actions(&pkt->write_actions.of1x);
					break;
			case OF1X_IT_WRITE_ACTIONS: __of1x_update_packet_write_actions(&pkt->write_actions.of1x, instructions->instructions[i].write_actions);
					break;
    			case OF1X_IT_WRITE_METADATA:
				{
					packet_matches_t* matches = &pkt->matches;
					matches->__metadata = 	(matches->__metadata & ~instructions->instructions[i].write_metadata.metadata_mask) |
								(instructions->instructions[i].write_metadata.metadata & instructions->instructions[i].write_metadata.metadata_mask);
				}
					break;
			case OF1X_IT_EXPERIMENTER: //TODO:
					break;
			case OF1X_IT_METER: //TODO:
					break;
    			case OF1X_IT_GOTO_TABLE: return instructions->instructions[i].go_to_table; 
					break;
				
			default: //Empty instruction 
				break;
		}
	}		

	return 0; //NO go-to-table
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_INSTRUCTION_PP
