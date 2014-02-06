#ifndef __TIMERS_HARD_TIMEOUT_H__
#define __TIMERS_HARD_TIMEOUT_H__

#include "rofl/datapath/pipeline/physical_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_table.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_timers.h"
#include "rofl/datapath/pipeline/platform/lock.h"
#include "rofl/datapath/pipeline/platform/memory.h"

void main_test(void);

void time_forward(uint64_t sec, uint64_t usec, struct timeval * time);
/*
static int setup_test(of1x_pipeline_t* pipeline, of1x_flow_table_t* table, of1x_flow_entry_t ** entries);
static int clean_up(of1x_flow_entry_t * entries, of1x_flow_table_t* table);

void test_insert_and_expiration_static(of1x_pipeline_t * pipeline, uint32_t hard_timeout, of1x_flow_entry_t * entry);
void test_insert_and_extract_static(of1x_pipeline_t * pipeline, uint32_t hard_timeout, int num_of_entries, of1x_flow_entry_t * entry);
void test_simple_idle_static(of1x_pipeline_t * pipeline, of1x_flow_entry_t * entry, uint32_t ito);
void test_insert_both_expires_one_check_the_other_static(of1x_pipeline_t * pipeline, of1x_flow_entry_t * entry, uint32_t hto, uint32_t ito);

void test_incremental_insert_and_expiration_dynamic(of1x_pipeline_t * pipeline, of1x_flow_entry_t* entry);
void test_insert_and_extract_dynamic(of1x_pipeline_t * pipeline, uint32_t hard_timeout, int num_of_entries, of1x_flow_entry_t * entry);
void test_simple_idle_dynamic(of1x_pipeline_t * pipeline, of1x_flow_entry_t * entry, uint32_t ito);
void test_insert_both_expires_one_check_the_other_dynamic(of1x_pipeline_t * pipeline, of1x_flow_entry_t * entry, uint32_t hto, uint32_t ito);
*/

#endif //__TIMERS_HARD_TIMEOUT_H__
