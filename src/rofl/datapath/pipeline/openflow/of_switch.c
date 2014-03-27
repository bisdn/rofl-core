#include "of_switch.h"
#include "../common/datapacket.h"

//OpenFlow specific switch implementations 
#include "openflow1x/of1x_switch.h"
#include "openflow1x/pipeline/of1x_pipeline.h"
//Add more here...

#include "openflow1x/pipeline/matching_algorithms/matching_algorithms.h"


//Wrapping destroy 
rofl_result_t of_destroy_switch(const of_switch_t* sw){
	
	switch(sw->of_ver){
		case OF_VERSION_10: 
		case OF_VERSION_12: 
		case OF_VERSION_13: 
			return __of1x_destroy_switch(((of1x_switch_t*)sw));
		default: 
			return ROFL_FAILURE;
	}
}	

rofl_result_t of_reconfigure_switch(of_switch_t* sw, of_version_t version){

	if(version == OF_VERSION_10 || version == OF_VERSION_12 || version == OF_VERSION_13){
		return __of1x_reconfigure_switch((of1x_switch_t*)sw, version);
	}else{
		//Add other pipeline versions here..
	}

	return ROFL_FAILURE;
}


//Wrapping of timers processing
void of_process_pipeline_tables_timeout_expirations(const of_switch_t* sw){
	
	switch(sw->of_ver){
		case OF_VERSION_10: 
		case OF_VERSION_12: 
		case OF_VERSION_13: 
			__of1x_process_pipeline_tables_timeout_expirations(&((of1x_switch_t*)sw)->pipeline);
			break;
		default: 
			//return ROFL_FAILURE;
			break;
	}
}	


rofl_result_t __of_attach_port_to_switch_at_port_num(of_switch_t* sw, unsigned int port_num, switch_port_t* port){
	switch(sw->of_ver){
		case OF_VERSION_10: 
		case OF_VERSION_12: 
		case OF_VERSION_13: 
			return __of1x_attach_port_to_switch_at_port_num((of1x_switch_t*)sw, port_num, port); 
		default: 
			return ROFL_FAILURE;
	}
}

rofl_result_t __of_attach_port_to_switch(of_switch_t* sw, switch_port_t* port, unsigned int* port_num){
	switch(sw->of_ver){
		case OF_VERSION_10: 
		case OF_VERSION_12: 
		case OF_VERSION_13: 
			return __of1x_attach_port_to_switch((of1x_switch_t*)sw,port, port_num); 
		default: 
			return ROFL_FAILURE;
	}
}

rofl_result_t __of_detach_port_from_switch_by_port_num(of_switch_t* sw, unsigned int port_num){
	switch(sw->of_ver){
		case OF_VERSION_10: 
		case OF_VERSION_12: 
		case OF_VERSION_13: 
			return __of1x_detach_port_from_switch_by_port_num((of1x_switch_t*)sw, port_num); 
		default: 
			return ROFL_FAILURE;
	}
}

rofl_result_t __of_detach_port_from_switch(of_switch_t* sw, switch_port_t* port){
	switch(sw->of_ver){
		case OF_VERSION_10: 
		case OF_VERSION_12: 
		case OF_VERSION_13: 
			return __of1x_detach_port_from_switch((of1x_switch_t*)sw, port); 
		default: 
			return ROFL_FAILURE;
	}
}

rofl_result_t __of_detach_all_ports_from_switch(of_switch_t* sw){
	switch(sw->of_ver){
		case OF_VERSION_10: 
		case OF_VERSION_12: 
		case OF_VERSION_13: 
			return __of1x_detach_all_ports_from_switch((of1x_switch_t*)sw); 
		default: 
			return ROFL_FAILURE;
	}
}

rofl_result_t of_get_switch_matching_algorithms(of_version_t of_version, const char * const** name_list, int *count){

	switch (of_version) {
		case OF_VERSION_10:
		case OF_VERSION_12:
		case OF_VERSION_13:
		{
			static const char const * names[] = OF1X_MATCHING_ALGORITHM_NAMES;

			*count = of1x_matching_algorithm_count;
			*name_list = names;
			return ROFL_SUCCESS;
		}
		default:
			return ROFL_FAILURE;
	}
}


of_switch_snapshot_t* __of_switch_get_snapshot(of_switch_t* sw){
	switch (sw->of_ver){
		case OF_VERSION_10:
		case OF_VERSION_12:
		case OF_VERSION_13:
			return (of_switch_snapshot_t*)__of1x_switch_get_snapshot((of1x_switch_t*)sw);
		default:
			return NULL;
	}
}
void of_switch_destroy_snapshot(of_switch_snapshot_t* snapshot){
	switch (snapshot->of_ver){
		case OF_VERSION_10:
		case OF_VERSION_12:
		case OF_VERSION_13:
			__of1x_switch_destroy_snapshot((of1x_switch_snapshot_t*)snapshot);
		default:
			break;
	}
}
