#include "reset_pipeline_test.h"

static of1x_switch_t* sw10=NULL;
static of1x_switch_t* sw12=NULL;
static of1x_switch_t* sw13=NULL;
static of1x_switch_t* sw=NULL;
	
static enum of1x_matching_algorithm_available ma_list[4]={of1x_matching_algorithm_loop, of1x_matching_algorithm_loop, of1x_matching_algorithm_loop, of1x_matching_algorithm_loop};


int set_up(){

	physical_switch_init();

	//Create instances	
	sw = of1x_init_switch("Test switch", OF_VERSION_12, 0x0101,4,ma_list);
	sw10 = of1x_init_switch("Test switch10", OF_VERSION_10, 0x0101,4,ma_list);
	sw12 = of1x_init_switch("Test switch12", OF_VERSION_12, 0x0101,4,ma_list);
	sw13 = of1x_init_switch("Test switch13", OF_VERSION_13, 0x0101,4,ma_list);
	
	if(!sw || !sw10 || !sw12 || !sw13)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int tear_down(){
	//Destroy the switch
	if(__of1x_destroy_switch(sw) != ROFL_SUCCESS)
		return EXIT_FAILURE;
	if(__of1x_destroy_switch(sw10) != ROFL_SUCCESS)
		return EXIT_FAILURE;
	if(__of1x_destroy_switch(sw12) != ROFL_SUCCESS)
		return EXIT_FAILURE;
	if(__of1x_destroy_switch(sw13) != ROFL_SUCCESS)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}

void test_purge(){

	unsigned int i;
	of1x_flow_entry_t* entry;

	//Install a flow in each table
	for(i=0;i<4;i++){
		entry = of1x_init_flow_entry(false); 
		CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(1)) == ROFL_SUCCESS);
	
		CU_ASSERT(entry != NULL);
		CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, i, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	}

	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	CU_ASSERT(sw->pipeline.tables[1].num_of_entries == 1);
	CU_ASSERT(sw->pipeline.tables[2].num_of_entries == 1);
	CU_ASSERT(sw->pipeline.tables[3].num_of_entries == 1);
	
	//Purge 
	CU_ASSERT(__of1x_purge_pipeline_entries(&sw->pipeline) == ROFL_SUCCESS);

	//Check size of the table again
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 0);
	CU_ASSERT(sw->pipeline.tables[1].num_of_entries == 0);
	CU_ASSERT(sw->pipeline.tables[2].num_of_entries == 0);
	CU_ASSERT(sw->pipeline.tables[3].num_of_entries == 0);
}

static void reset_sw(of_version_t version){
	CU_ASSERT(__of1x_destroy_switch(sw) == ROFL_SUCCESS);
	sw = of1x_init_switch("Test switch", version, 0x0101,4,ma_list);
	CU_ASSERT(sw != NULL);
}

static int compare(of1x_switch_t* sw1, of1x_switch_t* sw2){
	if(memcmp(&sw1->pipeline.tables[0].config, &sw2->pipeline.tables[0].config, sizeof(sw1->pipeline.tables[0].config)) != 0)
		return 1;
	if(memcmp(&sw1->pipeline.tables[0].default_action, &sw2->pipeline.tables[0].default_action, sizeof(sw1->pipeline.tables[0].default_action)) != 0)
		return 1;

	return 0;
}

void test_reconfigure(){

	/*
	* initially OF12
	*/
	//OF12->OF10
	reset_sw(OF_VERSION_12);
	CU_ASSERT(of_reconfigure_switch((of_switch_t*)sw, OF_VERSION_10) == ROFL_SUCCESS);
	CU_ASSERT(compare(sw, sw10) == 0);

	//OF12->OF13
	reset_sw(OF_VERSION_12);
	CU_ASSERT(of_reconfigure_switch((of_switch_t*)sw, OF_VERSION_13) == ROFL_SUCCESS);
	CU_ASSERT(compare(sw, sw13) == 0);

	/*
	* initially OF10
	*/
	//OF10->OF12
	reset_sw(OF_VERSION_10);
	CU_ASSERT(of_reconfigure_switch((of_switch_t*)sw, OF_VERSION_12) == ROFL_SUCCESS);
	CU_ASSERT(compare(sw, sw12) == 0);

	//OF10->OF13
	reset_sw(OF_VERSION_10);
	CU_ASSERT(of_reconfigure_switch((of_switch_t*)sw, OF_VERSION_13) == ROFL_SUCCESS);
	CU_ASSERT(compare(sw, sw13) == 0);



	/*
	* initially OF13
	*/
	//OF13->OF10
	reset_sw(OF_VERSION_13);
	CU_ASSERT(of_reconfigure_switch((of_switch_t*)sw, OF_VERSION_10) == ROFL_SUCCESS);
	CU_ASSERT(compare(sw, sw10) == 0);

	//OF13->OF12
	reset_sw(OF_VERSION_13);
	CU_ASSERT(of_reconfigure_switch((of_switch_t*)sw, OF_VERSION_12) == ROFL_SUCCESS);
	CU_ASSERT(compare(sw, sw12) == 0);
}
