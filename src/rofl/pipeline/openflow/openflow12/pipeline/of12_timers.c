#include "of12_timers.h"
#include "of12_pipeline.h"
#include "of12_flow_table.h"
#include "of12_flow_entry.h"

#include "../../../platform/memory.h"

#include <stdio.h>

static rofl_result_t of12_destroy_all_entries_from_timer_group(of12_timer_group_t* tg, of12_flow_table_t * table);
#if ! OF12_TIMER_STATIC_ALLOCATION_SLOTS
static of12_timer_group_t* of12_dynamic_slot_search(of12_flow_table_t* const table, uint64_t expiration_time);
#endif

/**
 * of12_fill_new_timer_entry_info
 * initialize the values for a new the timer entry
 */
void of12_fill_new_timer_entry_info(of12_flow_entry_t * entry, uint32_t hard_timeout, uint32_t idle_timeout)
{
	entry->timer_info.hard_timeout = hard_timeout;
	entry->timer_info.idle_timeout = idle_timeout;
	entry->timer_info.hard_timer_entry = NULL;
	entry->timer_info.idle_timer_entry = NULL;
}

void of12_time_forward(uint64_t sec, uint64_t usec, struct timeval * time)
{
	static uint64_t abs_sec = 0, abs_usec = 0;
	abs_sec+=sec;
	abs_usec+=usec;
	if(abs_usec > 1000000)
	{
		abs_usec-=1000000;
		abs_sec++;
	}
	if(time)
	{
		time->tv_sec=abs_sec;
		time->tv_usec=abs_usec;
	}
	return;
}

/**
 * of12_gettimeofday wrapper for the system time
 */
int of12_gettimeofday(struct timeval * tval, struct timezone * tzone)
{
#ifdef TIMERS_FAKE_TIME
	of12_time_forward(0,0,tval);
	fprintf(stderr,"NOT usig real system time (%lu:%lu)\n", tval->tv_sec, tval->tv_usec);
	return 0;
#else
	//gettimeofday(tval,tzone);
	//fprintf(stderr,"<%s:%d> Time %lu:%lu\n",__func__,__LINE__,tval->tv_sec, tval->tv_usec);
	//return 0;
	return gettimeofday(tval,tzone);
#endif
}

/**
 * of12_dump_timers_structure
 * this function is ment to show the timer groups existing
 * and the entries related
 */
void of12_dump_timers_structure(of12_timer_group_t * timer_group)
{
	of12_timer_group_t * tg = timer_group;
	of12_entry_timer_t * et;
	if(!tg)
	{
		fprintf(stderr,"Timer group list is empty\n");
		return;
	}
#if OF12_TIMER_STATIC_ALLOCATION_SLOTS
	int i;
	for(i=0;i<OF12_TIMER_GROUPS_MAX;i++)
	{
		fprintf(stderr,"timer group on position %p\n", &(tg[i]));
		fprintf(stderr,"	[%p] TO:%"PRIu64" Nent:%d h:%p t:%p\n",&(tg[i]), tg[i].timeout,
			tg[i].list.num_of_timers, tg[i].list.head, tg[i].list.tail);
		for(et=tg[i].list.head; et; et=et->next)
			fprintf(stderr,"	[%p] fe:%p prev:%p next:%p tg:%p\n", et,et->entry, et->prev, et->next, et->group);
	}
	
#else
	if(tg->prev)
		fprintf(stderr,"NOT the first group!!\n");
	for(;tg!=NULL;tg=tg->next)
	{
		fprintf(stderr,"timer group on position %p\n", tg);
		fprintf(stderr,"	[%p] TO:%"PRIu64" next:%p prev:%p Nent:%d h:%p t:%p\n",tg, tg->timeout, tg->next, tg->prev,
			tg->list.num_of_timers, tg->list.head, tg->list.tail);
		for(et=tg->list.head; et; et=et->next)
			fprintf(stderr,"	[%p] fe:%p prev:%p next:%p tg:%p\n", et,et->entry, et->prev, et->next, et->group);
	}
#endif
}

/**
 * transforms the timeval to a single uint64_t unit time in miliseconds
 */
inline uint64_t of12_get_time_ms(struct timeval *time)
{
	return time->tv_sec*1000+time->tv_usec/1000;
}

/**
 * of12_get_expiration_time_slotted
 */
uint64_t of12_get_expiration_time_slotted (uint32_t timeout,struct timeval *now)
{
	uint64_t expiration_time_ms;
	// exact expiration time calculation
	//WARNING it could happen that the calculated value does not fit in a uint64_t
	// then we might have to use floats.
	expiration_time_ms = (now->tv_sec+timeout)*1000+now->tv_usec/1000;
	// now it must be set in one of the slots
	//NOTE we are selecting the next slot so we round up the expiration time
	if(expiration_time_ms%OF12_TIMER_SLOT_MS)
		return (expiration_time_ms/OF12_TIMER_SLOT_MS + 1)*OF12_TIMER_SLOT_MS;
	else
		return expiration_time_ms;
}

#if OF12_TIMER_STATIC_ALLOCATION_SLOTS
/**
 * of12_timer_group_static_init
 * initializes the timeout values
 * initializes the values for the entry lists
 */
void of12_timer_group_static_init(of12_flow_table_t* table)
{
	int i;
	struct timeval now;
	uint64_t time_next_slot;
	of12_gettimeofday(&now,NULL);
	time_next_slot = of12_get_expiration_time_slotted(0,&now);
	for(i=0; i<OF12_TIMER_GROUPS_MAX;i++)
	{
		table->timers[i].timeout=time_next_slot+OF12_TIMER_SLOT_MS*i;
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
static void of12_timer_group_rotate(of12_timer_group_t *tg, of12_flow_table_t *table)
{
	if(tg->list.head)
	{
		//erase_all_entries;
		if(of12_destroy_all_entries_from_timer_group(tg, table)!=ROFL_SUCCESS)
			fprintf(stderr,"ERROR in destroying timer group\n");
	}
	tg->timeout += OF12_TIMER_SLOT_MS*OF12_TIMER_GROUPS_MAX;
	tg->list.num_of_timers=0;
	tg->list.head = tg->list.tail = NULL;

	table->current_timer_group++;
	if(table->current_timer_group>=OF12_TIMER_GROUPS_MAX)
		table->current_timer_group=0;
}

#else
/**
 * of12_timer_group_init creates a new timer group and places it in
 * between tg_next and tg_prev, ordered by timeout values.
 * This function is NOT thread safe, you must lock the timer before.
 */
static of12_timer_group_t* of12_timer_group_init(uint64_t timeout, of12_timer_group_t* tg_next, of12_timer_group_t* tg_prev, of12_flow_table_t* table)
{
	// create the new timer_group
	of12_timer_group_t* new_group;
	new_group = cutil_malloc_shared(sizeof(of12_timer_group_t));
	if(new_group == NULL)
	{
		fprintf(stderr,"<%s:%d> Error allocating memory\n",__func__,__LINE__);
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

#if ! OF12_TIMER_STATIC_ALLOCATION_SLOTS
	if(table->timers == tg_next)
		table->timers = new_group;
#endif
	return new_group;
}

/**
 * of12_destroy_timer_group removes a timer_group and frees the memory
 * This function is NOT thread safe, you must lock the timer before.
 */
static void of12_destroy_timer_group(of12_timer_group_t* tg, of12_flow_table_t* table)
{
	if(tg->prev)
		(tg->prev)->next=tg->next;
	else
		if(table->timers == tg)
			table->timers = tg->next;
	if(tg->next)
		(tg->next)->prev=tg->prev;
	cutil_free_shared(tg);
	tg = NULL;
}
#endif

/**
 * of12_entry_timer_init
 * adds a new entry in the list
 */
static of12_entry_timer_t* of12_entry_timer_init(of12_timer_group_t* tg, of12_flow_entry_t* entry, of12_timer_timeout_type_t is_idle, struct timeval * last_update)
{
	of12_entry_timer_t* new_entry;
	new_entry = cutil_malloc_shared(sizeof(of12_entry_timer_t));
	if(new_entry==NULL)
	{
		fprintf(stderr,"<%s:%d> Error allocating memory\n",__func__,__LINE__);
		return NULL;
	}
	new_entry->entry = entry;
	new_entry->group = tg;
	
	if(last_update)
	{
		new_entry->time_last_update.tv_sec = last_update->tv_sec;
		new_entry->time_last_update.tv_usec = last_update->tv_usec;
	}
	else
	{
		new_entry->time_last_update.tv_sec = new_entry->time_last_update.tv_usec = 0;
	}	
	
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
	
	if(is_idle)
		entry->timer_info.idle_timer_entry=new_entry;
	else
		entry->timer_info.hard_timer_entry=new_entry;
	
	new_entry->type=is_idle;
	
	return new_entry;
}

/**
 * of12_destroy_single_timer_entry_clean
 * when the time comes the list of entries must be erased from the list
 * This is ment to be used when a single entry is deleted.
 */
//NOTE is this function responsible for the destruction of the flow entry? I guess not
static rofl_result_t of12_destroy_single_timer_entry_clean(of12_entry_timer_t* entry, of12_flow_table_t * table)
{	
	if(entry)
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
#if ! OF12_TIMER_STATIC_ALLOCATION_SLOTS
		//we need to check if this entry was the last one and delete the timer group
		if(entry->group->list.num_of_timers == 0)
			of12_destroy_timer_group(entry->group,table);
#endif
		cutil_free_shared(entry);
		entry = NULL;

		return ROFL_SUCCESS;

	}else{
		fprintf(stderr,"<%s:%d> Not a valid timer entry %p\n",__func__,__LINE__,entry);
		return ROFL_FAILURE;
	}
}

/**
 * of12_destroy_timer_entry
 * When a flow entry is removed from the table this function
 * will be called in order to remove the timer enrties associated as well
 * (this function will only be called from outside of the timers structure
 * and assumes that the mutex is already locked.)
 */
rofl_result_t of12_destroy_timer_entries(of12_flow_entry_t * entry){
	// We need to erase both hard and idle timer_entries
	// NOTE do I need to put the timeout value to zero
	// or I just supose that the whole entry is going to be deleted?

	if(!entry->table)
		return ROFL_FAILURE;

	if(entry->timer_info.hard_timeout){
		if(of12_destroy_single_timer_entry_clean(entry->timer_info.hard_timer_entry, entry->table)!=ROFL_SUCCESS)
			return ROFL_FAILURE;
		entry->timer_info.hard_timer_entry = NULL;
	}
		
	if(entry->timer_info.idle_timeout){
		if(of12_destroy_single_timer_entry_clean(entry->timer_info.idle_timer_entry, entry->table)!=ROFL_SUCCESS)
			return ROFL_FAILURE;
		entry->timer_info.idle_timer_entry = NULL;
	}
		
#if DEBUG_NO_REAL_PIPE
	of12_fill_new_timer_entry_info(entry,0,0);
#endif
		
	return ROFL_SUCCESS;
}

/**
 * of12_reschedule_idle_timer
 * check if there is the need of re-scheduling an idle timer
 */
static rofl_result_t of12_reschedule_idle_timer(of12_entry_timer_t * entry_timer, of12_flow_table_t * table)
{
	struct timeval system_time;
	of12_gettimeofday(&system_time,NULL);
	uint64_t now = of12_get_time_ms(&system_time);
	uint64_t expiration_time;
	expiration_time = of12_get_expiration_time_slotted(entry_timer->entry->timer_info.idle_timeout, &(entry_timer->time_last_update));
	of12_timer_timeout_type_t is_idle = IDLE_TO;
	
	if(expiration_time <= now)
	{
		//check if there is another entry for the hard TO and delete it
		if (entry_timer->entry->timer_info.hard_timer_entry)
			of12_destroy_single_timer_entry_clean(entry_timer->entry->timer_info.hard_timer_entry, table);
#ifdef DEBUG_NO_REAL_PIPE
		fprintf(stderr,"NOT erasing real entries of table \n");
		of12_fill_new_timer_entry_info(entry_timer->entry,0,0);
#else
		//fprintf(stderr,"Erasing real entries of table \n"); //NOTE Delete
		of12_remove_specific_flow_entry_table(table,entry_timer->entry, MUTEX_ALREADY_ACQUIRED_BY_TIMER_EXPIRATION);
#endif
		return ROFL_SUCCESS; // timeout expired so no need to reschedule !!! we have to delete the entry
	}
	
#if OF12_TIMER_STATIC_ALLOCATION_SLOTS
	
	int slot_delta = expiration_time - table->timers[table->current_timer_group].timeout; //ms
	int slot_position = (table->current_timer_group + slot_delta/OF12_TIMER_SLOT_MS) % OF12_TIMER_GROUPS_MAX;
	if(of12_entry_timer_init(&(table->timers[slot_position]), entry_timer->entry, is_idle, &(entry_timer->time_last_update))==NULL)
		return ROFL_FAILURE;
#else
		
	of12_timer_group_t * tg_iterator=of12_dynamic_slot_search(table, expiration_time);
	if(tg_iterator==NULL)
		return ROFL_FAILURE;
	// add entry to this group. new_group.list->num_of_timers++; ...
	if(of12_entry_timer_init(tg_iterator, entry_timer->entry, is_idle, &(entry_timer->time_last_update)) == NULL)
		return ROFL_FAILURE;
#endif
	return ROFL_SUCCESS;
}

/**
 * of12_destroy_all_entries_from_timer_group()
 * implements for loop that destroys the complete list of entries
 * This is ment to be used when a timer expires and we want to delete
 * all the entries of the group and the group itself
 */
static rofl_result_t of12_destroy_all_entries_from_timer_group(of12_timer_group_t* tg, of12_flow_table_t * table)
{
	of12_entry_timer_t* entry_iterator, *next;
	if(tg->list.num_of_timers>0 && tg->list.head)
	{
		for(entry_iterator = tg->list.head; entry_iterator; entry_iterator=next)
		{
			next = entry_iterator->next;
			
			if(entry_iterator->type == IDLE_TO)
			{
				if(of12_reschedule_idle_timer(entry_iterator, table)!=ROFL_SUCCESS) //WARNING return value for erasing the idle timeout
					return ROFL_FAILURE;
			}
			else
			{
				//check if there is another entry for the idle TO and delete it
				if (entry_iterator->entry->timer_info.idle_timer_entry)
					of12_destroy_single_timer_entry_clean(entry_iterator->entry->timer_info.idle_timer_entry, table);
#ifdef DEBUG_NO_REAL_PIPE
				fprintf(stderr,"NOT erasing real entries of table \n");
				of12_fill_new_timer_entry_info(entry_iterator->entry,0,0);
#else
				//fprintf(stderr,"Erasing real entries of table \n"); //NOTE DELETE
				of12_remove_specific_flow_entry_table(table,entry_iterator->entry, MUTEX_ALREADY_ACQUIRED_BY_TIMER_EXPIRATION);
#endif
			}
			if(entry_iterator)
			{
				cutil_free_shared(entry_iterator);
				entry_iterator = NULL;
			}
		}
	}
	return ROFL_SUCCESS;
}

#if ! OF12_TIMER_STATIC_ALLOCATION_SLOTS
/**
 * of12_dynamic_slot_search
 * extraction of a function used to search a timer group with a specific timeout
 * and if it does not exist create it.
 */
static of12_timer_group_t * of12_dynamic_slot_search(of12_flow_table_t* const table, uint64_t expiration_time)
{
	of12_timer_group_t* tg_iterator;
	//Determine the slot, check if is already defined, if not allocate
	//We search the timer group with the timeout corresponding to the expiration_time calculated
	for(tg_iterator = table->timers; tg_iterator && tg_iterator->timeout<expiration_time && tg_iterator->next; tg_iterator=tg_iterator->next);
	// After that we expect 3 different situations:
	
	if(!tg_iterator)
	{
		//the list is empty, we sould create the first group
		table->timers = of12_timer_group_init(expiration_time,NULL,NULL, table);
		if(table->timers == NULL)
			return NULL;

		tg_iterator = table->timers;
	}
	else if(tg_iterator->timeout>expiration_time)
	{
		// Create new group and allocate it before the current group
		tg_iterator = of12_timer_group_init(expiration_time,tg_iterator,tg_iterator->prev, table);
		if(tg_iterator == NULL)
			return NULL;
	}
	else if(tg_iterator->timeout < expiration_time && !tg_iterator->next)
	{
		// Create new group and allocate it after the current group
		tg_iterator = of12_timer_group_init(expiration_time,NULL, tg_iterator, table);
		if(tg_iterator == NULL)
			return NULL;
	}
	else
	{
		if (!(tg_iterator->timeout==expiration_time))
		{
			// Unexpected outcome ...
			fprintf(stderr,"<%s:%d> No proper position for this entry found\n",__func__, __LINE__);
			return NULL;
		}
	}
	
	return tg_iterator;
}
#endif

/**
 * of12_timer_update_entry
 * when an entry has been used we must 
 * update the time_last_update in order to
 * show that its not idle
 */
void of12_timer_update_entry(of12_flow_entry_t * flow_entry)
{
	if(flow_entry->timer_info.idle_timeout == 0)
		return; //no idle timeout => no need to update time
	else
	{
		of12_gettimeofday(&(flow_entry->timer_info.idle_timer_entry->time_last_update), NULL);
	}
}

static rofl_result_t of12_add_single_timer(of12_flow_table_t* const table, const uint32_t timeout, of12_flow_entry_t* entry, of12_timer_timeout_type_t is_idle)
{
	uint64_t expiration_time;
	struct timeval now;
    of12_gettimeofday(&now,NULL);
	expiration_time = of12_get_expiration_time_slotted(timeout, &now);
#if OF12_TIMER_STATIC_ALLOCATION_SLOTS
	// add entries to the position tc + hard_timeout_s
	if(timeout > OF12_TIMER_GROUPS_MAX)
	{
		fprintf(stderr,"Timeout value excedded maximum value (hto=%d, MAX=%d)\n", timeout,OF12_TIMER_GROUPS_MAX);
		return ROFL_FAILURE;
	}
	
	
	int slot_delta = expiration_time - table->timers[table->current_timer_group].timeout; //ms
	
	//NOTE we allocate the timer in the next slot rounding up:
	//so the actual expiration time will be a value in [hard_timeout,hard_timeout+OF12_TIMER_SLOT_MS)
	int slot_position = (table->current_timer_group+(slot_delta/OF12_TIMER_SLOT_MS))%(OF12_TIMER_GROUPS_MAX);
	if(of12_entry_timer_init(&(table->timers[slot_position]), entry, is_idle, NULL)==NULL)
		return ROFL_FAILURE;

#else
	
	of12_timer_group_t* tg_iterator=of12_dynamic_slot_search(table, expiration_time);
	if (tg_iterator==NULL)
		return ROFL_FAILURE;
	
	// add entry to this group. new_group.list->num_of_timers++; ...
	if(of12_entry_timer_init(tg_iterator, entry, is_idle, NULL) == NULL)
		return ROFL_FAILURE;
	
#endif
	return ROFL_SUCCESS;
}

//Add timer to a table
rofl_result_t of12_add_timer(of12_flow_table_t* const table, of12_flow_entry_t* const entry){
	rofl_result_t res;
	//NOTE we don't use that lock because this is only called from of12_add_flow_entry...()
	//platform_mutex_lock(table->mutex);
	
	if(entry->timer_info.idle_timeout)
	{
		res = of12_add_single_timer(table, entry->timer_info.idle_timeout, entry, IDLE_TO); //is_idle = 1
		if(res == ROFL_FAILURE)
		{
			//platform_mutex_unlock(table->mutex);
			return ROFL_FAILURE;
		}
	}
	if(entry->timer_info.hard_timeout)
	{
		res = of12_add_single_timer(table, entry->timer_info.hard_timeout, entry, HARD_TO); //is_idle = 0
		if(res == ROFL_FAILURE)
		{
			//platform_mutex_unlock(table->mutex);
			return ROFL_FAILURE;
		}
	}
	
	//platform_mutex_unlock(table->mutex);
	return ROFL_SUCCESS;
}

void of12_process_pipeline_tables_timeout_expirations(const of12_pipeline_t* pipeline){

	unsigned int i;
	
	struct timeval system_time;
	of12_gettimeofday(&system_time,NULL);
	uint64_t now = of12_get_time_ms(&system_time);
	//fprintf(stderr,"<%s:%d> Now = %lu\n",__func__,__LINE__, now);

	for(i=0;i<pipeline->num_of_tables;i++)
	{
		of12_flow_table_t* table = &pipeline->tables[i];
		platform_mutex_lock(table->mutex);
#if OF12_TIMER_STATIC_ALLOCATION_SLOTS
		while(table->timers[table->current_timer_group].timeout<=now)
		{
			//rotate the timers
			of12_timer_group_rotate(&(table->timers[table->current_timer_group]),table);
		}
#else	
		of12_timer_group_t* slot_it, *next;
		for(slot_it=table->timers; slot_it; slot_it=next)
		{
			if(now<slot_it->timeout) //Current slot time > time_now. We are done
				break;
			//remove all entries and the timer group.
			if(of12_destroy_all_entries_from_timer_group(slot_it, table)!=ROFL_SUCCESS)
			{
				fprintf(stderr,"Error while destroying timer group\n");
				platform_mutex_unlock(table->mutex);
				return;
			}
			next = slot_it->next;
			if(slot_it)
				of12_destroy_timer_group(slot_it, table);
		}		
#endif
		platform_mutex_unlock(table->mutex);
	}
	return;
}
