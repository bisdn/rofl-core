/*
 * platform_hooks_of12.c
 *
 *  Created on: Feb 7, 2013
 *      Author: tobi
 */

#include <assert.h>
#include <rofl/datapath/pipeline/openflow/openflow12/of12_async_events_hooks.h>
#include <rofl/datapath/pipeline/openflow/openflow12/of12_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_flow_table.h>


/*
* Hooks for configuration of the switch
*/

rofl_result_t platform_post_init_of12_switch(of12_switch_t* sw){

	return ROFL_SUCCESS;
}

rofl_result_t platform_pre_destroy_of12_switch(of12_switch_t* sw){

	//TODO: put datapacket store and ringbuffer here
	
	return ROFL_SUCCESS;
}


/*
* Packet in
*/

void platform_of12_packet_in(const of12_switch_t* sw, uint8_t table_id, datapacket_t* pkt, of_packet_in_reason_t reason)
{

}

void platform_of12_notify_flow_removed(const of12_switch_t* sw, of12_flow_remove_reason_t reason, of12_flow_entry_t *entry)
{

}
