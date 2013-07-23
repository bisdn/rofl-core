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
	//FIXME uint64_t has 8 bytes =>  0x0102030405060708 !!!
	uint64_t value[2] = 			{0x12345678ffffffff,0x12345678ffff1234}, mask[2] = 			{0xffffffffffffffff,0xffffffffffff0000};
	uint64_t ex_value[2] = 			{0x12345678ffffffff,0x12345678fff01234}, ex_mask[2] = 			{0xffffffffffffffff,0xffffffff00000000};
	uint64_t ex_value_2[2] = 		{0x1234567affffffff,0x12345678ffff1234},
			ex_value_3[2] = 		{0x12345678ffffffff,0x12345678ffff123f};
	uint64_t ex_value_fail[2] = 	{0x12345678fffffffc,0x12345678ffff1234}, ex_mask_fail[2] = 	{0xfffffffffffffffe,0xffffffff00000000};
	
	tern = (utern128_t *)__init_utern128(value,mask);
	CU_ASSERT(tern != NULL);
	
	res = __utern_compare128(tern,value);
	CU_ASSERT(res==true);
	
	res = __utern_compare128(tern,ex_value);
	CU_ASSERT(res==false);
	
	res = __utern_compare128(tern,ex_value_2);
	CU_ASSERT(res==false);
	
	res = __utern_compare128(tern,ex_value_3);
	CU_ASSERT(res==true);
	
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

void ipv6_alike_test_low(void){
	utern128_t * tern1,*tern2,*res;
	//						{		LOW		,		HIGH		}
	uint64_t mask[2] =   {0xffffffffffffffff,0xffffffffffffffff};
	uint64_t value1[2] = {0x1111222233334444,0xaaaabbbbccccdddd};
	uint64_t value2[2] = {0x1111222233335444,0xaaaabbbbccccdddd};
	
	tern1 = (utern128_t *)__init_utern128(value1,mask);
	tern2 = (utern128_t *)__init_utern128(value2,mask);
	res = (utern128_t *)__utern128_get_alike(*tern1,*tern2);
	
	CU_ASSERT(res!=NULL);
	if(res){
		printf("1- masks 0x%lx 0x%lx\n",res->mask[UTERN128_HIGH],res->mask[UTERN128_LOW]);
		CU_ASSERT(res->mask[UTERN128_HIGH]==0xffffffffffffffff);
		CU_ASSERT(res->mask[UTERN128_LOW] ==0xffffffffffffe000);
	}
	__destroy_utern((utern_t*)tern1);
	__destroy_utern((utern_t*)tern2);
	__destroy_utern((utern_t*)res);
}
	
void ipv6_alike_test_high(void){
	utern128_t * tern1,*tern2,*res;
	// Second test: change of mask and add different bytes in higher part
	//						{		LOW		,		HIGH		}
	uint64_t mask[2] =   {0xfffffffffff00000,0xffffffffffffffff};
	uint64_t value1[2] = {0x1111222233334444,0xaaaabbbbccccdddf};
	uint64_t value2[2] = {0x1111222233335444,0xaaaabbbbccccdddd};
	
	tern1 = (utern128_t *)__init_utern128(value1,mask);
	tern2 = (utern128_t *)__init_utern128(value2,mask);
	res = (utern128_t *)__utern128_get_alike(*tern1,*tern2);
	CU_ASSERT(res!=NULL);
	if(res){
		printf("2- masks 0x%lx 0x%lx\n",res->mask[UTERN128_HIGH],res->mask[UTERN128_LOW]);
		CU_ASSERT(res->mask[UTERN128_HIGH]==0xfffffffffffffffc);
		CU_ASSERT(res->mask[UTERN128_LOW] ==0x0000000000000000);
	}
	__destroy_utern((utern_t*)tern1);
	__destroy_utern((utern_t*)tern2);
	__destroy_utern((utern_t*)res);

}

void ipv6_install_flow_mod(void){
	printf("ipv6 test flow mod\n");
	uint64_t value[2] = {0xffffffffffffffff,0xffffffffffff1234}, mask[2] = {0xffffffffffffffff,0xffffffffffff0000};
	
	//Create a simple flow_mod
	of12_flow_entry_t* entry = of12_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	
	
	entry->cookie_mask = 0x1;
	
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

void ipv6_install_flow_mod_complete(void){
	printf("ipv6 test flow mod\n");
	uint64_t value128[2] = {0xffffffffffffffff,0xffffffffffff1234}, mask128[2] = {0xffffffffffffffff,0xffffffffffff0000};
	uint64_t value64 = 0xffffffffffff1234, mask64 = 0xffffffffffff0000;
	
	//Create a simple flow_mod
	of12_flow_entry_t* entry = of12_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	
	
	entry->cookie_mask = 0x1;
	
	//add IPv6 match
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip6_src_match(NULL,NULL,value128,mask128)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip6_dst_match(NULL,NULL,value128,mask128)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip6_flabel_match(NULL,NULL,value64,mask64)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip6_nd_target_match(NULL,NULL,value128,mask128)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip6_nd_sll_match(NULL,NULL,value64,mask64)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip6_nd_tll_match(NULL,NULL,value64,mask64)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip6_exthdr_match(NULL,NULL,value64,mask64)) == ROFL_SUCCESS);
	
	//Install
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, false,false) == ROFL_OF12_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	
	//Uninstall (specific)	
	CU_ASSERT(of12_remove_flow_entry_table(sw->pipeline, 0, entry, STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);
	
	
}

void icmpv6_install_flow_mod_complete(void){
	printf("ipv6 test flow mod\n");
	uint64_t value64 = 0xffffffffffff1234, mask64 = 0xffffffffffff0000;
	
	//Create a simple flow_mod
	of12_flow_entry_t* entry = of12_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	
	
	entry->cookie_mask = 0x1;
	
	//add IPv6 match
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_icmpv6_type_match(NULL,NULL,value64,mask64)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_icmpv6_code_match(NULL,NULL,value64,mask64)) == ROFL_SUCCESS);
	
	//Install
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, false,false) == ROFL_OF12_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	
	//Uninstall (specific)	
	CU_ASSERT(of12_remove_flow_entry_table(sw->pipeline, 0, entry, STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);
}