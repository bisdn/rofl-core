/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AFA_DRIVER_H
#define AFA_DRIVER_H 

#include <stdbool.h>
#include <rofl/datapath/pipeline/monitoring.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/pipeline/switch_port.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/matching_algorithms.h>

#include "afa.h"
#include "afa_utils.h"


//fwd_module_info_t char array lengths
#define FWD_MOD_CODE_NAME_MAX_LEN 16
#define FWD_MOD_VERSION_MAX_LEN 16 
#define FWD_MOD_DESCRIPTION_MAX_LEN 2048*4
#define FWD_MOD_USAGE_MAX_LEN 2048*4
#define FWD_MOD_EXTRA_PARAMS_MAX_LEN 2048*4

/** 
* Forwarding 
*/
typedef struct{
	/**
	* Forwarding module code name aaa-bbb-ccc
	*/
	char code_name[FWD_MOD_CODE_NAME_MAX_LEN];
	/**
	* Version and (optionally) build number
	*/
	char version[FWD_MOD_VERSION_MAX_LEN];
	/**
	* Detailed description of the forwarding module
	*/	
	char description[FWD_MOD_DESCRIPTION_MAX_LEN];
	/**
	* Usage details, specially extra parameters and its synax (if supported)
	*/	
	char usage[FWD_MOD_USAGE_MAX_LEN];
	/**
	* String containing the *parsed and understood* extra_params
	*/
	char extra_params[FWD_MOD_EXTRA_PARAMS_MAX_LEN];	
}fwd_module_info_t;

//C++ extern C
AFA_BEGIN_DECLS

//Init&destroy forward module
/**
* @name    fwd_module_init
* @brief   Initialises forward module. Before using the AFA_DRIVER routines, higher layers must allow forward module to initialise itself
* @ingroup fwd_module_management
* @param extra_params Forwarding module specific extra parameters string. Refer to the usage for details of this parameter. Most fwd_modules
* do not use it
*/
afa_result_t fwd_module_init(const char* extra_params);

/**
* @name    fwd_module_get_info
* @brief   Get the information of the forwarding_module (code-name, version, usage...)
* @ingroup fwd_module_management
* @warning Calling fwd_module_get_info() before fwd_module_init() has an undefined behaviour
*/
void fwd_module_get_info(fwd_module_info_t* info);

/**
* @name    fwd_module_destroy
* @brief   Destroy forward module state. Allows platform state to be properly released. 
* @ingroup fwd_module_management
*/
afa_result_t fwd_module_destroy(void);


/*
* Monitoring
*/

/**
* @name    fwd_module_get_chassis_info
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
* @brief   Checks if an LSI with the specified dpid exists 
* @ingroup logical_switch_management
*/
bool fwd_module_switch_exists(uint64_t dpid); 

/**
* @brief   Retrieve the list of LSIs dpids
* @ingroup logical_switch_management
* @retval  List of available dpids, which MUST be deleted using dpid_list_destroy().
*/
dpid_list_t* fwd_module_get_all_lsi_dpids(void);

/**
* @name    fwd_module_create_switch 
* @brief   Instruct forward module to create an OF logical switch 
* @ingroup logical_switch_management
*/
afa_result_t fwd_module_create_switch(char* name, uint64_t dpid, of_version_t of_version, unsigned int num_of_tables, int* ma_list);

/**
 * @name fwd_module_get_switch_snapshot_by_dpid 
 * @brief Retrieves a snapshot of the current state of a switch port, if the port name is found. The snapshot MUST be deleted using switch_port_destroy_snapshot()
 * @ingroup logical_switch_management
 * @retval  Pointer to of_switch_snapshot_t instance or NULL 
 */
of_switch_snapshot_t* fwd_module_get_switch_snapshot_by_dpid(uint64_t dpid);

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
/**
* @brief   Checks if a port with the specified name exists 
* @ingroup port_management 
*/
bool fwd_module_port_exists(const char *name);

/**
* @brief   Retrieve the list of names of the available ports of the platform. You may want to 
* 	   call fwd_module_get_port_snapshot_by_name(name) to get more information of the port 
* @ingroup port_management
* @retval  List of available port names, which MUST be deleted using switch_port_name_list_destroy().
*/
switch_port_name_list_t* fwd_module_get_all_port_names(void);

/**
 * @name fwd_module_get_port_by_name
 * @brief Retrieves a snapshot of the current state of a switch port, if the port name is found. The snapshot MUST be deleted using switch_port_destroy_snapshot()
 * @ingroup port_management
 */
switch_port_snapshot_t* fwd_module_get_port_snapshot_by_name(const char *name);

/**
 * @name fwd_module_get_port_by_num
 * @brief Retrieves a snapshot of the current state of the port of the Logical Switch Instance with dpid at port_num, if exists. The snapshot MUST be deleted using switch_port_destroy_snapshot()
 * @ingroup port_management
 * @param dpid DatapathID 
 * @param port_num Port number
 */
switch_port_snapshot_t* fwd_module_get_port_snapshot_by_num(uint64_t dpid, unsigned int port_num);


//Attachment

/**
* @name    fwd_module_attach_physical_port_to_switch
* @brief   Attempts to attach a system's port to switch, at port_num if defined, otherwise in the first empty port number.
* @ingroup management
*
* @param dpid Datapath ID of the switch to attach the ports to
* @param name Port name (system's name)
* @param port_num If *port_num is non-zero, try to attach to port_num of the logical switch, otherwise try to attach to the first available port and return the result in port_num
*/
afa_result_t fwd_module_attach_port_to_switch(uint64_t dpid, const char* name, unsigned int* port_num);

/**
* @name    fwd_module_connect_switches
* @brief   Attempts to connect two logical switches via a virtual port. Forwarding module may or may not support this functionality. On success, the two ports must be functional and process packets and the fwd_module MUST inform the CMM of the new ports via two separate port_add messages, with the appropriate information of attachment of the ports. 
* @ingroup management
*
* @param dpid_lsi1 Datapath ID of the LSI1
* @param port1 A pointer to a snapshot of the virtual port attached to the LS1 that MUST be destroyed using switch_port_destroy_snapshot()
* @param dpid_lsi2 Datapath ID of the LSI2
* @param port1 A pointer to a snapshot of the virtual port attached to the LS2 that MUST be destroyed using switch_port_destroy_snapshot()
*/
afa_result_t fwd_module_connect_switches(uint64_t dpid_lsi1, switch_port_snapshot_t** port1, uint64_t dpid_lsi2, switch_port_snapshot_t** port2);


/**
* @name    fwd_module_detach_port_from_switch
* @brief   Detaches a port from the switch. If as a consequence of the detachment of a port, one or more ports are deleted and detached (e.g. virtual ports or links), the fwd_module MUST inform the CMM of these deletions via separate port_delete messages, with the appropriate information of attachment (marked as attached and with a valid dpid). 
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
* @param port_num Port number 
*/
afa_result_t fwd_module_detach_port_from_switch_at_port_num(uint64_t dpid, const unsigned int port_num);

//Port control

/**
* @name    fwd_module_bring_port_up
* @brief   Brings up a system's port. The function shall call the CMM with the appropriate port_status_changed message. 
* @ingroup port_management
*
* @param name Port system name 
*/
afa_result_t fwd_module_bring_port_up(const char* name);

/**
* @name    fwd_module_bring_port_down
* @brief   Brings down a system's port. The function shall call the CMM with the appropriate port_status_changed message. 
* @ingroup port_management
*
* @param name Port system name 
*/
afa_result_t fwd_module_bring_port_down(const char* name);

/**
* @name    fwd_module_bring_port_up_by_num
* @brief   Brings up a port from a Logical Switch Instance. The function shall call the CMM with the appropriate port_status_changed message. 
* @ingroup port_management
*
* @param dpid DatapathID 
* @param port_num Port number
*/
afa_result_t fwd_module_bring_port_up_by_num(uint64_t dpid, unsigned int port_num);

/**
* @name    fwd_module_bring_port_down_by_num
* @brief   Brings down a port from a Logical Switch Instance. The function shall call the CMM with the appropriate port_status_changed message.
* @ingroup port_management
*
* @param dpid DatapathID 
* @param port_num Port number
*/
afa_result_t fwd_module_bring_port_down_by_num(uint64_t dpid, unsigned int port_num);


/**
 * @brief Retrieve a snapshot of the monitoring state. If rev is 0, or the current monitoring 
 * has changed (monitoring->rev != rev), a new snapshot of the monitoring state is made. Warning: this 
 * is expensive.
 * @ingroup fwd_module_management
 *
 * @param rev Last seen revision. Set to 0 to always get a new snapshot 
 * @return A snapshot of the monitoring state that MUST be destroyed using monitoring_destroy_snapshot() or NULL if there have been no changes (same rev)
 */ 
monitoring_snapshot_state_t* fwd_module_get_monitoring_snapshot(uint64_t rev);

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
// [+] Add more versions here...

#endif /* AFA_DRIVER_H */
