/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_GROUP_TABLE_H__
#define __OF1X_GROUP_TABLE_H__

#include "rofl.h"
#include "of1x_statistics.h"
#include "of1x_action.h"
#include "of1x_flow_entry.h"
#include "of1x_group_types.h"
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
	__of1x_stats_bucket_t stats;
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

//Group table configuration
typedef struct{
	bitmap128_t supported_actions;	/* Bitmap of (1 << OF1X_AT_* that are supported by the group table. */
}of1x_group_table_config_t;

typedef struct of1x_group_table{

	//Configuration
	of1x_group_table_config_t config;

	uint32_t num_of_entries;

	platform_mutex_t *mutex;
	platform_rwlock_t *rwlock;

	struct of1x_group *head;
	struct of1x_group *tail;

	//Reference back
	struct of1x_pipeline* pipeline;

}of1x_group_table_t;

typedef enum{
	ROFL_OF1X_GM_OK 		= 0,	/* No error  */
	ROFL_OF1X_GM_EXISTS 		= 1,   	/* Group already exists */
	ROFL_OF1X_GM_INVAL	 	= 2, 	/* Invalid group -wrong properties- */
	ROFL_OF1X_GM_WEIGHT	 	= 3,	/* Weights not supported */
	ROFL_OF1X_GM_OGRUPS 		= 4,	/* Out of groups  */
	ROFL_OF1X_GM_OBUCKETS		= 5,   	/* Out of buckets */
	ROFL_OF1X_GM_CHAIN	 	= 6, 	/* Chaining not supported */
	ROFL_OF1X_GM_WATCH	 	= 7,	/* Watch not supported */
	ROFL_OF1X_GM_LOOP		= 8,	/* Loop in group  */
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

/**
 * @brief Initializes the group table.
 * @ingroup core_of1x
 *
 * This is done during the  initialization of the pipeline
 */
of1x_group_table_t* of1x_init_group_table(struct of1x_pipeline *pipeline);

/**
 * @brief Destroys the group table.
 * @ingroup core_of1x
 *
 * This is done during the reset of the pipeline
 */
void of1x_destroy_group_table(of1x_group_table_t* gt);

/**
 * @brief Adds a group to the table.
 * @ingroup core_of1x
 *
 * If a group with the ID already exists returns error
 */
rofl_of1x_gm_result_t of1x_group_add(of1x_group_table_t *gt, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t **buckets);

/**
 * @brief Modifies a group on the table.
 * @ingroup core_of1x
 *
 * Searches the group with the defined ID and modifies the bucket list and the type
 */
rofl_of1x_gm_result_t of1x_group_modify(of1x_group_table_t *gt, of1x_group_type_t type, uint32_t id, of1x_bucket_list_t **buckets);

/**
 * @brief Deletes a group of the table.
 * @ingroup core_of1x
 *
 * Deletes the group structure. If id == OF1X_GROUP_ALL, all groups are deleted. If the group id is not found no error is send.
 */
rofl_of1x_gm_result_t of1x_group_delete( struct of1x_pipeline *pipeline, of1x_group_table_t *gt, uint32_t id);

/**
 * @brief Initializes a list of buckets.
 * @ingroup core_of1x
 *
 * Allocates memory for a bucket list
 */
of1x_bucket_list_t* of1x_init_bucket_list(void);

/**
 * @brief Destroys the bucket list.
 * @ingroup core_of1x
 *
 * Frees memory of a bucket list
 */
void of1x_destroy_bucket_list(of1x_bucket_list_t *bc_list);

/**
 * @brief Initializes a bucket.
 * @ingroup core_of1x
 *
 * Allocates memory and sets the values for a bucket
 */
of1x_bucket_t* of1x_init_bucket(uint16_t weight, uint32_t port, uint32_t group, of1x_action_group_t* actions);

/**
 * @brief Inserts an initialized bucket in the list of buckets.
 * @ingroup core_of1x
 *
 */
rofl_result_t of1x_insert_bucket_in_list(of1x_bucket_list_t *bu_list,of1x_bucket_t *bucket);

of1x_group_t* __of1x_group_search(of1x_group_table_t *gt, uint32_t id);
void __of12_set_group_table_defaults(of1x_group_table_t *gt);
void __of13_set_group_table_defaults(of1x_group_table_t *gt);
/*
* Dump group table. Not recommended to use it directly
*
* @param raw_nbo Show values in the pipeline internal byte order (NBO). Warning: some values are intentionally unaligned.
*/
void of1x_dump_group_table(of1x_group_table_t *gt, bool raw_nbo);

//C++ extern C
ROFL_END_DECLS

#endif // __OF1X_GROUP_TABLE_H__
