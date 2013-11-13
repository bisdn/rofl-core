/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_PIPELINE_H__
#define __OF1X_PIPELINE_H__

#include <stdlib.h>
#include "rofl.h" 
#include "of1x_flow_table.h"
#include "of1x_group_table.h"
#include "../../../common/bitmap.h"
#include "../../../common/datapacket.h"
#include "../../of_switch.h"

#define OF1X_MAX_FLOWTABLES 255 //As per 1.2 spec
#define OF1X_FLOW_TABLE_ALL 0xFF //As per 1.2 spec
#define OF1X_DEFAULT_MISS_SEND_LEN 128 //As per 1.2 spec

/**
* @file of1x_pipeline.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief OpenFlow v1.0, 1.2 and 1.3.2 pipeline abstraction
*/

/**
* Capabilities supported by the datapath pipeline. WARNING: This is not a direct mapping of OFPC_XXX. 
*/
enum of1x_capabilities {
	OF1X_CAP_FLOW_STATS     = 1 << 0,  /* Flow statistics. */
	OF1X_CAP_TABLE_STATS    = 1 << 1,  /* Table statistics. */
	OF1X_CAP_PORT_STATS     = 1 << 2,  /* Port statistics. */
	OF1X_CAP_GROUP_STATS    = 1 << 3,  /* Group statistics. Added in OF1.1 (caution OF wire protocol collides with STP) */
	OF1X_CAP_IP_REASM       = 1 << 5,  /* Can reassemble IP fragments. */
	OF1X_CAP_QUEUE_STATS    = 1 << 6,  /* Queue statistics. */
	OF1X_CAP_ARP_MATCH_IP   = 1 << 7,  /* Match IP addresses in ARP pkts. Invalid for OF13 */
	OF1X_CAP_PORT_BLOCKED   = 1 << 8,  /* Match IP addresses in ARP pkts. Added in OF1.3 */

	//Deprecated (OF1.0)
	OF1X_CAP_STP   		= 1 << 31  /* STP capable switch (OF1.0 ONLY) */ 
};

//Fwd declaration
struct of1x_switch;

/** 
* OpenFlow v1.0, 1.2 and 1.3.2 pipeline abstraction data structure
*/
typedef struct of1x_pipeline{
	
	//Number of tables
	unsigned int num_of_tables;
	
	//Number of buffers
	unsigned int num_of_buffers;

	//Capabilities bitmap (OF1X_CAP_FLOW_STATS, OF1X_CAP_TABLE_STATS, ...)
	bitmap32_t capabilities;

	//Miss send length
	uint16_t miss_send_len;

	//Array of tables; 
	of1x_flow_table_t* tables;
	
	//Group table
	of1x_group_table_t* groups;

	//Reference back
	struct of1x_switch* sw;	
}of1x_pipeline_t;

//C++ extern C
ROFL_BEGIN_DECLS

/* Pipeline mgmt */
of1x_pipeline_t* __of1x_init_pipeline(struct of1x_switch* sw, const unsigned int num_of_tables, enum of1x_matching_algorithm_available* list);
rofl_result_t __of1x_destroy_pipeline(of1x_pipeline_t* pipeline);

//Purge of all entries in the pipeline (reset)	
rofl_result_t __of1x_purge_pipeline_entries(of1x_pipeline_t* pipeline);

//Packet processing
void __of1x_process_packet_pipeline(const of_switch_t *sw, datapacket_t *const pkt);

//Set the default tables(flow and group tables) configuration according to the new version
rofl_result_t __of1x_set_pipeline_tables_defaults(of1x_pipeline_t* pipeline, of_version_t version);


//Process the packet out

//Wrapping of processing
/**
* @brief Processes a packet-out through the OpenFlow pipeline.  
* @ingroup sw_runtime 
*/
void of1x_process_packet_out_pipeline(const struct of1x_switch *sw, datapacket_t *const pkt, const of1x_action_group_t* apply_actions_group);

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_PIPELINE
