/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
* @file port_queue.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief This file contains the abstractions to monitor particular elements
* of the platform, like sensors or other hardware state
*/

#ifndef __MONITORING_H__
#define __MONITORING_H__

#include <rofl.h>
#include <stdbool.h>
#include <inttypes.h>
#include <time.h>
#include "platform/lock.h"

//fwd decl
struct monitored_entity;

enum sensor_data_type{
	SENSOR_DATA_T_OHTER=1,		//a measure other than those listed below
	SENSOR_DATA_T_UNKNOWN=2,	//unknown measurement, or arbitrary, relative numbers
	SENSOR_DATA_T_VOLTS_AC=3,	//electric potential
	SENSOR_DATA_T_VOLTS_DC=4,	//electric potential
	SENSOR_DATA_T_AMPERES=5,	//electric current
	SENSOR_DATA_T_WATTS=6,		//power
	SENSOR_DATA_T_HERTZ=7,		//frequency
	SENSOR_DATA_T_CELSIUS=8,	//temperature
	SENSOR_DATA_T_PERCENTRH=9,	//percent relative humidity
	SENSOR_DATA_T_RPM=10,		//shaft revolutions per minute
	SENSOR_DATA_T_CMM=11,		//cubic meters per minute (airflow)
	SENSOR_DATA_T_TRUTH_VALUE=12	//value takes { true(1), false(2) }
};

enum sensor_data_scale{
	SENSOR_DATA_S_YOCTO=1,	//10^-24
	SENSOR_DATA_S_ZEPTO=2,	//10^-21
	SENSOR_DATA_S_ATTO=3,	//10^-18
	SENSOR_DATA_S_FEMTO=4,	//10^-15
	SENSOR_DATA_S_PICO=5,	//10^-12
	SENSOR_DATA_S_NANO=6,	//10^-9
	SENSOR_DATA_S_MICRO=7,	//10^-6
	SENSOR_DATA_S_MILLI=8,	//10^-3
	SENSOR_DATA_S_UNITS=9,	//10^0
	SENSOR_DATA_S_KILO=10,	//10^3
	SENSOR_DATA_S_MEGA=11,	//10^6
	SENSOR_DATA_S_GIGA=12,	//10^9
	SENSOR_DATA_S_TERA=13,	//10^12
	SENSOR_DATA_S_EXA=14,	//10^15
	SENSOR_DATA_S_PETA=15,	//10^18
	SENSOR_DATA_S_ZETTA=16,	//10^21
	SENSOR_DATA_S_YOTTA=17	//10^24
};

enum sensor_data_oper_status{
	SENSOR_DATA_OS_OK=1,
	SENSOR_DATA_OS_UNAVAILABLE=2,
	SENSOR_DATA_OS_NONOPERATIONAL=3,
};

/**
* Sensor data information
*/
typedef struct sensor_data{
	enum sensor_data_type type;
	enum sensor_data_scale scale;
	unsigned int precision;
	double value;
	enum sensor_data_oper_status operational_status;
	unsigned int unitsdisplay;
	time_t value_timestamp;
	unsigned int value_update_rate;
}sensor_data_t; 


/**
* Monitoring entity type enum
* defined according to RFC2737
*/
enum monitored_entity_type{
	ME_TYPE_OTHER=1,
	ME_TYPE_UNKNOWN=2,
	ME_TYPE_CHASSIS=3,
	ME_TYPE_BACKPLANE=4,
	ME_TYPE_CONTAINER=5,
	ME_TYPE_POWER_SUPPLY=6,
	ME_TYPE_FAN=7,
	ME_TYPE_SENSOR=8,
	ME_TYPE_MODULE=9,
	ME_TYPE_PORT=10,
	ME_TYPE_STACK=11
};

#define MONITORED_ENTITY_MAX_SENSOR_DATA 8

/**
* Monitored entity data 
*/
typedef struct monitored_entity{

	//Type
	enum monitored_entity_type type;

	//Data
	unsigned int physical_index;
	char* description;
	char* vendor_type;
	char* class_type;
	char* name;
	char* hardware_rev;
	char* firmware_rev;
	char* software_rev;
	char* serial_num;
	char* manufacturer_model_name;
	char* model_name;
	char* alias;
	char* asset_id;
	bool is_field_replaceable_unit;

	//Sensor
	sensor_data_t sensors[MONITORED_ENTITY_MAX_SENSOR_DATA];

	/*
	* Navigational elements
	*/

	//Parent
	struct monitored_entity* parent;		

	//Inner elements
	struct monitored_entity* inner;		

	//double-linked list 
	struct monitored_entity* prev;
	struct monitored_entity* next;
}monitored_entity_t; 


/**
* Wrapper for the monitoMonitored entity data 
*/
typedef struct monitoring_state{

	//Reusing the same struct for the snapshot
	bool is_snapshot;
	
	//Simple counter which mantains
	//the version of the monitored data
	//Useful to avoid snapshots
	uint64_t last_rev;		//1->2...

	//Main chassis monitored entity(root)
	monitored_entity_t chassis;
	
	/* 
	* Protects state while creating snapshots
	* Should be acquired in WRITE mode when updating
	* information. READ mode should only be used while
	* creating snapshots
	*/
	platform_rwlock_t* rwlock;
	
	//Not used at all (except for platform_atomic_inc64())
	platform_mutex_t* mutex;
}monitoring_state_t;

//Alias
typedef monitoring_state_t monitoring_snapshot_state_t;

//C++ extern C
ROFL_BEGIN_DECLS

//Monitoring API

/**
* Initializes the monitoring state 
*/
rofl_result_t __monitoring_init(monitoring_state_t* monitoring);

/**
* @brief Dumps the monitoring state, only meaningful for debugging purposes 
* @ingroup  mgmt
*/
void monitoring_dump(monitoring_state_t* monitoring);

/**
* @brief Dumps the monitoring state of a snapshot
* @ingroup  mgmt
*/
static inline void monitoring_dump_snapshot(monitoring_snapshot_state_t* snapshot){
	return monitoring_dump(snapshot);
}

//Destroys the monitoring state 
void __monitoring_destroy(monitoring_state_t* monitoring);

/**
* @brief Returns true if the monitoring state has changed 
* @ingroup  mgmt
*/
static inline bool monitoring_has_changed(monitoring_state_t* state, uint64_t* last_seen_rev){
	return *last_seen_rev != state->last_rev;
}

/**
* @brief Get a snapshot of the current monitoring state.
*
* This function may be expensive, as it involves a deep copy in dynamic
* memory of all the monitored data.
*
* The monitoring is copied atomically (platform_rdlock) over the read lock
* The monitoring snapshots needs to be destroyed by calling monitoring_destroy_snapshot()
*
* @ingroup  mgmt
*/
monitoring_snapshot_state_t* monitoring_get_snapshot(monitoring_state_t* monitoring);

/**
* @brief Clone a monitoring snapshot 
* @ingroup  mgmt
*/
static inline monitoring_snapshot_state_t* monitoring_clone_snapshot(monitoring_snapshot_state_t* orig){
	return monitoring_get_snapshot(orig);
}

/**
* @brief Destroy a snapshot previously generated via monitoring_get_snapshot() routine. 
* @ingroup  mgmt
*/
static inline void monitoring_destroy_snapshot(monitoring_snapshot_state_t* snapshot){
	__monitoring_destroy((monitoring_state_t*)snapshot);
}

/**
* @brief Creates a monitored entity object and links it to the linked-list, in the position of prev OR parent.
* If no monitoring reference is passed, then an unlinked instance will be returned.
* @ingroup  mgmt
*/
monitored_entity_t* monitoring_add_monitored_entity(monitoring_state_t* monitoring, enum monitored_entity_type type, monitored_entity_t* prev, monitored_entity_t* parent);


//fwd declaration
rofl_result_t __monitoring_remove_monitored_entity(monitoring_state_t* monitoring, monitored_entity_t* entity, bool lock_acquired);
/**
* @brief Destroys an detaches monitored entity object
* @warning This routine is not thread safe
* @ingroup  mgmt
*/
static inline rofl_result_t monitoring_remove_monitored_entity(monitoring_state_t* monitoring, monitored_entity_t* entity){
	return __monitoring_remove_monitored_entity(monitoring, entity, false);
}

//C++ extern C
ROFL_END_DECLS

#endif //MONITORING
