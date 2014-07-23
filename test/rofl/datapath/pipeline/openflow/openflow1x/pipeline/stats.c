/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#include "lib_assert.h"
#include "lib_random.h"

#include "../../../../../../src/rofl/pipeline/openflow/openflow1x/of1x_switch.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow1x/pipeline/of1x_flow_table.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow1x/pipeline/of1x_timers.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow1x/pipeline/of1x_statistics.h"
//#include "../../../../../../src/rofl/pipeline/platform/cutil.h"
//#include "../../../../../../src/rofl/pipeline/platform/lock.h"

#define STATS_TEST_NUM_OF_CORES 32
#define STATS_TEST_MAX_ENTRIES 100000
#define STATS_TEST_MAX_PACKETS 5000
#define STATS_TEST_PACKET_SIZE 1477

//21-Nov-2012 11:38
#define STATS_TEST_SEC 1353494293 
#define STATS_TEST_USEC 940861

typedef struct
{
	of1x_pipeline_t* pipe;
	of1x_flow_entry_t* entries;
	of1x_stats_port_t *port_stats;
	of1x_stats_queue_t *queue_stats;
	int core_number;
} core_args_t;

void * core_main (void * args)
{
	core_args_t *info = (core_args_t *) args;
	fprintf(stderr,"I am core nº %d\n",info->core_number);
	
	int i;
	uint64_t num_bytes = STATS_TEST_PACKET_SIZE;
	for(i=0;i<STATS_TEST_MAX_PACKETS;i++)
	{
		//flow
		of1x_stats_flow_inc(&(info->entries[0]), num_bytes);
		of1x_stats_flow_duration_update(&(info->entries[0]));
		of1x_stats_flow_get_duration(&info->entries[0]);
		
		//table
		of1x_stats_table_lookup_inc(info->pipe->tables);
		if(info->core_number%2) of1x_stats_table_matches_inc(info->pipe->tables);
		
		//aggregate
		
		//port
		of1x_stats_port_rx_packet_inc(info->port_stats, num_bytes);
		of1x_stats_port_tx_packet_inc(info->port_stats, num_bytes);
		
		//queue
		of1x_stats_queue_tx_packet_inc(info->queue_stats, num_bytes);
		of1x_stats_queue_tx_errors_inc(info->queue_stats);
	}
	
	return NULL;
}

void setup(of1x_pipeline_t * pipeline, of1x_flow_table_t * table, of1x_flow_entry_t ** entries, of1x_stats_port_t *port_stats, of1x_stats_queue_t *queue_stats)//, platform_mutex_t *mutex_time)
{
	pipeline.tables = table;
	(*entries) = malloc(sizeof(of1x_flow_entry_t)*STATS_TEST_MAX_ENTRIES);
	if((*entries)==NULL)
	{
		fprintf(stderr,"<%s:%d> Malloc failed\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
	}
	
	of1x_time_forward(STATS_TEST_SEC, STATS_TEST_USEC, NULL);
	//platform_mutex_init(mutex_time, NULL);
	table->mutex = platform_mutex_init(NULL);
	of1x_stats_flow_init(entries[0]);
	of1x_stats_table_init(pipeline.tables);
	of1x_stats_port_init(port_stats);
	of1x_stats_queue_init(queue_stats);
	
}

void clean_up(of1x_flow_table_t* table, of1x_flow_entry_t ** entries, of1x_stats_port_t *port_stats, of1x_stats_queue_t *queue_stats)//, platform_mutex_t *mutex_time)
{
	of1x_stats_flow_destroy(entries[0]);
	//platform_mutex_destroy(mutex_time);
	platform_mutex_destroy(table->mutex);
	of1x_stats_port_destroy(port_stats);
	of1x_stats_queue_destroy(queue_stats);
	free((*entries));
}

int main(int args, char** argv)
{
	of1x_pipeline_t pipeline;
	of1x_flow_table_t table;
	of1x_flow_entry_t* entries;
	of1x_stats_port_t port_stats;
	of1x_stats_queue_t queue_stats;
	pthread_t cores[STATS_TEST_NUM_OF_CORES];
	core_args_t info[STATS_TEST_NUM_OF_CORES];
	//platform_mutex_t mutex_time;
	struct timeval now, end;
	int i, rand_sec, rand_usec, total_sec=0, total_usec=0;
	
	setup(&pipeline, &table, &entries, &port_stats, &queue_stats);//, &mutex_time);
	
	of1x_gettimeofday(&now,NULL);
	fprintf(stderr,"<%s:%d> Init time %lu:%lu\n", __func__,__LINE__,now.tv_sec, now.tv_usec);
	
	
	
	for(i=0;i<STATS_TEST_NUM_OF_CORES;i++)
	{
		rand_sec = rand() % 500;
		rand_usec = rand() % 1000000;
		total_sec+=rand_sec;
		total_usec+=rand_usec;
		if(total_usec > 1000000)
		{
			total_usec-=1000000;
			total_sec++;
		}
		of1x_time_forward(rand_sec,rand_usec,NULL);
		
		info[i].pipe = &pipeline;
		info[i].entries = entries;
		info[i].port_stats = &port_stats;
		info[i].queue_stats = &queue_stats;
		info[i].core_number = i;
		pthread_create(&(cores[i]),NULL, core_main, &info[i]);
	}
	
	for(i=0;i<STATS_TEST_NUM_OF_CORES;i++)
	{
		pthread_join(cores[i],NULL);
	}
	
	fprintf(stderr,"<%s:%d> table stats: LU:%lu M:%lu \n",__func__, __LINE__, table.stats.lookup_count, table.stats.matched_count);
	fprintf(stderr,"<%s:%d> n_packets %lu (%lu bytes)\n",__func__,__LINE__,entries[0].stats.packet_count,entries[0].stats.byte_count);
	fprintf(stderr,"<%s:%d> duration of entry %u:%u \n",__func__,__LINE__,entries[0].stats.duration_sec,entries[0].stats.duration_nsec);
	
	of1x_gettimeofday(&end,NULL);
	fprintf(stderr,"<%s:%d> Init time %lu:%lu (%lu,%lu)\n", __func__,__LINE__,end.tv_sec, end.tv_usec, end.tv_sec-now.tv_sec, end.tv_usec-now.tv_usec);
	
	ASSERT(entries[0].stats.packet_count==STATS_TEST_NUM_OF_CORES*STATS_TEST_MAX_PACKETS,"Total packets error");
	ASSERT(entries[0].stats.byte_count == STATS_TEST_NUM_OF_CORES*STATS_TEST_MAX_PACKETS*STATS_TEST_PACKET_SIZE, "Total bytes error");
	//TODO ASSERT flow duration!
	ASSERT(entries[0].stats.duration_sec == total_sec,"Wrong total seconds");
	ASSERT(entries[0].stats.duration_nsec == total_usec*1000,"Wrong total nano-seconds");
	
	//table
	ASSERT(pipeline.tables->stats.lookup_count==STATS_TEST_NUM_OF_CORES*STATS_TEST_MAX_PACKETS,"Wrong lookup_count in table");
	ASSERT(pipeline.tables->stats.matched_count==STATS_TEST_NUM_OF_CORES*STATS_TEST_MAX_PACKETS/2,"Wrong matched_count in table");
	
	//port
	ASSERT(port_stats.rx_packets == STATS_TEST_MAX_PACKETS*STATS_TEST_NUM_OF_CORES,"wrong packets in port");
	ASSERT(port_stats.rx_bytes == STATS_TEST_MAX_PACKETS*STATS_TEST_NUM_OF_CORES*STATS_TEST_PACKET_SIZE,"wrong bytes in port");
	ASSERT(port_stats.tx_packets == STATS_TEST_MAX_PACKETS*STATS_TEST_NUM_OF_CORES,"wrong packets in port");
	ASSERT(port_stats.tx_bytes == STATS_TEST_MAX_PACKETS*STATS_TEST_NUM_OF_CORES*STATS_TEST_PACKET_SIZE,"wrong bytes in port");
	
	//queue
	ASSERT(queue_stats.tx_packets == STATS_TEST_MAX_PACKETS*STATS_TEST_NUM_OF_CORES, "wrong queue tx_packets");
	ASSERT(queue_stats.tx_bytes == STATS_TEST_MAX_PACKETS*STATS_TEST_NUM_OF_CORES*STATS_TEST_PACKET_SIZE, "wrong queue byte_count");
	ASSERT(queue_stats.tx_errors == STATS_TEST_MAX_PACKETS*STATS_TEST_NUM_OF_CORES, "wrong queue errors");
	
	clean_up(&table,&entries, &port_stats, &queue_stats);//, &mutex_time);
	fprintf(stderr,"<%s:%d> Test for statistics passed\n",__func__,__LINE__);
	return EXIT_SUCCESS;
}
