/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "CUnit/Basic.h"
#include "../../../../../src/rofl/datapath/pipeline/physical_switch.h"
#include "../../../../../src/rofl/datapath/pipeline/monitoring.h"

int set_up(){
	return 0;
}

int tear_down(){
	fprintf(stderr,"Tearing down..\n");
	__monitoring_destroy(&(get_physical_switch()->monitoring));
	return 0;
}

void reset(){
	fprintf(stderr,"Reset..\n");
	__monitoring_destroy(&(get_physical_switch()->monitoring));
	__monitoring_init(&(get_physical_switch()->monitoring));
}

void test_simple_insertions(){
	int i;
	monitoring_state_t* mon = &(get_physical_switch()->monitoring);
	
	//Add backplane as inner element to chassis
	monitoring_dump(mon);

	CU_ASSERT(mon->chassis.next == NULL);
	CU_ASSERT(mon->chassis.prev == NULL);
	CU_ASSERT(mon->chassis.inner == NULL);
	CU_ASSERT(mon->chassis.parent == NULL);

	monitored_entity_t* backplane = monitoring_add_monitored_entity(mon, ME_TYPE_BACKPLANE, NULL, &mon->chassis);
	
	CU_ASSERT(backplane != NULL);
	CU_ASSERT(mon->chassis.inner == backplane);
	CU_ASSERT(backplane->parent == &mon->chassis);
	CU_ASSERT(backplane->inner == NULL);
	CU_ASSERT(backplane->next == NULL);
	CU_ASSERT(backplane->prev == NULL);
	
	monitoring_dump(mon);


	//Add N elements (in front)
	monitored_entity_t* entity2=NULL;
	for(i=0;i<50;i++){
		entity2 = monitoring_add_monitored_entity(mon, ME_TYPE_PORT, NULL, backplane);
		CU_ASSERT(entity2 != NULL);
		//Assign id
		entity2->physical_index = i;
	}

	monitoring_dump(mon);
	
	//Go backwards and check correct insertion order
	for(i=49;entity2;entity2 = entity2->prev,i--){
		CU_ASSERT(entity2->physical_index == i);
		assert(entity2->physical_index == i);
	}

	//Add other to the same level of the backplane 
	monitored_entity_t* other = monitoring_add_monitored_entity(mon, ME_TYPE_OTHER, backplane, NULL);

	CU_ASSERT(other != NULL);
	CU_ASSERT(other->parent == &mon->chassis);
	CU_ASSERT(other->prev == backplane);
	CU_ASSERT(other->next == NULL);

	monitoring_dump(mon);
		
	CU_ASSERT(mon->chassis.next == NULL);
	CU_ASSERT(mon->chassis.prev == NULL);
	CU_ASSERT(mon->chassis.parent == NULL);
	
	reset();
		
	CU_ASSERT(mon->chassis.next == NULL);
	CU_ASSERT(mon->chassis.prev == NULL);
	CU_ASSERT(mon->chassis.parent == NULL);

	monitoring_dump(mon);
}

void test_simple_deletions(){
	
	int i;
	monitoring_state_t* mon = &(get_physical_switch()->monitoring);
	
	monitoring_dump(&(get_physical_switch()->monitoring));

	//Add backplane as inner element to chassis
	monitoring_dump(mon);

	CU_ASSERT(mon->chassis.next == NULL);
	CU_ASSERT(mon->chassis.prev == NULL);
	CU_ASSERT(mon->chassis.inner == NULL);
	CU_ASSERT(mon->chassis.parent == NULL);

	monitored_entity_t* backplane = monitoring_add_monitored_entity(mon, ME_TYPE_BACKPLANE, NULL, &mon->chassis);
	
	CU_ASSERT(backplane != NULL);
	CU_ASSERT(mon->chassis.inner == backplane);
	CU_ASSERT(backplane->parent == &mon->chassis);
	CU_ASSERT(backplane->inner == NULL);
	CU_ASSERT(backplane->next == NULL);
	CU_ASSERT(backplane->prev == NULL);
	
	monitoring_dump(mon);


	//Add N elements (in front)
	monitored_entity_t* entity2=NULL;
	for(i=0;i<50;i++){
		entity2 = monitoring_add_monitored_entity(mon, ME_TYPE_PORT, NULL, backplane);
			
		CU_ASSERT(entity2 != NULL);
		//Assign id
		entity2->physical_index = i;
	}
	
	monitoring_dump(mon);

	fprintf(stderr,"Trying to delete me: %p\n", entity2);
	
	//Delete the first (i=0)
	CU_ASSERT(monitoring_remove_monitored_entity(mon, entity2) == ROFL_SUCCESS); 
	
	CU_ASSERT(backplane->inner->parent == backplane);
	CU_ASSERT(backplane->inner->physical_index == 48);
	CU_ASSERT(backplane->inner->next->physical_index == 47);

	monitoring_dump(mon);

	//Delete the 47th element
	//First add some dummy stuff
	entity2 = monitoring_add_monitored_entity(mon, ME_TYPE_STACK, NULL, backplane->inner->next);
		
	CU_ASSERT(entity2 != NULL);
	CU_ASSERT(entity2->next == NULL);
	CU_ASSERT(entity2->prev == NULL);
	CU_ASSERT(entity2->inner == NULL);
	CU_ASSERT(entity2->parent == backplane->inner->next);
	
	//Now delete this element only
	CU_ASSERT(monitoring_remove_monitored_entity(mon, backplane->inner->next) == ROFL_SUCCESS);

	//Verify	
	CU_ASSERT(backplane->inner->parent == backplane);
	CU_ASSERT(backplane->inner->physical_index == 48);
	CU_ASSERT(backplane->inner->next->physical_index == 46);
	
}

void test_snapshots(){
	
	monitoring_state_t* mon = &(get_physical_switch()->monitoring);

	//Add other to the same level of the backplane
	monitored_entity_t* backplane = mon->chassis.inner; 
	monitored_entity_t* other = monitoring_add_monitored_entity(mon, ME_TYPE_OTHER, backplane, NULL);

	monitoring_dump(mon);

	CU_ASSERT(other != NULL);
	CU_ASSERT(other->parent == &mon->chassis);
	CU_ASSERT(other->prev == backplane);
	CU_ASSERT(other->next == NULL);

	//The state is the same left in "test_simple_deletions"
	monitoring_dump(mon);
	
	//Get a snapshot
	monitoring_snapshot_state_t* snapshot = monitoring_get_snapshot(mon);
	
	//Minor verifications	
	CU_ASSERT(snapshot != NULL);
	CU_ASSERT(snapshot->is_snapshot);
	CU_ASSERT(snapshot->last_rev == mon->last_rev);
	
	monitoring_dump_snapshot(snapshot);
	
	//Delete it
	monitoring_destroy_snapshot(snapshot);
}

int main(int args, char** argv){

	physical_switch_init();

	int return_code;
	//main to call all the other tests written in the oder files in this folder
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("Suite_Pipeline_Monitoring", set_up, tear_down);

	if (NULL == pSuite){
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	/* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
	if ((NULL == CU_add_test(pSuite, "test simple insertions", test_simple_insertions)) ||
	(NULL == CU_add_test(pSuite, "test simple_deletions", test_simple_deletions)) ||
	(NULL == CU_add_test(pSuite, "test snapshots", test_snapshots))
		)
	{
		fprintf(stderr,"ERROR WHILE ADDING TEST\n");
		return_code = CU_get_error();
		CU_cleanup_registry();
		return return_code;
	}
	
	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	return_code = CU_get_number_of_failures();
	CU_cleanup_registry();

	return return_code;
}
