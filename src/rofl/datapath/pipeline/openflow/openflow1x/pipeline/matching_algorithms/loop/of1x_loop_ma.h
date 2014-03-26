#ifndef __OF1X_LOOP_MATCH_H__
#define __OF1X_LOOP_MATCH_H__

#include "rofl.h"
#include "../matching_algorithms.h"
#include "../../of1x_flow_table.h"

//C++ extern C
ROFL_BEGIN_DECLS

extern of1x_flow_entry_t* of1x_find_best_match_loop(of1x_flow_table_t *const table, packet_matches_t *const pkt_matches);

//C++ extern C
ROFL_END_DECLS

#endif //LOOP_MATCH
