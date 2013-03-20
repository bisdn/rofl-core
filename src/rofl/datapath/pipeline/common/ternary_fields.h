#ifndef __TERNARY_FIELDS_H__
#define __TERNARY_FIELDS_H__

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include "rofl.h"
#include "../platform/cutil.h"

enum utern_type_t {
	UTERN8_T = 0,  //8 bit
	UTERN16_T = 1, //16 bit
	UTERN20_T = 2, //20 bit -> not used
	UTERN32_T = 3, //32 bit
	UTERN48_T = 4, //48 bit -> not used
	UTERN64_T = 5, //64 bit
	UTERN128_T = 6 //128 bit
};

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

#if 0
//Not used
//128 bit type
typedef struct{
	enum utern_type_t type;
	uint8_t value[128];	
	uint8_t mask[128];
	utern_t* next; //Next field
}utern128_t;
#endif

/*
* Functions 
*/

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

//Initializers
utern_t* init_utern8(uint8_t value, uint8_t mask);
utern_t* init_utern16(uint16_t value, uint16_t mask);
utern_t* init_utern32(uint32_t value, uint32_t mask);
utern_t* init_utern64(uint64_t value, uint64_t mask);

//Destructor
rofl_result_t destroy_utern(utern_t* utern );

//Comparison
bool utern_compare8(const utern8_t* tern, const uint8_t value);
bool utern_compare16(const utern16_t* tern, const uint16_t value);
bool utern_compare32(const utern32_t* tern, const uint32_t value);
bool utern_compare64(const utern64_t* tern, const uint64_t value);
//bool utern_compare128(const utern128_t* tern, const uin128_t* value);

//Check if two ternary values are equal
bool utern_equals8(const utern8_t* tern1, const utern8_t* tern2);
bool utern_equals16(const utern16_t* tern1, const utern16_t* tern2);
bool utern_equals32(const utern32_t* tern1, const utern32_t* tern2);
bool utern_equals64(const utern64_t* tern1, const utern64_t* tern2);

//Check if a ternary value is a subset of another 
bool utern_is_contained8(const utern8_t* extensive_tern, const utern8_t* tern);
bool utern_is_contained16(const utern16_t* extensive_tern, const utern16_t* tern);
bool utern_is_contained32(const utern32_t* extensive_tern, const utern32_t* tern);
bool utern_is_contained64(const utern64_t* extensive_tern, const utern64_t* tern);


//Ternary alike functions
utern_t* utern8_get_alike(const utern8_t tern1, const utern8_t tern2);
utern_t* utern16_get_alike(const utern16_t tern1, const utern16_t tern2);
utern_t* utern32_get_alike(const utern32_t tern1, const utern32_t tern2);
utern_t* utern64_get_alike(const utern64_t tern1, const utern64_t tern2);

//Slow
//bool utern_compare(const utern_t* tern, void* value);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //TERNARY_FIELDS
