#ifndef __PHYSICAL_SWITCH_H__
#define __PHYSICAL_SWITCH_H__

#include <stdlib.h> 
#include <string.h> 
#include "rofl.h"
#include "platform/lock.h"
#include "util/rofl_pipeline_utils.h"
#include "openflow/of_switch.h"
#include "switch_port.h"

/*
*
* Implements the physical switch state 
* containing multiple logical switch instances 
*      Author: msune
*
*/

#ifndef PHYSICAL_SWITCH_MAX_LS
	#define PHYSICAL_SWITCH_MAX_LS 64
#endif

#ifndef PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS
	#define PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS 48 
#endif

#ifndef PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS
	#define PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS 128
#endif

#ifndef PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS
	#define PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS 32
#endif
	
#define PHYSICAL_SWITCH_MAX_NUM_META_PORTS 8

/*
*
* Physical switch abstraction
*
*/

//Opaque platform state (to be used, maybe, for platform hooks)
typedef void platform_physical_switch_state_t;

struct physical_switch{

	/*
	* List of all logical switches in the system
	*/
	unsigned int num_of_logical_switches;
	of_switch_t* logical_switches[PHYSICAL_SWITCH_MAX_LS];

	/*
	* Ports
	*/
	//physical: index is the physical port of the platform.
	switch_port_t* physical_ports[PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS];

	//tunnel ports
	switch_port_t* tunnel_ports[PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS]; //Not used yet

	//virtual ports (which are not tunnel)
	switch_port_t* virtual_ports[PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS]; //Not used yet

	//meta ports (esoteric ports). This is NOT an array of pointers!
	switch_port_t meta_ports[PHYSICAL_SWITCH_MAX_NUM_META_PORTS]; 

	/* 
	* Other state 
	*/
	//Mutex
	platform_mutex_t* mutex;
	
	//Opaque platform specific extra state 
	platform_physical_switch_state_t* platform_state;	
};
typedef struct physical_switch physical_switch_t; 

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/* Switch and logical switch management */
// init&destroy
void physical_switch_init(void);
physical_switch_t* get_physical_switch(void);
void physical_switch_destroy(void);

//Add/remove methods
rofl_result_t physical_switch_add_logical_switch(of_switch_t* sw);
rofl_result_t physical_switch_remove_logical_switch_by_dpid(const uint64_t dpid);
rofl_result_t physical_switch_remove_logical_switch(of_switch_t* sw);

//Getters
of_switch_t* physical_switch_get_logical_switch_by_dpid(const uint64_t dpid);
of_switch_t* physical_switch_get_logical_switch_attached_to_port(const switch_port_t port);
switch_port_t* physical_switch_get_port_by_name(const char *name);

/* Port management routines*/

//Physical switch list
void physical_switch_get_physical_ports(switch_port_t* ports, unsigned int* num_of_ports);
/*
TODO: add create virtual link and add/remove tunnel.
//void physical_switch_get_virtual_ports(switch_port_t* ports, unsigned int* num_of_ports);
//void physical_switch_get_tunnel_ports(switch_port_t* ports, unsigned int* num_of_ports);
*/

//Logical switch port management
rofl_result_t get_logical_switch_ports(of_switch_t* sw, logical_switch_port_t** ports, unsigned int* num_of_ports, unsigned int* logical_sw_max_ports);
rofl_result_t physical_switch_attach_port_num_to_logical_switch(unsigned int port_num, of_switch_t* sw, unsigned int* logical_switch_port_num);
rofl_result_t physical_switch_attach_port_to_logical_switch(switch_port_t* port, of_switch_t* sw, unsigned int* port_num);
rofl_result_t physical_switch_attach_port_to_logical_switch_at_port_num(switch_port_t* port, of_switch_t* sw, unsigned int port_num);
rofl_result_t physical_switch_detach_port_num_from_logical_switch(unsigned int port_num, of_switch_t* sw);
rofl_result_t physical_switch_detach_port_from_logical_switch(switch_port_t* port, of_switch_t* sw);
rofl_result_t physical_switch_detach_all_ports_from_logical_switch(of_switch_t* sw);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //PHYSICAL_SWITCH
