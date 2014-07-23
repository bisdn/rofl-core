/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdlib.h>
#include <rofl/pipeline/platform/cutil.h>


/*
* Endian conversion
*/

uint16_t cutil_htobe16( uint16_t host16 ){ //TODO: do appropriate proccesing
	return htobe16(host16);
} 
uint16_t cutil_be16toh( uint16_t big16 ){ //TODO: do appropriate proccesing
	return be16toh(big16);
} 
uint32_t cutil_htobe32( uint32_t host32 ){ //TODO: do appropriate proccesing
	return htobe32(host32);
} 
uint32_t cutil_be32toh( uint32_t big32 ){ //TODO: do appropriate proccesing
	return be32toh(big32);
}
uint64_t cutil_htobe64( uint64_t host64 ){ //TODO: do appropriate proccesing
	return htobe64(host64);
} 
uint64_t cutil_be64toh( uint64_t big64 ){ //TODO: do appropriate proccesing
	return be64toh(big64);
}

