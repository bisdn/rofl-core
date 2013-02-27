#ifndef __OF12_GROUP_TABLE_H__
#define __OF12_GROUP_TABLE_H__

#include "of12_statistics.h"
#include "of12_action.h"
#include "../openflow12.h"

struct of12_bucket{
	uint16_t weigth;
	uint32_t port;
	uint32_t group;
	of12_action_group_t actions;
	of12_stats_bucket_counter_t stats;
};
typedef struct of12_action_buckets of12_action_buckets_t;

struct of12_group_entry{
	uint32_t id;
	enum of12p_group_type type;
	of12_stats_group_t stats;
	of12_action_buckets_t *buckets_list;
	
	struct of12_group_entry *next;
	struct of12_group_entry *prev;
};
typedef struct of12_group_entry of12_group_entry_t;

struct of12_group_table{
	uint32_t num_of_entries;
	struct of12_group_entry *head;
	struct of12_group_entry *tail;
};
typedef struct of12_group_table of12_group_table_t;


#endif // __OF12_GROUP_TABLE_H__
