#include "of12_pipeline.h"
#include "of12_instruction.h"
#include "../../../platform/lock.h"
#include "../../../platform/memory.h"
#include "../../../platform/platform_hooks.h"
#include "../../../platform/openflow/openflow12/platform_hooks_of12.h"
#include "matching_algorithms/matching_algorithms_available.h"
#include "../of12_switch.h"

#include <stdio.h>

/* 
* This file implements the abstraction of a pipeline
*/

/* Management operations */
of12_pipeline_t* of12_init_pipeline(const unsigned int num_of_tables, enum matching_algorithm_available* list, const of12_flow_table_miss_config_t table_config){
	int i;	
	of12_pipeline_t* pipeline;

	//Verify params
	if( ! (num_of_tables <= OF12_MAX_FLOWTABLES && num_of_tables > 0) )
		return NULL;
	
	pipeline = (of12_pipeline_t*)cutil_malloc_shared(sizeof(of12_pipeline_t));

	if(!pipeline)
		return NULL;
	
	//Fill in
	pipeline->num_of_tables = num_of_tables;


	//Allocate tables and initialize	
	pipeline->tables = (of12_flow_table_t*)cutil_malloc_shared(sizeof(of12_flow_table_t)*num_of_tables);
	
	if(!pipeline->tables){
		cutil_free_shared(pipeline);
		return NULL;
	}

	for(i=0;i<num_of_tables;i++){
		//TODO: if we would have tables with different config, table_config should be an array of table_config_t objects, one for each table
		if(of12_init_table(&pipeline->tables[i],i,table_config, list[i]) != ROFL_SUCCESS){
			cutil_free_shared(pipeline->tables);
			cutil_free_shared(pipeline);
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
	

	return pipeline;
}

rofl_result_t of12_destroy_pipeline(of12_pipeline_t* pipeline){

	int i;
	
	for(i=0;i<pipeline->num_of_tables;i++){
		//We don't care about errors here, maybe add trace TODO
		of12_destroy_table(&pipeline->tables[i]);
	}
			
	//Now release table resources (allocated as single block)
	cutil_free_shared(pipeline->tables);

	cutil_free_shared(pipeline);

	return ROFL_SUCCESS;

}


/*
*
* Packet processing through pipeline
*
*/
void of12_process_packet_pipeline(const of_switch_t *sw, datapacket_t *const pkt){

	//Loop over tables
	unsigned int i, table_to_go;
	of12_flow_entry_t* match;
	
	//Temporal stack vars for matches and write actions (faster than dyn. mem)
	of12_packet_matches_t pkt_matches;
	of12_write_actions_t write_actions;
	
	//Initialize packet for OF1.2 pipeline processing 
	of12_init_packet_matches(pkt, &pkt_matches); 
	of12_init_packet_write_actions(pkt, &write_actions); 
		
	//FIXME: add metadata+write operations 
	for(i=OF12_FIRST_FLOW_TABLE_INDEX; i < ((of12_switch_t*)sw)->pipeline->num_of_tables ; i++){
		
		//Perform lookup	
		match = of12_find_best_match_table((of12_flow_table_t* const)&((of12_switch_t*)sw)->pipeline->tables[i],(of12_packet_matches_t *const)&pkt_matches);
		
		if(match){
			fprintf(stderr,"Matched at table: %u, entry: %p\n",i,match);

			//Update table and entry statistics
			of12_stats_table_matches_inc(&((of12_switch_t*)sw)->pipeline->tables[i]);
			of12_stats_flow_update_match(match, pkt_matches.pkt_size_bytes);

			//Update entry timers
			of12_timer_update_entry(match);

			//Process instructions
			table_to_go = of12_process_instructions((of12_switch_t*)sw, i, pkt, &match->inst_grp);

	
			if(table_to_go > i && table_to_go < OF12_MAX_FLOWTABLES){

				fprintf(stderr,"Going to table %u->%u\n",i,table_to_go);
				i = table_to_go-1;

				//Unlock the entry so that it can eventually be modified/deleted
				platform_rwlock_rdunlock(match->rwlock);
	
				continue;
			}

			//Process WRITE actions
			of12_process_write_actions((of12_switch_t*)sw, i, pkt, match->inst_grp.has_multiple_outputs);

			//Unlock the entry so that it can eventually be modified/deleted
			platform_rwlock_rdunlock(match->rwlock);

			//Drop packet Only if there has been copy(cloning of the packet) due to 
			//multiple output actions
			if(match->inst_grp.has_multiple_outputs)
				platform_packet_drop(pkt);
							

			return;	
		}else{
			//Update table statistics
			of12_stats_table_lookup_inc(&((of12_switch_t*)sw)->pipeline->tables[i]);

			//Not matched, look for table_miss behaviour 
			if(((of12_switch_t*)sw)->pipeline->tables[i].default_action == OF12_TABLE_MISS_DROP){

				fprintf(stderr,"Table MISS_DROP %u\n",i);	
				platform_packet_drop(pkt);
				return;

			}else if(((of12_switch_t*)sw)->pipeline->tables[i].default_action == OF12_TABLE_MISS_CONTROLLER){
			
				fprintf(stderr,"Table MISS_CONTROLLER %u\n",i);	
				fprintf(stderr,"Packet at %p generated a PACKET_IN event to the controller\n",pkt);

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
void of12_process_packet_out_pipeline(const of_switch_t *sw, datapacket_t *const pkt, const of12_action_group_t* apply_actions_group){

	//Temporal stack vars for matches and write actions
	of12_packet_matches_t pkt_matches;
	of12_write_actions_t write_actions;
	
	//Initialize packet for OF1.2 pipeline processing 
	of12_init_packet_matches(pkt, &pkt_matches); 
	of12_init_packet_write_actions(pkt, &write_actions); 

	//Just process the action group
	of12_process_apply_actions((of12_switch_t*)sw, 0, pkt, apply_actions_group, apply_actions_group->num_of_output_actions > 1 );
	
}
	
