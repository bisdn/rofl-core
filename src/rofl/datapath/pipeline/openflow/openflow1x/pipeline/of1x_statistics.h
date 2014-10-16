/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_STATISTICS_H__
#define __OF1X_STATISTICS_H__

#include <inttypes.h>
#include <sys/time.h>
#include <string.h>
#include "rofl.h"
#include "of1x_group_types.h"
#include "../../../platform/lock.h"

#define OF1X_STATS_NS_IN_A_SEC 1000000000

/**
* @file of1x_statistics.h
* @author Victor Alvarez<victor.alvarez (at) bisdn.de>, Marc Sune<marc.sune (at) bisdn.de>
* @brief OpenFlow v1.0, 1.2 and 1.3.2 statistics subsystem 
*/


/*From OpenFlow 1.2 specification, page 50*/
/*
 * Types of statistics:
 * DESC, FLOW, AGGREGATE, TABLE, PORT, QUEUE, GROUP, GROUP_DESC, GROUP_FEATURES, EXPERIMENTER
 */

/*counters OF 1.2 specification page 13*/

//fwd declarations
struct of1x_action_group;
struct of1x_instruction_group;
struct of1x_flow_entry;
struct of1x_flow_table;
struct of1x_match;
struct of1x_match_group;
struct of1x_pipeline;

//
// Inner pipeline stats
//

/* Flows */
//Per thread flow stats
typedef struct __of1x_stats_flow_tid{
	uint64_t packet_count;
	uint64_t byte_count;
}__of1x_stats_flow_tid_t;

/**
* Flow entry stats
*/
typedef struct of1x_stats_flow{

	union __of1x_stats_flow_tids{	
		__of1x_stats_flow_tid_t counters;
		
		//array of counters per thread to be used internally
		__of1x_stats_flow_tid_t __internal[ROFL_PIPELINE_MAX_TIDS];
	}s;

	//And more not so interesting
	struct timeval initial_time;

	platform_mutex_t* mutex; //Mutual exclusion stats
}of1x_stats_flow_t;

/* Table */

//Per thread table stats
typedef struct __of1x_stats_table_tid{
	uint64_t lookup_count; /* Number of packets looked up in table. */
	uint64_t matched_count; /* Number of packets that hit table. */
}__of1x_stats_table_tid_t;

/**
* Table stats
*/
typedef struct of1x_stats_table{

	union __of1x_stats_table_tids{ 
		/* Flow table counters */
		__of1x_stats_table_tid_t counters;
	
		//array of counters per thread to be used internally
		__of1x_stats_table_tid_t __internal[ROFL_PIPELINE_MAX_TIDS];	
	}s;

	platform_mutex_t* mutex; //Mutual exclusion only for stats
}of1x_stats_table_t;

/* Groups */

//Bucket
typedef struct __of1x_stats_bucket_tid{
	uint64_t packet_count;
	uint64_t byte_count;
}__of1x_stats_bucket_tid_t;

typedef __of1x_stats_bucket_tid_t of1x_stats_bucket_t; //Used only for msgs

/**
* Bucket stats
*/
typedef struct __of1x_stats_bucket{
	union __of1x_stats_bucket_tids{ 
		/* Bucket counters */
		__of1x_stats_bucket_tid_t counters;
	
		//array of counters per thread to be used internally
		__of1x_stats_bucket_tid_t __internal[ROFL_PIPELINE_MAX_TIDS];	
	}s;

	platform_mutex_t* mutex;
}__of1x_stats_bucket_t;

//Per group table stats
typedef struct __of1x_stats_group_tid{
	uint64_t packet_count;
	uint64_t byte_count;
}__of1x_stats_group_tid_t;

/**
* Group stats
*/
typedef struct of1x_stats_group{
	
	uint32_t ref_count;
	
	union __of1x_stats_group_tids{ 
		/* Group counters */
		__of1x_stats_group_tid_t counters;
	
		//array of counters per thread to be used internally
		__of1x_stats_group_tid_t __internal[ROFL_PIPELINE_MAX_TIDS];	
	}s;

	platform_mutex_t* mutex;
}of1x_stats_group_t;


/* Meters */

/**
* Meter band stats 
*/
typedef struct of1x_stats_meter_band {
	//Packet count
	uint64_t packet_band_count;
	//Byte count
	uint64_t byte_band_count;
}of1x_stats_meter_band_t;

//Per tid stats
typedef struct __of1x_stats_meter_tid {
	uint32_t flow_count;
	uint64_t packet_count;
	uint64_t byte_count;
}__of1x_stats_meter_tid_t;

/**
* Meter stats
*/
typedef struct of1x_stats_meter {

	union __of1x_stats_meter_tids{ 
		/* Bucket counters */
		__of1x_stats_meter_tid_t counters;
	
		//array of counters per thread to be used internally
		__of1x_stats_meter_tid_t __internal[ROFL_PIPELINE_MAX_TIDS];	
	}s;

	//And more not so interesting
	struct timeval initial_time;

	//Mutex
	platform_mutex_t* mutex;
}of1x_stats_meter_t;

//
// Flow stats / Group stats message section
//

/**
* @ingroup core_of1x 
* Single flow entry stats message
*/
typedef struct of1x_stats_single_flow_msg{
	uint8_t table_id;
	uint16_t priority;
	uint64_t cookie;
	
	uint32_t duration_sec;
	uint32_t duration_nsec;
	
	uint16_t idle_timeout;
	uint16_t hard_timeout;
	
	uint16_t flags;

	uint64_t packet_count;
	uint64_t byte_count;
	
	struct of1x_match* matches;
	struct of1x_instruction_group* inst_grp;
	
	struct of1x_stats_single_flow_msg* next;
}of1x_stats_single_flow_msg_t;

/**
* @ingroup core_of1x 
* Linked list containing all the individual flow stats 
*/
typedef struct of1x_stats_flow_msg{
	uint32_t 			num_of_entries; 
	of1x_stats_single_flow_msg_t* 	flows_head;
	of1x_stats_single_flow_msg_t* 	flows_tail;
}of1x_stats_flow_msg_t;

/**
* @ingroup core_of1x 
* Aggregated flow stats message 
*/
typedef struct of1x_stats_flow_aggregate_msg{
	uint64_t packet_count;
	uint64_t byte_count;
	uint32_t flow_count;
}of1x_stats_flow_aggregate_msg_t;


typedef struct of1x_stats_group_msg{
	uint32_t group_id;
	uint32_t ref_count;
	uint64_t packet_count;
	uint64_t byte_count;
	int num_of_buckets;/*needed?*/
	of1x_stats_bucket_t *bucket_stats;
	struct of1x_stats_group_msg *next;
} of1x_stats_group_msg_t;

typedef struct of1x_stats_bucket_desc_msg{
	uint16_t weight;
	uint32_t port;
	uint32_t group;
	struct of1x_action_group *actions;
	struct of1x_stats_bucket_desc_msg *next;
}of1x_stats_bucket_desc_msg_t;

typedef struct of1x_stats_group_desc_msg{
	uint32_t group_id;
	of1x_group_type_t type;
	of1x_stats_bucket_desc_msg_t *bucket;
	struct of1x_stats_group_desc_msg *next;
}of1x_stats_group_desc_msg_t;

/** operations in statistics.c **/

ROFL_BEGIN_DECLS

void __of1x_init_flow_stats(struct of1x_flow_entry * entry);
void __of1x_destroy_flow_stats(struct of1x_flow_entry * entry);

//msgs
void __of1x_push_single_flow_stats_to_msg(of1x_stats_flow_msg_t* msg, of1x_stats_single_flow_msg_t* sfs);
of1x_stats_single_flow_msg_t* __of1x_init_stats_single_flow_msg(struct of1x_flow_entry* entry);
void __of1x_destroy_stats_single_flow_msg(of1x_stats_single_flow_msg_t* msg);

of1x_stats_flow_msg_t* __of1x_init_stats_flow_msg(void);
/**
* @ingroup core_of1x 
* Destroy a flow_stats message
*/
void of1x_destroy_stats_flow_msg(of1x_stats_flow_msg_t* msg);

//Push to msg

//Aggregate messages
of1x_stats_flow_aggregate_msg_t* __of1x_init_stats_flow_aggregate_msg(void);
/**
* @ingroup core_of1x 
* Destroy aggreagated flow_stats message
*/
void of1x_destroy_stats_flow_aggregate_msg(of1x_stats_flow_aggregate_msg_t* msg);

/**
* @ingroup core_of1x 
* Get the duration of the flow_entry 
*/
void of1x_stats_flow_get_duration(struct of1x_flow_entry * entry, uint32_t* sec, uint32_t* nsec);

void __of1x_stats_flow_reset_counts(struct of1x_flow_entry * entry);

static inline void __of1x_stats_flow_consolidate(of1x_stats_flow_t* stats, __of1x_stats_flow_tid_t* c){
	int i;
	c->byte_count = c->packet_count = 0x0ULL;
	
	for(i=0;i<ROFL_PIPELINE_MAX_TIDS;i++){
		c->packet_count += stats->s.__internal[i].packet_count;
		c->byte_count += stats->s.__internal[i].byte_count;
	}
}
static inline void __of1x_stats_copy_flow_stats(of1x_stats_flow_t* origin, of1x_stats_flow_t* copy){
	memcpy(&copy->s.__internal,&origin->s.__internal, sizeof(__of1x_stats_flow_tid_t)*ROFL_PIPELINE_MAX_TIDS); 
	copy->initial_time = origin->initial_time;
}

void __of1x_stats_table_init(struct of1x_flow_table * table);
void __of1x_stats_table_destroy(struct of1x_flow_table * table);

static inline void __of1x_stats_table_consolidate(of1x_stats_table_t* stats, __of1x_stats_table_tid_t* c){
	int i;
	c->lookup_count = c->matched_count = 0x0ULL;
	
	for(i=0;i<ROFL_PIPELINE_MAX_TIDS;i++){
		c->lookup_count += stats->s.__internal[i].lookup_count;
		c->matched_count += stats->s.__internal[i].matched_count;
	}
}

void __of1x_init_group_stats(of1x_stats_group_t *group_stats);
void __of1x_destroy_group_stats(of1x_stats_group_t* group_stats);

static inline void __of1x_stats_group_consolidate(of1x_stats_group_t* stats, __of1x_stats_group_tid_t* c){
	int i;
	c->byte_count = c->packet_count = 0x0ULL;
	
	for(i=0;i<ROFL_PIPELINE_MAX_TIDS;i++){
		c->packet_count += stats->s.__internal[i].packet_count;
		c->byte_count += stats->s.__internal[i].byte_count;
	}
}

static inline void __of1x_stats_bucket_consolidate(__of1x_stats_bucket_t* stats, __of1x_stats_bucket_tid_t* c){
	int i;
	c->byte_count = c->packet_count = 0x0ULL;
	
	for(i=0;i<ROFL_PIPELINE_MAX_TIDS;i++){
		c->packet_count += stats->s.__internal[i].packet_count;
		c->byte_count += stats->s.__internal[i].byte_count;
	}
}

void __of1x_stats_group_inc_reference(of1x_stats_group_t *gr_stats);
void __of1x_stats_group_dec_reference(of1x_stats_group_t *gr_stats);

/**
 * @brief Retrieves the statistics of a group
 * @ingroup core_of1x
 * 
 * Returns a structure with the statistics that needs to be freed by the calling element
 */
of1x_stats_group_msg_t* of1x_get_group_stats(struct of1x_pipeline* pipeline,uint32_t id);

/**
 * @brief Frees memory of statistics message
 * @ingroup core_of1x
 *
 * Needs to be called after retrieving the statistics of the groups
 */
void of1x_destroy_stats_group_msg(of1x_stats_group_msg_t *msg);

void __of1x_init_bucket_stats(__of1x_stats_bucket_t *bc_stats);
void __of1x_destroy_buckets_stats(__of1x_stats_bucket_t *bc_stats);

/*
* External interfaces
*/
/**
* @ingroup core_of1x 
* Retrieves individual flow stats 
* @return of1x_stats_flow_msg_t instance that must be destroyed using of1x_destroy_stats_flow_msg() 
*/
of1x_stats_flow_msg_t* of1x_get_flow_stats(struct of1x_pipeline* pipeline, uint8_t table_id, uint32_t cookie, uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, struct of1x_match_group* matchs);

/**
* @ingroup core_of1x 
* Retrieves aggregated flow stats 
* @return of1x_stats_flow_aggregate_msg_t instance that must be destroyed using of1x_destroy_stats_flow_aggregate_msg() 
*/
of1x_stats_flow_aggregate_msg_t* of1x_get_flow_aggregate_stats(struct of1x_pipeline* pipeline, uint8_t table_id, uint32_t cookie, uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, struct of1x_match_group* matchs);

/**
 * @ingroup core_of1x
 * Frees the memory for a of1x_stats_group_desc_msg_t structure
 */
void of1x_destroy_group_desc_stats(of1x_stats_group_desc_msg_t *msg);

/**
 * @ingroup core_of1x
 * Retrieves a copy of the group and bucket structure
 * @return of1x_stats_group_desc_msg_t instance that must be destroyed using of1x_destroy_group_desc_stats()
 */
of1x_stats_group_desc_msg_t *of1x_get_group_desc_stats(struct of1x_pipeline* pipeline);

ROFL_END_DECLS

#endif

