/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_STATISTICS_PP_H__
#define __OF1X_STATISTICS_PP_H__

#include "rofl.h"
#include "../../../util/pp_guard.h" //Never forget to include the guard

#include "of1x_statistics.h"

//Platform stuff
#include "../../../platform/lock.h"
#include "../../../platform/likely.h"
#include "../../../platform/memory.h"
#include "../../../platform/packet.h"
#include "../../../platform/atomic_operations.h"
#include "../of1x_async_events_hooks.h"


/**
* @file of1x_statistics_pp.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Statistics related to packet processing routines
*/

//Flow
static inline void __of1x_stats_flow_update_match(unsigned int tid, of1x_stats_flow_t* stats, uint64_t bytes_rx){
	
	if(unlikely(tid == ROFL_PIPELINE_LOCKED_TID)){
		platform_atomic_inc64(&stats->s.__internal[ROFL_PIPELINE_LOCKED_TID].packet_count, stats->mutex);
		platform_atomic_add64(&stats->s.__internal[ROFL_PIPELINE_LOCKED_TID].byte_count, bytes_rx, stats->mutex);
	}else{
		stats->s.__internal[tid].packet_count++;
		stats->s.__internal[tid].byte_count+=bytes_rx;
	} 
}

//Flow table
static inline void __of1x_stats_table_update_match(unsigned int tid, of1x_stats_table_t* stats){
	
	if(unlikely(tid == ROFL_PIPELINE_LOCKED_TID)){
		platform_atomic_inc64(&stats->s.__internal[tid].lookup_count,stats->mutex);
		platform_atomic_inc64(&stats->s.__internal[tid].matched_count,stats->mutex);
	}else{
		stats->s.__internal[tid].lookup_count++;
		stats->s.__internal[tid].matched_count++;
	}
}

static inline void __of1x_stats_table_update_no_match(unsigned int tid, of1x_stats_table_t* stats){

	if(unlikely(tid == ROFL_PIPELINE_LOCKED_TID)){
		platform_atomic_inc64(&stats->s.__internal[tid].lookup_count,stats->mutex);
	}else{
		stats->s.__internal[tid].lookup_count++;
	}
}

//Group
static void __of1x_stats_group_update(unsigned int tid, of1x_stats_group_t *gr_stats, uint64_t bytes){

	if(unlikely(tid == ROFL_PIPELINE_LOCKED_TID)){
		platform_atomic_inc64(&gr_stats->s.__internal[tid].packet_count, gr_stats->mutex);
		platform_atomic_add64(&gr_stats->s.__internal[tid].byte_count, bytes, gr_stats->mutex);
	}else{
		gr_stats->s.__internal[tid].packet_count++;
		gr_stats->s.__internal[tid].byte_count += bytes;
	}

}

//Bucket
static void __of1x_stats_bucket_update(unsigned int tid, __of1x_stats_bucket_t* bc_stats, uint64_t bytes){
	
	if(unlikely(tid == ROFL_PIPELINE_LOCKED_TID)){
		platform_atomic_inc64(&bc_stats->s.__internal[tid].packet_count, bc_stats->mutex);
		platform_atomic_add64(&bc_stats->s.__internal[tid].byte_count, bytes, bc_stats->mutex);
	}else{
		bc_stats->s.__internal[tid].packet_count++;
		bc_stats->s.__internal[tid].byte_count += bytes;
	}
}

#endif //OF1X_STATISTICS_PP_H
