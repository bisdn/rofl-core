#ifndef __OF1X_L2HASH_MATCH_H__
#define __OF1X_L2HASH_MATCH_H__

#include "rofl.h"
#include "../matching_algorithms.h"
#include "../../of1x_flow_table.h"

//C++ extern C
ROFL_BEGIN_DECLS

//Bucket
typedef struct l2hash_ht_bucket{

	//Bucket fields
	uint64_t eth_dst;
	uint16_t vid;	
	
	//Flow entry pointer
	of1x_flow_entry_t* entry;
	
	//Double linked list
	struct l2hash_ht_bucket* prev;
	struct l2hash_ht_bucket* next;
}l2hash_ht_bucket_t;

//Hash table entry
typedef struct l2hash_ht_entry{
	unsigned int num_of_buckets;
	struct l2hash_ht_bucket* bucket_list;
}l2hash_ht_entry_t;

//Hash table

#define L2HASH_MAX_ENTRIES 0xFFFF

typedef struct l2hash_ht_table{
	unsigned int num_of_entries;
	l2hash_ht_entry_t table[L2HASH_MAX_ENTRIES];
}l2hash_ht_table_t;


//State
typedef struct l2hash_state{
	//Hash tables
	l2hash_ht_table_t vlan;
	l2hash_ht_table_t no_vlan;
}l2hash_state_t;

//Keys
typedef struct l2hash_novlan_key{
	uint64_t eth_dst; //+2 bytes padding
}l2hash_novlan_key_t;

typedef struct l2hash_vlan{
	uint64_t eth_dst; //+2 bytes padding
	uint16_t vid;
}l2hash_vlan_key_t;


//Matrix of T elements for Pearsons's algorithm
extern uint16_t l2hash_ht_T[L2HASH_MAX_ENTRIES];

//Key
static inline uint16_t l2hash_ht_hash16(const char* key, unsigned int size){
	
	unsigned int i;
	unsigned char c;
	uint16_t hash;	

	for(i=0; i<size; i++) {
		c = key[i];
		hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & c)];
	}
	
	return hash;	
}


//C++  xtern C
ROFL_END_DECLS

#endif //L2HASH_MATCH
