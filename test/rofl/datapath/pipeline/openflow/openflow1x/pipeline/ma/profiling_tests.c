/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "profiling_tests.h"
#include "rofl/datapath/pipeline/openflow/of_switch_pp.h"
#include "utils.h"

static of1x_switch_t* sw=NULL;
static datapacket_t pkt;
static uint64_t accumulated_time;

#define NUM_OF_ITERATONS 1000000
//#define NUM_OF_ITERATONS 10000
int set_up(){

	physical_switch_init();

	enum of1x_matching_algorithm_available ma_list[4]={of1x_loop_matching_algorithm, of1x_loop_matching_algorithm,
	of1x_loop_matching_algorithm, of1x_loop_matching_algorithm};

	//Create instance	
	sw = of1x_init_switch("Test switch", OF_VERSION_12, 0x0101,4,ma_list);
	
	if(!sw)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int tear_down(){
	//Destroy the switch
	if(__of1x_destroy_switch(sw) != ROFL_SUCCESS)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}

//tmp val
extern uint128__t tmp_val;

//Single flow_mod profiling
void profile_basic_match(){

	int i;
	uint64_t tics;
	uint32_t port_in=1, average_tics;
	of1x_flow_entry_t* entry;

	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 0);

	//Set PORT_IN_MATCH 
	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(port_in)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);

	//PKT
	*((uint32_t*)&tmp_val) = 1;

	//Execute
	for(i=0, accumulated_time=0;i<NUM_OF_ITERATONS;i++){
		//Measure time
		tics = rdtsc();

		//Process
		of_process_packet_pipeline((const struct of_switch *)sw, &pkt);
		
		//Accumulate
		accumulated_time += rdtsc() - tics;
	}

	//Calculate average
	average_tics = accumulated_time / NUM_OF_ITERATONS; 

	//Print
	fprintf(stderr,"\n%s MATCH num_of_iterations: %u average cycles: %u\n",  __func__, NUM_OF_ITERATONS, average_tics); 
	//TODO output results 
}
	
void profile_basic_no_match(){

	int i;
	uint64_t tics;
	uint32_t average_tics;

	//PKT
	*((uint32_t*)&tmp_val) = 2;

	//Execute
	for(i=0, accumulated_time=0;i<NUM_OF_ITERATONS;i++){
		//Measure time
		tics = rdtsc();

		//Process
		of_process_packet_pipeline((const struct of_switch *)sw, &pkt);
		
		//Accumulate
		accumulated_time += rdtsc() - tics;
	}

	//Calculate average
	average_tics = accumulated_time / NUM_OF_ITERATONS; 

	//Print
	fprintf(stderr,"\n%s NO-MATCH num_of_iterations: %u average cycles: %u\n",  __func__, NUM_OF_ITERATONS, average_tics); 
		
	//TODO output results 
}
