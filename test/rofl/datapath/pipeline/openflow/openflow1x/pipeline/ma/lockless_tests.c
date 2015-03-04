#include "lockless_tests.h"
#include "rofl/datapath/pipeline/openflow/of_switch_pp.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "CUnit/Basic.h"
#include "lockless_tests.h"


static of1x_switch_t* sw=NULL;

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


//Force a lot of context swaps
#define NUM_OF_IO_THREADS ROFL_PIPELINE_MAX_TIDS
#define NUM_OF_ITERATIONS 100000

unsigned int ids[NUM_OF_IO_THREADS];
pthread_t mgmt;
pthread_t threads[NUM_OF_IO_THREADS];
bool keep_on=true;

void* mgmt_thread(void* none){

	unsigned int cnt = 0;
	of1x_flow_entry_t* entry;
	uint32_t port_in;

	while(cnt != NUM_OF_ITERATIONS){

		entry = of1x_init_flow_entry(false);

		if(rand() % 2)
			port_in = 1;
		else
			port_in = 2;

		of1x_add_match_to_entry(entry,of1x_init_port_in_match(port_in));

		//First destroy
		of1x_remove_flow_entry_table(&sw->pipeline, 0, entry, NOT_STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY);

		//The add
		of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false);

		cnt++;
		if(cnt%100 == 0)
			fprintf(stderr, "Processed : %u\n", cnt);
	}


	return NULL;
}

void* io_thread(void* id){

	datapacket_t pkt;
	unsigned int tid = *((unsigned int*)id);
	unsigned int cnt = 0;

	fprintf(stderr, "Launching I/O thread: %d\n",tid);

	while(keep_on){
		//PKT
		if(rand() % 2)
			*((uint32_t*)&tmp_val) = 1;
		else
			*((uint32_t*)&tmp_val) = 2;

		//Process
		of_process_packet_pipeline(tid, (const struct of_switch *)sw, &pkt);



		//if(rand() % 100 > 40)
		//	usleep(10);

		cnt++;
	}

	//Check our stats
	CU_ASSERT(sw->pipeline.tables[0].stats.s.__internal[tid].lookup_count == cnt);

	return NULL;
}

//Single flow_mod profiling
void lockless_basic(){

	int i;

	srand(time(NULL));

	//launch mgmt_thread
	int return_val = pthread_create(&mgmt, NULL, mgmt_thread, NULL);
	CU_ASSERT(return_val == 0);


	//Launch workers
	for(i=0;i<NUM_OF_IO_THREADS;i++){
		int return_val;
		ids[i] = i;
		return_val = pthread_create(&threads[i], NULL, io_thread, &ids[i]);
		CU_ASSERT(return_val == 0);
	}

	//Stop mgmt core
	keep_on=false;
	pthread_join(mgmt, NULL);


	//Wait for them
	for(i=0;i<NUM_OF_IO_THREADS;i++){
		pthread_join(threads[i], NULL);
	}

	//Check all stats
	__of1x_stats_table_tid_t c;
	__of1x_stats_table_consolidate(&sw->pipeline.tables[0].stats, &c);
}

int main(int args, char** argv){

	int return_code;
	//main to call all the other tests written in the oder files in this folder
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("Suite_Pipeline_lockless", set_up, tear_down);

	if (NULL == pSuite){
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	/* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
	if (
		(NULL == CU_add_test(pSuite, "Lockless", lockless_basic) )

	)
	{
		fprintf(stderr,"ERROR WHILE ADDING TEST\n");
		return_code = CU_get_error();
		CU_cleanup_registry();
		return return_code;
	}

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	return_code = CU_get_number_of_failures();
	CU_cleanup_registry();

	return return_code;
}
