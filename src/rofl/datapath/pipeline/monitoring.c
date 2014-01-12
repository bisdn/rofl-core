#include "monitoring.h"
#include <assert.h>
#include <string.h>
#include "platform/memory.h"

//initialize in dynamic memory
rofl_result_t init_monitored_entity(enum monitored_entity_type type, monitored_entity_t* prev, monitored_entity_t* parent){

	monitored_entity_t* entity;	

	//We need to know really to place it
	if(prev && parent)
		return ROFL_FAILURE;

	//Entity	
	entity = platform_malloc_shared(sizeof(monitored_entity_t));
	
	if(!entity)
		return ROFL_FAILURE;

	//memset to zero
	memset(entity,0,sizeof(*entity));

	//Setting the type
	entity->type = type;	

	//Link it to the double-linked list	

	return ROFL_SUCCESS;	
}

//destroy monitored entity
void destroy_monitored_entity(monitored_entity_t* entity){

	monitored_entity_t *inner_it, *next;
	
	//Deleting the base always present chassis
	//monitored entity is not allowed
	if(!entity->parent && !entity->prev){
		assert(0);	
	}

	//Delete (all) nested inner elements
	inner_it = entity->inner;
	while(inner_it){
		next = inner_it->next;
		destroy_monitored_entity(inner_it);
		inner_it = next;
	}

	//Unlink from linked list
	if(!entity->prev){
		//Head of the list
		if(entity->next)
			entity->next->prev = NULL;	
		if(entity->parent)
			entity->parent->inner = entity->next;
	}else{
		//Not the head of the list (really?)
		if(entity->next)
			entity->next->prev = entity->prev;
		entity->prev->next = entity->next;	
	}
	
	//Free dynamic memory
	platform_free_shared(entity);
}

