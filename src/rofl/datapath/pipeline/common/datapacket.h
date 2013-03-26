/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __DATAPACKET_H__
#define __DATAPACKET_H__

#include <stdbool.h>

/**
* @file datapacket.h
* @author Marc Sune<marc.sune (at) bisdn.de>
* 
* @brief Defines the common packet abstraction that 
* a logical switch can process through its pipeline.
*
* The datapacket is an OF version agnostic data packet abstraction
* which may also contain platform specific state (likely at least a
* reference to the packet buffer in the platform). 
*
*/

//Typedef to void. This is dependant to the version of the pipeline
typedef void of_packet_matches_t;
typedef void of_write_actions_t;
typedef void platform_datapacket_state_t; 

/**
* @brief Data packet abstraction
*
* Abstraction that represents a data packet that may transverse
* one Logical Switch Openflow Pipeline. This abstraction is Openflow 
* version agnostic. It contains a (void*) reference, platform_state
* to allow the user of the library to keep platform specific state 
* while transversing the pipeline, which the packet mangling APIs may
* use afterwards.
*/
typedef struct{

	//Generic parts of an Openflow (version agnostic)
	of_packet_matches_t* matches;
	of_write_actions_t* write_actions;

	/**
	* Flag indicating if it is a replica of the original packet
	* (used for multi-output matches)
	*/
	bool is_replica;
	
	/** 
	* @brief Platform specific state. 
	* 
	* This is not OF related state and platform  specific, and may be
	* used by the library user to keep platform specific state.
	* This may typically be, at least, a reference to the packet
	* buffer in the platform.
	*/
	platform_datapacket_state_t* platform_state;

}datapacket_t;

#endif //DATAPACKET

