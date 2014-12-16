/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FRAG_EXTENSIONS_H
#define FRAG_EXTENSIONS_H

/**
* @file frag.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief IPv4 fragmentation and reassembly driver extensions
*/

#include <stdbool.h>

#include "../hal.h"
#include "../hal_utils.h"

typedef struct hal_far_ext_ops{

	/**
	 * @brief   Instructs driver to enable IPv4 fragmentation in the LSI with dpid
	 */
	hal_result_t (*enable_ipv4_frag_filter)(const uint64_t dpid);

	/**
	 * @brief   Instructs driver to disable IPv4 fragmentation in the LSI with dpid
	 */
	hal_result_t (*disable_ipv4_frag_filter)(const uint64_t dpid);

	/**
	 * @brief   Get the status of the ipv4 fragmentation filter 
	 */
	bool (*ipv4_frag_filter_status)(const uint64_t dpid);

	/**
	 * @brief   Instructs driver to enable IPv4 reassembly in the LSI with dpid
	 */
	hal_result_t (*enable_ipv4_reas_filter)(const uint64_t dpid);

	/**
	 * @brief   Instructs driver to disable IPv4 reassembly in the LSI with dpid
	 */
	hal_result_t (*disable_ipv4_reas_filter)(const uint64_t dpid);

	/**
	 * @brief   Get the status of the ipv4 reassembly filter 
	 */
	bool (*ipv4_reas_filter_status)(const uint64_t dpid);

}hal_far_ext_ops_t;

//C++ extern C
HAL_BEGIN_DECLS

//C++ extern C
HAL_END_DECLS

#endif /* FRAG_EXTENSIONS_H */

