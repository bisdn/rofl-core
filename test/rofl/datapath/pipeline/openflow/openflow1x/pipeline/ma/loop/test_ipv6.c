#include <stdio.h>
#include <assert.h>
#include "CUnit/Basic.h"
#include "test_ipv6.h"

#define HI(x) *(uint64_t*)&x.val[0]
#define LO(x) *(uint64_t*)&x.val[8]

static of1x_switch_t* sw=NULL;

int ipv6_set_up(void){
	//this is the set up for all tests (its only called once before all test, not for each one)
	printf("ipv6 setting up\n");
	
	physical_switch_init();

	enum of1x_matching_algorithm_available ma_list[4]={of1x_matching_algorithm_loop, of1x_matching_algorithm_loop,
	of1x_matching_algorithm_loop, of1x_matching_algorithm_loop};

	//Create instance	
	sw = of1x_init_switch("Test switch", OF_VERSION_12, 0x0101, 4, ma_list);
	
	if(!sw)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}

int ipv6_tear_down(void){
	//this is the tear down for all tests (its only called once after all test, not for each one)
	printf("\nipv6 tear down\n");
	
	//Destroy the switch
	if(__of1x_destroy_switch(sw) != ROFL_SUCCESS)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}


void ipv6_basic_test(void){
	printf("ipv6 test passed!\n");
}

void ipv6_utern_test(void){
	bool res;
	utern_t *tern, *ex_tern, *ex_tern_fail;
	uint128__t value = 				{{0x12,0x34,0x56,0x78,0xff,0xff,0xff,0xff,0x12,0x34,0x56,0x78,0xff,0xff,0x12,0x34}};
	uint128__t mask = 				{{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00}};
	uint128__t ex_value = 			{{0x12,0x34,0x56,0x78,0xff,0xff,0xff,0xff,0x12,0x34,0x56,0x78,0xff,0xf0,0x12,0x34}};
	uint128__t ex_mask = 			{{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00}};
	uint128__t ex_value_2 = 		{{0x12,0x34,0x56,0x7a,0xff,0xff,0xff,0xff,0x12,0x34,0x56,0x78,0xff,0xff,0x12,0x34}};
	uint128__t ex_value_3 = 		{{0x12,0x34,0x56,0x78,0xff,0xff,0xff,0xff,0x12,0x34,0x56,0x78,0xff,0xff,0x12,0x3f}};
	uint128__t ex_value_fail = 		{{0x12,0x34,0x56,0x78,0xff,0xff,0xff,0xfc,0x12,0x34,0x56,0x78,0xff,0xff,0x12,0x34}};
	uint128__t ex_mask_fail = 		{{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00}};
	
	tern = __init_utern128(value,mask);
	CU_ASSERT(tern != NULL);
	
	res = __utern_compare128(tern,value);
	CU_ASSERT(res==true);
	
	res = __utern_compare128(tern,ex_value);
	CU_ASSERT(res==false);
	
	res = __utern_compare128(tern,ex_value_2);
	CU_ASSERT(res==false);
	
	res = __utern_compare128(tern,ex_value_3);
	CU_ASSERT(res==true);
	
	ex_tern = __init_utern128(ex_value,ex_mask);
	res = __utern_is_contained(ex_tern,tern);
	CU_ASSERT(res==true);
	
	ex_tern_fail = __init_utern128(ex_value_fail,ex_mask_fail);
	res = __utern_is_contained(ex_tern_fail,tern);
	CU_ASSERT(res==false);
	
	__destroy_utern((utern_t*)tern);
	__destroy_utern((utern_t*)ex_tern);
	__destroy_utern((utern_t*)ex_tern_fail);
}

void ipv6_alike_test_low(void){
	utern_t * tern1,*tern2,*res;
	//WARNING!!!						{		HIGH		,		LOW		}
	uint128__t mask; HI(mask) =  0xffffffffffffffff; LO(mask) = 0xffffffffffffffff;
	uint128__t value1; HI(value1) = 0xaaaabbbbccccdddd; LO(value1) =0x1111222233334444;
	uint128__t value2; HI(value2) = 0xaaaabbbbccccdddd; LO(value2) =0x1111222233335444;
	
	tern1 = __init_utern128(value1,mask);
	tern2 = __init_utern128(value2,mask);
	res = __utern_get_alike(*tern1,*tern2);
	
	CU_ASSERT(res!=NULL);
	if(res){
		printf("1- masks 0x%lx 0x%lx\n",HI(res->mask.u128),LO(res->mask.u128));
		CU_ASSERT(HI(res->mask.u128)==0xffffffffffffffff);
		CU_ASSERT(LO(res->mask.u128) ==0xffffffffffffe000);
	}
	__destroy_utern((utern_t*)tern1);
	__destroy_utern((utern_t*)tern2);
	__destroy_utern((utern_t*)res);
}
	
void ipv6_alike_test_high(void){
	utern_t * tern1,*tern2,*res;
	// Second test: change of mask and add different bytes in higher part
	uint128__t mask;   HI(mask) = 0xffffffffffffffff; LO(mask) = 0xfffffffffff00000;
	uint128__t value1; HI(value1) = 0xaaaabbbbccccdddf; LO(value1) = 0x1111222233334444;
	uint128__t value2; HI(value2) = 0xaaaabbbbccccdddd; LO(value2) = 0x1111222233335444; 
	
	tern1 = __init_utern128(value1,mask);
	tern2 = __init_utern128(value2,mask);
	res = __utern_get_alike(*tern1,*tern2);
	CU_ASSERT(res!=NULL);
	if(res){
		printf("2- masks 0x%lx 0x%lx\n",HI(res->mask.u128),LO(res->mask.u128));
		CU_ASSERT(HI(res->mask.u128)==0xfffffffffffffffc);
		CU_ASSERT(LO(res->mask.u128)==0x0000000000000000);
	}
	__destroy_utern((utern_t*)tern1);
	__destroy_utern((utern_t*)tern2);
	__destroy_utern((utern_t*)res);
}

void ipv6_alike_test_wrong(void){
	utern_t * tern1,*tern2,*res;
	// masks have nothing in common
	uint128__t value1; HI(value1) = 0xaaaaaaaaffffffff; LO(value1) = 0x1111222233334444;
	uint128__t mask1;  HI(mask1)  = 0xffffffffffffffff; LO(mask1)  = 0x0000000000000000;
	uint128__t value2; HI(value2) = 0xaaaabbbbccccdddd; LO(value2) = 0x1111222233335444; 
	uint128__t mask2;  HI(mask2)  = 0x0000000000000000; LO(mask2)  = 0xffffffffffffffff;
	
	tern1 = __init_utern128(value1,mask1);
	tern2 = __init_utern128(value2,mask2);
	res = __utern_get_alike(*tern1,*tern2);
	CU_ASSERT(res==NULL);
	
	__destroy_utern((utern_t*)tern1);
	__destroy_utern((utern_t*)tern2);
	__destroy_utern((utern_t*)res);
	
	// masks have something in commom but they are not continuous
	HI(value1) = 0xaaaaaaaaffffffff; LO(value1) = 0x1111222233334444;
	HI(mask1)  = 0x0000000000000000; LO(mask1)  = 0xffffffff00000000;
	HI(value2) = 0xaaaaaaaaffffffff; LO(value2) = 0x1111222233334444; 
	HI(mask2)  = 0x0000000000000000; LO(mask2)  = 0x00ffffffffffffff;
	
	tern1 = __init_utern128(value1,mask1);
	tern2 = __init_utern128(value2,mask2);
	res = __utern_get_alike(*tern1,*tern2);
	CU_ASSERT(res==NULL);
	
	__destroy_utern((utern_t*)tern1);
	__destroy_utern((utern_t*)tern2);
	__destroy_utern((utern_t*)res);
	
}

/*TODO do tests using masks that doesn't make sense (not continuous ones) to check for failure*/

void ipv6_install_flow_mod(void){
	printf("ipv6 test flow mod\n");
	uint128__t value; HI(value) = 0xffffffffffffffff; LO(value) = 0xffffffffffff1234;
	uint128__t mask;  HI(value) = 0xffffffffffffffff; LO(value) = 0xffffffffffff0000;
	
	//Create a simple flow_mod
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	
	
	entry->cookie_mask = 0x1;
	
	//add IPv6 match
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip6_src_match(NULL,NULL,value,mask)) == ROFL_SUCCESS);
	
	//Install
	CU_ASSERT(of1x_add_flow_entry_table(sw->pipeline, 0, entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	
	//Uninstall (specific)	
	CU_ASSERT(of1x_remove_flow_entry_table(sw->pipeline, 0, entry, STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);
	
	
}

void ipv6_install_flow_mod_complete(void){
	printf("ipv6 test flow mod\n");
	uint128__t value128; HI(value128) = 0xffffffffffffffff; LO(value128) = 0xffffffffffff1234;
	uint128__t mask128; HI(mask128) = 0xffffffffffffffff; LO(mask128) = 0xffffffffffff0000;
	uint64_t value64 = 0xffffffffffff1234, mask64 = 0xffffffffffff0000;
	
	//Create a simple flow_mod
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	
	
	entry->cookie_mask = 0x1;
	
	//add IPv6 match
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip6_src_match(NULL,NULL,value128,mask128)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip6_dst_match(NULL,NULL,value128,mask128)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip6_flabel_match(NULL,NULL,value64)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip6_nd_target_match(NULL,NULL,value128)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip6_nd_sll_match(NULL,NULL,value64)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip6_nd_tll_match(NULL,NULL,value64)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip6_exthdr_match(NULL,NULL,value64,mask64)) == ROFL_SUCCESS);
	
	//Install
	CU_ASSERT(of1x_add_flow_entry_table(sw->pipeline, 0, entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	
	//Uninstall (specific)	
	CU_ASSERT(of1x_remove_flow_entry_table(sw->pipeline, 0, entry, STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);
	
	
}

void icmpv6_install_flow_mod_complete(void){
	printf("ipv6 test flow mod\n");
	uint64_t value64 = 0xffffffffffff1234;
	
	//Create a simple flow_mod
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	
	
	entry->cookie_mask = 0x1;
	
	//add IPv6 match
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_icmpv6_type_match(NULL,NULL,value64)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_icmpv6_code_match(NULL,NULL,value64)) == ROFL_SUCCESS);
	
	//Install
	CU_ASSERT(of1x_add_flow_entry_table(sw->pipeline, 0, entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	
	//Uninstall (specific)	
	CU_ASSERT(of1x_remove_flow_entry_table(sw->pipeline, 0, entry, STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);
}

void ipv6_process_packet(void){
	/*Create test to process a packet with ipv6 matches*/
	
	
}
