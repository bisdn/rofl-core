/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
* @file port_queue.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Port queue abstraction
*/

#ifndef __PORT_QUEUE_H__
#define __PORT_QUEUE_H__

#include <stdbool.h>
#include <inttypes.h>

#include "rofl.h"
#include "platform/lock.h"

#define PORT_QUEUE_MAX_LEN_NAME 32

//Opaque platform queue state (to be used, maybe, for platform hooks)
typedef void platform_queue_state_t;

/**
* @brief Queue stats 
* @ingroup core
*/
typedef struct queue_stats {
	uint64_t tx_packets;	/* Number of transmitted packets by the queue. */
	uint64_t tx_bytes;	/* Number of transmitted bytes by the queue. */
	uint64_t overrun;	/* Number of packets dropped due to overrun. */
	
	//Mutex for statistics
	platform_mutex_t* mutex;
}queue_stats_t;


/**
* @brief Switch queue abstraction 
* @ingroup core
*
* The library does not supose any special meanining over the queue id
* (e.g. queue_id 1 has higher priority than queue_id 0). Most implementations
* though use the library being 0 best-effort and the highest the number the
* highest priority the queue has.
*/
typedef struct port_queue{

	bool set; //Signal if queue is set (running or used)
	
	/**
	* Queue id
	*/
	uint32_t id; 
		
	/**
	* Queue  name
	*/
	char name[PORT_QUEUE_MAX_LEN_NAME];

	/**
	* Length of the queue (slot num.)
	*/
	uint16_t length;

	/**
	* Minimum rate (0 when unknown)
	*/
	uint16_t min_rate;

	/**
	* Maximum rate (0 when unknown)
	*/
	uint16_t max_rate;

	/**
	* Queue statistics
	*/
	queue_stats_t stats;

	/* Opaque platform queue specific extra state */
	platform_queue_state_t* platform_queue_state; 
}port_queue_t;


/* 
* Functions
*/

//C++ extern C
ROFL_BEGIN_DECLS

/**
* @brief Increments atomically all the statistics of the queue; shall be used by queues on TX. 
* Fill in with 0 the ones that should
* be left untouched.
* @ingroup  mgmt
*/
void port_queue_stats_inc(port_queue_t* queue, 
				uint64_t tx_packets,
				uint64_t tx_bytes,
				uint64_t overrun);


/*
* @brief Init a port_queue structure
* @ingroup  mgmt
*/
rofl_result_t __port_queue_init(port_queue_t* queue, uint32_t id, char* name, uint16_t length, uint16_t min_rate, uint16_t max_rate);

/*
* @brief Destroy a port_queue structure
* @ingroup  mgmt
*/
rofl_result_t __port_queue_destroy(port_queue_t* queue);

//C++ extern C
ROFL_END_DECLS

#endif //PORT_QUEUE
