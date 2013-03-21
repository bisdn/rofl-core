#ifndef __OF12_FLOW_TABLEH__
#define __OF12_FLOW_TABLEH__

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdint.h>
#include "rofl.h"
#include "../../../util/rofl_pipeline_utils.h"
#include "../../../platform/lock.h"
#include "of12_flow_entry.h"
#include "of12_timers.h"
#include "of12_statistics.h"
#include "of12_utils.h"
#include "matching_algorithms/matching_algorithms.h"


/*
* Flow table abstraction
*/

#define OF12_FIRST_FLOW_TABLE_INDEX 0 //As per 1.2 spec
#define OF12_MAX_NUMBER_OF_TABLE_ENTRIES 0xFFFFFFFF
#define OF12_MAX_TABLE_NAME_LEN 32

//fwd decl
struct of12_timer_group;
struct of12_pipeline;

//Agnostic auxiliary matching structures. 
typedef void matching_auxiliary_t;

//Table behaviour (ofp_table_config)
typedef enum{
	OF12_TABLE_MISS_CONTROLLER = 0,      /* Send to controller. */
	OF12_TABLE_MISS_CONTINUE   = 1 << 0, /* Continue to the next table in the */
	OF12_TABLE_MISS_DROP       = 1 << 1, /* Drop the packet. */
	OF12_TABLE_MISS_MASK       = 3
}of12_flow_table_miss_config_t; 

typedef struct{
	//Configuration stuff
	uint64_t match;		 	/* Bitmap of (1 << OF12_MATCH_*) that indicate the	fields the table can match on. */
	uint64_t wildcards;      	/* Bitmap of (1 << OF12_MATCH_*) wildcards that are supported by the table. */
	uint32_t write_actions;  	/* Bitmap of OFPAT_* that are supported by the table with OFPIT_WRITE_ACTIONS. */
	uint32_t apply_actions;  	/* Bitmap of OFPAT_* that are supported by the table with OFPIT_APPLY_ACTIONS. */
	uint64_t write_setfields;	/* Bitmap of (1 << OF12_MATCH_*) header fields that can be set with OFPIT_WRITE_ACTIONS. */
	uint64_t apply_setfields;	/* Bitmap of (1 << OF12_MATCH_*) header fields that can be set with OFPIT_APPLY_ACTIONS. */
	uint64_t metadata_match; 	/* Bits of metadata table can match. */
	uint64_t metadata_write; 	/* Bits of metadata table can write. */
	uint32_t instructions;	 	/* Bitmap of OF12_IT_* values supported. */
	uint32_t table_miss_config;	/* Bitmap of OF12_TABLE_MISS_* values */
}of12_flow_table_config_t;


/**
 * Main table abstraction
 */
typedef struct of12_flow_table{

	//Table number
	unsigned int number;

	//Table name
	char name[OF12_MAX_TABLE_NAME_LEN];
	
	//List of flow_entry_t pointers	
	of12_flow_entry_t* entries;
	unsigned int num_of_entries;
	unsigned int max_entries;    	/* Max number of entries supported. */

	//Timers associated
#if OF12_TIMER_STATIC_ALLOCATION_SLOTS
	unsigned int current_timer_group; /*in case of static allocation indicates the timer group*/
	struct of12_timer_group timers[OF12_TIMER_GROUPS_MAX];
#else
	struct of12_timer_group* timers;
#endif
	
	//Table config
	of12_flow_table_miss_config_t default_action; 
	of12_flow_table_config_t config;

	//statistics
	of12_stats_table_t stats;
	
	//Place-holder for auxiliary algorithm state 
	matching_auxiliary_t* matching_aux[2];

	//Mutexes
	platform_mutex_t* mutex; //Mutual exclusion among insertion/deletion threads
	platform_rwlock_t* rwlock; //Readers mutex

	//Reference back
	struct of12_pipeline* pipeline;

	/* 
	* Matching algorithm related function pointers. Matching algorithm should implement them. 
	*/
	struct matching_algorithm_functions maf;

}of12_flow_table_t;

/*
*
* Function prototypes
*
*/

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/*
* Table init and destroy
*/
rofl_result_t of12_init_table(struct of12_pipeline* pipeline, of12_flow_table_t* table, const unsigned int table_index, const enum matching_algorithm_available algorithm);
rofl_result_t of12_destroy_table(of12_flow_table_t* table);

/*
* Flow-mod installation and removal
*/
rofl_of12_fm_result_t of12_add_flow_entry_table(struct of12_pipeline *const pipeline, const unsigned int table_id, of12_flow_entry_t *const entry, bool check_overlap, bool reset_counts);

rofl_result_t of12_modify_flow_entry_table(struct of12_pipeline *const pipeline, const unsigned int table_id, of12_flow_entry_t *const entry, const enum of12_flow_removal_strictness strict, bool reset_counts);
	
rofl_result_t of12_remove_flow_entry_table(struct of12_pipeline *const pipeline, const unsigned int table_id, of12_flow_entry_t* entry, const enum of12_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group);

//This API call is meant to ONLY be used internally within the pipeline library (timers)
rofl_result_t of12_remove_specific_flow_entry_table(struct of12_pipeline *const pipeline, const unsigned int table_id, of12_flow_entry_t *const specific_entry, of12_flow_remove_reason_t reason, of12_mutex_acquisition_required_t mutex_acquired);

/*
* Entry lookup
*/ 
of12_flow_entry_t* of12_find_best_match_table(of12_flow_table_t *const table, of12_packet_matches_t *const pkt); 

/*
* Table dumping
*/
void of12_dump_table(of12_flow_table_t* table);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_FLOW_TABLE
