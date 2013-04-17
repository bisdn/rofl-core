/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
* @file switch_port.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Port abstraction and API 
*/


#ifndef __PHYSICAL_PORT_H__
#define __PHYSICAL_PORT_H__

#include <stdbool.h>
#include <inttypes.h>

#include "rofl.h"
#include "platform/lock.h"


//fwd decl
struct of_switch;

#define SW_PORT_ETH_ALEN 6
#define SWITCH_PORT_MAX_LEN_NAME 20

/**
* @brief Port state 
* @ingroup core 
*/
typedef enum{
    PORT_STATE_LINK_DOWN	= 1 << 0, 
    PORT_STATE_BLOCKED 		= 1 << 1, //We are not hybrid we might not need this
    PORT_STATE_LIVE 		= 1 << 2,
}port_state_t;

/**
* @brief Port features 
* @ingroup core
*/
typedef enum port_features{
    PORT_FEATURE_10MB_HD    = 1 << 0,  /* 10 Mb half-duplex rate support. */
    PORT_FEATURE_10MB_FD    = 1 << 1,  /* 10 Mb full-duplex rate support. */
    PORT_FEATURE_100MB_HD   = 1 << 2,  /* 100 Mb half-duplex rate support. */
    PORT_FEATURE_100MB_FD   = 1 << 3,  /* 100 Mb full-duplex rate support. */
    PORT_FEATURE_1GB_HD     = 1 << 4,  /* 1 Gb half-duplex rate support. */
    PORT_FEATURE_1GB_FD     = 1 << 5,  /* 1 Gb full-duplex rate support. */
    PORT_FEATURE_10GB_FD    = 1 << 6,  /* 10 Gb full-duplex rate support. */
    PORT_FEATURE_40GB_FD    = 1 << 7,  /* 40 Gb full-duplex rate support. */
    PORT_FEATURE_100GB_FD   = 1 << 8,  /* 100 Gb full-duplex rate support. */
    PORT_FEATURE_1TB_FD     = 1 << 9,  /* 1 Tb full-duplex rate support. */
    PORT_FEATURE_OTHER      = 1 << 10, /* Other rate, not in the list. */

    PORT_FEATURE_COPPER     = 1 << 11, /* Copper medium. */
    PORT_FEATURE_FIBER      = 1 << 12, /* Fiber medium. */
    PORT_FEATURE_AUTONEG    = 1 << 13, /* Auto-negotiation. */
    PORT_FEATURE_PAUSE      = 1 << 14, /* Pause. */
    PORT_FEATURE_PAUSE_ASYM = 1 << 15  /* Asymmetric pause. */
}port_features_t;

//Opaque platform port state (to be used, maybe, for platform hooks)
typedef void platform_port_state_t;

/**
* @brief Port stats 
* @ingroup core
*/
typedef struct port_stats {
	uint64_t rx_packets;     /* Number of received packets. */
	uint64_t tx_packets;     /* Number of transmitted packets. */
	uint64_t rx_bytes;       /* Number of received bytes. */
	uint64_t tx_bytes;       /* Number of transmitted bytes. */
	uint64_t rx_dropped;     /* Number of packets dropped by RX. */
	uint64_t tx_dropped;     /* Number of packets dropped by TX. */
	uint64_t rx_errors;      /* Number of receive errors.  This is a super-set
				of more specific receive errors and should be
				greater than or equal to the sum of all
				rx_*_err values. */
	uint64_t tx_errors;      /* Number of transmit errors.  This is a super-set
				of more specific transmit errors and should be
				greater than or equal to the sum of all
				tx_*_err values (none currently defined.) */
	uint64_t rx_frame_err;   /* Number of frame alignment errors. */
	uint64_t rx_over_err;    /* Number of packets with RX overrun. */
	uint64_t rx_crc_err;     /* Number of CRC errors. */
	uint64_t collisions;     /* Number of collisions. */

	//Mutex for statistics
	platform_mutex_t* mutex;
}port_stats_t;

/**
* @brief Port type enumeration 
* @ingroup core
*
* META ports are fake ports. META distinction is "group" of 
* port types which are fake ports (not fitting traditional
* port types classification).
*/
typedef enum port_type{
	PORT_TYPE_INVALID 		= 0,
	PORT_TYPE_PHYSICAL 		= 1,
	PORT_TYPE_VIRTUAL 		= 2,
	PORT_TYPE_TUNNEL 		= 3,

	//Special META(fake) ports
	//PORT_TYPE_META		= 4, 
	PORT_TYPE_META_FLOOD		= 5
}port_type_t;


/**
* @brief Port abstraction
* @ingroup core
*
* The switch_port implements the switch port abstraction.
* There are four types of ports:
*
*  - PORT_TYPE_PHYSICAL: representing a system's interface or device.
*  - PORT_TYPE_TUNNEL: a tunnel endpoint termination.
*  - PORT_TYPE_VIRTUAL: anything that does not fall to previous cat.
*  - PORT_TYPE_META: meta type should NEVER be used by the user. This 
*    has nothing to do with Openflow METADATA. 
*
*/
typedef struct switch_port{

	//mac address
	uint8_t hwaddr[SW_PORT_ETH_ALEN];

	/* General state */
	//Admin. state of the port
	bool up;

	//Forward packets
	bool forward_packets;	

	//Drop incomming packets
	bool drop_received;

	//Is virtual/tun
	port_type_t type;

	//Port name
	char name[SWITCH_PORT_MAX_LEN_NAME];

	//Port state
	port_state_t state;
	
	// Port statistics
	port_stats_t stats;

	//Port capabilities; bitmaps!
	port_features_t curr;          /* Current features. */
	port_features_t advertised;    /* Features being advertised by the port. */
	port_features_t supported;     /* Features supported by the port. */
	port_features_t peer;          /* Features advertised by peer. */ 

	//Current speeds
	port_features_t curr_speed;
	port_features_t curr_max_speed;
		
	/* 
	* OF related stuff
	*/
	bool of_generate_packet_in;
	//OF port number != phyisical port num => probably should be somehow merged into the ofXX_port.. let's see
	unsigned int of_port_num; //XXX: 
	//Pointer to current logical switch attached
	struct of_switch* attached_sw;	
 	
	/* Opaque platform port specific extra state */
	platform_port_state_t* platform_port_state; 
}switch_port_t;


/* Logical port abstraction (to be used by logical switches) */
typedef enum logical_switch_port_attachment_state{
	LOGICAL_PORT_STATE_FREE = 0,
	LOGICAL_PORT_STATE_ATTACHED = 1,
	LOGICAL_PORT_STATE_DETACHED = 2
}logical_switch_port_attachment_state_t; 

typedef struct logical_switch_port{
	logical_switch_port_attachment_state_t attachment_state;
	switch_port_t* port;
}logical_switch_port_t;

/* 
* Functions
*/

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/**
* @brief Init a switch_port structure
* @ingroup  mgmt
*/
switch_port_t* switch_port_init(char* name, bool up, port_type_t type, port_state_t state);

/**
* @brief Destroy a switch_port structure
* @ingroup  mgmt
*/
rofl_result_t switch_port_destroy(switch_port_t* port);

//Port Statistics

/**
* @brief Increments atomically all the statistics of the port; shall be used by ports on TX/RX. 
* Fill in with 0 the ones that should
* be left untouched.
* @ingroup  mgmt
*/
void switch_port_stats_inc(switch_port_t* port,
				uint64_t rx_packets,
				uint64_t tx_packets,
				uint64_t rx_bytes,
				uint64_t tx_bytes,
				uint64_t rx_dropped,
				uint64_t tx_dropped
				/*uint64_t rx_errors,
				uint64_t tx_errors,
				uint64_t rx_frame_err,
				uint64_t rx_over_err,
				uint64_t rx_crc_err,
				uint64_t collisions*/);


/*
* Conveninent wrappers just to avoid messing up with the bitmaps
*/

/**
* @brief Adds capabilities to the port
* @ingroup  mgmt
*/
void switch_port_add_capabilities(port_features_t* bitmap, port_features_t features);

/**
* @brief Removes capabilities to the port
* @ingroup  mgmt
*/
void switch_port_remove_capabilities(port_features_t* bitmap, port_features_t features);

/**
* @brief Sets current speed
* @ingroup  mgmt
*/
void switch_port_set_current_speed(switch_port_t* port, port_features_t speed);

/**
* @brief Sets current MAX speed 
* @ingroup  mgmt
*/
void switch_port_set_current_max_speed(switch_port_t* port, port_features_t speed);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //PHYSICAL_PORT
