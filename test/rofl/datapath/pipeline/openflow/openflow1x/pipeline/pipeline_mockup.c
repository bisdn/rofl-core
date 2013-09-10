#include <rofl/pipeline/openflow/openflow1x/of1x_switch.h>
#include <rofl/pipeline/openflow/openflow1x/pipeline/of1x_flow_table.h>
#include <rofl/pipeline/platform/platform_hooks.h>

rofl_result_t of1x_init_table(of1x_flow_table_t *table, const unsigned int table_index, const of1x_flow_table_miss_config_t config, const enum matching_algorithm_available matching_algorithm){
	return ROFL_SUCCESS;
}

rofl_result_t of1x_destroy_table(of1x_flow_table_t *table){
	return ROFL_SUCCESS;
}
void of1x_init_packet_matches(datapacket_t* const pkt, of1x_packet_matches_t * pkt_matches){
}

of1x_flow_entry_t* of1x_find_best_match_table(of1x_flow_table_t* const table, of1x_packet_matches_t * const pkt){
	return NULL;
}

void of1x_stats_table_matches_inc(of1x_flow_table_t* table){
}

void of1x_stats_flow_update_match(of1x_flow_entry_t* entry, uint64_t bytes_rx){
}

void of1x_timer_update_entry(of1x_flow_entry_t* entry){
}

unsigned int of1x_process_instructions(const struct of1x_switch* sw, const unsigned int table_id, datapacket_t* const pkt, const of1x_instruction_group_t* instructions){
	return 0;
}

void platform_packet_drop(datapacket_t * pkt){
}

void of1x_stats_table_lookup_inc(of1x_flow_table_t* table){
}

/*
void platform_packet_drop(datapacket_t* pkt){
}
void of1x_init_packet_matches(datapacket_t* const pkt, of1x_packet_matches_t* pkt_matches){
}
*/
