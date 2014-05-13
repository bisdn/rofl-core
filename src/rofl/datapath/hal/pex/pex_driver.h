/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PEX_DRIVER_H
#define PEX_DRIVER_H

/**
* @file pex_driver.h
* @author Ivano Cerrato<ivano.cerrato (at) polito.it>
*
* @brief PEX related operations exposed to the CMM by the driver
* 
*/

#include "../hal.h"
#include "../hal_utils.h"
#include "../../pipeline/pex_connected.h"

//C++ extern C
HAL_BEGIN_DECLS

/**
*	According to the PEX type, the xDPD behaves differently when PEX commands
*	are called through the CMM.
*
*	@brief:	INTERNAL			PEX and xDPD are the same process
*	@brief: DPDK				PEX is a DPDK secondary process
*	@brief: EXTERNAL			PEX is something external, e.g., a VM
*/
enum PexType {INTERNAL,DPDK,EXTERNAL};

/**
 * @name hal_driver_pex_exists
 * @brief Checks if a PEX with the specified name exists
 *
 * @param pex_name	Name of the PEX to be checked
 */
bool hal_driver_pex_exists(const char *pex_name);

/**
* @brief   Retrieve the list of names of the available PEX of the platform 
* @ingroup pex_management
* @retval  List of available PEX names, which MUST be deleted using pex_name_list_destroy().
*/
pex_name_list_t* hal_driver_get_all_pex_names();

/**
 * @name    hal_result_t hal_driver_pex_create_pex
 * @brief   Instructs driver to create a new PEX 
 *
 * @param pex_name				Name of the PEX to be created
 * @param pex_type				Type of the PEX to be created
 * @param path					Path of the PEX excutable (may be not needed)

 */
hal_result_t hal_driver_pex_create_pex(const char *pex_name, PexType pexType, const char *path);

/**
 * @name    hal_result_t hal_driver_pex_destroy_pex
 * @brief   Instructs driver to destroy a PEX 
 *
 * @param pex_name		Name of the PEX to be destroyed
 */
hal_result_t hal_driver_pex_destroy_pex(const char *pex_name);

/**
 * @name    hal_result_t hal_driver_pex_start_pex
 * @brief   Instructs driver to start a PEX (associated with an existing PEX port)
 *
 * @param pex_id				Identifier of the PEX to be created
 */
hal_result_t hal_driver_pex_start_pex(uint32_t pex_id);

/**
 * @name    hal_result_t hal_driver_pex_stop_pex
 * @brief   Instructs driver to stop a PEX 
 *
 * @param pex_id		Identifier of the PEX to be stopped
 */
hal_result_t hal_driver_pex_stop_pex(uint32_t pex_id);


// [+] Add more here..

//C++ extern C
HAL_END_DECLS

#endif /* PEX_DRIVER_H */

