#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "../../../../../../src/rofl/pipeline/openflow/openflow12/of12_switch.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_pipeline.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_flow_entry.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_flow_table.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_timers.h"
//#include "../../../../../../src/rofl/pipeline/platform/cutil.h"
#include "../../../../../../src/rofl/pipeline/platform/lock.h"
#include "../../../../../../src/rofl/pipeline/platform/memory.h"

#include "lib_assert.h"
#include "lib_random.h"

#define OF12_TIMERS_TEST_MAX_TIMER_ENTRIES 10000
/*
 * Test for the insertion of entries to be deleted at
 * the corresponding timeout.
 * 
 * simple loops
 * a) insert -> extract
 * b) insert -> expire
 * c) (IDLE) insert -> update -> reschedule -> expire
 * ...
 */

#if 0
/**
 * test one insertion
 */
void test_one_insertion(of12_flow_table_t* table)
{
	uint32_t idle_timeout=0, hard_timeout=22;
	table->entries = cutil_malloc(sizeof(of12_flow_entry_t));
	table->num_of_entries = 1;
	
	of12_add_timer(table, idle_timeout, hard_timeout, table->entries);
	
	cutil_free(table->entries);
}

void test_two_insertions(of12_flow_table_t* table)
{
	uint32_t idle_timeout=0, hard_timeout=22;
	table->entries = cutil_malloc(sizeof(of12_flow_entry_t));
	table->entries->next = cutil_malloc(sizeof(of12_flow_entry_t));
	table->num_of_entries = 2;
	
	of12_add_timer(table, idle_timeout, hard_timeout, table->entries);
	hard_timeout = 44;
	of12_add_timer(table, idle_timeout, hard_timeout, table->entries->next);
	
	cutil_free(table->entries->next);
	cutil_free(table->entries);
}

void test_two_insertions_and_delete(of12_pipeline_t* pipeline)
{
	int i=0;
	of12_flow_table_t* table = pipeline->tables;
	uint32_t idle_timeout=0, hard_timeout=22;
	table->entries = cutil_malloc(sizeof(of12_flow_entry_t));
	table->entries->next = cutil_malloc(sizeof(of12_flow_entry_t));
	table->num_of_entries = 2;
	
	of12_add_timer(table, idle_timeout, hard_timeout, table->entries);
	hard_timeout = 44;
	of12_add_timer(table, idle_timeout, hard_timeout, table->entries->next);
	
	for(i=0;i<110;i++)
		of12_process_pipeline_tables_timeout_expirations(pipeline);
	
	cutil_free(table->entries->next);
	cutil_free(table->entries);
}

void test_several_insertions_and_delete(of12_pipeline_t* pipeline)
{
	int i;
	
	of12_flow_table_t* table = pipeline->tables;
	uint32_t idle_timeout=0, hard_timeout=22;
	table->entries = cutil_malloc(sizeof(of12_flow_entry_t));
	table->entries->next = cutil_malloc(sizeof(of12_flow_entry_t));
	table->num_of_entries = 2;
	
	of12_add_timer(table, idle_timeout, hard_timeout, table->entries);
	of12_add_timer(table, idle_timeout, hard_timeout, table->entries);
	of12_add_timer(table, idle_timeout, hard_timeout, table->entries);
	hard_timeout = 44;
	of12_add_timer(table, idle_timeout, hard_timeout, table->entries->next);
	of12_add_timer(table, idle_timeout, hard_timeout, table->entries->next);
	of12_add_timer(table, idle_timeout, hard_timeout, table->entries->next);
	
	for(i=0;i<110;i++)
		of12_process_pipeline_tables_timeout_expirations(pipeline);
	
	cutil_free(table->entries->next);
	cutil_free(table->entries);
}

/**
 * Need to perform a hard test
 * -> deletion of timeouts
 */
void test_deletion_from_outside_dynamic(of12_pipeline_t* pipeline)
{
	int j;
	int i;
	of12_flow_table_t* table = pipeline->tables;
	uint32_t idle_timeout=0, hard_timeout=22;
	of12_flow_entry_t entries[20];
	//table->entries = cutil_malloc(sizeof(of12_flow_entry_t));
	//table->num_of_entries = 1;
	
	for(j=0; j<10; j++)
	{
		entries[j].timer_info.hard_timeout = hard_timeout;
		entries[j].timer_info.idle_timeout = idle_timeout;
		of12_add_timer(table, idle_timeout, hard_timeout, &(entries[j]));
	}
	
	of12_dump_timers_structure(table->timers);
	
	hard_timeout = 44;
	for(; j<20; j++)
	{
		entries[j].timer_info.hard_timeout = hard_timeout;
		entries[j].timer_info.idle_timeout = idle_timeout;
		of12_add_timer(table, idle_timeout, hard_timeout, &(entries[j]));
	}
	
	of12_dump_timers_structure(table->timers);
	for(;table->timers->list.head->next;)
	{
		fprintf(stderr,"DELETION:\n");
		platform_mutex_lock(&table->mutex);
		of12_destroy_timer_entries(table->timers->list.head->next->entry);
		platform_mutex_unlock(&table->mutex);
		of12_dump_timers_structure(table->timers);
	}
	
	for(i=0;i<500;i++)
	{
		of12_process_pipeline_tables_timeout_expirations(pipeline);
	}
	of12_dump_timers_structure(table->timers);
}

/**
 * Need to perform a hard test
 * -> deletion of timeouts
 */
void test_deletion_from_outside_static(of12_pipeline_t* pipeline)
{
	int j; //index de entries
	int i;
	of12_flow_table_t* table = pipeline->tables;
	of12_flow_entry_t entries[50];
	uint32_t idle_timeout=0, hard_timeout=22;
	
	for(j=0; j<10; j++)
		of12_add_timer(table, idle_timeout, hard_timeout, &(entries[j]));
	
	of12_dump_timers_structure(table->timers);
	
	hard_timeout = 44;
	for(; j<20; j++)
		of12_add_timer(table, idle_timeout, hard_timeout, &(entries[j]));
	
	of12_dump_timers_structure(table->timers);
	for(;table->timers[22].list.head->next;)
	{
		fprintf(stderr,"DELETION:\n");
		platform_mutex_lock(&table->mutex);
		ASSERT(of12_destroy_timer_entries( table->timers[22].list.head->next->entry )==EXIT_SUCCESS,"timer erase failed");//of12_destroy_entry_timer(table->timers[22].list.head->next);
		platform_mutex_unlock(&table->mutex);
		of12_dump_timers_structure(table->timers);
	}
	
	for(i=0;i<505;i++)
	{
		of12_process_pipeline_tables_timeout_expirations(pipeline);
	}
	of12_dump_timers_structure(table->timers);
	
}
#endif

#if OF12_TIMER_STATIC_ALLOCATION_SLOTS

void test_insert_and_expiration_static(of12_pipeline_t * pipeline, uint32_t hard_timeout, of12_flow_entry_t * entry)
{
	of12_flow_table_t* table = pipeline->tables;
	struct timeval now;
	of12_time_forward(0,0,&now);
	of12_fill_new_timer_entry_info(entry,hard_timeout,0);
	
	ASSERT(of12_add_timer(table, entry)==EXIT_SUCCESS,"add timer failed");

	int slot = (now.tv_sec+hard_timeout)%OF12_TIMER_GROUPS_MAX;
	//of12_dump_timers_structure(table->timers);
	ASSERT(table->timers[slot].list.num_of_timers==1, "wrong num of timers");
	ASSERT(table->timers[slot].list.head != NULL, "head poiter is NULL");//this can improve with the actual addersses
	ASSERT(table->timers[slot].list.tail != NULL, "tail poiter is NULL");
	ASSERT(table->timers[slot].list.head == table->timers[slot].list.tail, "head and tail are different");
	
	of12_time_forward(hard_timeout+1,0,&now); //we need to go to hard_timeout +1 because of possible rounding up.
	of12_process_pipeline_tables_timeout_expirations(pipeline);
	
	//of12_dump_timers_structure(table->timers);
	ASSERT(table->timers[slot].list.num_of_timers==0, "wrong num of timers");//this can improve with the actual addersses
	ASSERT(table->timers[slot].list.head == NULL, "head poiter is NOT NULL");
	ASSERT(table->timers[slot].list.tail == NULL, "tail poiter is NULL");
	ASSERT(table->timers[slot].list.head == table->timers[slot].list.tail, "head and tail are different");
	fprintf(stderr,"<%s> test passed\n",__func__);
}

void test_insert_and_extract_static(of12_pipeline_t * pipeline, uint32_t hard_timeout, int num_of_entries, of12_flow_entry_t * entry)
{
	int i;
	of12_flow_table_t* table = pipeline->tables;
	struct timeval now;
	of12_time_forward(0,0,&now);
	
	int slot = (now.tv_sec+hard_timeout)%OF12_TIMER_GROUPS_MAX; //WARNING needs to be mora accurate when MS per SLOT != 1000

	//adding the entries
	for(i=0; i< num_of_entries; i++)
	{
		of12_fill_new_timer_entry_info(&(entry[i]),hard_timeout,0); 	//WARNING supposition: the entry is filled up alone
		ASSERT(of12_add_timer(table, &(entry[i]))==EXIT_SUCCESS,"add timer failed");
		
		ASSERT(table->timers[slot].list.num_of_timers==i+1, "wrong num of timers");
		ASSERT(table->timers[slot].list.head != NULL, "head poiter is NULL"); //this can improve with the actual addersses
		ASSERT(table->timers[slot].list.tail != NULL, "tail poiter is NULL");
		if(i==0)
			ASSERT(table->timers[slot].list.head == table->timers[slot].list.tail, "head and tail are different");
		else
			ASSERT(table->timers[slot].list.head != table->timers[slot].list.tail, "head and tail are equal");
	}
	
	//check pointers of entries
	i=0;
	of12_entry_timer_t * entry_iterator=table->timers[slot].list.head;
	for(;entry_iterator->next;entry_iterator=entry_iterator->next)
	{
		i++;
	}
	ASSERT(i==num_of_entries-1,"Number of hops incorrect");
	
	//external extraction of the entries
	for(i=0; i< num_of_entries; i++)
	{
		platform_mutex_lock(&table->mutex);

		if(i==num_of_entries-2)
		{
			ASSERT(of12_destroy_timer_entries(&(entry[i]), table)==EXIT_SUCCESS,"erase timer failed");
			ASSERT(table->timers[slot].list.head == table->timers[slot].list.tail, "head and tail are different");
		}
		else if(i==num_of_entries-1)
		{
			ASSERT(of12_destroy_timer_entries(&(entry[i]), table)==EXIT_SUCCESS,"erase timer failed");
			ASSERT(table->timers[slot].list.head == NULL, "head poiter is NOT NULL");
			ASSERT(table->timers[slot].list.tail == NULL, "tail poiter is NULL");
		}
		else
		{
			fprintf(stderr,"i %d entry %p hto %d ne %d \n", i, &(entry[i]), hard_timeout, num_of_entries);
			ASSERT(of12_destroy_timer_entries(&(entry[i]), table)==EXIT_SUCCESS,"erase timer failed");
		}
		ASSERT(table->timers[slot].list.num_of_timers==(num_of_entries-i-1), "wrong num of timers");
		platform_mutex_unlock(&table->mutex);
	}
	
	//test extraction without entries NOTE it does not make sense anymore
	//ASSERT(of12_destroy_timer_entries(&(entry[0]), table)==EXIT_FAILURE, "extraction when list is empty failed");
	
	fprintf(stderr,"<%s> test passed\n",__func__);
	
}

/**
 * Test for Idle timers
 */
void test_simple_idle_static(of12_pipeline_t * pipeline, of12_flow_entry_t * entry, uint32_t ito)
{
	of12_flow_table_t * table = pipeline->tables;
	struct timeval now;
	//WARNING not working for slots different than seconds (1000 ms)
	int slot, i;
	of12_fill_new_timer_entry_info(&(entry[0]),0,ito);
	
	//insert a timer
	ASSERT(of12_add_timer(table, &(entry[0]))==EXIT_SUCCESS,"error add timer");
	of12_time_forward(0,0,&now);
	fprintf(stderr,"added idle TO (%p) at time %lu:%lu for %d seconds\n", &(entry[0]), now.tv_sec, now.tv_usec, ito);
	slot = (now.tv_sec+ito)%OF12_TIMER_GROUPS_MAX;
	ASSERT(table->timers[slot].list.head->entry == &(entry[0]),"entry address");
	
	//update the counter
	of12_time_forward(ito-1,0,&now);
	of12_timer_update_entry(&(entry[0]));
	of12_process_pipeline_tables_timeout_expirations(pipeline);
	fprintf(stderr,"updated last used. TO (%p) at time %lu:%lu for %d seconds\n", &(entry[0]), now.tv_sec, now.tv_usec, ito);
	slot = (now.tv_sec+1)%OF12_TIMER_GROUPS_MAX;
	ASSERT(table->timers[slot].list.head->entry == &(entry[0]),"entry address");
	
	//check that it is not expired but rescheduled
	of12_time_forward(1,0,&now);
	of12_process_pipeline_tables_timeout_expirations(pipeline);
	slot = (now.tv_sec+ito-1)%OF12_TIMER_GROUPS_MAX;
	ASSERT(table->timers[slot].list.head->entry == &(entry[0]),"entry address");
	
	//check final expiration
	of12_time_forward(ito,0,&now);
	of12_process_pipeline_tables_timeout_expirations(pipeline);
	for(i=0; i<OF12_TIMER_GROUPS_MAX; i++)
			ASSERT(table->timers[i].list.num_of_timers == 0,"entry address still there!");
	
	fprintf(stderr,"<%s> test passed\n",__func__);
}

void test_insert_both_expires_one_check_the_other_static(of12_pipeline_t * pipeline, of12_flow_entry_t * entry, uint32_t hto, uint32_t ito)
{
	of12_flow_table_t * table = pipeline->tables;
	struct timeval now;
	of12_time_forward(0,0,&now);
	int slot, i;
	
	of12_fill_new_timer_entry_info(&(entry[0]),hto,ito);
	
	ASSERT(of12_add_timer(table, &(entry[0]))==EXIT_SUCCESS,"error add timer");
	
	if(hto==ito)
	{
		of12_time_forward(ito,0,&now);
		of12_process_pipeline_tables_timeout_expirations(pipeline);
		for(i=0; i<OF12_TIMER_GROUPS_MAX; i++)
			ASSERT(table->timers[i].list.num_of_timers == 0,"entry address still there!");
	}
	else
	{
		uint32_t min = (hto<ito ? hto : ito);
		uint32_t max = (hto>ito ? hto : ito);
		fprintf(stderr,"<%s:%d>hto %u ito %u min %u max %u\n",__func__,__LINE__,hto,ito,min, max);
		slot = (now.tv_sec+ito)%OF12_TIMER_GROUPS_MAX;
		ASSERT(table->timers[slot].list.head->entry==&(entry[0]),"There should be an entry for ito");
		slot = (now.tv_sec+hto)%OF12_TIMER_GROUPS_MAX;
		ASSERT(table->timers[slot].list.head->entry==&(entry[0]),"There should be en entry for hto");
		slot = (now.tv_sec+min)%OF12_TIMER_GROUPS_MAX;
		ASSERT(table->timers[slot].timeout==(min+now.tv_sec)*1000+now.tv_usec/1000,"TO value incorrect");
		slot = (now.tv_sec+max)%OF12_TIMER_GROUPS_MAX;
		ASSERT(table->timers[slot].timeout==(max+now.tv_sec)*1000+now.tv_usec/1000,"TO value incorrect");
		of12_time_forward(min,0,&now);
		of12_process_pipeline_tables_timeout_expirations(pipeline);
		for(i=0; i<OF12_TIMER_GROUPS_MAX; i++)
			ASSERT(table->timers[i].list.num_of_timers == 0,"entry address still there!");
	}
	fprintf(stderr,"<%s> test passed\n",__func__);
}

#else
/**
 * testing insertion and extraction with dynamic timer groups
 */
void test_insert_and_extract_dynamic(of12_pipeline_t * pipeline, uint32_t hard_timeout, int num_of_entries, of12_flow_entry_t * entry)
{
	int i;
	of12_flow_table_t* table = pipeline->tables;
	
	//adding the entries
	for(i=0; i< num_of_entries; i++)
	{
		//WARNING supposition: the entry is filled up alone
		of12_fill_new_timer_entry_info(&(entry[i]),hard_timeout,0);
		
		//test delete entry that has not been inserted
		//if(i==1)
			//ASSERT(of12_destroy_timer_entries(&(entry[i]), table)==EXIT_FAILURE, "extraction when list is empty failed");
		
		ASSERT(of12_add_timer(table, &(entry[i]))==EXIT_SUCCESS,"add timer failed");
		
		ASSERT(table->timers->list.num_of_timers==i+1, "wrong num of timers");
		ASSERT(table->timers->list.head != NULL, "head poiter is NULL");
		ASSERT(table->timers->list.tail != NULL, "tail poiter is NULL");
		if(i==0)
			ASSERT(table->timers->list.head == table->timers->list.tail, "head and tail are different");
		else
			ASSERT(table->timers->list.head != table->timers->list.tail, "head and tail are equal");
	}
	
	//check pointers of entries
	i=0;
	of12_entry_timer_t * entry_iterator=table->timers->list.head;
	for(;entry_iterator->next;entry_iterator=entry_iterator->next)
	{
		i++;
	}
	ASSERT(i==num_of_entries-1,"Number of hops incorrect");
	
	for(i=0; i< num_of_entries; i++)
	{
		platform_mutex_lock(&table->mutex);

		if(i==num_of_entries-1)
		{
			ASSERT(of12_destroy_timer_entries(&(entry[i]), table)==EXIT_SUCCESS,"erase timer failed");
			ASSERT(table->timers == NULL, "timers poiter is NOT NULL");
		}
		else if(i==num_of_entries-2)
		{
			ASSERT(of12_destroy_timer_entries(&(entry[i]), table)==EXIT_SUCCESS,"erase timer failed");
			ASSERT(table->timers->list.head == table->timers->list.tail, "head and tail are different");
			ASSERT(table->timers->list.num_of_timers==(num_of_entries-i-1), "wrong num of timers");
		}
		else
		{
			fprintf(stderr,"i %d entry %p hto %d ne %d \n", i, &(entry[i]), hard_timeout, num_of_entries);
			ASSERT(of12_destroy_timer_entries(&(entry[i]), table)==EXIT_SUCCESS,"erase timer failed");
			ASSERT(table->timers->list.num_of_timers==(num_of_entries-i-1), "wrong num of timers");
		}
		platform_mutex_unlock(&table->mutex);
	}
	
	//test extraction without entries
	//ASSERT(of12_destroy_timer_entries(&(entry[0]), table)==EXIT_FAILURE, "extraction when list is empty failed");
	
	fprintf(stderr,"<%s> test passed\n",__func__);
}


/**
 * random insert and time expiration
 */
void test_incremental_insert_and_expiration_dynamic(of12_pipeline_t * pipeline, of12_flow_entry_t* entry)
{
	of12_flow_table_t * table = pipeline->tables;
	of12_timer_group_t * tg_pointer;
	struct timeval now;
	int i;
	
	for(i=0; i<OF12_TIMER_GROUPS_MAX; i++)
	{
		uint32_t timeout = i+1;
		of12_fill_new_timer_entry_info(&(entry[i]),timeout,0);
		ASSERT(of12_add_timer(table, &(entry[i]))==EXIT_SUCCESS,"error add timer");
		
		if(i==0)
			tg_pointer = table->timers;
		else
			tg_pointer = tg_pointer->next;
		
		ASSERT(tg_pointer->list.head->entry == &(entry[i]),"entry address");
		
		//number of entries
	}
	
	//comprovar que els timers han sigut ben assignats
	for(i=0; i<OF12_TIMER_GROUPS_MAX; i++)
	{
		//vaig fent processes i passant el temps
		of12_time_forward(1,0,&now);
		of12_process_pipeline_tables_timeout_expirations(pipeline);
		if(i<OF12_TIMER_GROUPS_MAX-1)
			ASSERT(table->timers->list.head->entry == &(entry[i+1]), "error in deletion");
	}
	fprintf(stderr,"<%s> test passed\n",__func__);
}

/**
 * Test for Idle timers
 */
void test_simple_idle_dynamic(of12_pipeline_t * pipeline, of12_flow_entry_t * entry, uint32_t ito)
{
	of12_flow_table_t * table = pipeline->tables;
	struct timeval now;
	
	of12_fill_new_timer_entry_info(&(entry[0]),0,ito);
	
	//insert a timer
	ASSERT(of12_add_timer(table, &(entry[0]))==EXIT_SUCCESS,"error add timer");
	of12_time_forward(0,0,&now);
	fprintf(stderr,"added idle TO (%p) at time %lu:%lu for %d seconds\n", &(entry[0]), now.tv_sec, now.tv_usec, ito);
	ASSERT(table->timers->list.head->entry == &(entry[0]),"entry address");
	
	//update the counter
	of12_time_forward(ito-1,0,&now);
	of12_timer_update_entry(&(entry[0]));
	of12_process_pipeline_tables_timeout_expirations(pipeline);
	fprintf(stderr,"updated last used. TO (%p) at time %lu:%lu for %d seconds\n", &(entry[0]), now.tv_sec, now.tv_usec, ito);
	ASSERT(table->timers->list.head->entry == &(entry[0]),"entry address");
	
	//check that it is not expired but rescheduled
	of12_time_forward(1,0,&now);
	of12_process_pipeline_tables_timeout_expirations(pipeline);
	ASSERT(table->timers->list.head->entry == &(entry[0]),"entry address");
	
	//check final expiration
	of12_time_forward(ito-1,0,&now);
	of12_process_pipeline_tables_timeout_expirations(pipeline);
	ASSERT(table->timers == NULL,"entry address still there!");
	fprintf(stderr,"<%s> test passed\n",__func__);
}

void test_insert_both_expires_one_check_the_other_dynamic(of12_pipeline_t * pipeline, of12_flow_entry_t * entry, uint32_t hto, uint32_t ito)
{
	of12_flow_table_t * table = pipeline->tables;
	struct timeval now;
	of12_time_forward(0,0,&now);
	
	of12_fill_new_timer_entry_info(&(entry[0]),hto,ito);
	
	ASSERT(of12_add_timer(table, &(entry[0]))==EXIT_SUCCESS,"error add timer");
	
	if(hto==ito)
	{
		of12_time_forward(ito,0,&now);
		of12_process_pipeline_tables_timeout_expirations(pipeline);
		ASSERT(table->timers == NULL,"entry address still there!");
	}
	else
	{
		uint32_t min = (hto<ito ? hto : ito);
		uint32_t max = (hto>ito ? hto : ito);
		fprintf(stderr,"<%s:%d>hto %u ito %u min %u max %u\n",__func__,__LINE__,hto,ito,min, max);
		ASSERT(table->timers->next!=NULL,"There should be 2 entries!");
		ASSERT(table->timers->timeout==(min+now.tv_sec)*1000+now.tv_usec/1000,"TO value incorrect");
		ASSERT(table->timers->next->timeout==(max+now.tv_sec)*1000+now.tv_usec/1000,"TO value incorrect");
		of12_time_forward(min,0,&now);
		of12_process_pipeline_tables_timeout_expirations(pipeline);
		ASSERT(table->timers==NULL,"There should be no timer groups -> because both were from the same entry");
	}
	fprintf(stderr,"<%s> test passed\n",__func__);
}

#endif

static int setup_test(of12_pipeline_t* pipeline, of12_flow_table_t* table, of12_flow_entry_t ** entries)
{
	table->mutex = platform_mutex_init(NULL);
	if(table->mutex==NULL)
	{
		fprintf(stderr,"<%s:%d> Mutex init failed\n", __func__, __LINE__);
		return EXIT_FAILURE;
	}
	pipeline->tables = table;
	pipeline->num_of_tables=1;
	
	(*entries) = malloc(sizeof(of12_flow_entry_t)*OF12_TIMERS_TEST_MAX_TIMER_ENTRIES);
	
	fprintf(stderr,"address of entries %p\n",*entries);
	
	if (entries == NULL)
	{
		fprintf(stderr,"something was wrong in malloc\n");
		perror("malloc");
		return EXIT_FAILURE;
	}
	
	fprintf(stderr,"address of entries %p\n",*entries);
	
	return EXIT_SUCCESS;
}

static int clean_up(of12_flow_entry_t * entries, of12_flow_table_t* table)
{
	platform_mutex_destroy(table->mutex);
// 	if(platform_mutex_destroy(&table->mutex))
// 	{
// 		perror("mutex");
// 		return EXIT_FAILURE;
// 	}
	cutil_free(entries);
	
	return EXIT_SUCCESS;
}

//Tests
int main(int args, char** argv)
{
// 	int one=1;
// 	assert(one);
// 	fprintf(stderr,"Allright\n");
	
	/*
	 * steps:
	 * 1- create a table which needs to have the timer_group defined 
	 * (table->timers)
	 * 2- create some entries: they dont need to be full, just to have a timeout or smthg
	 * 3- add some enrteies with some timeouts and pretend the time has passed
	 * 
	 * fer dumps
	 * fer hooks per gettimeof day per example DONE
	 */
	of12_pipeline_t pipeline;
	of12_flow_table_t table;
	of12_flow_entry_t * entries=NULL;
	
	if(setup_test(&pipeline, &table, &entries)!=EXIT_SUCCESS)
	{
		fprintf(stderr,"<%s:%d> Setup failed\n", __func__, __LINE__);
		return EXIT_FAILURE;
	}
	
#if OF12_TIMER_STATIC_ALLOCATION_SLOTS
	of12_timer_group_static_init(&table);
	//fprintf(stderr,"Size allocated in memory %lu Bytes per table (%d entries)\n",sizeof(pipeline.tables[0].timers), OF12_TIMER_GROUPS_MAX);
#else
	table.timers=NULL; //WARNING can I suppose that?
#endif

	int i;
	uint32_t rnd_to, rnd_toh,rnd_entries;
	for(i=0;i<5;i++)
	{
		rnd_to = (random32()%OF12_TIMER_GROUPS_MAX)+1;
		rnd_toh = (random32()%OF12_TIMER_GROUPS_MAX)+1;
		rnd_entries = random32()%OF12_TIMERS_TEST_MAX_TIMER_ENTRIES;
		fprintf(stderr,"<%s:%d> Rnd values: ito %d hto %d n_entries %d\n", __func__, __LINE__,
				rnd_to, rnd_toh, rnd_entries);
#if OF12_TIMER_STATIC_ALLOCATION_SLOTS
		test_insert_and_expiration_static(&pipeline, rnd_to, entries);
		test_insert_and_extract_static(&pipeline, rnd_to, rnd_entries, entries);
		test_simple_idle_static(&pipeline,entries, rnd_to);
		test_insert_both_expires_one_check_the_other_static(&pipeline,entries,rnd_toh, rnd_to);
#else
		test_insert_and_extract_dynamic(&pipeline,rnd_to, rnd_entries, entries);
		test_simple_idle_dynamic(&pipeline, entries, rnd_to);
		test_insert_both_expires_one_check_the_other_dynamic(&pipeline,entries, rnd_toh, rnd_to );
#endif
	}
	

#if ! OF12_TIMER_STATIC_ALLOCATION_SLOTS
	test_incremental_insert_and_expiration_dynamic(&pipeline,entries);
#endif
	
	if(clean_up(entries, &table)!=EXIT_SUCCESS)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}