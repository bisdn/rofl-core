/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_LOOP_MATCH_PP_H__
#define __OF1X_LOOP_MATCH_PP_H__

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
#include "of1x_loop_ma.h"

//C++ extern C
ROFL_BEGIN_DECLS

/* FLOW entry lookup entry point */ 
static inline of1x_flow_entry_t* of1x_find_best_match_loop_ma(of1x_flow_table_t *const table, datapacket_t *const pkt){
	
	of1x_match_t* it;
	of1x_flow_entry_t *entry;

	//Prevent writers to change structure during matching
	platform_rwlock_rdlock(table->rwlock);
	
	//Table is sorted out by nº of hits and priority N. First full match => best_match 
	for(entry = table->entries;entry!=NULL;entry = entry->next){
		bool matched = true;
		
		for( it=entry->matches.head ; it ; it=it->next ){
			if(!__of1x_check_match(pkt, it)){
				matched = false;
				break;
			}
		}

		if(matched){
			//Lock writers to modify the entry while packet processing. WARNING!!!! this must be released by the pipeline, once packet is processed!
			platform_rwlock_rdlock(entry->rwlock);

			//Green light for writers
			platform_rwlock_rdunlock(table->rwlock);
			return entry;
		}
	}
	
	//No match
	//Green light for writers
	platform_rwlock_rdunlock(table->rwlock);
	return NULL; 
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_LOOP_MATCH_PP
