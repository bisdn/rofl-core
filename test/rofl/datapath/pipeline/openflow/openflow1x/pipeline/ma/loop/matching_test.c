#include "matching_test.h"

static of1x_switch_t* sw=NULL;
	
int set_up(){

	physical_switch_init();

	enum of1x_matching_algorithm_available ma_list[4]={of1x_loop_matching_algorithm, of1x_loop_matching_algorithm,
	of1x_loop_matching_algorithm, of1x_loop_matching_algorithm};

	//Create instance	
	sw = of1x_init_switch("Test switch", OF_VERSION_12, 0x0101,4,ma_list);
	
	if(!sw)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int tear_down(){
	//Destroy the switch
	if(__of1x_destroy_switch(sw) != ROFL_SUCCESS)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}

void test_install_empty_flow_mod(){

	//Create a simple flow_mod
	of1x_flow_entry_t* entry = of1x_init_flow_entry(false); 
	
	CU_ASSERT(entry != NULL);	

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	CU_ASSERT(entry == NULL);


	//New entry	
	entry = of1x_init_flow_entry(false); 
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
	//Uninstall (specific)	
	CU_ASSERT(of1x_remove_flow_entry_table(&sw->pipeline, 0, entry, STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 0);
	
}

void test_install_overlapping_specific(){

	unsigned int i, num_of_flows=rand()%20;
	of1x_flow_entry_t* entry;

	//Install N flowmods which identical => should put only one
	for(i=0;i<num_of_flows;i++){
		entry = of1x_init_flow_entry(false); 
		CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(1)) == ROFL_SUCCESS);
	
		CU_ASSERT(entry != NULL);
		CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	}

	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
	//Uninstall all 
	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(1)) == ROFL_SUCCESS);
	rofl_result_t specific_remove_result = of1x_remove_flow_entry_table(&sw->pipeline, 0, entry, STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY);
	CU_ASSERT( specific_remove_result == ROFL_SUCCESS ); //First must succeeed

	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 0);	
	
	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(1)) == ROFL_SUCCESS);
	specific_remove_result = of1x_remove_flow_entry_table(&sw->pipeline, 0, entry, STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY);
	CU_ASSERT( specific_remove_result == ROFL_SUCCESS ); //Second too according to spec (no entries)


	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 0);	
}


static void test_uninstall_wildcard_add_flows(of1x_flow_entry_t** entries, unsigned int num_of_flows){
	unsigned int i;

	//Install N flowmods with one identical match and the rest randomly generated
	for(i=0;i<num_of_flows;i++){
		entries[i] = of1x_init_flow_entry(false); 
	
		//Add two match common
		CU_ASSERT(of1x_add_match_to_entry(entries[i],of1x_init_port_in_match(1)) == ROFL_SUCCESS);
		CU_ASSERT(of1x_add_match_to_entry(entries[i],of1x_init_eth_src_match(0x012345678901, 0xFFFFFFFFFFFF)) == ROFL_SUCCESS);

		//Add random matches 
		CU_ASSERT(of1x_add_match_to_entry(entries[i],of1x_init_eth_dst_match(rand()%0xFFFFFFFF, 0xFFFFFFFFFFFF)) == ROFL_SUCCESS);

		if(rand()%100 > 50)
			CU_ASSERT(of1x_add_match_to_entry(entries[i],of1x_init_ip4_dst_match(rand()%0xFFFFFFFF, 0xFFFFFFF)) == ROFL_SUCCESS);

		if(rand()%100 > 80)
			CU_ASSERT(of1x_add_match_to_entry(entries[i],of1x_init_ip4_src_match(rand()%0xFFFFFFFF, 0xFFFFFFF)) == ROFL_SUCCESS);
	
		CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entries[i], false,false) == ROFL_OF1X_FM_SUCCESS);
	}

}

static void clean_pipeline(of1x_switch_t* sw){
	
	of1x_flow_entry_t* deleting_entry = of1x_init_flow_entry(false); 

	CU_ASSERT(deleting_entry != NULL);

	CU_ASSERT(of1x_remove_flow_entry_table(&sw->pipeline, 0, deleting_entry, NOT_STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 0);


}

void test_uninstall_wildcard(){

	unsigned int num_of_flows=rand()%20;
	of1x_flow_entry_t* entries[20], *deleting_entry;


//With PORT_IN (first match)
	//Add flows	
	test_uninstall_wildcard_add_flows(entries, num_of_flows);

	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == num_of_flows);

	//Do the deletion with the common match
	deleting_entry = of1x_init_flow_entry(false); 
	CU_ASSERT(deleting_entry != NULL);
	CU_ASSERT(of1x_add_match_to_entry(deleting_entry,of1x_init_port_in_match(1)) == ROFL_SUCCESS);

	CU_ASSERT(of1x_remove_flow_entry_table(&sw->pipeline, 0, deleting_entry, NOT_STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 0);

//With ETH_SRC only (second match)	
	//Reload
	test_uninstall_wildcard_add_flows(entries, num_of_flows);

	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == num_of_flows);

	//Do the deletion with the common match
	deleting_entry = of1x_init_flow_entry(false); 
	CU_ASSERT(deleting_entry != NULL);
	CU_ASSERT(of1x_add_match_to_entry(deleting_entry,of1x_init_eth_src_match(0x012345678901, 0xFFFFFFFFFFFF)) == ROFL_SUCCESS);

	CU_ASSERT(of1x_remove_flow_entry_table(&sw->pipeline, 0, deleting_entry, NOT_STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 0);

//With ALL with masks to 0 (the ones possible)
	//Reload
	test_uninstall_wildcard_add_flows(entries, num_of_flows);

	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == num_of_flows);

	//Do the deletion with the common match
	deleting_entry = of1x_init_flow_entry(false); 
	CU_ASSERT(deleting_entry != NULL);
	CU_ASSERT(of1x_add_match_to_entry(deleting_entry,of1x_init_port_in_match(1)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(deleting_entry,of1x_init_eth_src_match(0x999999999999, 0x000000000000)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(deleting_entry,of1x_init_ip4_dst_match(rand()%0x11111111, 0x0000000)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(deleting_entry,of1x_init_ip4_dst_match(rand()%0x22222222, 0x0000000)) == ROFL_SUCCESS);

	CU_ASSERT(of1x_remove_flow_entry_table(&sw->pipeline, 0, deleting_entry, NOT_STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 0);


//With no-match(complete wildcard)

	//Reload
	test_uninstall_wildcard_add_flows(entries, num_of_flows);

#if 0 
	//Do the deletion with no matches
	deleting_entry = of1x_init_flow_entry(false); 
	CU_ASSERT(deleting_entry != NULL);
	
	CU_ASSERT(of1x_remove_flow_entry_table(&sw->pipeline, 0, deleting_entry, NOT_STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY) == ROFL_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 0);
#endif
	clean_pipeline(sw);
}

void test_overlap(){

	of1x_flow_entry_t* entry;

/*
*  
* non overlapping matches 
*
*/
	/* 1 match - */
	//Create two entries 1 match same matches != scope and add with overlap=1
	entry = of1x_init_flow_entry(false); 

	//Add match
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(1)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Add second entry
	entry = of1x_init_flow_entry(false); 

	//Add match
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(2)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_SUCCESS);
	
	clean_pipeline(sw);	

	/* 4 match -  */
	//Create two entries with 4 match same matches != scope and add with overlap=1
	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(1)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_eth_src_match(0x999999999999, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_dst_match(0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_src_match(0x22222222, 0xfffffff)) == ROFL_SUCCESS);
	
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_SUCCESS);

	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(2)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_eth_src_match(0x7777, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_dst_match(0x44444444, 0xfffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_src_match(0x33333333, 0xfffffff)) == ROFL_SUCCESS);

	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_SUCCESS);

	clean_pipeline(sw);	

	/* 1 match - different types */
	//Create two entries with 2 match with different types and add with overlap=1
	entry = of1x_init_flow_entry(false); 

	//Add match
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(1)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Add second entry
	entry = of1x_init_flow_entry(false); 

	//Add match
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_eth_src_match(0x999999999999, 0xffffffffffff)) == ROFL_SUCCESS);
	
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_OVERLAP_FAILURE);
	
	clean_pipeline(sw);	

/*
*  
* overlapping matches 
*
*/
	/* 4 match -  */
	//Create two entries with 4 match same matches != scope except some and add with overlap=1
	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(1)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_eth_src_match(0x999999999999, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_dst_match(0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_src_match(0x22222233, 0xfffff00)) == ROFL_SUCCESS);
	
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_SUCCESS);

	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(1)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_eth_src_match(0x999999999999, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_dst_match(0x11111144, 0xfffff00)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_src_match(0x22222233, 0xfffff00)) == ROFL_SUCCESS);

	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) != ROFL_OF1X_FM_SUCCESS);

	clean_pipeline(sw);	

	/* 2 match -  */
	//Create two entries with 2 match same matches, one with different scope, and one that overlaps add with overlap=1
	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_dst_match(0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_src_match(0x22222233, 0xffffff0)) == ROFL_SUCCESS);
	
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_SUCCESS);

	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_dst_match(0x11111144, 0xfffff00)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_src_match(0x22222233, 0xfffff00)) == ROFL_SUCCESS);

	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) != ROFL_OF1X_FM_SUCCESS);

	clean_pipeline(sw);	


/* non overlapping matchies */
	
	//Create two entries 1 different matches 
	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_dst_match(0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_SUCCESS);

	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_src_match(0x22222233, 0xfffff00)) == ROFL_SUCCESS);

	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) != ROFL_OF1X_FM_SUCCESS);


	clean_pipeline(sw);	

	//Create two entries with same matches different scope 
	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_src_match(0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_SUCCESS);

	entry = of1x_init_flow_entry(false); 
	CU_ASSERT(entry != NULL);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip4_src_match(0x22222233, 0xfffff00)) == ROFL_SUCCESS);

	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_SUCCESS);


	clean_pipeline(sw);	


}

void test_overlap2(){

	//Create instance	
	enum of1x_matching_algorithm_available ma_list2[1]={of1x_loop_matching_algorithm};
	of1x_switch_t* sw10 = of1x_init_switch("Test switch2", OF_VERSION_10, 0x0102,1,ma_list2);
	
	of1x_flow_entry_t* entry;

	/*
	*  
	* Regression test (matches 1.0 bug) 
	*
	*/

	//Entry	
	entry = of1x_init_flow_entry(false); 
	entry->priority = 0xEFF5;
	entry->cookie = 0x1;

	//Add match
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(2)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_eth_dst_match(0x2aea33b376fa, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_eth_src_match(0x4012534e57aa, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_vlan_vid_match(1620, 0xffff, true)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_vlan_pcp_match(7)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_flow_entry_table(&sw10->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Add second entry
	entry = of1x_init_flow_entry(false); 
	entry->priority = 0xEFF5;
	entry->cookie = 0x2;

	//Add match
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_port_in_match(2)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_eth_dst_match(0x2aea33b376fa, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_eth_src_match(0x4012534e57aa, 0xffffffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_eth_type_match(0x800)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_vlan_vid_match(1620, 0xffff, true)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_vlan_pcp_match(7)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_ip_dscp_match(0xf)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_nw_src_match(0x991ff310,0xfffffffe)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_nw_dst_match(0x91920029,0xffffffff)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_tp_src_match(205)) == ROFL_SUCCESS);
	CU_ASSERT(of1x_add_match_to_entry(entry,of1x_init_tp_dst_match(88)) == ROFL_SUCCESS);

	CU_ASSERT(of1x_add_flow_entry_table(&sw10->pipeline, 0, &entry, true,false) == ROFL_OF1X_FM_OVERLAP_FAILURE); //Check overlap == 1 => MUST FAIL
	
	of_destroy_switch((of_switch_t*)sw10);
}

void test_flow_modify(){

	of1x_flow_entry_t* entry1, *entry2;
	of1x_action_group_t* group1, *group2;
	wrap_uint_t field;
/*
*  
* Simple modify test with STRICT and NOT-STRICT 
*
*/
	entry1 = of1x_init_flow_entry(false); 
	CU_ASSERT(entry1 != NULL);
	
	//Add one match
	CU_ASSERT(of1x_add_match_to_entry(entry1,of1x_init_ip4_dst_match(0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	
	//Add one action
	group1 = of1x_init_action_group(NULL);
	CU_ASSERT(group1 != NULL);
	field.u16 = 1;
	of1x_push_packet_action_to_group(group1,of1x_init_packet_action(OF1X_AT_OUTPUT,field,0x0));
	
	of1x_add_instruction_to_group(&entry1->inst_grp, OF1X_IT_APPLY_ACTIONS, group1, NULL, NULL, 0);
	
	//Insert in the table	
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry1, true,false) == ROFL_OF1X_FM_SUCCESS);

	/*****/

	entry2 = of1x_init_flow_entry(false); 
	CU_ASSERT(entry2 != NULL);

	//Add one match
	CU_ASSERT(of1x_add_match_to_entry(entry2,of1x_init_ip4_dst_match(0x11111111, 0xfffffff)) == ROFL_SUCCESS);
	
	//Add a different action
	group2 = of1x_init_action_group(NULL);
	CU_ASSERT(group2 != NULL);
	of1x_push_packet_action_to_group(group2, of1x_init_packet_action(OF1X_AT_SET_FIELD_IP_DSCP, field, 0x0));
	
	of1x_add_instruction_to_group(&entry2->inst_grp, OF1X_IT_APPLY_ACTIONS, group2, NULL, NULL, 0);

	//MODIFY strict
	CU_ASSERT(of1x_modify_flow_entry_table(&sw->pipeline, 0, &entry2, STRICT, true) == ROFL_OF1X_FM_SUCCESS);

	
	//Check actions are first entry of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	CU_ASSERT(sw->pipeline.tables[0].entries->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions != NULL);
	CU_ASSERT(sw->pipeline.tables[0].entries->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions->head->type == OF1X_AT_SET_FIELD_IP_DSCP);
//	CU_ASSERT(sw->pipeline.tables[0].entries.type == OF1X_IT_NO_INSTRUCTION);

	/*****/

	//Create a new entry
	
	//Reupdate with NO-Strict

}
