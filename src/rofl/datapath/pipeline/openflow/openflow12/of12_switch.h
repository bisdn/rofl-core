#ifndef __OF12_SWITCH_H__
#define __OF12_SWITCH_H__

#include <stdlib.h>
#include <string.h>
#include "rofl.h"
#include "../of_switch.h"
#include "../of_switch.h"
#include "pipeline/of12_pipeline.h"
#include "../../util/rofl_pipeline_utils.h"
#include "../../platform/lock.h"

struct of12_switch{
	
	//General switch instance information
	
	/* This part is common and MUST be at the very beginning */ 
	of_version_t of_ver; 
	uint64_t dpid;
	char* name;
	unsigned int num_of_ports;
	
	//Switch logical ports 
	logical_switch_port_t logical_ports[LOGICAL_SWITCH_MAX_LOG_PORTS];
 	
	//Platform agnostic pointer
	of_switch_platform_state_t* platform_state;
	/* End of common part */

	//pipeline
	of12_pipeline_t* pipeline;
	
	//Mutex
	platform_mutex_t* mutex;

};

typedef struct of12_switch of12_switch_t;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/* Initializer and destroyer */
of12_switch_t* of12_init_switch(const char* name, uint64_t dpid, unsigned int num_of_tables, enum matching_algorithm_available* list,of12_flow_table_miss_config_t config);
rofl_result_t of12_destroy_switch(of12_switch_t* sw);

/* Port management */
rofl_result_t of12_get_switch_ports(of12_switch_t* sw, logical_switch_port_t** ports, unsigned int* num_of_ports, unsigned int* logical_sw_max_ports);
rofl_result_t of12_attach_port_to_switch_at_port_num(of12_switch_t* sw, unsigned int port_num, switch_port_t* port);
rofl_result_t of12_attach_port_to_switch(of12_switch_t* sw, switch_port_t* port, unsigned int* port_num);
rofl_result_t of12_detach_port_from_switch_by_port_num(of12_switch_t* sw, unsigned int port_num);
rofl_result_t of12_detach_port_from_switch(of12_switch_t* sw, switch_port_t* port);
rofl_result_t of12_detach_all_ports_from_switch(of12_switch_t* sw);

/* Dump */
void of12_dump_switch(of12_switch_t* sw);
void of12_full_dump_switch(of12_switch_t* sw);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_SWITCH
