#ifndef __OF1X_LOOP_MATCH_H__
#define __OF1X_LOOP_MATCH_H__

#include "rofl.h"
#include "../matching_algorithms.h"
#include "../../of1x_flow_table.h"

//C++ extern C
ROFL_BEGIN_DECLS

rofl_of1x_fm_result_t of1x_add_flow_entry_loop(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, bool check_overlap, bool reset_counts);

rofl_result_t of1x_modify_flow_entry_loop(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, const enum of1x_flow_removal_strictness strict, bool reset_counts);

rofl_result_t of1x_remove_flow_entry_loop(of1x_flow_table_t *const table , of1x_flow_entry_t *const entry, of1x_flow_entry_t *const specific_entry, const enum of1x_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group, of1x_flow_remove_reason_t reason, of1x_mutex_acquisition_required_t mutex_acquired);

rofl_result_t of1x_get_flow_stats_loop(struct of1x_flow_table *const table,
		uint64_t cookie,
		uint64_t cookie_mask,
		uint32_t out_port, 
		uint32_t out_group,
		of1x_match_group_t *const matches,
		of1x_stats_flow_msg_t* msg);

rofl_result_t of1x_get_flow_aggregate_stats_loop(struct of1x_flow_table *const table,
		uint64_t cookie,
		uint64_t cookie_mask,
		uint32_t out_port, 
		uint32_t out_group,
		of1x_match_group_t *const matches,
		of1x_stats_flow_aggregate_msg_t* msg);

of1x_flow_entry_t* of1x_find_entry_using_group_loop(of1x_flow_table_t *const table, const unsigned int group_id);

rofl_result_t of1x_destroy_loop(struct of1x_flow_table *const table);

//C++ extern C
ROFL_END_DECLS

#endif //LOOP_MATCH
