/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __TERNARY_FIELDS_H__
#define __TERNARY_FIELDS_H__

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include "rofl.h"
#include "wrap_types.h"

/**
* @author Marc Sune<marc.sune (at) bisdn.de>
* @author Victor Alvarez<victor.alvarez (at) bisdn.de>
*/

/* select between the lower and higher parts of the 128 bits value*/

//NOTE simplifying all types in a general one
typedef enum utern_type {
	UTERN8_T = 0,  //8 bit
	UTERN16_T = 1, //16 bit
	//UTERN20_T = 2, //20 bit -> not used
	UTERN32_T = 3, //32 bit
	//UTERN48_T = 4, //48 bit -> not used
	UTERN64_T = 5, //64 bit
	UTERN128_T = 6 //128 bit
}utern_type_t;

typedef struct utern{
	utern_type_t type;
	wrap_uint_t value;
	wrap_uint_t mask;
} utern_t;

/*
* Functions 
*/

//C++ extern C
ROFL_BEGIN_DECLS

//Initializers
utern_t* __init_utern8(uint8_t value, uint8_t mask);
utern_t* __init_utern16(uint16_t value, uint16_t mask);
utern_t* __init_utern32(uint32_t value, uint32_t mask);
utern_t* __init_utern64(uint64_t value, uint64_t mask);
utern_t* __init_utern128(uint128__t value, uint128__t mask);

//Destructor
rofl_result_t __destroy_utern(utern_t* utern );

//Comparison
static inline bool __utern_compare8(const utern_t* tern, const uint8_t value){
	return (tern->value.u8 & tern->mask.u8) == (value & tern->mask.u8); 
}
static inline bool __utern_compare16(const utern_t* tern, const uint16_t value){
	return (tern->value.u16 & tern->mask.u16) == (value & tern->mask.u16); 
}
static inline bool __utern_compare32(const utern_t* tern, const uint32_t value){
	return (tern->value.u32 & tern->mask.u32) == (value & tern->mask.u32); 
}
static inline bool __utern_compare64(const utern_t* tern, const uint64_t value){
	return (tern->value.u64 & tern->mask.u64) == (value & tern->mask.u64); 
}
static inline bool __utern_compare128(const utern_t* tern, const uint128__t value){
	return ( (UINT128__T_HI(tern->value.u128) & UINT128__T_HI(tern->mask.u128)) == (UINT128__T_HI(value) & UINT128__T_HI(tern->mask.u128)) )&&
			( (UINT128__T_LO(tern->value.u128) & UINT128__T_LO(tern->mask.u128)) == (UINT128__T_LO(value) & UINT128__T_LO(tern->mask.u128)) );
	
}
//Check if two ternary values are equal
bool __utern_equals(const utern_t* tern1, const utern_t* tern2);

//Check if a ternary value is a subset of another 
bool __utern_is_contained(const utern_t* extensive_tern, const utern_t* tern);

//Ternary alike functions
utern_t* __utern_get_alike(const utern_t tern1, const utern_t tern2);

//C++ extern C
ROFL_END_DECLS

#endif //TERNARY_FIELDS
