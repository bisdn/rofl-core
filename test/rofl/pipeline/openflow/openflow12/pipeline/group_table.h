#ifndef __GROUP_TABLE_H__
#define __GROUP_TABLE_H__

#include "../../../../../../src/rofl.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/openflow12.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_group_table.h"
#include "../../../../../../src/rofl/pipeline/openflow/of_switch.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/of12_switch.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_pipeline.h"

struct test_utils{
	of12_group_table_t* gt;
	of12_group_type_t type;
	uint32_t id;
	uint32_t weight;
	uint32_t group;
	uint32_t port;
	of12_action_group_t *actions;
};

int set_up(void);
int tear_down(void);
void basic_test(void);
void expected_errors_test(void);
void add_and_delete_buckets_test(void);
void concurrency_test(void);
void references_test(void);

#endif //__GROUP_TABLE_H__