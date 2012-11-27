#ifndef __PLATFORM_ATOMIC_OPERATIONS_H__
#define __PLATFORM_ATOMIC_OPERATIONS_H__

#include <inttypes.h>
/**
 * calls to platform-specific atomic funcions
 */

#include "lock.h"

void platform_atomic_inc64(uint64_t* counter, platform_mutex_t* mutex);
void platform_atomic_inc32(uint32_t* counter, platform_mutex_t* mutex);

void platform_atomic_add64(uint64_t* counter, uint64_t* value, platform_mutex_t* mutex);
void platform_atomic_add32(uint32_t* counter, uint32_t* value, platform_mutex_t* mutex);
	
#endif
