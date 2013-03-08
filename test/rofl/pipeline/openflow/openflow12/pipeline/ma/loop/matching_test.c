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

void test_install_empty_flow_mod(){

	//Create a simple flow_mod
	of12_flow_entry_t* entry = of12_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	

	//Install
	CU_ASSERT(of12_add_flow_entry_table(&sw->pipeline->tables[0], entry, false,false) == ROFL_OF12_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	
	//Uninstall (specific)	
	CU_ASSERT(of12_remove_flow_entry_table(&sw->pipeline->tables[0], entry, STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);
	
}

void test_install_overlapping_specific(){

	unsigned int i, num_of_flows=rand()%20;
	of12_flow_entry_t* entries[20];

	//Install N flowmods which identical => should put only one
	for(i=0;i<num_of_flows;i++){
		entries[i] = of12_init_flow_entry(NULL, NULL, false); 
		CU_ASSERT(entries[i] != NULL);
		if(i>1)	
			CU_ASSERT(entries[i] != entries[i-1]);
		CU_ASSERT(of12_add_flow_entry_table(&sw->pipeline->tables[0], entries[i], false,false) == ROFL_OF12_FM_SUCCESS);
	}

	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	
	//Uninstall all using specific method
	for(i=0;i<num_of_flows;i++){
		rofl_result_t specific_remove_result = of12_remove_flow_entry_table(&sw->pipeline->tables[0], entries[i], STRICT, OF12_PORT_ANY, OF12_GROUP_ANY);

		if(i){
			CU_ASSERT( specific_remove_result != ROFL_SUCCESS ); //First must succeeed
		}else{
			CU_ASSERT( specific_remove_result == ROFL_SUCCESS ); //Rest fail
		}
		if(!i && (specific_remove_result != ROFL_SUCCESS)){
			of12_dump_table(&sw->pipeline->tables[0]);
		}
	}

	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);	
}


static void test_uninstall_wildcard_add_flows(of12_flow_entry_t** entries, unsigned int num_of_flows){
	unsigned int i;

	//Install N flowmods with one identical match and the rest randomly generated
	for(i=0;i<num_of_flows;i++){
		entries[i] = of12_init_flow_entry(NULL, NULL, false); 
	
		//Add two match common
		CU_ASSERT(of12_add_match_to_entry(entries[i],of12_init_port_in_match(NULL,NULL,1)) == ROFL_SUCCESS);
		CU_ASSERT(of12_add_match_to_entry(entries[i],of12_init_eth_src_match(NULL,NULL,0x012345678901, 0xFFFFFFFFFFFF)) == ROFL_SUCCESS);

		//Add random matches 
		CU_ASSERT(of12_add_match_to_entry(entries[i],of12_init_eth_dst_match(NULL,NULL,rand()%0xFFFFFFFF, 0xFFFFFFFFFFFF)) == ROFL_SUCCESS);

		if(rand()%100 > 50)
			CU_ASSERT(of12_add_match_to_entry(entries[i],of12_init_ip4_dst_match(NULL,NULL,rand()%0xFFFFFFFF, 0xFFFFFFF)) == ROFL_SUCCESS);

		if(rand()%100 > 80)
			CU_ASSERT(of12_add_match_to_entry(entries[i],of12_init_ip4_src_match(NULL,NULL,rand()%0xFFFFFFFF, 0xFFFFFFF)) == ROFL_SUCCESS);
	
		CU_ASSERT(of12_add_flow_entry_table(&sw->pipeline->tables[0], entries[i], false,false) == ROFL_OF12_FM_SUCCESS);
	}

}

void test_uninstall_wildcard(){

	unsigned int num_of_flows=rand()%20;
	of12_flow_entry_t* entries[20], *deleting_entry;


//With PORT_IN (first match)
	//Add flows	
	test_uninstall_wildcard_add_flows(entries, num_of_flows);

	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == num_of_flows);

	//Do the deletion with the common match
	deleting_entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(deleting_entry != NULL);
	CU_ASSERT(of12_add_match_to_entry(deleting_entry,of12_init_port_in_match(NULL,NULL,1)) == ROFL_SUCCESS);

	CU_ASSERT(of12_remove_flow_entry_table(&sw->pipeline->tables[0], deleting_entry, NOT_STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);

//With ETH_SRC only (second match)	
	//Reload
	test_uninstall_wildcard_add_flows(entries, num_of_flows);

	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == num_of_flows);

	//Do the deletion with the common match
	deleting_entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(deleting_entry != NULL);
	CU_ASSERT(of12_add_match_to_entry(deleting_entry,of12_init_eth_src_match(NULL,NULL,0x012345678901, 0xFFFFFFFFFFFF)) == ROFL_SUCCESS);

	CU_ASSERT(of12_remove_flow_entry_table(&sw->pipeline->tables[0], deleting_entry, NOT_STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);

//With ALL with masks to 0 (the ones possible)
	//Reload
	test_uninstall_wildcard_add_flows(entries, num_of_flows);

	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == num_of_flows);

	//Do the deletion with the common match
	deleting_entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(deleting_entry != NULL);
	CU_ASSERT(of12_add_match_to_entry(deleting_entry,of12_init_port_in_match(NULL,NULL,1)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(deleting_entry,of12_init_eth_src_match(NULL,NULL,0x999999999999, 0x000000000000)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(deleting_entry,of12_init_ip4_dst_match(NULL,NULL,rand()%0x11111111, 0x0000000)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(deleting_entry,of12_init_ip4_dst_match(NULL,NULL,rand()%0x22222222, 0x0000000)) == ROFL_SUCCESS);

	CU_ASSERT(of12_remove_flow_entry_table(&sw->pipeline->tables[0], deleting_entry, NOT_STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);


//With no-match(complete wildcard 
	//Reload
	test_uninstall_wildcard_add_flows(entries, num_of_flows);

	//Do the deletion with no matches
	deleting_entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(deleting_entry != NULL);
	
	CU_ASSERT(of12_remove_flow_entry_table(&sw->pipeline->tables[0], deleting_entry, NOT_STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);

}
#if 0
void add_and_delete_buckets_test(){
	
}

void concurrency_routine(void * param){

}

void concurrency_test(void){
	
}
#endif
