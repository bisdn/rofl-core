#ifndef __OF1X_METER_H__
#define __OF1X_METER_H__

#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "rofl.h"
#include "of1x_statistics.h"
#include "of1x_utils.h"

/**
* @file of1x_meter.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief OpenFlow meters
*/

/**
* Meter band type
*/
typedef enum of1x_meter_band_type {
	OF1X_METER_TYPE_DROP	 	= 1,		/* Drop packet. */
	OF1X_METER_TYPE_DSCP_REMARK	= 2,		/* Remark DSCP in the IP header. */
	
	//[+] Add more here...
	
	OF1X_METER_TYPE_EXPERIMENTER	= 0xFFFF,	/* Experimenter meter band. */
}of1x_meter_band_type_t;

/**
* Band struct
*/
typedef enum of1x_meter_band {
	//Band type
	of1x_meter_band_type_t type;

	//Band specific paremeters
	union of1x_meter_band_spec{
		uint8_t prec_level;	//DSCP prescedence level
		uint32_t exp_id;	//Experimented Id		
	}specific;

	//Band rate
	uint32_t rate;
	
	//Burst size
	uint32_t burst_size;

	//Stats
	of1x_stats_meter_band_t stats;
}of1x_meter_band_t;

/**
* Meter struct
*/
typedef struct of1x_meter{
	//Meter id
	uint32_t id;
	
	//Meter band
	of1x_meter_band_t* bands;
	
	//Stats
	of1x_stats_meter_t stats;
	
	//RWlock
	platform_rwlock_t* rwlock;
}of1x_meter_entry_t;

/**
* Meter table
*/
typedef struct of1x_meter_table{
	
	//Number of meters installed
	uint32_t num_of_entries;

	//Entries	
	//TODO

	//Mutexes
	platform_mutex_t *mutex;
	platform_rwlock_t *rwlock;
}of1x_meter_table_t;


//C++ extern C
ROFL_BEGIN_DECLS

//Meter bands
/**
 * @brief Create a meter band 
 * @ingroup core_of1x
 *
 * Band specific parameters, such as exp_id and pre_level shall be 0 when not 
 * applicable for the band type
 */
of1x_meter_band_t* of1x_meter_band_init(of1x_meter_band_type_t type, uint32_t rate, uint32_t burst_size, uint32_t exp_id, uint8_t prec_level);

/**
 * @brief Destroy a meter band 
 * @ingroup core_of1x
 */
void of1x_meter_band_destroy(of1x_meter_band_t* meter_band); 

//Meter entries

/**
 * @brief Create a meter entry 
 * @ingroup core_of1x
 */
of1x_meter_entry_t* of1x_meter_init(uint32_t id);

/**
 * @brief Destroy a meter entry 
 * @ingroup core_of1x
 */
void of1x_meter_destroy(of1x_meter_entry_t* meter);


//
//Meter table init/destroy internal APIs 
//

//Initializes the meter table.
rofl_result_t __of1x_init_meter_table(of1x_meter_table_t* mt);

//Destroys the meter table.
void __of1x_destroy_meter_table(of1x_meter_table_t* mt);

//Meter table operations

/**
 * @brief Adds a meter to the table.
 * @ingroup core_of1x
 * 
 * If a meter with the ID already exists returns error
 */
rofl_of1x_me_result_t of1x_meter_add(of1x_meter_table_t* mt, of1x_meter_entry_t* meter);

/**
 * @brief Modifies a meter in the table.
 * @ingroup core_of1x

 */
rofl_of1x_me_result_t of1x_meter_modify(of1x_meter_table_t *mt, of1x_meter_entry_t* meter_mod);

/**
 * @brief Deletes a meter from the table.
 * @ingroup core_of1x
 */
rofl_of1x_me_result_t of1x_meter_delete(of1x_meter_table_t *mt, uint32_t id);

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_METER
