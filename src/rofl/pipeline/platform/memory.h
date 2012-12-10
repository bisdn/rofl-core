/*
 * memory.h
 *
 *  Created on: Nov 29, 2012
 *      Author: tobi
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <stddef.h>
#include <stdlib.h>


// todo rename
void* cutil_malloc( size_t length );
void* cutil_malloc_shared( size_t length );
void cutil_free( void *data );
void cutil_free_shared( void *data );

#endif /* MEMORY_H_ */
