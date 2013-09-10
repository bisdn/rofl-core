/*
 * memory.c
 *
 *  Created on: Nov 29, 2012
 *      Author: tobi
 */

//#include "../memory.h" VICTOR

#include <stdlib.h>


/*
 * malloc and free should be replaced if necessary by platform specific memory allocators
 */

//Per core memory allocators
inline void* platform_malloc( size_t length ){
	return malloc( length );
}
inline void platform_free( void *data ){
	free( data );
}

//Shared memory allocators
/*inline*/ void* platform_malloc_shared( size_t length ){ //VICTOR
	return malloc( length );
}

inline void platform_free_shared( void *data ){
	free( data );
}

