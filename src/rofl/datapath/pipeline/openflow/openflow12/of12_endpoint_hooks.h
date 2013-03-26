/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PLATFORM_HOOKS_OF12_H_
#define PLATFORM_HOOKS_OF12_H_

#include "rofl.h"
#include "../of_switch.h"
#include "of12_switch.h"
#include "pipeline/of12_flow_entry.h"
#include "../../../pipeline/common/datapacket.h"

/**
* @file of12_endpoint_hooks.h
* @author Marc Sune<marc.sune (at) bisdn.de>, Tobias Jungel<tobias.jungel (at) bisdn.de>  
*
* @brief Openflow v1.2 switch(fwd module) -> endpoint interface(hooks)  
*
* This file contains the notification, and other related hooks that the library expects to be
* implemented by the library user.
*
*/

/**
* Packet-in reason (of12p_packet_in_reason enum)
* @ingroup core_of12 
*/
enum of12_packet_in_reason {
    OF12_PKT_IN_NO_MATCH = 0,      /* No matching flow. */
    OF12_PKT_IN_ACTION = 1,        /* Action explicitly output to controller. */
    OF12_PKT_IN_INVALID_TTL = 2	/* Packet has invalid TTL */
};


//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/*
* Configuration hooks
*/
//TODO: evaluate if they must return errors

/**
* @brief Allows the user of the library to pre-configure the switch, as well as perform
* other platform related actions on switch creation according to platform capabilities 
* @ingroup endpoint_of12 
*/
rofl_result_t platform_post_init_of12_switch(of12_switch_t* sw);

/**
* @brief Allows the user of the library to perform other platform related actions when
* a switch is about the be destroyed.
* @ingroup endpoint_of12 
*/
rofl_result_t platform_pre_destroy_of12_switch(of12_switch_t* sw);

/**
* @brief Packet in event notification 
* @ingroup endpoint_of12 
*/
void platform_of12_packet_in(const of12_switch_t* sw, uint8_t table_id, datapacket_t* pkt, of_packet_in_reason_t reason);

/**
* @brief Flow removed event notification 
* @ingroup endpoint_of12 
*/
void platform_of12_notify_flow_removed(const of12_switch_t* sw, 	
						of12_flow_remove_reason_t reason, 
						of12_flow_entry_t* removed_flow_entry);


//Extern C
ROFL_PIPELINE_END_DECLS

#endif /* PLATFORM_HOOKS_OF12_H_ */
