#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include "group_table.h"
#define NUM_THREADS 10

switch_port_t flood_meta_port;
struct test_utils tu;
typedef struct info_th{
		int id;
	}info_th_t;

	
int gt_set_up(void){
	tu.gt = of1x_init_group_table();
	if( tu.gt == NULL){
		fprintf(stderr,"<%s:%d>Error\n",__func__,__LINE__);
		return ROFL_FAILURE;
	}
	
	tu.type = OF1XPGT_ALL;
	tu.id = 12;
	
	//FIXME WARNING this test got old by no use
	//NEED TO CREATE A BUCKET LIST -> tu.bu_list = malloc();
	
	tu.weight = 1;
	tu.group = 1;
	tu.port = 1;
	tu.actions = NULL;
	return ROFL_SUCCESS;
}

int gt_tear_down(){
	//NEED TO DESTROY THE BUCKET LIST -> free(tu.bu_list)
	of1x_destroy_group_table(tu.gt);
	return ROFL_SUCCESS;
}

void gt_basic_test(){
	
	assert (of1x_group_add(tu.gt, tu.type, tu.id, tu.bu_list)==ROFL_SUCCESS);
	
	assert(tu.gt->num_of_entries==1);
	
	tu.type = OF1XPGT_SELECT;
	
	assert(of1x_group_modify(tu.gt,tu.type,tu.id,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	
	assert(tu.gt->num_of_entries==1);
	
	assert(of1x_group_delete(tu.gt,tu.id)==ROFL_SUCCESS);
	
	assert(tu.gt->num_of_entries==0);
	
	fprintf(stderr,"<%s:%d>Test passed\n",__func__,__LINE__);
}

void gt_expected_errors_test(){
	
	assert(of1x_group_delete(tu.gt,tu.id)==ROFL_SUCCESS); // no notification when trying to delete a non existing entry
	
	assert(of1x_group_add(tu.gt,tu.type,tu.id,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	assert(of1x_group_add(tu.gt,tu.type,tu.id+1,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	assert(tu.gt->num_of_entries == 2);
	assert(of1x_group_add(tu.gt,tu.type,tu.id+1,tu.weight, tu.group, tu.port, tu.actions)==ROFL_FAILURE);
	assert(tu.gt->num_of_entries == 2);
	
	assert(of1x_group_add(tu.gt,tu.type,tu.id+2,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	assert(of1x_group_delete(tu.gt,tu.id+1)==ROFL_SUCCESS);
	assert(tu.gt->num_of_entries == 2);
	
	assert(of1x_group_modify(tu.gt,tu.type,tu.id+1,tu.weight, tu.group, tu.port, tu.actions)==ROFL_FAILURE);
	assert(of1x_group_modify(tu.gt,tu.type+2,tu.id+2,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	assert(tu.gt->num_of_entries==2);
	assert(tu.gt->head->type==tu.type);
	assert(tu.gt->tail->type==tu.type+2);
	
}

void gt_add_and_delete_buckets_test(){
	
	of1x_packet_action_t* pkt_action;
	of1x_switch_t sw;
	enum matching_algorithm_available list=matching_algorithm_loop;
	sw.pipeline = of1x_init_pipeline(1,&list,OF1X_TABLE_MISS_DROP);
	
	tu.actions = of1x_init_action_group(NULL);
	
	pkt_action = of1x_init_packet_action(&sw,OF1X_AT_COPY_TTL_IN,1,0,NULL,NULL);
	of1x_push_packet_action_to_group(tu.actions,pkt_action);
	pkt_action = of1x_init_packet_action(&sw,OF1X_AT_GROUP,1,0,NULL,NULL);
	of1x_push_packet_action_to_group(tu.actions,pkt_action);
	
	of1x_group_add(tu.gt,tu.type,tu.id, tu.weight, tu.group, tu.port, tu.actions);
	of1x_group_delete(tu.gt,tu.id);
	
	of1x_destroy_action_group(tu.actions);
	of1x_destroy_pipeline(sw.pipeline);
}

void gt_concurrency_routine(void * param){
	info_th_t* info = (info_th_t *)param;
	int i=0;

	(void)info;

	fprintf (stderr,"Hallo\n");
	
	assert (of1x_group_add(tu.gt, info->id, info->id, tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	
	for (i=0;i<500;i++){
		assert(of1x_group_modify(tu.gt,info->id,info->id,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
		usleep(100);
	}
	
	assert(of1x_group_delete(tu.gt,info->id)==ROFL_SUCCESS);
}

void gt_concurrency_test(void){
	/*create some threads and thy crating ande deleting */
	
	pthread_t thread[NUM_THREADS];
	info_th_t info[NUM_THREADS];
	int i;
	
	for (i=0; i<NUM_THREADS; i++){
		info[i].id=i;
		pthread_create(&thread[i],NULL,(void *)&concurrency_routine,&info[i]);
	}
	
	for (i=0; i<NUM_THREADS; i++){
		pthread_join(thread[i],NULL);
	}
	
}

void gt_references_test(void){
	of1x_flow_entry_t entry, entry2;
	assert (of1x_group_add(tu.gt, tu.type, tu.id, tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	assert(of1x_add_reference_entry_in_group(tu.gt->head,&entry)==ROFL_SUCCESS);
	assert(tu.gt->head->referencing_entries != NULL);
	
	assert(of1x_add_reference_entry_in_group(tu.gt->head,&entry2)==ROFL_SUCCESS);
	assert(of1x_delete_reference_entry_in_group(tu.gt->head,&entry2)==ROFL_SUCCESS);
	assert(of1x_delete_reference_entry_in_group(tu.gt->head,&entry2)==ROFL_FAILURE);
	
	assert(of1x_group_delete(tu.gt, tu.id)==ROFL_SUCCESS);
}
