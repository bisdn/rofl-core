#include "lib_random.h"

/* Random generators */
uint8_t random8(){
	return rand()%0xFF;
}
uint16_t random16(){
	return rand()%0xFFFF;
}
uint32_t random32(){
	return rand()%0xFFFFFFFF;
}
uint64_t random64(){
	return ((uint64_t)rand()%0xFFFFFFFF<<32)|rand()%0xFFFFFFFF;
}
uint8_t random_mask8(){
	uint8_t tmp;
	while (!(tmp = (0xFF << ((rand()%8)))));
	return tmp; 
}
uint16_t random_mask16(){
	uint8_t tmp;
	while(!(tmp= (0xFFFF << ((rand()%16))))); 
	return tmp;
}
uint32_t random_mask32(){
	uint8_t tmp;
	while(!(tmp =  (0xFFFFFFFF << ((rand()%32))))); 
	return tmp;
}
uint64_t random_mask64(){
	uint8_t tmp;
	while(!(tmp = (0xFFFFFFFFFFFFFFFF << ((rand()%64))))); 
	return tmp;
}

