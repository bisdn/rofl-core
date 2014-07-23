/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "of1x_switch.h"

#include "../../switch_port.h"
#include "../../platform/lock.h"
#include "../../platform/memory.h"
#include "../../platform/likely.h"
#include "../../util/logging.h"
#include "../of_switch.h"
#include "of1x_async_events_hooks.h"

/* Initializer and destructor */
of1x_switch_t* of1x_init_switch(const char* name, of_version_t version, uint64_t dpid, unsigned int num_of_tables, enum of1x_matching_algorithm_available* list){

	of1x_switch_t* sw;
	sw = (of1x_switch_t*)platform_malloc_shared(sizeof(of1x_switch_t));
	if( unlikely(sw == NULL) )
		return NULL;

	//Filling in values
	sw->of_ver = version;
	sw->dpid = dpid;
	strncpy(sw->name,name,LOGICAL_SWITCH_MAX_LEN_NAME);

	//Initialize logical_ports
	platform_memset(sw->logical_ports,0,sizeof(logical_switch_port_t)*LOGICAL_SWITCH_MAX_LOG_PORTS);
	sw->num_of_ports=0;
	sw->max_ports=LOGICAL_SWITCH_MAX_LOG_PORTS;
	
	//Initialize platform state to NULL
	sw->platform_state=NULL;
	
	//Mutex
	if(NULL == (sw->mutex = platform_mutex_init(NULL))){
		platform_free_shared(sw);
		return NULL; 
	}
	
	//Setup pipeline	
	if(__of1x_init_pipeline(sw, num_of_tables, list) != ROFL_SUCCESS){
		platform_free_shared(sw);
		return NULL;
	}
	
	//Allow the platform to add specific configurations to the switch
	if(platform_post_init_of1x_switch(sw) != ROFL_SUCCESS){
		__of1x_destroy_pipeline(&sw->pipeline);	
		platform_free_shared(sw);
		return NULL;
	}

	return sw;
}

rofl_result_t __of1x_destroy_switch(of1x_switch_t* sw){

	rofl_result_t result;

	//Allow the platform to do specific things before deletion 
	if(platform_pre_destroy_of1x_switch(sw) != ROFL_SUCCESS)
		return ROFL_FAILURE;
		
	result = __of1x_destroy_pipeline(&sw->pipeline);

	//TODO: trace if result != SUCCESS
	(void)result;

	//TODO: rwlock
	
	platform_mutex_destroy(sw->mutex);
	platform_free_shared(sw);
	
	return ROFL_SUCCESS;
}

//Reconfiguration of the pipeline
rofl_result_t __of1x_reconfigure_switch(of1x_switch_t* sw, of_version_t version){

	if(sw->of_ver == version)
		return ROFL_SUCCESS;

	if(version == OF_VERSION_11)
		return ROFL_FAILURE; //Early return OF11 NOT supported

	//Purge all entries(flow&groups) in the pipeline
	if(__of1x_purge_pipeline_entries(&sw->pipeline) != ROFL_SUCCESS)
		return ROFL_FAILURE;

	//Set the default tables(flow and group tables) configuration according to the new version
	if(__of1x_set_pipeline_tables_defaults(&sw->pipeline, version) != ROFL_SUCCESS)
		return ROFL_FAILURE;

	//Set switch to the new version and return
	sw->of_ver = version;
	return ROFL_SUCCESS; 
}

/* Port management */
rofl_result_t __of1x_attach_port_to_switch_at_port_num(of1x_switch_t* sw, unsigned int port_num, switch_port_t* port){

	if( unlikely(port==NULL) || unlikely(port_num==0) || unlikely(port_num >= LOGICAL_SWITCH_MAX_LOG_PORTS) )
		return ROFL_FAILURE;	

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);
	
	if(sw->logical_ports[port_num].attachment_state != LOGICAL_PORT_STATE_FREE){
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

rofl_result_t __of1x_attach_port_to_switch(of1x_switch_t* sw, switch_port_t* port, unsigned int* port_num){
	unsigned int i;

	if( unlikely(port==NULL) || unlikely(port->attached_sw!=NULL) ) 
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

rofl_result_t __of1x_detach_port_from_switch_by_port_num(of1x_switch_t* sw, unsigned int port_num){

	if( unlikely(port_num==0) )
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

	//sw->logical_ports[port_num].attachment_state = LOGICAL_PORT_STATE_DETACHED;
	//Marking it as free so it can be reused
	sw->logical_ports[port_num].attachment_state = LOGICAL_PORT_STATE_FREE;
	sw->logical_ports[port_num].port = NULL;
	sw->num_of_ports--;
	
	//return success
	platform_mutex_unlock(sw->mutex);
	return ROFL_SUCCESS;
}

rofl_result_t __of1x_detach_port_from_switch(of1x_switch_t* sw, switch_port_t* port){

	unsigned int i;

	if( unlikely(port==NULL) ) 
		return ROFL_FAILURE;	

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);

	for(i=0;i<LOGICAL_SWITCH_MAX_LOG_PORTS;i++){
		if(sw->logical_ports[i].port == port){
			
			//Free port
			sw->logical_ports[i].port->attached_sw = NULL;
			sw->logical_ports[i].port->of_port_num = 0;

			//Detach
			//sw->logical_ports[i].attachment_state = LOGICAL_PORT_STATE_DETACHED;
			//Marking it as free so it can be reused
			sw->logical_ports[i].attachment_state = LOGICAL_PORT_STATE_FREE;
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

rofl_result_t __of1x_detach_all_ports_from_switch(of1x_switch_t* sw){

	unsigned int i;

	//Allow single add/remove operation over the switch 
	platform_mutex_lock(sw->mutex);

	for(i=0;i<LOGICAL_SWITCH_MAX_LOG_PORTS;i++){
		
		//Mark port as non-attached to current sw
		if(sw->logical_ports[i].port)
			sw->logical_ports[i].port->attached_sw = NULL;
		
		//Detach
		//sw->logical_ports[i].attachment_state = LOGICAL_PORT_STATE_DETACHED;
		//Marking it as free so it can be reused
		sw->logical_ports[i].attachment_state = LOGICAL_PORT_STATE_FREE;
		sw->logical_ports[i].port = NULL;
	}	
	
	//Not found 
	platform_mutex_unlock(sw->mutex);
	return ROFL_SUCCESS;
}

/* Dumping */
void of1x_dump_switch(of1x_switch_t* sw, bool nbo){
	ROFL_PIPELINE_INFO("OpenFlow switch instance (%p)\n",sw);
	ROFL_PIPELINE_INFO("========================\n");
	ROFL_PIPELINE_INFO("Name: %s\n",sw->name);
	ROFL_PIPELINE_INFO("OpenFlow version: %d\n",sw->of_ver);
	ROFL_PIPELINE_INFO("OpenFlow datapathid: 0x%" PRIx64 "\n",sw->dpid);
}

void of1x_full_dump_switch(of1x_switch_t* sw, bool nbo){
	int i;
	of1x_dump_switch(sw, nbo);

	/* Dumping tables */		
	for(i=0;i<sw->pipeline.num_of_tables;i++)
		of1x_dump_table(&sw->pipeline.tables[i], nbo);
	ROFL_PIPELINE_INFO("--End of pipeline tables--\n");
	
	of1x_dump_group_table(sw->pipeline.groups, nbo);
	ROFL_PIPELINE_DEBUG("--End of group table--\n\n");
}

//
// Snapshots
//

//Creates a snapshot of the running of LSI 
of1x_switch_snapshot_t* __of1x_switch_get_snapshot(of1x_switch_t* sw){

	int i;

	//Allocate a snapshot
	of1x_switch_snapshot_t* sn = platform_malloc_shared(sizeof(of1x_switch_snapshot_t));

	if(!sn)
		return NULL;

	//Serialize actions over the switch
	platform_mutex_lock(sw->mutex);
	
	//Copy contents
	memcpy(sn, sw, sizeof(of1x_switch_snapshot_t));

	//Cleanup the stuff that should not be exported
	sn->mutex = sn->platform_state = NULL;
	
	//Snapshot ports
	for(i=0;i<LOGICAL_SWITCH_MAX_LOG_PORTS;i++){
		if(sw->logical_ports[i].port){
			//Exists, snapshot it
			sn->logical_ports[i].port = __switch_port_get_snapshot(sw->logical_ports[i].port);
			if(!sn->logical_ports[i].port)
				goto ports_only_snapshoting_error;	
		}
	}
	
	//Snapshot pipeline	
	if(__of1x_pipeline_get_snapshot(&sw->pipeline, &sn->pipeline) != ROFL_SUCCESS)
		goto pipeline_snapshoting_error;	

	platform_mutex_unlock(sw->mutex);

	return sn;

pipeline_snapshoting_error:
	//There is no need to do nothing, since
	//call should have already cleanup his own memory
ports_only_snapshoting_error:
	//Delete snapshotted ports
	for(;i>0;i--){
		if(sn->logical_ports[i].port)
			switch_port_destroy_snapshot(sn->logical_ports[i].port);
	}

	platform_mutex_unlock(sw->mutex);
	
	//Free main snapshot
	platform_free_shared(sn);

	return NULL;
}

//Destroy a previously generated snapshot
void __of1x_switch_destroy_snapshot(of1x_switch_snapshot_t* sn){
	
	int i;

	//Let the pipeline destroy its state
	__of1x_pipeline_destroy_snapshot(&sn->pipeline);	
	
	//Destroy port snapshots
	for(i=0;i<LOGICAL_SWITCH_MAX_LOG_PORTS;i++){
		if(sn->logical_ports[i].port)
			switch_port_destroy_snapshot(sn->logical_ports[i].port);
	}
	
	//Free main snapshot
	platform_free_shared(sn);
}
