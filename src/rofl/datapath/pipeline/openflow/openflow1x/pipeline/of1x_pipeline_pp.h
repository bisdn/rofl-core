/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_PIPELINE_PP_H__
#define __OF1X_PIPELINE_PP_H__

#include "rofl.h"
#include "../../../util/pp_guard.h" //Never forget to include the guard
#include "../of1x_switch.h"
#include "of1x_pipeline.h"
#include "of1x_flow_table_pp.h"
#include "of1x_instruction_pp.h"

//This block is not necessary but it is useful to prevent
//double definitions of static inline methods
#include "of1x_action.h"
#include "of1x_instruction.h"
#include "of1x_flow_table.h"
#include "of1x_group_table.h"
#include "of1x_timers.h"

//Platform stuff
#include "../../../platform/lock.h"
#include "../../../platform/likely.h"
#include "../../../platform/memory.h"
#include "../../../platform/packet.h"
#include "../../../platform/atomic_operations.h"
#include "../of1x_async_events_hooks.h"

#include "matching_algorithms/available_ma_pp.h"

#include "../../../util/logging.h"


/**
* @file of1x_pipeline_pp.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief OpenFlow v1.0, 1.2 and 1.3.2 pipeline packet processing routines
*/

//C++ extern C
ROFL_BEGIN_DECLS


/*
* Packet processing through pipeline
*
*/
static inline void __of1x_process_packet_pipeline(const of_switch_t *sw, datapacket_t *const pkt){

	//Loop over tables
	unsigned int i, table_to_go, num_of_outputs;
	of1x_flow_table_t* table;
	of1x_flow_entry_t* match;
	
	//Initialize packet for OF1.X pipeline processing 
	__of1x_init_packet_write_actions(&pkt->write_actions.of1x);

	//Mark packet as being processed by this sw
	pkt->sw = sw;
	
	ROFL_PIPELINE_DEBUG("Packet[%p] entering switch [%s] pipeline (1.X)\n",pkt,sw->name);	

#ifdef DEBUG
	dump_packet_matches(&pkt->matches, false);
#endif
	
	for(i=OF1X_FIRST_FLOW_TABLE_INDEX; i < ((of1x_switch_t*)sw)->pipeline.num_of_tables ; i++){

		table = &((of1x_switch_t*)sw)->pipeline.tables[i];

		//Perform lookup	
		match = __of1x_find_best_match_table((of1x_flow_table_t* const)table, pkt);
		
		if(likely(match != NULL)){
			
			ROFL_PIPELINE_DEBUG("Packet[%p] matched at table: %u, entry: %p\n", pkt, i,match);

			//Update table and entry statistics
			platform_atomic_inc64(&table->stats.lookup_count,table->stats.mutex);
			platform_atomic_inc64(&table->stats.matched_count,table->stats.mutex);

			//Update flow statistics
			platform_atomic_inc64(&match->stats.packet_count,match->stats.mutex);
			platform_atomic_add64(&match->stats.byte_count, pkt->matches.__pkt_size_bytes, match->stats.mutex);

			//Process instructions
			table_to_go = __of1x_process_instructions((of1x_switch_t*)sw, i, pkt, &match->inst_grp);

			if(table_to_go > i && likely(table_to_go < OF1X_MAX_FLOWTABLES)){

				ROFL_PIPELINE_DEBUG("Packet[%p] Going to table %u->%u\n",pkt, i,table_to_go);
				i = table_to_go-1;

				//Unlock the entry so that it can eventually be modified/deleted
				platform_rwlock_rdunlock(match->rwlock);
	
				continue;
			}

			//Process WRITE actions
			__of1x_process_write_actions((of1x_switch_t*)sw, i, pkt, __of1x_process_instructions_must_replicate(&match->inst_grp));

			//Recover the num_of_outputs to release the lock asap
			num_of_outputs = match->inst_grp.num_of_outputs;

			//Unlock the entry so that it can eventually be modified/deleted
			platform_rwlock_rdunlock(match->rwlock);

			//Drop packet Only if there has been copy(cloning of the packet) due to 
			//multiple output actions
			if(num_of_outputs != 1)
				platform_packet_drop(pkt);
							
			return;	
		}else{
			//Update table statistics
			platform_atomic_inc64(&table->stats.lookup_count,table->stats.mutex);

			//Not matched, look for table_miss behaviour 
			if(table->default_action == OF1X_TABLE_MISS_DROP){

				ROFL_PIPELINE_DEBUG("Packet[%p] table MISS_DROP %u\n",pkt, i);	
				platform_packet_drop(pkt);
				return;

			}else if(table->default_action == OF1X_TABLE_MISS_CONTROLLER){
			
				ROFL_PIPELINE_DEBUG("Packet[%p] table MISS_CONTROLLER. It Will get a PACKET_IN event to the controller\n",pkt);

				platform_of1x_packet_in((of1x_switch_t*)sw, i, pkt, ((of1x_switch_t*)sw)->pipeline.miss_send_len, OF1X_PKT_IN_NO_MATCH);
				return;
			}
			//else -> continue with the pipeline	
		}
	}
	
	//No match/default table action -> DROP the packet	
	platform_packet_drop(pkt);

}

/**
* @brief Processes a packet-out through the OpenFlow pipeline.  
* @ingroup core_pp 
*/
static inline void of1x_process_packet_out_pipeline(const of1x_switch_t *sw, datapacket_t *const pkt, const of1x_action_group_t* apply_actions_group){
	
	bool has_multiple_outputs=false;
	of1x_group_table_t *gt = sw->pipeline.groups;

	//Validate apply_actions_group
	__of1x_validate_action_group(NULL, (of1x_action_group_t*)apply_actions_group, gt);

	if(apply_actions_group->num_of_output_actions == 0){
		//No output actions or groups; drop and return	
		platform_packet_drop(pkt);
		return;
	}
	
	has_multiple_outputs = (apply_actions_group->num_of_output_actions > 1);

	//Just process the action group
	__of1x_process_apply_actions((of1x_switch_t*)sw, 0, pkt, apply_actions_group, has_multiple_outputs);
		
	if(has_multiple_outputs){
		//Packet was replicated. Drop original packet
		platform_packet_drop(pkt);
	}
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_PIPELINE_PP
