#include "of1x_flow_table.h"

#include "../../../platform/likely.h"
#include "../../../util/logging.h"

#include "of1x_group_table.h"
#include "of1x_pipeline.h"
#include "of1x_action.h"
#include "of1x_match.h"
#include "../of1x_switch.h"




/* 
* OpenFlow table operations
*
* Assumptions:
* - A Table MUST always have entries ordered by Nº of OF hits first, and for the priority field in second place
* - A Table contains no more than OF1X_MAX_NUMBER_OF_TABLE_ENTRIES
* -  
*/

/*
* Table mgmt 
*/ 

void __of10_set_table_defaults(of1x_flow_table_t* table){

	//Set default behaviour MISS Controller	
	//XXX: Original of10 was not not handling more than one table, why does
	//the spec now have it? 
	table->default_action = OF1X_TABLE_MISS_CONTROLLER;
	
	/* Setting up basic characteristics of the table */
	table->config.table_miss_config = 0x0; //No meaning in OF1.0

	//Match
	bitmap128_clean(&table->config.match);
	bitmap128_set(&table->config.match, OF1X_MATCH_IN_PORT);
	bitmap128_set(&table->config.match, OF1X_MATCH_ETH_DST);
	bitmap128_set(&table->config.match, OF1X_MATCH_ETH_SRC);
	bitmap128_set(&table->config.match, OF1X_MATCH_ETH_TYPE);
	bitmap128_set(&table->config.match, OF1X_MATCH_VLAN_VID);
	bitmap128_set(&table->config.match, OF1X_MATCH_VLAN_PCP);
	bitmap128_set(&table->config.match, OF1X_MATCH_IP_DSCP);
	bitmap128_set(&table->config.match, OF1X_MATCH_NW_PROTO);
	bitmap128_set(&table->config.match, OF1X_MATCH_NW_SRC);
	bitmap128_set(&table->config.match, OF1X_MATCH_NW_DST);
	bitmap128_set(&table->config.match, OF1X_MATCH_TP_SRC); //Only for OF10
	bitmap128_set(&table->config.match, OF1X_MATCH_TP_DST); //Only for OF10
	bitmap128_set(&table->config.match, OF1X_MATCH_PPPOE_CODE);
	bitmap128_set(&table->config.match, OF1X_MATCH_PPPOE_TYPE);
	bitmap128_set(&table->config.match, OF1X_MATCH_PPPOE_SID);
	bitmap128_set(&table->config.match, OF1X_MATCH_PPP_PROT);
	bitmap128_set(&table->config.match, OF1X_MATCH_GTP_MSG_TYPE);
	bitmap128_set(&table->config.match, OF1X_MATCH_GTP_TEID);

	//Wildcards
	bitmap128_clean(&table->config.wildcards);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_ETH_DST);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_ETH_SRC);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_NW_SRC);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_NW_DST);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_MPLS_LABEL);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_GTP_TEID);

	//Apply actions
	bitmap128_clean(&table->config.apply_actions);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_ETH_DST);
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_COPY_TTL_IN
	bitmap128_set(&table->config.apply_actions, OF1X_AT_POP_VLAN);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_POP_PPPOE);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_PUSH_PPPOE);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_PUSH_VLAN);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_DEC_NW_TTL);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_NW_TTL);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_QUEUE);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ETH_DST);  		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ETH_SRC);  		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ETH_TYPE); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_VLAN_VID); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_VLAN_PCP); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_NW_PROTO);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_NW_SRC);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_NW_DST);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IP_DSCP);  		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IP_ECN);   		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IP_PROTO); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IPV4_SRC); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IPV4_DST); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_TP_SRC);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_TP_DST);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_PPPOE_CODE);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_PPPOE_TYPE);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_PPPOE_SID);	   	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_PPP_PROT); 	   	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_GTP_MSG_TYPE);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_GTP_TEID);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_OUTPUT);
	
	//Write actions	
	bitmap128_clean(&table->config.write_actions); //Not supported in OF10 

	//METADATA
	table->config.metadata_match = 0x0; //Not supported in OF10
	table->config.metadata_write = 0x0; //Not supported in OF10

	//Instructions
	table->config.instructions = 0x0; //Not supported in OF10 

}

void __of12_set_table_defaults(of1x_flow_table_t* table){

	//Set default behaviour MISS Controller	
	table->default_action = OF1X_TABLE_MISS_CONTROLLER;
	
	/* Setting up basic characteristics of the table */
	table->config.table_miss_config = (1 << OF1X_TABLE_MISS_CONTROLLER) | (1 << OF1X_TABLE_MISS_CONTINUE) | (1 << OF1X_TABLE_MISS_DROP);

	//Match
	bitmap128_clean(&table->config.match);
	bitmap128_set(&table->config.match, OF1X_MATCH_IN_PORT);
	bitmap128_set(&table->config.match, OF1X_MATCH_IN_PHY_PORT);
	bitmap128_set(&table->config.match, OF1X_MATCH_METADATA);
	bitmap128_set(&table->config.match, OF1X_MATCH_ETH_DST);
	bitmap128_set(&table->config.match, OF1X_MATCH_ETH_SRC);
	bitmap128_set(&table->config.match, OF1X_MATCH_ETH_TYPE);
	bitmap128_set(&table->config.match, OF1X_MATCH_VLAN_VID);
	bitmap128_set(&table->config.match, OF1X_MATCH_VLAN_PCP);
	bitmap128_set(&table->config.match, OF1X_MATCH_ARP_OP);
	bitmap128_set(&table->config.match, OF1X_MATCH_ARP_SHA);
	bitmap128_set(&table->config.match, OF1X_MATCH_ARP_SPA);
	bitmap128_set(&table->config.match, OF1X_MATCH_ARP_THA);
	bitmap128_set(&table->config.match, OF1X_MATCH_ARP_TPA);
	bitmap128_set(&table->config.match, OF1X_MATCH_IP_DSCP);
	bitmap128_set(&table->config.match, OF1X_MATCH_IP_ECN);
	bitmap128_set(&table->config.match, OF1X_MATCH_IP_PROTO);
	bitmap128_set(&table->config.match, OF1X_MATCH_IPV4_SRC);
	bitmap128_set(&table->config.match, OF1X_MATCH_IPV4_DST);
	bitmap128_set(&table->config.match, OF1X_MATCH_TCP_SRC);
	bitmap128_set(&table->config.match, OF1X_MATCH_TCP_DST);
	bitmap128_set(&table->config.match, OF1X_MATCH_UDP_SRC);
	bitmap128_set(&table->config.match, OF1X_MATCH_UDP_DST);
	bitmap128_set(&table->config.match, OF1X_MATCH_SCTP_SRC);
	bitmap128_set(&table->config.match, OF1X_MATCH_SCTP_DST);
	bitmap128_set(&table->config.match, OF1X_MATCH_ICMPV4_TYPE);
	bitmap128_set(&table->config.match, OF1X_MATCH_ICMPV4_CODE);
	bitmap128_set(&table->config.match, OF1X_MATCH_IPV6_SRC);
	bitmap128_set(&table->config.match, OF1X_MATCH_IPV6_DST);
	bitmap128_set(&table->config.match, OF1X_MATCH_IPV6_FLABEL);
	bitmap128_set(&table->config.match, OF1X_MATCH_IPV6_ND_TARGET);
	bitmap128_set(&table->config.match, OF1X_MATCH_IPV6_ND_SLL);
	bitmap128_set(&table->config.match, OF1X_MATCH_IPV6_ND_TLL);
	bitmap128_set(&table->config.match, OF1X_MATCH_IPV6_EXTHDR);
	bitmap128_set(&table->config.match, OF1X_MATCH_ICMPV6_CODE);
	bitmap128_set(&table->config.match, OF1X_MATCH_ICMPV6_TYPE);
	bitmap128_set(&table->config.match, OF1X_MATCH_MPLS_LABEL);
	bitmap128_set(&table->config.match, OF1X_MATCH_MPLS_TC);
	bitmap128_set(&table->config.match, OF1X_MATCH_MPLS_BOS);
	//bitmap128_set(&table->config.match, OF1X_MATCH_PBB_ISID);
	//bitmap128_set(&table->config.match, OF1X_MATCH_TUNNEL_ID);
	bitmap128_set(&table->config.match, OF1X_MATCH_PPPOE_CODE);
	bitmap128_set(&table->config.match, OF1X_MATCH_PPPOE_TYPE);
	bitmap128_set(&table->config.match, OF1X_MATCH_PPPOE_SID);
	bitmap128_set(&table->config.match, OF1X_MATCH_PPP_PROT);
	bitmap128_set(&table->config.match, OF1X_MATCH_GTP_MSG_TYPE);
	bitmap128_set(&table->config.match, OF1X_MATCH_GTP_TEID);

	//Wildcards
	bitmap128_clean(&table->config.wildcards);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_METADATA);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_ETH_DST);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_ETH_SRC);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_VLAN_VID);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_ARP_SHA);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_ARP_SPA);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_ARP_THA);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_ARP_TPA);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_IPV4_SRC);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_IPV4_DST);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_IPV6_SRC);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_IPV6_DST);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_IPV6_FLABEL);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_IPV6_EXTHDR);
	//bitmap128_set(&table->config.wildcards, OF1X_MATCH_PBB_ISID);
	//bitmap128_set(&table->config.wildcards, OF1X_MATCH_TUNNEL_ID);
	bitmap128_set(&table->config.wildcards, OF1X_MATCH_GTP_TEID);


	//Apply actions
	bitmap128_clean(&table->config.apply_actions);
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_COPY_TTL_IN
	bitmap128_set(&table->config.apply_actions, OF1X_AT_POP_VLAN);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_POP_MPLS);		
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_POP_GTP);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_POP_PPPOE);		
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_POP_PBB);		
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_PUSH_PBB);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_PUSH_PPPOE);		
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_PUSH_GTP);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_PUSH_MPLS);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_PUSH_VLAN);		
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_COPY_TTL_OUT);			
	bitmap128_set(&table->config.apply_actions, OF1X_AT_DEC_NW_TTL);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_DEC_MPLS_TTL);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_MPLS_TTL);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_NW_TTL);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_QUEUE);		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ETH_DST);  		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ETH_SRC);  		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ETH_TYPE); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_MPLS_LABEL);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_MPLS_TC);  	   	
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_MPLS_BOS);  	   	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_VLAN_VID); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_VLAN_PCP); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ARP_OPCODE);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ARP_SHA);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ARP_SPA);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ARP_THA);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ARP_TPA);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_NW_PROTO);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_NW_SRC);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_NW_DST);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IP_DSCP);  		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IP_ECN);   		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IP_PROTO); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IPV4_SRC); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IPV4_DST); 		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IPV6_SRC);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IPV6_DST);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IPV6_FLABEL);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IPV6_ND_TARGET);
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IPV6_ND_SLL);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IPV6_ND_TLL);	
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_IPV6_EXTHDR);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_TCP_SRC);  		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_TCP_DST);  		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_UDP_SRC);  		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_UDP_DST);  		
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_SCTP_SRC);  		
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_SCTP_DST);  		
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_TP_SRC);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_TP_DST);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ICMPV4_TYPE);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ICMPV4_CODE);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ICMPV6_TYPE);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_ICMPV6_CODE);	
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_PBB_ISID);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_TUNNEL_ID);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_PPPOE_CODE);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_PPPOE_TYPE);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_PPPOE_SID);	   	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_PPP_PROT); 	   	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_GTP_MSG_TYPE);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_SET_FIELD_GTP_TEID);	
	bitmap128_set(&table->config.apply_actions, OF1X_AT_GROUP);			
	//bitmap128_set(&table->config.apply_actions, OF1X_AT_EXPERIMENTER);
	bitmap128_set(&table->config.apply_actions, OF1X_AT_OUTPUT);
		
	//Write actions
	table->config.write_actions = table->config.apply_actions;

	//METADATA (full metadata support)
	table->config.metadata_match = 0xFFFFFFFFFFFFFFFFUL;  
	table->config.metadata_write = 0xFFFFFFFFFFFFFFFFUL; 

	//Instructions
	table->config.instructions = 	(1 << OF1X_IT_APPLY_ACTIONS) |
					(1 << OF1X_IT_CLEAR_ACTIONS) |
					(1 << OF1X_IT_WRITE_ACTIONS) |
					(1 << OF1X_IT_WRITE_METADATA) |
					(1 << OF1X_IT_GOTO_TABLE);
}

void __of13_set_table_defaults(of1x_flow_table_t* table){
	
	//Being lazy...
	__of12_set_table_defaults(table);

	//Setting the default behaviour to continue to the next table.
	table->default_action = OF1X_TABLE_MISS_CONTINUE; 
	
	//Adding OF1.3 only matches
	bitmap128_set(&table->config.match, OF1X_MATCH_MPLS_BOS); 

	//TODO: add METER instruction when implemented
	
	table->config.table_miss_config = 0x0;
}


/* Initalizer. Table struct has been allocated by pipeline initializer. */
rofl_result_t __of1x_init_table(struct of1x_pipeline* pipeline, of1x_flow_table_t* table, const unsigned int table_index, const enum of1x_matching_algorithm_available algorithm){

	//Safety checks
	if( unlikely(pipeline==NULL) || unlikely(table==NULL) )
		return ROFL_FAILURE;	

	//Initializing mutexes
	table->mutex = platform_mutex_init(NULL);
	if( unlikely(NULL==table->mutex) )
		return ROFL_FAILURE;
	table->rwlock = platform_rwlock_init(NULL);
	if( unlikely(NULL==table->rwlock) )
		return ROFL_FAILURE;
	
	table->pipeline = pipeline;
	table->number = table_index;
	table->entries = NULL;
	table->num_of_entries = 0;
	table->max_entries = OF1X_MAX_NUMBER_OF_TABLE_ENTRIES;

	//Set name
	snprintf(table->name, OF1X_MAX_TABLE_NAME_LEN, "table%u", table_index);
	
	//Setting up the matching algorithm	
	if(! (algorithm < of1x_matching_algorithm_count)){
		platform_mutex_destroy(table->mutex);
		platform_rwlock_destroy(table->rwlock);
		return ROFL_FAILURE;
	}

	//Set algorithm
	table->matching_algorithm = algorithm;

	//Auxiliary matching algorithm structs 
	table->matching_aux[0] = NULL; 
	table->matching_aux[1] = NULL;

	//Initializing timers. NOTE does that need to be done here or somewhere else?
#if OF1X_TIMER_STATIC_ALLOCATION_SLOTS
	__of1x_timer_group_static_init(table);
#else
	table->timers = NULL;
#endif

	switch(pipeline->sw->of_ver){
		case OF_VERSION_10:
			__of10_set_table_defaults(table);
			break; 
		case OF_VERSION_12:
			__of12_set_table_defaults(table);
			break; 
		case OF_VERSION_13:
			__of13_set_table_defaults(table);
			break; 
		default:
			platform_mutex_destroy(table->mutex);
			platform_rwlock_destroy(table->rwlock);
			return ROFL_FAILURE;
	}

	//Init stats
	__of1x_stats_table_init(table);

	//Allow matching algorithms to do stuff	
	if(of1x_matching_algorithms[table->matching_algorithm].init_hook)
		return of1x_matching_algorithms[table->matching_algorithm].init_hook(table);
	
	return ROFL_SUCCESS;
}

/* Destructor. Table object is freed by pipeline destructor */
rofl_result_t __of1x_destroy_table(of1x_flow_table_t* table){
	
	platform_mutex_lock(table->mutex);
	platform_rwlock_wrlock(table->rwlock);

	//Let the matching algorithm destroy its own state
	if(of1x_matching_algorithms[table->matching_algorithm].destroy_hook)
		of1x_matching_algorithms[table->matching_algorithm].destroy_hook(table);

	platform_mutex_destroy(table->mutex);
	platform_rwlock_destroy(table->rwlock);
	
	//Destroy stats
	__of1x_stats_table_destroy(table);

	//Do NOT free table, since it was allocated in a single buffer in pipeline.c	
	return ROFL_SUCCESS;
}


/* 
* Interfaces for generic add/remove flow entry 
* Specific matchings may point them to their own routines, but they MUST always call
* of1x_[whatever]_flow_entry_table_imp in order to update the main tables
*/
inline rofl_of1x_fm_result_t of1x_add_flow_entry_table(of1x_pipeline_t *const pipeline, const unsigned int table_id, of1x_flow_entry_t **const entry, bool check_overlap, bool reset_counts){

	rofl_of1x_fm_result_t result;
	of1x_flow_table_t* table;
	
	//Verify table_id
	if(table_id >= pipeline->num_of_tables)
		return ROFL_OF1X_FM_FAILURE;

	table = &pipeline->tables[table_id];

	//Take rd lock over the grouptable (avoid deletion of groups while flow entry insertion)
	platform_rwlock_rdlock(pipeline->groups->rwlock);

	//Verify entry
	if(__of1x_validate_flow_entry(*entry, pipeline, table_id) != ROFL_SUCCESS){
		//Release rdlock
		platform_rwlock_rdunlock(pipeline->groups->rwlock);
		return ROFL_OF1X_FM_FAILURE;
	}


	//Perform insertion (node that in 1.0 operation ADD must always reset counters on overlap)
	result = of1x_matching_algorithms[table->matching_algorithm].add_flow_entry_hook(table, *entry, check_overlap, reset_counts || ( pipeline->sw->of_ver == OF_VERSION_10 ));

	if(result != ROFL_OF1X_FM_SUCCESS){
		//Release rdlock
		platform_rwlock_rdunlock(pipeline->groups->rwlock);
		return result;
	}
	
	//Add timer
	__of1x_add_timer(table, *entry);

	//Release rdlock
	platform_rwlock_rdunlock(pipeline->groups->rwlock);

	//Was successful set the pointer to NULL
	//so that is not further used outside the pipeline
	*entry = NULL;

	//Return value
	return result;
}


inline rofl_result_t of1x_modify_flow_entry_table(of1x_pipeline_t *const pipeline, const unsigned int table_id, of1x_flow_entry_t **const entry, const enum of1x_flow_removal_strictness strict, bool reset_counts){
	rofl_result_t result;
	of1x_flow_table_t* table;
	
	//Verify table_id
	if(table_id >= pipeline->num_of_tables)
		return ROFL_FAILURE;

	table = &pipeline->tables[table_id];

	//Take rd lock over the grouptable (avoid deletion of groups while flow entry insertion)
	platform_rwlock_rdlock(pipeline->groups->rwlock);

	//Verify entry
	if(__of1x_validate_flow_entry(*entry, pipeline, table_id) != ROFL_SUCCESS){
		//Release rdlock
		platform_rwlock_rdunlock(pipeline->groups->rwlock);
		return ROFL_FAILURE;
	}

	//Perform insertion
	result = of1x_matching_algorithms[table->matching_algorithm].modify_flow_entry_hook(table, *entry, strict, reset_counts);

	if(result != ROFL_SUCCESS){
		//Release rdlock
		platform_rwlock_rdunlock(pipeline->groups->rwlock);
		return result;
	}
	
	//Release rdlock
	platform_rwlock_rdunlock(pipeline->groups->rwlock);

	//Was successful set the pointer to NULL
	//so that is not further used outside the pipeline
	*entry = NULL;

	//Return value
	return result;
}

inline rofl_result_t of1x_remove_flow_entry_table(of1x_pipeline_t *const pipeline, const unsigned int table_id, of1x_flow_entry_t* entry, const enum of1x_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group){
	
	of1x_flow_table_t* table;
	
	//Verify table_id
	if(table_id >= pipeline->num_of_tables)
		return ROFL_FAILURE;

	//Verify entry
	if(__of1x_validate_flow_entry(entry, pipeline, table_id) != ROFL_SUCCESS){
		return ROFL_FAILURE;
	}

	//Recover table pointer
	table = &pipeline->tables[table_id];
	
	return of1x_matching_algorithms[table->matching_algorithm].remove_flow_entry_hook(table, entry, NULL, strict,  out_port, out_group, OF1X_FLOW_REMOVE_DELETE, MUTEX_NOT_ACQUIRED);
}

//This API call should NOT be called from outside pipeline library
rofl_result_t __of1x_remove_specific_flow_entry_table(of1x_pipeline_t *const pipeline, const unsigned int table_id, of1x_flow_entry_t *const specific_entry, of1x_flow_remove_reason_t reason, of1x_mutex_acquisition_required_t mutex_acquired){
	of1x_flow_table_t* table;
	
	//Verify table_id
	if(table_id >= pipeline->num_of_tables)
		return ROFL_FAILURE;

	//Recover table pointer
	table = &pipeline->tables[table_id];
	
	return of1x_matching_algorithms[table->matching_algorithm].remove_flow_entry_hook(table, NULL, specific_entry, STRICT, OF1X_PORT_ANY, OF1X_GROUP_ANY, reason, mutex_acquired);
}

/* Dump methods */
void of1x_dump_table(of1x_flow_table_t* table, bool nbo){
	of1x_flow_entry_t* entry;
	int i;	

	ROFL_PIPELINE_INFO("\n"); //This is done in purpose 
	ROFL_PIPELINE_INFO("Dumping table # %u (%p). Default action: %u. # of entries: %d\n", table->number, table, table->default_action,table->num_of_entries);
	
	if(!table->entries){
		ROFL_PIPELINE_INFO("\t[*] No entries\n");
		return;	
	}
	for(entry=table->entries, i=0;entry!=NULL;entry=entry->next,i++){
		ROFL_PIPELINE_INFO("\t[%d] ",i);
		of1x_dump_flow_entry(entry, nbo);
	}
	
	ROFL_PIPELINE_INFO("\t[*] No more entries...\n");
	
	if(of1x_matching_algorithms[table->matching_algorithm].dump_hook){
		ROFL_PIPELINE_INFO("\tMatching algorithm %u specific state\n", table->matching_algorithm);
		of1x_matching_algorithms[table->matching_algorithm].dump_hook(table, nbo);
	}
}
