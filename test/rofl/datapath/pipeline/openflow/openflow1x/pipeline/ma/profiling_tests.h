#ifndef MATCHING_TEST
#define MATCHING_TEST

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <CUnit/Basic.h>

#include "rofl/datapath/pipeline/physical_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_match.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_table.h"

/* Setup/teardown */
int set_up(void);
int tear_down(void);
	
/* Test cases */
void profile_basic_match(void);
void profile_basic_no_match(void);

#endif
