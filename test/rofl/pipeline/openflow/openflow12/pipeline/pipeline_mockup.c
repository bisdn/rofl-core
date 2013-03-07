#include <rofl/pipeline/openflow/openflow12/of12_switch.h>
#include <rofl/pipeline/openflow/openflow12/pipeline/of12_flow_table.h>
#include <rofl/pipeline/platform/platform_hooks.h>

rofl_result_t of12_init_table(of12_flow_table_t *table, const unsigned int table_index, const of12_flow_table_miss_config_t config, const enum matching_algorithm_available matching_algorithm){
	return ROFL_SUCCESS;
}

rofl_result_t of12_destroy_table(of12_flow_table_t *table){
	return ROFL_SUCCESS;
}
void of12_init_packet_matches(datapacket_t* const pkt, of12_packet_matches_t * pkt_matches){
}

of12_flow_entry_t* of12_find_best_match_table(of12_flow_table_t* const table, of12_packet_matches_t * const pkt){
	return NULL;
}

void of12_stats_table_matches_inc(of12_flow_table_t* table){
}

void of12_stats_flow_update_match(of12_flow_entry_t* entry, uint64_t bytes_rx){
}

void of12_timer_update_entry(of12_flow_entry_t* entry){
}

unsigned int of12_process_instructions(const struct of12_switch* sw, const unsigned int table_id, datapacket_t* const pkt, const of12_instruction_group_t* instructions){
	return 0;
}

void platform_packet_drop(datapacket_t * pkt){
}

void of12_stats_table_lookup_inc(of12_flow_table_t* table){
}

/*
void platform_packet_drop(datapacket_t* pkt){
}
void of12_init_packet_matches(datapacket_t* const pkt, of12_packet_matches_t* pkt_matches){
}
*/
