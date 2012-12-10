/*
 * memory.c
 *
 *  Created on: Nov 29, 2012
 *      Author: tobi
 */

#include "../memory.h"

#include <stdlib.h>


/*
 * malloc and free should be replaced if necessary by platform specific memory allocators
 */

//Per core memory allocators
inline void* cutil_malloc( size_t length ){
	return malloc( length );
}
inline void cutil_free( void *data ){
	free( data );
}

//Shared memory allocators
inline void* cutil_malloc_shared( size_t length ){
	return malloc( length );
}

inline void cutil_free_shared( void *data ){
	free( data );
}

