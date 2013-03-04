#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "CUnit/Basic.h"


#include "../../../../../../src/rofl.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/openflow12.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_group_table.h"
#include "../../../../../../src/rofl/pipeline/openflow/of_switch.h"

switch_port_t flood_meta_port;
struct test_utils{
	of12_group_table_t* gt;
	of12_group_type_t type;
	uint32_t id;
	uint32_t weight;
	uint32_t group;
	uint32_t port;
	of12_action_group_t *actions;
};

int set_up(struct test_utils *tu){
	tu->gt = of12_init_group_table();
	if( tu->gt == NULL){
		fprintf(stderr,"<%s:%d>Error\n",__func__,__LINE__);
		return ROFL_FAILURE;
	}
	
	tu->type = OF12PGT_ALL;
	tu->id = 12;
	tu->weight = 1;
	tu->group = 1;
	tu->port = 1;
	tu->actions = NULL;
	return ROFL_SUCCESS;
}

int tear_down(){
	return ROFL_SUCCESS;
}

int basic_test(){
	
	struct test_utils tu;
	if ( set_up(&tu) != ROFL_SUCCESS)
		return ROFL_FAILURE;
	
	if (of12_group_add(tu.gt, tu.type, tu.id, tu.weight, tu.group, tu.port, tu.actions)!=ROFL_SUCCESS){
		fprintf(stderr,"<%s:%d>Error\n",__func__,__LINE__);
		return ROFL_FAILURE;
	}
	
	assert(tu.gt->num_of_entries==1);
	
	tu.type = OF12PGT_SELECT;
	
	if(of12_group_modify(tu.gt,tu.type,tu.id,tu.weight, tu.group, tu.port, tu.actions)!=ROFL_SUCCESS){
		fprintf(stderr,"<%s:%d>Error\n",__func__,__LINE__);
		return ROFL_FAILURE;
	}
	
	assert(tu.gt->num_of_entries==1);
	
	if(of12_group_delete(tu.gt,tu.id)!=ROFL_SUCCESS){
		fprintf(stderr,"<%s:%d>Error\n",__func__,__LINE__);
		return ROFL_FAILURE;
	}
	
	assert(tu.gt->num_of_entries==0);
	
	of12_destroy_group_table(tu.gt);
	
	fprintf(stderr,"<%s:%d>Test passed\n",__func__,__LINE__);
	return ROFL_SUCCESS;
}

int expected_errors_test(){
	
	struct test_utils tu;
	if ( set_up(&tu) != ROFL_SUCCESS)
		return ROFL_FAILURE;
	
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
	
	of12_destroy_group_table(tu.gt);
	
	return ROFL_SUCCESS;
}

int add_and_delete_buckets_test(){
	
	struct test_utils tu;
	of12_packet_action_t* pkt_action;
	
	set_up(&tu);
	
	tu.actions = of12_init_action_group(NULL);
	
	pkt_action = of12_init_packet_action(OF12_AT_COPY_TTL_IN,1,NULL,NULL);
	of12_push_packet_action_to_group(tu.actions,pkt_action);
	pkt_action = of12_init_packet_action(OF12_AT_GROUP,1,NULL,NULL);
	of12_push_packet_action_to_group(tu.actions,pkt_action);
	
	of12_group_add(tu.gt,tu.type,tu.id, tu.weight, tu.group, tu.port, tu.actions);
	of12_group_delete(tu.gt,tu.id);
	
	of12_destroy_action_group(tu.actions);
	
	of12_destroy_group_table(tu.gt);
	return ROFL_SUCCESS;
}

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
