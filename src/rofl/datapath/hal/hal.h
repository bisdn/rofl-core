/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HAL_H
#define HAL_H 

/*
 * Hardware abstraction layer 
 *
 *  Created on: 16.08.2012
 *      Author: andreas, msune
 */

#include <stdlib.h>
#include <rofl.h>
#include <rofl/datapath/pipeline/switch_port.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>

#include "hal_utils.h"

//Return values
typedef enum hal_result {
	HAL_SUCCESS	= ROFL_SUCCESS,
	HAL_FAILURE	= ROFL_FAILURE,
	
	//Extra codes
	HAL_FM_OVERLAP_FAILURE
}hal_result_t;

#endif /* HAL_H */
