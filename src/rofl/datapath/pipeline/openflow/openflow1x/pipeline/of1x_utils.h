/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_UTILS_H__
#define __OF1X_UTILS_H__

#include <inttypes.h>
#include "../../of_switch.h" 

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
	of_version_t min_ver;	//of_version_t
	of_version_t max_ver;	//of_version_t
}of1x_ver_req_t;

#define OF1X_MIN_VERSION OF_VERSION_10
#define OF1X_MAX_VERSION OF_VERSION_13

/*
* Useful constants (endianness dependant)
*/
#if defined(BIG_ENDIAN_DETECTED)
	/*
	* Useful masks
	*/
	//Byte masks
	#define OF1X_8_BYTE_MASK	0xFFFFFFFFFFFFFFFFULL 
	#define OF1X_6_BYTE_MASK	0x0000FFFFFFFFFFFFULL
	#define OF1X_4_BYTE_MASK	0x00000000FFFFFFFF
	#define OF1X_3_BYTE_MASK	0x0000000000FFFFFF
	#define OF1X_2_BYTE_MASK	0x000000000000FFFF
	#define OF1X_1_BYTE_MASK	0x00000000000000FF
	  
	//Non-multiple of byte masks
	#define OF1X_48_BITS_MASK	0x0000FFFFFFFFFFFFULL 
	#define OF1X_20_BITS_MASK	0x00000000000FFFFF
	#define OF1X_13_BITS_MASK	0x0000000000001FFF
	#define OF1X_12_BITS_MASK	0x0000000000000FFF
	#define OF1X_9_BITS_MASK	0x00000000000001FF
	#define OF1X_6_BITS_MASK	0x000000000000003F
	#define OF1X_4_BITS_MASK	0x000000000000000F
	#define OF1X_3_BITS_MASK	0x0000000000000007
	#define OF1X_2_BITS_MASK	0x0000000000000003
	#define OF1X_1_BIT_MASK		0x0000000000000001

#elif defined(LITTLE_ENDIAN_DETECTED)
	
	/*
	* Useful masks
	*/
	//Byte masks
	#define OF1X_8_BYTE_MASK	0xFFFFFFFFFFFFFFFFULL 
	#define OF1X_6_BYTE_MASK	0xFFFFFFFFFFFF0000ULL
	#define OF1X_4_BYTE_MASK	0xFFFFFFFF
	#define OF1X_3_BYTE_MASK	0xFFFFFF00
	#define OF1X_2_BYTE_MASK	0xFFFF
	#define OF1X_1_BYTE_MASK	0xFF
	  
	//Non-multiple of byte masks
	#define OF1X_48_BITS_MASK	0xFFFFFFFFFFFF0000ULL 
	#define OF1X_20_BITS_MASK	0xFFFFF000
	#define OF1X_13_BITS_MASK	0xFFF8
	#define OF1X_12_BITS_MASK	0xFFF0
	#define OF1X_9_BITS_MASK	0xFF80
	#define OF1X_6_BITS_MASK	0xFC
	#define OF1X_4_BITS_MASK	0xF0
	#define OF1X_3_BITS_MASK	0xE0
	#define OF1X_2_BITS_MASK	0xC0
	#define OF1X_1_BIT_MASK		0x80
#else
	#error Unknwon endianness
#endif

/*
* Align to 64bit
*/
#define OF1X_ALIGN_64_BIT(x) (((x/8)+1)*8)

#endif //OF1X_UTILS
