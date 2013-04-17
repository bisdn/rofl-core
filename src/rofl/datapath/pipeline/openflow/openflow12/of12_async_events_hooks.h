/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ASYNC_EVENTS_HOOKS_OF12_H_
#define ASYNC_EVENTS_HOOKS_OF12_H_

#include "rofl.h"
#include "../of_switch.h"
#include "of12_switch.h"
#include "pipeline/of12_flow_entry.h"
#include "../../../pipeline/common/datapacket.h"

/**
* @file of12_async_events_hooks_hooks.h
* @author Marc Sune<marc.sune (at) bisdn.de>, Tobias Jungel<tobias.jungel (at) bisdn.de>  
*
* @brief Openflow v1.2 switch(fwd module) -> asynchronous event notification and hooks) 
*
* This file contains event notifications, and other related hooks that the library expects to be
* implemented by the library user, in order to be called appropiately from the pipeline library.
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
* @ingroup async_events_hooks_of12 
*/
rofl_result_t platform_post_init_of12_switch(of12_switch_t* sw);

/*
* Async events
*/

/**
* @brief Allows the user of the library to perform other platform related actions when
* a switch is about the be destroyed.
* @ingroup async_events_hooks_of12 
*/
rofl_result_t platform_pre_destroy_of12_switch(of12_switch_t* sw);

/**
* @brief Packet in event notification 
* @ingroup async_events_hooks_of12
*
* The implementation of packet_in must either store the buffer (pkt) for later reference
* by the controller or DROP (release) the buffer (pkt) if there was an error on sending
* the PKT_IN event or no controller is attached. 
*/
void platform_of12_packet_in(const of12_switch_t* sw, uint8_t table_id, datapacket_t* pkt, of_packet_in_reason_t reason);

/**
* @brief Flow removed event notification 
* @ingroup async_events_hooks_of12 
*/
void platform_of12_notify_flow_removed(const of12_switch_t* sw, 	
						of12_flow_remove_reason_t reason, 
						of12_flow_entry_t* removed_flow_entry);


//Extern C
ROFL_PIPELINE_END_DECLS

#endif /* ASYNC_EVENTS_HOOKS_OF12_H_ */
