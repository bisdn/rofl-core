#include "of1x_trie_ma.h"

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

#define TRIE_DESCRIPTION "Trie algorithm performs the lookup using a patricia trie"


//
// Constructors and destructors
//
rofl_result_t of1x_init_trie(struct of1x_flow_table *const table){

	//Allocate main trie struct
	table->matching_aux[0] = (void*)platform_malloc_shared(sizeof(struct of1x_trie));
	of1x_trie_t* trie = (of1x_trie_t*)table->matching_aux[0];

	//Set values
	trie->active = trie->secondary = NULL;

	return ROFL_SUCCESS;
}

//Recursively destroy leafs
static void of1x_destroy_leaf(struct of1x_trie_leaf* leaf){

	if(!leaf)
		return;

	//Destroy inner leafs
	of1x_destroy_leaf(leaf->inner)

	//Destroy next one(s)
	of1x_destroy_leaf(leaf->next);

	//Free our memory
	platform_free_shared(leaf);
}

rofl_result_t of1x_destroy_trie(struct of1x_flow_table *const table){

	of1x_trie_t* trie = (of1x_trie_t*)table->matching_aux[0];

	//Free all the leafs
	of1x_destroy_leaf(trie->tries[0]);
	of1x_destroy_leaf(trie->tries[1]);

	//Free main leaf structure
	platform_free_shared(table->matching_aux[0]);

	return ROFL_FAILURE;
}

//Helper functions
of1x_flow_entry_t* _reen(struct of1x_trie_leaf** leaf){


	return NULL;
}

//
// Main routines
//


rofl_of1x_fm_result_t of1x_add_flow_entry_trie(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, bool check_overlap, bool reset_counts){

	struct of1x_trie_leaf* leaf;
	of1x_flow_entry_t* curr_entry;
	of1x_trie_t* trie = (of1x_trie_t*)table->matching_aux[0];
	struct of1x_trie_leaf *curr, *active;
	bool present;

	//Allow single add/remove operation over the table
	platform_mutex_lock(table->mutex);
	curr = trie->secondary;
	active = trie->active;
TRIE_ADD:

	//Check overlap
	//TODO

	//Check existent
	present = false;
	while( (curr_entry = of1x_find_existent_reen_trie(&curr) ) ){
		ROFL_PIPELINE_DEBUG("[trie][flowmod-modify(%p)] Existing entry (%p) will be updated with (%p)\n", entry, curr_etry, entry);
		if(__of1x_update_flow_entry(curr_entry, entry, reset_counts) != ROFL_SUCCESS)
			return ROFL_FAILURE;
		present = true;
	}

	if(!present){
		//It was not existent. Just add it
	}

	if(active == trie->active){
		//Swap tries and redo
		trie->active = trie->secondary;
		trie->secondary = active;
		goto TRIE_ADD;
	}

	platform_mutex_unlock(table->mutex);
	return ROFL_SUCCESS;
}

rofl_result_t of1x_modify_flow_entry_trie(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, const enum of1x_flow_removal_strictness strict, bool reset_counts){
	return ROFL_FAILURE;
}

rofl_result_t of1x_remove_flow_entry_trie(of1x_flow_table_t *const table , of1x_flow_entry_t *const entry, of1x_flow_entry_t *const specific_entry, const enum of1x_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group, of1x_flow_remove_reason_t reason, of1x_mutex_acquisition_required_t mutex_acquired){
	return ROFL_FAILURE;
}

//Define the matching algorithm struct
OF1X_REGISTER_MATCHING_ALGORITHM(trie) = {
	//Init and destroy hooks
	.init_hook = of1x_init_trie,
	.destroy_hook = of1x_destroy_trie,

	//Flow mods
	.add_flow_entry_hook = of1x_add_flow_entry_trie,
	.modify_flow_entry_hook = of1x_modify_flow_entry_trie,
	.remove_flow_entry_hook = of1x_remove_flow_entry_trie,

	//Stats
	.get_flow_stats_hook = of1x_get_flow_stats_loop,
	.get_flow_aggregate_stats_hook = of1x_get_flow_aggregate_stats_trie,

	//Find group related entries
	.find_entry_using_group_hook = of1x_find_entry_using_group_trie,

	//Dumping
	.dump_hook = NULL,
	.description = TRIE_DESCRIPTION,
};
