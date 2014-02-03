#include "switch_port.h"

#include <string.h>
#include "platform/memory.h"
#include "openflow/of_switch.h"

/*
* Inits a port structure including statistics 
*/
switch_port_t* switch_port_init(char* name, bool up, port_type_t type, port_state_t state){

	switch_port_t* port;

	//Allocate space
	port = platform_malloc_shared(sizeof(switch_port_t));

	if(!port)
		return NULL;

	//Memset stats. Must be here!
	memset(&port->stats,0,sizeof(port->stats));
	
	//Clear queues
	port->max_queues = SWITCH_PORT_MAX_QUEUES;	
	memset(&port->queues,0,sizeof(port->queues));

	//Init mutexes 
	port->mutex = platform_mutex_init(NULL);
	if(!port->mutex){
		platform_free_shared(port);
		return NULL;
	}

	port->stats.mutex = platform_mutex_init(NULL);
	if(!port->stats.mutex){
		platform_mutex_destroy(port->mutex);
		platform_free_shared(port);
		return NULL;
	}

	//Fill values	
	port->type = type;
	port->up = up;
	port->state = state;

	//Copy name	
	strncpy(port->name, name, SWITCH_PORT_MAX_LEN_NAME);

	//Set to initial values the rest
	port->forward_packets = true;	
	port->drop_received = false; 
	port->no_flood = false;
	port->curr = port->advertised = port->supported = port->peer = 0x0;	
	port->curr_speed = port->curr_max_speed = 0;
	port->of_port_num = 0;
	port->of_generate_packet_in = true;
	port->attached_sw = NULL;

	//Platform state
	port->platform_port_state = NULL;

	return port;
}

rofl_result_t switch_port_destroy(switch_port_t* port){

	unsigned int i;

	//Destroy queues
	for(i=0;i<SWITCH_PORT_MAX_QUEUES;i++){
		if(port->queues[i].set)
			__port_queue_destroy(&port->queues[i]);
	}

	//Destroy port stats mutex
	platform_mutex_destroy(port->stats.mutex);
	
	//Destroy port mutex
	platform_mutex_destroy(port->mutex);
	
	//Free shared port
	platform_free_shared(port);
	
	return ROFL_SUCCESS;
}


/*
* Add queue to port 
*/
rofl_result_t switch_port_add_queue(switch_port_t* port, uint32_t id, char* name, uint16_t length, uint16_t min_rate, uint16_t max_rate){
	
	platform_mutex_lock(port->mutex);

	if(id >=SWITCH_PORT_MAX_QUEUES ||
		port->queues[id].set ){
		platform_mutex_unlock(port->mutex);
		return ROFL_FAILURE;
	}
	
	//Init switch queue
	__port_queue_init(&port->queues[id], id, name, length, min_rate, max_rate);

	platform_mutex_unlock(port->mutex);
	return ROFL_SUCCESS;
	
}

/*
* Remove queue from port 
*/
rofl_result_t switch_port_remove_queue(switch_port_t* port, uint32_t id){

	platform_mutex_lock(port->mutex);

	if(id >=SWITCH_PORT_MAX_QUEUES ||
		!port->queues[id].set ){
		platform_mutex_unlock(port->mutex);
		return ROFL_FAILURE;
	}
	
	//destroy queue
	__port_queue_destroy(&port->queues[id]);

	platform_mutex_unlock(port->mutex);
	return ROFL_SUCCESS;
}

/*
* Conveninent wrappers just to avoid messing up with the bitmaps
*/
void switch_port_add_capabilities(bitmap32_t* bitmap, bitmap32_t features){
	*bitmap |= features;
}
void switch_port_remove_capabilities(bitmap32_t* bitmap, bitmap32_t features){
	*bitmap &= (~features);
}
void switch_port_set_current_speed(switch_port_t* port, port_features_t speed){
	if(speed > PORT_FEATURE_1TB_FD)
		return;
	port->curr_speed = speed;
}
void switch_port_set_current_max_speed(switch_port_t* port, port_features_t speed){
	if(speed > PORT_FEATURE_1TB_FD)
		return;
	port->curr_max_speed = speed;
}


/*
* Internal call to copy the port
*/
switch_port_snapshot_t* __switch_port_get_snapshot(switch_port_t* port){

	int i;

	//Allocate it
	switch_port_snapshot_t* s = platform_malloc_shared(sizeof(switch_port_snapshot_t)); 

	if(!s)
		return NULL;

	//Copy the contents of the port
	memcpy(s, port, sizeof(switch_port_snapshot_t)); //switch_port_t == switch_port_snapshot_t
	
	//Leave the appropiate values blank;
	s->platform_port_state=s->mutex=s->attached_sw=s->stats.mutex=NULL;
	for(i=0;i<SWITCH_PORT_MAX_QUEUES;i++)
		s->queues[i].stats.mutex = NULL;
	
	//Copy missing information
	s->attached_sw = NULL;
	s->is_attached_to_sw = (port->attached_sw != NULL);
	if(port->attached_sw)
		s->attached_sw_dpid = port->attached_sw->dpid;
	else
		s->attached_sw_dpid = 0x0;
		
	return s;
}
void switch_port_destroy_snapshot(switch_port_snapshot_t* port){
	if(port)
		platform_free_shared(port);
}

//Destroy a port name list
void switch_port_name_list_destroy(switch_port_name_list_t* list){
	if(list){
		platform_free_shared(list->names);
		platform_free_shared(list);
	}
}
