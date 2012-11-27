#include "../lock.h"
/*
*
* pthread compatible platform_lock interface implementation
*      Author: msune
*
*/

/* MUTEX operations */
//Init&destroy
inline int platform_mutex_init(platform_mutex_t* mutex, void* params){
	return pthread_mutex_init(mutex, params);
}

inline void platform_mutex_destroy(platform_mutex_t* mutex){
	pthread_mutex_destroy(mutex);
}

//Operations
inline void platform_mutex_lock(platform_mutex_t* mutex){
	pthread_mutex_lock(mutex);
}

inline void platform_mutex_unlock(platform_mutex_t* mutex){
	pthread_mutex_unlock(mutex);
}


/* RWLOCK */
//Init&destroy
inline int platform_rwlock_init(platform_rwlock_t* rwlock, void* params){
	return pthread_rwlock_init(rwlock, params);
}

inline void platform_rwlock_destroy(platform_rwlock_t* rwlock){
	pthread_rwlock_destroy(rwlock);
}

//Read
void platform_rwlock_rdlock(platform_rwlock_t* rwlock){
	pthread_rwlock_rdlock(rwlock);
}

void platform_rwlock_rdunlock(platform_rwlock_t* rwlock){
	pthread_rwlock_unlock(rwlock);
}


//Write
void platform_rwlock_wrlock(platform_rwlock_t* rwlock){
	pthread_rwlock_wrlock(rwlock);
}
void platform_rwlock_wrunlock(platform_rwlock_t* rwlock){
	pthread_rwlock_unlock(rwlock);
}
