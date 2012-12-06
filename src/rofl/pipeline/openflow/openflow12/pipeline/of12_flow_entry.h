#ifndef __OF12_FLOW_ENTRY_H__
#define __OF12_FLOW_ENTRY_H__

#include <stdlib.h>
#include <string.h>

#include "../../../util/rofl_pipeline_utils.h"
#include "../../../common/ternary_fields.h"
#include "../../../platform/lock.h"
#include "of12_match.h"
#include "of12_instruction.h"
#include "of12_timers.h"
#include "of12_statistics.h"

struct of12_flow_table;
struct of12_timers_info;

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
	of12_instruction_group_t instructions;

	//Counters
	//FIXME

	//RWlock
	platform_rwlock_t rwlock;
	
	//timer
	struct of12_timers_info timer_info;
	
	//statistics
	of12_stats_flow_t stats;
};
typedef struct of12_flow_entry of12_flow_entry_t;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

of12_flow_entry_t* of12_init_flow_entry(const uint16_t priority, of12_match_t* matchs, of12_flow_entry_t* prev, of12_flow_entry_t* next); 
unsigned int of12_destroy_flow_entry(of12_flow_entry_t* entry); 

//Add match
unsigned int of12_add_match_to_entry(of12_flow_entry_t* entry, of12_match_t* match);


//Dump flow
void of12_dump_flow_entry(of12_flow_entry_t* entry);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_FLOW_ENTRY
