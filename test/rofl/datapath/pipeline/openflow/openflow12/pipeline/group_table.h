#ifndef __GROUP_TABLE_H__
#define __GROUP_TABLE_H__

#include "rofl.h"
#include "rofl/datapath/pipeline/openflow/openflow12/openflow12.h"
#include "rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_group_table.h"
#include "rofl/datapath/pipeline/openflow/of_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow12/of12_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_pipeline.h"

struct test_utils{
	of12_group_table_t* gt;
	of12_group_type_t type;
	uint32_t id;
	uint32_t weight;
	uint32_t group;
	uint32_t port;
	of12_action_group_t *actions;
	of12_bucket_list_t *bu_list;
};

int gt_set_up(void);
int gt_tear_down(void);
void gt_basic_test(void);
void gt_expected_errors_test(void);
void gt_add_and_delete_buckets_test(void);
void gt_concurrency_test(void);
void gt_references_test(void);

#endif //__GROUP_TABLE_H__
