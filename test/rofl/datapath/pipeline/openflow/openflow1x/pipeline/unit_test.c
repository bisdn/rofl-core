/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"

#include "pp_isolation.h"
#include "group_table.h"
#include "timers_hard_timeout.h"
#include "output_actions.h"

int main(int args, char** argv){

	int return_code;
	//main to call all the other tests written in the oder files in this folder
	CU_pSuite output_suite = NULL, timers_hard_suite=NULL;
	CU_pSuite pp_isolation = NULL; 

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
/*
	// add a suite to the registry 
	pSuite = CU_add_suite("Suite_group_table", gt_set_up, gt_tear_down);
	if (NULL == pSuite) {
	CU_cleanup_registry();
	return CU_get_error();
	}
	// add the tests to the suite
	// NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf()
	if ((NULL == CU_add_test(pSuite, "test basic", gt_basic_test)) ||
	(NULL == CU_add_test(pSuite, "test expected errors", gt_expected_errors_test)) ||
	(NULL == CU_add_test(pSuite, "test buckets", gt_add_and_delete_buckets_test)) || 
		(NULL == CU_add_test(pSuite, "test threads concurrency", gt_concurrency_test)) || 
		(NULL == CU_add_test(pSuite, "test references", gt_references_test)) )
	{
	CU_cleanup_registry();
	return CU_get_error();
	}
*/

	if((output_suite = CU_add_suite("suite for the output actions", NULL, NULL))==NULL){
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((CU_add_test(output_suite,"basic test",oa_basic_test))==NULL ||
		(CU_add_test(output_suite,"no output ",oa_no_output))==NULL ||
		(CU_add_test(output_suite,"only apply ",oa_only_apply))==NULL ||
		(CU_add_test(output_suite,"only write ",oa_only_write))==NULL ||
		(CU_add_test(output_suite,"2 out apply ",oa_two_outputs_apply))==NULL ||
		(CU_add_test(output_suite,"2 out write ",oa_two_outputs_write))==NULL ||
		(CU_add_test(output_suite," write and group ",oa_write_and_group))==NULL ||
		(CU_add_test(output_suite,"apply and group",oa_apply_and_group))==NULL ||
			(CU_add_test(output_suite,"groups",oa_test_with_groups))==NULL){
		CU_cleanup_registry();
		return CU_get_error();
	}
	
	timers_hard_suite = CU_add_suite("Suite_timers_hard", NULL, NULL);
	if (NULL == timers_hard_suite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(timers_hard_suite, "main test", main_test)) ){
		CU_cleanup_registry();
		return CU_get_error();
	}

	pp_isolation = CU_add_suite("Packet processing API isolation", NULL, NULL);
	if ((NULL == CU_add_test(pp_isolation, "Packet processing API isolation", pp_test)) ){
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
