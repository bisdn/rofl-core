#include "port_queue.h"

#include <string.h>


rofl_result_t port_queue_init(port_queue_t* queue, uint32_t id, char* name, uint16_t length, uint16_t min_rate, uint16_t max_rate){

	//Put 0 stats
	memset(&queue->stats,0,sizeof(queue_stats_t));

	//Init stats mutex
	queue->stats.mutex = platform_mutex_init(NULL);

	if(!queue->stats.mutex){
		return ROFL_FAILURE;
	}

	//Fill in values
	queue->set = true;
	queue->id = id;
	queue->length = length;
	strncpy(queue->name, name, PORT_QUEUE_MAX_LEN_NAME);
	queue->min_rate = min_rate;
	queue->max_rate = max_rate;

	queue->platform_queue_state = NULL;

	return ROFL_SUCCESS;
}

rofl_result_t port_queue_destroy(port_queue_t* queue){
	//Destroy
	platform_mutex_destroy(queue->stats.mutex);
	memset(queue,0,sizeof(port_queue_t));
	return ROFL_SUCCESS;
}

void port_queue_stats_inc(port_queue_t* queue, 
				uint64_t tx_packets,
				uint64_t tx_bytes,
				uint64_t overrun){

	//TODO: evaluate to use lock-less strategies (single threaded I/O subsystems)
	//or target platform specific atomic_incs
	platform_mutex_lock(queue->stats.mutex);
	
	//Do all the stuff
	queue->stats.tx_packets += tx_packets;
	queue->stats.tx_bytes += tx_bytes;
	queue->stats.overrun += overrun;

	platform_mutex_unlock(queue->stats.mutex);
}


