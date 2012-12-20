/*
 * memory.h
 *
 *  Created on: Nov 29, 2012
 *      Author: tobi
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "../util/rofl_pipeline_utils.h"


//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

#include <stddef.h>
#include <stdlib.h>

// todo rename
void* cutil_malloc( size_t length );
void* cutil_malloc_shared( size_t length );
void cutil_free( void *data );
void cutil_free_shared( void *data );
void* cutil_memcpy( void *dst, const void *src, size_t length );
void* cutil_memset( void *src, int c, size_t length );

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif /* MEMORY_H_ */
