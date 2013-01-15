#include "of12_pipeline.h"
#include "of12_instruction.h"
#include "../../../platform/lock.h"
#include "../../../platform/memory.h"
#include "matching_algorithms/matching_algorithms_available.h"

#include <stdio.h>

/* 
* This file implements the abstraction of a pipeline
*/

/* Management operations */
of12_pipeline_t* of12_init_pipeline(const unsigned int num_of_tables, enum matching_algorithm_available* list, const of12_flow_table_config_t table_config){
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
		if(of12_init_table(&pipeline->tables[i],i,table_config, list[i]) != EXIT_SUCCESS){
			cutil_free_shared(pipeline->tables);
			cutil_free_shared(pipeline);
			return NULL;
		}
	}
	
	return pipeline;
}

unsigned int of12_destroy_pipeline(of12_pipeline_t* pipeline){

	int i;
	
	for(i=0;i<pipeline->num_of_tables;i++){
		//We don't care about errors here, maybe add trace TODO
		of12_destroy_table(&pipeline->tables[i]);
	}
			
	//Now release table resources (allocated as single block)
	cutil_free_shared(pipeline->tables);

	cutil_free_shared(pipeline);

	return EXIT_SUCCESS;

}


/*
*
* Packet processing through pipeline
*
*/
unsigned int of12_process_packet_pipeline(const of12_pipeline_t* pipeline , datapacket_t *const pkt){

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
	for(i=OF12_FIRST_FLOW_TABLE_INDEX;i<pipeline->num_of_tables;i++){
		
		//Perform lookup	
		match = of12_find_best_match_table((of12_flow_table_t* const)&pipeline->tables[i],(of12_packet_matches_t *const)&pkt_matches);	
		
		if(match){
			fprintf(stderr,"Matched at table: %u, entry: %p\n",i,match);

			table_to_go = of12_process_instructions(pkt, &match->instructions);
	
			if(table_to_go > i && table_to_go < OF12_MAX_FLOWTABLES){
				fprintf(stderr,"Going to table %u->%u\n",i,table_to_go);
				i = table_to_go-1;

				//Green light for writers
				platform_rwlock_rdunlock(match->rwlock);
	
				continue;
			}

			//Process WRITE actions
			of12_process_write_actions(pkt);

			//Green light for writers
			platform_rwlock_rdunlock(match->rwlock);
	
			return EXIT_SUCCESS;
		}else{
			//Not matched, look for table_miss behaviour 
			if(pipeline->tables[i].default_action == OF12_TABLE_MISS_DROP){
				fprintf(stderr,"Table MISS_DROP %u\n",i);	
				return EXIT_FAILURE;
			}else if(pipeline->tables[i].default_action == OF12_TABLE_MISS_CONTROLLER){
				fprintf(stderr,"Table MISS_CONTROLLER %u\n",i);	
				/* FIXME: Generate packet in*/fprintf(stderr,"Packet at %p generated a PACKET_IN event to the controller\n",pkt);
				return EXIT_FAILURE;	
			}
			//else -> continue with the pipeline	
		}
	}	
	return EXIT_FAILURE;
}
