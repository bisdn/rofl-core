#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "rofl/datapath/pipeline/openflow/of_switch_pp.h"

#include "matching_test.h"
#include "test_ipv6.h"

int main(int args, char** argv){

	int return_code;
	//main to call all the other tests written in the oder files in this folder
	CU_pSuite pSuite = NULL, ipv6_suite = NULL;

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
	(NULL == CU_add_test(pSuite, "test uninstall wildcard", test_uninstall_wildcard)) || 
	(NULL == CU_add_test(pSuite, "test check overlap addition", test_overlap)) || 
	(NULL == CU_add_test(pSuite, "test check overlap addition2", test_overlap2)) || 
	(NULL == CU_add_test(pSuite, "test flow modify", test_flow_modify)) 
	
		)
	{
		fprintf(stderr,"ERROR WHILE ADDING TEST\n");
		return_code = CU_get_error();
		CU_cleanup_registry();
		return return_code;
	}
	
	
	if((ipv6_suite = CU_add_suite("suite for ipv6 compatibility", ipv6_set_up, ipv6_tear_down))==NULL){
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((CU_add_test(ipv6_suite,"basic test",ipv6_basic_test)==NULL) ||
		(CU_add_test(ipv6_suite,"install flow mod",ipv6_install_flow_mod)==NULL) ||
		(CU_add_test(ipv6_suite,"install flow mod complete",ipv6_install_flow_mod_complete)==NULL) ||
		(CU_add_test(ipv6_suite,"icmpv6",icmpv6_install_flow_mod_complete)==NULL) ||
		(CU_add_test(ipv6_suite,"utern 128 bits",ipv6_utern_test)==NULL)	||
		(CU_add_test(ipv6_suite,"get_alike_low",ipv6_alike_test_low)==NULL)	||
		(CU_add_test(ipv6_suite,"get_alike_high",ipv6_alike_test_high)==NULL) ||
		(CU_add_test(ipv6_suite,"get_alike_wrong",ipv6_alike_test_wrong)==NULL)
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
