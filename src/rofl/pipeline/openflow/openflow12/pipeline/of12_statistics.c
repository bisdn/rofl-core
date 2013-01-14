#include "of12_statistics.h"
#include "of12_pipeline.h"
#include "of12_flow_table.h"
#include "of12_flow_entry.h"
#include "of12_timers.h"
#include <rofl/pipeline/platform/atomic_operations.h>

/**
 * Here the functions that initialize and update
 * the statistics are defined
 * 
 * Need to see in which are the input parameters that
 * make it easyer and logic
 */

//Flow Statistics functions
/**
 * of12_stats_flow_init
 * stores the time when the flow entry was created
 */
void of12_stats_flow_init(of12_flow_entry_t * entry)
{
	struct timeval now;
	of12_gettimeofday(&now, NULL);
	entry->stats.initial_time_sec = now.tv_sec;
	entry->stats.initial_time_nsec = now.tv_usec * 1000;
	entry->stats.packet_count = 0;
	entry->stats.byte_count = 0;

	entry->stats.mutex = platform_mutex_init(NULL);

	return;
}

/**
 * of12_stats_flow_destroy
 * basically destroys the mutex
 */
void of12_stats_flow_destroy(of12_flow_entry_t* entry)
{
	platform_mutex_destroy(entry->stats.mutex);
}

/**
 * of12_stats_flow_duration_update()
 */
void of12_stats_flow_duration_update(of12_flow_entry_t * entry)
{
	struct timeval now;
	of12_gettimeofday(&now, NULL);
	
	platform_mutex_lock(entry->stats.mutex);
	entry->stats.last_time_sec = now.tv_sec;
	entry->stats.last_time_usec = now.tv_usec;
	platform_mutex_unlock(entry->stats.mutex);
}

/**
 * of12_stats_flow_get_duration()
 */
void of12_stats_flow_get_duration(of12_flow_entry_t * entry)
{
	uint32_t nsec_now;
	
	platform_mutex_lock(entry->stats.mutex);
	
	nsec_now = entry->stats.last_time_usec*1000;
	
	entry->stats.duration_sec = entry->stats.last_time_sec - entry->stats.initial_time_sec;
	
	if(nsec_now >= entry->stats.initial_time_nsec)
	{
		entry->stats.duration_nsec = nsec_now - entry->stats.initial_time_nsec;
	}
	else
	{
		entry->stats.duration_nsec = OF12_STATS_NS_IN_A_SEC + nsec_now - entry->stats.initial_time_nsec;
		entry->stats.duration_sec--;
	}
	
	platform_mutex_unlock(entry->stats.mutex);
	
	return;
}

/**
 * of12_stats_flow_update
 * input arguments: bytes_rx, flow_entry
 */
inline void of12_stats_flow_inc(of12_flow_entry_t * entry,uint64_t bytes_rx)
{
	platform_atomic_inc64(&entry->stats.packet_count,entry->stats.mutex);
	platform_atomic_add64(&entry->stats.byte_count,&bytes_rx, entry->stats.mutex);
}

//Table Statistics functions
/**
 * sets the table statistics to zero
 * WARNING need to be atomic?
 */
void of12_stats_table_init(of12_flow_table_t * table)
{
	table->stats.lookup_count = 0;
	table->stats.matched_count = 0;
}

//NOTE this functions add too much overhead!
/**
 * of12_stats_table_lookup_update
 * input arguments: flow_table ...?
 */
inline void of12_stats_table_lookup_inc(of12_flow_table_t * table)
{
	platform_atomic_inc64(&table->stats.lookup_count,table->mutex);
}
/**
 * of12_stats_table_matched_update
 * input arguments: flow_table ...?
 */
inline void of12_stats_table_matches_inc(of12_flow_table_t * table)
{
	platform_atomic_inc64(&table->stats.matched_count,table->mutex);
}

//Aggregate Statistics functions
/**
 * of12_stats_aggregate_collect
 * WARNING no idea how to do that
 * When an aggregate flow statistics is received
 * we need to look for the entries that match  the specified
 * matches and aggregate all their statistics
 */
void of12_stats_aggregate_collect (of12_flow_entry_t *entry, of12_stats_aggregate_t *aggregate)
{
	aggregate->flow_count++;
	aggregate->byte_count += entry->stats.byte_count;
	aggregate->packet_count += entry->stats.packet_count;
	return;
}

//Port & Queue functions
void of12_stats_port_init(of12_stats_port_t *port_stats)
{
	port_stats->rx_packets = 0;
	port_stats->tx_packets = 0;
	port_stats->rx_bytes = 0;
	port_stats->tx_bytes = 0;
	
	if (0 == (port_stats->mutex = platform_mutex_init(NULL)))
	{
		// log error
		return;
	}
}

void of12_stats_port_destroy(of12_stats_port_t *port_stats)
{
	platform_mutex_destroy(port_stats->mutex);
}

inline void of12_stats_port_rx_packet_inc(of12_stats_port_t *port_stats, uint64_t bytes_rx)
{
	platform_atomic_inc64(&port_stats->rx_packets,port_stats->mutex);
	platform_atomic_add64(&port_stats->rx_bytes,&bytes_rx, port_stats->mutex);
}

inline void of12_stats_port_tx_packet_inc(of12_stats_port_t *port_stats, uint64_t bytes_tx)
{
	platform_atomic_inc64(&port_stats->tx_packets,port_stats->mutex);
	platform_atomic_add64(&port_stats->tx_bytes,&bytes_tx, port_stats->mutex);
}

/** TODO not yet implemented for ports
 * rx_drops
 * tx_drops
 * rx_errors
 * tx_errors
 * rx_align_err
 * rx_overrun_err
 * rc_crc_err
 * collisions
 */

void of12_stats_queue_init(of12_stats_queue_t *queue_stats)
{
	queue_stats->tx_bytes=0;
	queue_stats->tx_packets=0;
	queue_stats->tx_errors=0;
	if (0 == (queue_stats->mutex = platform_mutex_init(NULL)))
	{
		return;
	}
}

void of12_stats_queue_destroy(of12_stats_queue_t *queue_stats)
{
	platform_mutex_destroy(queue_stats->mutex);
}

inline void of12_stats_queue_tx_packet_inc(of12_stats_queue_t *queue_stats, uint64_t bytes)
{
	platform_atomic_inc64(&queue_stats->tx_packets, queue_stats->mutex);
	platform_atomic_add64(&queue_stats->tx_bytes, &bytes, queue_stats->mutex);
}

inline void of12_stats_queue_tx_errors_inc(of12_stats_queue_t *queue_stats)
{
	platform_atomic_inc64(&queue_stats->tx_errors, queue_stats->mutex);
}

/**
 * TODO  not yet implemented
 * of12_stats_group_update
 * of12_stats_bucket_update
 */
