#ifndef __OF1X_L2HASH_MATCH_PP_H__
#define __OF1X_L2HASH_MATCH_PP_H__

#include "rofl.h"
#include "../../of1x_pipeline.h"
#include "../../of1x_flow_table.h"
#include "../../of1x_flow_entry.h"
#include "../../of1x_match_pp.h"
#include "../../of1x_group_table.h"
#include "../../of1x_instruction_pp.h"
#include "../../../of1x_async_events_hooks.h"
#include "../../../../../platform/lock.h"
#include "../../../../../platform/likely.h"
#include "../../../../../platform/memory.h"
#include "of1x_l2hash_ma.h"

//C++ extern C
ROFL_BEGIN_DECLS

/* FLOW entry lookup entry point */ 
static inline of1x_flow_entry_t* of1x_find_best_match_l2hash_ma(of1x_flow_table_t *const table, datapacket_t *const pkt){
	
	//TODO
	
	return NULL; 
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_L2HASH_MATCH_PP
