#ifndef __TIMERS_HARD_TIMEOUT_H__
#define __TIMERS_HARD_TIMEOUT_H__

#include "../../../../../../src/rofl/pipeline/openflow/openflow12/of12_switch.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_pipeline.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_flow_entry.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_flow_table.h"
#include "../../../../../../src/rofl/pipeline/openflow/openflow12/pipeline/of12_timers.h"
#include "../../../../../../src/rofl/pipeline/platform/lock.h"
#include "../../../../../../src/rofl/pipeline/platform/memory.h"

void main_test(void);

/*
static int setup_test(of12_pipeline_t* pipeline, of12_flow_table_t* table, of12_flow_entry_t ** entries);
static int clean_up(of12_flow_entry_t * entries, of12_flow_table_t* table);

void test_insert_and_expiration_static(of12_pipeline_t * pipeline, uint32_t hard_timeout, of12_flow_entry_t * entry);
void test_insert_and_extract_static(of12_pipeline_t * pipeline, uint32_t hard_timeout, int num_of_entries, of12_flow_entry_t * entry);
void test_simple_idle_static(of12_pipeline_t * pipeline, of12_flow_entry_t * entry, uint32_t ito);
void test_insert_both_expires_one_check_the_other_static(of12_pipeline_t * pipeline, of12_flow_entry_t * entry, uint32_t hto, uint32_t ito);

void test_incremental_insert_and_expiration_dynamic(of12_pipeline_t * pipeline, of12_flow_entry_t* entry);
void test_insert_and_extract_dynamic(of12_pipeline_t * pipeline, uint32_t hard_timeout, int num_of_entries, of12_flow_entry_t * entry);
void test_simple_idle_dynamic(of12_pipeline_t * pipeline, of12_flow_entry_t * entry, uint32_t ito);
void test_insert_both_expires_one_check_the_other_dynamic(of12_pipeline_t * pipeline, of12_flow_entry_t * entry, uint32_t hto, uint32_t ito);
*/

#endif //__TIMERS_HARD_TIMEOUT_H__
