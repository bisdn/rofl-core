#include "of1x_meter.h"
#include <stdio.h>
#include "../../../platform/lock.h"
#include "../../../platform/memory.h"
#include "../../../platform/likely.h"
#include "../../../util/logging.h"


//Create a meter band 
of1x_meter_band_t* of1x_meter_band_init(of1x_meter_band_type_t type, uint32_t rate, uint32_t burst_size, uint32_t exp_id, uint8_t prec_level){
	of1x_meter_band_t* band = (of1x_meter_band_t *) platform_malloc_shared(sizeof(of1x_meter_band_t));
	
	if( unlikely(band==NULL) )
		return NULL;

	//Initialize stats
	of1x_stats_meter_band_init(&band->stats);
	
	//Set elements
	band->type = type;
	band->rate = rate;
	band->burst_size = burst_size;

	//Band type specific parameters
	memset(&band->specific,0,sizeof(band->specific));	
	if(type == OF1X_METER_TYPE_DSCP_REMARK){
		band->specific.prec_level = prec_level;
	}else if(type == OF1X_METER_TYPE_EXPERIMENTER){
		band->specific.exp_id = exp_id;
	}
	
	return band;
}

//Destroy a meter band 
void of1x_meter_band_destroy(of1x_meter_band_t* meter_band){

	//Destroy stats
	of1x_stats_meter_band_destroy(&band->stats);

	//TODO: rwlock?	

	//Release mem
	platform_malloc_free(meter_band);
} 

//
//Meter entries
//

//Create a meter entry 
of1x_meter_entry_t* of1x_meter_init(uint32_t id){
	of1x_meter_entry_t* meter = (of1x_meter_entry_t *) platform_malloc_shared(sizeof(of1x_meter_entry_t));
	
	if( unlikely(meter==NULL) )
		return NULL;

	//Init stats&mutexes 
	of1x_stats_meter_init(&meter->stats);
	meter->rwlock = platform_rwlock_init(NULL);
	 
	meter->id = id;

	return meter;	
}

//Destroy a meter entry 
void of1x_meter_destroy(of1x_meter_entry_t* meter){
	of1x_stats_meter_destoy(&meter->stats);

	//Release mem
	platform_malloc_free(meter);
}


//
//Meter table init/destroy internal APIs 
//

//Initializes the meter table.
rofl_result_t __of1x_init_meter_table(of1x_meter_table_t* mt){

	//Initialize mutexes and rwlocks
	mt->rwlock = platform_rwlock_init(NULL);
	mt->mutex = platform_mutex_init(NULL);

	//Init values
	mt->num_of_entries = 0;
	
	//TODO: init hash table
}

//Destroys the meter table.
void __of1x_destroy_meter_table(of1x_meter_table_t* mt){

	//TODO: destroy hash table

	//Initialize mutexes and rwlocks
	platform_rwlock_destroy(mt->rwlock);
	platform_mutex_destroy(mt->mutex);

}

//
//Meter table operations
//

//Adds a meter to the table.
rofl_of1x_me_result_t of1x_meter_add(of1x_meter_table_t* mt, of1x_meter_entry_t* meter){


}

//Modifies a meter in the table.
rofl_of1x_me_result_t of1x_meter_modify(of1x_meter_table_t *mt, of1x_meter_entry_t* meter_mod){

}

 //Deletes a meter from the table.
rofl_of1x_me_result_t of1x_meter_delete(of1x_meter_table_t *mt, uint32_t id){

}
