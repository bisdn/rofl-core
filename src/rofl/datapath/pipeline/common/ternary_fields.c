#include "ternary_fields.h"

#include <assert.h>

#include "../platform/memory.h"
#include <rofl/datapath/pipeline/common/ternary_fields.h>

#define HI(x) *(uint64_t*)&x.val[0]
#define LO(x) *(uint64_t*)&x.val[8]
/*
 * NOTE OUT!!
typedef struct wrap_u128{
	uint64_t hi;
	uint64_t lo;
}w128_t;
*/

/*
* Initializers
*/
inline utern_t* __init_utern8(uint8_t value, uint8_t mask){
	utern_t* tern = (utern_t*)platform_malloc_shared(sizeof(utern_t));

	if(!tern)
		return NULL;

	tern->type = UTERN8_T;
	tern->value.u8 = value;
	tern->mask.u8 = mask;
	return (utern_t*)tern;
}
inline utern_t* __init_utern16(uint16_t value, uint16_t mask){
	utern_t* tern = (utern_t*)platform_malloc_shared(sizeof(utern_t));

	if(!tern)
		return NULL;

	tern->type = UTERN16_T;
	tern->value.u16 = value;
	tern->mask.u16 = mask;
	return (utern_t*)tern;
}
inline utern_t* __init_utern32(uint32_t value, uint32_t mask){
	utern_t* tern = (utern_t*)platform_malloc_shared(sizeof(utern_t));

	if(!tern)
		return NULL;
	
	tern->type = UTERN32_T;
	tern->value.u32 = value;
	tern->mask.u32 = mask;
	return (utern_t*)tern;
}
inline utern_t* __init_utern64(uint64_t value, uint64_t mask){
	utern_t* tern = (utern_t*)platform_malloc_shared(sizeof(utern_t));

	if(!tern)
		return NULL;
	
	tern->type = UTERN64_T;
	tern->value.u64 = value;
	tern->mask.u64 = mask;
	return (utern_t*)tern;
}
inline utern_t* __init_utern128(uint128__t value, uint128__t mask){ //uint128_t funny!
	utern_t* tern = (utern_t*)platform_malloc_shared(sizeof(utern_t));
	
	if(!tern)
		return NULL;
	
	tern->type = UTERN128_T;
	tern->value.u128 = value;
	tern->mask.u128 = mask;
	return (utern_t*)tern;
}

/*
* Single destructor
*/
rofl_result_t __destroy_utern(utern_t* utern){
	platform_free_shared(utern);
	//FIXME: maybe check returning value
	return ROFL_SUCCESS; 
}	


/*
* Comparison 
*/

inline bool __utern_compare8(const utern_t* tern, const uint8_t value){
	return (tern->value.u8 & tern->mask.u8) == (value & tern->mask.u8); 
}
inline bool __utern_compare16(const utern_t* tern, const uint16_t value){
	return (tern->value.u16 & tern->mask.u16) == (value & tern->mask.u16); 
}
inline bool __utern_compare32(const utern_t* tern, const uint32_t value){
	return (tern->value.u32 & tern->mask.u32) == (value & tern->mask.u32); 
}

inline bool __utern_compare64(const utern_t* tern, const uint64_t value){
	return (tern->value.u64 & tern->mask.u64) == (value & tern->mask.u64); 
}

inline bool __utern_compare128(const utern_t* tern, const uint128__t value){
#if 0
	return ( (tern->value.low & tern->mask.low) == (value.low & tern->mask.low) ) &&
		( (tern->value.high & tern->mask.high) == (value.high & tern->mask.high) ); 
		
	return ( ( *(uint64_t*)&tern->value.u128.val[0] & *(uint64_t*)&tern->mask.u128.val[0]) == (*(uint64_t*)&value.val[0] & *(uint64_t*)&tern->mask.u128.val[0]) ) &&
		( (*(uint64_t*)&tern->value.u128.val[8] & *(uint64_t*)&tern->mask.u128.val[8]) == (*(uint64_t*)&value.val[8] & *(uint64_t*)&tern->mask.u128.val[8]) ); 
			
	w128_t *tern_val, *tern_mask, *val;
	tern_val = (w128_t *)&tern->value.u128;
	tern_mask = (w128_t *)&tern->mask.u128;
	val = (w128_t *)&value;
			
	return ( (tern_val->lo & tern_mask->lo) == (val->lo & tern_mask->lo) &&
				(tern_val->hi & tern_mask->hi) == (val->hi & tern_mask->hi) );
#endif
	
	return ( (HI(tern->value.u128) & HI(tern->mask.u128)) == (HI(value) & HI(tern->mask.u128)) )&&
			( (LO(tern->value.u128) & LO(tern->mask.u128)) == (LO(value) & LO(tern->mask.u128)) );
	
}

/*
* Contained 
*/
//Extensive tern is a more generic (with a less restrictive mask or equal) to tern
inline bool __utern_is_contained(const utern_t* extensive_tern, const utern_t* tern){
	
	switch(extensive_tern->type){
		
		case UTERN8_T:
			if(((extensive_tern->mask.u8 ^ tern->mask.u8) & extensive_tern->mask.u8) > 0)
				return false;
			return (extensive_tern->value.u8 & extensive_tern->mask.u8) == (tern->value.u8 & extensive_tern->mask.u8);
			break;
		case UTERN16_T:
			if(((extensive_tern->mask.u16 ^ tern->mask.u16) & extensive_tern->mask.u16) > 0)
				return false;
			return (extensive_tern->value.u16 & extensive_tern->mask.u16) == (tern->value.u16 & extensive_tern->mask.u16);
			break;
		case UTERN32_T:
			if(((extensive_tern->mask.u32 ^ tern->mask.u32) & extensive_tern->mask.u32) > 0)
				return false;
			return (extensive_tern->value.u32 & extensive_tern->mask.u32) == (tern->value.u32 & extensive_tern->mask.u32);
			break;
		case UTERN64_T:
			if(((extensive_tern->mask.u64 ^ tern->mask.u64) & extensive_tern->mask.u64) > 0)
				return false;
			return (extensive_tern->value.u64 & extensive_tern->mask.u64) == (tern->value.u64 & extensive_tern->mask.u64);
			break;
		case UTERN128_T:
#if 0
			if((((extensive_tern->mask.low ^ tern->mask.low) & extensive_tern->mask.low) > 0 ) || 
				(((extensive_tern->mask.high ^ tern->mask.high) & extensive_tern->mask.high) > 0 ) )
				return false;
			return ( (extensive_tern->value.low & extensive_tern->mask.low) == (tern->value.low & extensive_tern->mask.low) &&
				(extensive_tern->value.high & extensive_tern->mask.high) == (tern->value.high & extensive_tern->mask.high)	);
#endif
			
			if( (( (LO(extensive_tern->mask.u128) ^ LO(tern->mask.u128)) & LO(extensive_tern->mask.u128) ) > 0 ) || 
				(( (HI(extensive_tern->mask.u128) ^ HI(tern->mask.u128)) & HI(extensive_tern->mask.u128) ) > 0 ) )
				return false;
			return ( (LO(extensive_tern->value.u128) & LO(extensive_tern->mask.u128)) == (LO(tern->value.u128) & LO(extensive_tern->mask.u128)) ) &&
				((HI(extensive_tern->value.u128) & HI(extensive_tern->mask.u128)) == (HI(tern->value.u128) & HI(extensive_tern->mask.u128))	);
			
			break;
		default:
			assert(0); //we should never reach this point
			return false;
	}
}

/*
* Check if two ternary values are equal
*/
inline bool __utern_equals(const utern_t* tern1, const utern_t* tern2){
	switch(tern1->type)	{
		
		case UTERN8_T:
			return (tern1->value.u8 == tern2->value.u8) && (tern1->mask.u8 == tern2->mask.u8);
			break;
		case UTERN16_T:
			return (tern1->value.u16 == tern2->value.u16) && (tern1->mask.u16 == tern2->mask.u16);
			break;
		case UTERN32_T:
			return (tern1->value.u32 == tern2->value.u32) && (tern1->mask.u32 == tern2->mask.u32);
			break;
		case UTERN64_T:
			return (tern1->value.u64 == tern2->value.u64) && (tern1->mask.u64 == tern2->mask.u64);
			break;
		case UTERN128_T:
			return (LO(tern1->value.u128) == LO(tern2->value.u128)) && (LO(tern1->mask.u128) == LO(tern2->mask.u128)) &&
				(HI(tern1->value.u128) == HI(tern2->value.u128)) && (HI(tern1->mask.u128) == HI(tern2->mask.u128));
			break;
		default:
			assert(0); // we should never reach this point
			return false;
	}
}

//Ternary alike functions. Tern2 MUST always have more restrictive mask
inline utern_t* __utern_get_alike(const utern_t tern1, const utern_t tern2){
	//TODO: there might be more efficient impl. maybe erasing 1s in diff... but dunno
	
	wrap_uint_t diff, new_mask;
	
	switch(tern1.type){
		case UTERN8_T:

			diff.u8 = ~( 
					(tern1.value.u8 & tern1.mask.u8)	
					^
					(tern2.value.u8 & tern2.mask.u8)
					);
			//erase right 1.
			for(new_mask.u8=0xFF;new_mask.u8;new_mask.u8=new_mask.u8<<1)
				if((diff.u8&new_mask.u8) == new_mask.u8) break; 

			if(tern1.mask.u8 < new_mask.u8 || tern2.mask.u8 < new_mask.u8 )
				return NULL;
			
			if(new_mask.u8)
				return __init_utern8(tern1.value.u8,new_mask.u8);

			return NULL;
			
			break;
			
		case UTERN16_T:
	
			diff.u16 = ~( 
					(tern1.value.u16 & tern1.mask.u16)	
					^
					(tern2.value.u16 & tern2.mask.u16)
					);
			//erase right 1.
			for(new_mask.u16=0xFFFF;new_mask.u16;new_mask.u16=new_mask.u16<<1)
				if((diff.u16&new_mask.u16) == new_mask.u16) break; 
			
			if(tern1.mask.u16 < new_mask.u16 || tern2.mask.u16 < new_mask.u16 )
				return NULL;
			
			if(new_mask.u16)
				return __init_utern16(tern1.value.u16,new_mask.u16);
			
			return NULL;
			
			break;
			
		case UTERN32_T:

			diff.u32 = ~( 
					(tern1.value.u32 & tern1.mask.u32)	
					^
					(tern2.value.u32 & tern2.mask.u32)
					);
			//erase right 1.
			for(new_mask.u32=0xFFFFFFFF;new_mask.u32;new_mask.u32=new_mask.u32<<1)
				if((diff.u32&new_mask.u32) == new_mask.u32) break; 
			
			if(tern1.mask.u32 < new_mask.u32 || tern2.mask.u32 < new_mask.u32 )
				return NULL;
			
			if(new_mask.u32)
				return __init_utern32(tern1.value.u32,new_mask.u32);
			
			return NULL;
			
			break;
		
		case UTERN64_T:

			diff.u64 = ~( 
					(tern1.value.u64 & tern1.mask.u64)	
					^
					(tern2.value.u64 & tern2.mask.u64)
					);
			//erase right 1.
			for(new_mask.u64=0xFFFFFFFFFFFFFFFF;new_mask.u64;new_mask.u64=new_mask.u64<<1)
				if((diff.u64&new_mask.u64) == new_mask.u64) break; 
			
			//FIXME assert unlikely
			//FIXME this condition happens also when two values are different in the non masked part.
				//in this case we sould return the utern as it is now.
			if(tern1.mask.u64 < new_mask.u64 || tern2.mask.u64 < new_mask.u64 )
				return NULL;
			
			if(new_mask.u64)
				return __init_utern64(tern1.value.u64,new_mask.u64);
			
			return NULL;
			
			break;
			
		case UTERN128_T:

			LO(diff.u128) = ~(	(LO(tern1.value.u128) & LO(tern1.mask.u128))	^	(LO(tern2.value.u128) & LO(tern2.mask.u128))	);
			HI(diff.u128) = ~(	(HI(tern1.value.u128) & HI(tern1.mask.u128))	^	(HI(tern2.value.u128) & HI(tern2.mask.u128))	);
			
			//We first look for the common mask in the lower part
			for(LO(new_mask.u128)=0xFFFFFFFFFFFFFFFF;LO(new_mask.u128);LO(new_mask.u128)=LO(new_mask.u128)<<1)
				if((LO(diff.u128)&LO(new_mask.u128)) == LO(new_mask.u128)) break; 
			
			if( (LO(tern1.mask.u128) < LO(new_mask.u128) || LO(tern2.mask.u128) < LO(new_mask.u128)) && HI(diff.u128) == 0xffffffffffffffff )
				return NULL;
				
			if( LO(new_mask.u128) && HI(diff.u128) == 0xffffffffffffffff ){
				HI(new_mask.u128) = 0xffffffffffffffff;
				return __init_utern128(tern1.value.u128,new_mask.u128);
			}
			
			//Now we look for it in the higher part
			for(HI(new_mask.u128)=0xFFFFFFFFFFFFFFFF;HI(new_mask.u128);HI(new_mask.u128)=HI(new_mask.u128)<<1)
				if((HI(diff.u128)&HI(new_mask.u128)) == HI(new_mask.u128)) break;
				
			if(HI(tern1.mask.u128)<HI(new_mask.u128) || HI(tern2.mask.u128) < HI(new_mask.u128) )
				return NULL;
			
			if(HI(new_mask.u128)){
				LO(new_mask.u128)=0x0000000000000000;
				return __init_utern128(tern1.value.u128,new_mask.u128);
			}
			
			return NULL;
			
			break;
		
		default:
			assert(0); // we should never reach this point
			return NULL;
	}
	return NULL;
}

#if 0
//Not used yet
inline bool __utern_compare128(const utern_t* tern, const void* value){
	utern128_t* tmp = (utern128_t*)tern;
	uint64_t* tmp_tern1 = (uint64_t*)tmp->value;
	uint64_t* tmp_tern2 = (uint64_t*)(tmp->value+64);
	uint64_t* tmp_mask1 = (uint64_t*)tmp->mask;
	uint64_t* tmp_mask2 = (uint64_t*)(tmp->mask+64);

	uint64_t* tmp_value1 = (uint64_t*)value;
	uint64_t* tmp_value2 = (uint64_t*)(value+64);
	
	return ((*tmp_tern1&*tmp_mask1) == (*tmp_value1&*tmp_mask1))
		&&
		((*tmp_tern2&*tmp_mask2) == (*tmp_value2&*tmp_mask2));
}
#endif

#if 0
///SLOW

/* Exported method utern_compare */ 
inline bool __utern_compare(const utern_t* tern, void* value){
	
	//TODO: tweak order for performance
	
	//Order is in purpose, not using switch too ;)
	if(tern->type == UTERN16_T){
		return utern_compare16(tern,value); 
	}else if(tern->type == UTERN32_T){
		return utern_compare32(tern,value); 
	}else if(tern->type == UTERN64_T){
		return utern_compare64(tern,value); 
	}else if(tern->type == UTERN8_T){
		return utern_compare8(tern,value); 
	}else if(tern->type == UTERN128_T){
		return utern_compare128(tern,value); 
	}else 
		return false;
}*/
#endif
