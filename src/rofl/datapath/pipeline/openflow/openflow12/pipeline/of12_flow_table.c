#include "of12_flow_table.h"

#include "../openflow12.h" //FIXME: necessary for the OF12PAT_. Probably wise to redefine only them in of12_action.h
#include "../../../util/logging.h"

#include "of12_group_table.h"
#include "of12_pipeline.h"

/* 
* Openflow table operations
*
* Assumptions:
* - A Table MUST always have entries ordered by Nº of OF hits first, and for the priority field in second place
* - A Table contains no more than OF12_MAX_NUMBER_OF_TABLE_ENTRIES
* -  
*/

/*
* Table mgmt 
*/ 

/* Initalizer. Table struct has been allocated by pipeline initializer. */
rofl_result_t __of12_init_table(struct of12_pipeline* pipeline, of12_flow_table_t* table, const unsigned int table_index, const enum of12_matching_algorithm_available algorithm){

	//Safety checks
	if(!pipeline || !table)
		return ROFL_FAILURE;	

	//Initializing mutexes
	if(NULL == (table->mutex = platform_mutex_init(NULL)))
		return ROFL_FAILURE;
	if(NULL == (table->rwlock = platform_rwlock_init(NULL)))
		return ROFL_FAILURE;
	
	table->pipeline = pipeline;
	table->number = table_index;
	table->entries = NULL;
	table->num_of_entries = 0;
	table->max_entries = OF12_MAX_NUMBER_OF_TABLE_ENTRIES;
	table->default_action = OF12_TABLE_MISS_CONTROLLER;

	//Set name FIXME: put proper name (table0, table1...)
	strncpy(table->name,"table",OF12_MAX_TABLE_NAME_LEN);
	
	//Setting up the matching algorithm	
	if(! (algorithm < of12_matching_algorithm_count)){
		platform_mutex_destroy(table->mutex);
		platform_rwlock_destroy(table->rwlock);
		return ROFL_FAILURE;
	}

	//Auxiliary matching algorithm structs 
	table->matching_aux[0] = NULL; 
	table->matching_aux[1] = NULL;

	//Initializing timers. NOTE does that need to be done here or somewhere else?
#if OF12_TIMER_STATIC_ALLOCATION_SLOTS
	__of12_timer_group_static_init(table);
#else
	table->timers = NULL;
#endif

	//Set default behaviour MISS Controller	
	table->default_action = OF12_TABLE_MISS_CONTROLLER;
	
	/* Setting up basic characteristics of the table */
	table->config.table_miss_config = (1 << OF12_TABLE_MISS_CONTROLLER) | (1 << OF12_TABLE_MISS_CONTINUE) | (1 << OF12_TABLE_MISS_DROP);

	//Match
	table->config.match = 	 (1UL << OF12_MATCH_IN_PHY_PORT) |
				   //(1UL << OF12_MATCH_METADATA) |
				   (1UL << OF12_MATCH_ETH_DST) |
				   (1UL << OF12_MATCH_ETH_SRC) |
				   (1UL << OF12_MATCH_ETH_TYPE) |
				   (1UL << OF12_MATCH_VLAN_VID) |
				   (1UL << OF12_MATCH_VLAN_PCP) |
				   (1UL << OF12_MATCH_IP_DSCP) |
				   (1UL << OF12_MATCH_IP_ECN) |
				   (1UL << OF12_MATCH_IP_PROTO) |
				   (1UL << OF12_MATCH_IPV4_SRC) |
				   (1UL << OF12_MATCH_IPV4_DST) |
				   (1UL << OF12_MATCH_TCP_SRC) |
				   (1UL << OF12_MATCH_TCP_DST) |
				   (1UL << OF12_MATCH_UDP_SRC) |
				   (1UL << OF12_MATCH_UDP_DST) |
				   (1UL << OF12_MATCH_SCTP_SRC) |
				   (1UL << OF12_MATCH_SCTP_DST) |
				   (1UL << OF12_MATCH_ICMPV4_TYPE) |
				   (1UL << OF12_MATCH_ICMPV4_CODE) |
				   (UINT64_C(1) << OF12_MATCH_MPLS_LABEL) |
				   (UINT64_C(1) << OF12_MATCH_MPLS_TC) |
				   (UINT64_C(1) << OF12_MATCH_PPPOE_CODE) |
				   (UINT64_C(1) << OF12_MATCH_PPPOE_TYPE) |
				   (UINT64_C(1) << OF12_MATCH_PPPOE_SID) |
				   (UINT64_C(1) << OF12_MATCH_PPP_PROT);

	//Wildcards
	table->config.wildcards =  (1UL << OF12_MATCH_ETH_DST) |
				   (1UL << OF12_MATCH_ETH_SRC) |
				   (1UL << OF12_MATCH_VLAN_VID) |
				   (1UL << OF12_MATCH_IP_DSCP) |
				   (1UL << OF12_MATCH_IPV4_SRC) |
				   (1UL << OF12_MATCH_IPV4_DST) |
				   (1UL << OF12_MATCH_ICMPV4_TYPE) |
				   (1UL << OF12_MATCH_ICMPV4_CODE) |
				   (UINT64_C(1) << OF12_MATCH_MPLS_LABEL);

	//Write actions and apply actions
	table->config.apply_actions =   ( 1 << OF12PAT_OUTPUT ) |
					( 1 << OF12PAT_COPY_TTL_OUT ) |
					( 1 << OF12PAT_COPY_TTL_IN ) |
					( 1 << OF12PAT_SET_MPLS_TTL ) |
					( 1 << OF12PAT_DEC_MPLS_TTL ) |
					( 1 << OF12PAT_PUSH_VLAN ) |
					( 1 << OF12PAT_POP_VLAN ) |
					( 1 << OF12PAT_PUSH_MPLS ) |
					( 1 << OF12PAT_POP_MPLS ) |
					( 1 << OF12PAT_SET_QUEUE ) |
					//( 1 << OF12PAT_GROUP ) | //FIXME: add when groups are implemented 
					( 1 << OF12PAT_SET_NW_TTL ) |
					( 1 << OF12PAT_DEC_NW_TTL ) |
					( 1 << OF12PAT_SET_FIELD ) |
					( 1 << OF12PAT_PUSH_PPPOE ) |
					( 1 << OF12PAT_POP_PPPOE );
						
	/*
	//This is the translation to internal OF12_AT of above's statement
	
	table->config.apply_actions = (1U << OF12_AT_OUTPUT) |
					(1U << OF12_AT_COPY_TTL_OUT) |
					(1U << OF12_AT_COPY_TTL_IN ) |
					(1U << OF12_AT_SET_MPLS_TTL ) |
					(1U << OF12_AT_DEC_MPLS_TTL ) |
					(1U << OF12_AT_PUSH_VLAN ) |
					(1U << OF12_AT_POP_VLAN ) | 
					(1U << OF12_AT_PUSH_MPLS ) |
					(1U << OF12_AT_POP_MPLS ) |
					(1U << OF12_AT_SET_QUEUE ) |
					(1U << OF12_AT_GROUP ) |
					(1U << OF12_AT_SET_NW_TTL ) |
					(1U << OF12_AT_DEC_NW_TTL ) |
					(1U << OF12_AT_SET_FIELD ) | 
					(1U << OF12_AT_PUSH_PPPOE ) |
					(1U << OF12_AT_POP_PPPOE );
					//(1U << OF12_AT_EXPERIMENTER );
	*/


	table->config.write_actions = table->config.apply_actions;

	//Write actions and apply actions set fields
	table->config.write_setfields = table->config.match; 
	table->config.apply_setfields = table->config.match; 

	//Set fields
	table->config.metadata_match = 0x0; //FIXME: implement METADATA
	table->config.metadata_write = 0x0; //FIXME: implement METADATA

	//Instructions
	table->config.instructions = (1 << OF12PIT_GOTO_TABLE) |
					(1 << OF12PIT_WRITE_METADATA) |
					(1 << OF12PIT_WRITE_ACTIONS) |
					(1 << OF12PIT_APPLY_ACTIONS) |
					(1 << OF12PIT_CLEAR_ACTIONS);   
	
	
	//Init stats
	__of12_stats_table_init(table);

	//Allow matching algorithms to do stuff	
	if(of12_matching_algorithms[table->matching_algorithm].init_hook)
		return of12_matching_algorithms[table->matching_algorithm].init_hook(table);
	
	return ROFL_SUCCESS;
}

/* Destructor. Table object is freed by pipeline destructor */
rofl_result_t __of12_destroy_table(of12_flow_table_t* table){
	
	platform_mutex_lock(table->mutex);
	platform_rwlock_wrlock(table->rwlock);

	//Let the matching algorithm destroy its own state
	if(of12_matching_algorithms[table->matching_algorithm].destroy_hook)
		return of12_matching_algorithms[table->matching_algorithm].destroy_hook(table);

	platform_mutex_destroy(table->mutex);
	platform_rwlock_destroy(table->rwlock);
	
	//Destroy stats
	__of12_stats_table_destroy(table);

	//Do NOT free table, since it was allocated in a single buffer in pipeline.c	
	return ROFL_SUCCESS;
}


/* 
* Interfaces for generic add/remove flow entry 
* Specific matchings may point them to their own routines, but they MUST always call
* of12_[whatever]_flow_entry_table_imp in order to update the main tables
*/
inline rofl_of12_fm_result_t of12_add_flow_entry_table(of12_pipeline_t *const pipeline, const unsigned int table_id, of12_flow_entry_t *const entry, bool check_overlap, bool reset_counts){

	rofl_of12_fm_result_t result;
	of12_flow_table_t* table;
	
	//Verify table_id
	if(table_id >= pipeline->num_of_tables)
		return ROFL_OF12_FM_FAILURE;

	table = &pipeline->tables[table_id];

	//Take rd lock over the grouptable (avoid deletion of groups while flow entry insertion)
	platform_rwlock_rdlock(pipeline->groups->rwlock);

	//Verify entry
	if(__of12_validate_flow_entry(pipeline->groups, entry) != ROFL_SUCCESS){
		//Release rdlock
		platform_rwlock_rdunlock(pipeline->groups->rwlock);
		return ROFL_OF12_FM_FAILURE;
	}


	//Perform insertion
	result = of12_matching_algorithms[table->matching_algorithm].add_flow_entry_hook(table, entry, check_overlap, reset_counts);

	if(result != ROFL_OF12_FM_SUCCESS){
		//Release rdlock
		platform_rwlock_rdunlock(pipeline->groups->rwlock);
		return result;
	}
	
	//Add timer
	__of12_add_timer(table, entry);

	//Release rdlock
	platform_rwlock_rdunlock(pipeline->groups->rwlock);

	//Return value
	return result;
}


inline rofl_result_t of12_modify_flow_entry_table(of12_pipeline_t *const pipeline, const unsigned int table_id, of12_flow_entry_t *const entry, const enum of12_flow_removal_strictness strict, bool reset_counts){
	rofl_result_t result;
	of12_flow_table_t* table;
	
	//Verify table_id
	if(table_id >= pipeline->num_of_tables)
		return ROFL_FAILURE;

	table = &pipeline->tables[table_id];

	//Take rd lock over the grouptable (avoid deletion of groups while flow entry insertion)
	platform_rwlock_rdlock(pipeline->groups->rwlock);

	//Verify entry
	if(__of12_validate_flow_entry(pipeline->groups,entry) != ROFL_SUCCESS){
		//Release rdlock
		platform_rwlock_rdunlock(pipeline->groups->rwlock);
		return ROFL_FAILURE;
	}

	//Perform insertion
	result = of12_matching_algorithms[table->matching_algorithm].modify_flow_entry_hook(table, entry, strict, reset_counts);

	if(result != ROFL_SUCCESS){
		//Release rdlock
		platform_rwlock_rdunlock(pipeline->groups->rwlock);
		return result;
	}
	
	//Release rdlock
	platform_rwlock_rdunlock(pipeline->groups->rwlock);

	//Return value
	return result;
}

inline rofl_result_t of12_remove_flow_entry_table(of12_pipeline_t *const pipeline, const unsigned int table_id, of12_flow_entry_t* entry, const enum of12_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group){
	
	of12_flow_table_t* table;
	
	//Verify table_id
	if(table_id >= pipeline->num_of_tables)
		return ROFL_FAILURE;

	//Recover table pointer
	table = &pipeline->tables[table_id];
	
	return of12_matching_algorithms[table->matching_algorithm].remove_flow_entry_hook(table, entry, NULL, strict,  out_port, out_group, OF12_FLOW_REMOVE_DELETE, MUTEX_NOT_ACQUIRED);
}

//This API call should NOT be called from outside pipeline library
rofl_result_t __of12_remove_specific_flow_entry_table(of12_pipeline_t *const pipeline, const unsigned int table_id, of12_flow_entry_t *const specific_entry, of12_flow_remove_reason_t reason, of12_mutex_acquisition_required_t mutex_acquired){
	of12_flow_table_t* table;
	
	//Verify table_id
	if(table_id >= pipeline->num_of_tables)
		return ROFL_FAILURE;

	//Recover table pointer
	table = &pipeline->tables[table_id];
	
	return of12_matching_algorithms[table->matching_algorithm].remove_flow_entry_hook(table, NULL, specific_entry, STRICT, OF12_PORT_ANY, OF12_GROUP_ANY, reason, mutex_acquired);
}

/* Main process_packet_through */
inline of12_flow_entry_t* __of12_find_best_match_table(of12_flow_table_t *const table, of12_packet_matches_t *const pkt){
	return of12_matching_algorithms[table->matching_algorithm].find_best_match_hook(table, pkt);
}	


/* Dump methods */
void of12_dump_table(of12_flow_table_t* table){
	of12_flow_entry_t* entry;
	int i;	

	ROFL_PIPELINE_INFO("\n"); //This is done in purpose 
	ROFL_PIPELINE_INFO("Dumping table # %u (%p). Default action: %u. # of entries: %d\n", table->number, table, table->default_action,table->num_of_entries);
	
	if(!table->entries){
		ROFL_PIPELINE_INFO("\t[*] No entries\n");
		return;	
	}
	for(entry=table->entries, i=0;entry!=NULL;entry=entry->next,i++){
		ROFL_PIPELINE_INFO("\t[%d] ",i);
		of12_dump_flow_entry(entry);
	}
	
	ROFL_PIPELINE_INFO("\t[*] No more entries...\n");
	
	if(of12_matching_algorithms[table->matching_algorithm].dump_hook)
		of12_matching_algorithms[table->matching_algorithm].dump_hook(table);
}
