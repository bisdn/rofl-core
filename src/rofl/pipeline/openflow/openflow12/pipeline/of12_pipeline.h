#ifndef __OF12_PIPELINE_H__
#define __OF12_PIPELINE_H__

#include <stdlib.h>
#include "../../../util/rofl_pipeline_utils.h"
#include "of12_flow_table.h"
#include "../../../common/datapacket.h"

#define OF12_MAX_FLOWTABLES 256 //As per 1.2 spec

struct of12_pipeline{
	
	//Number of tables
	unsigned int num_of_tables;
	
	//Number of buffers
	unsigned int num_of_buffers;

	//Capabilities (OFPC_FLOW_STATS, OFPC_TABLE_STATS, ...)
	unsigned int capabilities;

	//Array of tables; 
	of12_flow_table_t* tables;
	
};
typedef struct of12_pipeline of12_pipeline_t;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/* Pipeline mgmt */
of12_pipeline_t* of12_init_pipeline(const unsigned int num_of_tables, const of12_flow_table_config_t table_config);
unsigned int of12_destroy_pipeline(of12_pipeline_t* pipeline);

//Packet processing
unsigned int of12_process_packet_pipeline(const of12_pipeline_t* pipeline, datapacket_t *const pkt);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_PIPELINE
