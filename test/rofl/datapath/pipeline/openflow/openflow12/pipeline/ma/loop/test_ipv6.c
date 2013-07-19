#include <stdio.h>
#include <assert.h>
#include "CUnit/Basic.h"
#include "test_ipv6.h"

static of12_switch_t* sw=NULL;

int ipv6_set_up(void){
	//this is the set up for all tests (its only called once before all test, not for each one)
	printf("ipv6 setting up\n");
	
	physical_switch_init();

	enum of12_matching_algorithm_available ma_list[4]={of12_matching_algorithm_loop, of12_matching_algorithm_loop,
	of12_matching_algorithm_loop, of12_matching_algorithm_loop};

	//Create instance	
	sw = of12_init_switch("Test switch",0x0101,4,ma_list);
	
	if(!sw)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}

int ipv6_tear_down(void){
	//this is the tear down for all tests (its only called once after all test, not for each one)
	printf("\nipv6 tear down\n");
	
	//Destroy the switch
	if(__of12_destroy_switch(sw) != ROFL_SUCCESS)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}


void ipv6_basic_test(void){
	printf("ipv6 test passed!\n");
}

void ipv6_utern_test(void){
	bool res;
	utern128_t *tern, *ex_tern, *ex_tern_fail;
	uint64_t value[2] = {0xffffffff,0xffff1234}, mask[2] = {0xffffffff,0xffff0000};
	uint64_t ex_value[2] = {0xffffffff,0x0fff1234}, ex_mask[2] = {0xffffffff,0x00000000};
	uint64_t ex_value_fail[2] = {0xfffffffc,0xffff1234}, ex_mask_fail[2] = {0xfffffffe,0x00000000};
	
	tern = (utern128_t *)__init_utern128(value,mask);
	CU_ASSERT(tern != NULL);
	
	res = __utern_compare128(tern,value);
	CU_ASSERT(res==true);
	
	res = __utern_compare128(tern,ex_value);
	CU_ASSERT(res==false);
	
	ex_tern = (utern128_t *)__init_utern128(ex_value,ex_mask);
	res = __utern_is_contained128(ex_tern,tern);
	CU_ASSERT(res==true);
	
	ex_tern_fail = (utern128_t *)__init_utern128(ex_value_fail,ex_mask_fail);
	res = __utern_is_contained128(ex_tern_fail,tern);
	CU_ASSERT(res==false);
	
	__destroy_utern((utern_t*)tern);
	__destroy_utern((utern_t*)ex_tern);
	__destroy_utern((utern_t*)ex_tern_fail);
}

void ipv6_install_flow_mod(void){
	printf("ipv6 test flow mod\n");
	uint64_t value[2] = {0xffffffff,0xffff1234}, mask[2] = {0xffffffff,0xffff0000};
	
	//Create a simple flow_mod
	of12_flow_entry_t* entry = of12_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	
	
	//add IPv6 match
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip6_src_match(NULL,NULL,value,mask)) == ROFL_SUCCESS);
	
	//Install
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, false,false) == ROFL_OF12_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	
	//Uninstall (specific)	
	CU_ASSERT(of12_remove_flow_entry_table(sw->pipeline, 0, entry, STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);
	
	
}