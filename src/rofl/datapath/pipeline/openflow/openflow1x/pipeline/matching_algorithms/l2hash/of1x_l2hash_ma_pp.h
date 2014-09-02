#ifndef __OF1X_L2HASH_MATCH_PP_H__
#define __OF1X_L2HASH_MATCH_PP_H__

#include "rofl.h"
#include "../../of1x_pipeline.h"
#include "../../of1x_flow_table.h"
#include "../../of1x_flow_entry.h"
#include "../../of1x_match_pp.h"
#include "../../of1x_group_table.h"
#include "../../of1x_instruction_pp.h"
#include "../../../of1x_async_events_hooks.h"
#include "../../../../../platform/lock.h"
#include "../../../../../platform/likely.h"
#include "../../../../../platform/memory.h"
#include "of1x_l2hash_ma.h"

//C++ extern C
ROFL_BEGIN_DECLS

static inline void l2_hash_check_all_buckets_no_vlan(l2hash_ht_bucket_t* bucket, l2hash_novlan_key_t* key, of1x_flow_entry_t** best_match){

	while(bucket){
		if(key->eth_dst == bucket->eth_dst){
			*best_match = bucket->entry;
			return;
		}
		bucket = bucket->next;
	}
}

static inline void l2_hash_check_all_buckets_vlan(l2hash_ht_bucket_t* bucket, l2hash_vlan_key_t* key, of1x_flow_entry_t** best_match){

	while(bucket){
		if(key->eth_dst == bucket->eth_dst && key->vid == bucket->vid){
			*best_match = bucket->entry;
			return;
		}
		bucket = bucket->next;
	}
}

/* FLOW entry lookup entry point */ 
static inline of1x_flow_entry_t* of1x_find_best_match_l2hash_ma(of1x_flow_table_t *const table, datapacket_t *const pkt){

	l2hash_novlan_key_t key_novlan;
	l2hash_vlan_key_t key_vlan;
	of1x_flow_entry_t *best_match = NULL, *tmp=NULL;
	l2hash_ht_entry_t* ht_entry;

	//Table hash table 
	l2hash_state_t* state = (l2hash_state_t*)table->matching_aux[0];
	
	//Recover keys	
	key_novlan.eth_dst = key_vlan.eth_dst = *platform_packet_get_eth_dst(pkt) & OF1X_6_BYTE_MASK;
	key_vlan.vid = *platform_packet_get_vlan_vid(pkt)&OF1X_VLAN_ID_MASK;

	//Check no-VLAN table-hash
	if(state->no_vlan.num_of_entries > 0){
		ht_entry = &state->no_vlan.table[l2hash_ht_hash16((const char*)&key_novlan, sizeof(key_novlan))];	
		//Check buckets
		l2_hash_check_all_buckets_no_vlan(ht_entry->bucket_list, &key_novlan, &best_match);
	}	
	
	//Check VLAN table-hash
	if(state->vlan.num_of_entries > 0){
		ht_entry = &state->vlan.table[l2hash_ht_hash16((const char*)&key_vlan, sizeof(key_vlan))];	
		if(best_match){
			l2_hash_check_all_buckets_vlan(ht_entry->bucket_list, &key_vlan, &tmp);
			if(tmp && (tmp->priority > best_match->priority))
				best_match = tmp;
		}else{
			l2_hash_check_all_buckets_vlan(ht_entry->bucket_list, &key_vlan, &best_match);
		}	
	}
	return best_match; 
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_L2HASH_MATCH_PP
