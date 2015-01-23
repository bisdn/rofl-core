/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_TIMERSH__
#define __OF1X_TIMERSH__

#include <stdlib.h>
#include <inttypes.h>
#include <sys/time.h>
#include "rofl.h"

/**
* @file of1x_timers.h
* @author Victor Alvarez<victor.alvarez (at) bisdn.de>
* @brief OpenFlow v1.0, 1.2 and 1.3.2 timers subsystem
*
* @warning This is an internal subsystem and should never be used
* outside the pipeline
*/

/*
* OF1X Timers. Implementation assumes (and enforces) that ALL timers will expire in a multiple of OF1X_TIMER_SLOT_MS ms
*/
#define OF1X_TIMER_SLOT_MS 1000 //1s

//Flag to define the usage of static allocation of slots for the timers
#ifdef OF1X_TIMER_DYNAMIC_ALLOCATION_SLOTS
	#error "Dynamic timers are not yet supported."
	#define OF1X_TIMER_STATIC_ALLOCATION_SLOTS 0
#else
	#define OF1X_TIMER_STATIC_ALLOCATION_SLOTS 1
#endif

#define OF1X_TIMER_GROUPS_MAX 65536 //timeouts are given in a uint16_t=> 2^16

//fwd declarations
struct of1x_pipeline;
struct of1x_flow_entry;
struct of1x_flow_table;
struct of1x_timer_group;

typedef enum{
	HARD_TO=0,
	IDLE_TO=1,
}of1x_timer_timeout_type_t;

typedef struct of1x_entry_timer{
	struct of1x_flow_entry* entry;
	struct of1x_timer_group* group;

	//linked list
	struct of1x_entry_timer* prev;
	struct of1x_entry_timer* next;
	of1x_timer_timeout_type_t type;
}of1x_entry_timer_t;

typedef struct of1x_timers_info{
	uint32_t hard_timeout;
	uint32_t idle_timeout;

	// time when the entry was last used (0 for hard timeouts)
	//struct timeval time_last_update;
	//checks the number of packets that hit the entry to implement a lazy expiration
	// less accurate but more eficient
	uint64_t last_packet_count;

	of1x_entry_timer_t * idle_timer_entry;
	of1x_entry_timer_t * hard_timer_entry;

}of1x_timers_info_t;

typedef struct of1x_timer_list{
	unsigned int num_of_timers;
	of1x_entry_timer_t* head;
	of1x_entry_timer_t* tail;
}of1x_timer_list_t;



typedef struct of1x_timer_group{
	uint64_t timeout; //Expiration time in ms (slot)
	of1x_timer_list_t list; //List of entries that expire at this timeout

#if ! OF1X_TIMER_STATIC_ALLOCATION_SLOTS
	struct of1x_timer_group* prev; //linked list
	struct of1x_timer_group* next;
#endif

}of1x_timer_group_t;

//C++ extern C
ROFL_BEGIN_DECLS

//Timer functions outside tu
rofl_result_t __of1x_add_timer(struct of1x_flow_table* const table, struct of1x_flow_entry* const entry);
rofl_result_t __of1x_destroy_timer_entries(struct of1x_flow_entry * entry);

void __of1x_process_pipeline_tables_timeout_expirations(struct of1x_pipeline *const pipeline);

void __of1x_dump_timers_structure(of1x_timer_group_t * timer_group);
rofl_result_t __of1x_timer_group_static_init(struct of1x_flow_table* table);
void __of1x_timer_group_static_destroy(struct of1x_flow_table* table);
//void __of1x_timer_update_entry(struct of1x_flow_entry * flow_entry, struct timeval ts);
void __of1x_fill_new_timer_entry_info(struct of1x_flow_entry * entry, uint32_t hard_timeout, uint32_t idle_timeout);
// public for testing
uint64_t __of1x_get_expiration_time_slotted (uint32_t timeout,struct timeval *now);
inline uint64_t __of1x_get_time_ms(struct timeval *time);

static inline
void __of1x_reset_last_packet_count_idle_timeout(of1x_timers_info_t *timer_info){
	timer_info->last_packet_count=0;
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_TIMERS
