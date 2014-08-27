/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef NF_DRIVER_H
#define NF_DRIVER_H

/**
* @file nf_driver.h
* @author Ivano Cerrato<ivano.cerrato (at) polito.it>
*
* @brief NF related operations exposed to the CMM by the driver
* 
*/

#include "../hal.h"
#include "../hal_utils.h"

//C++ extern C
HAL_BEGIN_DECLS

/**
 * @name    hal_result_t hal_driver_nf_create_nf_port
 * @brief   Instructs driver to create a new NF port
 *
 * @param nf_name				Name of the NF associated with the port to be created
 * @param nf_port_name				Name of the NF port to be created
 * @param nf_type				Type of the NF to be created

 */
hal_result_t hal_driver_nf_create_nf_port(const char *nf_name, const char *nf_port_name, port_type_t nf_port_type);

/**
 * @name    hal_result_t hal_driver_nf_destroy_nf_port
 * @brief   Instructs driver to destroy a NF port
 *
 * @param nf_port_name		Name of the NF port to be destroyed
 */
hal_result_t hal_driver_nf_destroy_nf_port(const char *nf_port_name);

/**
 * @name    hal_result_t hal_driver_nf_start_nf_port
 * @brief   Instructs driver to start a NF port
 *
 * @param nf_port_name		Name of the NF port to be started
 */
hal_result_t hal_driver_nf_start_nf_port(const char *nf_port_name);

/**
 * @name    hal_result_t hal_driver_nf_stop_nf_port
 * @brief   Instructs driver to stop a NF port
 *
 * @param nf_port_name		Name of the NF port to be stopped
 */
hal_result_t hal_driver_nf_stop_nf_port(const char *nf_port_name);


// [+] Add more here..

//C++ extern C
HAL_END_DECLS

#endif /* NF_DRIVER_H */

