#include "physical_switch.h"

#include <assert.h>
#include "platform/memory.h"
#include "platform/likely.h"
#include "util/logging.h"
#include "openflow/of_switch.h"
#include "openflow/openflow1x/pipeline/matching_algorithms/matching_algorithms.h"

static physical_switch_t* psw=NULL;

//Meta port shortcuts
#define META_PORT_FLOOD_INDEX 0
#define META_PORT_IN_PORT_INDEX 1
#define META_PORT_ALL_INDEX 2

switch_port_t* flood_meta_port=NULL;
switch_port_t* in_port_meta_port=NULL;
switch_port_t* all_meta_port=NULL;


//
// Physical switch mgmt
//

//Generate the matching algorithm list for all OpenFlow versions	
void __physical_switch_generate_matching_algorithm_list(){
	__of1x_generate_matching_algorithm_list();
	//Add more versions here...
}

//Init
rofl_result_t physical_switch_init(){

	ROFL_PIPELINE_DEBUG("Initializing physical switch\n");

	//Allocate memory for the physical switch structure
	psw = platform_malloc_shared(sizeof(physical_switch_t));
	
	if( unlikely(psw==NULL) )
		return ROFL_FAILURE;	
	
	psw->mutex = platform_mutex_init(NULL);
	if(!psw->mutex)
		return ROFL_FAILURE;
	
	platform_memset(psw->logical_switches, 0, sizeof(psw->logical_switches));
	psw->num_of_logical_switches = 0;	

	platform_memset(psw->physical_ports, 0, sizeof(psw->physical_ports));
	platform_memset(psw->tunnel_ports, 0, sizeof(psw->tunnel_ports));
	platform_memset(psw->virtual_ports, 0, sizeof(psw->virtual_ports));
	platform_memset(psw->meta_ports, 0, sizeof(psw->meta_ports));
	
	//Generate metaports
	//Flood
	psw->meta_ports[META_PORT_FLOOD_INDEX].type = PORT_TYPE_META_FLOOD;
	strncpy(psw->meta_ports[META_PORT_FLOOD_INDEX].name, "Flood meta port", SWITCH_PORT_MAX_LEN_NAME);
	//In port
	psw->meta_ports[META_PORT_IN_PORT_INDEX].type = PORT_TYPE_META_IN_PORT;
	strncpy(psw->meta_ports[META_PORT_IN_PORT_INDEX].name, "In port meta port", SWITCH_PORT_MAX_LEN_NAME);
	//All
	psw->meta_ports[META_PORT_ALL_INDEX].type = PORT_TYPE_META_ALL;
	strncpy(psw->meta_ports[META_PORT_ALL_INDEX].name, "All meta port", SWITCH_PORT_MAX_LEN_NAME);

	//Set extern pointer
	flood_meta_port = &psw->meta_ports[META_PORT_FLOOD_INDEX];
	in_port_meta_port = &psw->meta_ports[META_PORT_IN_PORT_INDEX];
	all_meta_port = &psw->meta_ports[META_PORT_ALL_INDEX];

	//Initialize monitoring data
	if(__monitoring_init(&psw->monitoring) != ROFL_SUCCESS)
		return ROFL_FAILURE;		

	//Generate matching algorithm lists
	__physical_switch_generate_matching_algorithm_list();

	return ROFL_SUCCESS;	
}

physical_switch_t* get_physical_switch(){
	return psw;
}
//Only used in multi-process deployments (with shared memory)
void __set_physical_switch(physical_switch_t* sw){
	psw = sw;
}

//Destroy
void physical_switch_destroy(){
	
	unsigned int i;
	
	ROFL_PIPELINE_DEBUG("Destroying physical switch\n");

	//Serialize
	platform_mutex_lock(psw->mutex);

	//Destroy logical switches
	for(i=0;i<PHYSICAL_SWITCH_MAX_LS;i++){
		if(psw->logical_switches[i])
			of_destroy_switch(psw->logical_switches[i]);	
	}

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

	//Destroy monitoring
	__monitoring_destroy(&psw->monitoring);		
	
	//Destroy mutex
	platform_mutex_destroy(psw->mutex);
	
	//destroy physical switch
	platform_free_shared(psw);
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

	if( unlikely(lsw==NULL) )
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


	if( unlikely(port==NULL) )
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

	if( unlikely(name==NULL) )
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

	ROFL_PIPELINE_DEBUG("Removing logical switch with dpid: 0x%"PRIx64"\n",dpid);

	//Serialize
	platform_mutex_lock(psw->mutex);

	if(!physical_switch_get_logical_switch_by_dpid(dpid)){
		platform_mutex_unlock(psw->mutex);
		ROFL_PIPELINE_WARN("Logical switch not found\n");	
		return ROFL_FAILURE;
	}
	
	for(i=0;i<PHYSICAL_SWITCH_MAX_LS;i++){
		if(psw->logical_switches[i] && psw->logical_switches[i]->dpid == dpid){

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

//Attach
rofl_result_t physical_switch_attach_port_to_logical_switch(switch_port_t* port, of_switch_t* sw, unsigned int* port_num){

	rofl_result_t return_val;

	if( unlikely(sw==NULL) || unlikely(port==NULL) || unlikely(port->attached_sw!=NULL) )
		return ROFL_FAILURE;
	
	//Serialize
	platform_mutex_lock(psw->mutex);

	return_val = __of_attach_port_to_switch(sw, port, port_num);
	
	platform_mutex_unlock(psw->mutex);
	return return_val;

}

rofl_result_t physical_switch_attach_port_to_logical_switch_at_port_num(switch_port_t* port, of_switch_t* sw, unsigned int port_num){

	rofl_result_t return_val;

	if( unlikely(sw==NULL) || unlikely(port==NULL) || unlikely(port->attached_sw!=NULL) )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw->mutex);

	return_val = __of_attach_port_to_switch_at_port_num(sw, port_num, port);

	platform_mutex_unlock(psw->mutex);
	return return_val;
}


rofl_result_t physical_switch_detach_port_num_from_logical_switch(unsigned int port_num, of_switch_t* sw){
	rofl_result_t return_val;

	if( unlikely(sw==NULL) )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw->mutex);

	return_val = __of_detach_port_from_switch_by_port_num(sw, port_num);
	
	platform_mutex_unlock(psw->mutex);
	return return_val;

}

rofl_result_t physical_switch_detach_port_from_logical_switch(switch_port_t* port, of_switch_t* sw){

	rofl_result_t return_val;

	if( unlikely(sw==NULL) || unlikely(port==NULL) )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw->mutex);

	return_val = __of_detach_port_from_switch(sw, port);
	
	platform_mutex_unlock(psw->mutex);
	return return_val;


}

rofl_result_t physical_switch_detach_all_ports_from_logical_switch(of_switch_t* sw){

	rofl_result_t return_val;

	if( unlikely(sw==NULL) )
		return ROFL_FAILURE;

	//Serialize
	platform_mutex_lock(psw->mutex);

	return_val = __of_detach_all_ports_from_switch(sw);
	
	platform_mutex_unlock(psw->mutex);
	return return_val;

}


//
// Snapshots
//

//List of ports
switch_port_name_list_t* physical_switch_get_all_port_names(void){

	switch_port_name_list_t* list;
	__switch_port_name_t* names;
	unsigned int num_of_ports, i;

	//Serialize
	platform_mutex_lock(psw->mutex);

	//Determine the number of (currenly) exisitng ports
	num_of_ports=0;
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS;i++){
		if(psw->physical_ports[i])
			num_of_ports++;
	}
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS;i++){
		if(psw->tunnel_ports[i])
			num_of_ports++;
	}
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS;i++){
		if(psw->virtual_ports[i])
			num_of_ports++;
	}
	
	//Allocate memory
	list = platform_malloc_shared(sizeof(switch_port_name_list_t));
	names = platform_malloc_shared(sizeof(__switch_port_name_t)*num_of_ports);

	if(!list || !names){
		platform_mutex_unlock(psw->mutex);
		if(list)
			platform_free_shared(list);
		if(names)
			platform_free_shared(names);
		return NULL;
	}
	
	//Fill in
	list->names = names;
	list->num_of_ports = num_of_ports;

	num_of_ports=0;
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS;i++){
		if(psw->physical_ports[i]){
			memcpy(&list->names[num_of_ports], &psw->physical_ports[i]->name, SWITCH_PORT_MAX_LEN_NAME);
			num_of_ports++;
		}
	}
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS;i++){
		if(psw->tunnel_ports[i]){
			memcpy(&list->names[num_of_ports], &psw->tunnel_ports[i]->name, SWITCH_PORT_MAX_LEN_NAME);
			num_of_ports++;
		}
	}
	for(i=0;i<PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS;i++){
		if(psw->virtual_ports[i]){
			memcpy(&list->names[num_of_ports], &psw->virtual_ports[i]->name, SWITCH_PORT_MAX_LEN_NAME);
			num_of_ports++;
		}
	}
	
	platform_mutex_unlock(psw->mutex);
	
	return list;
}

//Get the port snapshot
switch_port_snapshot_t* physical_switch_get_port_snapshot(const char* name){

	switch_port_t* port;
	switch_port_snapshot_t* snapshot;

	if(!name)
		return NULL;

	//Serialize
	platform_mutex_lock(psw->mutex);

	port = physical_switch_get_port_by_name(name);
	
	if(!port){
		platform_mutex_unlock(psw->mutex);
		return NULL;
	} 

	snapshot = __switch_port_get_snapshot(port);

	platform_mutex_unlock(psw->mutex);
	
	return snapshot;
}

//LSIs
dpid_list_t* physical_switch_get_all_lsi_dpids(void){

	int i,j;
	dpid_list_t* list;

	list = platform_malloc_shared(sizeof(dpid_list_t));

	//Prevent management actions to screw the walk through the LSIs
	platform_mutex_lock(psw->mutex);

	//Set the number of elements
	list->num_of_lsis = psw->num_of_logical_switches;

	//Allocate the list space
	list->dpids = platform_malloc_shared(sizeof(uint64_t)*list->num_of_lsis);
	
	if(!list->dpids){
		platform_mutex_unlock(psw->mutex);
		return NULL;
	}
	
	//Fill it with 0s	
	platform_memset(list->dpids,0,sizeof(uint64_t)*list->num_of_lsis);
	for(i=0,j=0;i<PHYSICAL_SWITCH_MAX_LS;i++){
		if(psw->logical_switches[i]){
			list->dpids[j] = psw->logical_switches[i]->dpid; 
			j++;
		}
	}

	platform_mutex_unlock(psw->mutex);
	
	return list;
}

void dpid_list_destroy(dpid_list_t* list){
	platform_free_shared(list->dpids);
	platform_free_shared(list);
}

of_switch_snapshot_t* physical_switch_get_logical_switch_snapshot(const uint64_t dpid){

	of_switch_t* sw;
	of_switch_snapshot_t* to_return=NULL;

	
	//Serialize
	platform_mutex_lock(psw->mutex);

	//Try to find the switch
	sw = physical_switch_get_logical_switch_by_dpid(dpid);  

	if(sw)
		to_return = __of_switch_get_snapshot(sw); 

	platform_mutex_unlock(psw->mutex);

	return to_return;
}
