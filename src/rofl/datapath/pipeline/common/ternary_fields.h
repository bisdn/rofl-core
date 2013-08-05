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
#if 0
// Whatever type
typedef struct{
	enum utern_type_t type;
}utern_t;

/* 
* Ternary types. In mask 0 values are wildcarded. Use all 1 to not wildcard
*/
//8 bit type
typedef struct{
	enum utern_type_t type;
	uint8_t value;	
	uint8_t mask;
	utern_t* next; //Next field
}utern8_t;

//16 bit type
typedef struct{
	enum utern_type_t type;
	uint16_t value;	
	uint16_t mask;
	utern_t* next; //Next field
}utern16_t;

#if 0 
//20 bit type
typedef struct{
	enum utern_type_t type;
	uint8_t value[5];	
	uint8_t mask[5];
	utern_t* next; //Next field
}utern20_t;
#endif

//32 bit type
typedef struct{
	enum utern_type_t type;
	uint32_t value;	
	uint32_t mask;
	utern_t* next; //Next field
}utern32_t;

#if 0
//48 bit type
typedef struct{
	enum utern_type_t type;
	uint8_t value[48];	
	uint8_t mask[48];
	utern_t* next; //Next field
}utern48_t;
#endif

//64 bit type
typedef struct{
	enum utern_type_t type;
	uint64_t value;	
	uint64_t mask;
	utern_t* next; //Next field
}utern64_t;

typedef struct double64{
	uint64_t high;
	uint64_t low;
}double64_t;

/*Union to hold both 64 bit (and smaller) types and 128 bit types */
typedef union of_uint182{
	uint64_t u64;
	double64_t u128;
}of_uint128_t;

//128 bit type
typedef struct{
	enum utern_type_t type;
	double64_t value;	
	double64_t mask;
	utern_t* next; //Next field
}utern128_t;
#endif 

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
bool __utern_compare8(const utern_t* tern, const uint8_t value);
bool __utern_compare16(const utern_t* tern, const uint16_t value);
bool __utern_compare32(const utern_t* tern, const uint32_t value);
bool __utern_compare64(const utern_t* tern, const uint64_t value);
bool __utern_compare128(const utern_t* tern, const uint128__t value);

//Check if two ternary values are equal
bool __utern_equals(const utern_t* tern1, const utern_t* tern2);
//bool __utern_equals16(const utern_t* tern1, const utern_t* tern2);
//bool __utern_equals32(const utern_t* tern1, const utern_t* tern2);
//bool __utern_equals64(const utern_t* tern1, const utern_t* tern2);
//bool __utern_equals128(const utern_t* tern1, const utern_t* tern2);

//Check if a ternary value is a subset of another 
bool __utern_is_contained(const utern_t* extensive_tern, const utern_t* tern);
//bool __utern_is_contained16(const utern_t* extensive_tern, const utern_t* tern);
//bool __utern_is_contained32(const utern_t* extensive_tern, const utern_t* tern);
//bool __utern_is_contained64(const utern_t* extensive_tern, const utern_t* tern);
//bool __utern_is_contained128(const utern_t* extensive_tern, const utern_t* tern);


//Ternary alike functions
utern_t* __utern_get_alike(const utern_t tern1, const utern_t tern2);
//utern_t* __utern16_get_alike(const utern_t tern1, const utern_t tern2);
//utern_t* __utern32_get_alike(const utern_t tern1, const utern_t tern2);
//utern_t* __utern64_get_alike(const utern_t tern1, const utern_t tern2);
//utern_t* __utern128_get_alike(const utern_t tern1, const utern_t tern2);

//Slow
//bool __utern_compare(const utern_t* tern, void* value);

//C++ extern C
ROFL_END_DECLS

#endif //TERNARY_FIELDS
