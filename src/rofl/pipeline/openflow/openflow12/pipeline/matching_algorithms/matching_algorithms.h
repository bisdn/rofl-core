/*
 * matching_algorithms.h
 *
 *  Created on: Nov 28, 2012
 *      Author: tobi
 */

#ifndef MATCHING_ALGORITHMS_H_
#define MATCHING_ALGORITHMS_H_

#include "../of12_flow_entry.h"
#include "matching_algorithms_available.h"
/**
 * forward declarations
 */
struct of12_flow_table;

enum of12_mutex_acquisition_required
{
	MUTEX_NOT_ACQUIRED = 0, /*mutex has not been acquired and we must take it*/
	MUTEX_ALREADY_ACQUIRED_BY_TIMER_EXPIRATION, /*mutex was taken when checking for expirations. We shouldn't call the timers functions*/
	MUTEX_ALREADY_ACQUIRED_NON_STRICT_SEARCH /*mutex was taken when looking for entries with a non strict definition*/
};
typedef enum of12_mutex_acquisition_required of12_mutex_acquisition_required_t;

//FlOW removal operations
enum of12_flow_removal_strictness_t
{
	NOT_STRICT = 0,
	STRICT
};

struct matching_algorithm_functions
{
	// init and destroy
	unsigned int
	(*init_hook)(struct of12_flow_table * const);
	unsigned int
	(*destroy_hook)(struct of12_flow_table * const); //Mutual exclusion will already be taken by the of12_flow_table destructor

	// flow management
	unsigned int
	(*add_flow_entry_hook)(struct of12_flow_table * const,
			of12_flow_entry_t * const);
	unsigned int
	(*remove_flow_entry_hook)(struct of12_flow_table * const,
			of12_flow_entry_t * const, of12_flow_entry_t * const,
			const enum of12_flow_removal_strictness_t,
			of12_mutex_acquisition_required_t);

	// lookup
	of12_flow_entry_t*
	(*find_best_match_hook)(struct of12_flow_table * const,
			of12_packet_matches_t * const);

	// dump flow table
	void
	(*dump_hook)(struct of12_flow_table * const);
};

void
load_matching_algorithm(enum matching_algorithm_available m,
		struct matching_algorithm_functions *f);

#endif /* MATCHING_ALGORITHMS_H_ */
