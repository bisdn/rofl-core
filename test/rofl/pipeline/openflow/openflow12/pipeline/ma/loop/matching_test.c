#include "matching_test.h"

static of12_switch_t* sw=NULL;
	
int set_up(){

	enum matching_algorithm_available ma_list[4]={matching_algorithm_loop, matching_algorithm_loop,
	matching_algorithm_loop, matching_algorithm_loop};

	//Create instance	
	sw = of12_init_switch("Test switch",0x0101,4,ma_list,1);
	
	if(!sw)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int tear_down(){
	//Destroy the switch
	if(of12_destroy_switch(sw) != ROFL_SUCCESS)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}

void test_install_simple_flow_mod(){

	//Create a simple flow_mod
	of12_flow_entry_t* entry = of12_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	

	//Install
	CU_ASSERT(of12_add_flow_entry_table(&sw->pipeline->tables[0], entry, false,false) == ROFL_OF12_FM_SUCCESS);
	
	//Uninstall (specific)	
	CU_ASSERT(of12_remove_flow_entry_table(&sw->pipeline->tables[0], entry, STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check really size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);
	
}

void test_uninstall_all(){

	//Install N flowmods
	
	//Uninstall all

	//Make sure size is 0 
	
}

void test_overlap(){

	//Install
	
	//Try overlapping(same flow_mod)

	//Try less restricting flow_mod overlapping

	//Test non overlapping flow_mod

	//Uninstall
	
}
#if 0
void add_and_delete_buckets_test(){
	
}

void concurrency_routine(void * param){

}

void concurrency_test(void){
	
}
#endif
