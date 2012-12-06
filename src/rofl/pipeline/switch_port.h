#ifndef __PHYSICAL_PORT_H__
#define __PHYSICAL_PORT_H__

#include <stdbool.h>
#include <inttypes.h>

/*
*
* Implements the physical port abstraction 
*      Author: msune
*
*/

//fwd decl
struct of_switch;

#define SW_PORT_ETH_ALEN 6

//Port state
typedef enum{
    PORT_STATE_LINK_DOWN, 
    PORT_STATE_BLOCKED,
    PORT_STATE_LIVE
}port_state_t;

//Port capabilities
enum port_features{
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
};
typedef uint32_t port_features_t;

//Opaque platform port state (to be used, maybe, for platform hooks)
typedef void platform_port_state_t;

//Port state
struct switch_port{

	//mac address
	uint8_t hwaddr[SW_PORT_ETH_ALEN];

	/* General state */
	//Admin. state of the port
	bool up;

	//Is virtual/tun
	bool is_virtual;
	bool is_tunnel;

	//Forward packets
	bool forward_packets;	
	
	//Port name
	char* name;

	//Port state
	port_state_t state;	

	//Port capabilities; bitmaps!
	port_features_t curr;          /* Current features. */
	port_features_t advertised;    /* Features being advertised by the port. */
	port_features_t supported;     /* Features supported by the port. */
	port_features_t peer;          /* Features advertised by peer. */ 

	/* OF related stuff */
	bool of_generate_packet_in;

	//OF port number != phyisical port num => probably should be somehow merged into the ofXX_port.. let's see
	unsigned int of_port_num; //XXX: 
	
	/* Pointer to current logical switch attached */
	struct of_switch* attached_sw;	
 	
	/* Opaque platform port specific extra state */
	platform_port_state_t* platform_port_state; 
};
typedef struct switch_port switch_port_t;


/* Logical port abstraction (to be used by logical switches) */
typedef enum{
	LOGICAL_PORT_STATE_FREE = 0,
	LOGICAL_PORT_STATE_ATTACHED = 1,
	LOGICAL_PORT_STATE_DETACHED = 2
}logical_switch_port_attachment_state_t; 

typedef void of_stats_port;

typedef struct{
	logical_switch_port_attachment_state_t attachment_state;
	switch_port_t* port;
	of_stats_port* stats;
}logical_switch_port_t;

#endif //PHYSICAL_PORT
