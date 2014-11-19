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
#include "of1x_statistics_pp.h"

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
static inline void __of1x_process_packet_pipeline(const unsigned int tid, const of_switch_t *sw, datapacket_t *const pkt){

	//Loop over tables
	unsigned int i, table_to_go, num_of_outputs;
	of1x_flow_table_t* table;
	of1x_flow_entry_t* match;
	
	//Initialize packet for OF1.X pipeline processing 
	__init_packet_metadata(pkt);
	__of1x_init_packet_write_actions(&pkt->write_actions.of1x);

	//Mark packet as being processed by this sw
	pkt->sw = sw;
	
	ROFL_PIPELINE_INFO("Packet[%p] entering switch %s [%p] pipeline (1.X)\n",pkt,sw->name, sw);	

	for(i=OF1X_FIRST_FLOW_TABLE_INDEX; i < ((of1x_switch_t*)sw)->pipeline.num_of_tables ; i++){

		table = &((of1x_switch_t*)sw)->pipeline.tables[i];

#ifdef ROFL_PIPELINE_LOCKLESS
		//Mark core presence 
		tid_mark_as_present(tid, &table->tid_presence_mask);
#endif

#ifdef DEBUG
		dump_packet_matches(pkt, false);
#endif
	
		//Perform lookup	
		match = __of1x_find_best_match_table(tid, (of1x_flow_table_t* const)table, pkt);
		
		if (match)
			pkt->__cookie = match->cookie;

		if(likely(match != NULL)){
			
			ROFL_PIPELINE_INFO("Packet[%p] matched at table: %u, entry: %p\n", pkt, i,match);

			//Update table and entry statistics
			__of1x_stats_table_update_match(tid, &table->stats);
			
			//Update flow statistics
			__of1x_stats_flow_update_match(tid, &match->stats, platform_packet_get_size_bytes(pkt));

			//Process instructions
			table_to_go = __of1x_process_instructions(tid, (of1x_switch_t*)sw, i, pkt, &match->inst_grp);

			if(table_to_go > i && likely(table_to_go < OF1X_MAX_FLOWTABLES)){

				ROFL_PIPELINE_INFO("Packet[%p] Going to table %u->%u\n",pkt, i,table_to_go);
				i = table_to_go-1;

#ifdef ROFL_PIPELINE_LOCKLESS
				//Unmark core presence in the table
				tid_mark_as_not_present(tid, &table->tid_presence_mask);
#else
				//Unlock the entry so that it can eventually be modified/deleted
				platform_rwlock_rdunlock(match->rwlock);
#endif
				continue;
			}

			//Process WRITE actions
			__of1x_process_write_actions(tid, (of1x_switch_t*)sw, i, pkt, __of1x_process_instructions_must_replicate(&match->inst_grp));

			//Recover the num_of_outputs to release the lock asap
			num_of_outputs = match->inst_grp.num_of_outputs;

#ifdef ROFL_PIPELINE_LOCKLESS
			//Unmark core presence in the table
			tid_mark_as_not_present(tid, &table->tid_presence_mask);
#else
			//Unlock the entry so that it can eventually be modified/deleted
			platform_rwlock_rdunlock(match->rwlock);
#endif

			//Drop packet Only if there has been copy(cloning of the packet) due to 
			//multiple output actions
			if(num_of_outputs != 1)
				platform_packet_drop(pkt);
							
			return;	
		}else{
#ifdef ROFL_PIPELINE_LOCKLESS
			//Unmark core presence in the table
			tid_mark_as_not_present(tid, &table->tid_presence_mask);
#endif

			//Update table statistics
			__of1x_stats_table_update_no_match(tid, &table->stats);

			//Not matched, look for table_miss behaviour 
			if(table->default_action == OF1X_TABLE_MISS_DROP){

				ROFL_PIPELINE_INFO("Packet[%p] table MISS_DROP %u\n",pkt, i);	
				platform_packet_drop(pkt);
				return;

			}else if(table->default_action == OF1X_TABLE_MISS_CONTROLLER){
			
				ROFL_PIPELINE_INFO("Packet[%p] table MISS_CONTROLLER. Generating a PACKET_IN event towards the controller\n",pkt);

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
static inline void of1x_process_packet_out_pipeline(const unsigned int tid, const of1x_switch_t *sw, datapacket_t *const pkt, const of1x_action_group_t* apply_actions_group){
	
	bool has_multiple_outputs=false;
	datapacket_t* reinject_pkt=NULL;
	of1x_group_table_t *gt = sw->pipeline.groups;

	//Validate apply_actions_group
	__of1x_validate_action_group(NULL, (of1x_action_group_t*)apply_actions_group, gt, true);

#ifdef DEBUG
	ROFL_PIPELINE_INFO("Packet[%p] Processing PKT_OUT, action group: ",pkt);
	__of1x_dump_action_group((of1x_action_group_t*)apply_actions_group, false);
	ROFL_PIPELINE_INFO_NO_PREFIX("\n");
#endif
	
	if(apply_actions_group->num_of_output_actions == 0){
		//No output actions or groups; drop and return	
		ROFL_PIPELINE_INFO("Packet[%p] WARNING: dropping! No output/group actions.\n",pkt);
		platform_packet_drop(pkt);
		return;
	}
	
	has_multiple_outputs = (apply_actions_group->num_of_output_actions > 1);
	

	//Just process the action group
	__of1x_process_apply_actions(tid, (of1x_switch_t*)sw, 0, pkt, apply_actions_group, has_multiple_outputs, &reinject_pkt);

	//Reinject if necessary
	if(reinject_pkt)
		__of1x_process_packet_pipeline(ROFL_PIPELINE_LOCKED_TID, (const of_switch_t*)sw, reinject_pkt);

	if(has_multiple_outputs){
		//Packet was replicated. Drop original packet
		ROFL_PIPELINE_DEBUG("Packet[%p] Dropping original pkt (previously cloned due to multiple output actions).\n", pkt);
		platform_packet_drop(pkt);
	}
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_PIPELINE_PP
