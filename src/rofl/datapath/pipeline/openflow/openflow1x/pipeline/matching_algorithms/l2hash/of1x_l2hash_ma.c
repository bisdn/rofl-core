#include "of1x_l2hash_ma.h"

#include <stdlib.h>
#include <assert.h>
#include "../../of1x_pipeline.h"
#include "../../of1x_flow_table.h"
#include "../../of1x_flow_entry.h"
#include "../../of1x_match.h"
#include "../../of1x_group_table.h"
#include "../../of1x_instruction.h"
#include "../../../of1x_async_events_hooks.h"
#include "../../../../../platform/lock.h"
#include "../../../../../platform/likely.h"
#include "../../../../../platform/memory.h"
#include "../matching_algorithms.h"
#include "../loop/of1x_loop_ma.h"

#define L2HASH_DESCRIPTION "The l2hash algorithm searches the list of entries by its priority order. On the worst case the performance is o(N) with the number of entries"


//
// Constructors and destructors
//
rofl_result_t of1x_init_l2hash(struct of1x_flow_table *const table){

	//Allocate memory for the hash table
	table->matching_aux[0] = (void*)platform_malloc_shared(sizeof(l2hash_state_t));
	//Cleanup everything
	memset(table->matching_aux[0], 0, sizeof(l2hash_state_t));	

	//Matches and wildcards support
	bitmap128_clean(&table->config.match);
	bitmap128_set(&table->config.match, OF1X_MATCH_ETH_DST);
	bitmap128_set(&table->config.match, OF1X_MATCH_VLAN_VID);
	
	bitmap128_clean(&table->config.wildcards);
	
	return ROFL_SUCCESS; 
}


static void l2hash_destroy_ht(l2hash_ht_table_t* ht){
	unsigned int i;	
	l2hash_ht_bucket_t *bucket, *next_bucket;

	if(ht->num_of_entries)
		return;

	//Loop over tables and de
	for(i=0;i<L2HASH_MAX_ENTRIES;i++){
		if(ht->table[i].num_of_buckets){
			bucket = ht->table[i].bucket_list;
			while(bucket){
				next_bucket = bucket->next;
				platform_free_shared(bucket);
				bucket = next_bucket;
			}	
		}
	}
}

//Add&Remove routine
void l2hash_ht_add_bucket(l2hash_ht_table_t* ht, uint16_t hash, l2hash_ht_bucket_t* bucket){
	l2hash_ht_entry_t* ht_e = &ht->table[hash];
	l2hash_ht_bucket_t* it;

	//Insertion priority
	uint32_t priority = bucket->entry->priority;
 
	//Assign HT entry to the buck
	bucket->ht_entry = ht_e;

	if(ht_e->bucket_list){
		it = ht_e->bucket_list;
		//Find the postion
		while(it){
			if(it->entry->priority <= priority)
				break;
			it = it->next;
		}

		//Assign first the next and previous on our node
		bucket->next = it;
		bucket->prev = it->prev;	
		it->prev = bucket;

		//Add before it
		if(it->prev)
			it->prev->next = bucket;
		else
			ht_e->bucket_list = bucket;
	}else{
		//Put in the head and continue
		ht_e->bucket_list = bucket;
		bucket->next = bucket->prev = NULL;
	}
	
	ht_e->num_of_buckets++;
}

void l2hash_ht_remove_bucket(l2hash_ht_bucket_t* bucket){
	
	//Recover the entry from the bucket
	l2hash_ht_entry_t* ht_e = bucket->ht_entry; 

	//Adjust prev and next pointers
	if(bucket->next)
		bucket->next->prev = bucket->prev;

	if(bucket->prev){
		bucket->prev->next = bucket->next;
	}else{
		ht_e->bucket_list = bucket->next; 
	}
	
	ht_e->num_of_buckets--;
}


//
// L2 hash table state
//
rofl_result_t of1x_destroy_l2hash(struct of1x_flow_table *const table){

	l2hash_destroy_ht(&((struct l2hash_state*)table->matching_aux[0])->vlan);
	l2hash_destroy_ht(&((struct l2hash_state*)table->matching_aux[0])->no_vlan);
	platform_free_shared(table->matching_aux[0]);
	
	return ROFL_FAILURE; 
}

//
//Hooks
//
void of1x_add_hook_l2hash(of1x_flow_entry_t *const entry){

	uint16_t hash;
	of1x_match_t *vlan=NULL, *eth_dst=NULL, *match;	
	l2hash_entry_ps_t* ps;
	l2hash_ht_bucket_t* bucket;
	l2hash_state_t* state = (l2hash_state_t*)entry->table->matching_aux[0];
		
	for(match = entry->matches.head;match;){
		of1x_match_t *next = match->next;
		
		if(match->type == OF1X_MATCH_VLAN_VID){
			vlan = match;
		}else if(match->type == OF1X_MATCH_ETH_DST){
			eth_dst=match;
		}	
		match = next;
	}

	if(unlikely(eth_dst == NULL)){
		assert(0);
		return;//ROFL_FAILURE;
	}

	//allocate flow entry additional state
	ps = (l2hash_entry_ps_t*)platform_malloc_shared(sizeof(l2hash_entry_ps_t));
	bucket = (l2hash_ht_bucket_t*)platform_malloc_shared(sizeof(l2hash_ht_bucket_t));

	//Check for allocations
	if(unlikely(ps == NULL) || unlikely(bucket == NULL)){
		assert(0);
		return;// ROFL_FAILURE;
	}

	//That could not happen
	assert( bitmap128_is_bit_set(&entry->matches.match_bm, OF1X_MATCH_VLAN_VID) == (vlan != NULL) );


	//Assign common stuff of the bucket
	bucket->entry = entry;

	if(vlan){
		//VLAN
		l2hash_vlan_key_t key;
		key.vid = vlan->__tern->value.u16 & vlan->__tern->mask.u16;	
		key.eth_dst = eth_dst->__tern->value.u64 & eth_dst->__tern->mask.u64;
		//calculate hash	
		hash = l2hash_ht_hash96((const char*)&key, sizeof(l2hash_vlan_key_t)); 		
		//Fill in ps
		ps->has_vlan = false;

		//Fill-in bucket
		bucket->eth_dst = key.eth_dst;	
		bucket->vid = key.vid;	
	
		//Add to the bucket list
		l2hash_ht_add_bucket(&state->vlan, hash, bucket);

		//Increase the number of entries
		state->vlan.num_of_entries++;
	}else{
		//NO-VLAN
		l2hash_novlan_key_t key;
		key.eth_dst = eth_dst->__tern->value.u64 & eth_dst->__tern->mask.u64;
		//calculate hash	
		hash = l2hash_ht_hash64((const char*)&key, sizeof(l2hash_novlan_key_t));

		//Fill in ps
		ps->has_vlan = false;
	
		//Fill-in bucket
		bucket->eth_dst = key.eth_dst;	
	
		//Add to the bucket list
		l2hash_ht_add_bucket(&state->no_vlan, hash, bucket);
		
		//Increase the number of entries
		state->no_vlan.num_of_entries++;
	}
	
	//Store ps to entry	
	ps->bucket = bucket;	
	entry->platform_state = (void*)ps;
}
void of1x_modify_hook_l2hash(of1x_flow_entry_t *const entry){
	//We don't care
}
void of1x_remove_hook_l2hash(of1x_flow_entry_t *const entry){
	
	l2hash_state_t* state = (l2hash_state_t*)entry->table->matching_aux[0];
	
	if(unlikely(entry->platform_state == NULL)){
		assert(0);
		return;
	}
	
	l2hash_entry_ps_t* ps = (l2hash_entry_ps_t*)entry->platform_state;

	//Perform the remove
	l2hash_ht_remove_bucket(ps->bucket);

	if(ps->has_vlan){
		state->vlan.num_of_entries--;
	}else{
		state->no_vlan.num_of_entries--;
	}

	platform_free_shared(entry->platform_state);
	entry->platform_state = NULL;
}

//
// Main routines
// 


/* Conveniently wraps call with mutex.  */
rofl_of1x_fm_result_t of1x_add_flow_entry_l2hash(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, bool check_overlap, bool reset_counts){
	//Call loop with the right hooks
	return __of1x_add_flow_entry_loop(table, entry, check_overlap, reset_counts, of1x_add_hook_l2hash);
}

rofl_result_t of1x_modify_flow_entry_l2hash(of1x_flow_table_t *const table, of1x_flow_entry_t *const entry, const enum of1x_flow_removal_strictness strict, bool reset_counts){
	//Call loop with the right hooks
	return __of1x_modify_flow_entry_loop(table, entry, strict, reset_counts, of1x_add_hook_l2hash, of1x_modify_hook_l2hash);
}

rofl_result_t of1x_remove_flow_entry_l2hash(of1x_flow_table_t *const table , of1x_flow_entry_t *const entry, of1x_flow_entry_t *const specific_entry, const enum of1x_flow_removal_strictness strict, uint32_t out_port, uint32_t out_group, of1x_flow_remove_reason_t reason, of1x_mutex_acquisition_required_t mutex_acquired){
	//Call loop with the right hooks
	return __of1x_remove_flow_entry_loop(table, entry, specific_entry, strict, out_port, out_group, reason, mutex_acquired, of1x_remove_hook_l2hash);
}

//Define the matching algorithm struct
OF1X_REGISTER_MATCHING_ALGORITHM(l2hash) = {
	//Init and destroy hooks
	.init_hook = of1x_init_l2hash,
	.destroy_hook = of1x_destroy_l2hash,

	//Flow mods
	.add_flow_entry_hook = of1x_add_flow_entry_l2hash,
	.modify_flow_entry_hook = of1x_modify_flow_entry_l2hash,
	.remove_flow_entry_hook = of1x_remove_flow_entry_l2hash,

	//Stats
	.get_flow_stats_hook = of1x_get_flow_stats_loop,
	.get_flow_aggregate_stats_hook = of1x_get_flow_aggregate_stats_loop,

	//Find group related entries	
	.find_entry_using_group_hook = of1x_find_entry_using_group_loop,

	//Dumping	
	.dump_hook = NULL,
	.description = L2HASH_DESCRIPTION,
};

//Hash T
uint16_t l2hash_ht_T[L2HASH_MAX_ENTRIES] = {
      // 256 values 0-255 in any (random) order suffices
       98,  6, 85,150, 36, 23,112,164,135,207,169,  5, 26, 64,165,219, //  1

       61, 20, 68, 89,130, 63, 52,102, 24,229,132,245, 80,216,195,115, //  2

       90,168,156,203,177,120,  2,190,188,  7,100,185,174,243,162, 10, //  3

      237, 18,253,225,  8,208,172,244,255,126,101, 79,145,235,228,121, //  4

      123,251, 67,250,161,  0,107, 97,241,111,181, 82,249, 33, 69, 55, //  5

       59,153, 29,  9,213,167, 84, 93, 30, 46, 94, 75,151,114, 73,222, //  6

      197, 96,210, 45, 16,227,248,202, 51,152,252,125, 81,206,215,186, //  7

       39,158,178,187,131,136,  1, 49, 50, 17,141, 91, 47,129, 60, 99, //  8

      154, 35, 86,171,105, 34, 38,200,147, 58, 77,118,173,246, 76,254, //  9

      133,232,196,144,198,124, 53,  4,108, 74,223,234,134,230,157,139, // 10

      189,205,199,128,176, 19,211,236,127,192,231, 70,233, 88,146, 44, // 11

      183,201, 22, 83, 13,214,116,109,159, 32, 95,226,140,220, 57, 12, // 12

      221, 31,209,182,143, 92,149,184,148, 62,113, 65, 37, 27,106,166, // 13

        3, 14,204, 72, 21, 41, 56, 66, 28,193, 40,217, 25, 54,179,117, // 14

      238, 87,240,155,180,170,242,212,191,163, 78,218,137,194,175,110, // 15

       43,119,224, 71,122,142, 42,160,104, 48,247,103, 15, 11,138,239  // 16

	//TODO: the rest are 0
};
