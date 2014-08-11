/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
* @file threading_pp.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Threading Packet Processing API utils 
*/

#ifndef __THREADING_PP_H__
#define __THREADING_PP_H__

#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include "rofl.h"
#include "platform/likely.h" 

#if !defined(__GNUC__) && !defined(__INTEL_COMPILER)
	#error Unknown compiler; could not guess which compare-and-swap instructions to use
#else
	#define CAS(ptr, oldval, newval) __sync_bool_compare_and_swap(ptr, oldval, newval)
	#define tid_memory_barrier __sync_synchronize
#endif

//Basic type
typedef uint64_t tid_presence_t;

static inline void tid_init_presence_mask(tid_presence_t* presence_mask){
	presence_mask = 0x0ULL;
}

/**
* Set thread presence  
*/
static inline void tid_mark_as_present(unsigned int tid, tid_presence_t* presence_mask){
	tid_presence_t old_val, new_val;
	
	do{

TID_MARK_AS_PRESENT_RETRY:
		old_val = *presence_mask;
	
		//Check for other threads within  
		if( unlikely(tid == ROFL_PIPELINE_LOCKED_TID ) && unlikely( ( old_val & ( 1<<tid ) ) > 0)){
			//There some other thread in ROFL_PIPELINE_LOCKED_TID
			usleep(0);
			goto TID_MARK_AS_PRESENT_RETRY;
		}

		//Set our
		new_val = old_val | ( 1<<tid );

	}while( CAS(presence_mask, old_val, new_val) == false);
}
	
/**
* Unset thread presence  
*/
static inline void tid_mark_as_not_present(unsigned int tid, tid_presence_t* presence_mask){
	tid_presence_t old_val, new_val;
	
	do{
		old_val = *presence_mask;

		//Double check
		assert( ( old_val & (1<<tid) ) > 0);	

		//Set our
		new_val = old_val & ~(1 << tid);

	}while( CAS(presence_mask, old_val, new_val) == false);
}

/**
*
*/
static inline void tid_wait_all_not_present(tid_presence_t* presence_mask){
	int i;
	uint64_t tid;
	tid_presence_t present;
	tid_presence_t tmp; 

	//Memory barrier first
	tid_memory_barrier();	

	//Recover current present
	present = *presence_mask;
	
	for(i=0;i<ROFL_PIPELINE_MAX_TIDS;i++){	
		tid = 1 << i;

		if( unlikely( ( present & tid) > 0 ) ){
			//Wait until the core is out of the pipeline
			do{
				tmp = *presence_mask;
				if( likely( ( tmp&tid ) == 0) )
					break;
				usleep(0);
			}while(1);
		}
	}
}

#endif //THREADING_PP
