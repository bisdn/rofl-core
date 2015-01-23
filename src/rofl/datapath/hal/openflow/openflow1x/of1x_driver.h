/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef OF1X_DRIVER_H
#define OF1X_DRIVER_H

#include <inttypes.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_action.h>
#include "../../hal.h"
#include "../../hal_utils.h"

/**
* @file of1x_driver.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief HAL driver OpenFlow specific management interface
*/

//C++ extern C
HAL_BEGIN_DECLS

/**
 * @brief   Instructs driver to modify port config state 
 * @ingroup hal_driver_of1x
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param drop_received		Drop packets received
 */
hal_result_t hal_driver_of1x_set_port_drop_received_config(uint64_t dpid, unsigned int port_num, bool drop_received);

/**
 * @brief   Instructs driver to modify port config state. When this flag is set the port will not forward flood packets
 * @ingroup hal_driver_of1x
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param no_flood		No flood allowed in port
 */
hal_result_t hal_driver_of1x_set_port_no_flood_config(uint64_t dpid, unsigned int port_num, bool no_flood);

/**
 * @brief   Instructs driver to modify port config state 
 * @ingroup hal_driver_of1x
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param forward		Forward packets
 */
hal_result_t hal_driver_of1x_set_port_forward_config(uint64_t dpid, unsigned int port_num, bool forward);

/**
 * @brief   Instructs driver to modify port config state 
 * @ingroup hal_driver_of1x
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param generate_packet_in	Generate packet in events for this port 
 */
hal_result_t hal_driver_of1x_set_port_generate_packet_in_config(uint64_t dpid, unsigned int port_num, bool generate_packet_in);

/**
 * @brief   Instructs driver to modify port advertise flags 
 * @ingroup hal_driver_of1x
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param advertise		Bitmap advertised
 */
hal_result_t hal_driver_of1x_set_port_advertise_config(uint64_t dpid, unsigned int port_num, uint32_t advertise);

/**
 * @brief   Instructs driver to process a PACKET_OUT event
 * @ingroup hal_driver_of1x
 *
 * @param dpid 		Datapath ID of the switch 
 * @param flags		Capabilities bitmap (OF1X_CAP_FLOW_STATS, OF1X_CAP_TABLE_STATS, ...)
 * @param miss_send_len	OF MISS_SEND_LEN
 */
hal_result_t hal_driver_of1x_set_pipeline_config(uint64_t dpid, unsigned int flags, uint16_t miss_send_len);


/**
 * @brief   Instructs driver to set table configuration(default action)
 * @ingroup hal_driver_of1x
 *
 * @param dpid 		Datapath ID of the switch
 * @param table_id	Table ID or 0xFF for all 
 * @param miss_send_len Table miss config	
 */
hal_result_t hal_driver_of1x_set_table_config(uint64_t dpid, unsigned int table_id, of1x_flow_table_miss_config_t config);

/**
 * @brief   Instructs driver to process a PACKET_OUT event
 * @ingroup hal_driver_of1x
 * 
 * @param dpid 		Datapath ID of the switch to process PACKET_OUT
 * @param buffer_id	Buffer ID. 0 or OF1XP_NO_BUFFER and implies no buffer
 * @param in_port 	Port IN
 * @param action_group 	Action group to apply
 * @param buffer	Pointer to the buffer
 * @param buffer_size	Buffer size
 */
hal_result_t hal_driver_of1x_process_packet_out(uint64_t dpid, uint32_t buffer_id, uint32_t in_port, of1x_action_group_t* action_group, uint8_t* buffer, uint32_t buffer_size);

/**
 * @brief   Instructs driver to process a FLOW_MOD add event
 * @ingroup hal_driver_of1x
 *
 * This method will add a flow_entry to the table of the switch referenced by the dpid. 
 * The flow entry shall already be initialized via of1x_init_flow_entry, and must already
 * contain the matches, instructions and actions.
 *
 * When check_overlap is enabled, addition will fail if there is at least one entry
 * which may potentally match the same packet, and this entry has the same priority.
 *
 * If (and only if) the mod operation is successful the contents of the pointer entry are set to NULL. If the result is failure, but the flow_entry is set to NULL, this indicates
 * that the flowmods was successfully installed, but the buffer ID was invalid/expired.
 *
 *
 * On success, the driver will instantiate the necessary state to handle timers and
 * statistics.
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id to install the flowmod
 * @param flow_entry	Flow entry to be installed
 * @param buffer_id	Buffer ID. 0 or OF1XP_NO_BUFFER and implies no buffer
 * @param check_overlap	Check OVERLAP flag
 * @param check_counts	Check RESET_COUNTS flag
 */
hal_result_t hal_driver_of1x_process_flow_mod_add(uint64_t dpid, uint8_t table_id, of1x_flow_entry_t** flow_entry, uint32_t buffer_id, bool check_overlap, bool reset_counts); 

/**
 * @brief   Instructs driver to process a FLOW_MOD modify event
 * @ingroup hal_driver_of1x
 *
 * The modify flow entry will modify any exisiting entry in the table that contains the
 * same matches as the parameter entry. The "entry" parameter is NOT a pointer to an existing
 * table entry. 
 *
 * If (and only if) the mod operation is successful the contents of the pointer entry are set to NULL. If the result is failure, but the flow_entry is set to NULL, this indicates
 * that the flowmods was successfully installed, but the buffer ID was invalid/expired.
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id from which to modify the flowmod
 * @param flow_entry	Flow entry 
 * @param buffer_id	Buffer ID. 0 or OF1XP_NO_BUFFER and implies no buffer
 * @param strictness 	Strictness (STRICT NON-STRICT)
 * @param check_counts	Check RESET_COUNTS flag
 */
hal_result_t hal_driver_of1x_process_flow_mod_modify(uint64_t dpid, uint8_t table_id, of1x_flow_entry_t** flow_entry, uint32_t buffer_id, of1x_flow_removal_strictness_t strictness, bool reset_counts); 


/**
 * @brief   Instructs driver to process a FLOW_MOD event
 * @ingroup hal_driver_of1x
 *
 * The remove flow entry will remove and destroy any exisiting entry in the table that contains
 * the same matches as the parameter entry. The "entry" parameter is NOT a pointer to an existing
 * table entry. 
 *
 * The entry parameter will never be modified by the library, and can be safely changed or removed
 * after the call of hal_driver_of1x_process_flow_mod_delete().
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
hal_result_t hal_driver_of1x_process_flow_mod_delete(uint64_t dpid, uint8_t table_id, of1x_flow_entry_t* flow_entry, uint32_t out_port, uint32_t out_group, of1x_flow_removal_strictness_t strictness); 

/**
 * @brief   Recovers the flow stats given a set of matches 
 * @ingroup hal_driver_of1x
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
of1x_stats_flow_msg_t* hal_driver_of1x_get_flow_stats(uint64_t dpid, uint8_t table_id, uint32_t cookie, uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, of1x_match_group_t *const matches);
 
/**
 * @brief   Recovers the aggregated flow stats given a set of matches 
 * @ingroup hal_driver_of1x
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
of1x_stats_flow_aggregate_msg_t* hal_driver_of1x_get_flow_aggregate_stats(uint64_t dpid, uint8_t table_id, uint32_t cookie, uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, of1x_match_group_t *const matches);

/**
 * @brief   Instructs driver to add a new GROUP
 * 
 * If (and only if) the mod operation is successful the contents of the pointer buckets are set to NULL. Any other reference to the buckets (**buckets) shall never be further used.
 *
 * @ingroup hal_driver_of1x
 *
 * @param dpid 		Datapath ID of the switch to install the GROUP
 */
rofl_of1x_gm_result_t hal_driver_of1x_group_mod_add(uint64_t dpid, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t **buckets);

/**
 * @brief   Instructs driver to modify the GROUP with identification ID
 *
 * If (and only if) the mod operation is successful the contents of the pointer buckets are set to NULL. Any other reference to the buckets (**buckets) shall never be further used.
 *
 * @ingroup hal_driver_of1x
 *
 * @param dpid 		Datapath ID of the switch to modify the GROUP
 */
rofl_of1x_gm_result_t hal_driver_of1x_group_mod_modify(uint64_t dpid, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t **buckets);

/**
 * @brief   Instructs driver to delete the GROUP with identification ID
 * @ingroup hal_driver_of1x
 *
 * @param dpid Datapath ID of the switch to delete the GROUP
 */
rofl_of1x_gm_result_t hal_driver_of1x_group_mod_delete(uint64_t dpid, uint32_t id);

/**
 * @ingroup core_of1x
 * Retrieves a copy of the group and bucket structure
 * @return of1x_stats_group_desc_msg_t instance that must be destroyed using of1x_destroy_group_desc_stats()
 */
of1x_stats_group_desc_msg_t *hal_driver_of1x_get_group_desc_stats(uint64_t dpid);

/**
 * @brief   Instructs driver to fetch the GROUP statistics
 * @ingroup hal_driver_of1x
 *
 * @param dpid Datapath ID of the switch where the GROUP is
 */
of1x_stats_group_msg_t* hal_driver_of1x_get_group_stats(uint64_t dpid, uint32_t id);

// [+] Add more here..

//C++ extern C
HAL_END_DECLS


#endif /* OF1X_DRIVER_H */


