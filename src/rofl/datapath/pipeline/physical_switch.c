#include "physical_switch.h"

#include <assert.h>
#include "platform/memory.h"
#include "util/logging.h"

static physical_switch_t* psw;

//Meta port FLOOD shortcut
#define META_PORT_FLOOD_INDEX 0
switch_port_t* flood_meta_port;

//
// Physical switch mgmt
//

//init&destroy
rofl_result_t physical_switch_init(){

	ROFL_PIPELINE_DEBUG("Initializing physical switch\n");

	//Allocate memory for the physical switch structure
	psw = platform_malloc_shared(sizeof(physical_switch_t));
	
	if(!psw)
		return ROFL_FAILURE;	
	
	//FIXME: check error
	psw->mutex = platform_mutex_init(NULL);
	
	memset(psw->logical_switches, 0, sizeof(psw->logical_switches));
	psw->num_of_logical_switches = 0;	

	memset(psw->physical_ports, 0, sizeof(psw->physical_ports));
	memset(psw->tunnel_ports, 0, sizeof(psw->tunnel_ports));
	memset(psw->virtual_ports, 0, sizeof(psw->virtual_ports));
	memset(psw->meta_ports, 0, sizeof(psw->meta_ports));
	
	//Generate metaports
	//Flood
	psw->meta_ports[META_PORT_FLOOD_INDEX].type = PORT_TYPE_META_FLOOD;
	strncpy(psw->meta_ports[META_PORT_FLOOD_INDEX].name, "Flood meta port", SWITCH_PORT_MAX_LEN_NAME);

	//Set extern pointer
	flood_meta_port = &psw->meta_ports[META_PORT_FLOOD_INDEX];
		
	return ROFL_SUCCESS;	
}

void physical_switch_destroy(){
	
	unsigned int i;
	
	ROFL_PIPELINE_DEBUG("Destroying physical switch\n");

	//Serialize
	platform_mutex_lock(psw->mutex);

	//Destroying ports
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS;i++){
		if( psw->physical_ports[i] != NULL ){ 
			switch_port_destroy(psw->physical_ports[i]);
		}
	}
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS;i++){
		if( psw->virtual_ports[i] != NULL ){ 
			switch_port_destroy(psw->virtual_ports[i]);
		}
	}
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS;i++){
		if( psw->tunnel_ports[i] != NULL ){ 
			switch_port_destroy(psw->tunnel_ports[i]);
		}
	}

	//Destroy mutex
	platform_mutex_destroy(psw->mutex);
}

//
// Port management routines
//

//Get the port by its name
switch_port_t* physical_switch_get_port_by_name(const char *name){

	unsigned int i;

	//Physical ports	
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS;i++){
		if( psw->physical_ports[i] != NULL && strncmp(psw->physical_ports[i]->name, name, SWITCH_PORT_MAX_LEN_NAME)==0 ){
			return psw->physical_ports[i];
		}
	}

	//Virtual ports
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS;i++){
		if( psw->virtual_ports[i] != NULL && strncmp(psw->virtual_ports[i]->name, name, SWITCH_PORT_MAX_LEN_NAME)==0 ){
			return psw->virtual_ports[i];
		}
	}

	//Tunnel ports
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS;i++){
		if( psw->tunnel_ports[i] != NULL && strncmp(psw->tunnel_ports[i]->name, name, SWITCH_PORT_MAX_LEN_NAME)==0 ){
			return psw->tunnel_ports[i];
		}
	}

	return NULL;
}


/* FIXME: this is dangerous. Better go for a copy of the ports */
//Get the reference to the physical ports
switch_port_t** physical_switch_get_physical_ports(unsigned int* max_ports){
	*max_ports = PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS;
	return psw->physical_ports;
}
//Get the reference to the virtual ports
switch_port_t** physical_switch_get_virtual_ports(unsigned int* max_ports){
	*max_ports = PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS;
	return psw->virtual_ports;
}
//Get the reference to the physical ports
switch_port_t** physical_switch_get_tunnel_ports(unsigned int* max_ports){
	*max_ports = PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS;
	return psw->tunnel_ports;
}

/*
* Get a port by port_num
*/
switch_port_t* physical_switch_get_port_by_num(const uint64_t dpid, unsigned int port_num){

	of_switch_t* lsw;	

	lsw = physical_switch_get_logical_switch_by_dpid(dpid);

	if(!lsw)
		return NULL;	

	//Check port range
	if( port_num >= LOGICAL_SWITCH_MAX_LOG_PORTS || port_num == 0)  
		return NULL;
	
	if( lsw->logical_ports[port_num].attachment_state != LOGICAL_PORT_STATE_ATTACHED )
		return NULL;
	
	return lsw->logical_ports[port_num].port;
}

/*
* Attempts to add a port to the physical switch pool port
*/
rofl_result_t physical_switch_add_port(switch_port_t* port){

	unsigned int i, max;
	switch_port_t** array = NULL;


	if(!port)
		return ROFL_FAILURE;	

	ROFL_PIPELINE_DEBUG("Trying to add port(%p) named %s to the physical switch\n", port, port->name);
	
	if(physical_switch_get_port_by_name(port->name)){
		ROFL_PIPELINE_DEBUG("There is already a port named:%s in the physical switch\n",port->name);
		return ROFL_FAILURE;
	}

	//Serialize
	platform_mutex_lock(psw->mutex);

	switch(port->type){

		case PORT_TYPE_PHYSICAL:
			max = PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS;
			array = psw->physical_ports; 
			break;			

		case PORT_TYPE_VIRTUAL:
			max = PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS;
			array = psw->virtual_ports; 
			break;			

		case PORT_TYPE_TUNNEL:
			max = PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS;
			array = psw->tunnel_ports; 
			break;			
		
		default:
			//Invalid type		
			platform_mutex_unlock(psw->mutex);
			return ROFL_FAILURE;
	}	

	//Look for the first empty slot
	for(i=0;i<max;i++){
		if(array[i] == NULL){
			array[i] = port;
			platform_mutex_unlock(psw->mutex);
			return ROFL_SUCCESS;
		}
	}
	
	platform_mutex_unlock(psw->mutex);

	//No free slots left in the pool
	ROFL_PIPELINE_DEBUG("Insertion failed of port(%p); no available slots\n",port);
	return ROFL_FAILURE;
}

/*
* Removes and destroys a port from the physical_switch pool referenced by its name
*/
rofl_result_t physical_switch_remove_port(const char* name){

	unsigned int i;
	switch_port_t* port;

	if(!name)
		return ROFL_FAILURE;
	
	//Serialize
	platform_mutex_lock(psw->mutex);

	//Looking in the physical ports list 
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS;i++){
		
		if(  psw->physical_ports[i] != NULL && strncmp(psw->physical_ports[i]->name, name, SWITCH_PORT_MAX_LEN_NAME) == 0 ){
			port = psw->physical_ports[i]; 
			psw->physical_ports[i] = NULL;
			platform_mutex_unlock(psw->mutex);
			switch_port_destroy(port);
			return ROFL_SUCCESS;
		}
	}
	
	//Looking in the virtual ports list 
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS;i++){
		if(  psw->virtual_ports[i] != NULL && strncmp(psw->virtual_ports[i]->name, name, SWITCH_PORT_MAX_LEN_NAME) == 0 ){
			port = psw->virtual_ports[i]; 
			psw->virtual_ports[i] = NULL;
			platform_mutex_unlock(psw->mutex);
			switch_port_destroy(port);
			return ROFL_SUCCESS;
		}
	}

	//Looking in the tunnel ports list 
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS;i++){
		if(  psw->tunnel_ports[i] != NULL && strncmp(psw->tunnel_ports[i]->name, name, SWITCH_PORT_MAX_LEN_NAME) == 0 ){
			port = psw->tunnel_ports[i]; 
			psw->tunnel_ports[i] = NULL;
			platform_mutex_unlock(psw->mutex);
			switch_port_destroy(port);
			return ROFL_SUCCESS;
		}
	}

	platform_mutex_unlock(psw->mutex);

	//Port not found
	return ROFL_FAILURE;
	 	

}




//
// Logical switch management
//

/*
* Retrieves the list of logical switches within the logical switch 
*/
of_switch_t** physical_switch_get_logical_switches(unsigned int* max_switches){

	*max_switches = PHYSICAL_SWITCH_MAX_LS;
	
	return psw->logical_switches; 	
}


//Get logical switch
of_switch_t* physical_switch_get_logical_switch_by_dpid(const uint64_t dpid){
	int i;
	
	for(i=0;i<PHYSICAL_SWITCH_MAX_LS;i++){
		if( psw->logical_switches[i] && psw->logical_switches[i]->dpid == dpid )
			return psw->logical_switches[i];
	}

	return NULL;
}

//Add/remove methods
rofl_result_t physical_switch_add_logical_switch(of_switch_t* sw){
	int i;

	if(physical_switch_get_logical_switch_by_dpid(sw->dpid))
		return ROFL_FAILURE;
	
	//Serialize
	platform_mutex_lock(psw->mutex);

	// check bounds
	if(psw->num_of_logical_switches == PHYSICAL_SWITCH_MAX_LS){
		//Serialize
		platform_mutex_unlock(psw->mutex);
		return ROFL_FAILURE;
	}
	
	//Look for an available slot
	for(i=0;i<PHYSICAL_SWITCH_MAX_LS;i++){
		if(!psw->logical_switches[i])
			break;
	}

	psw->logical_switches[i] = sw;
	psw->num_of_logical_switches++;

	platform_mutex_unlock(psw->mutex);
	return ROFL_SUCCESS;
}

rofl_result_t physical_switch_remove_logical_switch_by_dpid(const uint64_t dpid){

	int i;
	of_switch_t* sw;

	ROFL_PIPELINE_DEBUG("Removing logical switch with dpid: %"PRIu64"\n",dpid);

	//Serialize
	platform_mutex_lock(psw->mutex);

	if(!physical_switch_get_logical_switch_by_dpid(dpid)){
		platform_mutex_unlock(psw->mutex);
		ROFL_PIPELINE_WARN("Logical switch not found\n");	
		return ROFL_FAILURE;
	}
	
	for(i=0;i<PHYSICAL_SWITCH_MAX_LS;i++){
		if(psw->logical_switches[i]->dpid == dpid){

			sw = psw->logical_switches[i];

			psw->logical_switches[i] = NULL;
			psw->num_of_logical_switches--;

			//Free the rest to do stuff with the physical sw
			platform_mutex_unlock(psw->mutex);

			//Destroy the switch				
			of_destroy_switch(sw);				
			
			return ROFL_SUCCESS;
		}
	}
	
	//This statement can never be reached	
	platform_mutex_unlock(psw->mutex);
	return ROFL_FAILURE;
}

rofl_result_t physical_switch_remove_logical_switch(of_switch_t* sw){
	return physical_switch_remove_logical_switch_by_dpid(sw->dpid);
}

//
// Logical switches port management
//


//Getters
of_switch_t* physical_switch_get_logical_switch_attached_to_port(const switch_port_t port){
	return (of_switch_t*)port.attached_sw;
}

//Platform port mapping methods
#if 0
rofl_result_t physical_switch_attach_physical_port_num_to_logical_switch(unsigned int port_num, of_switch_t* sw, unsigned int* logical_switch_port_num){
	
	rofl_result_t return_val;

	if( !sw || port_num >= PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS || psw->physical_ports[port_num]->attached_sw )
		return ROFL_FAILURE;
	
	//Serialize
	platform_mutex_lock(psw->mutex);

	return_val = of_attach_port_to_switch(sw, psw->physical_ports[port_num], logical_switch_port_num);
	
	platform_mutex_unlock(psw->mutex);
	return return_val;
}
#endif

rofl_result_t physical_switch_attach_port_to_logical_switch(switch_port_t* port, of_switch_t* sw, unsigned int* port_num){

	rofl_result_t return_val;

	if( !sw || !port || port->attached_sw )
		return ROFL_FAILURE;
	
	//Serialize
	platform_mutex_lock(psw->mutex);

	return_val = __of_attach_port_to_switch(sw, port, port_num);
	
	platform_mutex_unlock(psw->mutex);
	return return_val;

}

rofl_result_t physical_switch_attach_port_to_logical_switch_at_port_num(switch_port_t* port, of_switch_t* sw, unsigned int port_num){

	rofl_result_t return_val;

	if( !sw || !port || port->attached_sw )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw->mutex);

	return_val = __of_attach_port_to_switch_at_port_num(sw, port_num, port);

	platform_mutex_unlock(psw->mutex);
	return return_val;
}


rofl_result_t physical_switch_detach_port_num_from_logical_switch(unsigned int port_num, of_switch_t* sw){
	rofl_result_t return_val;

	if( !sw )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw->mutex);

	return_val = __of_detach_port_from_switch_by_port_num(sw, port_num);
	
	platform_mutex_unlock(psw->mutex);
	return return_val;

}

rofl_result_t physical_switch_detach_port_from_logical_switch(switch_port_t* port, of_switch_t* sw){

	rofl_result_t return_val;

	if( !sw || !port )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw->mutex);

	return_val = __of_detach_port_from_switch(sw, port);
	
	platform_mutex_unlock(psw->mutex);
	return return_val;


}

rofl_result_t physical_switch_detach_all_ports_from_logical_switch(of_switch_t* sw){

	rofl_result_t return_val;

	if( !sw )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw->mutex);

	return_val = __of_detach_all_ports_from_switch(sw);
	
	platform_mutex_unlock(psw->mutex);
	return return_val;

}




