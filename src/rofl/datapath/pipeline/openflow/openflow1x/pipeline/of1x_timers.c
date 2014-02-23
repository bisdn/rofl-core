#include "of1x_timers.h"
#include "of1x_pipeline.h"
#include "of1x_flow_table.h"
#include "of1x_flow_entry.h"

#include "../../../platform/memory.h"
#include "../../../platform/likely.h"
#include "../../../platform/timing.h"
#include "../../../util/logging.h"

static rofl_result_t __of1x_destroy_all_entries_from_timer_group(of1x_timer_group_t* tg, of1x_pipeline_t *const pipeline, unsigned int id);
#if ! OF1X_TIMER_STATIC_ALLOCATION_SLOTS
static of1x_timer_group_t* __of1x_dynamic_slot_search(of1x_flow_table_t* const table, uint64_t expiration_time);
#endif

/**
 * of1x_fill_new_timer_entry_info
 * initialize the values for a new the timer entry
 */
void __of1x_fill_new_timer_entry_info(of1x_flow_entry_t * entry, uint32_t hard_timeout, uint32_t idle_timeout){

	entry->timer_info.hard_timeout = hard_timeout;
	entry->timer_info.idle_timeout = idle_timeout;
	entry->timer_info.hard_timer_entry = NULL;
	entry->timer_info.idle_timer_entry = NULL;

}

/**
 * Returns true if timeval a is later than timeval b
 */
bool __of1x_time_is_later(struct timeval *a, struct timeval *b){
	if(a->tv_sec > b->tv_sec){
		return true;
	}
	
	if (a->tv_sec == b->tv_sec){
		if(a->tv_usec > b->tv_usec){
			return true;
		}
	}
	
	return false;
}

/**
 * of1x_dump_timers_structure
 * this function is ment to show the timer groups existing
 * and the entries related
 */
void __of1x_dump_timers_structure(of1x_timer_group_t * timer_group){

	of1x_timer_group_t * tg = timer_group;
	of1x_entry_timer_t * et;
	if(!tg)
	{
		ROFL_PIPELINE_DEBUG("Timer group list is empty\n");
		return;
	}
#if OF1X_TIMER_STATIC_ALLOCATION_SLOTS
	int i;
	for(i=0;i<OF1X_TIMER_GROUPS_MAX;i++)
	{
		ROFL_PIPELINE_DEBUG("timer group on position %p\n", &(tg[i]));
		ROFL_PIPELINE_DEBUG("	[%p] TO:%"PRIu64" Nent:%d h:%p t:%p\n",&(tg[i]), tg[i].timeout,
			tg[i].list.num_of_timers, tg[i].list.head, tg[i].list.tail);
		for(et=tg[i].list.head; et; et=et->next)
			ROFL_PIPELINE_DEBUG("	[%p] fe:%p prev:%p next:%p tg:%p\n", et,et->entry, et->prev, et->next, et->group);
	}
	
#else
	if(tg->prev)
		ROFL_PIPELINE_DEBUG("NOT the first group!!\n");
	for(;tg!=NULL;tg=tg->next)
	{
		ROFL_PIPELINE_DEBUG("timer group on position %p\n", tg);
		ROFL_PIPELINE_DEBUG("	[%p] TO:%"PRIu64" next:%p prev:%p Nent:%d h:%p t:%p\n",tg, tg->timeout, tg->next, tg->prev,
			tg->list.num_of_timers, tg->list.head, tg->list.tail);
		for(et=tg->list.head; et; et=et->next)
			ROFL_PIPELINE_DEBUG("	[%p] fe:%p prev:%p next:%p tg:%p\n", et,et->entry, et->prev, et->next, et->group);
	}
#endif
}

/**
 * transforms the timeval to a single uint64_t unit time in miliseconds
 */
inline uint64_t __of1x_get_time_ms(struct timeval *time){

	return time->tv_sec*1000+time->tv_usec/1000;
}

/**
 * of1x_get_expiration_time_slotted
 */
uint64_t __of1x_get_expiration_time_slotted (uint32_t timeout,struct timeval *now){

	uint64_t expiration_time_ms;
	// exact expiration time calculation
	//WARNING it could happen that the calculated value does not fit in a uint64_t
	// then we might have to use floats.
	expiration_time_ms = (now->tv_sec+timeout)*1000+now->tv_usec/1000;
	// now it must be set in one of the slots
	//NOTE we are selecting the next slot so we round up the expiration time
	if(expiration_time_ms%OF1X_TIMER_SLOT_MS)
		return (expiration_time_ms/OF1X_TIMER_SLOT_MS + 1)*OF1X_TIMER_SLOT_MS;
	else
		return expiration_time_ms;
}

#if OF1X_TIMER_STATIC_ALLOCATION_SLOTS
/**
 * of1x_timer_group_static_init
 * initializes the timeout values
 * initializes the values for the entry lists
 */
void __of1x_timer_group_static_init(of1x_flow_table_t* table){

	int i;
	struct timeval now;
	uint64_t time_next_slot;
	platform_gettimeofday(&now);
	time_next_slot = __of1x_get_expiration_time_slotted(0,&now);
	for(i=0; i<OF1X_TIMER_GROUPS_MAX;i++)
	{
		table->timers[i].timeout=time_next_slot+OF1X_TIMER_SLOT_MS*i;
		table->timers[i].list.num_of_timers = 0;
		table->timers[i].list.head = table->timers[i].list.tail = NULL;
	}
	table->current_timer_group=0;
}

/**
 * if we are using static slots, we need to rotate the memory every time that a slot passes. steps:
 * - if there are entries we must erase them
 * - we must set the new timeout
 * - we must update the pointer to the next timer group
 */
static void __of1x_timer_group_rotate(of1x_pipeline_t *const pipeline, of1x_timer_group_t *tg , unsigned int id_table)
{
	if(tg->list.head)
	{
		//erase_all_entries;
		if(__of1x_destroy_all_entries_from_timer_group(tg, pipeline, id_table)!=ROFL_SUCCESS)
			ROFL_PIPELINE_DEBUG("ERROR in destroying timer group\n");
	}
	tg->timeout += OF1X_TIMER_SLOT_MS*OF1X_TIMER_GROUPS_MAX;
	tg->list.num_of_timers=0;
	tg->list.head = tg->list.tail = NULL;

	pipeline->tables[id_table].current_timer_group++;
	if(pipeline->tables[id_table].current_timer_group>=OF1X_TIMER_GROUPS_MAX)
		pipeline->tables[id_table].current_timer_group=0;
}

#else
/**
 * of1x_timer_group_init creates a new timer group and places it in
 * between tg_next and tg_prev, ordered by timeout values.
 * This function is NOT thread safe, you must lock the timer before.
 */
static of1x_timer_group_t* __of1x_timer_group_init(uint64_t timeout, of1x_timer_group_t* tg_next, of1x_timer_group_t* tg_prev, of1x_flow_table_t* table)
{
	// create the new timer_group
	of1x_timer_group_t* new_group;
	new_group = platform_malloc_shared(sizeof(of1x_timer_group_t));
	if(unlikely(new_group == NULL))
	{
		ROFL_PIPELINE_DEBUG("<%s:%d> Error allocating memory\n",__func__,__LINE__);
		return NULL;
	}
	new_group->timeout = timeout;
	new_group->list.num_of_timers=0;
	new_group->list.head=NULL;
	new_group->list.tail=NULL;
	
	// place the timer group
	new_group->prev=tg_prev;
	new_group->next=tg_next;
	
	//if there is a node afterwards we place the new one before
	if (tg_next) tg_next->prev=new_group;
	//if there is a node forewards we place the new one before
	if (tg_prev) tg_prev->next=new_group;
	
	if(table->timers == tg_next)
		table->timers = new_group;
	
	return new_group;
}

/**
 * of1x_destroy_timer_group removes a timer_group and frees the memory
 * This function is NOT thread safe, you must lock the timer before.
 */
static void of1x_destroy_timer_group(of1x_timer_group_t* tg, of1x_flow_table_t* table)
{
	if(tg->prev)
		(tg->prev)->next=tg->next;
	else
		if(table->timers == tg)
			table->timers = tg->next;
	if(tg->next)
		(tg->next)->prev=tg->prev;
	platform_free_shared(tg);
	tg = NULL;
}
#endif

/**
 * of1x_entry_timer_init
 * adds a new entry in the list
 */
static of1x_entry_timer_t* __of1x_entry_timer_init(of1x_timer_group_t* tg, of1x_flow_entry_t* entry, of1x_timer_timeout_type_t is_idle)
{
	of1x_entry_timer_t* new_entry;
	new_entry = platform_malloc_shared(sizeof(of1x_entry_timer_t));
	if(unlikely(new_entry==NULL))
	{
		ROFL_PIPELINE_DEBUG("<%s:%d> Error allocating memory\n",__func__,__LINE__);
		return NULL;
	}
	new_entry->entry = entry;
	new_entry->group = tg;
	
	// we add the new entries at the end
	new_entry->next=NULL;
	
	// we check if it is the first entry.
	if(tg->list.tail) //if it is not
	{
		new_entry->prev=tg->list.tail;
		//update the pointer OF the last entry on the list
		tg->list.tail->next=new_entry;
	}
	else //if it is the first one
	{
		new_entry->prev=NULL;
		//normally this should also apply:
		if(!tg->list.head)
			tg->list.head = new_entry;
	}
	
	// update the pointer TO the last entry of the list
	tg->list.tail = new_entry;
	// update the entries counter
	tg->list.num_of_timers++;
	
	new_entry->type=is_idle;
	
	if(is_idle)
		entry->timer_info.idle_timer_entry=new_entry;
	else
		entry->timer_info.hard_timer_entry=new_entry;
	
	return new_entry;
}

/**
 * of1x_destroy_single_timer_entry_clean
 * when the time comes the list of entries must be erased from the list
 * This is ment to be used when a single entry is deleted.
 */
static rofl_result_t __of1x_destroy_single_timer_entry_clean(of1x_entry_timer_t* entry, of1x_flow_table_t * table)
{	
	if(likely(entry!=NULL))
	{
		if(!entry->next && !entry->prev) // this is the only entry
		{
			entry->group->list.head=NULL;
			entry->group->list.tail=NULL;
		}
		else if(!entry->prev) //is the first entry
		{
			entry->group->list.head=entry->next;
			entry->next->prev = NULL;
		}
		else if(!entry->next) //last entry
		{
			entry->group->list.tail=entry->prev;
			entry->prev->next = NULL;
		}
		else
		{
			entry->next->prev = entry->prev;
			entry->prev->next = entry->next;
		}
		
		entry->group->list.num_of_timers--;
#if ! OF1X_TIMER_STATIC_ALLOCATION_SLOTS
		//we need to check if this entry was the last one and delete the timer group
		if(entry->group->list.num_of_timers == 0)
			__of1x_destroy_timer_group(entry->group,table);
#endif
		platform_free_shared(entry);
		entry = NULL;

		return ROFL_SUCCESS;

	}else{
		ROFL_PIPELINE_DEBUG("<%s:%d> Not a valid timer entry %p\n",__func__,__LINE__,entry);
		return ROFL_FAILURE;
	}
}

/**
 * of1x_destroy_timer_entry
 * When a flow entry is removed from the table this function
 * will be called in order to remove the timer enrties associated as well
 * (this function will only be called from outside of the timers structure
 * and assumes that the mutex is already locked.)
 */
rofl_result_t __of1x_destroy_timer_entries(of1x_flow_entry_t * entry){
	// We need to erase both hard and idle timer_entries

	if(unlikely(entry->table==NULL))
		return ROFL_FAILURE;

	if(entry->timer_info.hard_timeout){
		if(__of1x_destroy_single_timer_entry_clean(entry->timer_info.hard_timer_entry, entry->table)!=ROFL_SUCCESS)
			return ROFL_FAILURE;
		entry->timer_info.hard_timer_entry = NULL;
	}
		
	if(entry->timer_info.idle_timeout){
		if(__of1x_destroy_single_timer_entry_clean(entry->timer_info.idle_timer_entry, entry->table)!=ROFL_SUCCESS)
			return ROFL_FAILURE;
		entry->timer_info.idle_timer_entry = NULL;
	}
		
#if DEBUG_NO_REAL_PIPE
	__of1x_fill_new_timer_entry_info(entry,0,0);
#endif
		
	return ROFL_SUCCESS;
}

/**
 * of1x_reschedule_idle_timer
 * check if there is the need of re-scheduling an idle timer
 */
static rofl_result_t __of1x_reschedule_idle_timer(of1x_entry_timer_t * entry_timer, of1x_pipeline_t *const pipeline, unsigned int id_table)
{
	struct timeval system_time;
	uint64_t expiration_time;
	
	if(entry_timer->entry->stats.packet_count == entry_timer->entry->timer_info.last_packet_count)
	{
	// timeout expired so no need to reschedule !!! we have to delete the entry
#ifdef DEBUG_NO_REAL_PIPE
		ROFL_PIPELINE_DEBUG("NOT erasing real entries of table \n");
		__of1x_fill_new_timer_entry_info(entry_timer->entry,0,0);
		//we need to destroy the entries
		__of1x_destroy_timer_entries(entry_timer->entry);
#else
		__of1x_remove_specific_flow_entry_table(pipeline, id_table, entry_timer->entry, OF1X_FLOW_REMOVE_IDLE_TIMEOUT, MUTEX_ALREADY_ACQUIRED_BY_TIMER_EXPIRATION);
#endif
		return ROFL_SUCCESS;
	}
	
	entry_timer->entry->timer_info.last_packet_count = entry_timer->entry->stats.packet_count;
	//NOTE we calculate the new time of expiration from the checking time and not from the last time it was used (less accurate and more efficient)
	platform_gettimeofday(&system_time);
	expiration_time = __of1x_get_expiration_time_slotted(entry_timer->entry->timer_info.idle_timeout, &system_time);
	
#if OF1X_TIMER_STATIC_ALLOCATION_SLOTS
	
	int slot_delta = expiration_time - pipeline->tables[id_table].timers[pipeline->tables[id_table].current_timer_group].timeout; //ms
	int slot_position = (pipeline->tables[id_table].current_timer_group + slot_delta/OF1X_TIMER_SLOT_MS) % OF1X_TIMER_GROUPS_MAX;
	if(__of1x_entry_timer_init(&(pipeline->tables[id_table].timers[slot_position]), entry_timer->entry, IDLE_TO)==NULL)
		return ROFL_FAILURE;
#else
		
	of1x_timer_group_t * tg_iterator = __of1x_dynamic_slot_search(pipeline->tables[id_table], expiration_time);
	if(tg_iterator==NULL)
		return ROFL_FAILURE;
	// add entry to this group. new_group.list->num_of_timers++; ...
	if(__of1x_entry_timer_init(tg_iterator, entry_timer->entry, IDLE_TO) == NULL)
		return ROFL_FAILURE;
#endif
		
	//TODO delete timer_entry that has been rescheduled:
	__of1x_destroy_single_timer_entry_clean(entry_timer, &pipeline->tables[id_table]);
	
	return ROFL_SUCCESS;
}

/**
 * of1x_destroy_all_entries_from_timer_group()
 * implements for loop that destroys the complete list of entries
 * This is ment to be used when a timer expires and we want to delete
 * all the entries of the group and the group itself
 */
static rofl_result_t __of1x_destroy_all_entries_from_timer_group(of1x_timer_group_t* tg, of1x_pipeline_t *const pipeline, unsigned int id_table)
{
	of1x_entry_timer_t* entry_iterator, *next/*, *prev*/;
	if(tg->list.num_of_timers>0 && tg->list.head)
	{
		for(entry_iterator = tg->list.head; entry_iterator; entry_iterator=next)
		{
			next = entry_iterator->next;
			
			//NOTE actual removal of timer_entries is done in the destruction of the entry
			
			if(entry_iterator->type == IDLE_TO)
			{
				if(__of1x_reschedule_idle_timer(entry_iterator, pipeline, id_table)!=ROFL_SUCCESS)
					return ROFL_FAILURE;
			}
			else
			{
#ifdef DEBUG_NO_REAL_PIPE
				ROFL_PIPELINE_DEBUG("NOT erasing real entries of table \n");
				__of1x_fill_new_timer_entry_info(entry_iterator->entry,0,0);
				//we delete the enrty_timer form outside
				__of1x_destroy_timer_entries(entry_iterator->entry);
#else
				//ROFL_PIPELINE_DEBUG("Erasing real entries of table \n"); //NOTE DELETE
				__of1x_remove_specific_flow_entry_table(pipeline, id_table,entry_iterator->entry, OF1X_FLOW_REMOVE_HARD_TIMEOUT, MUTEX_ALREADY_ACQUIRED_BY_TIMER_EXPIRATION);
#endif
			}
		}
	}
	return ROFL_SUCCESS;
}

#if ! OF1X_TIMER_STATIC_ALLOCATION_SLOTS
/**
 * of1x_dynamic_slot_search
 * extraction of a function used to search a timer group with a specific timeout
 * and if it does not exist create it.
 */
static of1x_timer_group_t * of1x_dynamic_slot_search(of1x_flow_table_t* const table, uint64_t expiration_time)
{
	of1x_timer_group_t* tg_iterator;
	//Determine the slot, check if is already defined, if not allocate
	//We search the timer group with the timeout corresponding to the expiration_time calculated
	for(tg_iterator = table->timers; tg_iterator && tg_iterator->timeout<expiration_time && tg_iterator->next; tg_iterator=tg_iterator->next);
	// After that we expect 3 different situations:
	
	if(!tg_iterator)
	{
		//the list is empty, we sould create the first group
		table->timers = __of1x_timer_group_init(expiration_time,NULL,NULL, table);
		if(table->timers == NULL)
			return NULL;

		tg_iterator = table->timers;
	}
	else if(tg_iterator->timeout>expiration_time)
	{
		// Create new group and allocate it before the current group
		tg_iterator = __of1x_timer_group_init(expiration_time,tg_iterator,tg_iterator->prev, table);
		if(tg_iterator == NULL)
			return NULL;
	}
	else if(tg_iterator->timeout < expiration_time && !tg_iterator->next)
	{
		// Create new group and allocate it after the current group
		tg_iterator = __of1x_timer_group_init(expiration_time,NULL, tg_iterator, table);
		if(tg_iterator == NULL)
			return NULL;
	}
	else
	{
		if (!(tg_iterator->timeout==expiration_time))
		{
			// Unexpected outcome ...
			ROFL_PIPELINE_DEBUG("<%s:%d> No proper position for this entry found\n",__func__, __LINE__);
			return NULL;
		}
	}
	
	return tg_iterator;
}
#endif

static rofl_result_t __of1x_add_single_timer(of1x_flow_table_t* const table, const uint32_t timeout, of1x_flow_entry_t* entry, of1x_timer_timeout_type_t is_idle)
{
	uint64_t expiration_time;
	struct timeval now;
	platform_gettimeofday(&now);
	expiration_time = __of1x_get_expiration_time_slotted(timeout, &now);
#if OF1X_TIMER_STATIC_ALLOCATION_SLOTS
	// add entries to the position tc + hard_timeout_s
	if(timeout > OF1X_TIMER_GROUPS_MAX)
	{
		ROFL_PIPELINE_DEBUG("Timeout value excedded maximum value (hto=%d, MAX=%d)\n", timeout,OF1X_TIMER_GROUPS_MAX);
		return ROFL_FAILURE;
	}
	
	
	int slot_delta = expiration_time - table->timers[table->current_timer_group].timeout; //ms
	
	//NOTE we allocate the timer in the next slot rounding up:
	//so the actual expiration time will be a value in [hard_timeout,hard_timeout+OF1X_TIMER_SLOT_MS)
	int slot_position = (table->current_timer_group+(slot_delta/OF1X_TIMER_SLOT_MS))%(OF1X_TIMER_GROUPS_MAX);
	if(__of1x_entry_timer_init(&(table->timers[slot_position]), entry, is_idle)==NULL)
		return ROFL_FAILURE;

#else
	
	of1x_timer_group_t* tg_iterator = __of1x_dynamic_slot_search(table, expiration_time);
	if (tg_iterator==NULL)
		return ROFL_FAILURE;
	
	// add entry to this group. new_group.list->num_of_timers++; ...
	if(__of1x_entry_timer_init(tg_iterator, entry, is_idle, NULL) == NULL)
		return ROFL_FAILURE;
	
#endif
	return ROFL_SUCCESS;
}

//Add timer to a table
rofl_result_t __of1x_add_timer(of1x_flow_table_t* const table, of1x_flow_entry_t* const entry){
	rofl_result_t res;
	//NOTE we don't use that lock because this is only called from of1x_add_flow_entry...()
	
	if(entry->timer_info.idle_timeout)
	{
		res = __of1x_add_single_timer(table, entry->timer_info.idle_timeout, entry, IDLE_TO); //is_idle = 1
		if(res == ROFL_FAILURE)
		{
			return ROFL_FAILURE;
		}
	}
	if(entry->timer_info.hard_timeout)
	{
		res = __of1x_add_single_timer(table, entry->timer_info.hard_timeout, entry, HARD_TO); //is_idle = 0
		if(res == ROFL_FAILURE)
		{
			return ROFL_FAILURE;
		}
	}
	
	return ROFL_SUCCESS;
}

void __of1x_process_pipeline_tables_timeout_expirations(of1x_pipeline_t *const pipeline){

	unsigned int i;
	
	struct timeval system_time;
	platform_gettimeofday(&system_time);
	uint64_t now = __of1x_get_time_ms(&system_time);

	for(i=0;i<pipeline->num_of_tables;i++)
	{
		of1x_flow_table_t* table = &pipeline->tables[i];
		platform_mutex_lock(table->mutex);
#if OF1X_TIMER_STATIC_ALLOCATION_SLOTS
		while(table->timers[table->current_timer_group].timeout<=now)
		{
			//rotate the timers
			__of1x_timer_group_rotate(pipeline,&(table->timers[table->current_timer_group]),i);
		}
#else	
		of1x_timer_group_t* slot_it, *next;
		for(slot_it=table->timers; slot_it; slot_it=next)
		{
			if(now<slot_it->timeout) //Current slot time > time_now. We are done
				break;
			//remove all entries and the timer group.
			if(__of1x_destroy_all_entries_from_timer_group(slot_it, table)!=ROFL_SUCCESS)
			{
				ROFL_PIPELINE_DEBUG("Error while destroying timer group\n");
				platform_mutex_unlock(table->mutex);
				return;
			}
			next = slot_it->next;
			if(slot_it)
				__of1x_destroy_timer_group(slot_it, table);
		}		
#endif
		platform_mutex_unlock(table->mutex);
	}
	return;
}
