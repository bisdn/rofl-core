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
	(*init_hook)(struct of12_flow_table * const);
	rofl_result_t
	(*destroy_hook)(struct of12_flow_table * const); //Mutual exclusion will already be taken by the of12_flow_table destructor

	// flow management
	rofl_result_t
	(*add_flow_entry_hook)(struct of12_flow_table * const,
			of12_flow_entry_t * const,
			bool,
			bool);

	rofl_result_t
	(*modify_flow_entry_hook)(struct of12_flow_table * const,
			of12_flow_entry_t * const,
			const enum of12_flow_removal_strictness,
			bool);
	
	rofl_result_t
	(*remove_flow_entry_hook)(struct of12_flow_table * const,
			of12_flow_entry_t * const, of12_flow_entry_t * const,
			const enum of12_flow_removal_strictness,
			uint32_t out_port,
			uint32_t out_group,
			of12_mutex_acquisition_required_t);

	// lookup
	of12_flow_entry_t*
	(*find_best_match_hook)(struct of12_flow_table * const,
			of12_packet_matches_t * const);

	// dump flow table
	void
	(*dump_hook)(struct of12_flow_table * const);
	
	//description of the matching algorithm
	char description[OF12_MATCHING_ALGORITHMS_MAX_DESCRIPTION_LENGTH];
};

void load_matching_algorithm(enum matching_algorithm_available m, struct matching_algorithm_functions *f);

#endif /* MATCHING_ALGORITHMS_H_ */
