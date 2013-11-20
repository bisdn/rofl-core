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
/*		(CU_add_test(bufs_suite,"install flow mod",)==NULL) ||*/
		(CU_add_test(bufs_suite,"Single output apply action",bufs_apply_output_action)==NULL)
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
