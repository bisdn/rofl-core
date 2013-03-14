#include "physical_switch.h"

#include <assert.h>

static physical_switch_t psw;

//Meta port FLOOD shortcut
#define META_PORT_FLOOD_INDEX 0
switch_port_t* flood_meta_port = &psw.meta_ports[META_PORT_FLOOD_INDEX];

//init&destroy
void physical_switch_init(){
	//FIXME: check error
	psw.mutex = platform_mutex_init(NULL);
	
	memset(psw.physical_ports,0,sizeof(psw.physical_ports));
	memset(psw.tunnel_ports,0,sizeof(psw.tunnel_ports));
	memset(psw.virtual_ports,0,sizeof(psw.virtual_ports));
	memset(psw.meta_ports,0,sizeof(psw.meta_ports));
	
	//Generate metaports
	//Flood
	psw.meta_ports[META_PORT_FLOOD_INDEX].type = PORT_TYPE_META_FLOOD;
	strncpy(psw.meta_ports[META_PORT_FLOOD_INDEX].name, "Flood meta port", SWITCH_PORT_MAX_LEN_NAME);
}

physical_switch_t* get_physical_switch(){
	return &psw;
}

void physical_switch_destroy(){
	platform_mutex_destroy(psw.mutex);
}

//General static methods
of_switch_t* physical_switch_get_logical_switch_by_dpid(const uint64_t dpid){
	int i;
	
	for(i=0;i<PHYSICAL_SWITCH_MAX_LS;i++){
		if( psw.logical_switches[i] && psw.logical_switches[i]->dpid == dpid)
			return psw.logical_switches[i];
	}

	return NULL;
}

/*
 * Get the port by its name
 */
switch_port_t* physical_switch_get_port_by_name(const char *name){

	unsigned int i;
	
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS;i++){
		if(psw.physical_ports[i] != NULL && strncmp(psw.physical_ports[i]->name,name, SWITCH_PORT_MAX_LEN_NAME)==0){
			return psw.physical_ports[i];
		}
	}

	//TODO: add tunnel, virtual...

	return NULL;
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

//Add/remove methods
rofl_result_t physical_switch_add_logical_switch(of_switch_t* sw){
	int i;

	if(physical_switch_get_logical_switch_by_dpid(sw->dpid))
		return ROFL_FAILURE;
	
	//Serialize
	platform_mutex_lock(psw.mutex);

	if(psw.num_of_logical_switches == PHYSICAL_SWITCH_MAX_LS){
		//Serialize
		platform_mutex_unlock(psw.mutex);
		return ROFL_FAILURE;
	}
	
	//Look for an available slot
	for(i=0;i<PHYSICAL_SWITCH_MAX_LS;i++){
		if(!psw.logical_switches[i])
			break;
	}

	// check bounds
	if (PHYSICAL_SWITCH_MAX_LS <= i) {
		assert(0);
		platform_mutex_unlock(psw.mutex);
		return ROFL_FAILURE;
	}

	psw.logical_switches[i] = sw;
	psw.num_of_logical_switches++;

	platform_mutex_unlock(psw.mutex);
	return ROFL_SUCCESS;
}

rofl_result_t physical_switch_remove_logical_switch_by_dpid(const uint64_t dpid){

	int i;
	of_switch_t* sw;

	//Serialize
	platform_mutex_lock(psw.mutex);

	if(!physical_switch_get_logical_switch_by_dpid(dpid)){
		platform_mutex_unlock(psw.mutex);
		return ROFL_FAILURE;
	}
	
	for(i=0;i<PHYSICAL_SWITCH_MAX_LS;i++){
		if(psw.logical_switches[i]->dpid == dpid){

			sw = psw.logical_switches[i];

			psw.logical_switches[i] = NULL;
			psw.num_of_logical_switches--;

			//Free the rest to do stuff with the physical sw
			platform_mutex_unlock(psw.mutex);

			//Destroy the switch				
			of_destroy_switch(sw);				
			
			return ROFL_SUCCESS;
		}
	}
	
	//This statement can never be reached	
	platform_mutex_unlock(psw.mutex);
	return ROFL_FAILURE;
}

rofl_result_t physical_switch_remove_logical_switch(of_switch_t* sw){
	return physical_switch_remove_logical_switch_by_dpid(sw->dpid);
}

//Getters
of_switch_t* physical_switch_get_logical_switch_attached_to_port(const switch_port_t port){
	return (of_switch_t*)port.attached_sw;
}

//Platform port mapping methods
//TODO: take this from here
rofl_result_t get_logical_switch_ports(of_switch_t* sw, logical_switch_port_t** ports, unsigned int* num_of_ports, unsigned int* logical_sw_max_ports){
	if(!sw)
		return ROFL_FAILURE;

	return of_get_switch_ports(sw,ports,num_of_ports,logical_sw_max_ports);
}

rofl_result_t physical_switch_attach_physical_port_num_to_logical_switch(unsigned int port_num, of_switch_t* sw, unsigned int* logical_switch_port_num){
	
	rofl_result_t return_val;

	if( !sw || port_num >= PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS || psw.physical_ports[port_num]->attached_sw )
		return ROFL_FAILURE;
	
	//Serialize
	platform_mutex_lock(psw.mutex);

	return_val = of_attach_port_to_switch(sw, psw.physical_ports[port_num], logical_switch_port_num);
	
	platform_mutex_unlock(psw.mutex);
	return return_val;
}

rofl_result_t physical_switch_attach_port_to_logical_switch(switch_port_t* port, of_switch_t* sw, unsigned int* port_num){

	rofl_result_t return_val;

	if( !sw || !port || port->attached_sw )
		return ROFL_FAILURE;
	
	//Serialize
	platform_mutex_lock(psw.mutex);

	return_val = of_attach_port_to_switch(sw, port, port_num);
	
	platform_mutex_unlock(psw.mutex);
	return return_val;

}

rofl_result_t physical_switch_attach_port_to_logical_switch_at_port_num(switch_port_t* port, of_switch_t* sw, unsigned int port_num){

	rofl_result_t return_val;

	if( !sw || !port || port->attached_sw )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw.mutex);

	return_val = of_attach_port_to_switch_at_port_num(sw, port_num, port);

	platform_mutex_unlock(psw.mutex);
	return return_val;
}


rofl_result_t physical_switch_detach_port_num_from_logical_switch(unsigned int port_num, of_switch_t* sw){
	rofl_result_t return_val;

	if( !sw )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw.mutex);

	return_val = of_detach_port_from_switch_by_port_num(sw, port_num);
	
	platform_mutex_unlock(psw.mutex);
	return return_val;

}

rofl_result_t physical_switch_detach_port_from_logical_switch(switch_port_t* port, of_switch_t* sw){

	rofl_result_t return_val;

	if( !sw || !port )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw.mutex);

	return_val = of_detach_port_from_switch(sw, port);
	
	platform_mutex_unlock(psw.mutex);
	return return_val;


}

rofl_result_t physical_switch_detach_all_ports_from_logical_switch(of_switch_t* sw){

	rofl_result_t return_val;

	if( !sw )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw.mutex);

	return_val = of_detach_all_ports_from_switch(sw);
	
	platform_mutex_unlock(psw.mutex);
	return return_val;

}




