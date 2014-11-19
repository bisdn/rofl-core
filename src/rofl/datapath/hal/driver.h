/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HAL_DRIVER_H
#define HAL_DRIVER_H 

#include <stdbool.h>
#include <rofl/datapath/pipeline/monitoring.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/pipeline/switch_port.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/matching_algorithms.h>

#include "hal.h"
#include "hal_utils.h"
#include "extensions/extensions.h"

/**
* @file driver.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief HAL driver management interface
*/

//driver_info_t char array lengths
#define DRIVER_CODE_NAME_MAX_LEN 16
#define DRIVER_VERSION_MAX_LEN 16 
#define DRIVER_DESCRIPTION_MAX_LEN 2048*4
#define DRIVER_USAGE_MAX_LEN 2048*4
#define DRIVER_EXTRA_PARAMS_MAX_LEN 2048*4

/** 
* Driver information structure 
* @ingroup hal_driver
*/
typedef struct{
	/**
	* Driver code name aaa-bbb-ccc
	*/
	char code_name[DRIVER_CODE_NAME_MAX_LEN];
	/**
	* Version and (optionally) build number
	*/
	char version[DRIVER_VERSION_MAX_LEN];
	/**
	* Detailed description of the driver
	*/	
	char description[DRIVER_DESCRIPTION_MAX_LEN];
	/**
	* Usage details, specially extra parameters and its synax (if supported)
	*/	
	char usage[DRIVER_USAGE_MAX_LEN];
	/**
	* String containing the *parsed and understood* extra_params
	*/
	char extra_params[DRIVER_EXTRA_PARAMS_MAX_LEN];	
}driver_info_t;

//C++ extern C
HAL_BEGIN_DECLS

//Init&destroy driver
/**
* 
* @brief   Initialises driver. Before using the HAL_DRIVER routines, higher layers must allow driver to initialise itself
* @ingroup hal_driver
* @param extensions Driver must set all the function pointers for supported extensions or NULL. If none are supported, struct can be entirely memset to 0.
* @param extra_params Driver specific extra parameters string. Refer to the usage for details of this parameter. Most drivers
* do not use it
*/
hal_result_t hal_driver_init(hal_extension_ops_t* extensions, const char* extra_params);

/**
* 
* @brief   Get the information of the driver (code-name, version, usage...)
* @ingroup hal_driver
* @warning This call MUST be able to be called *before* hal_driver_init() is called 
*/
void hal_driver_get_info(driver_info_t* info);

/**
* 
* @brief   Destroy driver state. Allows platform state to be properly released. 
* @ingroup hal_driver
*/
hal_result_t hal_driver_destroy(void);

//Monitroing 

/**
* 
* @brief   Get the chassis information in the form of a monitored entity.
* @ingroup hal_driver
*/
monitored_entity_t* hal_driver_get_chassis_info(void);

/**
*
* Switch management functions
*
* All of the functions related to switch creation and destruction
* port management and other configuration
*/

/**
* @brief   Checks if an LSI with the specified dpid exists 
* @ingroup hal_driver
*/
bool hal_driver_switch_exists(uint64_t dpid); 

/**
* @brief   Retrieve the list of LSIs dpids
* @ingroup hal_driver
* @retval  List of available dpids, which MUST be deleted using dpid_list_destroy().
*/
dpid_list_t* hal_driver_get_all_lsi_dpids(void);

/**
* 
* @brief   Instruct driver to create an OF logical switch 
* @ingroup hal_driver
*/
hal_result_t hal_driver_create_switch(char* name, uint64_t dpid, of_version_t of_version, unsigned int num_of_tables, int* ma_list);

/**
 * 
 * @brief Retrieves a snapshot of the current state of a switch port, if the port name is found. The snapshot MUST be deleted using of_switch_destroy_snapshot() 
 * @ingroup hal_driver
 * @retval  Pointer to of_switch_snapshot_t instance or NULL 
 */
of_switch_snapshot_t* hal_driver_get_switch_snapshot_by_dpid(uint64_t dpid);

/**
* 
* @brief   Instructs the driver to destroy the switch with the specified dpid 
* @ingroup hal_driver
*/
hal_result_t hal_driver_destroy_switch_by_dpid(uint64_t dpid);


/**
* @brief   Checks if a port with the specified name exists 
* @ingroup hal_driver
*/
bool hal_driver_port_exists(const char *name);

/**
* @brief   Retrieve the list of names of the available ports of the platform. You may want to 
* 	   call hal_driver_get_port_snapshot_by_name(name) to get more information of the port 
* @ingroup hal_driver
* @retval  List of available port names, which MUST be deleted using switch_port_name_list_destroy().
*/
switch_port_name_list_t* hal_driver_get_all_port_names(void);

/**
 * @brief Retrieves a snapshot of the current state of a switch port, if the port name is found. The snapshot MUST be deleted using switch_port_destroy_snapshot()
 * @ingroup hal_driver
 */
switch_port_snapshot_t* hal_driver_get_port_snapshot_by_name(const char *name);

/**
 * @brief Retrieves a snapshot of the current state of the port of the Logical Switch Instance with dpid at port_num, if exists. The snapshot MUST be deleted using switch_port_destroy_snapshot()
 * @ingroup hal_driver
 * @param dpid DatapathID 
 * @param port_num Port number
 */
switch_port_snapshot_t* hal_driver_get_port_snapshot_by_num(uint64_t dpid, unsigned int port_num);


//Attachment

/**
* @brief   Attempts to attach a system's port to switch, at port_num if defined, otherwise in the first empty port number.
* @ingroup hal_driver
*
* @param dpid Datapath ID of the switch to attach the ports to
* @param name Port name (system's name)
* @param port_num If *port_num is non-zero, try to attach to port_num of the logical switch, otherwise try to attach to the first available port and return the result in port_num
*/
hal_result_t hal_driver_attach_port_to_switch(uint64_t dpid, const char* name, unsigned int* port_num);

/**
* @brief   Attempts to connect two logical switches via a virtual port. Driver may or may not support this functionality. On success, the two ports must be functional and process packets and the driver MUST inform the CMM of the new ports via two separate port_add messages, with the appropriate information of attachment of the ports. 
* @ingroup hal_driver
*
* @param dpid_lsi1 Datapath ID of the LSI1
* @param port_num1 If *port_num is non-zero, try to attach to port_num of the logical switch, otherwise try to attach to the first available port and return the result in port_num
* @param port1 A pointer to a snapshot of the virtual port attached to the LS1 that MUST be destroyed using switch_port_destroy_snapshot()
* @param dpid_lsi2 Datapath ID of the LSI2
* @param port_num2 If *port_num is non-zero, try to attach to port_num of the logical switch, otherwise try to attach to the first available port and return the result in port_num
* @param port1 A pointer to a snapshot of the virtual port attached to the LS2 that MUST be destroyed using switch_port_destroy_snapshot()
*/
hal_result_t hal_driver_connect_switches(uint64_t dpid_lsi1, unsigned int* port_num1, switch_port_snapshot_t** port1, uint64_t dpid_lsi2, unsigned int* port_num2, switch_port_snapshot_t** port2);


/**
* @brief   Detaches a port from the switch. If as a consequence of the detachment of a port, one or more ports are deleted and detached (e.g. virtual ports or links), the driver MUST inform the CMM of these deletions via separate port_delete messages, with the appropriate information of attachment (marked as attached and with a valid dpid). 
* @ingroup hal_driver
*
* @param dpid Datapath ID of the switch to detach the ports
* @param name Port name (system's name)
*/
hal_result_t hal_driver_detach_port_from_switch(uint64_t dpid, const char* name);

/**
* @brief   Detaches port_num of the logical switch identified with dpid 
* @ingroup hal_driver
*
* @param dpid Datapath ID of the switch to detach the ports
* @param port_num Port number 
*/
hal_result_t hal_driver_detach_port_from_switch_at_port_num(uint64_t dpid, const unsigned int port_num);

//Port control

/**
* @brief   Brings up a system's port. The function shall call the CMM with the appropriate port_status_changed message. 
* @ingroup hal_driver
*
* @param name Port system name 
*/
hal_result_t hal_driver_bring_port_up(const char* name);

/**
* @brief   Brings down a system's port. The function shall call the CMM with the appropriate port_status_changed message. 
* @ingroup hal_driver
*
* @param name Port system name 
*/
hal_result_t hal_driver_bring_port_down(const char* name);

/**
* @brief   Brings up a port from a Logical Switch Instance. The function shall call the CMM with the appropriate port_status_changed message. 
* @ingroup hal_driver
*
* @param dpid DatapathID 
* @param port_num Port number
*/
hal_result_t hal_driver_bring_port_up_by_num(uint64_t dpid, unsigned int port_num);

/**
* @brief   Brings down a port from a Logical Switch Instance. The function shall call the CMM with the appropriate port_status_changed message.
* @ingroup hal_driver
*
* @param dpid DatapathID 
* @param port_num Port number
*/
hal_result_t hal_driver_bring_port_down_by_num(uint64_t dpid, unsigned int port_num);


/**
 * @brief Retrieve a snapshot of the monitoring state. If rev is 0, or the current monitoring 
 * has changed (monitoring->rev != rev), a new snapshot of the monitoring state is made. Warning: this 
 * is expensive.
 * @ingroup hal_driver
 *
 * @param rev Last seen revision. Set to 0 to always get a new snapshot 
 * @return A snapshot of the monitoring state that MUST be destroyed using monitoring_destroy_snapshot() or NULL if there have been no changes (same rev)
 */ 
monitoring_snapshot_state_t* hal_driver_get_monitoring_snapshot(uint64_t rev);

/**
 * @brief get a list of available matching algorithms
 * @ingroup hal_driver
 *
 * @param of_version
 * @param name_list
 * @param count
 */
hal_result_t hal_driver_list_matching_algorithms(of_version_t of_version, const char * const** name_list, int *count);


//C++ extern C
HAL_END_DECLS

/* OpenFlow related events */
#include "openflow/openflow1x/of1x_driver.h"
// [+] Add more versions here...

#endif /* HAL_DRIVER_H */
