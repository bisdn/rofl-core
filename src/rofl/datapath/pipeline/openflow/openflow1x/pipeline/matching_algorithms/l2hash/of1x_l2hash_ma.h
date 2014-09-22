#ifndef __OF1X_L2HASH_MATCH_H__
#define __OF1X_L2HASH_MATCH_H__

#include "rofl.h"
#include "../matching_algorithms.h"
#include "../../of1x_flow_table.h"

//C++ extern C
ROFL_BEGIN_DECLS

//fwd decl
struct l2hash_ht_entry;

//Bucket
typedef struct l2hash_ht_bucket{

	//Bucket fields
	uint64_t eth_dst;
	uint16_t vid;	
	
	//Flow entry pointer
	of1x_flow_entry_t* entry;

	//Pointer back to the entry
	struct l2hash_ht_entry* ht_entry;
	
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



#if 0
//Matrix of T elements for Pearsons's algorithm
extern uint16_t l2hash_ht_T[L2HASH_MAX_ENTRIES];

//Hashing functions
static inline uint16_t l2hash_ht_hash64(const char* key, unsigned int size){
	
	uint16_t hash=0x0;	
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[0])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[1])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[2])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[3])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[4])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[5])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[6])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[7])];
	return hash;	
}

//Hashing functions
static inline uint16_t l2hash_ht_hash96(const char* key, unsigned int size){
	
	uint16_t hash=0x0;	
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[0])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[1])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[2])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[3])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[4])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[5])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[6])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[7])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[8])];
	hash = l2hash_ht_T[hash ^ (L2HASH_MAX_ENTRIES & key[9])];
	return hash;	
}
#else

/**
* FNV hash, based on: http://isthe.com/chongo/tech/comp/fnv/
*/

#define L2_HASH_FNV_MASK_16 0xFFFF
#define L2_HASH_FNV_PRIME 0x01000193 //16777619
#define L2_HASH_FNV_SEED 0x811C9DC5 //2166136261


static inline uint32_t l2hash_fnv1a(unsigned char c, uint32_t hash){
	return (c ^ hash) * L2_HASH_FNV_PRIME;
}
/// hash a single byte
static inline uint16_t l2hash_ht_hash64(const char* key, unsigned int size){

	uint32_t hash = L2_HASH_FNV_SEED;
	
	hash = l2hash_fnv1a( key[0], hash); 
	hash = l2hash_fnv1a( key[1], hash); 
	hash = l2hash_fnv1a( key[2], hash); 
	hash = l2hash_fnv1a( key[3], hash); 
	hash = l2hash_fnv1a( key[4], hash); 
	hash = l2hash_fnv1a( key[5], hash); 
	hash = l2hash_fnv1a( key[6], hash); 
	hash = l2hash_fnv1a( key[7], hash); 

	//Fold
	return  (hash>>16) ^ (hash & L2_HASH_FNV_MASK_16);
}

//96 bit
static inline uint16_t l2hash_ht_hash96(const char* key, unsigned int size){
	uint32_t hash = L2_HASH_FNV_SEED;
	
	hash = l2hash_fnv1a( key[0], hash); 
	hash = l2hash_fnv1a( key[1], hash); 
	hash = l2hash_fnv1a( key[2], hash); 
	hash = l2hash_fnv1a( key[3], hash); 
	hash = l2hash_fnv1a( key[4], hash); 
	hash = l2hash_fnv1a( key[5], hash); 
	hash = l2hash_fnv1a( key[6], hash); 
	hash = l2hash_fnv1a( key[7], hash); 
	hash = l2hash_fnv1a( key[8], hash); 
	hash = l2hash_fnv1a( key[9], hash); 

	//Fold
	return (hash>>16) ^ (hash & L2_HASH_FNV_MASK_16);
}

#endif
//Platform state
typedef struct l2hash_entry_ps{
	bool has_vlan;
	l2hash_ht_bucket_t* bucket;	
}l2hash_entry_ps_t;

//C++  xtern C
ROFL_END_DECLS

#endif //L2HASH_MATCH
