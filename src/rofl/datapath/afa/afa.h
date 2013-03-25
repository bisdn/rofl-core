/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AFA_H
#define AFA_H 

/*
 * afa.h
 *
 * AFA - Abstract Forwarding API 
 *
 *  Created on: 16.08.2012
 *      Author: andreas, msune
 */

#include <stdlib.h>
#include <rofl.h>
#include <rofl/datapath/pipeline/switch_port.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>

#include "afa_utils.h"

//Return values
typedef enum afa_result {
	AFA_SUCCESS	= ROFL_SUCCESS,
	AFA_FAILURE	= ROFL_FAILURE,
	
	//Extra codes
	AFA_FM_OVERLAP_FAILURE
}afa_result_t;

#endif /* AFA_H */
