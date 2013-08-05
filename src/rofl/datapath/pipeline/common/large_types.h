/*
 * This header file defines a structure for a type of 128 bits
 * It is meant to be shared between Rofl-core, pipeline and xDPd
 */
#ifndef __LARGE_TYPES_H__
#define __LARGE_TYPES_H__

#include <stdint.h>



// This is defined as a 16 vector so the pipeline doesn't has to deal with byte order at all (e.g. upper and lower u64, etc)
typedef struct uint128_{
	uint8_t val[16];
}uint128__t;



#endif //__LARGE_TYPES_H__
