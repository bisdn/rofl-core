#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"

#include "matching_test.h"

int main(int args, char** argv){
	//main to call all the other tests written in the oder files in this folder
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("Suite_Loop_matching algorithm", set_up, tear_down);

	if (NULL == pSuite){
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	/* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
	if ((NULL == CU_add_test(pSuite, "test install empty flowmod", test_install_empty_flow_mod)) ||
	(NULL == CU_add_test(pSuite, "test uninstall all", test_install_overlapping_specific)) ||
	//(NULL == CU_add_test(pSuite, "test uninstall all", test_uninstall_all_wildcarded)) ||
	(NULL == CU_add_test(pSuite, "test overlap add", test_overlap)) 
	
		)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
