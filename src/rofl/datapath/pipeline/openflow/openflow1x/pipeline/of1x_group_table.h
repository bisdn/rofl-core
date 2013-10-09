/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_GROUP_TABLE_H__
#define __OF1X_GROUP_TABLE_H__

#include "rofl.h"
#include "of1x_statistics.h"
#include "of1x_action.h"
#include "of1x_flow_entry.h"
#include "../../../platform/lock.h"

#define OF1X_GROUP_MAX 0xffffff00
#define OF1X_GROUP_ALL 0xfffffffc  /* Represents all groups for group delete commands. */
#define OF1X_GROUP_ANY 0xffffffff /* Wildcard group used only for flow stats */

/**
* @file of1x_group_table.h
* @author Victor Alvarez<victor.alvarez (at) bisdn.de>, Marc Sune<marc.sune (at) bisdn.de>
* @brief OpenFlow v1.0, 1.2 and 1.3.2 group table subsystem 
*/

/**
* @ingroup core_of1x 
* Group bucket
*/
typedef struct of1x_bucket{
	uint16_t weight;
	uint32_t port;
	uint32_t group;
	of1x_action_group_t *actions;
	of1x_stats_bucket_counter_t stats;
	
	struct of1x_bucket *next;
	
}of1x_bucket_t;

/**
* @ingroup core_of1x 
* Group bucket list 
*/
typedef struct of1x_bucket_list{
	unsigned int num_of_buckets;
	of1x_bucket_t* head;
	of1x_bucket_t *tail;
}of1x_bucket_list_t;

/**
* @ingroup core_of1x 
* Group type
*/
typedef enum{
	OF1X_GROUP_TYPE_ALL 		= 0,	/* All (multicast/broadcast) group.  */
	OF1X_GROUP_TYPE_SELECT 		= 1,   	/* Select group. */
	OF1X_GROUP_TYPE_INDIRECT 	= 2, 	/* Indirect group. */
	OF1X_GROUP_TYPE_FF	 	= 3,	/* Fast failover group. */
}of1x_group_type_t;

struct of1x_group_table;

/**
* @ingroup core_of1x 
* Group structure definition
*/ 
typedef struct of1x_group{
	uint32_t id;
	of1x_group_type_t type;
	of1x_stats_group_t stats;
	of1x_bucket_list_t *bc_list;
	
	struct of1x_group_table *group_table;
	
	platform_rwlock_t *rwlock;
	
	struct of1x_group *next;
	struct of1x_group *prev;
	
	unsigned int num_of_output_actions;
}of1x_group_t;

typedef struct of1x_group_table{
	uint32_t num_of_entries;
	
	platform_rwlock_t *rwlock;
	
	struct of1x_group *head;
	struct of1x_group *tail;
}of1x_group_table_t;

typedef enum{
	ROFL_OF1X_GM_OK 			= 0,	/* No error  */
	ROFL_OF1X_GM_EXISTS 		= 1,   	/* Group already exists */
	ROFL_OF1X_GM_INVAL	 	= 2, 	/* Invalid group -wrong properties- */
	ROFL_OF1X_GM_WEIGHT	 	= 3,	/* Weights not supported */
	ROFL_OF1X_GM_OGRUPS 		= 4,	/* Out of groups  */
	ROFL_OF1X_GM_OBUCKETS 	= 5,   	/* Out of buckets */
	ROFL_OF1X_GM_CHAIN	 	= 6, 	/* Chaining not supported */
	ROFL_OF1X_GM_WATCH	 	= 7,	/* Watch not supported */
	ROFL_OF1X_GM_LOOP			= 8,	/* Loop in group  */
	ROFL_OF1X_GM_UNKGRP 		= 9,   	/* Unkown group */
	ROFL_OF1X_GM_CHNGRP	 	= 10, 	/* Chained group */
	ROFL_OF1X_GM_BTYPE	 	= 11,	/* Bad type */
	ROFL_OF1X_GM_BCOMMAND		= 12,	/* Bad command  */
	ROFL_OF1X_GM_BBUCKET 		= 13,  	/* Bad bucket */
	ROFL_OF1X_GM_BWATCH	 	= 14, 	/* Bad watch */
	ROFL_OF1X_GM_EPERM	 	= 15,	/* Permissions error */
}rofl_of1x_gm_result_t;

//fwd decls
struct of1x_pipeline;

//C++ extern C
ROFL_BEGIN_DECLS

//function declarations

//FIXME: put documentation
of1x_group_table_t* of1x_init_group_table(void);
//FIXME: put documentation
void of1x_destroy_group_table(of1x_group_table_t* gt);

//FIXME: put documentation <- Is this function and the subsequent AFA one really necessary?
rofl_result_t of1x_fetch_group_table(struct of1x_pipeline *pipeline, of1x_group_table_t* group_table);

//FIXME: put documentation
rofl_of1x_gm_result_t of1x_group_add(of1x_group_table_t *gt, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t *buckets);

//FIXME: put documentation
rofl_of1x_gm_result_t of1x_group_modify(of1x_group_table_t *gt, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t *buckets);

//FIXME: put documentation
rofl_of1x_gm_result_t of1x_group_delete( struct of1x_pipeline *pipeline, of1x_group_table_t *gt, uint32_t id);

//FIXME: put documentation
of1x_bucket_list_t* of1x_init_bucket_list(void);

//FIXME: put documentation
void of1x_destroy_bucket_list(of1x_bucket_list_t *bc_list);

//FIXME: put documentation
of1x_bucket_t* of1x_init_bucket(uint16_t weight, uint32_t port, uint32_t group, of1x_action_group_t* actions);

//FIXME: put documentation
rofl_result_t of1x_insert_bucket_in_list(of1x_bucket_list_t *bu_list,of1x_bucket_t *bucket);

of1x_group_t* __of1x_group_search(of1x_group_table_t *gt, uint32_t id);

//C++ extern C
ROFL_END_DECLS

#endif // __OF1X_GROUP_TABLE_H__
