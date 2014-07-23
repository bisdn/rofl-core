/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * memory.c
 *
 *  Created on: Nov 29, 2012
 *      Author: tobi
 */

//#include "../memory.h" VICTOR

#include <stdlib.h>
#include <string.h>


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

void* platform_memset( void* src, int c, size_t length ){
	return memset(src,c,length);
}

//Shared memory allocators
/*inline*/ void* platform_malloc_shared( size_t length ){ //VICTOR
	return malloc( length );
}

inline void platform_free_shared( void *data ){
	free( data );
}

