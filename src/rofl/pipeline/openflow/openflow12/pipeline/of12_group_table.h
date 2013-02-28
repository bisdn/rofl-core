#ifndef __OF12_GROUP_TABLE_H__
#define __OF12_GROUP_TABLE_H__

#include "rofl.h"
#include "of12_statistics.h"
#include "of12_action.h"
#include "../openflow12.h"

typedef struct of12_action_bucket{
	uint16_t weigth;
	uint32_t port;
	uint32_t group;
	uint32_t num_of_actions;
	enum of12p_group_type action_type_list[];
	of12_stats_bucket_counter_t stats;
}of12_action_buckets_t;

typedef struct of12_group_entry{
	uint32_t id;
	enum of12p_group_type type;
	of12_stats_group_t stats;
	of12_action_buckets_t *buckets_list;
	
	struct of12_group_entry *next;
	struct of12_group_entry *prev;
}of12_group_entry_t;

typedef struct of12_group_table{
	uint32_t num_of_entries;
	struct of12_group_entry *head;
	struct of12_group_entry *tail;
}of12_group_table_t;


//function declarations
of12_group_table_t* of12_group_table_init();
void of12_group_table_destroy(of12_group_table_t* gt);
rofl_result_t of12_group_add(of12_group_table_t *gt, enum of12p_group_type type, uint32_t id);
rofl_result_t of12_group_delete(of12_group_table_t *gt, uint32_t id);
rofl_result_t of12_group_modify(of12_group_table_t *gt, enum of12p_group_type type, uint32_t id);


#endif // __OF12_GROUP_TABLE_H__
