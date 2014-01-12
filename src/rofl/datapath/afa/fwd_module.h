/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AFA_DRIVER_H
#define AFA_DRIVER_H 

#include <rofl/datapath/pipeline/monitoring.h>
#include <rofl/datapath/pipeline/switch_port.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/matching_algorithms.h>

#include "afa.h"
#include "afa_utils.h"

//C++ extern C
AFA_BEGIN_DECLS

//Init&destroy forward module
/**
* @name    fwd_module_init
* @brief   Initializes forward module. Before using the AFA_DRIVER routines, higher layers must allow forward module to initialize itself
* @ingroup fwd_module_management
*/
afa_result_t fwd_module_init(void);

/**
* @name    fwd_module_destroy
* @brief   Destroy forward module state. Allows platform state to be properly released. 
* @ingroup fwd_module_management
*/
afa_result_t fwd_module_destroy(void);

/**
* @name    fwd_module_get_chassis_info_
* @brief   Get the chassis information in the form of a monitored entity.
* @ingroup fwd_module_management
*/
monitored_entity_t* fwd_module_get_chassis_info(void);

/**
*
* Switch management functions
*
* All of the functions related to switch creation and destruction
* port management and other configuration
*/

/**
* @name    fwd_module_create_switch 
* @brief   Instruct forward module to create an OF logical switch 
* @ingroup logical_switch_management
* @retval  Pointer to of_switch_t instance 
*/
of_switch_t* fwd_module_create_switch(char* name, uint64_t dpid, of_version_t of_version, unsigned int num_of_tables, int* ma_list/**TODO add this list to the configuration*/);

/**
* @name    fwd_module_get_switch_by_dpid 
* @brief   Retrieve the switch with the specified dpid  
* @ingroup logical_switch_management
* @retval  Pointer to of_switch_t instance or NULL 
*/
of_switch_t* fwd_module_get_switch_by_dpid(uint64_t dpid);

/**
* @name    fwd_module_destroy_switch_by_dpid 
* @brief   Instructs the forward module to destroy the switch with the specified dpid 
* @ingroup logical_switch_management
*/
afa_result_t fwd_module_destroy_switch_by_dpid(uint64_t dpid);


/**
*
* Platform port management
*
* All of the functions related to platform port management 
*
*/

/***
 * @name fwd_module_get_port_by_name
 * @brief returns the port structure found with the specified name or NULL if its not found
 * @ingroup port_management
 */
switch_port_t* fwd_module_get_port_by_name(const char *name);

/**
* @name    fwd_module_list_platform_ports
* @brief   Retrieve the list of ports of the platform 
* @ingroup port_management
* @retval  Pointer to the first port. 
*/
//NOTE probably not in use anymore
switch_port_t* fwd_module_list_ports(void);

/**
* @name    fwd_module_get_physical_ports_ports
* @brief   Retrieve the list of the physical ports of the switch
* @ingroup port_management
* @param   Number of ports in the array(boundary)
* @retval  Pointer to the first port in the array of switch_port_t*. This array cannot
*          be modified, is READ_ONLY!
*/
switch_port_t** fwd_module_get_physical_ports(unsigned int* max_ports);

/**
* @name    fwd_module_get_virtual_ports
* @brief   Retrieve the list of virtual ports of the platform
* @ingroup port_management
* @param   Number of ports in the array(boundary)
* @retval  Pointer to the first port in the array of switch_port_t*. This array cannot
*          be modified, is READ_ONLY! 
*/
switch_port_t** fwd_module_get_virtual_ports(unsigned int* max_ports);

/**
* @name    fwd_module_get_tunnel_ports
* @brief   Retrieve the list of tunnel ports of the platform
* @ingroup port_management
* @param   Number of ports in the array(boundary)
* @retval  Pointer to the first port in the array of switch_port_t*. This array cannot
*          be modified, is READ_ONLY!
*/
switch_port_t** fwd_module_get_tunnel_ports(unsigned int* max_ports);


//Attachment

/**
* @name    fwd_module_attach_physical_port_to_switch
* @brief   Attemps to attach a system's port to switch, at of_port_num if defined, otherwise in the first empty OF port number.
* @ingroup management
*
* @param dpid Datapath ID of the switch to attach the ports to
* @param name Port name (system's name)
* @param of_port_num If *of_port_num is non-zero, try to attach to of_port_num of the logical switch, otherwise try to attach to the first available port and return the result in of_port_num
*/
afa_result_t fwd_module_attach_port_to_switch(uint64_t dpid, const char* name, unsigned int* of_port_num);

/**
* @name    fwd_module_connect_switches
* @brief   Attemps to connect two logical switches via a virtual port. Forwarding module may or may not support this functionality. On success, the two ports must be functional and process packets. 
* @ingroup management
*
* @param dpid_lsi1 Datapath ID of the LSI1
* @param port1 A pointer to the virtual port attached to the LS1
* @param dpid_lsi2 Datapath ID of the LSI2
* @param port1 A pointer to the virtual port attached to the LS2
*/
afa_result_t fwd_module_connect_switches(uint64_t dpid_lsi1, switch_port_t** port1, uint64_t dpid_lsi2, switch_port_t** port2);


/**
* @name    fwd_module_detach_port_from_switch
* @brief   Detaches a port from the switch 
* @ingroup port_management
*
* @param dpid Datapath ID of the switch to detach the ports
* @param name Port name (system's name)
*/
afa_result_t fwd_module_detach_port_from_switch(uint64_t dpid, const char* name);

/**
* @name    fwd_module_detach_port_from_switch_at_port_num
* @brief   Detaches port_num of the logical switch identified with dpid 
* @ingroup port_management
*
* @param dpid Datapath ID of the switch to detach the ports
* @param of_port_num Number of the port (OF number) 
*/
afa_result_t fwd_module_detach_port_from_switch_at_port_num(uint64_t dpid, const unsigned int of_port_num);

//Port control

/**
* @name    fwd_module_enable_port
* @brief   Brings up a system port. If the port is attached to an OF logical switch, this also schedules port for I/O and triggers PORTMOD message. 
* @ingroup port_management
*
* @param name Port system name 
*/
afa_result_t fwd_module_enable_port(const char* name);

/**
* @name    fwd_module_disable_port
* @brief   Shutdowns (brings down) a system port. If the port is attached to an OF logical switch, this also de-schedules port and triggers PORTMOD message. 
* @ingroup port_management
*
* @param name Port system name 
*/
afa_result_t fwd_module_disable_port(const char* name);

/**
* @name    fwd_module_enable_port_by_num
* @brief   Brings up a port from an OF logical switch (and the underlying physical interface). This function also triggers the PORTMOD message 
* @ingroup port_management
*
* @param dpid DatapathID 
* @param port_num OF port number
*/
afa_result_t fwd_module_enable_port_by_num(uint64_t dpid, unsigned int port_num);

/**
* @name    fwd_module_disable_port_by_num
* @brief   Brings down a port from an OF logical switch (and the underlying physical interface). This also triggers the PORTMOD message.
* @ingroup port_management
*
* @param dpid DatapathID 
* @param port_num OF port number
*/
afa_result_t fwd_module_disable_port_by_num(uint64_t dpid, unsigned int port_num);

/**
 * @brief get a list of available matching algorithms
 * @ingroup fwd_module_management
 *
 * @param of_version
 * @param name_list
 * @param count
 * @return
 */
afa_result_t fwd_module_list_matching_algorithms(of_version_t of_version, const char * const** name_list, int *count);


//C++ extern C
AFA_END_DECLS

/* OpenFlow related events */
#include "openflow/openflow1x/of1x_fwd_module.h"
//TODO: Add more versions here...

#endif /* AFA_DRIVER_H */
