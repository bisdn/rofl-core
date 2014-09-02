/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF_SWITCH_PP_H__
#define __OF_SWITCH_PP_H__

#include <inttypes.h>
#include "rofl.h" 
#include "../util/pp_guard.h" //Never forget to include the guard
#include "of_switch.h"
#include "openflow1x/pipeline/of1x_pipeline_pp.h"

#include "../util/logging.h"

/**
* @file of_switch_pp.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief OpenFlow logical switch packet processing routines 
*/

//C++ extern C
ROFL_BEGIN_DECLS

//Wrapping of processing
/**
* @brief Processes a packet through the OpenFlow pipeline.  
* @ingroup core_pp 
*
* This call may only be used if the platform does not have a hardware
* accelerated packet matching machinery. Packet matches of the datapacket_t MUST be initialized
* before calling of_process_packet_pipeline()
*
* The tid is a unique ID between 0 (ROFL_PIPELINE_LOCKED_TID) and ROFL_PIPELINE_MAX_PIPELINE_TIDS-1
* which is used to process packets lockless. It is up to the rofl-pipeline user to prevent two threads
* (or processes/bare metal CPUs sharing memory) to use the same TID.
*
* The special ROFL_PIPELINE_LOCKED_TID is a special ID, which allow at the expense of memory barriers and
* locking.
* 
* @param tid Thread ID. 
* @param sw The switch which has to process the packet 
* @param pkt A struct datapacket instance. All the fields must be set to NULL
* except maybe platform_state 
* @warning Packet matches of the datapacket_t MUST be initialized before calling of_process_packet_pipeline() 
*/
static inline rofl_result_t of_process_packet_pipeline(const unsigned int tid, const of_switch_t* sw, struct datapacket *const pkt){

#ifdef DEBUG
	if(unlikely(tid >= ROFL_PIPELINE_MAX_TIDS)){
		ROFL_PIPELINE_ERR("Invalid tid: %ui. ROFL_PIPELINE_MAX_TIDS is %u\n", tid, ROFL_PIPELINE_MAX_TIDS);
		assert(0);
	}
#endif

	__of1x_process_packet_pipeline(tid, sw, pkt);
	/*
	//Not necessary yet

	switch(sw->of_ver){
		case OF_VERSION_10: 
		case OF_VERSION_12: 
		case OF_VERSION_13: 
			__of1x_process_packet_pipeline(sw, pkt);
			break;
		default: 
			return ROFL_FAILURE;
	}
	*/
	return ROFL_SUCCESS;
}	

//C++ extern C
ROFL_END_DECLS

#endif //OF_SWITCH_PP
