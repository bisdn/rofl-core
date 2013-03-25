/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef OF12_CMM_H
#define OF12_CMM_H 

#include <inttypes.h>
#include <rofl/datapath/pipeline/openflow/openflow12/of12_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_flow_entry.h>
#include <rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_packet_matches.h>
#include "../../afa.h"
#include "../../afa_utils.h"

/*
* of12_cmm.h
*
* OF12 related operations exposed by the cmm to the forwarding module 
*
* Created on: 12.11.2012 
* 	Author: msune
*/

//C++ extern C
AFA_BEGIN_DECLS

/**
 * @name    cmm_process_of12_packet_in
 * @brief   Instructs the CMM to process a PACKET_IN event comming from the DP
 * @ingroup of12_cmm_async_event_processing
 *
 * @param sw Openflow 1.2 switch pointer that generated the PACKET_IN
 * @param table_id ID of the table that produced PACKET_IN
 * @param reason one of the OFPR_ ... constants
 * @param in_port Incomming packet port 
 * @param buffer_id ID of the HW buffer
 * @param pkt_buffer Buffer containing the packet
 * @param buf_len Buffer length (may be shorter than the packet stored in buffer)
 * @param total_len total length of buffer
 * @param matches OF1.2 packet matches
 */
afa_result_t cmm_process_of12_packet_in(const of12_switch_t* sw, 
					uint8_t table_id, 
					uint8_t reason,
					uint32_t in_port,
					uint32_t buffer_id,
					uint8_t* pkt_buffer,
					uint32_t buf_len,
					uint16_t total_len,
					of12_packet_matches_t matches);

/**
 * @name    cmm_process_of12_flow_removed
 * @brief   Instructs the CMM to process a FLOW_REMOVED event comming from the DP
 * @ingroup of12_cmm_async_event_processing
 *
 * @param sw Openflow 1.2 switch pointer that generated the FLOW_REMOVED
 * @param removed_flow_entry Pointer to the flow_entry that has been removed
 */
afa_result_t cmm_process_of12_flow_removed(const of12_switch_t* sw, 	
					uint8_t reason, 
					of12_flow_entry_t* removed_flow_entry);

//C++ extern C
AFA_END_DECLS

#endif /* OF12_CMM_H */


