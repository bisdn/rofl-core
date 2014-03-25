/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF_SWITCH_PP_H__
#define __OF_SWITCH_PP_H__

#include <inttypes.h> 
#include "../util/pp_guard.h" //Never forget to include the guard
#include "of_switch.h"
#include "openflow1x/pipeline/of1x_pipeline_pp.h"

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
* @param sw The switch which has to process the packet 
* @param pkt A struct datapacket instance. All the fields must be set to NULL
* except maybe platform_state 
* @warning Packet matches of the datapacket_t MUST be initialized before calling of_process_packet_pipeline() 
*/
static inline rofl_result_t of_process_packet_pipeline(const of_switch_t* sw, struct datapacket *const pkt){
	__of1x_process_packet_pipeline(sw, pkt);
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
