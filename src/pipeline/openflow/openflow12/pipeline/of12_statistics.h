#ifndef __OF12_STATISTICS_H__
#define __OF12_STATISTICS_H__

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

struct of12_stats_flow
{
	uint32_t duration_sec;
	uint32_t duration_nsec;
	uint64_t packet_count;
	uint64_t byte_count;
	//And more not so interesting
	
	uint64_t initial_time_sec;
	uint64_t initial_time_nsec;
	uint64_t last_time_sec;
	uint64_t last_time_usec;
	platform_mutex_t mutex; //Mutual exclusion among insertion/deletion threads
};
typedef struct of12_stats_flow of12_stats_flow_t;

struct of12_stats_aggregate
{
	uint64_t packet_count;
	uint64_t byte_count;
	uint32_t flow_count;
};
typedef struct of12_stats_aggregate of12_stats_aggregate_t;

struct of12_stats_table
{
	uint64_t lookup_count; /* Number of packets looked up in table. */
	uint64_t matched_count; /* Number of packets that hit table. */
};
typedef struct of12_stats_table of12_stats_table_t;

struct of12_stats_port
{
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
	
	platform_mutex_t mutex; /*mutual exclusion to update the statistics*/
};
typedef struct of12_stats_port of12_stats_port_t;

struct of12_stats_queue
{
	uint64_t tx_bytes;
	uint64_t tx_packets;
	uint64_t tx_errors;
	
	platform_mutex_t mutex;
};
typedef struct of12_stats_queue of12_stats_queue_t;

#if 0
//TODO
struct of12_stats_bucket_counter
{
	uint64_t packet_count;
	uint64_t byte_count;
};
typedef struct of12_stats_bucket_counter of12_stats_bucket_counter_t;

struct of12_stats_group
{
	uint32_t ref_count;
	uint64_t packet_count;
	uint64_t byte_count;
	struct of12_stats_bucket_counter bucket_stats[0];
};
typedef struct of12_stats_group of12_stats_group_t;
#endif

/** operations in statistics.c **/

//fwd declarations
struct of12_flow_entry;
struct of12_flow_table;

ROFL_PIPELINE_BEGIN_DECLS

void of12_stats_flow_init(struct of12_flow_entry * entry);
void of12_stats_flow_destroy(struct of12_flow_entry * entry);
void of12_stats_flow_duration_update(struct of12_flow_entry * entry);
void of12_stats_flow_get_duration(struct of12_flow_entry * entry);
inline void of12_stats_flow_inc(struct of12_flow_entry * entry,uint64_t bytes_rx);
void of12_stats_table_init(struct of12_flow_table * table);
void of12_stats_table_lookup_inc(struct of12_flow_table * table);
void of12_stats_table_matches_inc(struct of12_flow_table * table);
//void of12_stats_aggregate_collect (struct of12_flow_entry* entry, of12_stats_aggregate_t* aggregate);
void of12_stats_port_init(of12_stats_port_t* port_stats);
void of12_stats_port_destroy(of12_stats_port_t *port_stats);
inline void of12_stats_port_rx_packet_inc(of12_stats_port_t *port_stats, uint64_t bytes_rx);
inline void of12_stats_port_tx_packet_inc(of12_stats_port_t *port_stats, uint64_t bytes_tx);
void of12_stats_queue_init(of12_stats_queue_t *queue_stats);
void of12_stats_queue_destroy(of12_stats_queue_t *queue_stats);
inline void of12_stats_queue_tx_packet_inc(of12_stats_queue_t *queue_stats, uint64_t bytes);
inline void of12_stats_queue_tx_errors_inc(of12_stats_queue_t *queue_stats);

ROFL_PIPELINE_END_DECLS

#endif

