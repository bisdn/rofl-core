#include "matching_test.h"

static of12_switch_t* sw=NULL;
	
int set_up(){

	physical_switch_init();

	enum of12_matching_algorithm_available ma_list[4]={of12_matching_algorithm_loop, of12_matching_algorithm_loop,
	of12_matching_algorithm_loop, of12_matching_algorithm_loop};

	//Create instance	
	sw = of12_init_switch("Test switch",0x0101,4,ma_list);
	
	if(!sw)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int tear_down(){
	//Destroy the switch
	if(__of12_destroy_switch(sw) != ROFL_SUCCESS)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}

void test_install_empty_flow_mod(){

	//Create a simple flow_mod
	of12_flow_entry_t* entry = of12_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	

	//Install
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, false,false) == ROFL_OF12_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	
	//Uninstall (specific)	
	CU_ASSERT(of12_remove_flow_entry_table(sw->pipeline, 0, entry, STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);
	
}

void test_install_overlapping_specific(){

	unsigned int i, num_of_flows=rand()%20;
	of12_flow_entry_t* entry;

	//Install N flowmods which identical => should put only one
	for(i=0;i<num_of_flows;i++){
		entry = of12_init_flow_entry(NULL, NULL, false); 
		CU_ASSERT(of12_add_match_to_entry(entry,of12_init_port_in_match(NULL,NULL,1)) == ROFL_SUCCESS);
	
		CU_ASSERT(entry != NULL);
		CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, false,false) == ROFL_OF12_FM_SUCCESS);
	}

	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	
	//Uninstall all 
	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_port_in_match(NULL,NULL,1)) == ROFL_SUCCESS);
	rofl_result_t specific_remove_result = of12_remove_flow_entry_table(sw->pipeline, 0, entry, STRICT, OF12_PORT_ANY, OF12_GROUP_ANY);
	CU_ASSERT( specific_remove_result == ROFL_SUCCESS ); //First must succeeed

	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);	
	
	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_port_in_match(NULL,NULL,1)) == ROFL_SUCCESS);
	specific_remove_result = of12_remove_flow_entry_table(sw->pipeline, 0, entry, STRICT, OF12_PORT_ANY, OF12_GROUP_ANY);
	CU_ASSERT( specific_remove_result == ROFL_SUCCESS ); //Second too according to spec (no entries)


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
	
		CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entries[i], false,false) == ROFL_OF12_FM_SUCCESS);
	}

}

static void clean_pipeline(of12_switch_t* sw){
	
	of12_flow_entry_t* deleting_entry = of12_init_flow_entry(NULL, NULL, false); 

	CU_ASSERT(deleting_entry != NULL);

	CU_ASSERT(of12_remove_flow_entry_table(sw->pipeline, 0, deleting_entry, NOT_STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);


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

	CU_ASSERT(of12_remove_flow_entry_table(sw->pipeline, 0, deleting_entry, NOT_STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
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

	CU_ASSERT(of12_remove_flow_entry_table(sw->pipeline, 0, deleting_entry, NOT_STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
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

	CU_ASSERT(of12_remove_flow_entry_table(sw->pipeline, 0, deleting_entry, NOT_STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);


//With no-match(complete wildcard)

	//Reload
	test_uninstall_wildcard_add_flows(entries, num_of_flows);

#if 0 
	//Do the deletion with no matches
	deleting_entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(deleting_entry != NULL);
	
	CU_ASSERT(of12_remove_flow_entry_table(sw->pipeline, 0, deleting_entry, NOT_STRICT, OF12_PORT_ANY, OF12_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 0);
#endif
	clean_pipeline(sw);
}

void test_overlap(){

	of12_flow_entry_t* entry;

/*
*  
* non overlapping matches 
*
*/
	/* 1 match - */
	//Create two entries 1 match same matches != scope and add with overlap=1
	entry = of12_init_flow_entry(NULL, NULL, false); 

	//Add match
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_port_in_match(NULL,NULL,1)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) == ROFL_OF12_FM_SUCCESS);
	
	//Add second entry
	entry = of12_init_flow_entry(NULL, NULL, false); 

	//Add match
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_port_in_match(NULL,NULL,2)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) == ROFL_OF12_FM_SUCCESS);
	
	clean_pipeline(sw);	

	/* 4 match -  */
	//Create two entries with 4 match same matches != scope and add with overlap=1
	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_port_in_match(NULL,NULL,1)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_eth_src_match(NULL,NULL,0x999999999999, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_dst_match(NULL,NULL,0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_src_match(NULL,NULL,0x22222222, 0xfffffff)) == ROFL_SUCCESS);
	
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) == ROFL_OF12_FM_SUCCESS);

	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_port_in_match(NULL,NULL,2)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_eth_src_match(NULL,NULL,0x7777, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_dst_match(NULL,NULL,0x44444444, 0xfffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_src_match(NULL,NULL,0x33333333, 0xfffffff)) == ROFL_SUCCESS);

	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) == ROFL_OF12_FM_SUCCESS);

	clean_pipeline(sw);	

	/* 1 match - different types */
	//Create two entries with 2 match with different types and add with overlap=1
	entry = of12_init_flow_entry(NULL, NULL, false); 

	//Add match
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_port_in_match(NULL,NULL,1)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) == ROFL_OF12_FM_SUCCESS);
	
	//Add second entry
	entry = of12_init_flow_entry(NULL, NULL, false); 

	//Add match
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_eth_src_match(NULL,NULL,0x999999999999, 0xffffffffffff)) == ROFL_SUCCESS);
	
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) == ROFL_OF12_FM_OVERLAP);
	
	clean_pipeline(sw);	

/*
*  
* overlapping matches 
*
*/
	/* 4 match -  */
	//Create two entries with 4 match same matches != scope except some and add with overlap=1
	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_port_in_match(NULL,NULL,1)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_eth_src_match(NULL,NULL,0x999999999999, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_dst_match(NULL,NULL,0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_src_match(NULL,NULL,0x22222233, 0xfffff00)) == ROFL_SUCCESS);
	
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) == ROFL_OF12_FM_SUCCESS);

	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_port_in_match(NULL,NULL,1)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_eth_src_match(NULL,NULL,0x999999999999, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_dst_match(NULL,NULL,0x11111144, 0xfffff00)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_src_match(NULL,NULL,0x22222233, 0xfffff00)) == ROFL_SUCCESS);

	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) != ROFL_OF12_FM_SUCCESS);

	clean_pipeline(sw);	

	/* 2 match -  */
	//Create two entries with 2 match same matches, one with different scope, and one that overlaps add with overlap=1
	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_dst_match(NULL,NULL,0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_src_match(NULL,NULL,0x22222233, 0xffffff0)) == ROFL_SUCCESS);
	
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) == ROFL_OF12_FM_SUCCESS);

	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_dst_match(NULL,NULL,0x11111144, 0xfffff00)) == ROFL_SUCCESS);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_src_match(NULL,NULL,0x22222233, 0xfffff00)) == ROFL_SUCCESS);

	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) != ROFL_OF12_FM_SUCCESS);

	clean_pipeline(sw);	


/* non overlapping matchies */
	
	//Create two entries 1 different matches 
	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_dst_match(NULL,NULL,0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) == ROFL_OF12_FM_SUCCESS);

	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_src_match(NULL,NULL,0x22222233, 0xfffff00)) == ROFL_SUCCESS);

	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) != ROFL_OF12_FM_SUCCESS);


	clean_pipeline(sw);	

	//Create two entries with same matches different scope 
	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_src_match(NULL,NULL,0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) == ROFL_OF12_FM_SUCCESS);

	entry = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of12_add_match_to_entry(entry,of12_init_ip4_src_match(NULL,NULL,0x22222233, 0xfffff00)) == ROFL_SUCCESS);

	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry, true,false) == ROFL_OF12_FM_SUCCESS);


	clean_pipeline(sw);	


}

void test_flow_modify(){

	of12_flow_entry_t* entry1, *entry2;
	of12_action_group_t* group1, *group2;
	wrap_uint_t field;
/*
*  
* Simple modify test with STRICT and NOT-STRICT 
*
*/
	entry1 = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(entry1 != NULL);
	
	//Add one match
	CU_ASSERT(of12_add_match_to_entry(entry1,of12_init_ip4_dst_match(NULL,NULL,0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	
	//Add one action
	group1 = of12_init_action_group(NULL);
	CU_ASSERT(group1 != NULL);
	field.u16 = 1;
	of12_push_packet_action_to_group(group1,of12_init_packet_action(OF12_AT_OUTPUT,field,NULL,NULL));
	
	of12_add_instruction_to_group(&entry1->inst_grp, OF12_IT_APPLY_ACTIONS, group1, NULL,0);
	
	//Insert in the table	
	CU_ASSERT(of12_add_flow_entry_table(sw->pipeline, 0, entry1, true,false) == ROFL_OF12_FM_SUCCESS);

	/*****/

	entry2 = of12_init_flow_entry(NULL, NULL, false); 
	CU_ASSERT(entry2 != NULL);

	//Add one match
	CU_ASSERT(of12_add_match_to_entry(entry2,of12_init_ip4_dst_match(NULL,NULL,0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	
	//Add a different action
	group2 = of12_init_action_group(NULL);
	CU_ASSERT(group2 != NULL);
	of12_push_packet_action_to_group(group2, of12_init_packet_action(OF12_AT_SET_FIELD_IP_DSCP, field, NULL,NULL));
	
	of12_add_instruction_to_group(&entry2->inst_grp, OF12_IT_APPLY_ACTIONS, group2, NULL,0);

	//MODIFY strict
	CU_ASSERT(of12_modify_flow_entry_table(sw->pipeline, 0, entry2, STRICT, true) == ROFL_SUCCESS);

	
	//Check actions are first entry of the table
	CU_ASSERT(sw->pipeline->tables[0].num_of_entries == 1);
	CU_ASSERT(sw->pipeline->tables[0].entries->inst_grp.instructions[OF12_IT_APPLY_ACTIONS-1].apply_actions != NULL);
	CU_ASSERT(sw->pipeline->tables[0].entries->inst_grp.instructions[OF12_IT_APPLY_ACTIONS-1].apply_actions->head->type == OF12_AT_SET_FIELD_IP_DSCP);
//	CU_ASSERT(sw->pipeline->tables[0].entries.type == OF12_IT_NO_INSTRUCTION);

	/*****/

	//Create a new entry
	
	//Reupdate with NO-Strict

}
