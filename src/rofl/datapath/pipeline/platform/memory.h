/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PLATFORM_MEMORY_H_
#define PLATFORM_MEMORY_H_

#include <stddef.h>
#include <stdlib.h>
#include "rofl.h"

/**
* @file memory.h
* @author Marc Sune<marc.sune (at) bisdn.de>, Tobias Jungel<tobias.jungel (at) bisdn.de>  
*
* @brief Defines the memory management interface used by the library. 
* The user of the library MUST provide an implementation for his/her platform.
*
* The memory management API is the set of calls used by library
* to perform dynamic memory allocation/deallocation, as well as
* other memory operations (like copy or move).
*
* The user MUST provide an implementation for his/her particular
* platform. 
*
* The calls sufixed with "_shared" are meant to allocate memory that
* is shared between all the threads, hardware threads, cores or in general
* any system, that may perform operations (flow entry insertions, packet
* pipeline transvering...) over the SAME logical switch.
*
* In a typical POSIX/libc system this would probably be directly malloc/free
* and the respective other calls, except maybe for the "_shared_" calls 
* (depending on the threading/process used). E.g. in a single process application
* "_shared" and non-shared mallocs/free maps to the same LIBC malloc/free
*
* 
*/

//C++ extern C
ROFL_BEGIN_DECLS

/**
* @brief Allocates a chunk of dynamic memory of size length
* @ingroup platform_memory
*
* This in LIBC compatible platforms is equivalent to malloc()
*/
void* platform_malloc( size_t length );

/**
* @brief Allocates a chunk of dynamic memory of size length, which
* must be accessible (R/W) for all the threads/hw threads, cores...
* that may interact with the same logical switch.
* @ingroup platform_memory
*/
void* platform_malloc_shared( size_t length );


/**
* @brief Frees a chunk of dynamic memory previously allocated with
* platform_malloc().
* @ingroup platform_memory
*
* This in LIBC compatible platforms is equivalent to free()
*/
void platform_free( void* data );

/**
* @brief Frees a chunk of dynamic memory previously allocated with
* platform_malloc_shared().
* @ingroup platform_memory
*
* This in LIBC compatible platforms is equivalent to free()
*/
void platform_free_shared( void* data );

/**
* @brief Copies a chunk of memory. Equivalent to LIBC memcpy() 
* @ingroup platform_memory
*/
void* platform_memcpy( void* dst, const void* src, size_t length );

/**
* @brief Sets 'c' to the whole chunk of memory. Equivalent to LIBC memset() 
* @ingroup platform_memory
*/
void* platform_memset( void* src, int c, size_t length );

/**
* @brief Moves a chunk of memory from src to dst. Equivalent to memmove().
* @ingroup platform_memory
*
* The user of the library can assume both src and dst ointers will ALWAYS
* be allocated via the same call (malloc/malloc_shared)
*/
void* platform_memmove( void* dst, const void* src, size_t length );

//C++ extern C
ROFL_END_DECLS

#endif /* PLATFORM_MEMORY_H_ */
