/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_FLOW_ENTRY_H__
#define __OF1X_FLOW_ENTRY_H__

#include <stdlib.h>
#include <string.h>

#include "rofl.h"
#include "../../../common/ternary_fields.h"
#include "../../../platform/lock.h"
#include "of1x_match.h"
#include "of1x_instruction.h"
#include "of1x_timers.h"
#include "of1x_statistics.h"
#include "of1x_utils.h"

/**
* @file of1x_flow_entry.h
* @author Marc Sune<marc.sune (at) bisdn.de>, Tobias Jungel<tobias.jungel (at) bisdn.de>  
*
* @brief Openflow v1.2 flow entry abstraction
*
*/

//Fwd declarations
struct of1x_pipeline;
struct of1x_flow_table;
struct of1x_timers_info;
struct of1x_group_table;

/**
* Flow removal operations strictness
* @ingroup core_of1x 
*/
typedef enum of1x_flow_removal_strictness{
	NOT_STRICT = 0,
	STRICT
}of1x_flow_removal_strictness_t;

/**
* Flow remove reasons (enum ofp_flow_removed_reason)
* @ingroup core_of1x 
*/
typedef enum of1x_flow_remove_reason {
	OF1X_FLOW_REMOVE_IDLE_TIMEOUT=0,		/* Flow idle time exceeded idle_timeout. */
	OF1X_FLOW_REMOVE_HARD_TIMEOUT=1,		/* Time exceeded hard_timeout. */
	OF1X_FLOW_REMOVE_DELETE=2,			/* Evicted by a DELETE flow mod. */
	OF1X_FLOW_REMOVE_GROUP_DELETE=3,		/* Group was removed. */

	OF1X_FLOW_REMOVE_NO_REASON = 0xFF		/* No reason -> do not notify */
}of1x_flow_remove_reason_t;

/**
 * Platform dependent opaque state
 * @ingroup core_of1x
 */
typedef void of1x_flow_entry_platform_state_t;

//Used to pre-append priority, to emulate OF1.0 behaviour ONLY
#define OF10_NON_WILDCARDED_PRIORITY_FLAG 0x10000

/**
* Openflow v1.2 flow entry structure
* @ingroup core_of1x 
*/
typedef struct of1x_flow_entry{
	
	//Entry priority(lowest 16 bit is the OF priority)
	//17th bit is only set to 1/0 for OF1.0 (is wildcarded or not) 
	uint32_t priority;
	
	//Previous entry
	struct of1x_flow_entry* prev;
	
	//Next entry
	struct of1x_flow_entry* next;

	//Table in which rule is inserted (for fast safety checkings)
	struct of1x_flow_table* table;
	
	//Matches
	of1x_match_group_t matches;
	
	//Instructions
	of1x_instruction_group_t inst_grp;

	//Cookie
	uint64_t cookie;
	uint64_t cookie_mask;

	//Notify when removed
	bool notify_removal;

	//Timers
	struct of1x_timers_info timer_info;
	
	//statistics
	of1x_stats_flow_t stats;

	//RWlock
	platform_rwlock_t* rwlock;

	//Platform agnostic pointer
	of1x_flow_entry_platform_state_t* platform_state;
}of1x_flow_entry_t;

//C++ extern C
ROFL_BEGIN_DECLS

/**
* @brief Create an empty flow entry 
* @ingroup core_of1x 
*/
of1x_flow_entry_t* of1x_init_flow_entry(of1x_flow_entry_t* prev, of1x_flow_entry_t* next, bool notify_removal);

//This should never be used from outside the library
rofl_result_t __of1x_destroy_flow_entry_with_reason(of1x_flow_entry_t* entry, of1x_flow_remove_reason_t reason); 

/**
* @brief Destroy the flow entry, including stats, instructions and actions 
* @ingroup core_of1x 
*/
rofl_result_t of1x_destroy_flow_entry(of1x_flow_entry_t* entry); 

//Add match
/**
* @brief Adds a match to the flow_entry
* @ingroup core_of1x 
*/
rofl_result_t of1x_add_match_to_entry(of1x_flow_entry_t* entry, of1x_match_t* match);

//Update entry
rofl_result_t __of1x_update_flow_entry(of1x_flow_entry_t* entry_to_update, of1x_flow_entry_t* mod, bool reset_counts);

//Fast validation against OF version
rofl_result_t __of1x_validate_flow_entry(of1x_flow_entry_t* entry, struct of1x_pipeline* pipeline);
	
//Flow comparison
bool __of1x_flow_entry_check_equal(of1x_flow_entry_t*const original, of1x_flow_entry_t*const entry, uint32_t out_port, uint32_t out_group, bool check_cookie);
bool __of1x_flow_entry_check_overlap(of1x_flow_entry_t*const original, of1x_flow_entry_t*const entry, bool check_priority, bool check_cookie, uint32_t out_port, uint32_t out_group);
bool __of1x_flow_entry_check_contained(of1x_flow_entry_t*const original, of1x_flow_entry_t*const subentry, bool check_priority, bool check_cookie, uint32_t out_port, uint32_t out_group, bool reverse_out_check);

//Dump flow
/**
* @brief Dumps the flow entry for debugging purposes.  
* @ingroup core_of1x 
*/
void of1x_dump_flow_entry(of1x_flow_entry_t* entry);

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_FLOW_ENTRY
