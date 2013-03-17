#ifndef __OF12_GROUP_TABLE_H__
#define __OF12_GROUP_TABLE_H__

#include "rofl.h"
#include "of12_statistics.h"
#include "of12_action.h"
#include "of12_flow_entry.h"
#include <rofl/pipeline/platform/lock.h>

#define OF12_GROUP_ANY 0xffffffff /* Wildcard group used only for flow stats */

typedef struct of12_group_bucket{
	uint16_t weigth;
	uint32_t port;
	uint32_t group;
	uint32_t num_of_actions;
	of12_action_group_t *actions;
	of12_stats_bucket_counter_t stats;
	
	struct of12_group_bucket *next;
	
}of12_group_bucket_t;

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
	of12_group_bucket_t *bl_head;
	of12_group_bucket_t *bl_tail;
	
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

//fwd decls
struct of12_pipeline;

//function declarations
of12_group_table_t* of12_init_group_table(void);
void of12_destroy_group_table(of12_group_table_t* gt);
rofl_result_t of12_group_add(of12_group_table_t *gt, of12_group_type_t type, uint32_t id,
							 uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t **actions);
rofl_result_t of12_group_delete(of12_group_table_t *gt, uint32_t id, struct of12_pipeline *pipeline);
of12_group_t *of12_group_search(of12_group_table_t *gt, uint32_t id);
rofl_result_t of12_group_modify(of12_group_table_t *gt, of12_group_type_t type, uint32_t id,
								uint32_t weigth, uint32_t group, uint32_t port, of12_action_group_t **actions);

#endif // __OF12_GROUP_TABLE_H__
