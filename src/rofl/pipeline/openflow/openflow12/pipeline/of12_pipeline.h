#ifndef __OF12_PIPELINE_H__
#define __OF12_PIPELINE_H__

#include <stdlib.h>
#include "../../../util/rofl_pipeline_utils.h"
#include "of12_flow_table.h"
#include "../../../common/datapacket.h"
#include "../../of_switch.h"

#define OF12_MAX_FLOWTABLES 256 //As per 1.2 spec

/* Capabilities supported by the datapath pipeline. */
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

	//Capabilities (OF12_CAP_FLOW_STATS, OF12_CAP_TABLE_STATS, ...)
	unsigned int capabilities;

	//Array of tables; 
	of12_flow_table_t* tables;
	
};
typedef struct of12_pipeline of12_pipeline_t;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/* Pipeline mgmt */
of12_pipeline_t* of12_init_pipeline(const unsigned int num_of_tables, enum matching_algorithm_available* list, const of12_flow_table_miss_config_t table_config);
unsigned int of12_destroy_pipeline(of12_pipeline_t* pipeline);

//Packet processing
void of12_process_packet_pipeline(const of_switch_t* sw, datapacket_t *const pkt);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_PIPELINE
