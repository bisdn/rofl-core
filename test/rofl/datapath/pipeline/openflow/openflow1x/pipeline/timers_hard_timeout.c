/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "timers_hard_timeout.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "CUnit/Basic.h"
#include "lib_assert.h"
#include "lib_random.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_match.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_group_table.h"

#include <sys/mman.h>

#define OF1X_TIMERS_TEST_MAX_TIMER_ENTRIES 10000
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

void time_forward(uint64_t sec, uint64_t usec, struct timeval * time)
{
	static uint64_t abs_sec = 0, abs_usec = 0;
	abs_sec+=sec;
	abs_usec+=usec;
	if(abs_usec > 1000000)
	{
		abs_usec-=1000000;
		abs_sec++;
	}
	if(time)
	{
		time->tv_sec=abs_sec;
		time->tv_usec=abs_usec;
	}
	return;
}


#if OF1X_TIMER_STATIC_ALLOCATION_SLOTS

void test_insert_and_expiration_static(of1x_pipeline_t * pipeline, uint32_t hard_timeout)
{
	of1x_flow_table_t* table = pipeline->tables;
	struct timeval now;
	time_forward(0,0,&now);
	of1x_flow_entry_t *single_entry = of1x_init_flow_entry(false);
	CU_ASSERT(single_entry!=NULL);
	__of1x_fill_new_timer_entry_info(single_entry,hard_timeout,0);
	CU_ASSERT(single_entry->timer_info.hard_timeout==hard_timeout);
	CU_ASSERT(of1x_add_flow_entry_table(pipeline,0, &single_entry, false, false)==ROFL_OF1X_FM_SUCCESS);
	
	//ASSERT(of1x_add_timer(table, single_entry)==EXIT_SUCCESS,"add timer failed");

	int slot = (now.tv_sec+hard_timeout)%OF1X_TIMER_GROUPS_MAX;
	//of1x_dump_timers_structure(table->timers);
	CU_ASSERT(table->timers[slot].list.num_of_timers==1);
	CU_ASSERT(table->timers[slot].list.head != NULL);//this can improve with the actual addersses
	CU_ASSERT(table->timers[slot].list.tail != NULL);
	CU_ASSERT(table->timers[slot].list.head == table->timers[slot].list.tail);
	
	time_forward(hard_timeout+1,0,&now); //we need to go to hard_timeout +1 because of possible rounding up.
	__of1x_process_pipeline_tables_timeout_expirations(pipeline);
	
	//of1x_dump_timers_structure(table->timers);
	CU_ASSERT(table->timers[slot].list.num_of_timers==0);//this can improve with the actual addersses
	CU_ASSERT(table->timers[slot].list.head == NULL);
	CU_ASSERT(table->timers[slot].list.tail == NULL);
	CU_ASSERT(table->timers[slot].list.head == table->timers[slot].list.tail);
	fprintf(stderr,"<%s> test passed\n",__func__);
}

void test_insert_and_extract_static(of1x_pipeline_t * pipeline, uint32_t hard_timeout, int num_of_entries)
{
	int i;
	of1x_flow_table_t* table = pipeline->tables;
	struct timeval now;
	time_forward(0,0,&now);
	
	of1x_flow_entry_t** entry_list = malloc(num_of_entries*sizeof(of1x_entry_timer_t*));
	int slot = (now.tv_sec+hard_timeout)%OF1X_TIMER_GROUPS_MAX; //WARNING needs to be mora accurate when MS per SLOT != 1000
	
	//adding the entries
	for(i=0; i< num_of_entries; i++)
	{
		
		of1x_flow_entry_t* tmp;
		entry_list[i] = of1x_init_flow_entry(false);
		__of1x_fill_new_timer_entry_info(entry_list[i],hard_timeout,0); 	//WARNING supposition: the entry is filled up alone
		of1x_add_match_to_entry(entry_list[i],of1x_init_port_in_match(i));
		
		//Cheat pipeline
		tmp = entry_list[i];
		of1x_add_flow_entry_table(pipeline,0, &entry_list[i], false, false);
		entry_list[i] = tmp;
		
		CU_ASSERT(table->timers[slot].list.num_of_timers==i+1);
		CU_ASSERT(table->timers[slot].list.head != NULL); //this can improve with the actual addersses
		CU_ASSERT(table->timers[slot].list.tail != NULL);
		if(i==0)
		{
			CU_ASSERT(table->timers[slot].list.head == table->timers[slot].list.tail);
		}
		else
		{
			CU_ASSERT(table->timers[slot].list.head != table->timers[slot].list.tail);
		}
	}
	
	//check pointers of entries
	i=0;
	of1x_entry_timer_t * entry_iterator=table->timers[slot].list.head;
	for(;entry_iterator->next;entry_iterator=entry_iterator->next)
	{
		i++;
	}
	CU_ASSERT(i==num_of_entries-1);
	
	//external extraction of the entries
	for(i=0; i< num_of_entries; i++)
	{
		platform_mutex_lock(table->mutex);

		if(i==num_of_entries-2)
		{
			CU_ASSERT(__of1x_destroy_timer_entries(entry_list[i])==EXIT_SUCCESS);
			CU_ASSERT(table->timers[slot].list.head == table->timers[slot].list.tail);
		}
		else if(i==num_of_entries-1)
		{
			CU_ASSERT(__of1x_destroy_timer_entries(entry_list[i])==EXIT_SUCCESS);
			CU_ASSERT(table->timers[slot].list.head == NULL);
			CU_ASSERT(table->timers[slot].list.tail == NULL);
		}
		else
		{
			//fprintf(stderr,"i %d entry %p hto %d ne %d \n", i, entry_list[i], hard_timeout, num_of_entries);
			CU_ASSERT(__of1x_destroy_timer_entries(entry_list[i])==EXIT_SUCCESS);
		}
		CU_ASSERT(table->timers[slot].list.num_of_timers==(num_of_entries-i-1));
		platform_mutex_unlock(table->mutex);
		
		of1x_remove_flow_entry_table(pipeline,0, entry_list[i], NOT_STRICT,OF1X_PORT_ANY,OF1X_GROUP_ANY);
	}
		
	free(entry_list);
	fprintf(stderr,"<%s> test passed\n",__func__);
}

/**
 * Test for Idle timers
 */
void test_simple_idle_static(of1x_pipeline_t * pipeline, uint32_t ito)
{
	of1x_flow_table_t * table = pipeline->tables;
	of1x_flow_entry_t *tmp;
	of1x_flow_entry_t *entry=of1x_init_flow_entry(false); //NOTE entry is initiated but not inserted in the table. Therefore we must extract it.??
	struct timeval now;
	//WARNING not working for slots different than seconds (1000 ms)
	int slot, i;
	__of1x_fill_new_timer_entry_info(entry,0,ito);
	
	//insert to table?
	//Cheat pipeline
	tmp = entry;	
	of1x_add_flow_entry_table(pipeline, 0, &entry,false, false);
	entry = tmp;
	
	//insert a timer
	//CU_ASSERT(of1x_add_timer(table, entry)==EXIT_SUCCESS);
	time_forward(0,0,&now);
	fprintf(stderr,"added idle TO (%p) at time %lu:%lu for %d seconds\n", entry, now.tv_sec, now.tv_usec, ito);
	slot = (now.tv_sec+ito)%OF1X_TIMER_GROUPS_MAX;
	CU_ASSERT(table->timers[slot].list.head->entry == entry);
	
	//update the counter
	time_forward(ito-1,0,&now);
	entry->stats.packet_count++; //__of1x_timer_update_entry(entry,now);
	__of1x_process_pipeline_tables_timeout_expirations(pipeline);
	fprintf(stderr,"updated last used. TO (%p) at time %lu:%lu for %d seconds\n", entry, now.tv_sec, now.tv_usec, ito);
	slot = (now.tv_sec+1)%OF1X_TIMER_GROUPS_MAX;
	CU_ASSERT(table->timers[slot].list.head->entry == entry);
	
	//check that it is not expired but rescheduled
	time_forward(1,0,&now);
	__of1x_process_pipeline_tables_timeout_expirations(pipeline);
	//slot = (now.tv_sec+ito-1)%OF1X_TIMER_GROUPS_MAX; NOTE with the lazy expiration we reschedule depending on the time of chek of the expiration
	slot = (now.tv_sec+ito)%OF1X_TIMER_GROUPS_MAX;
	CU_ASSERT(table->timers[slot].list.head->entry == entry);
	
	//check final expiration
	time_forward(ito,0,&now);
	__of1x_process_pipeline_tables_timeout_expirations(pipeline);
	for(i=0; i<OF1X_TIMER_GROUPS_MAX; i++)
			CU_ASSERT(table->timers[i].list.num_of_timers == 0);
	
	//of1x_destroy_flow_entry(entry);
	fprintf(stderr,"<%s> test passed\n",__func__);
}

void debug_how_many_entries_static(of1x_flow_table_t *table){
	unsigned int num=0, i=0;
	for(i=0; i<OF1X_TIMER_GROUPS_MAX;i++){
		if(table->timers[i].list.num_of_timers>0)
			fprintf(stderr,"<%s:%d> i=%d\n",__func__,__LINE__,i);
		num+=table->timers[i].list.num_of_timers;
	}
	fprintf(stderr,"<%s:%d> Total num of entr = %d\n",__func__,__LINE__,num);
}

void test_insert_both_expires_one_check_the_other_static(of1x_pipeline_t * pipeline, uint32_t hto, uint32_t ito)
{
	of1x_flow_table_t * table = pipeline->tables;
	struct timeval now;
	time_forward(0,0,&now);
	int slot, i;
	
	of1x_flow_entry_t *tmp;
	of1x_flow_entry_t *single_entry = of1x_init_flow_entry(false);
	__of1x_fill_new_timer_entry_info(single_entry,hto,ito);
	
	//Cheat pipeline
	tmp = single_entry;
	of1x_add_flow_entry_table(pipeline,0,&single_entry, false, false);
	single_entry = tmp;
	
	//CU_ASSERT(of1x_add_timer(table, entry)==EXIT_SUCCESS,"error add timer");
	
	if(hto==ito)
	{
		time_forward(ito,0,&now);
		__of1x_process_pipeline_tables_timeout_expirations(pipeline);
		for(i=0; i<OF1X_TIMER_GROUPS_MAX; i++)
			CU_ASSERT(table->timers[i].list.num_of_timers == 0);
	}
	else
	{
		uint32_t min = (hto<ito ? hto : ito);
		uint32_t max = (hto>ito ? hto : ito);
		fprintf(stderr,"<%s:%d>hto %u ito %u min %u max %u\n",__func__,__LINE__,hto,ito,min, max);
		slot = (now.tv_sec+ito)%OF1X_TIMER_GROUPS_MAX;
		CU_ASSERT(table->timers[slot].list.head->entry==single_entry);
		slot = (now.tv_sec+hto)%OF1X_TIMER_GROUPS_MAX;
		CU_ASSERT(table->timers[slot].list.head->entry==single_entry);
		slot = (now.tv_sec+min)%OF1X_TIMER_GROUPS_MAX;
		CU_ASSERT(table->timers[slot].timeout==(min+now.tv_sec)*1000+now.tv_usec/1000);
		slot = (now.tv_sec+max)%OF1X_TIMER_GROUPS_MAX;
		CU_ASSERT(table->timers[slot].timeout==(max+now.tv_sec)*1000+now.tv_usec/1000);
		time_forward(min,0,&now);
		__of1x_process_pipeline_tables_timeout_expirations(pipeline);
		for(i=0; i<OF1X_TIMER_GROUPS_MAX; i++)
			CU_ASSERT(table->timers[i].list.num_of_timers == 0);
	}
	fprintf(stderr,"<%s> test passed\n",__func__);
}

#else
/**
 * testing insertion and extraction with dynamic timer groups
 */
void test_insert_and_extract_dynamic(of1x_pipeline_t * pipeline, uint32_t hard_timeout, int num_of_entries)
{
	int i;
	of1x_flow_table_t* table = pipeline->tables;
	of1x_flow_entry_t** entry_list = malloc(num_of_entries*sizeof(of1x_flow_entry_t*));
	
	//adding the entries
	for(i=0; i< num_of_entries; i++)
	{
		//WARNING supposition: the entry is filled up alone
		entry_list[i] = of1x_init_flow_entry(false);
		of1x_fill_new_timer_entry_info(entry_list[i],hard_timeout,0);
		of1x_add_match_to_entry(entry_list[i],of1x_init_port_in_match(i));
		of1x_add_flow_entry_table(pipeline,0, &entry_list[i], false, false);
		//CU_ASSERT(of1x_add_timer(table, entry_list[i]));
		
		CU_ASSERT(table->timers->list.num_of_timers==i+1);
		CU_ASSERT(table->timers->list.head != NULL);
		CU_ASSERT(table->timers->list.tail != NULL);
		if(i==0)
		{
			CU_ASSERT(table->timers->list.head == table->timers->list.tail);
		}
		else
		{
			CU_ASSERT(table->timers->list.head != table->timers->list.tail);
		}
	}
	
	//check pointers of entries
	i=0;
	of1x_entry_timer_t * entry_iterator=table->timers->list.head;
	for(;entry_iterator->next;entry_iterator=entry_iterator->next)
	{
		i++;
	}
	CU_ASSERT(i==num_of_entries-1);
	
	for(i=0; i< num_of_entries; i++)
	{
		platform_mutex_lock(table->mutex);

		if(i==num_of_entries-1)
		{
			CU_ASSERT(of1x_destroy_timer_entries(entry_list[i])==EXIT_SUCCESS);
			CU_ASSERT(table->timers == NULL);
		}
		else if(i==num_of_entries-2)
		{
			assert(of1x_destroy_timer_entries(entry_list[i])==EXIT_SUCCESS);
			assert(table->timers->list.head == table->timers->list.tail);
			assert(table->timers->list.num_of_timers==(num_of_entries-i-1));
		}
		else
		{
			fprintf(stderr,"i %d entry %p hto %d ne %d \n", i, entry_list[i], hard_timeout, num_of_entries);
			CU_ASSERT(of1x_destroy_timer_entries(entry_list[i])==EXIT_SUCCESS);
			CU_ASSERT(table->timers->list.num_of_timers==(num_of_entries-i-1));
		}
		platform_mutex_unlock(table->mutex);
		CU_ASSERT( of1x_destroy_flow_entry(entry_list[i])==ROFL_SUCCESS);
	}
	
	free(entry_list);
	fprintf(stderr,"<%s> test passed\n",__func__);
}

/**
 * Test for Idle timers
 */
void test_simple_idle_dynamic(of1x_pipeline_t * pipeline, uint32_t ito)
{
	of1x_flow_table_t * table = pipeline->tables;
	struct timeval now;
	
	of1x_flow_entry_t *entry=of1x_init_flow_entry(false);
	of1x_fill_new_timer_entry_info(entry,0,ito);
	CU_ASSERT(of1x_add_flow_entry_table(pipeline,0, &entry, false, false)==ROFL_OF1X_FM_SUCCESS);
	
	//insert a timer
	//CU_ASSERT(of1x_add_timer(table, entry)==EXIT_SUCCESS);
	of1x_time_forward(0,0,&now);
	fprintf(stderr,"added idle TO (%p) at time %lu:%lu for %d seconds\n", entry, now.tv_sec, now.tv_usec, ito);
	CU_ASSERT(table->timers->list.head->entry == entry);
	
	//update the counter
	of1x_time_forward(ito-1,0,&now);
	of1x_timer_update_entry(entry);
	of1x_process_pipeline_tables_timeout_expirations(pipeline);
	fprintf(stderr,"updated last used. TO (%p) at time %lu:%lu for %d seconds\n", entry, now.tv_sec, now.tv_usec, ito);
	CU_ASSERT(table->timers->list.head->entry == entry);
	
	//check that it is not expired but rescheduled
	of1x_time_forward(1,0,&now);
	of1x_process_pipeline_tables_timeout_expirations(pipeline);
	CU_ASSERT(table->timers->list.head->entry == entry);
	
	//check final expiration
	of1x_time_forward(ito-1,0,&now);
	of1x_process_pipeline_tables_timeout_expirations(pipeline);
	CU_ASSERT(table->timers == NULL);
	
	//of1x_destroy_flow_entry(entry);
	fprintf(stderr,"<%s> test passed\n",__func__);
}

void test_insert_both_expires_one_check_the_other_dynamic(of1x_pipeline_t * pipeline, uint32_t hto, uint32_t ito)
{
	of1x_flow_table_t * table = pipeline->tables;
	struct timeval now;
	of1x_time_forward(0,0,&now);
	
	of1x_flow_entry_t *single_entry = of1x_init_flow_entry(false);
	of1x_fill_new_timer_entry_info(single_entry,hto,ito);
	of1x_add_flow_entry_table(pipeline,0, single_entry, false, false);
	
	if(hto==ito)
	{
		of1x_time_forward(ito,0,&now);
		of1x_process_pipeline_tables_timeout_expirations(pipeline);
		assert(table->timers == NULL);
	}
	else
	{
		uint32_t min = (hto<ito ? hto : ito);
		uint32_t max = (hto>ito ? hto : ito);
		fprintf(stderr,"<%s:%d>hto %u ito %u min %u max %u\n",__func__,__LINE__,hto,ito,min, max);
		assert(table->timers->next!=NULL);
		assert(table->timers->timeout==(min+now.tv_sec)*1000+now.tv_usec/1000);
		assert(table->timers->next->timeout==(max+now.tv_sec)*1000+now.tv_usec/1000);
		of1x_time_forward(min,0,&now);
		of1x_process_pipeline_tables_timeout_expirations(pipeline);
		assert(table->timers==NULL);
	}
	fprintf(stderr,"<%s> test passed\n",__func__);
}

/**
 * random insert and time expiration
 */
void test_incremental_insert_and_expiration_dynamic(of1x_pipeline_t * pipeline)
{
	of1x_flow_table_t * table = pipeline->tables;
	of1x_timer_group_t * tg_pointer;
	struct timeval now;
	int i;
	of1x_flow_entry_t* entry_list[OF1X_TIMERS_TEST_MAX_TIMER_ENTRIES];// = malloc(OF1X_TIMERS_TEST_MAX_TIMER_ENTRIES*sizeof(of1x_flow_entry_t*));
	
	for(i=0; i<OF1X_TIMERS_TEST_MAX_TIMER_ENTRIES; i++)
	{
		uint32_t timeout = i+1;
		entry_list[i] = of1x_init_flow_entry(false);
		CU_ASSERT(entry_list[i]!=NULL);
		of1x_fill_new_timer_entry_info(entry_list[i],timeout,0);
		of1x_add_match_to_entry(entry_list[i],of1x_init_port_in_match(i));
		CU_ASSERT(of1x_add_flow_entry_table(pipeline,0, &entry_list[i], false, false)==ROFL_OF1X_FM_SUCCESS);
		
		//if(i==0)
		//{
		//	CU_ASSERT(mprotect(entry_list[i]->rwlock, sizeof(pthread_rwlock_t), PROT_NONE)==0);
		//}
		
		if(i==0)
		{
			tg_pointer = table->timers;
		}
		else
		{
			tg_pointer = tg_pointer->next;
		}
		
		CU_ASSERT(tg_pointer->list.head->entry == entry_list[i]);
		
		//number of entries
	}
	
	//comprovar que els timers han sigut ben assignats
	for(i=0; i<OF1X_TIMERS_TEST_MAX_TIMER_ENTRIES; i++)
	{
		//vaig fent processes i passant el temps
		of1x_time_forward(1,0,&now);
		of1x_process_pipeline_tables_timeout_expirations(pipeline);
		if(i<OF1X_TIMERS_TEST_MAX_TIMER_ENTRIES-1)
		{
			CU_ASSERT(table->timers->list.head->entry == entry_list[i+1]);
		}
	}
	//free(entry_list);
	fprintf(stderr,"<%s> test passed\n",__func__);
}


#endif

static int setup_test(of1x_switch_t** sw)
{
	physical_switch_init();	

	enum of1x_matching_algorithm_available ma_list= of1x_loop_matching_algorithm;
	(*sw) = of1x_init_switch("Test switch",OF_VERSION_12, 0x0101,1,&ma_list);
	CU_ASSERT(sw!=NULL);
/*	
	table->mutex = platform_mutex_init(NULL);
	if(table->mutex==NULL)
	{
		fprintf(stderr,"<%s:%d> Mutex init failed\n", __func__, __LINE__);
		return EXIT_FAILURE;
	}
	pipeline->tables = table;
	pipeline.num_of_tables=1;
*/
	return EXIT_SUCCESS;
}

static int clean_up(of1x_switch_t * sw)
{
	//platform_mutex_destroy(table->mutex);
	__of1x_destroy_switch(sw);
	
	return EXIT_SUCCESS;
}

//Tests
void main_test(void)
{	
	/*
	 * steps:
	 * 1- create a table which needs to have the timer_group defined 
	 * (table->timers)
	 * 2- create some entries: they dont need to be full, just to have a timeout or smthg
	 * 3- add some enrteies with some timeouts and pretend the time has passed
	 */
	of1x_switch_t *sw=NULL;
	CU_ASSERT(setup_test(&sw)==EXIT_SUCCESS);
	
	int i;
	uint32_t rnd_to, rnd_toh,rnd_entries;
	for(i=0;i<1;i++)
	{
		rnd_to = (random32()%OF1X_TIMER_GROUPS_MAX)+1;
		rnd_toh = (random32()%OF1X_TIMER_GROUPS_MAX)+1;
		rnd_entries = random32()%OF1X_TIMERS_TEST_MAX_TIMER_ENTRIES;
		fprintf(stderr,"<%s:%d> Rnd values: ito %d hto %d n_entries %d\n", __func__, __LINE__,
				rnd_to, rnd_toh, rnd_entries);
#if OF1X_TIMER_STATIC_ALLOCATION_SLOTS
		test_insert_and_expiration_static(&sw->pipeline, rnd_to);
		test_insert_and_extract_static(&sw->pipeline, rnd_to, rnd_entries);
		test_simple_idle_static(&sw->pipeline, rnd_to);
		test_insert_both_expires_one_check_the_other_static(&sw->pipeline,rnd_toh, rnd_to);
#else
		test_insert_and_extract_dynamic(&sw->pipeline,rnd_to, 2/*rnd_entries*/);
		test_simple_idle_dynamic(&sw->pipeline, rnd_to);
		test_insert_both_expires_one_check_the_other_dynamic(&sw->pipeline, rnd_toh, rnd_to );
#endif
	}
	

#if ! OF1X_TIMER_STATIC_ALLOCATION_SLOTS
	test_incremental_insert_and_expiration_dynamic(sw->pipeline);
#endif
	
	CU_ASSERT(clean_up(sw)==EXIT_SUCCESS);
	
}
