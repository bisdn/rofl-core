#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"

#include "test_bufs.h"


int main(int args, char** argv){

	int return_code;
	//main to call all the other tests written in the oder files in this folder
	CU_pSuite bufs_suite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
		
	if((bufs_suite = CU_add_suite("Suite for pipeline pkt transversing tests", bufs_set_up, bufs_tear_down))==NULL){
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((CU_add_test(bufs_suite,"No output action (drop)",bufs_no_output_action)==NULL) ||
		(CU_add_test(bufs_suite,"Single output apply action",bufs_apply_output_action)==NULL) ||
		(CU_add_test(bufs_suite,"Single output write action",bufs_write_output_action)==NULL) ||
		(CU_add_test(bufs_suite,"Multiple output apply actions",bufs_multiple_apply_output_actions)==NULL) ||
		(CU_add_test(bufs_suite,"Multiple output apply and write actions",bufs_multiple_apply_write_output_actions)==NULL) ||
		(CU_add_test(bufs_suite,"Single output in a group in apply_actions (indirect)",bufs_single_output_action_group_apply)==NULL) ||
		(CU_add_test(bufs_suite,"Single output in a group in write_actions (indirect)",bufs_single_output_action_group_write)==NULL) ||
		(CU_add_test(bufs_suite,"Apply and group(indirect) single output",bufs_apply_and_group_output_actions)==NULL) ||
		(CU_add_test(bufs_suite,"Write and group(indirect) single output",bufs_write_and_group_output_actions)==NULL) ||
		(CU_add_test(bufs_suite,"Multiple outputs in a group(type ALL)", bufs_multiple_output_actions_group)==NULL) ||
		(CU_add_test(bufs_suite,"No output action with goto", bufs_no_output_action_goto)==NULL) ||
		(CU_add_test(bufs_suite,"Output action(apply) with goto in the second table", bufs_apply_output_action_last_table_goto)==NULL) ||
		(CU_add_test(bufs_suite,"Output action on both tables", bufs_apply_output_action_both_tables_goto)==NULL)
	){
		fprintf(stderr,"ERROR WHILE ADDING TEST\n");
		CU_cleanup_registry();
		return CU_get_error();
	}
	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	return_code = CU_get_number_of_failures();
	CU_cleanup_registry();

	return return_code;
}


/*next test: install flow mod an mtch?*/
