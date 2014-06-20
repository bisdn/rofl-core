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
*	According to the PEX type, the driver behaves differently when PEX commands
*	are called through the CMM.
*
*	@brief:	DPDK_PRIMARY			PEX and xDPD are the same process
*	@brief: DPDK_SECONDARY			PEX is a DPDK (secondary) process
*	@brief: DPDK_KNI				PEX is a Docker container
*/
enum PexType {DPDK_PRIMARY,DPDK_SECONDARY,DPDK_KNI};

/**
 * @name    hal_result_t hal_driver_pex_create_pex_port
 * @brief   Instructs driver to create a new PEX port
 *
 * @param pex_name				Name of the PEX associated with the port to be created
 * @param pex_port_name			Name of the PEX port to be created
 * @param pex_type				Type of the PEX to be created

 */
hal_result_t hal_driver_pex_create_pex_port(const char *pex_name, const char *pex_port_name, PexType pexType);

/**
 * @name    hal_result_t hal_driver_pex_destroy_pex_port
 * @brief   Instructs driver to destroy a PEX port
 *
 * @param pex_port_name		Name of the PEX port to be destroyed
 */
hal_result_t hal_driver_pex_destroy_pex_port(const char *pex_port_name);

/**
 * @name    hal_result_t hal_driver_pex_start_pex_port
 * @brief   Instructs driver to start a PEX port
 *
 * @param pex_port_name		Name of the PEX port to be started
 * @param pex_port_type		Type of the PEX port to be started
 */
hal_result_t hal_driver_pex_start_pex_port(const char *pex_port_name, port_type_t pex_port_type);

/**
 * @name    hal_result_t hal_driver_pex_stop_pex_port
 * @brief   Instructs driver to stop a PEX port
 *
 * @param pex_port_name		Name of the PEX port to be stopped
 * @param pex_port_type		Type of the PEX port to be stopped
 */
hal_result_t hal_driver_pex_stop_pex_port(const char *pex_port_name, port_type_t pex_port_type);


// [+] Add more here..

//C++ extern C
HAL_END_DECLS

#endif /* PEX_DRIVER_H */

