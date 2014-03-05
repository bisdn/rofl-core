#include "monitoring.h"
#include <assert.h>
#include <string.h>
#include "platform/memory.h"
#include "util/logging.h"

//
// Monitored entities
//

static inline void __clone_copy_me_fields(monitored_entity_t* d, monitored_entity_t* orig){

	//Copy contents rawly
	memcpy(d,orig,sizeof(monitored_entity_t));
	
	//Set pointers to NULL
	d->parent=d->inner=d->prev=d->next=NULL;
}

//clones the monitored entity, and all next and inner ones
static inline monitored_entity_t* __clone_monitored_entity(monitored_entity_t* orig){

	monitored_entity_t* entity;	
	
	if(!orig)
		return NULL;

	//Create a new entity
	entity = platform_malloc_shared(sizeof(monitored_entity_t));
	
	if(!entity)
		return NULL;
	//memset to zero
	memset(entity,0,sizeof(*entity));

	//Clone fields	
	__clone_copy_me_fields(entity, orig);

	//Launch recursively the copies over the inner and the next
	if(orig->inner){
		entity->inner = __clone_monitored_entity(orig->inner);
		if(!entity->inner){
			assert(0);
		}
	}
	if(orig->next){
		entity->next = __clone_monitored_entity(orig->next);
		if(!entity->next){
			assert(0);
		}
	}

	return entity;
}

//Root needs a special treating
static inline rofl_result_t __clone_root_monitored_entity(monitoring_state_t* monitoring, monitored_entity_t* root_dest, monitored_entity_t* root_orig){
	
	//First clone the contents
	__clone_copy_me_fields(root_dest,root_orig);
	
	//Try to clone inner
	if(root_orig->inner){
		root_dest->inner = __clone_monitored_entity(root_orig->inner);
		if(!root_dest->inner)
			return ROFL_FAILURE;
	}

	//Try to clone next
	if(root_orig->next){
		root_dest->next = __clone_monitored_entity(root_orig->next);
		if(!root_dest->next){
			if(root_dest->inner)
				__monitoring_remove_monitored_entity(monitoring, root_dest->inner, true);
			return ROFL_FAILURE;
		}
	}
	
	return ROFL_SUCCESS;
}

//initialize in dynamic memory
monitored_entity_t* monitoring_add_monitored_entity(monitoring_state_t* monitoring, enum monitored_entity_type type, monitored_entity_t* prev, monitored_entity_t* parent){

	monitored_entity_t* entity;	

	//We need to know really to place it
	if( !monitoring || ( (prev && parent) || (!prev && !parent) ) ){
		assert(0);
		return NULL;
	}

	//The root node is unique
	if(prev && !prev->parent){ 
		assert(0);
		return NULL;
	}

	//Entity
	entity = platform_malloc_shared(sizeof(monitored_entity_t));
	
	if(!entity)
		return NULL;

	//memset to zero
	memset(entity,0,sizeof(*entity));

	//Setting the type
	entity->type = type;	

	//Add it to the linked list
	if(monitoring->rwlock)
		platform_rwlock_wrlock(monitoring->rwlock);

	if(prev){
		//Append it after the prev
		entity->next = prev->next;
		entity->prev = prev;
		prev->next = entity;
		entity->parent = prev->parent;
	}else{
		//Make it the first element of the inner leafs
		if(parent->inner){
			entity->next = parent->inner;
			entity->next->prev = entity;
			parent->inner = entity;	
			entity->parent = parent;
		}else{
			parent->inner = entity;
			entity->parent = parent;
		}
	}
	
	//Increment rev counter
	monitoring->last_rev++;	

	if(monitoring->rwlock)
		platform_rwlock_wrunlock(monitoring->rwlock);	

	return entity;	
}

void __monitoring_dump_me(int indentation, monitored_entity_t* me);

//destroy monitored entity
rofl_result_t __monitoring_remove_monitored_entity(monitoring_state_t* monitoring, monitored_entity_t* entity, bool lock_acquired){

	monitored_entity_t *inner_it, *next;

	if(!entity)
		return ROFL_SUCCESS;

	//Sanity checks
	if(!monitoring)
		return ROFL_FAILURE;	

	//Prevent readers (snapshot creators) to jump in
	if(!lock_acquired && monitoring->rwlock)
		platform_rwlock_wrlock(monitoring->rwlock);	
	
	
	//Delete (all) nested inner elements
	inner_it = entity->inner;
	while(inner_it){
		next = inner_it->next;
		__monitoring_remove_monitored_entity(monitoring, inner_it, true);
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
		//Not the head of the list
		if(entity->next)
			entity->next->prev = entity->prev;
		entity->prev->next = entity->next;	
	}
	
	
	//Increment rev counter
	monitoring->last_rev++;	

	if(!lock_acquired && monitoring->rwlock)
		platform_rwlock_wrunlock(monitoring->rwlock);	
	
	//Free dynamic memory(only if it is not the root me)
	if(entity != &monitoring->chassis){
		platform_free_shared(entity);
	}
		
	return ROFL_SUCCESS;	
}

//Cloning

//
// Montoring elements
//
rofl_result_t __monitoring_init(monitoring_state_t* monitoring){

	//Clear all
	memset(monitoring,0,sizeof(*monitoring));

	//General flags	
	monitoring->last_rev = 1; //Must be one
	
	//Set primary monitored entity as being chassis
	monitoring->chassis.type = ME_TYPE_CHASSIS; 	

	//Locking
	monitoring->mutex = platform_mutex_init(NULL);
	monitoring->rwlock = platform_rwlock_init(NULL);

	if(!monitoring->mutex || !monitoring->rwlock){
		return ROFL_FAILURE;
	}
	
	return ROFL_SUCCESS;
}

/**
* @brief Destroys the monitoring state 
* @ingroup  mgmt
*/
void __monitoring_destroy(monitoring_state_t* monitoring){

	if(!monitoring)
		return;

	//Lock rwlock (write)
	if(monitoring->rwlock)
		platform_rwlock_wrlock(monitoring->rwlock);	

	//Destroy the inner-most monitored entity
	__monitoring_remove_monitored_entity(monitoring, &monitoring->chassis, true);	

	//Release dynamic memory allocated	
	if(monitoring->rwlock)
		platform_rwlock_destroy(monitoring->rwlock);
	if(monitoring->mutex)
		platform_mutex_destroy(monitoring->mutex);
	
	if(monitoring->is_snapshot)
		platform_free_shared(monitoring);
}

void __monitoring_dump_me(int indentation, monitored_entity_t* me){

	int indentation_=indentation;

	if(!me)
		return;

	for(;indentation_ > 0;indentation_--)
		ROFL_PIPELINE_INFO_NO_PREFIX("  ");	//Two space identation per level

	//Dump current node
	ROFL_PIPELINE_INFO_NO_PREFIX("[" );
	switch(me->type){
		case ME_TYPE_OTHER:
			ROFL_PIPELINE_INFO_NO_PREFIX("t-OTHER");
			break;
		case ME_TYPE_UNKNOWN:
			ROFL_PIPELINE_INFO_NO_PREFIX("t-UNKNOWN");
			break;
		case ME_TYPE_CHASSIS:
			ROFL_PIPELINE_INFO_NO_PREFIX("t-CHASSIS");
			break;
		case ME_TYPE_BACKPLANE:
			ROFL_PIPELINE_INFO_NO_PREFIX("t-BACKPLANE");
			break;
		case ME_TYPE_CONTAINER:
			ROFL_PIPELINE_INFO_NO_PREFIX("t-CONTAINER");
			break;
		case ME_TYPE_POWER_SUPPLY:
			ROFL_PIPELINE_INFO_NO_PREFIX("t-POWER_SUPPLY");
			break;
		case ME_TYPE_FAN:
			ROFL_PIPELINE_INFO_NO_PREFIX("t-FAN");
			break;
		case ME_TYPE_SENSOR:
			ROFL_PIPELINE_INFO_NO_PREFIX("t-SENSOR");
			break;
		case ME_TYPE_MODULE:
			ROFL_PIPELINE_INFO_NO_PREFIX("t-MODULE");
			break;
		case ME_TYPE_PORT:
			ROFL_PIPELINE_INFO_NO_PREFIX("t-PORT");
			break;
		case ME_TYPE_STACK:
			ROFL_PIPELINE_INFO_NO_PREFIX("t-STACK");
			break;
				
		default:
			assert(0);
			break;
	}

	ROFL_PIPELINE_INFO_NO_PREFIX(" (%p)] {name: %s}\n", me, me->name);
	
	//FIXME print sensor data	

	//Dump inner
	__monitoring_dump_me(indentation+1, me->inner);
	
	//Dump next element
	__monitoring_dump_me(indentation, me->next);
}

//Dump
void monitoring_dump(monitoring_state_t* monitoring){

	if(!monitoring)
		return;

	if(monitoring->rwlock)
		platform_rwlock_rdlock(monitoring->rwlock);	

	ROFL_PIPELINE_INFO("\n"); //This is done in purpose 
	ROFL_PIPELINE_INFO("Dumping %smonitoring state(%p). Last rev: %"PRIu64"\n", monitoring->is_snapshot? "!SNAPSHOT! ":"", monitoring, monitoring->last_rev);

	//Dump chassis
	__monitoring_dump_me(0, &monitoring->chassis);

	//Release the rdlock
	if(monitoring->rwlock)
		platform_rwlock_rdunlock(monitoring->rwlock);	
	
	ROFL_PIPELINE_INFO_NO_PREFIX("\n\n"); //This is done in purpose 
}


//
// Snapshots
//

//Get a snapshot
monitoring_snapshot_state_t* monitoring_get_snapshot(monitoring_state_t* monitoring){

	monitoring_snapshot_state_t* sn;

	sn = platform_malloc_shared(sizeof(monitoring_state_t));
	if(!sn)
		return NULL;
	
	//Lock rwlock (read)
	if(monitoring->rwlock)
		platform_rwlock_rdlock(monitoring->rwlock);	
	
	//Copy the state
	memcpy(sn, monitoring, sizeof(monitoring_state_t));

	//Set auxilary pointers to null
	sn->mutex = sn->rwlock = NULL;	

	//Clone monitored data
	if(__clone_root_monitored_entity(monitoring, &sn->chassis, &monitoring->chassis) != ROFL_SUCCESS){
		assert(0);
		platform_free_shared(sn);
		return NULL;
	}

	//Release the rdlock
	if(monitoring->rwlock)
		platform_rwlock_rdunlock(monitoring->rwlock);	

	//Mark as snapshot
	sn->is_snapshot = true;

	return sn;
}
