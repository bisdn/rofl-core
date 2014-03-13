#include "port_queue.h"
#include "platform/memory.h"

#include <string.h>


rofl_result_t __port_queue_init(port_queue_t* queue, uint32_t id, char* name, uint16_t length, uint16_t min_rate, uint16_t max_rate){

	//Put 0 stats
	platform_memset(&queue->stats,0,sizeof(queue_stats_t));

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

rofl_result_t __port_queue_destroy(port_queue_t* queue){
	//Destroy
	platform_mutex_destroy(queue->stats.mutex);
	platform_memset(queue,0,sizeof(port_queue_t));
	return ROFL_SUCCESS;
}
