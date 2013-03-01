#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../../../../../../src/rofl.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_group_table.h"
#include "../../../../../../src/rofl/pipeline/openflow/of_switch.h"

switch_port_t flood_meta_port;

int basic_test(){
	
	of12_group_table_t *gt= of12_init_group_table();
	//create a table, create some entries, remove and modify
	if( gt == NULL){
		fprintf(stderr,"<%s:%d>Error\n",__func__,__LINE__);
		return ROFL_FAILURE;
	}
	
	enum of12p_group_type type = OF12PGT_ALL;
	uint32_t id=12;
	
	if (of12_group_add(gt, type, id)!=ROFL_SUCCESS){
		fprintf(stderr,"<%s:%d>Error\n",__func__,__LINE__);
		return ROFL_FAILURE;
	}
	
	assert(gt->num_of_entries==1);
	
	type = OF12PGT_SELECT;
	
	if(of12_group_modify(gt,type,id)!=ROFL_SUCCESS){
		fprintf(stderr,"<%s:%d>Error\n",__func__,__LINE__);
		return ROFL_FAILURE;
	}
	
	assert(gt->num_of_entries==1);
	
	if(of12_group_delete(gt,id)!=ROFL_SUCCESS){
		fprintf(stderr,"<%s:%d>Error\n",__func__,__LINE__);
		return ROFL_FAILURE;
	}
	
	assert(gt->num_of_entries==0);
	
	of12_destroy_group_table(gt);
	
	fprintf(stderr,"<%s:%d>Test passed\n",__func__,__LINE__);
	return ROFL_SUCCESS;
}

int expected_errors_test(){
	of12_group_table_t *gt= of12_init_group_table();
	enum of12p_group_type type = OF12PGT_ALL;
	uint32_t id=12;
	
	if(of12_group_delete(gt,id)!=ROFL_SUCCESS){ // no notification when trying to delete a non existing entry
		fprintf(stderr,"<%s:%d>Error\n",__func__,__LINE__);
		return ROFL_FAILURE;
	}
	
	assert(of12_group_add(gt,type,id)==ROFL_SUCCESS);
	assert(of12_group_add(gt,type,id+1)==ROFL_SUCCESS);
	assert(gt->num_of_entries == 2);
	assert(of12_group_add(gt,type,id+1)==ROFL_FAILURE);
	assert(gt->num_of_entries == 2);
	
	assert(of12_group_add(gt,type,id+2)==ROFL_SUCCESS);
	assert(of12_group_delete(gt,id+1)==ROFL_SUCCESS);
	assert(gt->num_of_entries == 2);
	
	assert(of12_group_modify(gt,type,id+1)==ROFL_FAILURE);
	assert(of12_group_modify(gt,type+1,id)==ROFL_SUCCESS);
	assert(gt->num_of_entries==2);
	assert(gt->head->type==type);
	assert(gt->tail->type==type+1);
	
	of12_destroy_group_table(gt);
	
	return ROFL_SUCCESS;
}

int main(int args, char **argv){
	int nok=0, nko=0;
	
	fprintf(stderr,"<%s:%d>Test for group table\n",__func__,__LINE__);
	
	if(basic_test()==ROFL_SUCCESS) nok++;
	else nko++;
	
	if(expected_errors_test()==ROFL_SUCCESS) nok++;
	else nko++;
	
	
	
	fprintf(stderr,"<%s:%d>Test finished OK:%d KO:%d\n",__func__,__LINE__,nok,nko);
	if (nko == 0) return ROFL_SUCCESS;
	else return ROFL_FAILURE;
}
