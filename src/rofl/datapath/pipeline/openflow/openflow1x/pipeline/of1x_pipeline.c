#include "of1x_pipeline.h"
#include "of1x_instruction.h"
#include "of1x_flow_table.h"
#include "of1x_group_table.h"
#include "of1x_timers.h"
#include "../../../platform/lock.h"
#include "../../../platform/likely.h"
#include "../../../platform/memory.h"
#include "../../../platform/packet.h"
#include "../../../platform/atomic_operations.h"
#include "../of1x_async_events_hooks.h"
#include "matching_algorithms/matching_algorithms_available.h"
#include "../of1x_switch.h"

#include "../../../util/logging.h"

/* 
* This file implements the abstraction of a pipeline
*/

/* Management operations */
rofl_result_t __of1x_init_pipeline(struct of1x_switch* sw, const unsigned int num_of_tables, enum of1x_matching_algorithm_available* list){
	
	int i;	
	of1x_pipeline_t* pipeline = &sw->pipeline;

	if(!sw)
		return ROFL_FAILURE;

	//Verify params
	if( ! (num_of_tables <= OF1X_MAX_FLOWTABLES && num_of_tables > 0) )
		return ROFL_FAILURE;
	

	//Fill in
	pipeline->sw = sw;
	pipeline->num_of_tables = num_of_tables;
	pipeline->num_of_buffers = 0; //Should be filled in the post_init hook


	//Allocate tables and initialize	
	pipeline->tables = (of1x_flow_table_t*)platform_malloc_shared(sizeof(of1x_flow_table_t)*num_of_tables);
	
	if(!pipeline->tables){
		return ROFL_FAILURE;
	}

	for(i=0;i<num_of_tables;i++){
		//TODO: if we would have tables with different config, table_config should be an array of table_config_t objects, one for each table
		if( (list[i] >= of1x_matching_algorithm_count) ||
		    (__of1x_init_table(pipeline, &pipeline->tables[i],i, list[i]) != ROFL_SUCCESS)
		){
			ROFL_PIPELINE_ERR("Creation of table #%d has failed in logical switch %s. This might be due to an invalid Matching Algorithm or that the system has run out of memory. Aborting Logical Switch creation\n",i,sw->name);	
			//Destroy already allocated tables
			for(--i; i>=0; i--){
				__of1x_destroy_table(&pipeline->tables[i]);
			}

			platform_free_shared(pipeline->tables);
			return ROFL_FAILURE;
		}
	}

	/*
	* Setting default capabilities and miss_send_lent. driver can afterwards 
	* modify them at its will, via the hook.
	*/

	//Set datapath capabilities
	pipeline->capabilities = 	OF1X_CAP_FLOW_STATS |
					OF1X_CAP_TABLE_STATS |
					OF1X_CAP_PORT_STATS |
					OF1X_CAP_GROUP_STATS | 
					//OF1X_CAP_IP_REASM |
					OF1X_CAP_QUEUE_STATS;
					//OF1X_CAP_ARP_MATCH_IP;

	//TODO: Evaluate if OF1X_CAP_PORT_BLOCKED should be added by default 

	//Set MISS-SEND length to default 
	pipeline->miss_send_len = OF1X_DEFAULT_MISS_SEND_LEN;

	//init groups
	pipeline->groups = of1x_init_group_table();

	return ROFL_SUCCESS;
}

rofl_result_t __of1x_destroy_pipeline(of1x_pipeline_t* pipeline){

	int i;
	
	//destrouy groups
	of1x_destroy_group_table(pipeline->groups);
	
	for(i=0; i<pipeline->num_of_tables; i++){
		//We don't care about errors here, maybe add trace TODO
		__of1x_destroy_table(&pipeline->tables[i]);
	}
			
	//Now release table resources (allocated as single block)
	platform_free_shared(pipeline->tables);

	return ROFL_SUCCESS;
}

//Purge of all entries in the pipeline (reset)	
rofl_result_t __of1x_purge_pipeline_entries(of1x_pipeline_t* pipeline){

	unsigned int i;
	rofl_result_t result = ROFL_SUCCESS;
	of1x_flow_entry_t* flow_entry;
	of1x_group_table_t* group_entry;

	//Create empty entries
	flow_entry = of1x_init_flow_entry(NULL,NULL,false);
	group_entry = of1x_init_group_table(); 
	
	if( unlikely(flow_entry==NULL) )
		return ROFL_FAILURE;
	if( unlikely(group_entry==NULL) ){
		of1x_destroy_flow_entry(flow_entry);
		return ROFL_FAILURE;
	}

	//Purge flow_mods	
	for(i=OF1X_FIRST_FLOW_TABLE_INDEX; i < pipeline->num_of_tables ; i++){
		//Purge flow_mods
		if(of1x_remove_flow_entry_table(pipeline, i, flow_entry, NOT_STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY) != ROFL_SUCCESS){
			
			result = ROFL_FAILURE;	
			break;
		}
	}

	//Purge group mods
	if(result == ROFL_SUCCESS){
		if(of1x_group_delete(pipeline, group_entry, OF1X_GROUP_ANY) != ROFL_OF1X_GM_OK)
			result = ROFL_FAILURE;
	}
	
	//Destroy entries
	of1x_destroy_flow_entry(flow_entry);
	of1x_destroy_group_table(group_entry);

	return result;
}

//Set the default tables(flow and group tables) configuration according to the new version
rofl_result_t __of1x_set_pipeline_tables_defaults(of1x_pipeline_t* pipeline, of_version_t version){

	unsigned int i;

	//Reset table defaults according to specific version
	for(i=OF1X_FIRST_FLOW_TABLE_INDEX; i < pipeline->num_of_tables ; i++){
		switch(version){
			case OF_VERSION_10:
				__of10_set_table_defaults(&pipeline->tables[i]);
				break; 
			case OF_VERSION_12:
				__of12_set_table_defaults(&pipeline->tables[i]);
				break; 
			case OF_VERSION_13:
				__of13_set_table_defaults(&pipeline->tables[i]);
				break; 
			default:
				return ROFL_FAILURE;			

		}
	}

	return ROFL_SUCCESS;
}


/*
*
* Packet processing through pipeline
*
*/
void __of1x_process_packet_pipeline(const of_switch_t *sw, datapacket_t *const pkt){

	//Loop over tables
	unsigned int i, table_to_go, num_of_outputs;
	of1x_flow_table_t* table;
	of1x_flow_entry_t* match;
	packet_matches_t* pkt_matches;
	
	//Initialize packet for OF1.X pipeline processing 
	__of1x_init_packet_write_actions(&pkt->write_actions.of1x);

	//Mark packet as being processed by this sw
	pkt->sw = sw;
	
	//Matches aux
	pkt_matches = &pkt->matches;

	ROFL_PIPELINE_DEBUG("Packet[%p] entering switch [%s] pipeline (1.X)\n",pkt,sw->name);	

#ifdef DEBUG
	dump_packet_matches(&pkt->matches);
#endif
	
	for(i=OF1X_FIRST_FLOW_TABLE_INDEX; i < ((of1x_switch_t*)sw)->pipeline.num_of_tables ; i++){

		table = &((of1x_switch_t*)sw)->pipeline.tables[i];

		//Perform lookup	
		match = __of1x_find_best_match_table((of1x_flow_table_t* const)table, pkt_matches);
		
		if(likely(match != NULL)){
			
			ROFL_PIPELINE_DEBUG("Packet[%p] matched at table: %u, entry: %p\n", pkt, i,match);

			//Update table and entry statistics
			platform_atomic_inc64(&table->stats.lookup_count,table->stats.mutex);
			platform_atomic_inc64(&table->stats.matched_count,table->stats.mutex);

			//Update flow statistics
			platform_atomic_inc64(&match->stats.packet_count,match->stats.mutex);
			platform_atomic_add64(&match->stats.byte_count, pkt_matches->pkt_size_bytes, match->stats.mutex);

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

				platform_of1x_packet_in((of1x_switch_t*)sw, i, pkt, OF1X_PKT_IN_NO_MATCH);
				return;
			}
			//else -> continue with the pipeline	
		}
	}
	
	//No match/default table action -> DROP the packet	
	platform_packet_drop(pkt);

}

/*
* Process the packet out 
*/
void of1x_process_packet_out_pipeline(const of1x_switch_t *sw, datapacket_t *const pkt, const of1x_action_group_t* apply_actions_group){
	
	bool has_multiple_outputs=false;
	of1x_group_table_t *gt = sw->pipeline.groups;

	//Validate apply_actions_group
	__of1x_validate_action_group((of1x_action_group_t*)apply_actions_group, gt);

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

//
// Snapshots
//

//Creates a snapshot of the running pipeline of an LSI 
rofl_result_t __of1x_pipeline_get_snapshot(of1x_pipeline_t* pipeline, of1x_pipeline_snapshot_t* sn){

	int i;
	of1x_flow_table_t* t;

	//Cleanup stuff coming from the cloning process
	sn->tables = NULL;
	sn->groups = NULL;
	sn->sw = NULL;		

	//Allocate tables and initialize	
	sn->tables = (of1x_flow_table_t*)platform_malloc_shared(sizeof(of1x_flow_table_t)*pipeline->num_of_tables);
	
	if(!sn->tables)
		return ROFL_FAILURE;

	//Copy contents of the tables and remove references
	memcpy(sn->tables, pipeline->tables, sizeof(of1x_flow_table_t)*pipeline->num_of_tables);

	//Snapshot tables tables
	for(i=0;i<pipeline->num_of_tables;i++){
		t = &sn->tables[i];
		t->pipeline = t->rwlock = t->mutex = t->matching_aux[0] = t->matching_aux[1] = NULL;
		
#if OF1X_TIMER_STATIC_ALLOCATION_SLOTS	
#else
		t->timers = NULL;
#endif
	}
	
	//TODO: deep entry copy?

	return ROFL_SUCCESS;
}

//Destroy a previously getd snapshot
void __of1x_pipeline_destroy_snapshot(of1x_pipeline_snapshot_t* sn){
	//Release tables memory
	platform_free_shared(sn->tables);
}
