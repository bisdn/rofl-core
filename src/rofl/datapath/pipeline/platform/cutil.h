#ifndef __CUTIL_H__
#define __CUTIL_H__

#include "rofl.h"

/*
 * interface for utilities which may be platform specific (doesn't necessarily have to be)
 */

//C++ extern C
ROFL_BEGIN_DECLS

#include <stdint.h>

uint16_t cutil_htobe16( uint16_t host16 );
uint16_t cutil_be16toh( uint16_t big16 );
uint32_t cutil_htobe32( uint32_t host32 );
uint32_t cutil_be32toh( uint32_t big32 );
uint64_t cutil_htobe64( uint64_t host64 );
uint64_t cutil_be64toh( uint64_t big64 );

//C++ extern C
ROFL_END_DECLS

#endif //CUTIL_H
