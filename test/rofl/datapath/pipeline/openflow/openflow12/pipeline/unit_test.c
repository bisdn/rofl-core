#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"

#include "group_table.h"
#include "timers_hard_timeout.h"

int main(int args, char** argv){

	int return_code;
	//main to call all the other tests written in the oder files in this folder
	CU_pSuite /*pSuite = NULL,*/ timers_hard_suite;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	/*
	// add a suite to the registry 
	pSuite = CU_add_suite("Suite_group_table", set_up, tear_down);
	if (NULL == pSuite) {
	CU_cleanup_registry();
	return CU_get_error();
	}
	// add the tests to the suite
	// NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf()
	if ((NULL == CU_add_test(pSuite, "test basic", basic_test)) ||
	(NULL == CU_add_test(pSuite, "test expected errors", expected_errors_test)) ||
	(NULL == CU_add_test(pSuite, "test buckets", add_and_delete_buckets_test)) || 
		(NULL == CU_add_test(pSuite, "test threads concurrency", concurrency_test)) || 
		(NULL == CU_add_test(pSuite, "test references", references_test)) )
	{
	CU_cleanup_registry();
	return CU_get_error();
	}
	*/
	timers_hard_suite = CU_add_suite("Suite_timers_hard", NULL, NULL);
	if (NULL == timers_hard_suite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(timers_hard_suite, "main test", main_test)) ){
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
