/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF12_FLOW_ENTRY_H__
#define __OF12_FLOW_ENTRY_H__

#include <stdlib.h>
#include <string.h>

#include "rofl.h"
#include "../../../common/ternary_fields.h"
#include "../../../platform/lock.h"
#include "of12_match.h"
#include "of12_instruction.h"
#include "of12_timers.h"
#include "of12_statistics.h"

/**
* @file of12_flow_entry.h
* @author Marc Sune<marc.sune (at) bisdn.de>, Tobias Jungel<tobias.jungel (at) bisdn.de>  
*
* @brief Openflow v1.2 flow entry abstraction
*
*/

struct of12_flow_table;
struct of12_timers_info;
struct of12_group_table;

/**
* Flow removal operations strictness
* @ingroup core_of12 
*/
typedef enum of12_flow_removal_strictness{
	NOT_STRICT = 0,
	STRICT
}of12_flow_removal_strictness_t;

/**
* Flow remove reasons (enum ofp_flow_removed_reason)
* @ingroup core_of12 
*/
typedef enum of12_flow_remove_reason {
	OF12_FLOW_REMOVE_IDLE_TIMEOUT=0,		/* Flow idle time exceeded idle_timeout. */
	OF12_FLOW_REMOVE_HARD_TIMEOUT=1,		/* Time exceeded hard_timeout. */
	OF12_FLOW_REMOVE_DELETE=2,			/* Evicted by a DELETE flow mod. */
	OF12_FLOW_REMOVE_GROUP_DELETE=3,		/* Group was removed. */

	OF12_FLOW_REMOVE_NO_REASON = 0xFF		/* No reason -> do not notify */
}of12_flow_remove_reason_t;

/**
* Openflow v1.2 flow entry structure
* @ingroup core_of12 
*/
typedef struct of12_flow_entry{
	
	//Entry priority	
	uint16_t priority;
	
	//Array of matches
	/**
	* Pointer to the first match of the linked list
	* @TODO: deprecate this in favour of match_group
	*/
	of12_match_t* matchs;
	
	//Number of necessary matches (hits)
	uint8_t num_of_matches;

	//Previous entry
	struct of12_flow_entry* prev;
	
	//Next entry
	struct of12_flow_entry* next;

	//Table in which rule is inserted (for fast safety checkings)
	struct of12_flow_table* table;
	
	//Instructions
	of12_instruction_group_t inst_grp;

	//Cookie
	uint64_t cookie;
	uint64_t cookie_mask;

	//Notify when removed
	bool notify_removal;

	//Timers
	struct of12_timers_info timer_info;
	
	//statistics
	of12_stats_flow_t stats;

	//RWlock
	platform_rwlock_t* rwlock;

}of12_flow_entry_t;

//C++ extern C
ROFL_BEGIN_DECLS

/**
* @brief Create an empty flow entry 
* @ingroup core_of12 
*/
of12_flow_entry_t* of12_init_flow_entry(of12_flow_entry_t* prev, of12_flow_entry_t* next, bool notify_removal);

//This should never be used from outside the library
rofl_result_t __of12_destroy_flow_entry_with_reason(of12_flow_entry_t* entry, of12_flow_remove_reason_t reason); 

/**
* @brief Destroy the flow entry, including stats, instructions and actions 
* @ingroup core_of12 
*/
rofl_result_t of12_destroy_flow_entry(of12_flow_entry_t* entry); 

//Add match
/**
* @brief Adds a match to the flow_entry
* @ingroup core_of12 
*/
rofl_result_t of12_add_match_to_entry(of12_flow_entry_t* entry, of12_match_t* match);

//Update entry
rofl_result_t __of12_update_flow_entry(of12_flow_entry_t* entry_to_update, of12_flow_entry_t* mod, bool reset_counts);

//check if the entry is valid for insertion
rofl_result_t __of12_validate_flow_entry(struct of12_group_table *gt, of12_flow_entry_t* entry);
	
//Flow comparison
bool __of12_flow_entry_check_equal(of12_flow_entry_t*const original, of12_flow_entry_t*const entry, uint32_t out_port, uint32_t out_group);
bool __of12_flow_entry_check_overlap(of12_flow_entry_t*const original, of12_flow_entry_t*const entry, bool check_priority, bool check_cookie, uint32_t out_port, uint32_t out_group);
bool __of12_flow_entry_check_contained(of12_flow_entry_t*const original, of12_flow_entry_t*const subentry, bool check_priority, bool check_cookie, uint32_t out_port, uint32_t out_group);

//Dump flow
/**
* @brief Dumps the flow entry for debugging purposes.  
* @ingroup core_of12 
*/
void of12_dump_flow_entry(of12_flow_entry_t* entry);

//C++ extern C
ROFL_END_DECLS

#endif //OF12_FLOW_ENTRY
