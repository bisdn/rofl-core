#include "profiling_tests.h"
#include "rofl/datapath/pipeline/openflow/of_switch_pp.h"
#include "utils.h"

static of1x_switch_t* sw=NULL;
static datapacket_t pkt;
static uint64_t accumulated_time;

#define NUM_OF_ITERATONS 1000000
//#define NUM_OF_ITERATONS 10000
int set_up(){
	
	of1x_flow_entry_t* entry;
	uint32_t port_in=1;
	
	physical_switch_init();

	enum of1x_matching_algorithm_available ma_list[4]={of1x_loop_matching_algorithm, of1x_loop_matching_algorithm,
	of1x_loop_matching_algorithm, of1x_loop_matching_algorithm};

	//Create instance	
	sw = of1x_init_switch("Test switch", OF_VERSION_12, 0x0101,4,ma_list);
	
	if(!sw)
		return EXIT_FAILURE;

	//Set PORT_IN_MATCH 
	entry = of1x_init_flow_entry(false); 
	of1x_add_match_to_entry(entry,of1x_init_port_in_match(port_in));
	of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false);

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
void profile_basic_match(bool lock){

	int i;
	uint64_t tics;
	uint32_t average_tics;
	unsigned int tid = (lock) ? ROFL_PIPELINE_LOCKED_TID  : 1;

	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);

	//PKT
	*((uint32_t*)&tmp_val) = 1;

	//Execute
	for(i=0, accumulated_time=0;i<NUM_OF_ITERATONS;i++){
		//Measure time
		tics = rdtsc();

		//Process
		of_process_packet_pipeline(tid, (const struct of_switch *)sw, &pkt);
		
		//Accumulate
		accumulated_time += rdtsc() - tics;
	}

	//Calculate average
	average_tics = accumulated_time / NUM_OF_ITERATONS; 

	//Print
	fprintf(stderr,"\n%s MATCH num_of_iterations: %u average cycles: %u\n",  __func__, NUM_OF_ITERATONS, average_tics); 
	//TODO output results 
}
	
void profile_basic_no_match(bool lock){

	int i;
	uint64_t tics;
	uint32_t average_tics;
	unsigned int tid = (lock) ? ROFL_PIPELINE_LOCKED_TID  : 1;

	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);

	//PKT
	*((uint32_t*)&tmp_val) = 2;

	//Execute
	for(i=0, accumulated_time=0;i<NUM_OF_ITERATONS;i++){
		//Measure time
		tics = rdtsc();

		//Process
		of_process_packet_pipeline(tid, (const struct of_switch *)sw, &pkt);
		
		//Accumulate
		accumulated_time += rdtsc() - tics;
	}

	//Calculate average
	average_tics = accumulated_time / NUM_OF_ITERATONS; 

	//Print
	fprintf(stderr,"\n%s NO-MATCH num_of_iterations: %u average cycles: %u\n",  __func__, NUM_OF_ITERATONS, average_tics); 
		
	//TODO output results 
}

/* Test cases */
void profile_basic_match_lock(void){
	profile_basic_match(true);
}

void profile_basic_no_match_lock(void){
	profile_basic_no_match(true);
}

void profile_basic_match_no_lock(void){
	profile_basic_match(false);
}

void profile_basic_no_match_no_lock(void){
	profile_basic_no_match(false);
}
