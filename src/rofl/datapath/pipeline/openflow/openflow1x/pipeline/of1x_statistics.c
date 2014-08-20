#include "of1x_statistics.h"

#include <assert.h> 
#include "of1x_pipeline.h"
#include "of1x_flow_table.h"
#include "of1x_flow_entry.h"
#include "of1x_instruction.h"
#include "of1x_timers.h"
#include "of1x_group_table.h"
#include "../../../platform/memory.h"
#include "../../../platform/likely.h"
#include "../../../platform/timing.h"
#include "../../../platform/atomic_operations.h"
#include "../../../util/time.h"

/**
 * Here the functions that initialize and update
 * the statistics are defined
 * 
 * Need to see in which are the input parameters that
 * make it easyer and logic
 */

//Flow Statistics functions
/**
 * of1x_stats_flow_init
 * stores the time when the flow entry was created
 */
void __of1x_init_flow_stats(of1x_flow_entry_t * entry)
{
	struct timeval now;
	platform_gettimeofday(&now);
	
	entry->stats.initial_time = now;
	entry->stats.packet_count = 0;
	entry->stats.byte_count = 0;

	entry->stats.mutex = platform_mutex_init(NULL);

	return;
}

/**
 * of1x_stats_flow_destroy
 * basically destroys the mutex
 */
void __of1x_destroy_flow_stats(of1x_flow_entry_t* entry)
{
	platform_mutex_destroy(entry->stats.mutex);
}


/*
* Msg aggregate flow stats
*/
of1x_stats_flow_aggregate_msg_t* __of1x_init_stats_flow_aggregate_msg(){

	of1x_stats_flow_aggregate_msg_t* msg = (of1x_stats_flow_aggregate_msg_t*)platform_malloc_shared(sizeof(of1x_stats_flow_aggregate_msg_t));

	//Init counters
	if(likely(msg!=NULL))
		platform_memset(msg,0,sizeof(*msg));

	return msg;
}
void of1x_destroy_stats_flow_aggregate_msg(of1x_stats_flow_aggregate_msg_t* msg){

	if(likely(msg!=NULL))
		platform_free_shared(msg);
}

/*
* Msg flow stats
*/
of1x_stats_single_flow_msg_t* __of1x_init_stats_single_flow_msg(of1x_flow_entry_t* entry){

	of1x_stats_single_flow_msg_t* msg;

	if(!entry)
		return NULL;

	msg = (of1x_stats_single_flow_msg_t*)platform_malloc_shared(sizeof(of1x_stats_single_flow_msg_t)); 

	if(unlikely(msg==NULL))
		return NULL;
	
	msg->inst_grp = (of1x_instruction_group_t*)platform_malloc_shared(sizeof(of1x_instruction_group_t)); 
	
	if(unlikely(msg->inst_grp==NULL)){
		platform_free_shared(msg);
		return NULL;
	}

	//Fill static values
	if(entry->table)
		msg->table_id = entry->table->number;
	msg->priority = entry->priority;
	msg->cookie = entry->cookie;
	msg->idle_timeout = entry->timer_info.idle_timeout;
	msg->hard_timeout = entry->timer_info.hard_timeout;
	msg->flags = entry->flags;
	msg->byte_count = entry->stats.byte_count;
	msg->packet_count = entry->stats.packet_count;

	//Get durations
	of1x_stats_flow_get_duration(entry, &msg->duration_sec, &msg->duration_nsec);

	//Copy matches
	//TODO: deprecate this in favour of group_matches
	msg->matches = __of1x_copy_matches(entry->matches.head);
	
	//Copy instructions
	__of1x_copy_instruction_group(&entry->inst_grp,msg->inst_grp);

	return msg;
}
void __of1x_destroy_stats_single_flow_msg(of1x_stats_single_flow_msg_t* msg){

	of1x_match_t* match;

	if(unlikely(msg==NULL))
		return;

	//TODO: deprecate this in favour of group_matches
	match = msg->matches;
	while(match){
		of1x_match_t *next = match->next;
		of1x_destroy_match(match);
		match = next;
	}

	//Destroy instructions
	__of1x_destroy_instruction_group(msg->inst_grp);
	
	
	platform_free_shared(msg->inst_grp);
	platform_free_shared(msg);
}

of1x_stats_flow_msg_t* __of1x_init_stats_flow_msg(){

	of1x_stats_flow_msg_t* msg = (of1x_stats_flow_msg_t*)platform_malloc_shared(sizeof(of1x_stats_flow_msg_t)); 

	//Init counters
	if(likely(msg!=NULL))
		platform_memset(msg,0,sizeof(*msg));

	return msg;
}
void of1x_destroy_stats_flow_msg(of1x_stats_flow_msg_t* msg){
	
	of1x_stats_single_flow_msg_t* item, *next_item; 
	
	for(item=msg->flows_head; item; item = next_item){
		next_item = item->next;
		__of1x_destroy_stats_single_flow_msg(item);
	}
	
	//If there are single flow messages delete them	

	if(msg)
		platform_free_shared(msg);
}

//Push to stats_flow_msg
void __of1x_push_single_flow_stats_to_msg(of1x_stats_flow_msg_t* msg, of1x_stats_single_flow_msg_t* sfs){

	if(unlikely(msg==NULL))
		return;

	if(!msg->flows_head)
		msg->flows_head = sfs;
	else
		msg->flows_tail->next = sfs;
	
	msg->flows_tail = sfs;
	sfs->next = NULL;
	msg->num_of_entries++;
}

/**
 * of1x_stats_flow_reset_counts
 */
void __of1x_stats_flow_reset_counts(of1x_flow_entry_t * entry){

	platform_mutex_lock(entry->stats.mutex);
	entry->stats.packet_count = entry->stats.byte_count =  0;
	platform_mutex_unlock(entry->stats.mutex);
}

/**
 * of1x_stats_flow_get_duration()
 */
void of1x_stats_flow_get_duration(struct of1x_flow_entry * entry, uint32_t* sec, uint32_t* nsec){

	struct timeval now, diff;

	platform_gettimeofday(&now);
	
	TIMERSUB(&now, &entry->stats.initial_time, &diff);
	*sec = diff.tv_sec;
	
	*nsec = ( (diff.tv_usec*1000)&0xFFFFFFFF00000000ULL )>>32;
}

//Table Statistics functions
/**
 * Initializes table statistics state
 */
void __of1x_stats_table_init(of1x_flow_table_t * table){

	table->stats.lookup_count = 0;
	table->stats.matched_count = 0;

	//Stats mutex	
	table->stats.mutex = platform_mutex_init(NULL);
}

/**
 * Destroys table statistics state
 */
void __of1x_stats_table_destroy(of1x_flow_table_t * table){

	platform_mutex_destroy(table->stats.mutex);
}
//NOTE this functions add too much overhead!


void __of1x_init_group_stats(of1x_stats_group_t *group_stats){
	//NOTE bucket stats are initialized when the group is created, before being attached to the list
	group_stats->mutex = platform_mutex_init(NULL);
	group_stats->byte_count = 0;
	group_stats->packet_count = 0;
	group_stats->ref_count = 0;
}

void __of1x_destroy_group_stats(of1x_stats_group_t* group_stats){
	platform_mutex_destroy(group_stats->mutex);
}

void __of1x_stats_group_update(of1x_stats_group_t *gr_stats, uint64_t bytes){
	platform_atomic_inc64(&gr_stats->packet_count, gr_stats->mutex);
	platform_atomic_add64(&gr_stats->byte_count, bytes, gr_stats->mutex);
}

void __of1x_stats_group_inc_reference(of1x_stats_group_t *gr_stats){
	platform_atomic_inc32(&gr_stats->ref_count, gr_stats->mutex);
}

void __of1x_stats_group_dec_reference(of1x_stats_group_t *gr_stats){
	platform_atomic_dec32(&gr_stats->ref_count, gr_stats->mutex);
}

of1x_stats_group_msg_t* __of1x_init_stats_group_msg(unsigned int num_buckets){
	
	of1x_stats_group_msg_t *msg = (of1x_stats_group_msg_t *) platform_malloc_shared(sizeof(of1x_stats_group_msg_t));
	if(likely(msg!=NULL)){
		msg->bucket_stats = (of1x_stats_bucket_counter_t*) platform_malloc_shared(sizeof(of1x_stats_bucket_counter_t) * num_buckets);
		if(msg->bucket_stats)
			return msg;
		else
		{
			platform_free_shared(msg);
			return NULL;
		}
	}
	else
		return NULL;
}

void of1x_destroy_stats_group_msg(of1x_stats_group_msg_t* msg){
	of1x_stats_group_msg_t* next, *it;
	
	for(it=msg;it;it=next){
		next=it->next;
		platform_free_shared(it->bucket_stats);
		platform_free_shared(it);
	}
}

of1x_stats_group_msg_t* __of1x_get_group_single_stats(of1x_group_t* group){
	of1x_bucket_t *bu_it;
	of1x_stats_group_msg_t* msg = __of1x_init_stats_group_msg(group->bc_list->num_of_buckets);

	msg->group_id = group->id;
	msg->ref_count = group->stats.ref_count;
	msg->packet_count = group->stats.packet_count;
	msg->byte_count = group->stats.packet_count;
	msg->num_of_buckets = group->bc_list->num_of_buckets;
	msg->next = NULL;
	
	//collect statistics from buckets
	int i=0;
	for(bu_it=group->bc_list->head;bu_it;bu_it=bu_it->next,i++){
		msg->bucket_stats[i].byte_count = bu_it->stats.byte_count;
		msg->bucket_stats[i].packet_count = bu_it->stats.packet_count;
	}
	return msg;
}

of1x_stats_group_msg_t* of1x_get_group_stats(of1x_pipeline_t* pipeline,uint32_t id){
	of1x_group_t* group=NULL;
	of1x_stats_group_msg_t *msg=NULL, *last=NULL, *head=NULL;
	
	if( id == OF1X_GROUP_ALL ){
	
		for(group=pipeline->groups->head;group;group=group->next){
			if(group == NULL)
				return NULL;
			
			msg = __of1x_get_group_single_stats(group);
			if(last)
				last->next = msg;
			if(!head)
				head=msg;
			
			last = msg;
		}
	} else{
		//find the group
		group = __of1x_group_search(pipeline->groups, id);
		if(group == NULL)
			return NULL;
		
		head = __of1x_get_group_single_stats(group);
	}

	return head;
}

void __of1x_destroy_bucket_desc_stats(of1x_stats_bucket_desc_msg_t* bucket){
	of1x_destroy_action_group(bucket->actions);
	platform_free_shared(bucket);
}

void of1x_destroy_group_desc_stats(of1x_stats_group_desc_msg_t *msg){
	of1x_stats_group_desc_msg_t *group_it, *next_gr;
	of1x_stats_bucket_desc_msg_t* bucket_it, *next_bu;
	
	for(group_it=msg; group_it; group_it=next_gr){
		next_gr = group_it->next;
		
		//destroy buckets
		for(bucket_it = msg->bucket; bucket_it; bucket_it=next_bu){
			next_bu = bucket_it->next;
			__of1x_destroy_bucket_desc_stats(bucket_it);
		}
		
		//destroy message
		platform_free_shared(msg);
	}
}

of1x_stats_bucket_desc_msg_t *__of1x_init_bucket_desc_stats(of1x_bucket_t *bucket){
	of1x_stats_bucket_desc_msg_t *desc_bucket;
	
	desc_bucket = platform_malloc_shared( sizeof(of1x_stats_bucket_desc_msg_t) );
	if ( desc_bucket == NULL ){
		return NULL;
	}
	desc_bucket->group = bucket->group;
	desc_bucket->weight = bucket->weight;
	desc_bucket->port = bucket->port;
	//copy actions
	desc_bucket->actions = __of1x_copy_action_group(bucket->actions);
	desc_bucket->next = NULL;
	
	return desc_bucket;
}

of1x_stats_group_desc_msg_t *__of1x_init_group_desc_single_stats(of1x_group_t* group){
	of1x_bucket_t *bu_it = NULL;
	of1x_stats_bucket_desc_msg_t *bu_desc=NULL, *head=NULL, *last=NULL;
	of1x_stats_group_desc_msg_t *msg = platform_malloc_shared( sizeof(of1x_stats_group_desc_msg_t) );
	
	if ( msg==NULL )
		return NULL;
	
	msg->group_id = group->id;
	msg->type = group->type;
	msg->next = NULL;
	
	for( bu_it = group->bc_list->head; bu_it; bu_it=bu_it->next){
		bu_desc = __of1x_init_bucket_desc_stats(bu_it);
		if(bu_desc == NULL){
			of1x_destroy_group_desc_stats(msg);
			return NULL;
		}
		
		if(last)
			last->next = bu_desc;
		if(!head)
			head=bu_desc;
		
		last = bu_desc;
	}
	
	msg->bucket = head;
	
	return msg;
}

of1x_stats_group_desc_msg_t *of1x_get_group_desc_stats(of1x_pipeline_t* pipeline){
	of1x_group_t* group;
	of1x_stats_group_desc_msg_t *msg=NULL, *last=NULL, *head=NULL;
	
	for(group=pipeline->groups->head;group;group=group->next){
		msg = __of1x_init_group_desc_single_stats(group);
		
		if (msg == NULL){
			if(head) of1x_destroy_group_desc_stats(head);
			return NULL;
		}
		
		if(last)
			last->next = msg;
		if(!head)
			head=msg;
		
		last = msg;
		}
	return head;
}

void __of1x_init_bucket_stats(of1x_stats_bucket_counter_t *bc_stats){
	bc_stats->mutex = platform_mutex_init(NULL);
	bc_stats->byte_count = 0;
	bc_stats->packet_count = 0;
}

void __of1x_destroy_buckets_stats(of1x_stats_bucket_counter_t *bc_stats){
	platform_mutex_destroy(bc_stats->mutex);
}

void __of1x_stats_bucket_update(of1x_stats_bucket_counter_t* bc_stats, uint64_t bytes){
	platform_atomic_inc64(&bc_stats->packet_count, bc_stats->mutex);
	platform_atomic_add64(&bc_stats->byte_count, bytes, bc_stats->mutex);
}

/*
* External interfaces
*/

of1x_stats_flow_msg_t* of1x_get_flow_stats(struct of1x_pipeline* pipeline, uint8_t table_id, uint32_t cookie, uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, struct of1x_match_group *const matches){

	uint32_t i,tid_start, tid_end;	
	of1x_stats_flow_msg_t* msg;

	//Verify table_id
	if(table_id >= pipeline->num_of_tables && table_id != OF1X_FLOW_TABLE_ALL)
		return NULL;

	//Create the message 
	msg = __of1x_init_stats_flow_msg();
	if(unlikely(msg==NULL))
		return NULL;

	//Set the tables to go through
	if(table_id == OF1X_FLOW_TABLE_ALL){
		tid_start = 0;
		tid_end = pipeline->num_of_tables;
	}else{
		tid_start = table_id;
		tid_end = table_id+1; 
	}

	for(i=tid_start;i<tid_end;i++){
		if(of1x_matching_algorithms[pipeline->tables[i].matching_algorithm].get_flow_stats_hook(&pipeline->tables[i], cookie, cookie_mask, out_port, out_group, matches, msg) != ROFL_SUCCESS){
			of1x_destroy_stats_flow_msg(msg);
			return NULL;
		} 
	}
	
	return msg;
}
of1x_stats_flow_aggregate_msg_t* of1x_get_flow_aggregate_stats(struct of1x_pipeline* pipeline, uint8_t table_id, uint32_t cookie, uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, struct of1x_match_group *const matches){
	
	uint32_t i, tid_start, tid_end;	
	of1x_stats_flow_aggregate_msg_t* msg;

	//Verify table_id
	if(table_id >= pipeline->num_of_tables && table_id != OF1X_FLOW_TABLE_ALL)
		return NULL;

	//Create the message 
	msg = __of1x_init_stats_flow_aggregate_msg();

	if(unlikely(msg==NULL))
		return NULL;

	//Set the tables to go through
	if(table_id == OF1X_FLOW_TABLE_ALL){
		tid_start = 0;
		tid_end = pipeline->num_of_tables;
	}else{
		tid_start = table_id;
		tid_end = table_id+1; 
	}

	for(i=tid_start;i<tid_end;i++){
		if(of1x_matching_algorithms[pipeline->tables[i].matching_algorithm].get_flow_aggregate_stats_hook(&pipeline->tables[i], cookie, cookie_mask, out_port, out_group, matches, msg) != ROFL_SUCCESS){
			of1x_destroy_stats_flow_aggregate_msg(msg);
			return NULL;
		} 
	}
	
	return msg;	
}

