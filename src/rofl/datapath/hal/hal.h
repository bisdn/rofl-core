/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HAL_H
#define HAL_H 

/**
* @file hal.h
* @author Marc Sune<marc.sune (at) bisdn.de>
* @author Andreas Koepsel<andreas.koepsel (at) bisdn.de>
*
* @brief Hardware Abstraction Layer (HAL) main header file
*/

#include <stdlib.h>
#include <rofl.h>
#include <rofl/datapath/pipeline/switch_port.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_utils.h>

#include "hal_utils.h"

/**
* HAL return values
*/
typedef enum hal_result {
	HAL_SUCCESS	= ROFL_SUCCESS,
	HAL_FAILURE	= ROFL_FAILURE,
	
	//Extra codes
	HAL_FM_BAD_TABLE_ID = ROFL_OF1X_FM_BAD_TABLE_ID,
	HAL_FM_OVERLAP_FAILURE = ROFL_OF1X_FM_OVERLAP_FAILURE,
	HAL_FM_TABLE_FULL_FAILURE = ROFL_OF1X_FM_TABLE_FULL_FAILURE,
	HAL_FM_VALIDATION_FAILURE = ROFL_OF1X_FM_VALIDATION_FAILURE,
	HAL_FM_BAD_TIMEOUT = ROFL_OF1X_FM_BAD_TIMEOUT,
	HAL_FM_BAD_FLAGS = ROFL_OF1X_FM_BAD_FLAGS,
}hal_result_t;

/**
* Cast from ROFL pipeline rofl_of1x_fm_result_t to hal_result_t
*/
#define HAL_CAST_FM_RET(x_val) ((hal_result_t)x_val)

#endif /* HAL_H */
