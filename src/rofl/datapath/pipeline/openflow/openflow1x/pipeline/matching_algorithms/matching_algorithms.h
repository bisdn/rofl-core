/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef MATCHING_ALGORITHMS_H_
#define MATCHING_ALGORITHMS_H_

/**
* @file matching_algorithm.h
* @author Tobias Jungel<tobias.jungel (at) bisdn.de>, Marc Sune<marc.sune (at) bisdn.de>
*
* @brief OpenFlow v1.0, 1.2 and 1.3.2 Extendable Matching Algorithm API
*
* The Extendable Matching Algorithm API (EMAA) is the set of interfaces that allow to extend
* the lookup of flow entries within a pipeline flow table. The API covers the lookup as well
* as the addition, modify and removal flow entry processes. Indeed, matching algorithms are
* in charge of mantaining, using the set of data structures of its choice, the list of entries,
* so that add/modify/remove and lookup operations can be performed.
*
* Matching algorithms may have specially tailored scopes (e.g. tailored to L2 forwarding), or
* simply smart version of generic lookup algorithms, using advanced techniques
*
* To fill in
*/

#include "rofl.h"
#include "../of1x_flow_entry.h"
#include "../of1x_statistics.h"
#include "../of1x_utils.h"
#include "available_ma.h"

/*
 * forward declarations
 */
struct of1x_flow_table;
enum of1x_mutex_acquisition_required;

#define OF1X_MATCHING_ALGORITHMS_MAX_DESCRIPTION_LENGTH 256

/**
* Registers a matching algorithm
*/
#define OF1X_REGISTER_MATCHING_ALGORITHM(NAME)\
		struct of1x_matching_algorithm_functions of1x_matching_algorithm_##NAME##_maf


/**
* @brief Set of functions calls to be implemented in order to extend Matching algorithms for OF versions 1.0, 1.2 and 1.3.2.
* @ingroup core_ma_of1x
* Some calls are optional.
*/
typedef struct of1x_matching_algorithm_functions{

	/**
	* @ingroup core_ma_of1x
	* @brief Allows to initialize matching algorithm table state.
	*
	* The algorithm may also initialize table->matching_aux structures with
	* its own data (e.g. smart table structures) for further use during operations.
	*
	* Remember that the matching algorithm is in charge of mantaining table entry state.
	*/
	rofl_result_t
	(*init_hook)(struct of1x_flow_table *const table);

	/**
	* @brief Allows to destroy matching algorithm table state.
	*
	* The algorithm MUST also destroy all the entries existing in the table, calling
	* the appropiate of1x_destroy_flow_entry_with_reason() function. The matching algorithm does
	* NOT need to care about statistics or timers.
	*
	* Remember that the matching algorithm is in charge of mantaining table entry state.
	*
	* @ingroup core_ma_of1x
	*/
	rofl_result_t
	(*destroy_hook)(struct of1x_flow_table *const table); //Mutual exclusion will already be taken by the of1x_flow_table destructor


	// flow management
	/**
	* @ingroup core_ma_of1x
	* @brief Adds a flow entry to the table
	*
	* The algorithm MUST implement the addition of the flow entry into the table. The
	* addition must behave exactly as defined in the of1x_add_flow_entry_to_table()
	*
	* The matching algorithm may use internally a different representation of the flow
	* entry, but it MUST always maintain the reference to the of1x_flow_entry instance,
	* and call appropiately of1x_destroy_flow_entry_with_reason() or of1x_update_flow_entry().
	*
	* The matching algorithm does NOT need to care about statistics or timers.
	*
	* Remember that the matching algorithm is in charge of mantaining table entry state.
	* The addition MUST comply with the behaviour defined in the OpenFlow specifications for versions 1.0, 1.2 and 1.3.2
	*
	*/
	rofl_of1x_fm_result_t
	(*add_flow_entry_hook)(struct of1x_flow_table *const table,
			of1x_flow_entry_t *const entry,
			bool check_overlap,
			bool reset_counts);


	/**
	* @ingroup core_ma_of1x
	* @brief Modifies a set of flow entries in the table
	*
	* The algorithm MUST implement the modification of the flow entry(ies) into the table. The
	* modification must behave exactly as defined in the of1x_modify_flow_entry_table()
	*
	* The matching algorithm may use internally a different representation of the flow
	* entry, but it MUST always maintain the reference to the of1x_flow_entry instance(s),
	* and call appropiately of1x_destroy_flow_entry_with_reason() or of1x_update_flow_entry().
	*
	* The matching algorithm shall use of1x_update_flow_entry() when a modification must
	* take place, regardless of the internal representation it is using.
	*
	* The matching algorithm does NOT need to care about statistics or timers.
	*
	* Remember that the matching algorithm is in charge of mantaining table entry state.
	* The modify operation MUST comply with the behaviour defined in the OpenFlow specifications for versions 1.0, 1.2 and 1.3.2
	*
	*/
	rofl_result_t
	(*modify_flow_entry_hook)(struct of1x_flow_table *const table,
			of1x_flow_entry_t *const entry,
			const enum of1x_flow_removal_strictness strict,
			bool reset_counts);


	/**
	* @ingroup core_ma_of1x
	* @brief Removes one or more flow entries in the table
	*
	* The algorithm MUST implement the removal of the flow entry into the table. The
	* remvoal must behave exactly as defined in the of1x_remove_flow_entry_table()
	*
	* The matching algorithm may use internally a different representation of the flow
	* entry, but it MUST always maintain the reference to the of1x_flow_entry instance(s),
	* and call appropiately of1x_destroy_flow_entry_with_reason().
	*
	* The matching algorithm shall use of1x_destroy_flow_entry_with_reason() when deletion must
	* take place, regardless of the internal representation it is using.
	*
	* The matching algorithm does NOT need to care about statistics or timers.
	*
	* Remember that the matching algorithm is in charge of mantaining table entry state.
	* The remove operation MUST comply with the behaviour defined in the OpenFlow specifications for versions 1.0, 1.2 and 1.3.2.
	*
	*/
	rofl_result_t
	(*remove_flow_entry_hook)(struct of1x_flow_table *const table,
			of1x_flow_entry_t *const entry,
			of1x_flow_entry_t *const specific_entry,
			const enum of1x_flow_removal_strictness strict,
			uint32_t out_port,
			uint32_t out_group,
			of1x_flow_remove_reason_t reason,
			of1x_mutex_acquisition_required_t mutex_acquired);



	//Packet matching lookup
	/**
	* @ingroup core_ma_of1x
	* @ingroup core_pp
	* @brief Finds the best match for the packet according the pkt_matches
	*
	* The lookup of the best match may use the algorithm internal representation
	* to speedup the process.
	*
	* In any case, the lookup MUST comply with the behaviour defined in the OpenFlow
	* specifications for versions 1.0, 1.2 and 1.3.2
	*/
	of1x_flow_entry_t*
	(*find_best_match_hook)(unsigned tid, struct of1x_flow_table *const table,
			packet_matches_t *const pkt_matches);



	// flow stats
	/**
	* @ingroup core_ma_of1x
	* Retrieves flow stats according to spec
	* The operation MUST comply with the behaviour defined in the OpenFlow specifications for versions 1.0, 1.2 and 1.3.2
	*/
	rofl_result_t
	(*get_flow_stats_hook)(struct of1x_flow_table *const table,
			uint64_t cookie,
			uint64_t cookie_mask,
			uint32_t out_port,
			uint32_t out_group,
			of1x_match_group_t *const matches,
			of1x_stats_flow_msg_t* msg);



	/**
	* @ingroup core_ma_of1x
	* Retrieves aggregate flow stats according to spec
	* The operation MUST comply with the behaviour defined in the OpenFlow specifications for versions 1.0, 1.2 and 1.3.2
	*/
	rofl_result_t
	(*get_flow_aggregate_stats_hook)(struct of1x_flow_table *const table,
			uint64_t cookie,
			uint64_t cookie_mask,
			uint32_t out_port,
			uint32_t out_group,
			of1x_match_group_t *const matches,
			of1x_stats_flow_aggregate_msg_t* msg);


	/**
	* @ingroup core_ma_of1x
	* The find_entry_using_group_hook() must retrieve the first entry in the
	* table that contain actions refering to group_id and return it.
	*
	* The matching algorithm may use of1x_apply_actions_has() and of1x_write_actions_has()
	* helper functions to perform the lookup.
	*
	* This is usually used by the core when group deletion occurs.
	*/
	of1x_flow_entry_t*
	(*find_entry_using_group_hook)(struct of1x_flow_table *const table,
			const unsigned int group_id);


	// dump flow table
	/**
	* @ingroup core_ma_of1x
	* Dump hook. Optionally implement this hook to dump internal state of the
	* flow table when of1x_dump_switch() is called.
	*
	* This is optional.
	* @param raw_nbo Show values in the pipeline internal byte order (NBO). Warning: some values are intentionally unaligned.
	*/
	void
	(*dump_hook)(struct of1x_flow_table *const table, bool raw_nbo);


	/**
	* @ingroup core_ma_of1x
	* Description of the matching algorithm. Use a precise description of the
	* matching algorith, including its strengths and weaknesses.
	*/
	char description[OF1X_MATCHING_ALGORITHMS_MAX_DESCRIPTION_LENGTH];
}of1x_matching_algorithms_functions_t;

/**
* Array containing all matching algorithms and the function pointers.
*/
extern of1x_matching_algorithms_functions_t of1x_matching_algorithms[];

/**
* Initializes all matching algorithms to be potentially used by the switches
*/
void __of1x_generate_matching_algorithm_list(void);

#endif /* MATCHING_ALGORITHMS_H_ */
