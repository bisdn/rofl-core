#ifndef __OF_SWITCH_H__
#define __OF_SWITCH_H__

#include <inttypes.h> 
#include "../switch_port.h"
#include "../util/rofl_pipeline_utils.h"
#include "../common/datapacket.h"

/*
* This structure is meant to be a simple wrapper for multiple version
* of OF switches, in order to be able to correctly derreference a
* pointer to a switch. 
*/

typedef enum{
	OF_VERSION_10 = 0x01,
	OF_VERSION_11 = 0x02,
	OF_VERSION_12 = 0x03,
	//Add more here...
}of_version_t;

//Opaque table config (OF version agnostic)
typedef uint32_t of_flow_table_config_t; 

//Platform dependent opaque state
typedef void of_switch_platform_state_t;

 struct of_switch{

	//Make sure ALL ofXX_switch_t contain this three fields
	//at the VERY beginning 

	/* This part is common and MUST be at the very beginning */ 
	of_version_t of_ver; 
	uint64_t dpid;
	char* name;
	unsigned int num_of_ports;
	
	//Platform agnostic pointer
	of_switch_platform_state_t* platform_state;
	/* End of common part */

};
typedef struct of_switch of_switch_t;

typedef int of_packet_in_reason_t;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

//Wrapping destroy
unsigned int of_destroy_switch(const of_switch_t* sw);

//Wrapping of processing
unsigned int of_process_packet_pipeline(const of_switch_t* sw, datapacket_t *const pkt);

//Wrapping timers
void of_process_pipeline_tables_timeout_expirations(const of_switch_t* sw);

//Wrapping port management
unsigned int of_get_switch_ports(of_switch_t* sw, logical_switch_port_t** ports, unsigned int* num_of_ports, unsigned int* logical_sw_max_ports);
unsigned int of_attach_port_to_switch_at_port_num(of_switch_t* sw, unsigned int port_num, switch_port_t* port);
unsigned int of_attach_port_to_switch(of_switch_t* sw, switch_port_t* port, unsigned int* port_num);
unsigned int of_detach_port_from_switch_by_port_num(of_switch_t* sw, unsigned int port_num);
unsigned int of_detach_port_from_switch(of_switch_t* sw, switch_port_t* port);
unsigned int of_detach_all_ports_from_switch(of_switch_t* sw);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF_SWITCH
