#include <stdio.h>
#include <assert.h>
#include "CUnit/Basic.h"
#include "test_bufs.h"
#include "io.h"

static of1x_switch_t* sw=NULL;
static datapacket_t* pkt=NULL;

int bufs_set_up(void){
	//this is the set up for all tests (its only called once before all test, not for each one)
	printf("bufs setting up\n");
	
	physical_switch_init();

	enum of1x_matching_algorithm_available ma_list[4]={of1x_matching_algorithm_loop, of1x_matching_algorithm_loop,
	of1x_matching_algorithm_loop, of1x_matching_algorithm_loop};

	//Create instance	
	sw = of1x_init_switch("Test switch", OF_VERSION_12, 0x0101, 4, ma_list);
	
	if(!sw)
		return EXIT_FAILURE;

	init_io();

	return EXIT_SUCCESS;
}

int bufs_tear_down(void){
	//this is the tear down for all tests (its only called once after all test, not for each one)
	printf("\nbufs tear down\n");
	
	//Destroy the switch
	if(__of1x_destroy_switch(sw) != ROFL_SUCCESS)
		return EXIT_FAILURE;
	
	destroy_io();
	
	return EXIT_SUCCESS;
}


////////////
// Tests //
//////////

//No output action (drop)
void bufs_no_output_action(void){
	
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(sw->pipeline, 0, entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	
	//Process packet through pipeline
	pkt = allocate_buffer();	
	
	CU_ASSERT(pkt != NULL);	
	
	if(!pkt)
		return;

	//Dump pipeline
	of1x_full_dump_switch(sw);

	//Process through pipeline. Packet should be dropped
	of_process_packet_pipeline((of_switch_t*)sw,pkt);

	//Checkings	
	CU_ASSERT(allocated == 1);	
	CU_ASSERT(released == 1);	
	CU_ASSERT(drops == 1);	
	CU_ASSERT(outputs == 0);	
	CU_ASSERT(replicas == 0);	
}


//Simple apply actions output
void bufs_apply_output_action(void){

}
//Simple write actions output

//Multiple output in apply actions

//An output action in both apply and write actions

//An output action in a group

//An output action in a group and in an apply actions

//An output action in a group and in write actions

//Multiple output actions in a group 

//Multiple output actions in a bucket list (group type all)

//No action output in the first table, no action on the second (drop)

//No action output in the first table, apply output action in the second

//No action output in the first table, write output action in the second

//No action output in the first table, group output action in the second

//No action output in the first table, group output action in the second

