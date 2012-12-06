#include "of_switch.h"

//Openflow specific switch implementations 
#include "openflow12/of12_switch.h"
#include "openflow12/pipeline/of12_pipeline.h"
//Add more here...

//Wrapping destroy 
inline unsigned int of_destroy_switch(const of_switch_t* sw){
	
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return of12_destroy_switch(((of12_switch_t*)sw));
		default: 
			return EXIT_FAILURE;
	}
}	

//Wrapping of processing
inline unsigned int of_process_packet_switch(const of_switch_t* sw, datapacket_t *const pkt){
	
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return of12_process_packet_pipeline(((of12_switch_t*)sw)->pipeline, pkt);
		default: 
			return EXIT_FAILURE;
	}
}	

//Wrapping of port management 
inline unsigned int of_get_switch_ports(of_switch_t* sw, logical_switch_port_t** ports, unsigned int* num_of_ports, unsigned int* logical_sw_max_ports){
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return of12_get_switch_ports((of12_switch_t*)sw, ports, num_of_ports, logical_sw_max_ports); 
		default: 
			return EXIT_FAILURE;
	}
}

inline unsigned int of_attach_port_to_switch_at_port_num(of_switch_t* sw, unsigned int port_num, switch_port_t* port){
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return of12_attach_port_to_switch_at_port_num((of12_switch_t*)sw, port_num, port); 
		default: 
			return EXIT_FAILURE;
	}
}

inline unsigned int of_attach_port_to_switch(of_switch_t* sw, switch_port_t* port, unsigned int* port_num){
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return of12_attach_port_to_switch((of12_switch_t*)sw,port, port_num); 
		default: 
			return EXIT_FAILURE;
	}
}

inline unsigned int of_detach_port_from_switch_by_port_num(of_switch_t* sw, unsigned int port_num){
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return of12_detach_port_from_switch_by_port_num((of12_switch_t*)sw, port_num); 
		default: 
			return EXIT_FAILURE;
	}
}

inline unsigned int of_detach_port_from_switch(of_switch_t* sw, switch_port_t* port){
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return of12_detach_port_from_switch((of12_switch_t*)sw, port); 
		default: 
			return EXIT_FAILURE;
	}
}

inline unsigned int of_detach_all_ports_from_switch(of_switch_t* sw){
	switch(sw->of_ver){
		case OF_VERSION_12: 
			return of12_detach_all_ports_from_switch((of12_switch_t*)sw); 
		default: 
			return EXIT_FAILURE;
	}
}
