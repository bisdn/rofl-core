#include "of1x_l2hash_ma.h"

#include <stdlib.h>
#include <assert.h>
#include "../../of1x_pipeline.h"
#include "../../of1x_flow_table.h"
#include "../../of1x_flow_entry.h"
#include "../../of1x_match.h"
#include "../../of1x_group_table.h"
#include "../../of1x_instruction.h"
#include "../../../of1x_async_events_hooks.h"
#include "../../../../../platform/lock.h"
#include "../../../../../platform/likely.h"
#include "../../../../../platform/memory.h"
#include "../matching_algorithms.h"
#include "../loop/of1x_loop_ma.h"

#define L2HASH_DESCRIPTION "The l2hash algorithm searches the list of entries by its priority order. On the worst case the performance is o(N) with the number of entries"


//
// Constructors and destructors
//
rofl_result_t of1x_init_l2hash(struct of1x_flow_table *const table){

	//XXX:TODO
		
	return ROFL_FAILURE; 
}

rofl_result_t of1x_destroy_l2hash(struct of1x_flow_table *const table){

	//XXX:TODO

	return ROFL_FAILURE; 
}

//
//Hooks
//
void of1x_add_hook_l2hash(of1x_flow_entry_t *const entry){
	//XXX
}
void of1x_modify_hook_l2hash(of1x_flow_entry_t *const entry){
	//XXX
}
void of1x_remove_hook_l2hash(of1x_flow_entry_t *const entry){
	//XXX
}
//
// Main routines
// 


/* Conveniently wraps call with mutex.  */
rofl_of1x_fm_result_t of1x_add_flow_entry_l2hash(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, bool check_overlap, bool reset_counts){
	//Call loop with the right hooks
	return __of1x_add_flow_entry_loop(table, entry, check_overlap, reset_counts, of1x_add_hook_l2hash);
}

rofl_result_t of1x_modify_flow_entry_l2hash(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, const enum of1x_flow_removal_strictness strict, bool reset_counts){
	//Call loop with the right hooks
	return __of1x_modify_flow_entry_loop(table, entry, strict, reset_counts, of1x_add_hook_l2hash, of1x_modify_hook_l2hash);
}

rofl_result_t of1x_remove_flow_entry_l2hash(of1x_flow_table_t *const table , of1x_flow_entry_t *const entry, of1x_flow_entry_t *const specific_entry, const enum of1x_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group, of1x_flow_remove_reason_t reason, of1x_mutex_acquisition_required_t mutex_acquired){
	//Call loop with the right hooks
	return __of1x_remove_flow_entry_loop(table, entry, specific_entry, strict, out_port, out_group, reason, mutex_acquired, of1x_remove_hook_l2hash);
}

//Define the matching algorithm struct
OF1X_REGISTER_MATCHING_ALGORITHM(l2hash) = {
	//Init and destroy hooks
	.init_hook = of1x_init_l2hash,
	.destroy_hook = of1x_destroy_l2hash,

	//Flow mods
	.add_flow_entry_hook = of1x_add_flow_entry_l2hash,
	.modify_flow_entry_hook = of1x_modify_flow_entry_l2hash,
	.remove_flow_entry_hook = of1x_remove_flow_entry_l2hash,

	//Stats
	.get_flow_stats_hook = of1x_get_flow_stats_loop,
	.get_flow_aggregate_stats_hook = of1x_get_flow_aggregate_stats_loop,

	//Find group related entries	
	.find_entry_using_group_hook = of1x_find_entry_using_group_loop,

	//Dumping	
	.dump_hook = NULL,
	.description = L2HASH_DESCRIPTION,
};
