/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __WRAP_TYPES_H__
#define __WRAP_TYPES_H__

/*
 * This header file defines a union used to pass values of any size
 * from 8 bits to 128
 */

#include "large_types.h"

typedef union wrap_uint{
	uint8_t 	u8;
	uint16_t 	u16;
	uint32_t 	u32;
	uint64_t 	u64;
	uint128__t 	u128;
}wrap_uint_t;


#endif //__WRAP_TYPES_H__
