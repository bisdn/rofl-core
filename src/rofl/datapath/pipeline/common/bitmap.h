/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __BITMAP_TYPES_H__
#define __BITMAP_TYPES_H__

#include <inttypes.h>

//Some helper typdefs for code readability 
typedef uint32_t bitmap32_t; 
typedef uint64_t bitmap64_t; 
typedef struct{
	bitmap64_t __submap[2];
}bitmap128_t; 

static inline void bitmap128_clean(bitmap128_t* bitmap){
	bitmap->__submap[0] = 0x0UL;
	bitmap->__submap[1] = 0x0UL;
}

static inline bool bitmap128_is_bit_set(const bitmap128_t* bitmap, unsigned int pos){
	if(pos >= 64)
		return ( bitmap->__submap[1] & 1UL<<(pos-64) ) > 0;
	else
		return ( bitmap->__submap[0] & 1UL<<pos ) > 0;
}

static inline void bitmap128_set(bitmap128_t* bitmap, unsigned int pos){
	if(pos >= 64)
		bitmap->__submap[1] |= 1UL<<(pos-64);
	else
		bitmap->__submap[0] |= 1UL<<(pos);
}

#endif //__BITMAP_TYPES_H__
