#include "of_switch.h"

//Openflow specific switch implementations 
#include "openflow12/of12_switch.h"
#include "openflow12/pipeline/of12_pipeline.h"
//Add more here...

#include "openflow12/pipeline/matching_algorithms/matching_algorithms_available.h"


//Wrapping destroy 
rofl_result_t of_destroy_switch(const of_switch_t* sw){
	
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return __of12_destroy_switch(((of12_switch_t*)sw));
		default: 
			return ROFL_FAILURE;
	}
}	

//Wrapping of processing
rofl_result_t of_process_packet_pipeline(const of_switch_t* sw, datapacket_t *const pkt){
	
	switch(sw->of_ver){
		case OF_VERSION_12: 
			__of12_process_packet_pipeline(sw, pkt);
			break;
		default: 
			return ROFL_FAILURE;
	}

	return ROFL_SUCCESS;
}	

//Wrapping of timers processing
void of_process_pipeline_tables_timeout_expirations(const of_switch_t* sw){
	
	switch(sw->of_ver){
		case OF_VERSION_12: 
			__of12_process_pipeline_tables_timeout_expirations(((of12_switch_t*)sw)->pipeline);
			break;
		default: 
			//return ROFL_FAILURE;
			break;
	}
}	


rofl_result_t __of_attach_port_to_switch_at_port_num(of_switch_t* sw, unsigned int port_num, switch_port_t* port){
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return __of12_attach_port_to_switch_at_port_num((of12_switch_t*)sw, port_num, port); 
		default: 
			return ROFL_FAILURE;
	}
}

rofl_result_t __of_attach_port_to_switch(of_switch_t* sw, switch_port_t* port, unsigned int* port_num){
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return __of12_attach_port_to_switch((of12_switch_t*)sw,port, port_num); 
		default: 
			return ROFL_FAILURE;
	}
}

rofl_result_t __of_detach_port_from_switch_by_port_num(of_switch_t* sw, unsigned int port_num){
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return __of12_detach_port_from_switch_by_port_num((of12_switch_t*)sw, port_num); 
		default: 
			return ROFL_FAILURE;
	}
}

rofl_result_t __of_detach_port_from_switch(of_switch_t* sw, switch_port_t* port){
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return __of12_detach_port_from_switch((of12_switch_t*)sw, port); 
		default: 
			return ROFL_FAILURE;
	}
}

rofl_result_t __of_detach_all_ports_from_switch(of_switch_t* sw){
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return __of12_detach_all_ports_from_switch((of12_switch_t*)sw); 
		default: 
			return ROFL_FAILURE;
	}
}

rofl_result_t of_get_switch_matching_algorithms(of_version_t of_version, const char * const** name_list, int *count){

	switch (of_version) {
		case OF_VERSION_12:
		{
			static const char const * names[] = OF12_MATCHING_ALGORITHM_NAMES;

			*count = of12_matching_algorithm_count;
			*name_list = names;
			return ROFL_SUCCESS;
		}
		default:
			return ROFL_FAILURE;
	}
}
