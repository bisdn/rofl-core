#ifndef __OF12_FLOW_ENTRY_H__
#define __OF12_FLOW_ENTRY_H__

#include <stdlib.h>
#include <string.h>

#include "rofl.h"
#include "../../../util/rofl_pipeline_utils.h"
#include "../../../common/ternary_fields.h"
#include "../../../platform/lock.h"
#include "of12_match.h"
#include "of12_instruction.h"
#include "of12_timers.h"
#include "of12_statistics.h"

struct of12_flow_table;
struct of12_timers_info;

//Flow removal operations
typedef enum of12_flow_removal_strictness{
	NOT_STRICT = 0,
	STRICT
}of12_flow_removal_strictness_t;

/*
* OF12 Flow enetry structure
*/
struct of12_flow_entry{
	
	//Entry priority	
	uint16_t priority;
	
	//Array of matches
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

};
typedef struct of12_flow_entry of12_flow_entry_t;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

//of12_flow_entry_t* of12_init_flow_entry(const uint16_t priority, of12_match_group_t* match_group, of12_flow_entry_t* prev, of12_flow_entry_t* next);
of12_flow_entry_t* of12_init_flow_entry(of12_flow_entry_t* prev, of12_flow_entry_t* next, bool notify_removal);
rofl_result_t of12_destroy_flow_entry(of12_flow_entry_t* entry); 

//Add match
rofl_result_t of12_add_match_to_entry(of12_flow_entry_t* entry, of12_match_t* match);

//Update entry
rofl_result_t of12_update_flow_entry(of12_flow_entry_t* entry_to_update, of12_flow_entry_t* mod, bool reset_counts);

//Flow comparison
bool of12_flow_entry_check_equal(of12_flow_entry_t*const original, of12_flow_entry_t*const entry, uint32_t out_port, uint32_t out_group);
bool of12_flow_entry_check_overlap(of12_flow_entry_t*const original, of12_flow_entry_t*const entry, bool check_priority, bool check_cookie, uint32_t out_port, uint32_t out_group);
bool of12_flow_entry_check_contained(of12_flow_entry_t*const original, of12_flow_entry_t*const subentry, bool check_priority, bool check_cookie, uint32_t out_port, uint32_t out_group);

//Dump flow
void of12_dump_flow_entry(of12_flow_entry_t* entry);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_FLOW_ENTRY
