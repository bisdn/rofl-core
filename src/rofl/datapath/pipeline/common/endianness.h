/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ENDIANNESS_H__
#define __ENDIANNESS_H__

#include <stdint.h>

/*
* Add a macro implementation for htonb and ntohb
*/

#if defined(BIG_ENDIAN_DETECTED)
	//Host -> Network
	#define HTONB16(x) x 
	#define HTONB32(x) x
	#define HTONB64(x) x

	//Network -> Host
	#define NTOHB16(x) x 
	#define NTOHB32(x) x
	#define NTOHB64(x) x
#elif defined(LITTLE_ENDIAN_DETECTED)
	#if defined(BYTESWAP_HEADER_DETECTED)
		#include <byteswap.h>
		
		//Host -> Network
		#define HTONB16(x) __bswap_16(x)  
		#define HTONB32(x) __bswap_32(x) 
		#define HTONB64(x) __bswap_64(x) 
	
		//Network -> Host
		#define NTOHB16(x) __bswap_16(x)
		#define NTOHB32(x) __bswap_32(x)
		#define NTOHB64(x) __bswap_64(x)
	#else
		//TODO: maybe add a non-assembly impl?
		#error byteswap.h not available; configure error?
	#endif

#else
	#error Unsupported endianness
#endif //endianness check

#endif //__ENDIANNESS_H__
