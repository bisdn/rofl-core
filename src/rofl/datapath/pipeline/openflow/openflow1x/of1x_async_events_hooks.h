/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ASYNC_EVENTS_HOOKS_OF1X_H_
#define ASYNC_EVENTS_HOOKS_OF1X_H_

#include "rofl.h"
#include "../of_switch.h"
#include "of1x_switch.h"
#include "pipeline/of1x_flow_entry.h"
#include "../../../pipeline/common/datapacket.h"

/**
* @file of1x_async_events_hooks.h 
* @author Marc Sune<marc.sune (at) bisdn.de>, Tobias Jungel<tobias.jungel (at) bisdn.de>  
*
* @brief OpenFlow v1.0, 1.2 and 1.3.2 switch(fwd module) -> asynchronous event notification and hooks) 
*
* This file contains event notifications, and other related hooks that the library expects to be
* implemented by the library user, in order to be called appropiately from the pipeline library.
*
*/

/**
* Packet-in reason (of12p_packet_in_reason enum)
* @ingroup core_of1x 
*/
enum of1x_packet_in_reason {
    OF1X_PKT_IN_NO_MATCH = 0,      /* No matching flow. */
    OF1X_PKT_IN_ACTION = 1,        /* Action explicitly output to controller. */
    OF1X_PKT_IN_INVALID_TTL = 2	/* Packet has invalid TTL */
};


//C++ extern C
ROFL_BEGIN_DECLS

/*
* Configuration hooks
*/
//TODO: evaluate if they must return errors

/**
* @brief Allows the user of the library to pre-configure the switch, as well as perform
* other platform related actions on switch creation according to platform capabilities 
* @ingroup async_events_hooks_of1x 
*/
rofl_result_t platform_post_init_of1x_switch(of1x_switch_t* sw);

/*
* Async events
*/

/**
* @brief Allows the user of the library to perform other platform related actions when
* a switch is about the be destroyed.
* @ingroup async_events_hooks_of1x 
*/
rofl_result_t platform_pre_destroy_of1x_switch(of1x_switch_t* sw);

/**
* @brief Packet in event notification. 
* @ingroup async_events_hooks_of1x
* @ingroup core_pp 
*
* The implementation of packet_in must either store the buffer (pkt) for later reference
* by the controller or DROP (release) the buffer (pkt) if there was an error on sending
* the PKT_IN event or no controller is attached. The packet matches can be obtained by calling fill_packet_matches().
*  
*/
void platform_of1x_packet_in(const of1x_switch_t* sw, uint8_t table_id, datapacket_t* pkt, uint16_t send_len, of_packet_in_reason_t reason);

/**
* @brief Flow removed event notification 
* @ingroup async_events_hooks_of1x 
*
* @param removed_flow_entry The entry shall ONLY be used for reading, and shall NEVER be removed (of1x_remove_flow_entry).
* This is done by the library itself
*/
void platform_of1x_notify_flow_removed(const of1x_switch_t* sw, 	
						of1x_flow_remove_reason_t reason, 
						of1x_flow_entry_t* removed_flow_entry);

/**
* @brief It can be used by hardware or other software (non rofl-pipeline) pipelines, to install the new entry. The entry has been already validated and added the state management
* @param new_entry		flow entry to add
* @ingroup async_events_hooks_of1x 
*/
void plaftorm_of1x_add_entry_hook(of1x_flow_entry_t* new_entry);

/**
* @brief It can be used by hardware or other software (non rofl-pipeline) pipelines, to modify an entry (single entry). The state management has already modified the state in rofl-pipeline 
* @param old_entry		the original entry
* @param mod			the modifications
* @param reset_count	if set reset statistic counters
* @ingroup async_events_hooks_of1x 
*/
void platform_of1x_modify_entry_hook(of1x_flow_entry_t* old_entry, of1x_flow_entry_t* mod, int reset_count);

/**
* @brief It can be used by hardware or other software (non rofl-pipeline) pipelines, to remove an entry (single entry). The state management has already removed the state in rofl-pipeline 
* @param entry			flow entry to remove
* @ingroup async_events_hooks_of1x 
*/
void platform_of1x_remove_entry_hook(of1x_flow_entry_t* entry);

/**
* @brief It can be used by hardware or other software (non rofl-pipeline) pipelines, to update the stats of an entry when a request for stats is done.
* @param entry			flow entry to update statistics from the platform
* @ingroup async_events_hooks_of1x 
*/
void platform_of1x_update_stats_hook(of1x_flow_entry_t* entry);

//Extern C
ROFL_END_DECLS

#endif /* ASYNC_EVENTS_HOOKS_OF1X_H_ */
