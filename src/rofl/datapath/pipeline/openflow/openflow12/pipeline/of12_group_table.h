/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF12_GROUP_TABLE_H__
#define __OF12_GROUP_TABLE_H__

#include "rofl.h"
#include "of12_statistics.h"
#include "of12_action.h"
#include "of12_flow_entry.h"
#include "../../../platform/lock.h"

#define OF12_GROUP_MAX 0xffffff00
#define OF12_GROUP_ALL 0xfffffffc  /* Represents all groups for group delete commands. */
#define OF12_GROUP_ANY 0xffffffff /* Wildcard group used only for flow stats */

/**
* @file of12_group_table.h
* @author Victor Alvarez<victor.alvarez (at) bisdn.de>, Marc Sune<marc.sune (at) bisdn.de>
* @brief Openflow v1.2 group table subsystem 
*/

/**
* @ingroup core_of12 
* Group bucket
*/
typedef struct of12_bucket{
	uint16_t weight;
	uint32_t port;
	uint32_t group;
	of12_action_group_t *actions;
	of12_stats_bucket_counter_t stats;
	
	struct of12_bucket *next;
	
}of12_bucket_t;

/**
* @ingroup core_of12 
* Group bucket list 
*/
typedef struct of12_bucket_list{
	unsigned int num_of_buckets;
	of12_bucket_t* head;
	of12_bucket_t *tail;
}of12_bucket_list_t;

/**
* @ingroup core_of12 
* Group type
*/
typedef enum{
	OF12_GROUP_TYPE_ALL 		= 0,	/* All (multicast/broadcast) group.  */
	OF12_GROUP_TYPE_SELECT 		= 1,   	/* Select group. */
	OF12_GROUP_TYPE_INDIRECT 	= 2, 	/* Indirect group. */
	OF12_GROUP_TYPE_FF	 	= 3,	/* Fast failover group. */
}of12_group_type_t;

struct of12_group_table;

/**
* @ingroup core_of12 
* Group structure definition
*/ 
typedef struct of12_group{
	uint32_t id;
	of12_group_type_t type;
	of12_stats_group_t stats;
	of12_bucket_list_t *bc_list;
	
	struct of12_group_table *group_table;
	
	platform_rwlock_t *rwlock;
	
	struct of12_group *next;
	struct of12_group *prev;
	
	unsigned int num_of_output_actions;
}of12_group_t;

typedef struct of12_group_table{
	uint32_t num_of_entries;
	
	platform_rwlock_t *rwlock;
	
	struct of12_group *head;
	struct of12_group *tail;
}of12_group_table_t;

typedef enum{
	ROFL_OF12_GM_OK 			= 0,	/* No error  */
	ROFL_OF12_GM_EXISTS 		= 1,   	/* Group already exists */
	ROFL_OF12_GM_INVAL	 	= 2, 	/* Invalid group -wrong properties- */
	ROFL_OF12_GM_WEIGHT	 	= 3,	/* Weights not supported */
	ROFL_OF12_GM_OGRUPS 		= 4,	/* Out of groups  */
	ROFL_OF12_GM_OBUCKETS 	= 5,   	/* Out of buckets */
	ROFL_OF12_GM_CHAIN	 	= 6, 	/* Chaining not supported */
	ROFL_OF12_GM_WATCH	 	= 7,	/* Watch not supported */
	ROFL_OF12_GM_LOOP			= 8,	/* Loop in group  */
	ROFL_OF12_GM_UNKGRP 		= 9,   	/* Unkown group */
	ROFL_OF12_GM_CHNGRP	 	= 10, 	/* Chained group */
	ROFL_OF12_GM_BTYPE	 	= 11,	/* Bad type */
	ROFL_OF12_GM_BCOMMAND		= 12,	/* Bad command  */
	ROFL_OF12_GM_BBUCKET 		= 13,  	/* Bad bucket */
	ROFL_OF12_GM_BWATCH	 	= 14, 	/* Bad watch */
	ROFL_OF12_GM_EPERM	 	= 15,	/* Permissions error */
}rofl_of12_gm_result_t;

//fwd decls
struct of12_pipeline;

//C++ extern C
ROFL_BEGIN_DECLS

//function declarations

//FIXME: put documentation
of12_group_table_t* of12_init_group_table(void);
//FIXME: put documentation
void of12_destroy_group_table(of12_group_table_t* gt);

//FIXME: put documentation <- Is this function and the subsequent AFA one really necessary?
rofl_result_t of12_fetch_group_table(struct of12_pipeline *pipeline, of12_group_table_t* group_table);

//FIXME: put documentation
rofl_of12_gm_result_t of12_group_add(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, of12_bucket_list_t *buckets);

//FIXME: put documentation
rofl_of12_gm_result_t of12_group_modify(of12_group_table_t *gt, of12_group_type_t type, uint32_t id, of12_bucket_list_t *buckets);

//FIXME: put documentation
rofl_of12_gm_result_t of12_group_delete( struct of12_pipeline *pipeline, of12_group_table_t *gt, uint32_t id);

//FIXME: put documentation
of12_bucket_list_t* of12_init_bucket_list(void);

//FIXME: put documentation
void of12_destroy_bucket_list(of12_bucket_list_t *bc_list);

//FIXME: put documentation
of12_bucket_t* of12_init_bucket(uint16_t weight, uint32_t port, uint32_t group, of12_action_group_t* actions);

//FIXME: put documentation
rofl_result_t of12_insert_bucket_in_list(of12_bucket_list_t *bu_list,of12_bucket_t *bucket);

of12_group_t* __of12_group_search(of12_group_table_t *gt, uint32_t id);

//C++ extern C
ROFL_END_DECLS

#endif // __OF12_GROUP_TABLE_H__
