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
#include <time.h>

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
* Sensor data information
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



//C++ extern C
ROFL_BEGIN_DECLS

/**
* @brief Creates a monitored entity object and links it to 
* the double linked list 
* @ingroup  mgmt
*/
rofl_result_t init_monitored_entity(enum monitored_entity_type type, monitored_entity_t* prev, monitored_entity_t* parent);

/**
* @brief Destroys an detaches monitored entity object
* @warning This routine is not thread safe
* @ingroup  mgmt
*/
void destroy_monitored_entity(monitored_entity_t* entity);


//C++ extern C
ROFL_END_DECLS

#endif //MONITORING
