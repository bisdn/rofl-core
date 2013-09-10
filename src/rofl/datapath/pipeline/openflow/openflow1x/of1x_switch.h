/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_SWITCH_H__
#define __OF1X_SWITCH_H__

#include <stdlib.h>
#include <string.h>
#include "rofl.h"
#include "../of_switch.h"
#include "../of_switch.h"
#include "pipeline/of1x_pipeline.h"
#include "../../platform/lock.h"

/**
* @file of1x_switch.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Openflow v1.2 logical switch abstraction
*
*/

#define OF1XP_NO_BUFFER	0xffffffff

/**
* @ingroup core_of1x 
* Openflow-enabled v1.2 switch abstraction
*/
typedef struct of1x_switch{
	
	//General switch instance information
	
	/* This part is common and MUST be at the very beginning */ 
	of_version_t of_ver; 
	uint64_t dpid;
	char* name;
	unsigned int max_ports;
	unsigned int num_of_ports;
	
	//Switch logical ports 
	logical_switch_port_t logical_ports[LOGICAL_SWITCH_MAX_LOG_PORTS];
 	
	//Platform agnostic pointer
	of_switch_platform_state_t* platform_state;
	/* End of common part */

	//pipeline
	of1x_pipeline_t* pipeline;
	
	//Mutex
	platform_mutex_t* mutex;

}of1x_switch_t;

//C++ extern C
ROFL_BEGIN_DECLS

/* Initializer and destroyer */
/**
* @brief Creates an Openflow v1.2 forwarding instance.  
* @ingroup core_of1x 
* @param num_of_tables Number of tables that the v1.2 pipeline should have. This is immutable 
* during the lifetime of the switch.
* @param ma_list An array with num_of_tables, with the matching algorithm that should
* be used in each table (0..num_of_tables-1) 
*/
of1x_switch_t* of1x_init_switch(const char* name, of_version_t version, uint64_t dpid, unsigned int num_of_tables, enum of1x_matching_algorithm_available* ma_list);

rofl_result_t __of1x_destroy_switch(of1x_switch_t* sw);

/* Port management */
rofl_result_t __of1x_attach_port_to_switch_at_port_num(of1x_switch_t* sw, unsigned int port_num, switch_port_t* port);
rofl_result_t __of1x_attach_port_to_switch(of1x_switch_t* sw, switch_port_t* port, unsigned int* port_num);
rofl_result_t __of1x_detach_port_from_switch_by_port_num(of1x_switch_t* sw, unsigned int port_num);
rofl_result_t __of1x_detach_port_from_switch(of1x_switch_t* sw, switch_port_t* port);
rofl_result_t __of1x_detach_all_ports_from_switch(of1x_switch_t* sw);

/* Dump */
/**
* @brief Dumps the Openflow v1.2 forwarding instance, for debugging purposes.  
* @ingroup core_of1x 
*/
void of1x_dump_switch(of1x_switch_t* sw);
/**
* @brief Dumps the Openflow v1.2 forwarding instance, for debugging purposes.  
* @ingroup core_of1x 
*/
void of1x_full_dump_switch(of1x_switch_t* sw);

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_SWITCH
