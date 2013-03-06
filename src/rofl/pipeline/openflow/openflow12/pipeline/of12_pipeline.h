#ifndef __OF12_PIPELINE_H__
#define __OF12_PIPELINE_H__

#include <stdlib.h>
#include "rofl.h" 
#include "../../../util/rofl_pipeline_utils.h"
#include "of12_flow_table.h"
#include "of12_group_table.h"
#include "../../../common/datapacket.h"
#include "../../of_switch.h"

#define OF12_MAX_FLOWTABLES 256 //As per 1.2 spec
#define OF12_DEFAULT_MISS_SEND_LEN 128 //As per 1.2 spec

/* Capabilities supported by the datapath pipeline. -> Direct mapping to  */
enum of12_capabilities {
    OF12_CAP_FLOW_STATS     = 1 << 0,  /* Flow statistics. */
    OF12_CAP_TABLE_STATS    = 1 << 1,  /* Table statistics. */
    OF12_CAP_PORT_STATS     = 1 << 2,  /* Port statistics. */
    OF12_CAP_GROUP_STATS    = 1 << 3,  /* Group statistics. */
    OF12_CAP_IP_REASM       = 1 << 5,  /* Can reassemble IP fragments. */
    OF12_CAP_QUEUE_STATS    = 1 << 6,  /* Queue statistics. */
    OF12_CAP_ARP_MATCH_IP   = 1 << 7   /* Match IP addresses in ARP pkts. */
};

struct of12_pipeline{
	
	//Number of tables
	unsigned int num_of_tables;
	
	//Number of buffers
	unsigned int num_of_buffers;

	//Capabilities bitmap (OF12_CAP_FLOW_STATS, OF12_CAP_TABLE_STATS, ...)
	unsigned int capabilities;

	//Miss send length
	uint16_t miss_send_len;

	//Array of tables; 
	of12_flow_table_t* tables;
	
	//Group table
	of12_group_table_t* groups;
	
};
typedef struct of12_pipeline of12_pipeline_t;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/* Pipeline mgmt */
of12_pipeline_t* of12_init_pipeline(const unsigned int num_of_tables, enum matching_algorithm_available* list, const of12_flow_table_miss_config_t table_config);
rofl_result_t of12_destroy_pipeline(of12_pipeline_t* pipeline);

//Packet processing
void of12_process_packet_pipeline(const of_switch_t *sw, datapacket_t *const pkt);

//Process the packet out
void of12_process_packet_out_pipeline(const of_switch_t *sw, datapacket_t *const pkt, const of12_action_group_t* apply_actions_group);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_PIPELINE
