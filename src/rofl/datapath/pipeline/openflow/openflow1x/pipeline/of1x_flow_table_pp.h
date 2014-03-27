/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_FLOW_TABLE_PPH__
#define __OF1X_FLOW_TABLE_PPH__

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdint.h>
#include "rofl.h"
#include "../../../util/pp_guard.h" //Never forget to include the guard
#include "../../../common/datapacket.h"
#include "matching_algorithms/available_ma_pp.h"
#include "of1x_flow_table.h"

//C++ extern C
ROFL_BEGIN_DECLS

/*
* Entry lookup. This should never be used directly
*/ 
/* Main process_packet_through */
static inline struct of1x_flow_entry* __of1x_find_best_match_table(struct of1x_flow_table *const table, datapacket_t *const pkt){
	return __of1x_matching_algorithms_find_best_match(table->matching_algorithm, table, pkt);
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_FLOW_TABLE_PP
