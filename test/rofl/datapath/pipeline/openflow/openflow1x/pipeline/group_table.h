/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GROUP_TABLE_H__
#define __GROUP_TABLE_H__

#include "rofl.h"
//#include "rofl/common/openflow/openflow1x.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_group_table.h"
#include "rofl/datapath/pipeline/openflow/of_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h"

struct test_utils{
	of1x_group_table_t* gt;
	of1x_group_type_t type;
	uint32_t id;
	uint32_t weight;
	uint32_t group;
	uint32_t port;
	of1x_action_group_t *actions;
	of1x_bucket_list_t *bu_list;
};

int gt_set_up(void);
int gt_tear_down(void);
void gt_basic_test(void);
void gt_expected_errors_test(void);
void gt_add_and_delete_buckets_test(void);
void gt_concurrency_test(void);
void gt_references_test(void);

#endif //__GROUP_TABLE_H__
