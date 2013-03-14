/*
 * matching_algorithms.h
 *
 *  Created on: Nov 28, 2012
 *      Author: tobi, msune
 */

#ifndef MATCHING_ALGORITHMS_H_
#define MATCHING_ALGORITHMS_H_

#include "rofl.h"
#include "../of12_flow_entry.h"
#include "../of12_statistics.h"
#include "../of12_utils.h"
#include "matching_algorithms_available.h"
/**
 * forward declarations
 */
struct of12_flow_table;
enum of12_mutex_acquisition_required;

#define OF12_MATCHING_ALGORITHMS_MAX_DESCRIPTION_LENGTH 256

struct matching_algorithm_functions
{
	// init and destroy
	rofl_result_t
	(*init_hook)(struct of12_flow_table *const table);
	rofl_result_t
	(*destroy_hook)(struct of12_flow_table *const table); //Mutual exclusion will already be taken by the of12_flow_table destructor

	// flow management
	rofl_of12_fm_result_t
	(*add_flow_entry_hook)(struct of12_flow_table *const table,
			of12_flow_entry_t *const entry,
			bool check_overlap,
			bool reset_counts);

	rofl_result_t
	(*modify_flow_entry_hook)(struct of12_flow_table *const table,
			of12_flow_entry_t *const entry,
			const enum of12_flow_removal_strictness strict,
			bool reset_counts);
	
	rofl_result_t
	(*remove_flow_entry_hook)(struct of12_flow_table *const table,
			of12_flow_entry_t *const entry, 
			of12_flow_entry_t *const specific_entry,
			const enum of12_flow_removal_strictness strict,
			uint32_t out_port,
			uint32_t out_group,
			of12_mutex_acquisition_required_t mutex_acquired);

	//Packet matching lookup
	of12_flow_entry_t*
	(*find_best_match_hook)(struct of12_flow_table *const table,
			of12_packet_matches_t *const pkt_matches);

	// flow stats
	of12_stats_flow_msg_t*	
	(*get_flow_stats_hook)(struct of12_pipeline *const pipeline,
			uint32_t table_id,
			uint64_t cookie,
			uint64_t cookie_mask,
			uint32_t out_port, 
			uint32_t out_group,
			of12_match_t *const matchs);
	of12_stats_flow_aggregate_msg_t*	
	(*get_flow_aggregate_stats_hook)(struct of12_pipeline *const pipeline,
			uint32_t table_id,
			uint64_t cookie,
			uint64_t cookie_mask,
			uint32_t out_port, 
			uint32_t out_group,
			of12_match_t *const matchs);

	//Finds the first entry that uses the group
	//this is usually used when a group is deleted
	of12_flow_entry_t*
	(*find_entry_using_group_hook)(struct of12_flow_table *const table,
			const unsigned int group_id);


	// dump flow table
	void
	(*dump_hook)(struct of12_flow_table *const table);
	
	//description of the matching algorithm
	char description[OF12_MATCHING_ALGORITHMS_MAX_DESCRIPTION_LENGTH];
};

void load_matching_algorithm(enum matching_algorithm_available m, struct matching_algorithm_functions *f);

#endif /* MATCHING_ALGORITHMS_H_ */
