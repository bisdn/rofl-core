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


#endif //__TEST_BUFS_H__
