#ifndef __OF12_GROUP_TABLE_H__
#define __OF12_GROUP_TABLE_H__

#include "rofl.h"
#include "of12_statistics.h"
#include "of12_action.h"
#include "of12_flow_entry.h"
#include <rofl/pipeline/platform/lock.h>

#define OF12_GROUP_MAX 0xffffff00
#define OF12_GROUP_ALL 0xfffffffc  /* Represents all groups for group delete commands. */
#define OF12_GROUP_ANY 0xffffffff /* Wildcard group used only for flow stats */

typedef struct of12_bucket{
	uint16_t weight;
	uint32_t port;
	uint32_t group;
	of12_action_group_t *actions;
	of12_stats_bucket_counter_t stats;
	
	struct of12_bucket *next;
	
}of12_bucket_t;

typedef struct of12_bucket_list{
	unsigned int num_of_buckets;
	of12_bucket_t* head;
	of12_bucket_t *tail;
}of12_bucket_list_t;

typedef enum{
	OF12_GROUP_TYPE_ALL 		= 0,	/* All (multicast/broadcast) group.  */
	OF12_GROUP_TYPE_SELECT 		= 1,   	/* Select group. */
	OF12_GROUP_TYPE_INDIRECT 	= 2, 	/* Indirect group. */
	OF12_GROUP_TYPE_FF	 	= 3,	/* Fast failover group. */
}of12_group_type_t;

struct of12_group_table;

typedef struct of12_group{
	uint32_t id;
	of12_group_type_t type;
	of12_stats_group_t stats;
	of12_bucket_list_t *bc_list;
	
	struct of12_group_table *group_table;
	
	platform_rwlock_t *rwlock;
	
	struct of12_group *next;
	struct of12_group *prev;
}of12_group_t;

typedef struct of12_group_table{
	uint32_t num_of_entries;
	
	platform_rwlock_t *rwlock;
	
	struct of12_group *head;
	struct of12_group *tail;
}of12_group_table_t;

typedef enum{
	OF12_GROUP_MOD_ERR_OK 			= 0,	/* No error  */
	OF12_GROUP_MOD_ERR_EXISTS 		= 1,   	/* Group already exists */
	OF12_GROUP_MOD_ERR_INVAL	 	= 2, 	/* Invalid group -wrong properties- */
	OF12_GROUP_MOD_ERR_WEIGHT	 	= 3,	/* Weights not supported */
	OF12_GROUP_MOD_ERR_OGRUPS 		= 4,	/* Out of groups  */
	OF12_GROUP_MOD_ERR_OBUCKETS 	= 5,   	/* Out of buckets */
	OF12_GROUP_MOD_ERR_CHAIN	 	= 6, 	/* Chaining not supported */
	OF12_GROUP_MOD_ERR_WATCH	 	= 7,	/* Watch not supported */
	OF12_GROUP_MOD_ERR_LOOP			= 8,	/* Loop in group  */
	OF12_GROUP_MOD_ERR_UNKGRP 		= 9,   	/* Unkown group */
	OF12_GROUP_MOD_ERR_CHNGRP	 	= 10, 	/* Chained group */
	OF12_GROUP_MOD_ERR_BTYPE	 	= 11,	/* Bad type */
	OF12_GROUP_MOD_ERR_BCOMMAND		= 12,	/* Bad command  */
	OF12_GROUP_MOD_ERR_BBUCKET 		= 13,  	/* Bad bucket */
	OF12_GROUP_MOD_ERR_BWATCH	 	= 14, 	/* Bad watch */
	OF12_GROUP_MOD_ERR_EPERM	 	= 15,	/* Permissions error */
}of12_group_mod_err_t;

//fwd decls
struct of12_pipeline;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

//function declarations
of12_group_table_t* of12_init_group_table(void);
void of12_destroy_group_table(of12_group_table_t* gt);
of12_group_mod_err_t of12_group_add(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, of12_bucket_list_t *buckets);
of12_group_mod_err_t of12_group_delete( struct of12_pipeline *pipeline, of12_group_table_t *gt, uint32_t id);
of12_group_t *of12_group_search(of12_group_table_t *gt, uint32_t id);
of12_group_mod_err_t of12_group_modify(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, of12_bucket_list_t *buckets);
of12_bucket_list_t *of12_init_bucket_list(void);
of12_bucket_t *of12_init_bucket(uint16_t weight, uint32_t port, uint32_t group, of12_action_group_t* actions);
rofl_result_t of12_insert_bucket_in_list(of12_bucket_list_t *bu_list,of12_bucket_t *bucket);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif // __OF12_GROUP_TABLE_H__
