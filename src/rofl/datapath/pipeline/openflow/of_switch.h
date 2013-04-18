/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF_SWITCH_H__
#define __OF_SWITCH_H__

#include <inttypes.h> 
#include "rofl.h"
#include "../switch_port.h"
#include "../common/datapacket.h"

/**
* @file of_switch.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Openflow logical switch meta-abstraction
*
* The of_switch type contains the commonalities of an Openflow-enabled
* switch. of_switch structure is the version agnostic of
* any ofXX_switch type. Any version specific of_switch struct MUST
* share the same initial memory layout. 
*/

typedef enum{
	OF_VERSION_10 = 0x01,
	OF_VERSION_11 = 0x02,
	OF_VERSION_12 = 0x03,
	//Add more here...
}of_version_t;

#ifndef LOGICAL_SWITCH_MAX_LOG_PORTS
	#define LOGICAL_SWITCH_MAX_LOG_PORTS 129 //128, 0 slot NEVER used
#endif

//Platform dependent opaque state
typedef void of_switch_platform_state_t;

/**
* @ingroup core 
* Openflow-enabled switch abstraction (version-indepedent part).
*/
typedef struct of_switch{

	//Make sure ALL ofXX_switch_t contain this three fields
	//at the VERY beginning 

	/* This part is common and MUST be at the very beginning */ 
	of_version_t of_ver; 
	uint64_t dpid;
	char* name;
	unsigned int num_of_ports;
	
	//Switch logical ports 
	logical_switch_port_t logical_ports[LOGICAL_SWITCH_MAX_LOG_PORTS];
 	
	/**
	* Platform agnostic state. This is often used to store platform
	* handles for hw switches or the datapacket_t processing buffers in
	* the case of sw switches.
	*/
	of_switch_platform_state_t* platform_state;
	/* End of common part */

}of_switch_t;

typedef int of_packet_in_reason_t;

//C++ extern C
ROFL_BEGIN_DECLS

//Wrapping destroy
/**
* @brief  Destroys an Openflow logical switch.
* @ingroup core 
*
* of_destroy_switch() an Openflow logical switch. The switch MUST have
* been already removed from the physical switch pool via 
* physical_switch_remove_logical_switch_by_dpid() or physical_switch_remove_logical_switch()
*
* of_destroy_switch() will call the appropiate OF version 
* sepecific destroyer.
*
* Any other thread/hw thread/process, may not attempt to process packets via 
* of_process_packet_pipeline() before calling this function.
* 
*/
rofl_result_t of_destroy_switch(const of_switch_t* sw);

//Wrapping of processing
/**
* @brief Processes a packet through the Openflow pipeline.  
* @ingroup sw_runtime 
*
* This call may only be used if the platform does not have a hardware
* accelerated packet matching machinery.
*
* @param sw The switch which has to process the packet 
* @param pkt A datapacket_t instance. All the fields must be set to NULL
* except maybe platform_state 
* 
*/
rofl_result_t of_process_packet_pipeline(const of_switch_t* sw, datapacket_t *const pkt);

//Wrapping timers
/**
* @brief Processes flow entry expirations in all the pipeline tables of the switch.
* @ingroup sw_runtime 
*
* This call may only be used if the platform does not have a hardware
* accelerated timers machinery. 
*
* The platform has to periodically call of_process_pipeline_tables_timeout_expirations() 
* (usually via some background thread). The optimal period is around 500ms. 
*
* @param sw The switch which has to check flow entry expirations 
* 
*/
void of_process_pipeline_tables_timeout_expirations(const of_switch_t* sw);

//Wrapping port management
rofl_result_t of_get_switch_ports(of_switch_t* sw, logical_switch_port_t** ports, unsigned int* num_of_ports, unsigned int* logical_sw_max_ports);
rofl_result_t of_attach_port_to_switch_at_port_num(of_switch_t* sw, unsigned int port_num, switch_port_t* port);
rofl_result_t of_attach_port_to_switch(of_switch_t* sw, switch_port_t* port, unsigned int* port_num);
rofl_result_t of_detach_port_from_switch_by_port_num(of_switch_t* sw, unsigned int port_num);
rofl_result_t of_detach_port_from_switch(of_switch_t* sw, switch_port_t* port);
rofl_result_t of_detach_all_ports_from_switch(of_switch_t* sw);

//C++ extern C
ROFL_END_DECLS

#endif //OF_SWITCH
