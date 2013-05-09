#include "of12_pipeline.h"
#include "of12_instruction.h"
#include "../../../platform/lock.h"
#include "../../../platform/memory.h"
#include "../../../platform/packet.h"
#include "../of12_async_events_hooks.h"
#include "matching_algorithms/matching_algorithms_available.h"
#include "../of12_switch.h"

#include "../../../util/logging.h"

/* 
* This file implements the abstraction of a pipeline
*/

/* Management operations */
of12_pipeline_t* __of12_init_pipeline(struct of12_switch* sw, const unsigned int num_of_tables, enum matching_algorithm_available* list){
	int i;	
	of12_pipeline_t* pipeline;

	if(!sw)
		return NULL;

	//Verify params
	if( ! (num_of_tables <= OF12_MAX_FLOWTABLES && num_of_tables > 0) )
		return NULL;
	
	pipeline = (of12_pipeline_t*)platform_malloc_shared(sizeof(of12_pipeline_t));

	if(!pipeline)
		return NULL;
	
	//Fill in
	pipeline->sw = sw;
	pipeline->num_of_tables = num_of_tables;
	pipeline->num_of_buffers = 2048; //FIXME: call platform to get this information 


	//Allocate tables and initialize	
	pipeline->tables = (of12_flow_table_t*)platform_malloc_shared(sizeof(of12_flow_table_t)*num_of_tables);
	
	if(!pipeline->tables){
		platform_free_shared(pipeline);
		return NULL;
	}

	for(i=0;i<num_of_tables;i++){
		//TODO: if we would have tables with different config, table_config should be an array of table_config_t objects, one for each table
		if( (list[i] >= matching_algorithm_count) ||
		    (__of12_init_table(pipeline, &pipeline->tables[i],i, list[i]) != ROFL_SUCCESS)
		){
			ROFL_PIPELINE_ERR("Creation of table #%d has failed in logical switch %s. This might be due to an invalid Matching Algorithm or that the system has run out of memory. Aborting Logical Switch creation\n",i,sw->name);	
			//Destroy already allocated tables
			for(--i; i>=0; i--){
				__of12_destroy_table(&pipeline->tables[i]);
			}

			platform_free_shared(pipeline->tables);
			platform_free_shared(pipeline);
			return NULL;
		}
	}

	/*
	* Setting default capabilities and miss_send_lent. driver can afterwards 
	* modify them at its will, via the hook.
	*/

	//Set datapath capabilities
	pipeline->capabilities = 	OF12_CAP_FLOW_STATS |
					OF12_CAP_TABLE_STATS |
					OF12_CAP_PORT_STATS |
					//OF12_CAP_GROUP_STATS | //FIXME: add when groups are implemented
					//OF12_CAP_IP_REASM |
					OF12_CAP_QUEUE_STATS;
					//OF12_CAP_ARP_MATCH_IP;

	//Set MISS-SEND length to default 
	pipeline->miss_send_len = OF12_DEFAULT_MISS_SEND_LEN;

	//init groups
	pipeline->groups = of12_init_group_table();

	return pipeline;
}

rofl_result_t of12_destroy_pipeline(of12_pipeline_t* pipeline){

	int i;
	
	//destrouy groups
	of12_destroy_group_table(pipeline->groups);
	
	for(i=0;i<pipeline->num_of_tables;i++){
		//We don't care about errors here, maybe add trace TODO
		__of12_destroy_table(&pipeline->tables[i]);
	}
			
	//Now release table resources (allocated as single block)
	platform_free_shared(pipeline->tables);

	platform_free_shared(pipeline);

	return ROFL_SUCCESS;

}


/*
*
* Packet processing through pipeline
*
*/
void __of12_process_packet_pipeline(const of_switch_t *sw, datapacket_t *const pkt){

	//Loop over tables
	unsigned int i, table_to_go;
	of12_flow_entry_t* match;
	
	//Temporal stack vars for matches and write actions (faster than dyn. mem)
	of12_packet_matches_t pkt_matches;
	of12_write_actions_t write_actions;
	
	//Initialize packet for OF1.2 pipeline processing 
	of12_init_packet_matches(pkt, &pkt_matches); 
	of12_init_packet_write_actions(pkt, &write_actions); 
		
	ROFL_PIPELINE_DEBUG("Packet[%p] entering switch [%s] pipeline (1.2)\n",pkt,sw->name);	
	
	//FIXME: add metadata+write operations 
	for(i=OF12_FIRST_FLOW_TABLE_INDEX; i < ((of12_switch_t*)sw)->pipeline->num_of_tables ; i++){
		
		//Perform lookup	
		match = __of12_find_best_match_table((of12_flow_table_t* const)&((of12_switch_t*)sw)->pipeline->tables[i],(of12_packet_matches_t *const)&pkt_matches);
		
		if(match){
			
			bool has_multiple_outputs;			

			ROFL_PIPELINE_DEBUG("Packet[%p] matched at table: %u, entry: %p\n", pkt, i,match);

			//Update table and entry statistics
			of12_stats_table_matches_inc(&((of12_switch_t*)sw)->pipeline->tables[i]);
			of12_stats_flow_update_match(match, pkt_matches.pkt_size_bytes);

			//Update entry timers
			__of12_timer_update_entry(match);

			//Process instructions
			table_to_go = of12_process_instructions((of12_switch_t*)sw, i, pkt, &match->inst_grp);

	
			if(table_to_go > i && table_to_go < OF12_MAX_FLOWTABLES){

				ROFL_PIPELINE_DEBUG("Packet[%p] Going to table %u->%u\n",pkt, i,table_to_go);
				i = table_to_go-1;

				//Unlock the entry so that it can eventually be modified/deleted
				platform_rwlock_rdunlock(match->rwlock);
	
				continue;
			}

			//Process WRITE actions
			of12_process_write_actions((of12_switch_t*)sw, i, pkt, match->inst_grp.has_multiple_outputs);

			//Copy flag to avoid race condition and release the entry asap 
			has_multiple_outputs = match->inst_grp.has_multiple_outputs;

			//Unlock the entry so that it can eventually be modified/deleted
			platform_rwlock_rdunlock(match->rwlock);

			//Drop packet Only if there has been copy(cloning of the packet) due to 
			//multiple output actions
			if(has_multiple_outputs)
				platform_packet_drop(pkt);
							

			return;	
		}else{
			//Update table statistics
			of12_stats_table_lookup_inc(&((of12_switch_t*)sw)->pipeline->tables[i]);

			//Not matched, look for table_miss behaviour 
			if(((of12_switch_t*)sw)->pipeline->tables[i].default_action == OF12_TABLE_MISS_DROP){

				ROFL_PIPELINE_DEBUG("Packet[%p] table MISS_DROP %u\n",pkt, i);	
				platform_packet_drop(pkt);
				return;

			}else if(((of12_switch_t*)sw)->pipeline->tables[i].default_action == OF12_TABLE_MISS_CONTROLLER){
			
				ROFL_PIPELINE_DEBUG("Packet[%p] table MISS_CONTROLLER. It Will generate a PACKET_IN event to the controller\n",pkt);

				platform_of12_packet_in((of12_switch_t*)sw, i, pkt, OF12_PKT_IN_NO_MATCH);
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
void __of12_process_packet_out_pipeline(const of_switch_t *sw, datapacket_t *const pkt, const of12_action_group_t* apply_actions_group){

	//Temporal stack vars for matches and write actions
	of12_packet_matches_t pkt_matches;
	of12_write_actions_t write_actions;
	
	//Initialize packet for OF1.2 pipeline processing 
	of12_init_packet_matches(pkt, &pkt_matches); 
	of12_init_packet_write_actions(pkt, &write_actions); 

	//Just process the action group
	of12_process_apply_actions((of12_switch_t*)sw, 0, pkt, apply_actions_group, apply_actions_group->num_of_output_actions > 1 );
	
}
	
