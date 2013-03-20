#ifndef __OF12_STATISTICS_H__
#define __OF12_STATISTICS_H__

#include <inttypes.h>
#include <sys/time.h>
#include "../../../platform/lock.h"
#define OF12_STATS_NS_IN_A_SEC 1000000000

/**
 * Definition of the structures that will hold all the data
 * for the statistics to be recorded
 * 
 * must decide what is implemented and in which way to make it
 * as fast as possible
 */

/*From openflow 1.2 specification, page 50*/
/*
 * Types of statistics:
 * DESC, FLOW, AGGREGATE, TABLE, PORT, QUEUE, GROUP, GROUP_DESC, GROUP_FEATURES, EXPERIMENTER
 */

/*counters OF 1.2 specification page 13*/

//fwd declarations
struct of12_flow_entry;
struct of12_flow_table;
struct of12_match;
struct of12_pipeline;


//Flow entry stats (entry state)
typedef struct of12_stats_flow{
	uint64_t packet_count;
	uint64_t byte_count;

	//And more not so interesting
	struct timeval initial_time;

	platform_mutex_t* mutex; //Mutual exclusion among insertion/deletion threads
}of12_stats_flow_t;

/*
* flow stats entry messages
*/
typedef struct of12_stats_single_flow_msg{
	uint8_t table_id;
	uint16_t priority;
	uint64_t cookie;
	
	uint32_t duration_sec;
	uint32_t duration_nsec;
	
	uint16_t idle_timeout;
	uint16_t hard_timeout;
	
	uint64_t packet_count;
	uint64_t byte_count;
	
	struct of12_match* matches;
	
	struct of12_stats_single_flow_msg* next;
}of12_stats_single_flow_msg_t;

//Variable length array
typedef struct of12_stats_flow_msg{
	uint32_t 			num_of_entries; 
	of12_stats_single_flow_msg_t* 	flows_head;
	of12_stats_single_flow_msg_t* 	flows_tail;
}of12_stats_flow_msg_t;

typedef struct of12_stats_flow_aggregate_msg{
	uint64_t packet_count;
	uint64_t byte_count;
	uint32_t flow_count;
}of12_stats_flow_aggregate_msg_t;

//Table stats (table state)
typedef struct of12_stats_table{
	uint64_t lookup_count; /* Number of packets looked up in table. */
	uint64_t matched_count; /* Number of packets that hit table. */
}of12_stats_table_t;

//Port stats (port state)
typedef struct of12_stats_port{
	uint64_t rx_packets;	/* Number of received packets. */
	uint64_t tx_packets;	/* Number of transmitted packets. */
	uint64_t rx_bytes;		/* Number of received bytes. */
	uint64_t tx_bytes;		/* Number of transmitted bytes. */
	//uint64_t rx_dropped;	/* Number of packets dropped by RX. */
	//uint64_t tx_dropped;	/* Number of packets dropped by TX. */
	/*uint64_t rx_errors;		// Number of receive errors. This is a super-set
								of more specific receive errors and should be
								greater than or equal to the sum of all
								rx_*_err values. */
	/*uint64_t tx_errors;		// Number of transmit errors. This is a super-set
								of more specific transmit errors and should be
								greater than or equal to the sum of all
								tx_*_err values (none currently defined.) */
	//uint64_t rx_frame_err;	/* Number of frame alignment errors. */
	//uint64_t rx_over_err;	/* Number of packets with RX overrun. */
	//uint64_t rx_crc_err;	/* Number of CRC errors. */
	//uint64_t collisions;	/* Number of collisions. */
	
	platform_mutex_t* mutex; /*mutual exclusion to update the statistics*/
}of12_stats_port_t;

//Port stats (port state)
typedef struct of12_stats_queue{
	uint64_t tx_bytes;
	uint64_t tx_packets;
	uint64_t tx_errors;
	
	platform_mutex_t* mutex;
}of12_stats_queue_t;

//Group stats, bucket
typedef struct of12_stats_bucket_counter{
	uint64_t packet_count;
	uint64_t byte_count;
	platform_mutex_t* mutex;
}of12_stats_bucket_counter_t;

//Group stats
typedef struct of12_stats_group{
	uint32_t ref_count;
	uint64_t packet_count;
	uint64_t byte_count;
	struct of12_stats_bucket_counter bucket_stats[0];
	platform_mutex_t* mutex;
}of12_stats_group_t;

/** operations in statistics.c **/

ROFL_PIPELINE_BEGIN_DECLS

void of12_init_flow_stats(struct of12_flow_entry * entry);
void of12_destroy_flow_stats(struct of12_flow_entry * entry);

//msgs
of12_stats_single_flow_msg_t* of12_init_stats_single_flow_msg(struct of12_flow_entry* entry);
void of12_destroy_stats_single_flow_msg(of12_stats_single_flow_msg_t* msg);

of12_stats_flow_msg_t* of12_init_stats_flow_msg(void);
void of12_destroy_stats_flow_msg(of12_stats_flow_msg_t* msg);

//Push to msg
void of12_push_single_flow_stats_to_msg(of12_stats_flow_msg_t* msg, of12_stats_single_flow_msg_t* sfs);


of12_stats_flow_aggregate_msg_t* of12_init_stats_flow_aggregate_msg(void);
void of12_destroy_stats_flow_aggregate_msg(of12_stats_flow_aggregate_msg_t* msg);

void of12_stats_flow_reset_counts(struct of12_flow_entry * entry);
void of12_stats_flow_get_duration(struct of12_flow_entry * entry, uint32_t* sec, uint32_t* nsec);
void of12_stats_flow_update_match(struct of12_flow_entry * entry,uint64_t bytes_rx);
void of12_stats_flow_inc(struct of12_flow_entry * entry,uint64_t bytes_rx);
void of12_stats_table_init(struct of12_flow_table * table);
void of12_stats_table_lookup_inc(struct of12_flow_table * table);
void of12_stats_table_matches_inc(struct of12_flow_table * table);

void of12_init_group_stats(of12_stats_group_t *group_stats);
void of12_stats_group_update(of12_stats_group_t *gr_stats, uint64_t bytes);
void of12_stats_group_inc_reference(of12_stats_group_t *gr_stats);
void of12_stats_group_dec_reference(of12_stats_group_t *gr_stats);

void of12_init_bucket_stats(of12_stats_bucket_counter_t *bc_stats);
void of12_stats_bucket_update(of12_stats_bucket_counter_t* bc_stats, uint64_t bytes);

/*
* FIXME TODO XXX Move it down to the port
*/
void of12_stats_port_init(of12_stats_port_t* port_stats);
void of12_stats_port_destroy(of12_stats_port_t *port_stats);
inline void of12_stats_port_rx_packet_inc(of12_stats_port_t *port_stats, uint64_t bytes_rx);
inline void of12_stats_port_tx_packet_inc(of12_stats_port_t *port_stats, uint64_t bytes_tx);
void of12_stats_queue_init(of12_stats_queue_t *queue_stats);
void of12_stats_queue_destroy(of12_stats_queue_t *queue_stats);
inline void of12_stats_queue_tx_packet_inc(of12_stats_queue_t *queue_stats, uint64_t bytes);
inline void of12_stats_queue_tx_errors_inc(of12_stats_queue_t *queue_stats);

/*
* External interfaces
*/
of12_stats_flow_msg_t* of12_get_flow_stats(struct of12_pipeline* pipeline, uint8_t table_id, uint32_t cookie, uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, struct of12_match* matchs);
of12_stats_flow_aggregate_msg_t* of12_get_flow_aggregate_stats(struct of12_pipeline* pipeline, uint8_t table_id, uint32_t cookie, uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, struct of12_match* matchs);

ROFL_PIPELINE_END_DECLS

#endif

