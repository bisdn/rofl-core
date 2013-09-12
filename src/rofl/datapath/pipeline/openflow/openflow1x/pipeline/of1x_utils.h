/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_UTILS_H__
#define __OF1X_UTILS_H__

#include <inttypes.h>

/**
* @file of1x_utils.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*/

/**
* @ingroup core_of1x 
* Extended flowmod return codes
*/
typedef enum rofl_of1x_fm_result{
	ROFL_OF1X_FM_SUCCESS	= EXIT_SUCCESS,
	ROFL_OF1X_FM_FAILURE	= EXIT_FAILURE,
	ROFL_OF1X_FM_OVERLAP
	
}rofl_of1x_fm_result_t;

/*
* Mutex state for flow_entry_removal 
*/
typedef enum of1x_mutex_acquisition_required{
	MUTEX_NOT_ACQUIRED = 0, 			/*mutex has not been acquired and we must take it*/
	MUTEX_ALREADY_ACQUIRED_BY_TIMER_EXPIRATION,	/*mutex was taken when checking for expirations. We shouldn't call the timers functions*/
	MUTEX_ALREADY_ACQUIRED_NON_STRICT_SEARCH	/*mutex was taken when looking for entries with a non strict definition*/
}of1x_mutex_acquisition_required_t;

/*
* Required pipeline version
*/
typedef struct of1x_ver_req_t{
	uint8_t min_ver;	//Bitmap of_version_t
	uint8_t max_ver;	//Bitmap of_version_t
}of1x_ver_req_t;

#endif //OF1X_UTILS
