#ifndef __OF12_SWITCH_H__
#define __OF12_SWITCH_H__

#include <stdlib.h>
#include <string.h>
#include "../../switch_port.h"
#include "../of_switch.h"
#include "pipeline/of12_pipeline.h"
#include "../../util/rofl_pipeline_utils.h"
#include "../../platform/lock.h"

#ifndef LOGICAL_SWITCH_MAX_LOG_PORTS
	#define LOGICAL_SWITCH_MAX_LOG_PORTS 129 //128, 0 slot NEVER used
#endif

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

	uint16_t flags; 			// SET-CONFIG/GET-CONFIG
	uint16_t miss_send_len;			// SET-CONFIG/GET-CONFIG
};

typedef struct of12_switch of12_switch_t;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/* Initializer and destroyer */
of12_switch_t* of12_init_switch(const char* name, uint64_t dpid, unsigned int num_of_tables, enum matching_algorithm_available* list,of12_flow_table_config_t config);
unsigned int of12_destroy_switch(of12_switch_t* sw);

/* Port management */
unsigned int of12_get_switch_ports(of12_switch_t* sw, logical_switch_port_t** ports, unsigned int* num_of_ports, unsigned int* logical_sw_max_ports);
unsigned int of12_attach_port_to_switch_at_port_num(of12_switch_t* sw, unsigned int port_num, switch_port_t* port);
unsigned int of12_attach_port_to_switch(of12_switch_t* sw, switch_port_t* port, unsigned int* port_num);
unsigned int of12_detach_port_from_switch_by_port_num(of12_switch_t* sw, unsigned int port_num);
unsigned int of12_detach_port_from_switch(of12_switch_t* sw, switch_port_t* port);
unsigned int of12_detach_all_ports_from_switch(of12_switch_t* sw);

/* Dump */
void of12_dump_switch(of12_switch_t* sw);
void of12_full_dump_switch(of12_switch_t* sw);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_SWITCH
