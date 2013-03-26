#ifndef __OF12_LOOP_MATCH_H__
#define __OF12_LOOP_MATCH_H__


#include "../matching_algorithms.h"
#include "../../of12_flow_table.h"
#include "rofl.h"

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

void load_matching_algorithm_loop(struct matching_algorithm_functions *f);
inline of12_flow_entry_t* of12_find_best_match_loop(of12_flow_table_t *const table, of12_packet_matches_t *const pkt);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //LOOP_MATCH
