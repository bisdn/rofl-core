/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef OF1X_FWD_MODULE_H
#define OF1X_FWD_MODULE_H

/**
* of1x_fwd_module.h
*
* OF1X related operations exposed to the CMM by the forwarding module 
*
* Created on: 12.11.2012 
* 	Author: msune
*/

#include <inttypes.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_action.h>
#include "../../afa.h"
#include "../../afa_utils.h"

//C++ extern C
AFA_BEGIN_DECLS

/**
 * @name    fwd_module_of1x_set_port_drop_received_config
 * @brief   Instructs forward module to modify port config state 
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param drop_received		Drop packets received
 */
afa_result_t fwd_module_of1x_set_port_drop_received_config(uint64_t dpid, unsigned int port_num, bool drop_received);

/**
 * @name    fwd_module_of1x_set_port_no_flood_config
 * @brief   Instructs driver to modify port config state. When this flag is set the port will not forward flood packets
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param no_flood		No flood allowed in port
 */
afa_result_t fwd_module_of1x_set_port_no_flood_config(uint64_t dpid, unsigned int port_num, bool no_flood);

/**
 * @name    fwd_module_of1x_set_port_forward_config
 * @brief   Instructs forward module to modify port config state 
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param forward		Forward packets
 */
afa_result_t fwd_module_of1x_set_port_forward_config(uint64_t dpid, unsigned int port_num, bool forward);

/**
 * @name    fwd_module_of1x_set_port_generate_packet_in_config
 * @brief   Instructs forward module to modify port config state 
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param generate_packet_in	Generate packet in events for this port 
 */
afa_result_t fwd_module_of1x_set_port_generate_packet_in_config(uint64_t dpid, unsigned int port_num, bool generate_packet_in);

/**
 * @name    fwd_module_of1x_set_port_advertise_config
 * @brief   Instructs forward module to modify port advertise flags 
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param advertise		Bitmap advertised
 */
afa_result_t fwd_module_of1x_set_port_advertise_config(uint64_t dpid, unsigned int port_num, uint32_t advertise);

/**
 * @name    fwd_module_of1x_set_pipeline_config
 * @brief   Instructs forward module to process a PACKET_OUT event
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch 
 * @param flags		Capabilities bitmap (OF1X_CAP_FLOW_STATS, OF1X_CAP_TABLE_STATS, ...)
 * @param miss_send_len	OF MISS_SEND_LEN
 */
afa_result_t fwd_module_of1x_set_pipeline_config(uint64_t dpid, unsigned int flags, uint16_t miss_send_len);


/**
 * @name    fwd_module_of1x_set_table_config
 * @brief   Instructs forward module to set table configuration(default action)
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch
 * @param table_id	Table ID or 0xFF for all 
 * @param miss_send_len Table miss config	
 */
afa_result_t fwd_module_of1x_set_table_config(uint64_t dpid, unsigned int table_id, of1x_flow_table_miss_config_t config);

/**
 * @name    fwd_module_of1x_process_packet_out
 * @brief   Instructs forward module to process a PACKET_OUT event
 * @ingroup of1x_fwd_module_async_event_processing
 * 
 * @param dpid 		Datapath ID of the switch to process PACKET_OUT
 * @param buffer_id	Buffer ID. 0 or OF1XP_NO_BUFFER and implies no buffer
 * @param in_port 	Port IN
 * @param action_group 	Action group to apply
 * @param buffer	Pointer to the buffer
 * @param buffer_size	Buffer size
 */
afa_result_t fwd_module_of1x_process_packet_out(uint64_t dpid, uint32_t buffer_id, uint32_t in_port, of1x_action_group_t* action_group, uint8_t* buffer, uint32_t buffer_size);

/**
 * @name    fwd_module_of1x_process_flow_mod_add
 * @brief   Instructs forward module to process a FLOW_MOD add event
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * This method will add a flow_entry to the table of the switch referenced by the dpid. 
 * The flow entry shall already be initialized via of1x_init_flow_entry, and must already
 * contain the matches, instructions and actions.
 *
 * When check_overlap is enabled, addition will fail if there is at least one entry
 * which may potentally match the same packet, and this entry has the same priority.
 *
 * If (and only if) the mod operation is successful the contents of the pointer entry are set to NULL. Any other reference to the real entry (**entry) shall never be further used.
 *
 *
 * On success, the forwarding module will instantiate the necessary state to handle timers and
 * statistics.
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id to install the flowmod
 * @param flow_entry	Flow entry to be installed
 * @param buffer_id	Buffer ID. 0 or OF1XP_NO_BUFFER and implies no buffer
 * @param check_overlap	Check OVERLAP flag
 * @param check_counts	Check RESET_COUNTS flag
 */
afa_result_t fwd_module_of1x_process_flow_mod_add(uint64_t dpid, uint8_t table_id, of1x_flow_entry_t** flow_entry, uint32_t buffer_id, bool check_overlap, bool reset_counts); 

/**
 * @name    fwd_module_of1x_process_flow_mod_modify
 * @brief   Instructs forward module to process a FLOW_MOD modify event
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * The modify flow entry will modify any exisiting entry in the table that contains the
 * same matches as the parameter entry. The "entry" parameter is NOT a pointer to an existing
 * table entry. 
 *
 * If (and only if) the mod operation is successful the contents of the pointer entry are set to NULL. Any other reference to the real entry (**entry) shall never be further used.
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id from which to modify the flowmod
 * @param flow_entry	Flow entry 
 * @param buffer_id	Buffer ID. 0 or OF1XP_NO_BUFFER and implies no buffer
 * @param strictness 	Strictness (STRICT NON-STRICT)
 * @param check_counts	Check RESET_COUNTS flag
 */
afa_result_t fwd_module_of1x_process_flow_mod_modify(uint64_t dpid, uint8_t table_id, of1x_flow_entry_t** flow_entry, uint32_t buffer_id, of1x_flow_removal_strictness_t strictness, bool reset_counts); 


/**
 * @name    fwd_module_of1x_process_flow_mod_delete
 * @brief   Instructs forward module to process a FLOW_MOD event
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * The remove flow entry will remove and destroy any exisiting entry in the table that contains
 * the same matches as the parameter entry. The "entry" parameter is NOT a pointer to an existing
 * table entry. 
 *
 * The entry parameter will never be modified by the library, and can be safely changed or removed
 * after the call of fwd_module_of1x_process_flow_mod_delete().
 *    
 * On success, the timers and statistics of the removed entries shall be purged.
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id from which to remove the flowmod
 * @param flow_entry	Flow entry
 * @param out_port 	Out port that entry must include
 * @param out_group 	Out group that entry must include	
 * @param strictness 	Strictness (STRICT NON-STRICT)
 */
afa_result_t fwd_module_of1x_process_flow_mod_delete(uint64_t dpid, uint8_t table_id, of1x_flow_entry_t* flow_entry, uint32_t out_port, uint32_t out_group, of1x_flow_removal_strictness_t strictness); 

/**
 * @name    fwd_module_of1x_get_flow_stats
 * @brief   Recovers the flow stats given a set of matches 
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id to get the flows of 
 * @param cookie	Cookie to be applied 
 * @param cookie_mask	Mask for the cookie
 * @param out_port 	Out port that entry must include
 * @param out_group 	Out group that entry must include	
 * @param matches	Matches
 * 
 * @return A pointer to an of1x_flow_msg_t struct or NULL on error. This pointer can be safely accessed and
 * modified, and MUST be destroyed via of1x_destroy_stats_flow_msg() once used.
 */
of1x_stats_flow_msg_t* fwd_module_of1x_get_flow_stats(uint64_t dpid, uint8_t table_id, uint32_t cookie, uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, of1x_match_group_t *const matches);
 
/**
 * @name    fwd_module_of1x_get_flow_aggregate_stats
 * @brief   Recovers the aggregated flow stats given a set of matches 
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id to get the flows of 
 * @param cookie	Cookie to be applied 
 * @param cookie_mask	Mask for the cookie
 * @param out_port 	Out port that entry must include
 * @param out_group 	Out group that entry must include	
 * @param matches	Matches
 *	
 * @return A pointer to an of1x_flow_aggregate_msg_t struct or NULL on error. This pointer can be 
 * safely accessed and modified, and MUST be destroyed via of1x_destroy_stats_flow_aggregate_msg() once used.
 */
of1x_stats_flow_aggregate_msg_t* fwd_module_of1x_get_flow_aggregate_stats(uint64_t dpid, uint8_t table_id, uint32_t cookie, uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, of1x_match_group_t *const matches);

/**
 * @name    fwd_module_of1x_group_mod_add
 * @brief   Instructs forward module to add a new GROUP
 * 
 * If (and only if) the mod operation is successful the contents of the pointer buckets are set to NULL. Any other reference to the buckets (**buckets) shall never be further used.
 *
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to install the GROUP
 */
rofl_of1x_gm_result_t fwd_module_of1x_group_mod_add(uint64_t dpid, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t **buckets);

/**
 * @name    fwd_module_of1x_group_mod_modify
 * @brief   Instructs forward module to modify the GROUP with identification ID
 *
 * If (and only if) the mod operation is successful the contents of the pointer buckets are set to NULL. Any other reference to the buckets (**buckets) shall never be further used.
 *
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to modify the GROUP
 */
rofl_of1x_gm_result_t fwd_module_of1x_group_mod_modify(uint64_t dpid, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t **buckets);

/**
 * @name    fwd_module_of1x_group_mod_del
 * @brief   Instructs fwd_module to delete the GROUP with identification ID
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid Datapath ID of the switch to delete the GROUP
 */
rofl_of1x_gm_result_t fwd_module_of1x_group_mod_delete(uint64_t dpid, uint32_t id);

/**
 * @name    fwd_module_of1x_group_search
 * @brief   Instructs driver to search the GROUP with identification ID
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to search the GROUP
 */
afa_result_t fwd_module_of1x_fetch_group_table(uint64_t dpid, of1x_group_table_t *group_table);

/**
 * @name    fwd_module_of1x_get_group_stats
 * @brief   Instructs fwd_module to fetch the GROUP statistics
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid Datapath ID of the switch where the GROUP is
 */
of1x_stats_group_msg_t * fwd_module_of1x_get_group_stats(uint64_t dpid, uint32_t id);

/**
 * @name    fwd_module_of1x_get_group_all_stats
 * @brief   Instructs driver to fetch the GROUP statistics from all the groups
 * @ingroup of1x_fwd_module_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch where the GROUPS are
 */
of1x_stats_group_msg_t * fwd_module_of1x_get_group_all_stats(uint64_t dpid, uint32_t id);

// [+] Add more here..

//C++ extern C
AFA_END_DECLS


#endif /* OF1X_FWD_MODULE_H */


