#ifndef __OF12_TIMERSH__
#define __OF12_TIMERSH__

#include <stdlib.h>
#include <inttypes.h>
#include <sys/time.h>
#include "rofl.h" 
#include "../../../util/rofl_pipeline_utils.h"

/*
* OF12 Timers. Implementation assumes (and enforces) that ALL timers will expire in a multiple of OF12_TIMER_SLOT_MS ms
*/
#define OF12_TIMER_SLOT_MS 1000 //1s

//Flag to define the usage of static allocation of slots for the timers
#define OF12_TIMER_STATIC_ALLOCATION_SLOTS 1

#define OF12_TIMER_GROUPS_MAX 65536 //timeouts are given in a uint16_t=> 2^16

//fwd declarations
struct of12_pipeline;
struct of12_flow_entry;
struct of12_flow_table;
struct of12_timer_group;

typedef enum{
	HARD_TO=0,
	IDLE_TO=1,
}of12_timer_timeout_type_t;

struct of12_entry_timer{
	struct of12_flow_entry* entry;
	struct of12_timer_group* group;

	//linked list	
	struct of12_entry_timer* prev;
	struct of12_entry_timer* next;
	struct timeval time_last_update;		// time when the entry was last used (0 for hard timeouts)
	of12_timer_timeout_type_t type;
};
typedef struct of12_entry_timer of12_entry_timer_t;

struct of12_timers_info{
	uint32_t hard_timeout;
	uint32_t idle_timeout;
	
	of12_entry_timer_t * idle_timer_entry;
	of12_entry_timer_t * hard_timer_entry;
};
typedef struct of12_timers_info of12_timers_info_t;


struct of12_timer_list{
	unsigned int num_of_timers;
	of12_entry_timer_t* head;
	of12_entry_timer_t* tail;	
};
typedef struct of12_timer_list of12_timer_list_t;




struct of12_timer_group{
	uint64_t timeout; //Expiration time in ms (slot)	
	of12_timer_list_t list; //List of entries that expire at this timeout
	
#if ! OF12_TIMER_STATIC_ALLOCATION_SLOTS	
	struct of12_timer_group* prev; //linked list
	struct of12_timer_group* next;
#endif
	
};
typedef struct of12_timer_group of12_timer_group_t;

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

//Timer functions outside tu
rofl_result_t of12_add_timer(struct of12_flow_table* const table, struct of12_flow_entry* const entry);
void of12_process_pipeline_tables_timeout_expirations(const struct of12_pipeline* pipeline);
rofl_result_t of12_destroy_timer_entries(struct of12_flow_entry * entry);
void of12_dump_timers_structure(of12_timer_group_t * timer_group);
void of12_timer_group_static_init(struct of12_flow_table* table);
void of12_time_forward(uint64_t sec, uint64_t usec, struct timeval * time);
void of12_timer_update_entry(struct of12_flow_entry * flow_entry);
void of12_fill_new_timer_entry_info(struct of12_flow_entry * entry, uint32_t hard_timeout, uint32_t idle_timeout);
// public for testing
int of12_gettimeofday(struct timeval * tval, struct timezone * tzone);
uint64_t of12_get_expiration_time_slotted (uint32_t timeout,struct timeval *now);
inline uint64_t of12_get_time_ms(struct timeval *time);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_TIMERS
