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

	//Add a fake port for output
	sw->logical_ports[1].attachment_state = LOGICAL_PORT_STATE_ATTACHED;
	sw->logical_ports[1].port = (switch_port_t*)0x1;
	
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
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
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
	
	wrap_uint_t field;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_action_group_t *apply_actions = of1x_init_action_group(NULL);
	of1x_packet_action_t* action = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	
	CU_ASSERT(entry != NULL);	
	CU_ASSERT(apply_actions != NULL);	
	CU_ASSERT(action != NULL);	

	of1x_push_packet_action_to_group(apply_actions, action);
	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions,
			NULL,
			NULL,
			/*go_to_table*/0);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
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
	CU_ASSERT(drops == 0);	
	CU_ASSERT(outputs == 1);	
	CU_ASSERT(replicas == 0);	

}

//Simple write actions output
void bufs_write_output_action(void){
	
	wrap_uint_t field;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_write_actions_t *write_actions = of1x_init_write_actions();
	of1x_packet_action_t* action = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	
	CU_ASSERT(entry != NULL);	
	CU_ASSERT(write_actions != NULL);	
	CU_ASSERT(action != NULL);	

	of1x_set_packet_action_on_write_actions(write_actions, action);
	of1x_add_instruction_to_group(
				&(entry->inst_grp),
				OF1X_IT_WRITE_ACTIONS,
				NULL,
				(of1x_write_actions_t*)write_actions,
				NULL,
				/*go_to_table*/0);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
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
	CU_ASSERT(drops == 0);	
	CU_ASSERT(outputs == 1);	
	CU_ASSERT(replicas == 0);	

}

//Multiple output in apply actions
void bufs_multiple_apply_output_actions(void){
	wrap_uint_t field;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_action_group_t *apply_actions = of1x_init_action_group(NULL);
	of1x_packet_action_t* action = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	
	CU_ASSERT(entry != NULL);	
	CU_ASSERT(apply_actions != NULL);	
	CU_ASSERT(action != NULL);	
	of1x_push_packet_action_to_group(apply_actions, action);

	action = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	CU_ASSERT(action != NULL);	
	of1x_push_packet_action_to_group(apply_actions, action);
	
	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions,
			NULL,
			NULL,
			/*go_to_table*/0);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
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
	CU_ASSERT(allocated == 3);
	CU_ASSERT(released == 3);	
	CU_ASSERT(drops == 1);
	CU_ASSERT(outputs == 2);	
	CU_ASSERT(replicas == 2);	
}

//An output action in both apply and write actions
void bufs_multiple_apply_write_output_actions(void){

	wrap_uint_t field;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_write_actions_t *write_actions = of1x_init_write_actions();
	of1x_packet_action_t* action = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	
	CU_ASSERT(entry != NULL);	
	CU_ASSERT(write_actions != NULL);	
	CU_ASSERT(action != NULL);	

	of1x_set_packet_action_on_write_actions(write_actions, action);
	of1x_add_instruction_to_group(
				&(entry->inst_grp),
				OF1X_IT_WRITE_ACTIONS,
				NULL,
				(of1x_write_actions_t*)write_actions,
				NULL,
				/*go_to_table*/0);

	of1x_action_group_t *apply_actions = of1x_init_action_group(NULL);
	action = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	
	CU_ASSERT(apply_actions != NULL);	
	CU_ASSERT(action != NULL);	
	of1x_push_packet_action_to_group(apply_actions, action);

	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions,
			NULL,
			NULL,
			/*go_to_table*/0);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
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
	CU_ASSERT(allocated == 3);
	CU_ASSERT(released == 3);	
	CU_ASSERT(drops == 1);	
	CU_ASSERT(outputs == 2);	
	CU_ASSERT(replicas == 2);
}

//An output action in a group (apply actions)
void bufs_single_output_action_group_apply(void){
	wrap_uint_t field, field_grp;
	unsigned int grp_id = 10; 
	field_grp.u32 = grp_id;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	
	of1x_action_group_t* ag=of1x_init_action_group(NULL);
	of1x_bucket_list_t* buckets=of1x_init_bucket_list();
	of1x_push_packet_action_to_group(ag, of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	of1x_insert_bucket_in_list(buckets,of1x_init_bucket(0,1,0,ag));
	of1x_group_add(sw->pipeline.groups,OF1X_GROUP_TYPE_INDIRECT,grp_id,&buckets);
	

	CU_ASSERT(entry != NULL);	
	CU_ASSERT(buckets == NULL);	

	of1x_action_group_t *apply_actions = of1x_init_action_group(NULL);
	CU_ASSERT(apply_actions != NULL);	
	of1x_push_packet_action_to_group(apply_actions,of1x_init_packet_action(OF1X_AT_GROUP,field_grp,NULL,NULL));
	
	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions,
			NULL,
			NULL,
			/*go_to_table*/0);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
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
	CU_ASSERT(allocated == 2);
	CU_ASSERT(released == 2);	
	CU_ASSERT(drops == 1);	
	CU_ASSERT(outputs == 1);	
	CU_ASSERT(replicas == 1);
}

//An output action in a group (write actions)
void bufs_single_output_action_group_write(void){
	wrap_uint_t field, field_grp;
	unsigned int grp_id = 10; 
	field_grp.u32 = grp_id;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	
	of1x_action_group_t* ag=of1x_init_action_group(NULL);
	of1x_bucket_list_t* buckets=of1x_init_bucket_list();
	of1x_push_packet_action_to_group(ag, of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	of1x_insert_bucket_in_list(buckets,of1x_init_bucket(0,1,0,ag));
	CU_ASSERT(of1x_group_add(sw->pipeline.groups,OF1X_GROUP_TYPE_INDIRECT,grp_id,&buckets) == ROFL_OF1X_GM_EXISTS); 
	

	CU_ASSERT(entry != NULL);	
	CU_ASSERT(buckets != NULL);	

	of1x_write_actions_t *write_actions = of1x_init_write_actions();
	of1x_packet_action_t* action = of1x_init_packet_action(OF1X_AT_GROUP,field_grp,NULL,NULL);
	
	CU_ASSERT(entry != NULL);	
	CU_ASSERT(write_actions != NULL);	
	CU_ASSERT(action != NULL);	

	of1x_set_packet_action_on_write_actions(write_actions, action);
	of1x_add_instruction_to_group(
				&(entry->inst_grp),
				OF1X_IT_WRITE_ACTIONS,
				NULL,
				(of1x_write_actions_t*)write_actions,
				NULL,
				/*go_to_table*/0);

	

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
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
	CU_ASSERT(allocated == 2);
	CU_ASSERT(released == 2);	
	CU_ASSERT(drops == 1);	
	CU_ASSERT(outputs == 1);	
	CU_ASSERT(replicas == 1);
}
//An output action in a group and in an apply actions
void bufs_apply_and_group_output_actions(void){
	wrap_uint_t field, field_grp;
	unsigned int grp_id = 10; 
	field_grp.u32 = grp_id;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	
	of1x_action_group_t* ag=of1x_init_action_group(NULL);
	of1x_bucket_list_t* buckets=of1x_init_bucket_list();
	of1x_push_packet_action_to_group(ag, of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	of1x_insert_bucket_in_list(buckets,of1x_init_bucket(0,1,0,ag));
	of1x_group_add(sw->pipeline.groups,OF1X_GROUP_TYPE_INDIRECT,grp_id,&buckets);
	

	CU_ASSERT(entry != NULL);	
	CU_ASSERT(buckets != NULL);	

	of1x_action_group_t *apply_actions = of1x_init_action_group(NULL);
	CU_ASSERT(apply_actions != NULL);	
	of1x_push_packet_action_to_group(apply_actions,of1x_init_packet_action(OF1X_AT_GROUP,field_grp,NULL,NULL));
	of1x_push_packet_action_to_group(apply_actions,of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	
	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions,
			NULL,
			NULL,
			/*go_to_table*/0);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
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
	CU_ASSERT(allocated == 3);
	CU_ASSERT(released == 3);	
	CU_ASSERT(drops == 1);	
	CU_ASSERT(outputs == 2);	
	CU_ASSERT(replicas == 2);
}

//An output action in a group and in write actions
void bufs_write_and_group_output_actions(void){
	wrap_uint_t field, field_grp;
	unsigned int grp_id = 10; 
	field_grp.u32 = grp_id;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	
	of1x_action_group_t* ag=of1x_init_action_group(NULL);
	of1x_bucket_list_t* buckets=of1x_init_bucket_list();
	of1x_push_packet_action_to_group(ag, of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	of1x_insert_bucket_in_list(buckets,of1x_init_bucket(0,1,0,ag));
	CU_ASSERT(of1x_group_add(sw->pipeline.groups,OF1X_GROUP_TYPE_INDIRECT,grp_id,&buckets) == ROFL_OF1X_GM_EXISTS); //Group should exist
	

	CU_ASSERT(entry != NULL);	
	CU_ASSERT(buckets != NULL);	

	of1x_write_actions_t *write_actions = of1x_init_write_actions();
	of1x_packet_action_t* action = of1x_init_packet_action(OF1X_AT_GROUP,field_grp,NULL,NULL);
	
	CU_ASSERT(entry != NULL);	
	CU_ASSERT(write_actions != NULL);	
	CU_ASSERT(action != NULL);	
	of1x_set_packet_action_on_write_actions(write_actions, action);
	
	action = of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL);
	CU_ASSERT(action != NULL);	
	of1x_set_packet_action_on_write_actions(write_actions, action);

	of1x_add_instruction_to_group(
				&(entry->inst_grp),
				OF1X_IT_WRITE_ACTIONS,
				NULL,
				(of1x_write_actions_t*)write_actions,
				NULL,
				/*go_to_table*/0);

	


	
	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
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
	CU_ASSERT(allocated == 3);
	CU_ASSERT(released == 3);	
	CU_ASSERT(drops == 1);	
	CU_ASSERT(outputs == 2);	
	CU_ASSERT(replicas == 2);
}

//Multiple output actions in a bucket list (group type all)
void bufs_multiple_output_actions_group(void){

	wrap_uint_t field, field_grp;
	unsigned int grp_id = 11; 
	field_grp.u32 = grp_id;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	
	of1x_action_group_t* ag=of1x_init_action_group(NULL);
	of1x_bucket_list_t* buckets=of1x_init_bucket_list();
	of1x_push_packet_action_to_group(ag, of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	of1x_push_packet_action_to_group(ag, of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	of1x_insert_bucket_in_list(buckets,of1x_init_bucket(0,1,0,ag));
	of1x_group_add(sw->pipeline.groups,OF1X_GROUP_TYPE_ALL,grp_id,&buckets);
	

	CU_ASSERT(entry != NULL);	
	CU_ASSERT(buckets == NULL);	

	of1x_action_group_t *apply_actions = of1x_init_action_group(NULL);
	CU_ASSERT(apply_actions != NULL);	
	of1x_push_packet_action_to_group(apply_actions,of1x_init_packet_action(OF1X_AT_GROUP,field_grp,NULL,NULL));
	
	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions,
			NULL,
			NULL,
			/*go_to_table*/0);

	of1x_dump_flow_entry(entry);
	
	//Dump pipeline
	of1x_full_dump_switch(sw);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	
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
	CU_ASSERT(allocated == 4);
	CU_ASSERT(released == 4);
	CU_ASSERT(drops == 2);	
	CU_ASSERT(outputs == 2);	
	CU_ASSERT(replicas == 3);
}

//No action output in the first table, no action on the second (drop)
void bufs_no_output_action_goto(void){
	
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_flow_entry_t* entry2 = of1x_init_flow_entry(NULL, NULL, false); 
	
	CU_ASSERT(entry != NULL);	

	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_GOTO_TABLE,
			NULL,	
			NULL,
			NULL,
			/*go_to_table*/1);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 1, &entry2, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	CU_ASSERT(sw->pipeline.tables[1].num_of_entries == 1);
	
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

//No action output in the first table, apply output action in the second
void bufs_apply_output_action_last_table_goto(void){
	
	wrap_uint_t field;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_flow_entry_t* entry2 = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_action_group_t *apply_actions = of1x_init_action_group(NULL);
	of1x_packet_action_t* action = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	
	CU_ASSERT(entry != NULL);	
	CU_ASSERT(entry2 != NULL);	
	CU_ASSERT(apply_actions != NULL);	
	CU_ASSERT(action != NULL);	

	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_GOTO_TABLE,
			NULL,	
			NULL,
			NULL,
			/*go_to_table*/1);

	of1x_push_packet_action_to_group(apply_actions, action);
	of1x_add_instruction_to_group(
			&(entry2->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions,
			NULL,
			NULL,
			/*go_to_table*/0);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 1, &entry2, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	CU_ASSERT(sw->pipeline.tables[1].num_of_entries == 1);
	
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
	CU_ASSERT(drops == 0);	
	CU_ASSERT(outputs == 1);	
	CU_ASSERT(replicas == 0);	

}

//Action output in the first table, write output action in the second
void bufs_apply_output_action_both_tables_goto(void){
	
	wrap_uint_t field;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_flow_entry_t* entry2 = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_action_group_t *apply_actions = of1x_init_action_group(NULL);
	of1x_action_group_t *apply_actions2 = of1x_init_action_group(NULL);
	of1x_packet_action_t* action = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	of1x_packet_action_t* action2 = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	
	CU_ASSERT(entry != NULL);	
	CU_ASSERT(entry2 != NULL);	
	CU_ASSERT(apply_actions != NULL);	
	CU_ASSERT(action != NULL);	

	of1x_push_packet_action_to_group(apply_actions, action);
	of1x_push_packet_action_to_group(apply_actions2, action2);

	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_GOTO_TABLE,
			NULL,	
			NULL,
			NULL,
			/*go_to_table*/1);

	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions,
			NULL,
			NULL,
			/*go_to_table*/0);


	of1x_add_instruction_to_group(
			&(entry2->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions2,
			NULL,
			NULL,
			/*go_to_table*/0);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 1, &entry2, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	CU_ASSERT(sw->pipeline.tables[1].num_of_entries == 1);
	
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
	CU_ASSERT(allocated == 2);
	CU_ASSERT(released == 2);	
	CU_ASSERT(drops == 0);	
	CU_ASSERT(outputs == 2);	
	CU_ASSERT(replicas == 1);	

}

//two outputs in first table, one in the second
void bufs_apply_output_action_both_tables_bis_goto(void){
	
	wrap_uint_t field;
	field.u32 = 1;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_flow_entry_t* entry2 = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_action_group_t *apply_actions = of1x_init_action_group(NULL);
	of1x_action_group_t *apply_actions2 = of1x_init_action_group(NULL);
	of1x_packet_action_t* action = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	of1x_packet_action_t* action2 = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	
	CU_ASSERT(entry != NULL);	
	CU_ASSERT(entry2 != NULL);	
	CU_ASSERT(apply_actions != NULL);	
	CU_ASSERT(action != NULL);	

	of1x_push_packet_action_to_group(apply_actions, action);
	action = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
	of1x_push_packet_action_to_group(apply_actions, action);
	of1x_push_packet_action_to_group(apply_actions2, action2);

	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_GOTO_TABLE,
			NULL,	
			NULL,
			NULL,
			/*go_to_table*/1);

	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions,
			NULL,
			NULL,
			/*go_to_table*/0);


	of1x_add_instruction_to_group(
			&(entry2->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions2,
			NULL,
			NULL,
			/*go_to_table*/0);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 1, &entry2, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	CU_ASSERT(sw->pipeline.tables[1].num_of_entries == 1);
	
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
	CU_ASSERT(allocated == 3);
	CU_ASSERT(released == 3);	
	CU_ASSERT(drops == 0);	
	CU_ASSERT(outputs == 3);	
	CU_ASSERT(replicas == 2);	

}
//No action output in the first table, group output action in the second
void bufs_output_first_table_output_on_group_second_table(void){
	
	wrap_uint_t field, field_grp;
	field.u32 = 1;
	unsigned int grp_id = 12; 
	field_grp.u32 = grp_id;
	reset_io_state();
	
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_flow_entry_t* entry2 = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_action_group_t *apply_actions2 = of1x_init_action_group(NULL);
	of1x_packet_action_t* action2 = of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL);
		
	of1x_action_group_t* ag=of1x_init_action_group(NULL);
	of1x_bucket_list_t* buckets=of1x_init_bucket_list();
	of1x_push_packet_action_to_group(ag, of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	of1x_insert_bucket_in_list(buckets,of1x_init_bucket(0,1,0,ag));
	of1x_group_add(sw->pipeline.groups,OF1X_GROUP_TYPE_INDIRECT,grp_id,&buckets);
	

	CU_ASSERT(entry != NULL);	
	CU_ASSERT(buckets == NULL);	

	of1x_action_group_t *apply_actions = of1x_init_action_group(NULL);
	CU_ASSERT(apply_actions != NULL);	
	of1x_push_packet_action_to_group(apply_actions,of1x_init_packet_action(OF1X_AT_GROUP,field_grp,NULL,NULL));
	
	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions,
			NULL,
			NULL,
			/*go_to_table*/0);


	CU_ASSERT(entry != NULL);	
	CU_ASSERT(entry2 != NULL);	
	CU_ASSERT(apply_actions != NULL);	

	of1x_push_packet_action_to_group(apply_actions2, action2);

	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_GOTO_TABLE,
			NULL,	
			NULL,
			NULL,
			/*go_to_table*/1);

	of1x_add_instruction_to_group(
			&(entry2->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions2,
			NULL,
			NULL,
			/*go_to_table*/0);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 1, &entry2, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	CU_ASSERT(sw->pipeline.tables[1].num_of_entries == 1);
	
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
	CU_ASSERT(allocated == 2);
	CU_ASSERT(released == 2);	
	CU_ASSERT(drops == 0);	
	CU_ASSERT(outputs == 2);	
	CU_ASSERT(replicas == 1);	

}
//Output on apply and group on first table, output on apply, group and write actions(output and group again) on the second table (write set on the first table)
void bufs_output_all(void){
	
	wrap_uint_t field, field_grp;
	field.u32 = 1;
	unsigned int grp_id = 13; 
	field_grp.u32 = grp_id;
	reset_io_state();

	//Group	
	of1x_action_group_t* ag=of1x_init_action_group(NULL);
	of1x_bucket_list_t* buckets=of1x_init_bucket_list();
	of1x_push_packet_action_to_group(ag, of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	of1x_push_packet_action_to_group(ag, of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	of1x_insert_bucket_in_list(buckets,of1x_init_bucket(0,1,0,ag));
	of1x_group_add(sw->pipeline.groups,OF1X_GROUP_TYPE_ALL,grp_id,&buckets);
	
	//
	//entry 1
	//
	
	//Apply
	of1x_flow_entry_t* entry = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_action_group_t* apply_actions = of1x_init_action_group(NULL);
	of1x_push_packet_action_to_group(apply_actions,of1x_init_packet_action(OF1X_AT_GROUP,field_grp,NULL,NULL));
	of1x_push_packet_action_to_group(apply_actions,of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions,
			NULL,
			NULL,
			/*go_to_table*/0);

	//Write
	of1x_write_actions_t *write_actions = of1x_init_write_actions();
	of1x_set_packet_action_on_write_actions(write_actions, of1x_init_packet_action(OF1X_AT_GROUP,field_grp,NULL,NULL));
	of1x_set_packet_action_on_write_actions(write_actions, of1x_init_packet_action(OF1X_AT_OUTPUT,field,NULL,NULL));
	of1x_add_instruction_to_group(
				&(entry->inst_grp),
				OF1X_IT_WRITE_ACTIONS,
				NULL,
				(of1x_write_actions_t*)write_actions,
				NULL,
				/*go_to_table*/0);


	//Goto
	of1x_add_instruction_to_group(
			&(entry->inst_grp),
			OF1X_IT_GOTO_TABLE,
			NULL,	
			NULL,
			NULL,
			/*go_to_table*/1);
	//
	//entry 2
	//
	of1x_flow_entry_t* entry2 = of1x_init_flow_entry(NULL, NULL, false); 
	of1x_action_group_t *apply_actions2 = of1x_init_action_group(NULL);
	of1x_push_packet_action_to_group(apply_actions2, of1x_init_packet_action( OF1X_AT_OUTPUT, field, NULL, NULL));
	of1x_push_packet_action_to_group(apply_actions2,of1x_init_packet_action(OF1X_AT_GROUP,field_grp,NULL,NULL));

	


	of1x_add_instruction_to_group(
			&(entry2->inst_grp),
			OF1X_IT_APPLY_ACTIONS,
			(of1x_action_group_t*)apply_actions2,
			NULL,
			NULL,
			/*go_to_table*/0);

	//Install
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 0, &entry, false,false) == ROFL_OF1X_FM_SUCCESS);
	CU_ASSERT(of1x_add_flow_entry_table(&sw->pipeline, 1, &entry2, false,false) == ROFL_OF1X_FM_SUCCESS);
	
	//Check real size of the table
	CU_ASSERT(sw->pipeline.tables[0].num_of_entries == 1);
	CU_ASSERT(sw->pipeline.tables[1].num_of_entries == 1);
	
	//Process packet through pipeline
	pkt = allocate_buffer();	
	
	CU_ASSERT(pkt != NULL);	
	
	if(!pkt)
		return;

	//Dump pipeline
	of1x_full_dump_switch(sw);

	//Process through pipeline. Packet should be dropped
	of_process_packet_pipeline((of_switch_t*)sw,pkt);

	CU_ASSERT(outputs == 9);
	//Checkings	
	CU_ASSERT(allocated == 13);
	CU_ASSERT(released == 13);	
	CU_ASSERT(drops == 4);	
	CU_ASSERT((drops+outputs) == released);	
	CU_ASSERT(replicas == 12)
}


