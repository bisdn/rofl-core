/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OUTPUT_ACTIONS_H__
#define __OUTPUT_ACTIONS_H__

#include "rofl.h"
// #include "rofl/datapath/pipeline/openflow/openflow1x/openflow1x.h"
#include "rofl/datapath/pipeline/physical_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_instruction.h"
//#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_group_table.h"


int oa_set_up(void);
int oa_tear_down(void);
void oa_basic_test(void);
void oa_test_with_groups(void);
void oa_no_output();
void oa_only_apply(void);
void oa_only_write(void);
void oa_two_outputs_apply(void);
void oa_two_outputs_write(void);
void oa_write_and_group(void);
void oa_apply_and_group(void);



#endif //__OUTPUT_ACTIONS_H__
