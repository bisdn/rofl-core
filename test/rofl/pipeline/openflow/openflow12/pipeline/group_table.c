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

	
int set_up(void){
	tu.gt = of12_init_group_table();
	if( tu.gt == NULL){
		fprintf(stderr,"<%s:%d>Error\n",__func__,__LINE__);
		return ROFL_FAILURE;
	}
	
	tu.type = OF12PGT_ALL;
	tu.id = 12;
	tu.weight = 1;
	tu.group = 1;
	tu.port = 1;
	tu.actions = NULL;
	return ROFL_SUCCESS;
}

int tear_down(){
	of12_destroy_group_table(tu.gt);
	return ROFL_SUCCESS;
}

void basic_test(){
	
	assert (of12_group_add(tu.gt, tu.type, tu.id, tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	
	assert(tu.gt->num_of_entries==1);
	
	tu.type = OF12PGT_SELECT;
	
	assert(of12_group_modify(tu.gt,tu.type,tu.id,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	
	assert(tu.gt->num_of_entries==1);
	
	assert(of12_group_delete(tu.gt,tu.id)==ROFL_SUCCESS);
	
	assert(tu.gt->num_of_entries==0);
	
	fprintf(stderr,"<%s:%d>Test passed\n",__func__,__LINE__);
}

void expected_errors_test(){
	
	assert(of12_group_delete(tu.gt,tu.id)==ROFL_SUCCESS); // no notification when trying to delete a non existing entry
	
	assert(of12_group_add(tu.gt,tu.type,tu.id,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	assert(of12_group_add(tu.gt,tu.type,tu.id+1,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	assert(tu.gt->num_of_entries == 2);
	assert(of12_group_add(tu.gt,tu.type,tu.id+1,tu.weight, tu.group, tu.port, tu.actions)==ROFL_FAILURE);
	assert(tu.gt->num_of_entries == 2);
	
	assert(of12_group_add(tu.gt,tu.type,tu.id+2,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	assert(of12_group_delete(tu.gt,tu.id+1)==ROFL_SUCCESS);
	assert(tu.gt->num_of_entries == 2);
	
	assert(of12_group_modify(tu.gt,tu.type,tu.id+1,tu.weight, tu.group, tu.port, tu.actions)==ROFL_FAILURE);
	assert(of12_group_modify(tu.gt,tu.type+2,tu.id+2,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	assert(tu.gt->num_of_entries==2);
	assert(tu.gt->head->type==tu.type);
	assert(tu.gt->tail->type==tu.type+2);
	
}

void add_and_delete_buckets_test(){
	
	of12_packet_action_t* pkt_action;
	
	tu.actions = of12_init_action_group(NULL);
	
	pkt_action = of12_init_packet_action(OF12_AT_COPY_TTL_IN,1,NULL,NULL);
	of12_push_packet_action_to_group(tu.actions,pkt_action);
	pkt_action = of12_init_packet_action(OF12_AT_GROUP,1,NULL,NULL);
	of12_push_packet_action_to_group(tu.actions,pkt_action);
	
	of12_group_add(tu.gt,tu.type,tu.id, tu.weight, tu.group, tu.port, tu.actions);
	of12_group_delete(tu.gt,tu.id);
	
	of12_destroy_action_group(tu.actions);
}

void concurrency_routine(void * param){
	info_th_t* info = (info_th_t *)param;
	int i=0;

	(void)info;

	fprintf (stderr,"Hallo\n");
	
	assert (of12_group_add(tu.gt, info->id, info->id, tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
	
	for (i=0;i<500;i++){
		assert(of12_group_modify(tu.gt,info->id,info->id,tu.weight, tu.group, tu.port, tu.actions)==ROFL_SUCCESS);
		usleep(100);
	}
	
	assert(of12_group_delete(tu.gt,info->id)==ROFL_SUCCESS);
}

void concurrency_test(void){
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

// TODO delete thid main and let cunit work!
/*
int main(int args, char **argv){
	int nok=0, nko=0;
	
	fprintf(stderr,"<%s:%d>Test for group table\n",__func__,__LINE__);
	
	if(basic_test()==ROFL_SUCCESS) nok++;
	else nko++;
	
	if(expected_errors_test()==ROFL_SUCCESS) nok++;
	else nko++;
	
	if(add_and_delete_buckets_test()==ROFL_SUCCESS) nok++;
	else nko++;
	
	if(add_and_delete_buckets_test()==ROFL_SUCCESS) nok++;
	else nko++;
	
	fprintf(stderr,"<%s:%d>Test finished OK:%d KO:%d\n",__func__,__LINE__,nok,nko);
	if (nko == 0) return ROFL_SUCCESS;
	else return ROFL_FAILURE;
}
*/
