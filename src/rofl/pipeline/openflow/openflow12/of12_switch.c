#include <stdio.h>
#include "of12_switch.h"

#include "../../platform/cutil.h"
#include "../../platform/memory.h"

/* Initializer and destructor */
of12_switch_t* of12_init_switch(const char* name, uint64_t dpid, unsigned int num_of_tables, of12_flow_table_config_t config){

	of12_switch_t* sw;
	sw = (of12_switch_t*)cutil_malloc_shared(sizeof(of12_switch_t));
	if(sw == NULL)
		return NULL;

	//Filling in values
	sw->of_ver = OF_VERSION_12;	
	sw->dpid = dpid;
	sw->name = (char*)cutil_malloc_shared(strlen(name)+1);
	if(sw->name == NULL){
		cutil_free_shared(sw);
		return NULL;
	}
	strcpy(sw->name,name);

	//Initialize logical_ports
	memset(sw->logical_ports,0,sizeof(logical_switch_port_t)*LOGICAL_SWITCH_MAX_LOG_PORTS);
	sw->num_of_ports=0;
	
	//Initialize platform state to NULL
	sw->platform_state=NULL;
	
	//Mutex
	if(NULL == (sw->mutex = platform_mutex_init(NULL))){
		cutil_free_shared(sw->name);
		cutil_free_shared(sw);
		return NULL; 
	}
	
	//Setup pipeline	
	sw->pipeline = of12_init_pipeline(num_of_tables, config);
	if(sw->pipeline == NULL){
		cutil_free_shared(sw->name);
		cutil_free_shared(sw);
		return NULL;
	}	
	return sw;
}

unsigned int of12_destroy_switch(of12_switch_t* sw){

	unsigned int result;

	result = of12_destroy_pipeline(sw->pipeline);

	//TODO: trace if result != SUCCESS
	(void)result;
	
	cutil_free_shared(sw->name);
	cutil_free_shared(sw);
	
	return EXIT_SUCCESS;
}

/* Port management */
unsigned int of12_get_switch_ports(of12_switch_t* sw, logical_switch_port_t** ports, unsigned int* num_of_ports, unsigned int* logical_sw_max_ports){
	*ports = sw->logical_ports;
	*num_of_ports = sw->num_of_ports;
	*logical_sw_max_ports = LOGICAL_SWITCH_MAX_LOG_PORTS;
	return EXIT_SUCCESS;
}

unsigned int of12_attach_port_to_switch_at_port_num(of12_switch_t* sw, unsigned int port_num, switch_port_t* port){

	if(!port || !port_num || port_num >= LOGICAL_SWITCH_MAX_LOG_PORTS)
		return EXIT_FAILURE;	

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);
	
	if(sw->logical_ports[port_num].attachment_state){
		platform_mutex_unlock(sw->mutex);
		return EXIT_FAILURE;
	}

	//Initialize logical port 
	sw->logical_ports[port_num].attachment_state = LOGICAL_PORT_STATE_ATTACHED;
	sw->logical_ports[port_num].port = port;
	sw->num_of_ports++;

	//Return success
	platform_mutex_unlock(sw->mutex);
	return EXIT_SUCCESS;
}

unsigned int of12_attach_port_to_switch(of12_switch_t* sw, switch_port_t* port, unsigned int* port_num){
	unsigned int i;

	if(!port || port->attached_sw) 
		return EXIT_FAILURE;	

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);

	for(i=1;i<LOGICAL_SWITCH_MAX_LOG_PORTS;i++){
		if(!sw->logical_ports[i].attachment_state){
			//Initialize logical port 
			sw->logical_ports[i].attachment_state = LOGICAL_PORT_STATE_ATTACHED;
			sw->logical_ports[i].port = port;
			sw->num_of_ports++;
			*port_num = i;
			
			
				
			//Return success
			platform_mutex_unlock(sw->mutex);
			return EXIT_SUCCESS;
		}
	}
	
	//No slots free
	platform_mutex_unlock(sw->mutex);
	return EXIT_FAILURE;
}

unsigned int of12_detach_port_from_switch_by_port_num(of12_switch_t* sw, unsigned int port_num){

	if(!port_num)
		return EXIT_FAILURE;

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);

	if(sw->logical_ports[port_num].attachment_state != LOGICAL_PORT_STATE_ATTACHED){
		platform_mutex_unlock(sw->mutex);
		return EXIT_FAILURE;
	}

	sw->logical_ports[port_num].attachment_state = LOGICAL_PORT_STATE_DETACHED;
	sw->logical_ports[port_num].port = NULL;
	sw->num_of_ports--;
	
	//return success
	platform_mutex_unlock(sw->mutex);
	return EXIT_SUCCESS;
}

unsigned int of12_detach_port_from_switch(of12_switch_t* sw, switch_port_t* port){

	unsigned int i;

	if(!port) 
		return EXIT_FAILURE;	

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);

	for(i=0;i<LOGICAL_SWITCH_MAX_LOG_PORTS;i++){
		if(sw->logical_ports[i].port == port){
			//Detach
			sw->logical_ports[i].attachment_state = LOGICAL_PORT_STATE_DETACHED;
			sw->logical_ports[i].port = NULL;
			sw->num_of_ports--;

			platform_mutex_unlock(sw->mutex);
			return EXIT_SUCCESS;
		}
	}	
	
	//Not found 
	platform_mutex_unlock(sw->mutex);
	return EXIT_FAILURE;
}

unsigned int of12_detach_all_ports_from_switch(of12_switch_t* sw){

	unsigned int i;

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);

	for(i=0;i<LOGICAL_SWITCH_MAX_LOG_PORTS;i++){
		//Detach
		sw->logical_ports[i].attachment_state = LOGICAL_PORT_STATE_DETACHED;
		sw->logical_ports[i].port = NULL;
	}	
	
	//Not found 
	platform_mutex_unlock(sw->mutex);
	return EXIT_SUCCESS;
}

/* Dumping */
void of12_dump_switch(of12_switch_t* sw){
	fprintf(stderr,"Openflow switch instance (%p)\n",sw);
	fprintf(stderr,"========================\n");
	fprintf(stderr,"Name: %s\n",sw->name);
	fprintf(stderr,"Openflow version: %d\n",sw->of_ver);
	fprintf(stderr,"Openflow datapathid: %llu\n",(long long unsigned)sw->dpid);
}

void of12_full_dump_switch(of12_switch_t* sw){
	int i;
	of12_dump_switch(sw);

	/* Dumping tables */		
	for(i=0;i<sw->pipeline->num_of_tables;i++)
		of12_dump_table(&sw->pipeline->tables[i]);
	fprintf(stderr,"--End of pipeline tables--\n\n");
}

