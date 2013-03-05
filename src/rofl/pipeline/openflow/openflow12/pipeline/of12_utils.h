#ifndef __OF12_UTILS_H__
#define __OF12_UTILS_H__

/*
* Mutex state for flow_entry_removal 
*/
typedef enum of12_mutex_acquisition_required{
	MUTEX_NOT_ACQUIRED = 0, 			/*mutex has not been acquired and we must take it*/
	MUTEX_ALREADY_ACQUIRED_BY_TIMER_EXPIRATION,	/*mutex was taken when checking for expirations. We shouldn't call the timers functions*/
	MUTEX_ALREADY_ACQUIRED_NON_STRICT_SEARCH	/*mutex was taken when looking for entries with a non strict definition*/
}of12_mutex_acquisition_required_t;

#endif //OF12_UTILS
