/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_FLOW_TABLEH__
#define __OF1X_FLOW_TABLEH__

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdint.h>
#include "rofl.h"
#include "../../../common/bitmap.h"
#include "../../../threading.h"
#include "of1x_flow_entry.h"
#include "of1x_timers.h"
#include "of1x_statistics.h"
#include "of1x_utils.h"
#include "matching_algorithms/matching_algorithms.h"

/**
* @file of1x_flow_table.h
* @author Marc Sune<marc.sune (at) bisdn.de>, Tobias Jungel<tobias.jungel (at) bisdn.de>  
*
* @brief OpenFlow v1.0, 1.2 and 1.3.2 flow table abstraction
*
* The of1x_flow_table structure contains the main abstraction of the
* a flow entry table (or a flowmod table).
*
* The basic operations which one can perform over a table are:
*  - Add a flow entry 
*  - Update flow entries
*  - Remove flow entries
*
* Additionally the table contain the pointer's to the matching algorithm using
* in this particular table. This is set during switch bootstrap and cannot be
* changed at runtime.
*
* The matching algorithm, in conjunction with the state of the table can perform
* a packet lookup operation. The API is defined in the data struct matching_algorithm_functions
* Just have a look to the matching algorithm subsystem for more information
*
*/

#define OF1X_FIRST_FLOW_TABLE_INDEX 0 //As per 1.2 spec
#define OF1X_MAX_NUMBER_OF_TABLE_ENTRIES 0xFFFFFFFF
#define OF1X_MAX_TABLE_NAME_LEN 32

//fwd decl
struct of1x_timer_group;
struct of1x_pipeline;

//Agnostic auxiliary matching structures. 
typedef void matching_auxiliary_t;

/**
* Table miss behaviour (ofp_table_config)
*/
typedef enum{
	OF1X_TABLE_MISS_CONTROLLER = 0,      /* Send to controller. */
	OF1X_TABLE_MISS_CONTINUE   = 1 << 0, /* Continue to the next table in the */
	OF1X_TABLE_MISS_DROP       = 1 << 1, /* Drop the packet. */
	OF1X_TABLE_MISS_MASK       = 3
}of1x_flow_table_miss_config_t; 

/**
* Table configuration
*/
typedef struct{
	//Configuration stuff
	bitmap128_t match;		 	/* Bitmap of (1 << OF1X_MATCH_*) that indicate the fields the table can match on. */
	bitmap128_t wildcards;			/* Bitmap of (1 << OF1X_MATCH_*) wildcards that are supported by the table. */
	bitmap128_t write_actions;		/* Bitmap of (1 << OF1X_AT_* that are supported by the table with OFPIT_WRITE_ACTIONS. */
	bitmap128_t apply_actions;		/* Bitmap of (1 << OF1X_AT_* that are supported by the table with OFPIT_APPLY_ACTIONS. */
	bitmap64_t metadata_match; 		/* Bits of metadata table can match. */
	bitmap64_t metadata_write;		/* Bits of metadata table can write. */
	bitmap32_t instructions;	 	/* Bitmap of OF1X_IT_* values supported. */
	bitmap32_t table_miss_config;		/* Bitmap of OF1X_TABLE_MISS_* values */
}of1x_flow_table_config_t;


/**
 * OpenFlow v1.0, 1.2 and 1.3.2 flow table abstraction
 */
typedef struct of1x_flow_table{

	//Table number
	unsigned int number;

	//Table name
	char name[OF1X_MAX_TABLE_NAME_LEN];
	
	/**
	* This pointer may or may not be used depending
	* on the matching algorithm. If used, it points
	* to the first entry of the table 
	*/
	of1x_flow_entry_t* entries;
	unsigned int num_of_entries;
	unsigned int max_entries;    	/* Max number of entries supported. */

	//Timers associated
#if OF1X_TIMER_STATIC_ALLOCATION_SLOTS
	unsigned int current_timer_group; /*in case of static allocation indicates the timer group*/
#endif
	struct of1x_timer_group* timers;
	
	//Table config
	of1x_flow_table_miss_config_t default_action; 
	of1x_flow_table_config_t config;

	//statistics
	of1x_stats_table_t stats;
	
	/**
	* Place-holder to allow matching algorithms
	* keep its own state
	*/
	matching_auxiliary_t* matching_aux[2];

#ifdef ROFL_PIPELINE_LOCKLESS
	tid_presence_t tid_presence_mask;
#endif 

	//Mutexes
	platform_mutex_t* mutex; //Mutual exclusion among insertion/deletion threads
	platform_rwlock_t* rwlock; //Readers mutex

	//Reference back
	struct of1x_pipeline* pipeline;

	/* 
	* Matching algorithm identifier 
	*/
	enum of1x_matching_algorithm_available matching_algorithm;

}of1x_flow_table_t;

/**
* Table snapshot
*/
typedef of1x_flow_table_t __of1x_flow_table_snapshot_t;

/*
*
* Function prototypes
*
*/

//C++ extern C
ROFL_BEGIN_DECLS

/*
* Table init and destroy
*/
rofl_result_t __of1x_init_table(struct of1x_pipeline* pipeline, of1x_flow_table_t* table, const unsigned int table_index, const enum of1x_matching_algorithm_available algorithm);

//Set defaults for OF1.0, 1.2 and 1.3
void __of10_set_table_defaults(of1x_flow_table_t* table);
void __of12_set_table_defaults(of1x_flow_table_t* table);
void __of13_set_table_defaults(of1x_flow_table_t* table);

rofl_result_t __of1x_destroy_table(of1x_flow_table_t* table);

/*
* Flow-mod installation, modify and removal
*/

/**
* @ingroup core_of1x 
* Add a flow_entry to a table.
*
* This method will add a flow_entry to the table. The flow entry shall already
* be initialized via of1x_init_flow_entry, and must already contain the matches,
* instructions and actions.
*
* When check_overlap is enabled, addition will fail if there is at least one entry
* which may potentally match the same packet, and this entry has the same priority.
*
* If (and only if) the mod operation is successful (ROFL_OF1X_FM_SUCCESS) the contents of the pointer *entry are set to NULL. Any other
* reference to the real entry (**entry) shall never be further used.
*
* On success, the library will instantiate the necessary state to handle timers and
* statistics.
* 
* @param pipeline Switch pipeline
* @param table_id Table index
* @param entry of1x_flow_entry_t previously initialized with of1x_init_flow_entry() 
* @param check_overlap Do not install if there are overlapping entries (would match the same packet)
* @param reset_counts If overlap flag is false, reset the counters on entry overwrite 
* @warning On success (ROFL_SUCCESS), the entry pointer (*entry) will be set to NULL. 
* or freed from outside the library.
*/
rofl_of1x_fm_result_t of1x_add_flow_entry_table(struct of1x_pipeline *const pipeline, const unsigned int table_id, of1x_flow_entry_t **const entry, bool check_overlap, bool reset_counts);

/**
* @ingroup core_of1x 
* Modify flow_entry(s) in the table
*
* The modify flow entry will modify any exisiting entry in the table that contains the
* same matches as the parameter entry. The "entry" parameter is NOT a pointer to an existing
* table entry. 
*
* If (and only if) the mod operation is successful (ROFL_OF1X_FM_SUCCESS) the contents of the pointer *entry are set to NULL. Any other
* reference to the real entry (**entry) shall never be further used.
*
* On success, the library will modify the necessary state to correctly handle timers and
* statistics of the modified entries.
*    
* @param pipeline Switch pipeline
* @param table_id Table index
* @param entry of1x_flow_entry_t that will update the existing (entries that have the same matches will be modified). This is NOT the entry TO update. 
* @param strict Strictness, check the matches in a strict way 
* @param reset_counts If overlap flag is false, reset the counters on entry overwrite 
* @warning On success (ROFL_SUCCESS), the entry pointer (*entry) will be set to NULL. 
* or freed from outside the library.
*/
rofl_result_t of1x_modify_flow_entry_table(struct of1x_pipeline *const pipeline, const unsigned int table_id, of1x_flow_entry_t **const entry, const enum of1x_flow_removal_strictness strict, bool reset_counts);
	
/**
* @ingroup core_of1x 
* Removes a flow_entry from the table.
*
* The remove flow entry will remove and destroy any exisiting entry in the table that contains
* the same matches as the parameter entry. The "entry" parameter is NOT a pointer to an existing
* table entry. 
*
* The entry parameter will never be modified by the library, and can be safely used or destroyed
* after the call of of1x_remove_flow_entry_table()
*    
* On success, the timers and statistics of the removed entries are purged.

* @param pipeline Switch pipeline
* @param table_id Table index
* @param entry All the entries having the same matches as "entry" will be removed. This is NOT
* the existing table entry to remove. 
* @param strict Strictness, check the matches in a strict way 
* @param out_port Only remove the entries that contain out_port in the actions
* @param out_group Only remove the entries that contain out_group in the actions
*/
rofl_result_t of1x_remove_flow_entry_table(struct of1x_pipeline *const pipeline, const unsigned int table_id, of1x_flow_entry_t* entry, const enum of1x_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group);

//This API call is meant to ONLY be used internally within the pipeline library (timers)
rofl_result_t __of1x_remove_specific_flow_entry_table(struct of1x_pipeline *const pipeline, const unsigned int table_id, of1x_flow_entry_t *const specific_entry, of1x_flow_remove_reason_t reason, of1x_mutex_acquisition_required_t mutex_acquired);

/*
* Table dumping. Not recommended to use it directly
*
* @param raw_nbo Show values in the pipeline internal byte order (NBO). Warning: some values are intentionally unaligned. 
*/
void of1x_dump_table(of1x_flow_table_t* table, bool raw_nbo);

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_FLOW_TABLE
