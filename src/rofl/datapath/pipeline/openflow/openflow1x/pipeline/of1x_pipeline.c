#include "of1x_pipeline.h"
#include "of1x_instruction.h"
#include "../../../platform/lock.h"
#include "../../../platform/memory.h"
#include "../../../platform/packet.h"
#include "../of1x_async_events_hooks.h"
#include "matching_algorithms/matching_algorithms_available.h"
#include "../of1x_switch.h"

#include "../../../util/logging.h"

/* 
* This file implements the abstraction of a pipeline
*/

/* Management operations */
of1x_pipeline_t* __of1x_init_pipeline(struct of1x_switch* sw, const unsigned int num_of_tables, enum of1x_matching_algorithm_available* list){
	int i;	
	of1x_pipeline_t* pipeline;

	if(!sw)
		return NULL;

	//Verify params
	if( ! (num_of_tables <= OF1X_MAX_FLOWTABLES && num_of_tables > 0) )
		return NULL;
	
	pipeline = (of1x_pipeline_t*)platform_malloc_shared(sizeof(of1x_pipeline_t));

	if(!pipeline)
		return NULL;
	
	//Fill in
	pipeline->sw = sw;
	pipeline->num_of_tables = num_of_tables;
	pipeline->num_of_buffers = 2048; //FIXME: call platform to get this information 


	//Allocate tables and initialize	
	pipeline->tables = (of1x_flow_table_t*)platform_malloc_shared(sizeof(of1x_flow_table_t)*num_of_tables);
	
	if(!pipeline->tables){
		platform_free_shared(pipeline);
		return NULL;
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
			platform_free_shared(pipeline);
			return NULL;
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
					//OF1X_CAP_GROUP_STATS | //FIXME: add when groups are implemented
					//OF1X_CAP_IP_REASM |
					OF1X_CAP_QUEUE_STATS;
					//OF1X_CAP_ARP_MATCH_IP;

	//Set MISS-SEND length to default 
	pipeline->miss_send_len = OF1X_DEFAULT_MISS_SEND_LEN;

	//init groups
	pipeline->groups = of1x_init_group_table();

	return pipeline;
}

rofl_result_t __of1x_destroy_pipeline(of1x_pipeline_t* pipeline){

	int i;
	
	//destrouy groups
	of1x_destroy_group_table(pipeline->groups);
	
	for(i=0;i<pipeline->num_of_tables;i++){
		//We don't care about errors here, maybe add trace TODO
		__of1x_destroy_table(&pipeline->tables[i]);
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
void __of1x_process_packet_pipeline(const of_switch_t *sw, datapacket_t *const pkt){

	//Loop over tables
	unsigned int i, table_to_go;
	of1x_flow_entry_t* match;
	of1x_packet_matches_t* pkt_matches;
	
	//Initialize packet for OF1.2 pipeline processing 
	__of1x_init_packet_matches(pkt); 
	__of1x_init_packet_write_actions(pkt); 

	//Matches aux
	pkt_matches = &pkt->matches.of1x;

	ROFL_PIPELINE_DEBUG("Packet[%p] entering switch [%s] pipeline (1.2)\n",pkt,sw->name);	

#ifdef DEBUG
	of1x_dump_packet_matches(&pkt->matches);
#endif
	
	//FIXME: add metadata+write operations 
	for(i=OF1X_FIRST_FLOW_TABLE_INDEX; i < ((of1x_switch_t*)sw)->pipeline->num_of_tables ; i++){
		
		//Perform lookup	
		match = __of1x_find_best_match_table((of1x_flow_table_t* const)&((of1x_switch_t*)sw)->pipeline->tables[i], pkt_matches);
		
		if(match){
			
			bool has_multiple_outputs;			

			ROFL_PIPELINE_DEBUG("Packet[%p] matched at table: %u, entry: %p\n", pkt, i,match);

			//Update table and entry statistics
			__of1x_stats_table_matches_inc(&((of1x_switch_t*)sw)->pipeline->tables[i]);
			__of1x_stats_flow_update_match(match, pkt_matches->pkt_size_bytes);

			//Update entry timers
			__of1x_timer_update_entry(match);

			//Process instructions
			table_to_go = __of1x_process_instructions((of1x_switch_t*)sw, i, pkt, &match->inst_grp);

	
			if(table_to_go > i && table_to_go < OF1X_MAX_FLOWTABLES){

				ROFL_PIPELINE_DEBUG("Packet[%p] Going to table %u->%u\n",pkt, i,table_to_go);
				i = table_to_go-1;

				//Unlock the entry so that it can eventually be modified/deleted
				platform_rwlock_rdunlock(match->rwlock);
	
				continue;
			}

			//Process WRITE actions
			__of1x_process_write_actions((of1x_switch_t*)sw, i, pkt, match->inst_grp.has_multiple_outputs);

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
			__of1x_stats_table_lookup_inc(&((of1x_switch_t*)sw)->pipeline->tables[i]);

			//Not matched, look for table_miss behaviour 
			if(((of1x_switch_t*)sw)->pipeline->tables[i].default_action == OF1X_TABLE_MISS_DROP){

				ROFL_PIPELINE_DEBUG("Packet[%p] table MISS_DROP %u\n",pkt, i);	
				platform_packet_drop(pkt);
				return;

			}else if(((of1x_switch_t*)sw)->pipeline->tables[i].default_action == OF1X_TABLE_MISS_CONTROLLER){
			
				ROFL_PIPELINE_DEBUG("Packet[%p] table MISS_CONTROLLER. It Will generate a PACKET_IN event to the controller\n",pkt);

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
void of1x_process_packet_out_pipeline(const of_switch_t *sw, datapacket_t *const pkt, const of1x_action_group_t* apply_actions_group){

	//Initialize packet for OF1.2 pipeline processing 
	__of1x_init_packet_matches(pkt); 
	__of1x_init_packet_write_actions(pkt); 

	//Just process the action group
	__of1x_process_apply_actions((of1x_switch_t*)sw, 0, pkt, apply_actions_group, apply_actions_group->num_of_output_actions > 1 );
	
}
	
