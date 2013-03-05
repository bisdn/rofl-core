#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"

#include "group_table.h"

int main(){
	//main to call all the other tests written in the oder files in this folder
	CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_group_table", set_up, tear_down);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   if ((NULL == CU_add_test(pSuite, "test basic", basic_test)) ||
       (NULL == CU_add_test(pSuite, "test expected errors", expected_errors_test)) ||
       (NULL == CU_add_test(pSuite, "test buckets", add_and_delete_buckets_test)) || 
		(NULL == CU_add_test(pSuite, "test conccurrent", concurrency_test)) )
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