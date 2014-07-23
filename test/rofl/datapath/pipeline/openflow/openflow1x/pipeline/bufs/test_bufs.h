/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __TEST_BUFS_H__
#define __TEST_BUFS_H__

#include "rofl.h"
#include "rofl/datapath/pipeline/physical_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_instruction.h"
#include "rofl/datapath/pipeline/common/large_types.h"


int bufs_set_up(void);
int bufs_tear_down(void);
void bufs_no_output_action(void);
void bufs_apply_output_action(void);
void bufs_write_output_action(void);
void bufs_multiple_apply_output_actions(void);
void bufs_multiple_apply_write_output_actions(void);
void bufs_single_output_action_group_apply(void);
void bufs_single_output_action_group_write(void);
void bufs_apply_and_group_output_actions(void);
void bufs_write_and_group_output_actions(void);
void bufs_multiple_output_actions_group(void);
void bufs_no_output_action_goto(void);
void bufs_apply_output_action_last_table_goto(void);
void bufs_apply_output_action_both_tables_goto(void);
void bufs_apply_output_action_both_tables_bis_goto(void);
void bufs_output_first_table_output_on_group_second_table(void);
void bufs_output_all(void);

#endif //__TEST_BUFS_H__
