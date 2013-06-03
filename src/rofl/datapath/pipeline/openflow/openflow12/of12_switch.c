#include "of12_switch.h"

#include "../../platform/memory.h"
#include "../../util/logging.h"
#include "../of_switch.h"
#include "of12_async_events_hooks.h"

/* Initializer and destructor */
of12_switch_t* of12_init_switch(const char* name, uint64_t dpid, unsigned int num_of_tables, enum of12_matching_algorithm_available* list){

	of12_switch_t* sw;
	sw = (of12_switch_t*)platform_malloc_shared(sizeof(of12_switch_t));
	if(sw == NULL)
		return NULL;

	//Filling in values
	sw->of_ver = OF_VERSION_12;	
	sw->dpid = dpid;
	sw->name = (char*)platform_malloc_shared(strlen(name)+1);
	if(sw->name == NULL){
		platform_free_shared(sw);
		return NULL;
	}
	strcpy(sw->name,name);

	//Initialize logical_ports
	memset(sw->logical_ports,0,sizeof(logical_switch_port_t)*LOGICAL_SWITCH_MAX_LOG_PORTS);
	sw->num_of_ports=0;
	sw->max_ports=LOGICAL_SWITCH_MAX_LOG_PORTS;
	
	//Initialize platform state to NULL
	sw->platform_state=NULL;
	
	//Mutex
	if(NULL == (sw->mutex = platform_mutex_init(NULL))){
		platform_free_shared(sw->name);
		platform_free_shared(sw);
		return NULL; 
	}
	
	//Setup pipeline	
	sw->pipeline = __of12_init_pipeline(sw, num_of_tables, list);
	if(sw->pipeline == NULL){
		platform_free_shared(sw->name);
		platform_free_shared(sw);
		return NULL;
	}
	
	//Allow the platform to add specific configurations to the switch
	if(platform_post_init_of12_switch(sw) != ROFL_SUCCESS){
		__of12_destroy_pipeline(sw->pipeline);	
		platform_free_shared(sw->name);
		platform_free_shared(sw);
		return NULL;
	}

	return sw;
}

rofl_result_t __of12_destroy_switch(of12_switch_t* sw){

	rofl_result_t result;

	//Allow the platform to do specific things before deletion 
	if(platform_pre_destroy_of12_switch(sw) != ROFL_SUCCESS)
		return ROFL_FAILURE;
		
	result = __of12_destroy_pipeline(sw->pipeline);

	//TODO: trace if result != SUCCESS
	(void)result;

	//TODO: rwlock
	
	platform_mutex_destroy(sw->mutex);
	
	platform_free_shared(sw->name);
	platform_free_shared(sw);
	
	return ROFL_SUCCESS;
}

/* Port management */
rofl_result_t __of12_attach_port_to_switch_at_port_num(of12_switch_t* sw, unsigned int port_num, switch_port_t* port){

	if(!port || !port_num || port_num >= LOGICAL_SWITCH_MAX_LOG_PORTS)
		return ROFL_FAILURE;	

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);
	
	if(sw->logical_ports[port_num].attachment_state){
		platform_mutex_unlock(sw->mutex);
		return ROFL_FAILURE;
	}

	//Initialize logical port 
	sw->logical_ports[port_num].attachment_state = LOGICAL_PORT_STATE_ATTACHED;
	sw->logical_ports[port_num].port = port;
	sw->num_of_ports++;
	
	//Initialize also port structure
	port->attached_sw = (of_switch_t*)sw;
	port->of_port_num = port_num; 

	//Return success
	platform_mutex_unlock(sw->mutex);
	return ROFL_SUCCESS;
}

rofl_result_t __of12_attach_port_to_switch(of12_switch_t* sw, switch_port_t* port, unsigned int* port_num){
	unsigned int i;

	if(!port || port->attached_sw) 
		return ROFL_FAILURE;	

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);

	for(i=1;i<LOGICAL_SWITCH_MAX_LOG_PORTS;i++){
		if(!sw->logical_ports[i].attachment_state){
			//Initialize logical port 
			sw->logical_ports[i].attachment_state = LOGICAL_PORT_STATE_ATTACHED;
			sw->logical_ports[i].port = port;
			sw->num_of_ports++;
			*port_num = i;
			
			//Initialize port
			port->attached_sw = (of_switch_t*)sw;
			port->of_port_num = i; 
				
			//Return success
			platform_mutex_unlock(sw->mutex);
			return ROFL_SUCCESS;
		}
	}
	
	//No slots free
	platform_mutex_unlock(sw->mutex);
	return ROFL_FAILURE;
}

rofl_result_t __of12_detach_port_from_switch_by_port_num(of12_switch_t* sw, unsigned int port_num){

	if(!port_num)
		return ROFL_FAILURE;

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);

	if(sw->logical_ports[port_num].attachment_state != LOGICAL_PORT_STATE_ATTACHED){
		platform_mutex_unlock(sw->mutex);
		return ROFL_FAILURE;
	}
	
	//Free port
	sw->logical_ports[port_num].port->attached_sw = NULL;
	sw->logical_ports[port_num].port->of_port_num = 0;

	sw->logical_ports[port_num].attachment_state = LOGICAL_PORT_STATE_DETACHED;
	sw->logical_ports[port_num].port = NULL;
	sw->num_of_ports--;
	
	//return success
	platform_mutex_unlock(sw->mutex);
	return ROFL_SUCCESS;
}

rofl_result_t __of12_detach_port_from_switch(of12_switch_t* sw, switch_port_t* port){

	unsigned int i;

	if(!port) 
		return ROFL_FAILURE;	

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);

	for(i=0;i<LOGICAL_SWITCH_MAX_LOG_PORTS;i++){
		if(sw->logical_ports[i].port == port){
			
			//Free port
			sw->logical_ports[i].port->attached_sw = NULL;
			sw->logical_ports[i].port->of_port_num = 0;

			//Detach
			sw->logical_ports[i].attachment_state = LOGICAL_PORT_STATE_DETACHED;
			sw->logical_ports[i].port = NULL;
			sw->num_of_ports--;

			platform_mutex_unlock(sw->mutex);
			return ROFL_SUCCESS;
		}
	}	
	
	//Not found 
	platform_mutex_unlock(sw->mutex);
	return ROFL_FAILURE;
}

rofl_result_t __of12_detach_all_ports_from_switch(of12_switch_t* sw){

	unsigned int i;

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);

	for(i=0;i<LOGICAL_SWITCH_MAX_LOG_PORTS;i++){
		
		//Mark port as non-attached to current sw
		if(sw->logical_ports[i].port)
			sw->logical_ports[i].port->attached_sw = NULL;
		
		//Detach
		sw->logical_ports[i].attachment_state = LOGICAL_PORT_STATE_DETACHED;
		sw->logical_ports[i].port = NULL;
	}	
	
	//Not found 
	platform_mutex_unlock(sw->mutex);
	return ROFL_SUCCESS;
}

/* Dumping */
void of12_dump_switch(of12_switch_t* sw){
	ROFL_PIPELINE_INFO("Openflow switch instance (%p)\n",sw);
	ROFL_PIPELINE_INFO("========================\n");
	ROFL_PIPELINE_INFO("Name: %s\n",sw->name);
	ROFL_PIPELINE_INFO("Openflow version: %d\n",sw->of_ver);
	ROFL_PIPELINE_INFO("Openflow datapathid: %" PRIu64 "\n",sw->dpid);
}

void of12_full_dump_switch(of12_switch_t* sw){
	int i;
	of12_dump_switch(sw);

	/* Dumping tables */		
	for(i=0;i<sw->pipeline->num_of_tables;i++)
		of12_dump_table(&sw->pipeline->tables[i]);
	ROFL_PIPELINE_INFO("--End of pipeline tables--\n\n");
}

